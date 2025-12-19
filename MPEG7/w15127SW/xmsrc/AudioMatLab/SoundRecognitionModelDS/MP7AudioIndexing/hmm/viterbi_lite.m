function [path, loglike] = viterbi_lite(...
    transitions, starts, prob_obs_given_state)
%VITERBI_LITE Viterbi dynamic programming for hidden Markov models.
%   [PATH,LOGLIKE] = VITERBI_LITE(T,S,P) takes the hidden Markov model whose
%   transitions and states are specified by T and S, respectively, and runs the
%   Viterbi algorithm using the observation probabilities in P.  Each row in P
%   corresponds to one time step; each column in P corresponds to one state in
%   the Markov model.
%
%   The output arguments are the maximum likelihood state sequence through the
%   model and the log-likelihood, respectively.
%
%   $Id: viterbi_lite.m,v 1.1.1.1 2002/03/05 12:53:38 karol2000 Exp $
