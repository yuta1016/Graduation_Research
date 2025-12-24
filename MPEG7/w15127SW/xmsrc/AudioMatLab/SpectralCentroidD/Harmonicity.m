function [harmonicityRatio,upperLimitOfHarmonicity] = harmonicity(signal2,window,samplingRate,minj_pos)

%% file: harmonicity.m
%% 
%% The function of this file is to calculate two measures: HarmonicRatio 
%% and UpperLimitOfHarmonicity.
%%
%% HarmonicRatio is loosely defined as the proportion of harmonic components 
%% within the power spectrum. It is derived from the output/input power ratio 
%% of a time domain comb filter tuned to the period 1/F0 of the signal. 
%%  
%% UpperLimitOfHarmonicity is loosely defined as the frequency 
%% beyond which the spectrum cannot be considered harmonic. It 
%% is calculated based on the power spectra of the original and 
%% comb-filtered signals. The algorithm is:
%% a)	Calculate r(j) = sum(s(i+j)-s(i)).^2)/(sum(s(i).^2)+sum(s(i+j).^2))
%%    , and find the value of j that minimizes it. A refined, fractional 
%%    estimate can be obtained by parabolic fitting.
%% b)	Calculate the DFTs of the signal s(i) and the comb-filtered 
%%    signal s(i)-s(i+j), using a 32 ms window shaped as a 
%%    raised cosine.  If j is fractionary, s(i+j) is calculated 
%%    by linear interpolation. Calculate power spectra, and 
%%    group the components below 62.5 Hz as explained for 
%%    AudioSpectrumEnvelopeType.
%% c)	For each frequency f calculate the sum of power beyond 
%%    that frequency, for both the original and comb-filtered 
%%    signal, and take their ratio.
%%    a(flim) = sum_fmin_fmax(p'(f))/sum_fmin_fmax(p(f))
%%    where p(f) and p'(f) are the power spectra of the 
%%    unfiltered and filtered signal respectively, and fmax is 
%%    the maximum frequency of the DFT. 
%% d)	Starting from fmin and moving down in frequency, find the 
%%    lowest frequency for which this ratio is smaller than a 
%%    threshold (0.5). If that frequency is 0, replace it by 
%%    31.25 Hz.
%% e) Convert this value to an octave scale based on 1 kHz. 
%% 
%%
%% Copyright (c), IME, All Rights Reserved
%%
%% Author: Dong Yan Huang
%% Version: 1.0  Time: 28 October 2000 (N3489)
%% Last Modified: 27 Mar 2001 (N3704)

samplingRate
minj_pos
%% === Parameters ===
sampleNum = length(window);
   
%% === Memory Allocation ==
snCombfiltered = zeros(sampleNum,1);
%snFFt = zeros(sampleNum,1);
%snComFiltFFt= zeros(sampleNum,1);
%snHarm = zeros(sampleNum,1);

%% === Initialization ===
harmonicityRatio = [];
lowHarmonicity = [];
aflim = [];
lowPos = [];  
%% === Calculate HarmonicRatio ===
%% === step b) calculate DFT of s(i) and s(i)-s(i+j) === 
sn = signal2(1:sampleNum);
snFFT = amplFFT(sn,window);
snPower = snFFT.^2;
for i=1:sampleNum
   pos1 = i+minj_pos;
   sniplusj = interp1(signal2, pos1); 
   snCombfiltered(i) = signal2(i)-sniplusj;
end
snCombFiltFFt = amplFFT(snCombfiltered,window);
snCombPower = snCombFiltFFt.^2;

%% === Calculate power spectra and group the components below 62.5 Hz ===
fftSize = length(snCombFiltFFt);
freqResolution = samplingRate/fftSize;
loFreqNum = fix(62.5/freqResolution);
harmonicityRatio =[harmonicityRatio sum(snCombPower(1:loFreqNum))/sum(snPower(1:loFreqNum))];

%% === For each frequency f calculate the sum of power beyond that frequency,===
%% === for both the original and comb-filtered signal, and take their ratio. ===

for k=loFreqNum+1:fftSize/2
   af = sum(abs(snCombPower(k:fftSize/2)).^2)/sum(abs(snPower(k:fftSize/2)).^2);
   aflim = [aflim af];
end
harmonicityRatio = [harmonicityRatio aflim];

%% === Starting from fmax(sampleNum)  and moving down in frequency, find the ===
%% === lowest frequency for which this ratio is smaller than a threshold (0.5). ===
%% === If that frequency is 0, replace it by 31.25 Hz   ===

for k=length(aflim):-1:1
 if (aflim(k) < 0.5)
   lowHarmonicity = [lowHarmonicity aflim(k)];
   lowPos = [lowPos (k-1)];
 end
end
j = lowPos(length(lowPos));
if j == 0 & length(j) == 0
    fmin = 31.25;
else
    fmin = j*freqResolution;
end

%% === Convert this value to an octave scale based on 1 kHz. ? ===
freq1KIndex = ceil(1000/freqResolution);
freqUpper = ceil((samplingRate/2)/freqResolution);

upperLimitOfHarmonicity = fmin;     
harmonicityRatio
upperLimitOfHarmonicity  