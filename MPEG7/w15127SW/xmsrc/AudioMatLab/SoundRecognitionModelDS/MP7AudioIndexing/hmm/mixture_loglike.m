% function [loglike,loglikes]=mixture_loglike(x,p,means,covariances)
%
% x -- stacked input vectors (row-major)
% p -- mixture priors
% means -- mixture means
% covariances -- mixture covariances


function [loglike,loglikes]=mixture_loglike(x,p,means,covariances)
n=length(p);
[t,d]=size(x);
if 1==t, x=x(:); t=d; d=1; end;		% if 1d, make sure x is a column vector
for i=1:n, px(:,i)=B3(x,covariances(:,:,i),means(i,:)); end;
q=px*p';
loglikes=(log(q+(q==0).*realmin));		% log-likelihood
loglike=sum(loglikes);