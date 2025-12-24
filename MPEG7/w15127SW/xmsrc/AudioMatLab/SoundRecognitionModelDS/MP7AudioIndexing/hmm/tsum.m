function M=tsum(v1,v2) % tensor sum of two vectors, v1->cols; v2->rows

r=length(v1);
c=length(v2);
M=zeros(r,c);
for i=1:r, a=v1(i); for j=1:c, M(i,j)=a+v2(j); end; end;
