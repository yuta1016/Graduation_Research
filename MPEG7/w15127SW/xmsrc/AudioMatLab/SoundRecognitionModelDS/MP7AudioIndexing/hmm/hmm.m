function [transitions, starts, means, icovariances, ...
     epochs, post, info, prob_factor] = hmm(observations, lengths, options, ...
    transitions, starts, means, icovariances, info, prob_factor)
%HMM Hidden Markov model training.
%   [TRANSITIONS, STARTS, MEANS, ICOVARIANCES,
%       EPOCHS, POST, INFO, PROB_FACTOR] = HMM(OBSERVATIONS, LENGTHS, OPTIONS,
%               TRANSITIONS, STARTS, MEANS, ICOVARIANCES, INFO, PROB_FACTOR)
%   trains a discrete-state hidden Markov model with either discrete or
%   continuous (Gaussian) outputs.
%   All input arguments except MEANS, ICOVARIANCES, INFO and PROB_FACTOR are
%   required, but LENGTH and OPTIONS can be empty, as can MEANS, ICOVARIANCES,
%   INFO and PROB_FACTOR.  Among the output arguments, EPOCHS is the number of
%   re-estimation cycles used, and POST is the final log-posterior.
%
%   OBSERVATIONS is a matrix of input vectors, one per row.  If the observations
%   are discrete, the width of OBSERVATIONS must be one.
%
%   LENGTHS is a two-column matrix, each row in which specifes the beginning and
%   ending observation indices for each training sequence.  While LENGTH must be
%   specified, it can be empty, in which case the entirety of OBSERVATIONS is
%   considered as one single sequence.
%
%   OPTIONS specifies additional control parameters; see TOPTIONS for details.
%
%   TRANSITIONS is the transition probability matrix in which each row is a
%   source state and each column is a target state.
%
%   STARTS is a row vector of initial state probabilities.
%
%   MEANS is a matrix in which each row vector is either the output
%   probabilities of a state (in the discrete case) or the output mean
%   coordinates of a state (in the continuous case).  If omitted, it is
%   initialized randomly from the observations.
%
%   ICOVARIANCES is either an empty matrix (in the discrete case) or the output
%   inverse covariance matrices of each state concatenated together in the third
%   dimension (in the continuous case).  If omitted, it is initialized from the
%   observations.
%
%   INFO is a cell vector in which each entry is associated with a state in the
%   model.  Initially, INFO defaults to {1,2,...,n}, where n is the number of
%   states.  If and when states are trimmed, their corresponding entries in INFO
%   are also removed.
%
%   PROB_FACTOR is a matrix with one row for each observation and one column for
%   each state; whenever HMM computes the conditional probability of an
%   observation given a state, it multiplies by the corresponding value in
%   PROB_FACTOR, if specified.  This can be used for supervised training.
%
%   $Id: hmm.m,v 1.1.1.1 2002/03/05 12:53:26 karol2000 Exp $

% Constants
persistent CONSTANTS_INITIALIZED
persistent REALMIN REALMAX TRIM_EPS CLEAR_EOL
if isempty(CONSTANTS_INITIALIZED),
  REALMIN = realmin;
  REALMAX = realmax;
  CLEAR_EOL = [27 '[K'];
  CONSTANTS_INITIALIZED = 1;
end;

% Initialize and check arguments and variables
error(nargchk(5, 9, nargin));
if isempty(lengths), lengths = [1 length(observations)]; end;
options = toptions(options);

% Determine whether the data is discrete
discrete = min(size(observations)) == 1 ...
    & ~any(rem(observations(1:min(100,length(observations))), 1));

% Clip off unused observations
points = sum(lengths(:,2) - lengths(:,1) + 1);
if points < size(observations, 2),
  offset = lengths(1,1) - 1;
  observations = observations(1+offset:points+offset,:);
  lengths = lengths - offset;
  clear offset;
end;

