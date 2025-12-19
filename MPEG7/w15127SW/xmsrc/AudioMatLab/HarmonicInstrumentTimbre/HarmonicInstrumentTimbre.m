% function [hsc, hsd, hss, hsv, lat] = HarmonicInstrumentTimbre(FILENAME);
%
% INPUTS:
% =======
% - FILENAME [ascii string]: name of the file to be processed
% - do     : 1 (Harmonic) 2 (percussive) 3 (Instrument)
% OUTPUTS:
% ========
%
%
% Target:   MP7-XM version
% Author:   CUIDADO/IRCAM/ G. Peeters 
% LastEdit: 2001/03/12
%

function[hsc, hsd, hss, hsv, lat] = HarmonicInstrumentTimbre(FILENAME);
  
  
  [data_v, sr_hz, nbits] = wavread(FILENAME);
  
  % ========================================
      % === compute f0

    % melanie import
    ivar_s     = mpeg7init(sr_hz);
    num_frames = floor((length(data_v) -2 * ivar_s.hopsize)/ ...
		       ivar_s.hopsize);
    timeF0     = (1:num_frames) * ivar_s.hopsize / sr_hz;
    f0_v       = fundfreq(data_v, ivar_s, num_frames);
    f0_bp      = [timeF0' f0_v];
    % end of melanie import
    
    pos_v  = find(f0_bp(:,2)> 10);
    mf0_hz = median(f0_bp(pos_v,2));
    
    % === parametres
    param.nbT0              = 8;
    param.overlap_factor    = 2;
    param.L_sec             = param.nbT0/mf0_hz;
    param.windowTYPE        = 'hamming';
    
    
    % 1) === compute STFTs
    X_m = Fspectre(data_v, sr_hz, param.L_sec, param.overlap_factor, param.windowTYPE);
    
    % 2) === estimate harmonic peaks
    pic_struct = Fharmo(X_m, sr_hz, f0_bp);
    
    % 3) === compute hsc, hsd, hss, hsv
    [hsc, hsd, hss, hsv] = FharmoiParam (pic_struct);
    
    % 4) === write XML file
    
    % === temporal signal envelope (informative)
    param.energy_cutfreq_hz        = 20;        
    param.energy_dsfact            = 3;         
    energy_bp = Fenergy(data_v, sr_hz, param.energy_cutfreq_hz, param.energy_dsfact);
    % === log-attack-time
    lat     = Flat(energy_bp, 2);
    
  
  % ========================================
    
