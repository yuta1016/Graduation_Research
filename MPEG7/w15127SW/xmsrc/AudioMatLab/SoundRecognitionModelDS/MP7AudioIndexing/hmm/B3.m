%B3 compute multivariate Gaussian probabilities
% function probs = B3(X,icovariance,mean)
%
% multivariate Gaussian; X is a row vector(s)
% takes pains to return a positive value for ill-conditioned covariance matrix

function probs = B3(X,icovariance,mean)

persistent CONSTANTS_INITIALIZED REALMIN REALMAX EPSILON TWOPI
if isempty(CONSTANTS_INITIALIZED),
  REALMIN = realmin;
  REALMAX = realmax;
  EPSILON = 1e-50;
  TWOPI= 2*pi;
  CONSTANTS_INITIALIZED = 1;
end;

bad=find(~isfinite(diag(icovariance)));	% remove dimensions w/infinite variance
if ~isempty(bad)
  icovariance(bad,:)=[]; 
  if isempty(icovariance), error('B3: entire covariance matrix is bad'); end;
  icovariance(:,bad)=[]; X(:,bad)=[]; mean(:,bad)=[]; 
end;

% Compute determinant, taking care of near-flat dimensions
idtr=det(icovariance);
if isinf(idtr)| idtr<=0, idtr=min(REALMAX,prod(max(EPSILON,eig(icovariance)))); end;


[T,d] = size(X);

if d~=size(mean,2)
  disp('B3: error, data and mean do not agree in dimensionality, press any key'); 
  pause;
end

if nargin>2, 
  probs = ...
      exp( -.5 .* mahalonobis(X,icovariance,mean)) ...
      ./ sqrt(TWOPI^d ./ (REALMIN+idtr)) ...
      ;
else
  probs = ...
      exp( -.5 .* mahalonobis(X,icovariance)) ...
      ./ sqrt(TWOPI^d ./ (REALMIN+idtr)) ...
      ;
end;
%probs=min(REALMAX, max(REALMIN, probs));

