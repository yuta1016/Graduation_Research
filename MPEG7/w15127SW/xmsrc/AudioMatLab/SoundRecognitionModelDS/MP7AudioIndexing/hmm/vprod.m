function res=vprod(a,b)

% Multiply a and b elementwise.  
% If they differ in dimension, repeatedly multiply one to the other.
% Assumes that the smaller operand is a vector.


[r1,c1]=size(a);
[r2,c2]=size(b);
r=max(r1,r2);
c=max(c1,c2);

if r1==r2, 
  res=zeros(r,c); 
  if r<c,
    for i=1:r1, res(i,:)=a(i,:).*b(i,:); end;
  elseif c1==c2
    res=a.*b;
  elseif c1==1, 
    for i=1:c2, res(:,i)=b(:,i).*a; end; 
  else
    for i=1:c1, res(:,i)=a(:,i).*b; end; 
  end
elseif c1==c2,
  res=zeros(r,c); 
  if c<r,
    for i=1:c1, res(:,i)=a(:,i).*b(:,i); end;
  elseif r1==1, 
    for i=1:r2, res(i,:)=b(i,:).*a; end; 
  else 
    for i=1:r1, res(i,:)=a(i,:).*b; end; 
  end
elseif (r1==1)&(c2==1), 
  res=b*a;
elseif (r2==1)&(c1==1), 
  res=a*b;
else error('vprod: incompatible sizes');
end;