% More variable initializations
n = size(transitions, 1);               % Number of states
[t, d] = size(observations);            % Number of input vectors and dimensions
nsequences = size(lengths, 1);          % Number of observation sequences
bw_lengths = (lengths(:,2) - lengths(:,1))' + 1;
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
trimepsilon = options.trimepsilon(:);
starts_bias = options.starts_bias;
transitions_bias = options.transitions_bias;
discrete_bias = options.discrete_bias;
if length(trimepsilon) == 1,
  trimepsilon = [trimepsilon trimepsilon trimepsilon];
elseif ((length(trimepsilon) == 2) & ~discrete) ...
      | ((length(trimepsilon) == 3) & discrete),
else error('Invalid size for ''trimepsilon'' option'); end;
trimcycles = options.trimcycles;
status = options.status;
plotting = options.plotting;
if plotting, global Capture Movie; end;
estoutput = options.estoutput;
schedule = options.schedule;

% Get reestimation transformation matrices, if specified
TtransitionsN  = options.TtransitionsN;
TstartsN       = options.TstartsN;
TmeansN        = options.TmeansN;
TmeansD        = options.TmeansD;
TcovariancesNt = options.TcovariancesN.';

if ~discrete,
  full_data_covar = fix_covariances(cov(observations)); 
  full_data_det = det(full_data_covar);
end;
% Initialize output parameters if they are not given
if nargin < 7 | isempty(means) | isempty(icovariances),
  if discrete,
    if nargin < 6 | isempty(means),
      means = stoch(rand(n, max(observations)));
    end;
    icovariances = [];
  else
    if nargin < 6 | isempty(means),
      % Place gaussians 1/2 way between random points and the whole-data mean.
      %means = vadd(mean(observations),observations(ceil(rand(n, 1).*t),:))./2;
      means = observations(ceil(rand(n, 1).*t),:);
    end;
    if nargin < 7 | isempty(icovariances),
      icovar = full_data_covar./4.^(1./d);   % Quadruple the volume
      icovariances = repmat(icovar, [1 1 n]);
      clear icovar;
    end;
    full_data_covar=full_data_covar./128.^(1./d);
    full_data_det=full_data_det./128;
  end;
end;

% Initialize info if not given
if nargin < 8 | isempty(info),
  info = cell(1, n);
  for i = 1:n, info{i} = i; end;
end;

% Default prob_factor unused
if nargin < 9, prob_factor = []; end;

% Prepare for EM loop
log2pied2 = 1/2 + (d./2).*log(2.*pi);   % A constant
epochs = 1;                 % Epoch number
priorp = 0;                 % Negative transition entropy
                            % (updated iff any_z)
priorsp = zeros(1,n);       % Same, per state except starts
                            % (updated iff any_z)
logtransitions = zeros(n);  % Log of transitions (or rather, transitions +
                            % realmin) (updated iff any_z)
priors0 = 0;                % Same, for starts
                            % (updated iff any_z)
logstarts = zeros(1,n);     % Log of starts (or rather, starts + realmin)
                            % (updated iff any_z)
priorc1 = 0;                % Negative observation entropy (unscaled by priors)
                            % (updated iff any_y1)
priorsc1 = zeros(1,n);      % Same, per state (unscaled by priors)
                            % (updated iff any_y | ~discrete) <-- not a typo
if any_y & discrete,
    priorsc1 = sum(means .* log(means + REALMIN), 2)';
elseif ~discrete,
    for i = 1:n,
        priorsc1(i) = 1./det(icovariances(:,:,i));
    end;
    priorsc1 = -log(abs(priorsc1) + REALMIN)./2 - log2pied2;
end;
priorc2 = 0;                % Negative observation entropy (scaled by priors)
                            % (updated iff any_y2)
priorsc2 = zeros(1,n);      % Same, per state (scaled by priors)
                            % (updated iff any_y2)
priort = 0;                 % log driving term for temperature
Otransitions = transitions; % Old transitions
Ostarts = starts;           % Old starts
Omeans = means;             % Old means
Oicovariances = icovariances; % Old icovariances
trimmed = 0;                % Whether we trimmed anything this epoch
                            % (updated iff status >= 0)
