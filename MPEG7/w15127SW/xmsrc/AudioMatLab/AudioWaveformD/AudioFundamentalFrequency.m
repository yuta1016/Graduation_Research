function [FundFreq_SeriesOfScalar,weight] = AudioFundamentalFrequency(auData,totalSampleNum,samplingRate,loLimit,hiLimit,framePeriod) 

%% File: AudioFundamentalFrequency.m
%% 
%% ----------------AudioFundamentalFrequencyType-------------
%% The function of this file is to describe the fundamental frequency 
%% of the audio signal.
%%
%% 
%% input:
%% -auData:                          incoming signal
%% -totalSampleNum:                  total Sample Number of signal
%% -samplingRate:                    sampling rate of the signal
%% -LoLimit:                         Lower limit of search space, in Hz
%% -HiLimit:                         Upper limit of search space, in Hz
%% output:
%% -FundamentalFreq_SeriesOfVector:series of samples of fundamental frequency 
%%                                      estimates (together with weights indicating 
%%                                      whether the estimates are reliable).
%% 
%%
%% Copyright (c), IME, All Rights Reserved
%%
%% Author: Dong Yan Huang
%% Version: 1.0  Time: 28 October 2000 (N3489)
%% Last Modified: 27 Mar 2001 (N3704)

% === Initialization  ===

hopSize = framePeriod;
analysisStep = hopSize*samplingRate;
sampleNum = fix(samplingRate*0.032);
frameNum = floor(totalSampleNum/sampleNum);

% === analysis parameters  ===

windowSize = sampleNum;
window = hamming(windowSize);


fid = fopen('AudioFundamentalFrequency.xml','w');
x = '<!-- ##################################################################### -->';
fprintf(fid, '%s\n',x);
x = '<!-- Definition of AudioFundamentalFrequencyType                           -->';
fprintf(fid, '%s\n',x);
x = '<!-- ##################################################################### -->';
fprintf(fid, '%s\n',x);
x = '<!-- <complexType name="AudioFundamentalFrequencyType"                     -->';
fprintf(fid, '%s\n',x);
x = '<!-- base="mpeg7:AudioSampledType" derivedBy="extension">                  -->';
fprintf(fid, '%s\n',x);
x = '<!-- <element name="Values" type="mpeg7:SeriesOfScalarType"                -->';
fprintf(fid, '%s\n',x);
x = '<!--                     maxOccurs="unbounded"/>                           -->';
fprintf(fid, '%s\n',x);
x = '<!-- <attribute name="loLimit" type="float" use="default" value="25"/>     -->';
fprintf(fid, '%s\n',x);
x = '<!-- <attribute name="hiLimit" type="float" use="optional"/>               -->';
fprintf(fid, '%s\n',x);
x = '<!-- </complexType>                                                        -->';
fprintf(fid, '%s\n',x);


x = '<AudioSegment idref="102">';
fprintf(fid, '%s\n',x);
x = ' <!-- MediaTime, etc. -->';
fprintf(fid, '%s\n',x);
x = '	<AudioFundamentalFrequency>';
fprintf(fid, '%s\n',x);
x = strcat('   <HopSize timeunit="', num2str(hopSize*1000), ' ms">1</HopSize>');
fprintf(fid,'%s\n',x);
x = strcat('   <loLimit = "', num2str(loLimit), '" hilimit = "',num2str(hiLimit),'">');
fprintf(fid,'%s\n',x);
%totalSampleNum
%sampleNum
%if totalSampleNum == sampleNum
%	frameNum = frameNum-1;
%end
FundFreq_SeriesOfScalar = zeros(1, frameNum);
weight = zeros(1,frameNum);

%frameNum

for i = 1:frameNum
   if i == 1
     signal = auData(1:sampleNum);
   else 
    signal = auData(1+(i-1)*(sampleNum - analysisStep):i*sampleNum - (i-1)*analysisStep);
   end
   [x, y] = autocorrPitch(signal,samplingRate,window,loLimit,hiLimit);
   FundFreq_SeriesOfVector(i) = x;
   weight(i) = y;
end

x = strcat('    <Value totalSampleNum="', num2str(totalSampleNum), '">');
fprintf(fid,'%s\n',x);

x = strcat('    <FundamentalFrequency>', num2str(FundFreq_SeriesOfVector), '</FundamentalFrequency>');
fprintf(fid,'%s\n',x);
x = strcat('    <weight>', num2str(weight), '</weight>');
fprintf(fid,'%s\n',x);

   
%% Close file 
x = strcat('    </Value>');
fprintf(fid,'%s\n',x);

x = '	</AudioFundamentalFrequency>';
fprintf(fid, '%s\n',x);
x = strcat('</AudioSegment>');
fprintf(fid, '%s \n',x);

fclose(fid);
     
     

