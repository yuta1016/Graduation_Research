function [c,m]=wcov(X,w,m,flag)
% function [covariance,mean]=wcov(data,weights,mean,flag)
%
% Calculate a weighted covariance around a given mean.  Divides by n-1 to
% yield the best unbiased estimate.  Since this is not the MMSE estimate,
% you can suppress division with the flag.
%
% data -- stacked vectors
% weights -- optional weights (default is all 1's)
% mean -- optional mean (default is sample mean of weighted data)
% flag -- if true, don't divide result by # of data points

num=length(X);
if nargin<2, 
  c=cov(X);
  if nargout>1, m=mean(X); end;
  return;
end;
% calculate mean
if isempty(w),
  m=mean(X)
else
  w=w(:);
  m=w'*X./sum(w); 		% mean of weighted data
end;
if nargin<4, flag=0; end;
% subtract mean from each row
X = vadd(-m,X);				
% calculate scatter
if isempty(w),
  c=X.'*X;
else
  c = X.' * vprod(w,X);			
end;
c=(c+c.')./2;
% normalize
if ~flag, 				
  if isempty(w),
    c=c./(n-1);
  else
    c=c./(sum(w)-1); 
  end;	
end;
