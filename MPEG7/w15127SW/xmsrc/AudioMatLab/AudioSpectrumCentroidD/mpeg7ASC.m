function AudioSpectrumCentroid = mpeg7ASC(fftout,standvar)

% This function describes the centre of gravity of log-frequency power spectrum
% audiosignal is Audio data


% Written by Melanie Jackson
% Version 1.0 12 Jan 2001
% Modified 16 March 2001 - Removed spectrum extraction to generic function

loedge = 62.5;
numframes = size(fftout,2); 
N = standvar.FFTsize;
fs = standvar.fs;
fft_freq =(0:N/2)*fs/N;
% replace frequencies less than 62.5Hz with nominal freq 31.25Hz
num_less_loedge = sum(fft_freq<loedge);
fft_freq = [31.25 fft_freq(num_less_loedge+1:end)];
% determine log scaled frequencies relative to 1kHz
fft_freq_log = log2(fft_freq/1000);

powers = fftout.^2;
powers = [sum(powers(1:num_less_loedge,:)); powers(num_less_loedge+1:end,:)];
AudioSpectrumCentroid = sum((fft_freq_log'*ones(1,numframes)).*powers)./(sum(powers)+eps);