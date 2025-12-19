function [fixed, result] = fix_covariances(covar, max_ratio)
%FIX_COVARIANCES Inflate covariance matrix to prevent numerical errors.
%   F = FIX_COVARIANCES(C, R) produces and inverts a covariance matrix F
%   similar to the covariance matrix C, ensuring that the reciprocal
%   condition number of F is greater than R and the determinant is nonzero.
%   It tries to preserve volume.  The default value for R, the ratio between
%   the biggest and smallest dimension, is 1e-8.
%
%   [F,RESULT] = FIX_COVARIANCES(C, R) is similar, but returns an additional
%   status code RESULT.  If C need not be fixed, then F is equal to C and
%   RESULT is 0.  If C was successfully fixed, RESULT is 1.  If C is
%   ill-conditioned but could not be fixed for some reason, RESULT is -1.
%
%   $Id: fix_covariances.m,v 1.1.1.1 2002/03/05 12:53:26 karol2000 Exp $

persistent MIN_VOLUME REALMIN;
if isempty(MIN_VOLUME), 
  REALMIN=realmin;
  MIN_LOG_VOLUME=log(REALMIN.*4); 
end

if nargin < 2, max_ratio = 1e-6; end
if rcond(covar) <= max_ratio,
  [eigen_vectors,eigen_values] = eig(covar);
  eigen_values = max(REALMIN,abs(diag(eigen_values)));
  d=length(eigen_values);
  % Enforce maximum ratio of largest to smallest eigen_value.
  % Necessary to ensure that covariance is invertible.
  new_eigen_values=max(eigen_values, max_ratio .* max(eigen_values));
  % Enforce minimum volume
  new_log_volume=sum(log(new_eigen_values));
  desired_volume=max(MIN_LOG_VOLUME,sum(log(eigen_values)));
  new_eigen_values=new_eigen_values.*exp((desired_volume-new_log_volume)./d);
  % Reconstitute as inverted covariance matrix
  fixed = -eigen_vectors * diag(1./new_eigen_values) * -eigen_vectors.';
  result = 1;
else
  fixed = inv(covar);
  result = 0;
end
fixed = (fixed + fixed.')./2;       % Make sure it's symmetric
