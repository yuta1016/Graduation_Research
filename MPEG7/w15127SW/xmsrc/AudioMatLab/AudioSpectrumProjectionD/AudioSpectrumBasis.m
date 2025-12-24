function [V,Y2,delta]=AudioSpectrumBasis(X, NUM_IC, ICA_FLAG, NORM_COLUMNS)

% AudioSpectrumBasis - calculate basis functions and basis projection
%                      of a spectrogram.
%
% [V,Y]=AudioSpectrumBasis(X, NUM_IC, IC_METHOD)
%
% Returns the AudioSpectrumBasis V and AudioBasisProjection Y
% of the AudioSpectrumEnvelope matrix X.
%
% The input matrix X is row-major time frames in the rows,
% i.e. frequency bins are in the columns of the matrix.
%
% Perform dimensionality reduction on spectral channels by
% setting NUM_IC to fewer components than frequency bins.
% For spectrum features, 5 basis functions are typical.
%
% IC_METHOD selects the algorithm for basis extraction.
%       1 = SVD only [default]
%       2 = SVD->ICA
%
% To reconstruct independent spectrograms use: Y(:,k)*pinv(V(:,k))
% where k is a vector of rqeuired component indices, e.g. [1 2 3].
%
$ Copyright (C) Michael A. Casey, 1997-2001, M.I.T., MERL
%                                    All Rights Reserved.


if nargin<4
    NORM_COLUMNS=1; % default: norm ICA matrix columns
end

if nargin < 3
    ICA_FLAG = 1; % default, use SVD only
end

if nargin < 2
    NUM_IC = 5; % default number of independent components.
end

if nargin < 1
    error('Not enough input arguments');
end

if(NUM_IC > size(X,2))
  error('NUM_IC must be less than columns of X');
end
  
if((ICA_FLAG<1)|(ICA_FLAG>2))
   error('IC_METHOD must be 1 (SVD) or 2 (ICA)');
end

delta = diag(eye(NUM_IC));

if ( (ICA_FLAG >= 1) & (ICA_FLAG <=2) )
    
    % Dimension Reduction Step
    fprintf('SVD...')
    [U,A,V] = svd(X,0);
    % figure;stem(diag(A));title('Observation Singular Values');drawnow;
    V1=V(:,1:NUM_IC);
    Y1=X*V1;
    V2=eye( NUM_IC );
    Y2=Y1; % Initialize output matrices
    delta=diag(A(:,1:NUM_IC));
    if ( ICA_FLAG==2 )
	fprintf('ICA[jade]...')
	% Use estimated unmixing matrix as stored basis
	[V2,XX] = jade(Y1', NUM_IC);
	if(NORM_COLUMNS)
	    fprintf('Norm Columns...');
	    [V2] = normcolumns(V2');
	    V2=V2';
	end
	Y2 = Y1*pinv(V2)';
    % Final Rotation Matrix is product of V1 and V2 rotations
    V=V1*pinv(V2)';
else
    fprintf('No dimensionality reduction, skipping...\n');    
    V=[]; % leave empty and later test with isempty(V)
end

% NORMCOLUMNS - make the columns of a matrix unit norm
function [V,delta] = normcolumns(V)

% TODO: cannot invert if any delta is singular, i.e. if close to 0
% Catch error and fix somehow

delta=diag(sqrt(sum(V.*V)));
V=V*inv(delta);

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

