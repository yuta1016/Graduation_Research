% function [lat, sc, tc] = PercussiveInstrumentTimbre(FILENAME);
%
% INPUTS:
% =======
% - FILENAME [ascii string]: name of the file to be processed
% OUTPUTS:
% ========
%
%
% Target:   MP7-XM version
% Author:   CUIDADO/IRCAM/ G. Peeters 
% LastEdit: 2001/03/12
%

function[lat, sc, tc] = PercussiveInstrumentTimbre(FILENAME);
    
  
  [data_v, sr_hz, nbits] = wavread(FILENAME);
  
  % ========================================
    % === temporal signal envelope (informative)
    param.energy_cutfreq_hz        = 20;        
    param.energy_dsfact            = 3;         
    energy_bp = Fenergy(data_v, sr_hz, param.energy_cutfreq_hz, param.energy_dsfact);
    
    % === log-attack-time
    lat     = Flat(energy_bp, 2);
    % === temporal centroid
    tc      = Ftc(energy_bp);
    
    % === power spectrum (informative)
    param.powerspectrum_resolution = 10;
    [power_v, freq_v] = spectrum(data_v, 2^param.powerspectrum_resolution, 0, hamming(2^param.powerspectrum_resolution), sr_hz);
    sqrtpower_v       = sqrt(power_v(:,1));
    H                 = length(sqrtpower_v);
    % === spectral centroid
    sc      = Fihsc(freq_v, sqrtpower_v, H);
    
    
