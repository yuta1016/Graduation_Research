function Z = ViterbiDecode(Y, X, numFrames, USE_ENV, NORMALIZE, MAXENV)

%VITERBIDECODE - Decode sound data against HMM Model
%
%
% Copyright (C) September 2000 Michael A. Casey, MERL, All Rights Reserved

if nargin < 6
    MAXENV=0.0;
end

if nargin<5
    NORMALIZE=0;
end

if nargin<4
    USE_ENV=0;
end

VITERBI=1; % Use viterbi decoder

if(~isempty(Y.V))
    % Project against model basis vectors
    if(USE_ENV)

	if(MAXENV>eps)
	    X = [X(:,1)/MAXENV X(:,2:end) * Y.V];
	else
	    X = [X(:,1) X(:,2:end) * Y.V];
	end
    else
	X = X * Y.V;
    end
end

% Re-scale normalized data if required
if(NORMALIZE)
    % Perform data scaling to model Y
    X = X * Y.scale ;
end

% Compute likelihood of model against new data
% For each sound, calculate the viterbi path and log likelihood of sequence


ZP = hmm_prob_obs_given_state( X, Y.M, Y.C ) ;
lens=lengths_to_indices(numFrames);

if(VITERBI)
    for k=1:size(lens,1)
	% Calculate state path
	[ Z.Path{k}, Z.LLM(k) ] = viterbi_lite ( Y.T, Y.S, ZP(lens(k,1):lens(k,2),:) ) ;
    end
else
    for k=1:size(lens,1)
	% Calculate likelihoods using forward-backward analysis
	Z.LLM(k) = fb_analysis ( Y.T, Y.S, ZP(lens(k,1):lens(k,2),:) ) ;
    end
end
