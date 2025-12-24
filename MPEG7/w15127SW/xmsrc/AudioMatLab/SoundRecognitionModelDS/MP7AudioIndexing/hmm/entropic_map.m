function [theta, loglike] = entropic_map(omega, z, kappa, bias)
%ENTROPIC_MAP Compute MLE values of theta given omega.
%
%   Copyright 1997-9 Matthew Brand, Mitsubishi Electric Research Labs
%   This is a beta version, NOT FOR DISTRIBUTION.
%
%   [THETA, LOGLIKE] = ENTROPIC_MAP(OMEGA, Z, KAPPA, BIAS) uses a fixpoint derived
%   from the Lagrangian of the log-likelihood to compute theta given omega.  The
%   inverse of the fixpoint is used to estimate theta, theta is normalized, and
%   then the fixpoint equation is used to see how far we are from the solution.
%   Convergence usually takes just a few iterations.  The output arguments
%   returned are the optimal theta vector and the final log-likelihood value
%
%       loglike = sum(  (omega+bias) .* log(theta)
%                       + z .* theta .* log(theta)
%                       + theta .* kappa)
%
%   theta -- multinomial parameters
%   omega -- evidence vector (default: 1)
%   z     -- exponent on prior (default: 1)
%   kappa -- auxiliary entropy (default: 0)
%   bias  -- bias on the evidence (default: 0)
%            e.g., Dirichlet exponents or Jeffrey's prior (bias=-1/2)
%
%   Warning: this is not numerically stable for sum(omega) < 1 or large z > 0.
%
%   $Id: entropic_map.m,v 1.1.1.1 2002/03/05 12:53:24 karol2000 Exp $

% Initialize and check arguments
error(nargchk(1, 4, nargin));
if any(omega < 0), 
  warning('entropic_map: ignoring negative evidence values'); 
  omega=max(0,omega);
end;
if nargin < 2 | isempty(z),     z = 1;     end;
if nargin < 3 | isempty(kappa), kappa = 0; end;
if nargin < 4 | isempty(bias),  
  bias = 0;  
else 
  omega = max(0,omega + bias); 
  if all(0==omega), theta=omega+1./length(omega); return; end;
end;


% Constants
persistent CONSTANTS_INITIALIZED
persistent PHI NPHI REALMAX REALMIN
if isempty(CONSTANTS_INITIALIZED),
  CONSTANTS_INITIALIZED = 1;
  PHI     = (sqrt(5)-1)./2;
  NPHI    = 1-PHI;
  REALMAX = realmax;
  REALMIN = realmin;
end;

% Special case: heat death
if z >= REALMAX,			% construct uniform distribution
  theta = omega > 0; 
  q = sum(theta);
  if q > 0, 
    theta = theta./q;		
  else 
    q = length(omega); theta = ones(size(omega))./q; 
  end;
  loglike = z.*log(q);			% posterior dominated by prior
  return;
end;

% Special case: zero exponent and zero kappa
if ~any(z) & ~any(kappa),
  theta = stoch(omega);
  if nargout >= 2, loglike = sum(omega .* log(theta)); end;
  return;
end;

% Check for values in omega that are very close to zero
% TODO: With z < 0 | any(kappa), we might want to handle zeros in omega diffly.
ok = omega > eps.^2;
if any(ok == 0),
  q = sum(ok);
  if q > 1,
    % Two or more nonzero parameters -- skip those that are zero
    if length(kappa) > 1, kappa = kappa(ok); end;
    if length(z    ) > 1, z     = z    (ok); end;
    [fix, loglike] = entropic_map(omega(ok), z, kappa);
    theta = ok; theta(ok) = stoch(fix); % TODO: why stoch? isn't it redundant?
  elseif q == 1,
    % Only one nonzero parameter -- return spike distribution
    theta = ok;
    loglike = 0;
  else
    % Everything is zero -- return uniform distribution
    theta = repmat(1/prod(size(omega)), size(omega));
    loglike = 0;
  end;
  return
end;

% Trivial case -- only one parameter
if length(omega) == 1,
  theta = [1];
  loglike = 1;
  return
end;

% Fixpoint loop
loglike         = -REALMAX;
dloglike        = 0;
theta           = omega ./ sum(omega) + REALMIN;
if z == 0,
  meankappa     = mean(kappa);
  minlambda     = max(kappa);
  maxlambda     = +Inf;
  lambda        = sum(omega) + meankappa;
else
  omegaz        = omega./z;
  kappaz        = kappa./z;
  logomegaz_pkz = log(omega) - log(abs(z)) + kappaz;
  if z < 0,
    minlambda   = max(logomegaz_pkz) - 700;
  else
    % For z > 0, we need to restrict minlambda so that the argument to
    % loglambertwn1 below stays within range
    minlambda   = max(logomegaz_pkz) + 2;
    minlambda   = minlambda * (1 + eps * sign(minlambda));
  end;
  maxlambda     = min(logomegaz_pkz) + 700;
  lambda        = sum(omegaz) + 1 + mean(kappaz) + max(log(theta));
