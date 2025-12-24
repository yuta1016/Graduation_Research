function [Y2,V,delta]=icasfxdata2(X, NUM_IC, ICA_FLAG, NORM_COLUMNS)

% ICASFXDATA - reduce dimensionality of sound data
%
% [X,V]=icasfxdata(X, NUM_IC)
%
% Perform dimensionality reduction on spectral channels

if nargin<4
    NORM_COLUMNS=0; % default, don't norm ICA matrix columns
end

if nargin < 3
    ICA_FLAG = 1; % default, use SVD only
end

delta = diag(eye(NUM_IC));

if ( (ICA_FLAG >= 1) & (ICA_FLAG <=3) )
    
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
	    [V2,delta] = normcolumns(V2');
	    V2=V2';
	end
	Y2 = Y1*pinv(V2)';
    elseif (ICA_FLAG==3)
	fprintf('ICA[fast]...')
	[XX,V2,W] = fastica(Y1', 'approach', 'symm', ...
	    'verbose', 'off', 'displayMode', 'off');	
	if( isempty( find( V2>eps ) ) | isnan(V2) | isinf(V2))
	    fprintf('warning: fixing V2...');
	    V2=pinv(W);
	end
	% Use estimated unmixing matrix as stored basis
	if(NORM_COLUMNS)
	    fprintf('Norm Columns...');
	    [V2,delta] = normcolumns(V2');
	    V2=V2';
	end
	Y2 = Y1*pinv(V2)';
    end
    % Final Rotation Matrix is product of V1 and V2 rotations
    V=V1*pinv(V2)';
elseif (ICA_FLAG==4)
    fprintf('ICA[fast REDUCE]...')
    [XX,V,W] = fastica(X', 'approach', 'symm', 'verbose', 'off', ...
	'displayMode', 'off', 'lastEig', NUM_IC); 
    if( isempty( find( V>eps ) ) | isnan(V) | isinf(V))
	fprintf('warning: fixing V...');
	V=pinv(W);
    end
    % Use estimated unmixing matrix as stored basis
    if(NORM_COLUMNS)
	fprintf('Norm Columns...');
	[V,delta] = normcolumns(V');
	V=V';
    end
    V = pinv(V)' ;
    Y2 = X*V ;
else
    fprintf('No dimensionality reduction, skipping...\n');    
    V=[]; % leave empty and later test with isempty(V)
    Y2=X;
end

% NORMCOLUMNS - make the columns of a matrix unit norm
function [V,delta] = normcolumns(V)

% TODO: cannot invert if any delta is singular, i.e. if close to 0
% Catch error and fix somehow

delta=diag(sqrt(sum(V.*V)));
V=V*inv(delta);

