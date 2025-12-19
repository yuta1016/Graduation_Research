function [Ae,Se]=jade(X,m)
%
% function [Ae,Se]=jade(X,m)
%
% Jade performs `Source Separation' in the following sense:
% X is an n x T  data matrix assumed to obey the following linear model
%
%         X = A S + N
%
% where o  A is an unknown n x m matrix with full rank.
%       o  S is a m x T data matrix (source signals) with the properties
%            a) for each t, the components of S(:,t) are statistically independent
%            b) for each p, the S(p,:) is the realization of a zero-mean `source signal'.
%            c) At most one of these processes has a vanishing 4th-order cumulant.
%       o  N is a n x T matrix. It is a realization of a spatially white Gaussian noise, i.e.
%            Cov(X) = sigma*eye(n) with unknown variance sigma
%
% Jade performs source separation via a :
% Joint
% Approximate
% Diagonalization of
% Eigen-matrices.
%
% Input :
%   * X: Each column of X is a sample from the n sensors
%   * m: m is an optional argument for the number of sources.
%     If ommited, JADE assumes as many sources as sensors.
%     Note: automatic detection should be included in future releases.
% Output :
%     * Ae is an estimate of the mixing matrix A
%     * Se is an estimate of the source signals S
%
% Version 1.2 : Feb 1994
% March, APril 97: Corrected some mistakes **in the comments !!**
%
% This version does *not* assume circularly distributed signals as 1.1 did.
% The difference only entails more computations in estimating the cumulants
% THIS VERSION STILL ASSUMES ZERO-MEAN SIGNALS  
%
% Contact JF Cardoso for any comment bug report,and UPDATED VERSIONS.
% email : cardoso@sig.enst.fr 
% 
% Reference:
%  @article{CS_iee_94,
%   author = "Jean-Fran\c{c}ois Cardoso and Antoine Souloumiac",
%   journal = "IEE Proceedings-F",
%   title = "Blind beamforming for non {G}aussian signals",
%   number = "6",
%   volume = "140",
%   month = dec,
%   pages = {362-370},
%   year = "1993"}
%_________________________________________________________________________


[n,T]=size(X);

% source detection not implemented yet !
if nargin==1, m=n ; end;

nem	= m;		% number of eigen-matrices to be diagonalized
nm	= nem*m; 	% for convenience
M	= zeros(m,nm);	% array to hold the significant eigen-matrices

%%% whitening
Rx=(X*X')/T;
if m<n, %assumes white noise
  [U,D]=eig(Rx); [puiss,k]=sort(diag(D));
  ibl= sqrt(puiss(n-m+1:n)-mean(puiss(1:n-m)));
   bl = ones(m,1) ./ ibl ;
   W=diag(bl)*U(1:n,k(n-m+1:n))';
  IW=U(1:n,k(n-m+1:n))*diag(ibl);
else    %assumes no noise
   IW=sqrtm(Rx);
   W=inv(IW);
end;
Y=W*X;

%%% moment holding
R	= zeros(m)	; % 2nd-order moments
Rs	= zeros(m)	; % 2nd-order moments without conjugation
Q	= zeros(m*m)	; % 4th-order moments and then 4th-order cum
Yt	= zeros(m,1)	; % buffer
Z	= zeros(m)	; % buffer

% moment estimation
for t=1:T,
	Yt	= Y(:,t)		;
	Rs	= Rs + Yt*Yt.'		;
	Z	=      Yt*Yt'		;
	R	= R  + Z		;
	Q	= Q  + Z(:)*Z(:)'	;
end;
Q=Q/T; R=R/T; Rs=Rs/T ;

% computation of the 4th-order cumulants
u=0;for k=1:m, for l=1:m,
	u	= u+1; 
  	Z(:)	= Q(:,u);
	Z	= Z - R(k,l)*R - R(:,l)*R(k,:) - Rs(:,k)*Rs(:,l)' ;
	Q(:,u)	= Z(:) ;
end, end

%%%computation and reshaping of the significant eigen matrices
[U,D]=eig(Q); [la,K]=sort(abs(diag(D)));h=m*m;
for u=1:m:nm, Z(:)=U(:,K(h));M(:,u:u+m-1)=la(h)*Z;h=h-1; end;

%%%joint approximate diagonalization
encore=1; V=eye(m); % init
seuil=1/sqrt(T)/100;% a statistical threshold
while encore, encore=0;
 for p=1:m-1,
  for q=p+1:m,
   %%% Givens rotations
   g=[   M(p,p:m:nm)-M(q,q:m:nm)  ;
         M(p,q:m:nm)+M(q,p:m:nm)  ;
      i*(M(q,p:m:nm)-M(p,q:m:nm)) ];
	  [vcp,D] = eig(real(g*g')); [la,K]=sort(diag(D));
   angles=vcp(:,K(3));angles=sign(angles(1))*angles;
   c=sqrt(0.5+angles(1)/2);
   sr=0.5*(angles(2)-j*angles(3))/c; sc=conj(sr);
   oui = abs(sr)>seuil ;
   encore=encore | oui ;
   if oui , %%%update of the M and V matrices 
    colp=M(:,p:m:nm);colq=M(:,q:m:nm);
    M(:,p:m:nm)=c*colp+sr*colq;M(:,q:m:nm)=c*colq-sc*colp;
    rowp=M(p,:);rowq=M(q,:);
    M(p,:)=c*rowp+sc*rowq;M(q,:)=c*rowq-sr*rowp;
    temp=V(:,p);
    V(:,p)=c*V(:,p)+sr*V(:,q);V(:,q)=c*V(:,q)-sc*temp;
   end%% if
  end%% q loop
 end%% p loop
end%% while

%%%estimation of the mixing matrix and signal separation
Ae	= IW*V;
Se	= V'*Y ;