trimepoch = 1;              % Epoch when we last performed trimming
trimpost = NaN;             % Posterior when we last performed trimming
numerrorokay = 0;           % Whether to continue even on numerical error
%snapshot hmm 0              % Take initial snapshot

% EM loop
if status >= 0, fprintf(2, '=== Begin EM loop\n'); end;
while epochs <= itmax,
  if epochs >= 2,
    % snapshot hmm
    oldpost = post;
  end;

  % Accumulate expectations for all training sequences
  % and calculate log-likelihood
  [transitionsN, transitionsD, startsN, covariancesN, ...
        meansN, meansD, like] = baumwelch_all(transitions, starts, ...
      icovariances, means, observations, bw_lengths, prob_factor);
  if trimperiod,
    % Remove states that have zero occupancy
    dead=find(0==meansD);
    if any(dead),
      fprintf(2,'Dead after Baumwelch: %d %d %d %d %d %d %d %d\n',dead);
      [startsN, transitionsN, meansN, covariancesN]= ...
	  kill_state(dead,startsN, transitionsN, meansN, covariancesN);
      [starts, transitions, means, icovariances]= ...
	  kill_state(dead,starts, transitions, means, icovariances);
      [logstarts,logtransitions,Omeans,Oicovariances]= ...
	  kill_state(dead,logstarts,logtransitions, Omeans, Oicovariances);
      starts=stoch(starts); transitions=stoch(transitions);
      meansD(dead)=[]; transitionsD(dead)=[];
      if length(priorsc1)>1, priorsc1(dead)=[];end; priorsc2(dead)=[];
      n = n - length(dead);
      info(dead) = [];
      if ~isempty(prob_factor), prob_factor(:,dead) = []; end;
      [TstartsN, TtransitionsN, TmeansN, TcovariancesNt]= ...
	  kill_state(dead,TstartsN, TtransitionsN, TmeansN, TcovariancesNt);
      trimmed=1;
    end;
  end;

  % Calculate priors on Markov model and output parameters
  if any_z,
    logtransitions = log(transitions + REALMIN);
    priorsp = sum(transitions .* logtransitions, 2)';
    logstarts = log(starts + REALMIN);
    priors0 = sum(starts .* logstarts);
    priorp = sum(priorsp) + priors0;
  end;
  if any_y,
    if any_y1,
      priorc1 = sum(priorsc1);
    end;
    if any_y2,
      total_occ = sum(meansD);
      stoch_occ = meansD ./ total_occ;
      priorsc2 = priorsc1 .* stoch_occ;
      priorc2 = sum(priorsc2);
    end;
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
    fprintf(2, '# %3d: %#8g = %#-8g', epochs, post, like);
    if any_z, fprintf(2, ' %#+-8g*%-5g', priorp, zz);
    else fprintf(2, ' +0'); end;
    if any_y1, fprintf(2, ' %#+-8g*%-5g', priorc1, y1);
    else fprintf(2, ' +0'); end;
    if any_y2, fprintf(2, ' %#+-8g*%-5g', priorc2, y2);
    else fprintf(2, ' +0'); end;
    fprintf(2, ' %+-g @%-5g%s', priort, temperature, CLEAR_EOL);
  end;

  % Draw pretty pictures
  if plotting,
    %posteriors(epochs) = post;
    %subplot(4,2,3); plot(posteriors);
    %axis([1 itmax min(posteriors) max(posteriors)+1]);
    %subplot(4,2,4); show_transitions(transitions); drawnow;
  end;

  if epochs > length(schedule) & epochs >= 2,
    % Test for numerical error
    if post <= oldpost,
      if status >= 0,
        fprintf(2, '\n*** Numerical error %g => %g => %g\n', ...
            postbase, oldpost, post);
      end;
      if ~numerrorokay,
        transitions = Otransitions; starts = Ostarts;
        means = Omeans; icovariances = Oicovariances;
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
    trimepoch = epochs;
    for trimcount = 1:trimcycles,
      trimpost = post;
      if n <= 1, break; end;
      just_trimmed = 0;

      % The derivatives of the log-likelihood ("like") with respect to
      % transitions and starts are:
      %
      %           dloglike_t = repmat(transitionsD', 1, n);
      %           dloglike_s = repmat(nsequences, 1, n);

      % Note that in the computation of dpost_{trim,dist}_[ts] below,
      % when it comes to the effect of trimming on the y2 term, we
      % approximate the derivative of meansD wrt transitions by
      %
      %              d meansD(k)
      %           ------------------ = (k == j) * transitionsD(i)
      %           d transitions(i,j)
      %
      % and approximate the derivative of meansD wrt starts by
      %
      %           d meansD(k)
      %           ----------- = (k == j) * nsequences.
      %           d starts(j)
      %
      % In other words, we ignore effects beyond one time step.
      % TODO: This can be made exact. Chain rule fun.

      % Also, the savings in the y1 term achieved by removing an entire
      % state is considered separately in the "y1 .* priorsc1" term in the
      % computation of "gpost_pinch" further down.

      % Calculate trimming criterion
      % Approximate gain of post wrt transitions, divided by transitions
      if y2,
        temp = y2 .* priorsc1 ./ total_occ;
	fprintf(2,'fix the derivative of the y2 option before using it !');
        dpost_trim_t = vadd(transitionsD', ...
            zz .* logtransitions + vprod(temp, transitionsD'));
      else
        temp = 0;
        dpost_trim_t = vadd(transitionsD', zz .* logtransitions);
      end;
      % Approximate gain of post wrt starts, divided by starts
      dpost_trim_s = zz .* logstarts + (temp + 1) .* nsequences;
      clear temp;
      % Approximate derivative of post wrt transitions
      dpost_dist_t = dpost_trim_t + zz;
      % Approximate derivative of post wrt starts
      dpost_dist_s = dpost_trim_s + zz;
      % Holes shall never be redistributed to
      dpost_dist_t(transitions == 0) = -Inf;
      dpost_dist_s(starts      == 0) = -Inf;
      % Approximate gain of post wrt transitions
      gpost_trim_t = dpost_trim_t .* transitions;
      % Approximate gain of post wrt starts
      gpost_trim_s = dpost_trim_s .* starts;
      % We can only rely on the approximations above where the probability
      % delta is small
      gpost_trim_t(transitions >= trimepsilon(2)) = Inf;
      gpost_trim_s(starts      >= trimepsilon(1)) = Inf;
      % For each source state, pick best target state to redistribute to
      [d_dist_t, i_dist_t] = max(dpost_dist_t, [], 2);
      % Pick best starting state to redistribute to
      [d_dist_s, i_dist_s] = max(dpost_dist_s, [], 2);
      % Redistribution gains for starts
      g_dist_s = d_dist_s .* starts;

      % First, try to zero all incoming links to a state (preferred)
      gpost_pinch ... % Approximate gain of pinching a state
          = d_dist_t' * transitions ...     % Redistribution gains (but no
          - (d_dist_t .* diag(transitions))' ...   % redistribute to self)
          + g_dist_s ...                 % Redistribution gains for starts
          - sum(gpost_trim_t, 1) ...  % Incoming transition trimming gains
          - sum(gpost_trim_t, 2)' ... % Outgoing transition trimming gains
          + diag(gpost_trim_t)' ...  % Don't double-count self-transitions
          - gpost_trim_s ...         % Starting probability trimming gains
          - y1 .* priorsc1;             % Model description trimming gains
      % States with no occupancy are good for pinching
      % TODO: Do we really need this as a special case?
      gpost_pinch(transitionsD == 0) = REALMIN;
      % Prevent redistributing to the state we're pinching
      gpost_pinch(i_dist_t(i_dist_t ~= (1:n)')) = -Inf;
      gpost_pinch(i_dist_s) = -Inf;
      % Find the best state to pinch, if any
      [g_remove, dead] = max(gpost_pinch);
      if g_remove > 0,
        % Pinch off state dead, redistributing as planned
        starts(dead) = 0;
        starts(i_dist_s) = 0;
        starts(i_dist_s) = max(0, 1 - sum(starts));
        transitions(:,dead) = 0;
        foo = (-n+1:0)' + (i_dist_t.*n);
        transitions(foo) = 0;
        transitions(foo) = max(0, 1 - sum(transitions, 2));
        clear foo;
	[starts, transitions, means, icovariances]= ...
	    kill_state(dead,starts, transitions, means, icovariances);
	starts=stoch(starts); transitions=stoch(transitions);
        if discrete,
          % For output param trimming below
          meansN(dead,:) = [];
          meansD(:,dead) = [];
          if any_y2,
            total_occ = sum(meansD);
            stoch_occ = meansD ./ total_occ;
          end;
        end;
        logstarts(dead) = [];
        logtransitions(dead,:) = [];
        logtransitions(:,dead) = [];
        priorsp(dead) = [];
        priorsc1(dead) = [];
        priorsc2(dead) = [];
        n = n - 1;
        info(dead) = [];
        if ~isempty(prob_factor),
          prob_factor(:,dead) = [];
        end;
	[TstartsN, TtransitionsN, TmeansN, TcovariancesNt]= ...
	    kill_state(dead,TstartsN, TtransitionsN, TmeansN, TcovariancesNt);
        just_trimmed = 1;
        if status >= 0,
          if status > 0, fprintf(2, ' X%d', dead); end;
          trimmed = 1;
        end;
      end;
      clear gpost_pinch g_remove dead;

      % Try to trim transitions, preferring to trim self-transitions
      if ~just_trimmed,
        % Approximate gain of trimming a transition
        gpost_any = vprod(d_dist_t, transitions) - gpost_trim_t;
        % Prevent redistributing to the transition we're trimming
        gpost_any((-n+1:0)' + (i_dist_t.*n)) = -Inf;
        % Prefer to trim self-transitions
        gpost_any((n+1) .* find(diag(gpost_any)>0) - n) = +Inf;
        % Find the best transitions to trim, if any
        [g_any, j_anys] = max(gpost_any, [], 2);
        % Trim at most one transition in each row
        for i_any = (find(g_any > 0))',
          % Trim transition i_any -> j_anys(i_any),
          % redistributing as planned
          transitions(i_any, j_anys(i_any)) = 0;
          transitions(i_any, i_dist_t(i_any)) = 0;
          transitions(i_any, i_dist_t(i_any)) = max(0, ...
              1 - sum(transitions(i_any, :)));
          just_trimmed = 1;
          if status >= 0,
            if status > 0,
              fprintf(2, ' X%d-%d', i_any,  j_anys(i_any));
            end;
            trimmed = 1;
          end;
        end;
        clear gpost_any g_any i_any j_anys;
      end;

      % If even that fails, try to trim from starts
      if ~just_trimmed,
        % Approximate gain of trimming a start
        gpost_start = g_dist_s - gpost_trim_s;
        % Prevent redistributing to the start we're trimming
        gpost_start(i_dist_s) = -Inf;
        % Find the best start to trim, if any
        [g_start, i_start] = max(gpost_start);
        if g_start > 0,
          % Trim start i_start, redistributing as planned
          starts(i_start) = 0;
          starts(i_dist_s) = 0;
          starts(i_dist_s) = max(0, 1 - sum(starts));
          just_trimmed = 1;
          if status >= 0,
            if status > 0, fprintf(2, ' X0-%d', i_start); end;
            trimmed = 1;
          end;
        end;
        clear gpost_start g_start i_start;
      end;

      clear dpost_trim_t dpost_trim_s ...
          dpost_dist_t dpost_dist_s ...
          gpost_trim_t gpost_trim_s ...
          d_dist_t i_dist_t d_dist_s i_dist_s g_dist_s;

      % For discrete HMMs, try to trim output parameters as well
      if discrete & any_y,
        if y2,
          yy = y1 + y2 .* stoch_occ';
        else
          yy = y1;
        end;
        foo = vprod(yy, log(means + REALMIN));
        bar = vadd (yy, log(means + REALMIN));
        % Approximate derivative of post wrt means
        dpost_dist_o = meansN ./ (means - REALMIN) + bar;
        % Approximate gain of post wrt means
        gpost_trim_o = meansN + foo .* means;
        % We can only rely on the approximations above where the
        % probability delta is small
        gpost_trim_o(means >= trimepsilon(3)) = Inf;
        % Holes shall never be redistributed to
        dpost_dist_o(means == 0) = -Inf;
        clear yy foo bar;
        % For each state, pick best output to redistribute to
        [d_dist_o, i_dist_o] = max(dpost_dist_o, [], 2);
        % Approximate gain of trimming an output
        gpost_out = vprod(d_dist_o, means) - gpost_trim_o;
        % Prevent redistributing to the output we're trimming
        gpost_out((-n+1:0)' + (i_dist_o.*n)) = -Inf;
        % Find the best outputs to trim, if any
        [g_out, j_outs] = max(gpost_out, [], 2);
        i_outs = find(g_out > 0);
        % Trim at most one output for each state
        if ~isempty(i_outs),
          for i_out = i_outs',
            % Trim output i_out -> j_outs(i_out),
            % redistributing as planned
            means(i_out, j_outs(i_out)) = 0;
            means(i_out, i_dist_o(i_out)) = 0;
            means(i_out, i_dist_o(i_out)) = max(0, ...
                1 - sum(means(i_out, :)));
            just_trimmed = 1;
            if status >= 0,
              if status > 0,
                fprintf(2, ' Y%d-%d', i_out, j_outs(i_out));
              end;
              trimmed = 1;
            end;
          end;
          % Recompute affected entries in priorsc1
          foo = means(i_outs, :);
          priorsc1(i_outs) = sum(foo .* log(foo + REALMIN), 2)';
          clear foo;
        end;
        clear gpost_out g_out i_outs i_out j_outs ...
            dpost_dist_o gpost_trim_o d_dist_o i_dist_o;
      end;

      % If we still haven't been able to trim anything, bail out
      if ~just_trimmed, break; end;

      % Trim states that are incidentally pinched off
      while n > 1,
        dead = find(~starts & ...
            ~any(transitions - diag(diag(transitions))));
        if isempty(dead), clear dead; break; end;
        % Pinch off states listed in dead; no need to redistribute
	[startsN, transitionsN, meansN, covariancesN]= ...
	    kill_state(dead,startsN, transitionsN, meansN, covariancesN);
        [starts, transitions, means, icovariances]= ...
            kill_state(dead,starts, transitions, means, icovariances);
	[logstarts,logtransitions,Omeans,Oicovariances]= ...
	    kill_state(dead,logstarts,logtransitions, Omeans, Oicovariances);
	starts=stoch(starts); transitions=stoch(transitions);
        priorsp(dead) = []; priorsc1(dead) = []; priorsc2(dead) = [];
        logstarts(dead) = [];
	logtransitions(dead,:) = []; logtransitions(:,dead) = [];
        n = n - length(dead);
        info(dead) = [];
        if ~isempty(prob_factor), prob_factor(:,dead) = []; end;
	[TstartsN, TtransitionsN, TmeansN, TcovariancesNt]= ...
	    kill_state(dead,TstartsN, TtransitionsN, TmeansN, TcovariancesNt);
        just_trimmed = 1;
        if status >= 0,
          if status > 0, fprintf(2, ' I%d', dead); end;
          trimmed = 1;
        end;
      end;

      if status > 0, fprintf(2, ','); end;
      [transitionsN, transitionsD, startsN, covariancesN, ...
            meansN, meansD, like] = baumwelch_all(transitions, starts, ...
          icovariances, means, observations, bw_lengths, prob_factor);
      if trimperiod,
	% Remove states that have zero occupancy
	dead=find(0==meansD);
	if any(dead),
	  fprintf(2,'Dead after trim: %d %d %d %d %d %d %d %d\n',dead);
	  [startsN, transitionsN, meansN, covariancesN]= ...
	      kill_state(dead,startsN, transitionsN, meansN, covariancesN);
	  [starts, transitions, means, icovariances]= ...
	      kill_state(dead,starts, transitions, means, icovariances);
	  [logstarts,logtransitions,Omeans,Oicovariances]= ...
	      kill_state(dead,logstarts,logtransitions, Omeans, Oicovariances);
	  starts=stoch(starts); transitions=stoch(transitions);
	  meansD(dead)=[]; transitionsD(dead)=[];
	  if length(priorsc1)>1, priorsc1(dead)=[];end; priorsc2(dead)=[];
	  n = n - length(dead);
	  info(dead) = [];
	  if ~isempty(prob_factor), prob_factor(:,dead) = []; end;
	  [TstartsN, TtransitionsN, TmeansN, TcovariancesNt]= ...
	      kill_state(dead,TstartsN, TtransitionsN, TmeansN, TcovariancesNt);
	  trimmed=1;
	end;
	clear dead;
      end;
      % Recompute likelihood and posterior
      if any_z,
        logtransitions = log(transitions + REALMIN);
        priorsp = sum(transitions .* logtransitions, 2)';
        logstarts = log(starts + REALMIN);
        priors0 = sum(starts .* logstarts);
        priorp = sum(priorsp) + priors0;
      end;
      if any_y1,
        priorc1 = sum(priorsc1);
      end;
      if any_y2,
        total_occ = sum(meansD);
        stoch_occ = meansD ./ total_occ;
        priorsc2 = priorsc1 .* stoch_occ;
        priorc2 = sum(priorsc2);
      end;
      prev_post = post;
      post = like + zz .* priorp + y1 .* priorc1 + y2 .* priorc2 + priort;

      % Check that the posterior went up
      % (if not, we have a numerical error)
      if post < prev_post,
        if status > 0,
          fprintf(2, ' error %g,', prev_post - post);
        end;
        break;
      end;
    end;
  end;
  if trimmed, % trimmed implies status >= 0
    if status == 0,
      fprintf(2, ',');
    end;
    if discrete,
      fprintf(2, ' %d[%d,%d,%d] left', n, nnz(starts), ...
	nnz(transitions), nnz(means));
    else
      fprintf(2, ' %d[%d,%d] left', n, nnz(starts), ...
          nnz(transitions));
    end;
    trimmed = 0;
  end;
  if status == 0,
    fprintf(2, '\r');
  elseif status == 1,
    fprintf(2, '\n');
  end

  % Apply reestimation transformation matrices, if specified
  if ~isempty(TtransitionsN), transitionsN = TtransitionsN * transitionsN; end;
  if ~isempty(TstartsN),      startsN      = TstartsN      * startsN;      end;
  if ~isempty(TmeansN),       meansN       = TmeansN       * meansN;       end;
  if ~isempty(TmeansD),       meansD       = TmeansD       * meansD(:);    end;
  if ~isempty(TcovariancesNt),
    covariancesN = reshape(reshape(covariancesN, ...
        d.*d, n) * TcovariancesNt, d, d, n);
  end;

  % Re-estimate transitions, starts, means and icovariances
  numerrorokay = 0;
  Otransitions = transitions; Ostarts = starts;
  for i = 1:n,
    if ~isempty(transitions_bias), 
      transitions(i,:) = entropic_map(transitionsN(i,:), zz, [], ...
	  transitions_bias(i,:));
    else
      transitions(i,:) = entropic_map(transitionsN(i,:), zz);
    end;
    if any(transitions(i,:) > 1), keyboard; end;
  end;
  starts = entropic_map(startsN, zz, [], starts_bias);
  if estoutput,
    Omeans = means;
    if discrete,
      % Re-estimate output multinomials
      if y2,
        z = y1 + y2 .* stoch_occ;
      else
        z = repmat(y1, 1, n);
      end;
      if ~isempty(discrete_bias),
	for i = 1:n,
	  means(i,:) = entropic_map(meansN(i,:), z(i), [], discrete_bias);
	end;
      else
	for i = 1:n,
	  means(i,:) = entropic_map(meansN(i,:), z(i));
	end;
      end;
      if any_y,
        priorsc1 = sum(means .* log(means + REALMIN), 2)';
      end;
      clear z;
    else %dbstop if naninf
      % Re-estimate output Gaussians
      Oicovariances = icovariances;
      if y2,
        denom = meansD + y1 + y2 .* stoch_occ;
	% Don't let it go negative
	denom=denom+log(1+exp(-8.*denom))./8;
      else
        denom = meansD;
      end;
      iccbreak = 0;
      for i = 1:n,
        if meansD(i) > 0,
          means(i,:) = meansN(i,:) ./ meansD(i);
        end;
	if (denom(i) > 0) & (rcond(covariancesN(:,:,i))>0)  ,
	  [icovar, fix_result] = fix_covariances(covariancesN(:,:,i) ./ denom(i));
	  switch fix_result,
	    case 1,
	      % Ill-conditioned icovariances fixed.
	      if status >= 0,
		fprintf(2, '*** Fixed ill-conditioned icovariances\n');
	      end;
	      numerrorokay = 1; % Ignore numerical errors in next epoch
	    case -1,
	      % Ill-conditioned icovariances could not be fixed.
	      if status >= 0,
		fprintf(2, '*** Unfixable ill-conditioned icovariances\n');
	      end;
	      numerrorokay = 1; % Ignore numerical errors in next epoch
	      if options.iccabort,
		iccbreak = 1; % Abort training procedure
	      end;
	  end;
	  if 0 & (det(icovar)<full_data_det),
	    % don't let icovariances grow larger than twice the full data set
	    icovariances(:,:,i) = full_data_covar;
	  else
	    icovariances(:,:,i) = icovar;
	  end;
          priorsc1(i) = max(REALMIN,1./det(icovar)); %if priorsc1(i)==0,keyboard;end;
	  if isinf(priorsc1(i))           % Sometimes det() overflows...
	    priorsc1(i)=min(REALMAX,det(covariancesN(:,:,i))./max(REALMIN,denom(i).^d));
	  end;
	end;
      end;
      if iccbreak, break; end;
      % We take the absolute value of priorsc1 to get around a numerical
      % bug in MATLAB that sometimes produces non-positive determinants
      % for badly conditioned icovariance matrices.
      priorsc1 = -log(abs(priorsc1) + REALMIN)./2 - log2pied2;
      %dbclear naninf
    end;
  end;

  % More pretty pictures
  if plotting,
    %subplot(2,1,2); cla;
    show_hmm(means, icovariances, observations, transitions);
    if ~isempty(Capture), 
      [capture_x, capture_map] = capture;
      imwrite(capture_x, capture_map, ...
        sprintf('%s%03d.tiff', Capture, epochs), 'tiff');
      clear capture_x capture_map
    end;
    %if ~isempty(Movie), Movie(:,epoch) = getframe(gcf); end; %TODO: figure(1)?
  end;

  % Advance epoch count
  epochs = epochs + 1;
end;

%if plotting,
%  subplot(4,1,1); cla;
%  show_transitions(transitions');     % TODO: Why transpose?
%  if ~isempty(Movie), Movie(:,epoch) = getframe(gcf); end; %TODO: figure(1)?
%end;

if epochs > itmax & status >= 0,
  if status == 0, fprintf(2, '\n'); end;
  fprintf(2, '=== Terminating after %d epochs%s\n', itmax, CLEAR_EOL);
end;

final = 1;
%snapshot hmm 0              % Take final snapshot

function [s,t,m,c]=kill_state(kill,s,t,m,c)
% modify data structures to remove a state
if nargout>0 & nargin>1 & ~isempty(s), s(kill)=[];     		  end;
if nargout>1 & nargin>2 & ~isempty(t), t(kill,:)=[];t(:,kill)=[]; end;
if nargout>2 & nargin>3 & ~isempty(m), m(kill,:)=[];   		  end;
if nargout>3 & nargin>4 & ~isempty(c), c(:,:,kill)=[]; 		  end;
