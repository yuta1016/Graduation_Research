function ss_f = SpectrumSpread(signal,window,scalingRatio,freqResolution,write_flag)
%% File: SpectrumSpread.m
%%
%% Copyright (c), IME, All Rights Reserved
%%
%% Author: Dong Yan Huang
%% Version: 1.0  Time: 28 October 2000 (N3489)
%% Last Modified: 27 Mar 2001 (N3704)
global fid;

amplFft_SeriesOfScalar = amplFFT(signal, window);
fftSize = length(amplFft_SeriesOfScalar);
PowerSpectrum = amplFft_SeriesOfScalar(2:fftSize/2+1).^2;

N = length(scalingRatio);
sc_f = zeros(1,10);
ss_f = zeros(1,10);
sNum = 1;
sumPowG = 0;
sumSqPowG = 0;
sumPow = 0;
for i=1:scalingRatio(1)
   x = log2(31.25*sNum/1000)*PowerSpectrum(sNum);
   sumPowG = sumPowG + x ;
   sumSqPowG = sumSqPowG + x*x;
   sumPow = sumPow + PowerSpectrum(sNum);
end
sc_f(1) = sumPowG/sumPow;
ss_f(1) = sqrt(sumSqPowG/sumPow) - sc_f(1);
for i=2:9
   sumPowG = 0;
   sumSqPowG = 0;
  sumPow = 0;
  for j=1:scalingRatio(i)
     x = log2(freqResolution*sNum/1000)*PowerSpectrum(sNum);
     sumPowG = sumPowG +  x;
     sumSqPowG = sumSqPowG + x*x;
     sumPow = sumPow + PowerSpectrum(sNum);
     sNum = sNum+1;   
  end
  sc_f(i) = sumPowG/sumPow;
  ss_f(i) = sqrt(sumSqPowG/sumPow) - sc_f(i);
end
%scalingRatio(10)
if scalingRatio(10) ~= 0
  sumPowG = 0;
  sumSqPowG = 0;
  sumPow = 0;
  for j=1:scalingRatio(10)
     x = log2(freqResolution*sNum/1000)*PowerSpectrum(sNum);
     sumPowG = sumPowG +  x;
     sumSqPowG = sumSqPowG + x*x;
	  sumPow = sumPow + PowerSpectrum(sNum);
     sNum = sNum+1;   
  end
  sc_f(10) = sumPowG/sumPow;
  ss_f(i) = sqrt(sumSqPowG/sumPow) - sc_f(i);
end
if write_flag == 1
   for k = 1:10
    x = strcat('<Scaling ratio="',num2str(scalingRatio(k)),'" elementNum="1"/>');
    fprintf(fid, '%s\n ',x);
   end
   x = strcat('<SpectrumSpread>', num2str(ss_f),'</SpectrumSpread>');
   fprintf(fid,'%s\n',x);
 end

 
