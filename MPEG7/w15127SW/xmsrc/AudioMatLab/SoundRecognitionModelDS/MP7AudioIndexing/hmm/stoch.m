function p=stoch(p)

% Row-normalize matrices, making sure rows are positive and sum to 1

if ndims(p)>2, error('Can only normalize matrices and vectors'); end;
p=abs(p);
[r,c]=size(p);
if min(r,c)>1,
  q=sum(p,2);				% row sums
  if any(q==0) keyboard; end;
  w=warning;warning('off');
  p=vprod(1./q,p);
  warning(w);
  if ~all(q),
    p(isnan(p))=1./c;
  end;
else
  q=sum(p);
  if q==0,
    p=p+1./max(r,c);
  else
    p=p./q;
  end;
end;
