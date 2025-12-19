function [means, covariances, priors, epochs, post, info] ...
    = mixture(observations, options, n, means, covariances, priors, info)
%MIXTURE Mixture model training.
%
%  Copyright 1997-9 Matthew Brand, MERL.  NOT FOR DISTRIBUTION
%
%   [MEANS, COVARIANCES, PRIORS, EPOCHS, POST]
%       = MIXTURE(OBSERVATIONS, OPTIONS, N, MEANS, COVARIANCES, PRIORS)
%   trains a mixture model of Gaussian distributions.  All input arguments
%   except OBSERVATIONS are optional.  Among the output arguments, EPOCHS is the
%   number of re-estimation cycles used, and POST is the final log-posterior.
%
%   OBSERVATIONS is a matrix of input vectors, one per row.  (If OBSERVATIONS is
%   a row vector, it is transposed.)
%
%   OPTIONS specifies additional control parameters; see TOPTIONS for details.
%
%   N is the number of mixtures in the initial model.  It defaults to the number
%   of input vectors.
%
%   MEANS, COVARIANCES and PRIORS are the parameters of the model.  MEANS
%   contains the means of the Gaussian components, one row per component.
%   COVARIANCES contains the covariances of the Gaussians, each components
%   concatenated together in the third dimension.  PRIORS is a row vector
%   containing the mixture prior probabilities.  If omitted from the input
%   arguments, they are randomly initialized based on the input vectors.
%
%   INFO is a cell vector in which each entry is associated with a component in
%   the model.  Initially, INFO defaults to {1,2,...,n}, where n is the number
%   of components.  If and when components are trimmed, their corresponding
%   entries in INFO are also removed.
%
%   $Id: mixture.m,v 1.1.1.1 2002/03/05 12:53:30 karol2000 Exp $

% Constants
persistent CONSTANTS_INITIALIZED
persistent REALMIN CLEAR_EOL
if isempty(CONSTANTS_INITIALIZED),
    REALMIN = realmin;
    CLEAR_EOL = [27 '[K'];
    CONSTANTS_INITIALIZED = 1;
end;

% Initialize and check arguments and variables
error(nargchk(1, 7, nargin));
[t, d] = size(observations);            % Number of input vectors and dimensions
if 1 == t,                              % If observations is one-dimensional,
    observations = observations'; t = d; d = 1; end; % make it a column vector
if nargin < 2, options = []; end;
options = toptions(options);
epsilon = options.epsilon;
itmax = options.itmax;
z_coef = options.z; any_z = any(z_coef);
y_coef = options.y; any_y = any(any(y_coef));
                    any_y1 = any(y_coef(1,:));
                    any_y2 = any(y_coef(2,:));
any_y1 = any(y_coef(1,:));
any_y1 = any(y_coef(1,:));
zmax = options.zmax;
trimperiod = options.trimperiod;
trimratio = options.trimratio;
trimcycles = options.trimcycles;
status = options.status;
plotting = options.plotting;
if plotting, global Capture Movie; end;
schedule = options.schedule;

% Random model initialization
if nargin < 3,
    % Number of components in mixture
    n = t;
end;
% Make sure that n is a positive integer
n = max(round(n), 1);
if nargin < 4,
    % Choose random points for means
    means = observations(randindex(n, t), :);
end;
if nargin < 5,
    % Begin with full-data covariances
    covariances = reshape(kron(randn(1,n) .* 0.01 + 2, ...
        cov(observations)), [d d n]);
end;
if nargin < 6,
    % Random mixture probabilities
    priors = stoch(randn(1,n) .* 0.01 + 1);
end;

% Initialize info if not given
if nargin < 7,
    info = cell(1, n);
    for i = 1:n, info{i} = i; end;
end;

% Prepare for EM loop
log2pied2 = 1/2 + (d./2).*log(2.*pi);   % A constant
epochs = 1;                 % Epoch number
px = zeros(t,n);            % Likelihoods for each input vector and component
q = zeros(t,1);             % Likelihoods for each input vector, or realmin if 0
priorp = 0;                 % Negative multinomial entropy
                            % (updated iff any_z)
priorsp = zeros(1,n);       % Same, per parameter
                            % (updated iff any_z)
logpriors = zeros(1,n);     % Log of priors (or rather, priors + realmin)
                            % (updated iff any_z)
priorc1 = 0;                % Negative covariance entropy (unscaled by priors)
                            % (updated iff any_y1)
priorsc1 = zeros(1,n);      % Same, per parameter (unscaled by priors)
                            % (updated even if ~any_y)
for i = 1:n,
    priorsc1(i) = det(covariances(:,:,i));
end;
priorsc1 = -log(abs(priorsc1) + REALMIN)./2 - log2pied2;
priorc2 = 0;                % Negative covariance entropy (scaled by priors)
                            % (updated iff any_y2)
