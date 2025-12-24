% function [hsc, hsd, hss, hsv] = FharmoiParam(pic_struct)
%
% INPUTS:
% =======
% - pic_struct: structure .freqh_v, .amplh_lin_v
%
% OUTPUTS:
% ========
% - hsc: harmonic spectral centroid
% - hsd: harmonic spectral deviation
% - hss: harmonic spectral spread
% - hsv: harmonic spectral variation
%
% Target:   MP7-XM version
% Author:   CUIDADO/IRCAM/ G. Peeters 
% LastEdit: 2001/03/12
%

function [hsc, hsd, hss, hsv] = FharmoiParam(pic_struct)

  nb_frames  = length(pic_struct);
  
  inrg = zeros(nb_frames, 1);
  ihsc = zeros(nb_frames, 1);
  ihsd = zeros(nb_frames, 1);
  ihsv = zeros(nb_frames, 1);
  ihss = zeros(nb_frames, 1);
  
  % === Instantaneous values
  for frame = 1:nb_frames
    
    freqh_v      = pic_struct(frame).freqh_v;
    amplh_lin_v  = pic_struct(frame).amplh_lin_v;
    H            = length(freqh_v);
    
    % ===== inrg
    inrg(frame) = sqrt(sum(amplh_lin_v(1:H).^2)); % === informatif
    % ===== ihsc
    ihsc(frame) = Fihsc(freqh_v, amplh_lin_v, H);
    % ===== ihsd
    SE_lin_v    = Fspecenv(amplh_lin_v); % === informatif
    ihsd(frame) = Fihsd(log(amplh_lin_v), log(SE_lin_v), H);
    % ===== ihss
    ihss(frame) = Fihss(freqh_v, amplh_lin_v, H);
    % ===== ihsv
    if frame > 1
      minH = min([H_old,H]);
      ihsv(frame) = Fihsv(amplh_lin_v_old(1:minH), amplh_lin_v(1:minH), minH);
    end
    amplh_lin_v_old = amplh_lin_v;
    H_old           = H;
  end
  
  % ================

  
  pos_v = find(inrg > max(inrg) * 0.1); % === informatif
  
  hsc   = mean(ihsc(pos_v));
  hsd   = mean(ihsd(pos_v));
  hss   = mean(ihss(pos_v));
  hsv   = mean(ihsv(pos_v));
  
  
  