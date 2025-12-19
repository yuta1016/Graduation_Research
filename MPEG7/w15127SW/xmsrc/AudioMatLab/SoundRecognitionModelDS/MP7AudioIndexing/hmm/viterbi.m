function [logpstar,istar,delta,logP] = viterbi(Transitions,Starts,Means,iCovariances,Observations)
% Viterbi algorithm.  Unless you want 3 or 4 output arguments, you probably
% want to use VITERBI_LITE instead, which is written in C and much faster.

[N,N] = size(Transitions);
[T,d] = size(Observations);

logP = hmm_logprob_obs_given_state(Observations, Means, iCovariances);
logA = log_quiet(Transitions);

% Initialization
delta = zeros(T,N); psi = zeros(T,N);
delta(1,:) = log_quiet(Starts) + logP(1,:);

% Recursion
for t = 2:T
  for j = 1:N
    [maxp,psi(t,j)] = max(delta(t-1,:) + logA(:,j)');
    delta(t,j) = maxp + logP(t,j);
  end
end

% Termination
[logpstar,maxi] = max(delta(T,1:N));
istar(T) = maxi;

% Path backtracking
for t = (T - 1):-1:1
  istar(t) = psi(t+1,istar(t+1));
end
