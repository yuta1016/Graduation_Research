% function [] = InstrumentTimbre(FILENAME, f0_bp, do, SCHEMA_FILE, OUTPUT_FILE);
%
% INPUTS:
% =======
% - FILENAME [ascii string]: name of the file to be processed
% - f0_bp                  : fundamental frequency breakpoint function
%   format : [first column: times [second] | second columns f0 [Hz]]
% - do     : 1 (Harmonic) 2 (percussive) 3 (Instrument)
% - SCHEMA_FILE [ascii string]: name of the schema file
% - OUTPUT_FILE [ascii string]: name of the xml output file
% OUTPUTS:
% ========
%
%
% Target:   MP7-XM version
% Author:   CUIDADO/IRCAM/ G. Peeters 
% LastEdit: 2001/03/12
%

function [] = InstrumentTimbre(FILENAME, f0_bp, do, SCHEMA_FILE, OUTPUT_FILE);
  
  if nargin < 3
    do = 3;
  end
  
  % FIRST CREATE THE DOMTREE
  outputXML(SCHEMA_FILE);
  
  [data_v, sr_hz, nbits] = wavread(FILENAME);
  
  % ========================================
  if do == 1 | do == 3
  
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
    
    if do == 1,
      outputXML('HarmonicInstrumentTimbre', 'HarmonicSpectralCentroid',  hsc);
      outputXML('HarmonicInstrumentTimbre', 'HarmonicSpectralDeviation', hsd);
      outputXML('HarmonicInstrumentTimbre', 'HarmonicSpectralSpread',    hss);
      outputXML('HarmonicInstrumentTimbre', 'HarmonicSpectralVariation', hsv);
      outputXML('HarmonicInstrumentTimbre', 'LogAttackTime',             lat);
    elseif do == 3
      outputXML('InstrumentTimbre', 'HarmonicSpectralCentroid',  hsc);
      outputXML('InstrumentTimbre', 'HarmonicSpectralDeviation', hsd);
      outputXML('InstrumentTimbre', 'HarmonicSpectralSpread',    hss);
      outputXML('InstrumentTimbre', 'HarmonicSpectralVariation', hsv);
    end 
  end
  
  
  % ========================================
  if do == 2 | do == 3

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
    
    if do == 2
      outputXML('PercussiveInstrumentTimbre', 'LogAttackTime',     lat);
      outputXML('PercussiveInstrumentTimbre', 'TemporalCentroid',  tc);
      outputXML('PercussiveInstrumentTimbre', 'SpectralCentroid',  sc);
    elseif do == 3
      outputXML('InstrumentTimbre', 'LogAttackTime',     lat);
      outputXML('InstrumentTimbre', 'TemporalCentroid',  tc);
      outputXML('InstrumentTimbre', 'SpectralCentroid',  sc);
    end
    
  end
    
  outputXML('save',OUTPUT_FILE)