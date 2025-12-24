function ret = hmm_prob_obs_given_state(observations, means, icovariances,fixZeros)
%HMM_PROB_OBS_GIVEN_STATEi Probabilities of observations given states in HMM.
%   HMM_PROB_OBS_GIVEN_STATE(OBSERVATIONS, MEANS, ICOVARIANCES) computes the
%   probability of each observation given in OBSERVATIONS conditional on each
%   state in the HMM model given by MEANS and inverse COVARIANCES.
%
%   $Id: hmm_prob_obs_given_state.m,v 1.1.1.1 2002/03/05 12:53:28 karol2000 Exp $

if ~exist('fixZeros','var')
  fixZeros=0;
end;

n = size(means, 1);
%%keyboard
if isempty(icovariances),
    % Discrete case
    ret = zeros(length(observations), n);
    for j = 1:n,
        ret(:, j) = means(j, observations)';
    end;
else
    ret = zeros(size(observations, 1), n);
    for j = 1:n,
      ret(:, j) = B3(observations, icovariances(:,:,j), means(j,:));
    end
    if fixZeros % optional, it moves the probabilities away from 0
      s=max(ret');
      zndx=find(log(s)<-300); % about ^-130< 10^-180;
      zndx
      ret(zndx,:)=ret(zndx,:)*10^200;
      fprintf('hmm_prob_obs_given_state: %d possibly zero probabilities fixed\n', length(zndx));
    end  
end
