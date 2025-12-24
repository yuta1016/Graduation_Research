%FB_ANALYSIS dynamic programming E-step for an HMM
% function [loglike,forward,backward,occupation,scale]= ...
%    fb_analysis(Transitions,Starts,P_obs_giv_state,frames,first);
% Transitions -- HMM transition matrix
% Starts -- HMM 1st state probabilities
% P_obs_giv_state -- matrix of output probabilities
% frames -- # frames to process (defaults to all)
% first -- index of beginning (defaults to 1)
% loglike -- log likelihood of entire sequence
% forward -- forward variables
% backward -- backward variables
% occupation -- state occupancy probabilities
% scale -- probability per frame

function [loglike,forward,backward,occupation,scale]= ...
  fb_analysis(Transitions,Starts,P_obs_giv_state,frames,first);


%dbstop if naninf
n_states=length(Starts);
if nargin<4, frames=size(P_obs_giv_state,1); end;
if nargin<5, first = 1; end;
offset=first-1;
awful=realmax./256;
% Calculate the forward variable
forward = zeros(frames,n_states); scale = zeros(1,frames); 	% for speed
forward(1,:) = Starts .* P_obs_giv_state(1,:);
scale(1) = 1./sum(forward(1,:));
forward(1,:) = scale(1) .* forward(1,:);
for t = 2:(frames)
  forward(t,:) = (forward(t-1,:) * Transitions) .* P_obs_giv_state(offset+t,:);
  q=sum(forward(t,:));
  if q>0,
    scale(t) = 1./q;
    forward(t,:) = scale(t) .* forward(t,:);
  else		% trick to protect against near-zero output probs
    scale(t) = awful;
    forward(t,:) = forward(t-1,:) * Transitions;
  end;
end

scale=max(realmin,scale);
loglike=-sum(log(scale));

if (nargout<3) return; end;

% Calculate the backward variable
backward = zeros(frames,n_states);
backward(frames,:) = scale(frames).*ones(1,n_states);
for t = (frames-1):-1:1
  backward(t,:)=min(realmax,(Transitions*(backward(t+1,:) .* P_obs_giv_state(offset+t+1,:)).').'.*scale(t));
end


% Calculate occupation table
% occupation(T,N) = probability of the sequence given state N at time T.
occupation = forward .* backward ; 
% necessary adjustment for scaling
for t = 1:frames, 
  occupation(t,:) = occupation(t,:) ./ scale(t);
end

if nargout>=5,
  scale=1./scale;
end;

dbclear naninf