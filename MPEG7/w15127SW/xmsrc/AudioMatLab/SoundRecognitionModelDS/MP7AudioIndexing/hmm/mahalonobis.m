function distance=mahalonobis(x,icov,mean)
%distance=mahalonobis(x,icov,mean) Compute mahalonobis distance from the mean.
%
% X=vadd(-mean,x); distance=sum((X*icov).*X,2);
X=vadd(-mean,x); 
distance=sum((X*icov).*X,2);
