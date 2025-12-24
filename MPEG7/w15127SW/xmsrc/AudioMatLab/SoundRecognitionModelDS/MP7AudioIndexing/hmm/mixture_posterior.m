function [post, like, post_p, post_c1, post_c2, post_t] ...
    = mixture_posterior(observations, means, covariances, ...
        priors, zz, y1, y2, temperature, occupancies)
%MIXTURE_POSTERIOR Calculate mixture model log-posterior.
%   [POST, LIKE, POST_P, POST_C1, POST_C2, POST_T] = MIXTURE_POSTERIOR(
%   OBSERVATIONS, MEANS, COVARIANCES, PRIORS, ZZ, Y1, Y2, TEMPERATURE) computes
%   the log-posterior for the mixture model.
%
%   OBSERVATIONS is the observation data.  MEANS, COVARIANCES and PRIORS form
%   the model itself.  ZZ, Y1 and Y2 are coefficients for the entropic prior
%   terms in the log-posterior.  TEMPERATURE is used in the driving term in the
%   log-posterior.
%
%   POST is the log-posterior; it is the sum of LIKE (the likelihood of the
%   observations), POST_P (the prior entropy term), POST_C1 (the covariance
%   description entropy term), POST_C2 (the covariance distribution entropy
%   term), and POST_T (the driving term).
%
%   If one additional argument OCCUPANCIES is given, the likelihood computed is
%   not the likelihood in the mixture model but the likelihood used during EM to
%   train the mixture model.
%
%   $Id: mixture_posterior.m,v 1.1.1.1 2002/03/05 12:53:32 karol2000 Exp $

error(nargchk(8, 9, nargin));

logpriors = log(priors + realmin);

if nargin == 8,
    like = mixture_loglike(observations, priors, means, covariances);
else
    px = zeros(size(observations,1), size(means,1));
    for i = 1:size(means,1),
        px(:,i) = B3(observations, covariances(:,:,i), means(i,:));
    end;
    like = sum(sum(occupancies) .* logpriors) ...
         + sum(log(sum(occupancies .* px, 2)));
end;

if zz,
    post_p = zz * sum(priors .* logpriors);
else
    post_p = 0;
end;

if y1 | y2,
    [n, d] = size(means);
    log2pied2 = 1/2 + (d./2).*log(2.*pi);

    priorsc1 = zeros(1,n);
    for i = 1:n, priorsc1(i) = det(covariances(:,:,i)); end;
    priorsc1 = -log(abs(priorsc1) + realmin)./2 - log2pied2;
    if y1, post_c1 = y1 * sum(priorsc1); else post_c1 = 0; end;
    if y2, post_c2 = y2 * sum(priorsc1 .* priors); else post_c2 = 0; end;
else
    post_c1 = 0;
    post_c2 = 0;
end;

post_t = -temperature.^2/2;

post = like + post_p + post_c1 + post_c2 + post_t;