priorsc2 = zeros(1,n);      % Same, per parameter (scaled by priors)
                            % (updated iff any_y2)
priort = 0;                 % log driving term for temperature
Omeans = means;             % Old means
Ocovariances = covariances; % Old covariances
Opriors = priors;           % Old priors
trimmed = 0;                % Whether we trimmed anything this epoch
                            % (updated iff status >= 0)
trimepoch = 1;              % Epoch when we last performed trimming
trimpost = NaN;             % Posterior when we last performed trimming
numerrorokay = 0;           % Whether to continue even on numerical error
snapshot mixture 0          % Take initial snapshot

% EM loop
if status >= 0, fprintf(2, '=== Begin EM loop\n'); end;
while epochs <= itmax,
    if epochs >= 2,
        snapshot mixture
        oldpost = post;
    end;

    % Calculate log-likelihood
    for i = 1:n,
        px(:,i) = B3(observations, covariances(:,:,i), means(i,:));
    end;
    q = px * priors.';
    q(q == 0) = REALMIN;
    like = sum(log(q));

    % Calculate priors on mixture model and component Gaussians
    if any_z,
        logpriors = log(priors + REALMIN);
        priorsp = priors .* logpriors;
        priorp = sum(priorsp);
    end;
    if any_y1,
        priorc1 = sum(priorsc1);
    end;
    if any_y2,
        priorsc2 = priorsc1 .* priors;
        priorc2 = sum(priorsc2);
    end;

    % Re-estimate MAP temperature (or take it from the given fixed schedule)
    % and compute the exponents zz, y1, and y2
    if isempty(schedule),
        temperature = z_coef(2) .* priorp ...
            + y_coef(1,2) .* priorc1 + y_coef(2,2) .* priorc2;
    elseif epochs <= length(schedule),
        temperature = schedule(epochs);
    else
        temperature = schedule(end);
    end;
    zz = z_coef(  1) + z_coef(  2) .* temperature;
    if zz > zmax,
        temperature = (zmax - z_coef(1)) / z_coef(2);
        zz = zmax;
    end;
    y1 = y_coef(1,1) + y_coef(1,2) .* temperature;
    y2 = y_coef(2,1) + y_coef(2,2) .* temperature;
    priort = -temperature.^2/2;

    % Calculate log-posterior
    post = like + zz .* priorp + y1 .* priorc1 + y2 .* priorc2 + priort;

    % Print status
    if status >= 0,
        fprintf(2, 'Epoch %3d: %#8g = %#-8g', epochs, post, like);
        if any_z, fprintf(2, ' %#+-8g*%-g', priorp, zz);
            else fprintf(2, ' +0'); end;
        if any_y1, fprintf(2, ' %#+-8g*%-g', priorc1, y1);
            else fprintf(2, ' +0'); end;
        if any_y2, fprintf(2, ' %#+-8g*%-g', priorc2, y2);
            else fprintf(2, ' +0'); end;
        fprintf(2, ' %+-g @%-g%s', priort, temperature, CLEAR_EOL);
    end;

    if epochs > length(schedule) & epochs >= 2,
        % Test for numerical error
        if post <= oldpost,
            if status >= 0,
                fprintf(2, '\n*** Numerical error %g => %g => %g\n', ...
                    postbase, oldpost, post);
            end;
            if ~numerrorokay,
                means = Omeans; covariances = Ocovariances; priors = Opriors;
                post = oldpost;
                break;                  % Bail out with old parameters
            end;
        end;

        % Test for convergence
        if abs((post - oldpost) ./ (post - postbase)) <= epsilon,
            if status >= 0,
                fprintf(2, '\n=== Terminating: %g => %g => %g\n', ...
                    postbase, oldpost, post);
            end;
            break;                      % Bail out with converged parameters
        end;
    end;

    % Save initial posterior in postbase
    if epochs == 1, postbase = post; trimpost = post; end;

    % Trim
    if (trimperiod & (epochs - trimepoch >= trimperiod)) ...
     | (trimratio & (epochs >= 2) ...
                  & (abs((post - oldpost) ./ (post - trimpost)) <= trimratio)),
        for trimcount = 1:trimcycles,
            trimpost = post;
            if n <= 1, break; end;
            % Calculate trimming criterion
            % Compute the derivative of the log-likelihood
            % wrt the mixture priors
            dloglike = sum(vprod(1./q, px));
            % Approximate the difference that trimming would make in posterior
            dpost_base = dloglike + zz .* logpriors + y2 .* priorsc1;
            dpost_trim = dpost_base + y1 .* priorsc1 ./ priors;
            dpost_dist = dpost_base + zz;
            clear dpost_base;
            % Decide which components to trim and distribute to
            [temp, i_dist] = max(dpost_dist);
            [temp, i_trim] = min(dpost_trim);
            if i_dist == i_trim, break; end;
            % Compute exact increase in posterior and check that it's positive
            newq = q + (px(:,i_dist) - px(:,i_trim)) .* priors(i_trim);
            newq(newq == 0) = REALMIN;
            newlike = sum(log(newq));
            priors_sum = priors(i_dist) + priors(i_trim) + REALMIN;
            priorsp_sum = priorsp(i_dist) + priorsp(i_trim);
            diff = newlike - like ...
                 + zz .* (priors_sum .* log(priors_sum) - priorsp_sum) ...
                 - y1 .* (priorsc1(i_trim)) ...
                 + y2 .* (priorsc1(i_dist)-priorsc1(i_trim)) .* priors(i_trim);
            clear priors_sum priorsp_sum;
            if diff <= 0, break; end;

            % Trim and recompute other variables
            means(i_trim,:) = [];
            covariances(:,:,i_trim) = [];
            priors(i_dist) = priors(i_dist) + priors(i_trim);
            priors(i_trim) = [];
            n = n - 1;
            info(i_trim) = [];
            px(:,i_trim) = [];
            q = newq;
            like = newlike;
            if any_z,
                logpriors = log(priors + REALMIN);
                priorsp = priors .* logpriors;
                priorp = sum(priorsp);
            else
                logpriors(i_trim) = [];
                priorsp(i_trim) = [];
            end;
            priorsc1(i_trim) = [];
            if any_y1,
                priorc1 = sum(priorsc1);
            end;
            if any_y2,
                priorsc2 = priorsc1 .* priors;
                priorc2 = sum(priorsc2);
            end;
            prev_post = post;
            post = like + zz .* priorp + y1 .* priorc1 + y2 .* priorc2 + priort;
            if status >= 0,
                if status > 0, fprintf(2, ' X%d(%g)', i_trim, post); end;
                trimmed = 1;
            end;

            % Check that the posterior went up
            % (if not, we have a numerical error)
            if post < prev_post,
                if status >= 0,
                    fprintf(2, '\n*** Trimming error %g => %g\n', ...
                        prev_post, post);
                end;
                break;
            end;
        end;
    end;
    if trimmed, % trimmed implies status >= 0
        fprintf(2, ', %d left\n', n);
        trimmed = 0;
    elseif status == 0,
        fprintf(2, '\r');
    elseif status == 1,
        fprintf(2, '\n');
    end;

    % Re-estimate means, covariances and priors
    numerrorokay = 0;
    Omeans = means; Ocovariances = covariances; Opriors = priors;
    if n == 1,
        % We can't use stochsafe if n == 1, since stochsafe would
        % operate along the first dimension.
        gammas = ones(t, 1);
    else
        gammas = stochsafe(vprod(priors, px));
    end;
    omega = sum(gammas);
    means = vprod(1 ./ omega.', gammas' * observations);
    denom = max(1e-20, y1 + y2 .* priors + omega);
    iccbreak = 0;
    for i = 1:n,
        [covar, fix_result] = fix_covariances(wcov(...
            observations, gammas(:,i), means(i,:), 1) ./ denom(i));
        switch fix_result,
        case 1,
            % Ill-conditioned covariances fixed.
            if status >= 0,
                fprintf(2, '*** Ill-conditioned covariances\n');
            end;
            numerrorokay = 1; % Ignore numerical errors in next epoch
        case -1,
            % Ill-conditioned covariances could not be fixed.
            if status >= 0,
                fprintf(2, '*** Really ill-conditioned covariances\n');
            end;
            numerrorokay = 1; % Ignore numerical errors in next epoch
            if options.iccabort,
                iccbreak = 1; % Abort training procedure
            end;
        end;
        covariances(:,:,i) = covar;
        priorsc1(i) = det(covar);
    end;
    if iccbreak, break; end;
    % We take the absolute value of priorsc1 to get around a numerical
    % bug in MATLAB that sometimes produces non-positive determinants
    % for badly conditioned covariance matrices.
    priorsc1 = -log(abs(priorsc1) + REALMIN)./2 - log2pied2;
    priors = entropic_map(omega, zz, y2 .* priorsc1);
    clear denom omega;

    % Pretty pictures
    if plotting,
        show_hmm(means, covariances, observations);
        if ~isempty(Capture), axis equal;
            [capture_x, capture_map] = capture;
            imwrite(capture_x, capture_map, ...
                sprintf('%s%03d.tiff', Capture, epochs), 'tiff');
            clear capture_x capture_map
        end;
    end;

    % Advance epoch count
    epochs = epochs + 1;
end;

if epochs > itmax & status >= 0,
    if status == 0, fprintf(2, '\n'); end;
    fprintf(2, '=== Terminating after %d epochs%s\n', itmax, CLEAR_EOL);
end;

final = 1;
snapshot mixture 0          % Take final snapshot
