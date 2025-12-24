function Y = TrainHiddenMarkovModel(X, maxenv, numFrames, nstates, NUM_IC, ICA_FLAG, ...
    NORM_COLUMNS, USE_ENV, NORMALIZE, SCALE, T0, HMM_SIMPLE)

% TrainHiddenMarkovModel - train a hidden markov model using an entropic prior and model trimming. 
%
% INPUTS:
%         X  - spectrogram data
%    maxenv  - maximum of energy envelope
%  numFrames - number of frames per sequence
%   nstates  - number of hidden states in HMM [5]
%    NUM_IC  - number of components to extract
%   ICA_FLAG - set to 1 for ICA dimension reduction [0]
%
% Y.[T,S,M,C,X,MM,lens,maxenv,V,p]
%
% OUTPUTS: Y - structure variable with fields:
%     T - state transition matrix
%     S - prior state distribution
%     M - stacked means
%     C - stacked covariances
%     X - the observation sequences
%  lens - observation start/stop times
% maxenv- peak of pre-normalized envelope function
%     V - ICA vectors
% delta - ICA original column norms
%     p - final log-posterior probability
%
% Copyright (C) August 2000 Michael A. Casey, MERL, All Rights Reserved

error(nargchk(3,12,nargin));

if nargin < 12
    HMM_SIMPLE=0;
end

if nargin < 11
    T0=100;
end

if nargin < 10
    SCALE = 10;
end

if nargin < 9
    NORMALIZE = 1; % Scale data to condition covariances
end

if nargin < 8 
    USE_ENV = 1; % Use energy envelope
end

if nargin <7
    NORM_COLUMNS = 1; % Default, norm ICA matrix columns
end

if nargin <6
    ICA_FLAG=2; % Use independent component subspace reduction
end

if nargin < 5
    NUM_IC=5;
end

if nargin < 4
    nstates = 5;
end

SR=16000;   % Sample rate
H=128;      % Hop size
FR=20;      % Observation frame rate for continuous raw data files

Y.maxenv = maxenv;
Y.numFrames = numFrames;
if(T0==0)
    Y.T0=mean(numFrames) ;
    fprintf('T0=%3.2f...',Y.T0);
else
    Y.T0 = T0 ; % Initial annealing temperature
end

Y.scale = 1.0;

if(USE_ENV)    
    % Perform dimension reduction and basis rotation
    [ Z, Y.V, Y.delta ] = icasfxdata2( X(:,2:end), NUM_IC, ICA_FLAG, ...
	NORM_COLUMNS );
    YX = [X(:,1) Z];    
else
    % Perform dimension reduction and basis rotation
    [ YX, Y.V, Y.delta ] = icasfxdata2( X, NUM_IC, ICA_FLAG, ...
	NORM_COLUMNS );    
end

if(NORMALIZE)
    fprintf('Scaling ...')
    % Fixed scale factor, helps condition covariances
    Y.scale=SCALE;
    YX = YX * Y.scale ;
end

fprintf('[%d components]...', size(YX,2)-(USE_ENV>0))

% Get the indices for each observation sequence
numSfxFiles = length( Y.numFrames );

if(numSfxFiles>1) % Obtain directly from file lengths
    Y.lens = lengths_to_indices(Y.numFrames);
else % Sample at FR Hz
    Y.lens = lengths_to_indices(ones(fix(FR*Y.numFrames/(SR/H)),1)*fix(SR/H/FR)); 
end

% TRAIN HMM

if(HMM_SIMPLE)
    fprintf('\nTraining Ordinary Baum-Welch HMM...\n\n')
    % [Y.T,Y.S,Y.C,Y.M,Y.p]=hmm_simple2(YX, Y.numFrames, nstates);
    
    [Y.M,Cov,Y.T,Y.S,Y.p]=hmm_simple(YX, Y.numFrames(1), nstates);
    Y.C=repmat(inv(Cov),[1 1 nstates]);    % Undo Zubin's Tied
    
else % Train Entropic
    fprintf('\nTraining entropic MAP HMM...\n\n')
    [Y.T,Y.S,Y.M,Y.C,e,Y.p]=hmm(YX,...
	Y.lens,...         % start and stop indicies of training data
	{'status', 1,...     % show status
	'epsilon', 1e-10,...   % min necessary proportional gain in loglikelihood
	'itmax', 200,...       % max iterations
	'z', [1 -1],...        % use entropic prior
	'y', [1 -1; 0 0],...   % minimize entropies of component distributions
	'trimperiod', 1,...    % try trimming every 5 epochs
	'trimratio', 1e-1,...  % don't remember what this does
	'trimepsilon', 1e-10,...  % tolerance of numerical error
	'trimcycles', 5,...    % ?
	'iccabort', 0},...     % abort if covariances become ill-conditioned
	ones(nstates,nstates)./nstates, stoch(1.5.^-(1:nstates)));
end
    
% 'schedule', Y.T0*[ones(1,20),0.9.^[0:1:100],0],...  % annealing schedule


