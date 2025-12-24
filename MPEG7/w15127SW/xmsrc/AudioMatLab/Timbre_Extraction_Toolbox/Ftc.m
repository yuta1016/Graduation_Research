% function [tc] = Ftc(envelop_bp)
%
% Estimate the temporal centroid of the energy envelop
%
% INPUTS
% ======
% - envelop_bp    : energy envelope (first column: time [second] | second column: value)
%
% OUTPUTS
% =======
% - tc            : temporal centroid
%
% Target:   MP7-XM version
% Author:   CUIDADO/IRCAM/ G. Peeters 
% LastEdit: 2001/03/12
%

function [tc] = Ftc(envelop_bp)

  time_v   = envelop_bp(:,1);
  energy_v = envelop_bp(:,2);
  
  tc = sum( energy_v .* time_v ) / sum(energy_v);
