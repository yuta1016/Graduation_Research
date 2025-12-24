function [TransitionsN, TransitionsD, StartsN, ...
      Scatter, MeansN, MeansD, likelihood, gammas] = ...
   baumwelch_all(Transitions, Starts, iCovariances, Means, ...
    Observations, Lengths, ProbFactor)
%BAUMWELCH_ALL -- accumulate expected sufficient statistics for an HMM
%
% $Id $


if nargin<6, Lengths=size(Observations,1); end;

[N,D] 	     = size(Means);		% # states, # dimensions
points       = sum(Lengths);		% # total # data points
gammas       = zeros(points,N);		% first P(datum|state), later P(state)
TransitionsN = zeros(N);		% accumulators
TransitionsD = zeros(1,N);
StartsN      = zeros(1,N);
if nargout>3,
  MeansD       = zeros(1,N);
end;
if points<size(Observations,1),
  Observations=Observations(1:points,:);
end;
discrete = isempty(iCovariances) & min(size(Observations))==1; % discrete output

% Calculate probabilities for every observation
gammas = hmm_prob_obs_given_state(Observations, Means, iCovariances);
if nargin >= 7 & ~isempty(ProbFactor), gammas = gammas .* ProbFactor; end;
%dbstop if naninf
base = 1;
likelihood = 0;
for i=Lengths(:)',
  [lik,forward,backward,gamma]=fb_analysis(Transitions,Starts,gammas,i,base);
  
  likelihood=likelihood+lik;
  
  P_state = sum(gamma(1:(i-1),:));
  if nargout>3, MeansD = MeansD + P_state + gamma(i,:); end;

  % Reestimate Starts
  StartsN = StartsN + gamma(1,:);

  % Reestimate Transitions
  TransitionsN = TransitionsN ...
      +(forward(1:(i-1),:).'...
      *(gammas(base+1:base+i-1,:).*backward(2:i,:))).*Transitions;
  TransitionsD = TransitionsD + P_state;
  
  % replace P(output|state) with P(state) for subsequence just processed
  if nargout>3, gammas(base:base+i-1,:)=gamma; end;
  base=base+i;
end;

if nargout>3,
  if discrete,
    MeansN = dense(gammas, Observations, D)';
    Scatter = [];
  else
    MeansN = gammas.' * Observations; 	% estimate weighted means
    Scatter = zeros(D, D, N);
    for j = 1:N,				% estimate weighted covariances
      Scatter(:,:,j) = wcov(Observations,gammas(:,j),Means(j,:),1);
      bad=D-sum(gammas(:,j)>eps);
      if bad>0,	% # supporting points < # dimensions; inflate remaining dimensions
	[u,v]=eig(Scatter(:,:,j));
	v=diag(v);
	[q,r]=sort(v);
	v(r(1:bad))=q(end)*1e-6;
	Scatter(:,:,j)=u*diag(v)*u';
      end;
    end;
  end;
end;

  % Show probability assignments of each state for the last sequence
global Plotting;
if Plotting,
  subplot(4,1,1);
  imagesc(log(1-log(gamma'+realmin)));
  %plot(gamma);
  %axis('off');axis([1 Lengths(length(Lengths)) 0 1]); 
  drawnow; 
end;


%dbclear naninf
