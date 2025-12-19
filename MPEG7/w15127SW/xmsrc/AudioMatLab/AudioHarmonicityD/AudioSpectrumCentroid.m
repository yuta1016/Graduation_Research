function SpectrumCentroid_SeriesOfVector = AudioSpectrumCentroid(auData,totalSampleNum,samplingRate,framePeriod) 

%% File: AudioSpectrumCentroid.m
%%
%% -------------AudioSpectrumCentroidType --------- 
%%
%% The function of the file is to describe the center of 
%% gravity of the log-frequency power spectrum.
%% 
%% MATLAB CODE for SpectralCentroid (sc)
%% input:
%% -auData:                          incoming signal
%% -totalSampleNum:                  total Sample Number of signal
%% -samplingRate:                    sampling rate of the signal
%% -framePeriod                      analysis step
%% output:
%% -SpectrumCentroid_SeriesOfVector: spectrum centroid data
%%   
%%
%% 
%%
%% Copyright (c), IME, All Rights Reserved
%%
%% Author: Dong Yan Huang
%% Version: 1.0  Time: 28 October 2000 (N3489)
%% Last Modified: 27 Mar 2001 (N3704)
global fid;
%global amplFft_SeriesOfScalar;

weight = [];
mean_flag = 1; 
write_flag = 1; % write_flag = 1 (write DDL in XML file)
                % write_flag = 0 (not write DDL in XML file)
                % write_flag = 2 (write values in XML file)
if length(weight)==0
   weight_flag = 0;
else
   weight_flag = 1;
end

% === Spectrum Extraction  ===
hopSize = framePeriod;
analysisStep = hopSize*samplingRate;
sampleNum = fix(samplingRate*0.032);
frameNum = floor(totalSampleNum/sampleNum);
windowSize = sampleNum;
% === analysis parameters  ===
window = hanning(windowSize);
fftSize = 2^nextpow2(windowSize);
freqResolution = samplingRate/fftSize;
% === Scale Ratio ===
  % === Octave division === 
freq1KIndex = ceil(1000/freqResolution);
freq1K6Index = ceil(16000/freqResolution);
x = log2(freq1KIndex);
m = x/fix(x);
scaleIndex = [1 3 5 9 17 33 65 129 257 513 513+fftSize/2-freq1K6Index];
if m == 1 & freq1KIndex - 32 ~= 0
   xl = x-5;
   for i=2:9
      scaleIndex(i+1) = scaleIndex(i+1)+ 2^i;
   end
   scaleIndex(11) = scaleIndex(10) + fftSize-freq1K6Index;
elseif freq1KIndex - 32 ~= 0
   xl = freq1KIndex - 32;
   xh = freq1K6Index -512;
   xl_av = fix(xl/4);  %?
   xh_av = fix(xh/4);  %?
   for i = 2:4
      scaleIndex(i+1) = scaleIndex(i+1) + xl_av;
      scaleIndex(i+5) = scaleIndex(i+5) + xh_av;
   end
   xl_end = xl - 3*xl_av;
   xh_end = xh - 3*xh_av;
end
for i = 1:10
      scaleRatio(i) = scaleIndex(i+1) - scaleIndex(i);
end
%scaleRatio
fid = fopen('AudioSpectrumCentroid.xml','w');
x = '<!-- ##################################################################### -->';
fprintf(fid, '%s\n',x);
x = '<!-- Definition of AudioSpectrumCentroidType, the center of gravity of     -->';
fprintf(fid, '%s\n',x);
x = '<!-- log-frequency power spectrum                                          -->';
fprintf(fid, '%s\n',x);
x = ' <!-- ##################################################################### -->';
fprintf(fid, '%s\n',x);
x = ' <!-- <complexType name="AudioSpectrumCentroidType">                        -->';
fprintf(fid, '%s\n',x);
x = ' <!-- <complexContent>                                                      -->';
fprintf(fid, '%s\n',x);
x = ' <!-- <extension base="mpeg7:AudioSampledType">                             -->';
fprintf(fid, '%s\n',x);
x = ' <!-- <sequence>                                                            -->';
fprintf(fid, '%s\n',x);
x = ' <!-- <element name="Value" type="mpeg7:SeriesOfScalarType" maxOccurs="unbounded"/> -->';
fprintf(fid, '%s\n',x);
x = ' <!-- </sequence>                                                           -->';
fprintf(fid, '%s\n',x);
x = ' <!-- </extension>                                                          -->';
fprintf(fid, '%s\n',x);
x = ' <!-- </complexContent>                                                     -->';
fprintf(fid, '%s\n',x);
x = ' <!-- </complexType>                                                        -->';
fprintf(fid, '%s\n',x);


x = '<AudioSegment idref="102">';
fprintf(fid, '%s\n',x);
x = ' <!-- MediaTime, etc. -->';
fprintf(fid, '%s\n',x);
x = '	<AudioSpectrumCentroid>';
fprintf(fid, '%s\n',x);
x = strcat('   <HopSize timeunit="', num2str(hopSize*1000), ' ms">1</HopSize>');
fprintf(fid,'%s\n',x);
x = strcat('   <Value totalSampleNum="', num2str(totalSampleNum), '">');
fprintf(fid,'%s\n',x);

SpectrumCentroid_SeriesOfVector = zeros(frameNum,10);
for i = 1:frameNum
   if i== 1
      signal = auData(1:sampleNum);
   else
      signal = auData(1+(i-1)*(sampleNum - analysisStep):i*sampleNum - (i-1)*analysisStep);
   end
   	SpectrumCentroid_SeriesOfVector(i,:)=SpectrumCentroid(signal,window,scaleRatio,freqResolution,write_flag); 
end
%% Close file 
x = strcat(' </Value>');
fprintf(fid,'%s\n',x);

x = '	</AudioSpectrumCentroid>';
fprintf(fid, '%s\n',x);
x = strcat('</AudioSegment>');
fprintf(fid, '%s \n',x);

fclose(fid);