end;
lambda          = min(maxlambda, max(minlambda, lambda));
oldlambda       = lambda;
dlambda         = REALMAX;
signz           = sign(z);

% Iterate fixpoint until numerical error intrudes.
if minlambda < maxlambda,
  while 1,

    % Store previous values
    oldtheta    = theta;
    oldloglike  = loglike;
    olddloglike = dloglike;
    olddlambda  = dlambda;

    % Step theta (inverse fixpoint)
    if z == 0,
      theta = max(omega ./ (lambda - kappa), 0);
    elseif z < 0,
      theta = max(omega ./ loglambertw0((lambda - 1) - logomegaz_pkz), 0);
    else
      theta = max(omega ./ loglambertwn1((lambda - 1) - logomegaz_pkz), 0);
    end;
    theta = theta ./ sum(theta) + REALMIN; % normalize
    logtheta = log(theta);

    % Compute new entropic MLE log-likelihood
    loglike = sum(omega .* logtheta + z .* theta .* logtheta + theta .* kappa);
    dloglike = loglike - oldloglike;

    % Compare and save
    if dloglike == 0,
      if signz ~= sign(dlambda), break; end;
      % Back up half a step
      theta   = oldtheta;
      loglike = oldloglike;
      dlambda = dlambda ./ 2;
      lambda  = lambda - dlambda;
    elseif dloglike < 0,
      % Golden mean
      theta = oldtheta;
      if olddloglike + dloglike <= 0,
        loglike = oldloglike;
        logtheta = log(theta);
        break;
      end;
      loglike = oldloglike;
      lambda = NPHI .* lambda + PHI .* oldlambda;
      dlambda = lambda - oldlambda;
      olddlambda = REALMAX;
    else
      % Improvement
      oldlambda = lambda;
      if z == 0,
        lambda = mean(omega./theta) + meankappa;
      else
        lambda = 1 + mean(omegaz./theta) + mean(logtheta);
      end;
      lambda = min(maxlambda, max(minlambda, lambda));
      dlambda = lambda - oldlambda;
    end;
  end;

else
  % The range of logomegaz_pkz seems totally out of whack -- what the heck,
  % let's just skip the fixpoint loop
  logtheta = log(theta);
end;

% Very close now; polish up with 2nd order Newton-Raphson with bisection.
nm1 = length(theta) - 1;        % n-1
nonm1 = 1 + 1/nm1;              % n/(n-1)
loglike = sum(omega .* logtheta + z .* theta .* logtheta + theta .* kappa);
while 1,

  % Store previous values
  oldloglike = loglike;
  oldtheta = theta;

  % Function we want root of
  ratio = omega ./ theta;
  dtheta = ratio + z .* logtheta + kappa;
  dtheta = dtheta - mean(dtheta);
  ddtheta = (z - ratio) ./ theta; % 1st derivative of dtheta

  % 1st order Newton-Raphson via Jacobian
  jacobian = vadd(-ddtheta ./ nm1, diag(ddtheta .* nonm1));
  [l,u] = lu(jacobian);
  if rcond(u) > 0,
    w = warning; warning off; delta=-(u\(l\(-dtheta')))'; warning(w);
  else
    dddtheta = (2.*ratio - z) ./ (theta.^2); % 2nd derivative of dtheta
    factor = ddtheta.^2 - 2.*dtheta.*dddtheta;
    if factor >= 0,
      delta = (ddtheta + sqrt(factor)) ./ dddtheta; % 2nd order Newton-Raphson
      if delta > theta | sum(abs(delta)) == 0,
        delta = (ddtheta - sqrt(factor)) ./ dddtheta; % 2nd order Newton-Raphson
      end;
      if delta > theta | sum(abs(delta)) == 0,
        ddtheta(ddtheta == 0) = REALMAX;
        delta = dtheta ./ ddtheta; % 1st order Newton-Raphson
      end;
    else
      ddtheta(ddtheta == 0) = REALMAX;
      delta = dtheta ./ ddtheta; % 1st order Newton-Raphson
    end;
  end;

  % If (omitted) higher-order terms are significant, must scale down delta
  [problem, where] = max(delta ./ theta);
  if problem > 1, delta = delta .* (NPHI ./ problem); end;
  theta = max(theta - delta, 0);
  theta = theta ./ sum(theta) + REALMIN;
  logtheta = log(theta);
  loglike = sum(omega .* logtheta + z .* theta .* logtheta + theta .* kappa);
  if loglike <= oldloglike,
    theta = theta .* NPHI + oldtheta .* PHI;
    logtheta = log(theta);
    loglike = sum(omega .* logtheta + z .* theta .* logtheta + theta .* kappa);
    if loglike <= oldloglike,
      theta = oldtheta;
      loglike = oldloglike;
      break;
    end;
  end;
end;
