function AudioSpectrumSpread = mpeg7ASS(fftout,standvar,centroid)

% This function describes the second moment of the log-frequency power spectrum
% fftout is the magnitude of the windowed FFT


% Written by Melanie Jackson
% Version 1.0 12 Jan 2001
% Modified 16 March 2001 - Removed spectrum extraction to generic function

loedge = 62.5;
N = standvar.FFTsize;
fs = standvar.fs;
numframes = size(fftout,2); 
fft_freq =(0:N/2)*fs/N;
% replace frequencies less than 62.5Hz with nominal freq 31.25Hz
num_less_loedge = sum(fft_freq<loedge);
fft_freq = [31.25 fft_freq(num_less_loedge+1:end)];
% determine log scaled frequencies relative to 1kHz
fft_freq_log = log2(fft_freq/1000);

Spread = [];

powers = fftout.^2;
powers = [sum(powers(1:num_less_loedge,:)); powers(num_less_loedge+1:end,:)];
if ~exist('centroid')
    centroid = sum((fft_freq_log'*ones(1,numframes)).*powers)./(sum(powers)+eps);
end


a=fft_freq_log'*ones(1,numframes)-ones(length(fft_freq_log),1)*centroid;
AudioSpectrumSpread = sqrt(sum((a.^2).*powers)./(sum(powers)+eps));
