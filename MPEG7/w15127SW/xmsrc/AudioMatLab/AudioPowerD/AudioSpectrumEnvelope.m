function Spectrum_SeriesOfVector = AudioSpectrumEnvelope(auData,totalSampleNum,samplingRate,loEdge,hiEdge,resolution,framePeriod) 

%%% File: AudioSpectrumEnvelope.m
%%
%% ------------- AudioSpectrumEnvelopeType Description-----------------
%% The function of this subroutine is to describe the short-term power 
%% spectrum of the audio waveform as a time series of spectra with 
%% a logarithmic frequency axis within the AudioSpectrumEnvelopeType  
%% 
%% Function:
%% AudioSpectrumEnvelope: Description of the power spectrum of the audio signal. 
%%                        The spectrum consists of one coefficient representing 
%%                        power between 0Hz and loEdge, a series of coefficients 
%%                        representing power in logarithmically spaced bands between 
%%                        loEdge and hiEdge, and a coefficient representing power 
%%                        beyond hiEdge, in this order.
%% Output:
%% -Spectrum_SeriesOfVector:        Spectrum data
%% Inputs:
%% -loEdge:                Lower edge of logarithmically-spaced bands.
%% -hiEdge:                Higher edge of logarithmically-spaced bands. 
%% -resolution:            Frequency resolution of logarithmic spectrum 
%%                         (width of each spectrum band between loEdge and hiEdge).
%% -auData:                Incoimg signal.
%% -Fs:                    sampling rate of incoming signal
%% ?-Window_size: size of the analysis frame
%% ?Outputs:
%% ?-AudioSpectrumEnvelope
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
%weight_flag
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
withinBandNum = 1/resolution*8;
storageCoeffNum = 3+ withinBandNum;
% === Octave division === 
lowFreqBand = ceil(loEdge/freqResolution);
hiFreqBand = ceil(hiEdge/freqResolution);
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
scaleRatio = [];
elementNum = [];

scaleRatio = [scaleRatio lowFreqBand];
elementNum = [elementNum  1];
if resolution < 1
   k = log2(1/resolution);
   xelement = 0;
   for i =2:k+1
      xelement = xelement + scaleIndex(i);
   end
   scaleRatio = [scaleRatio 1];
   elementNum = [elementNum xelement];
   l = 1;
   for i=k+2:10
      m = 2^l;
      scaleRatio = [scaleRatio m];
      xelement = fix(scaleIndex(i)/m);
      elementNum = [elementNum xelement];
      l = l+1;
   end
elseif resolution == 1
   for i = 1:10
      scaleRatio(i) = scaleIndex(i+1) - scaleIndex(i);
   end
   else 
   step = 8/resolution;
   for i = 1:step
      xscal = 0;
      for j=1+(i-1)*resolution:i*resolution
         xscale = xcale + scaleIndex(j+1);
      end
      scaleRatio = [scaleRatio xscale];
      elementNum = [elementNum,1];
   end
end
if scaleRatio(10) == 0
      scalingRatio = scaleRatio(1:9);
      elementNum = ones(1,9);
  else
     scalingRatio = scaleRatio(1:10);
 end
  
Spectrum_SeriesOfVector = zeros(frameNum, sum(elementNum));
fid = fopen('AudioSpectrumEnvelope.xml','w');
x = '<!-- ##################################################################### -->';
fprintf(fid, '%s\n',x);
x = '<!-- Definition of audioSpectrumAttributeGrp                               -->';
fprintf(fid, '%s\n',x);
x = ' <!-- ##################################################################### -->';
fprintf(fid, '%s\n',x);
x = '<!-- <attributeGroup name="audioSpectrumAttributeGrp">                      -->';
fprintf(fid, '%s\n',x);
x = '<!-- <annotation>                                                           -->';
fprintf(fid, '%s\n',x);
x = '<!--       <documentation>Edge values are in Hertz</documentation>          -->';
fprintf(fid, '%s\n',x);
x = '<!--     </annotation>                                                      -->';
fprintf(fid, '%s\n',x);
x = '<!--        <attribute name="loEdge" type="float" use="default" value="62.5"/>-->';
fprintf(fid, '%s\n',x);
x = '<!--        <attribute name="hiEdge" type="float" use="default" value="16000"/>-->';
fprintf(fid, '%s\n',x);
x = '<!--    <attribute name="resolution">                                          -->';
fprintf(fid, '%s\n',x);
x = '<!--   <simpleType>                                                            -->';
fprintf(fid, '%s\n',x);
x = '<!-- <restriction base="string">                                               -->';
fprintf(fid, '%s\n',x);
x = '<!--              <enumeration value="1/16 octave"/>                           -->';
fprintf(fid, '%s\n',x);
x = '<!--              <enumeration value="1/8 octave"/>                           -->';
fprintf(fid, '%s\n',x);
x = '<!--              <enumeration value="1/4 octave"/>                           -->';
fprintf(fid, '%s\n',x);
x = '<!--              <enumeration value="1/2 octave"/>                           -->';
fprintf(fid, '%s\n',x);
x = '<!--              <enumeration value="1 octave"/>                           -->';
fprintf(fid, '%s\n',x);
x = '<!--              <enumeration value="2 octave"/>                           -->';
fprintf(fid, '%s\n',x);
x = '<!--              <enumeration value="4 octave"/>                           -->';
fprintf(fid, '%s\n',x);
x = '<!--              <enumeration value="8 octave"/>                           -->';
fprintf(fid, '%s\n',x);
x = '<!--               </restriction>                                           -->';
fprintf(fid, '%s\n',x);
x = '<!--    </simpleType>                                                       -->';
fprintf(fid, '%s\n',x);
x = '<!         </attribute>                                                     -->';
fprintf(fid, '%s\n',x);
x = '<!       </attributeGroup>                                                  -->';
   
x = '<!-- ##################################################################### --> ';
fprintf(fid, '%s\n',x);
x = '<!-- Definition of AudioSpectrumEnvelopeType                               -->';
fprintf(fid, '%s\n',x);
x = '<!-- ##################################################################### -->';
fprintf(fid, '%s\n',x);
x = '<!-- <complexType name="AudioSpectrumEnvelopeType">                        -->';
fprintf(fid, '%s\n',x);
x = '<!--	 <complexContent>                                                   -->';
fprintf(fid, '%s\n',x);
x = '<!-- 		<extension base="mpeg7:AudioSampledType">                        -->';
fprintf(fid, '%s\n',x);
x = '<!--		  <sequence>                                                     -->';
fprintf(fid, '%s\n',x);
x = '<!--			<element name="SeriesOfVector" type="mpeg7:SeriesOfVectorType"/>-->';
fprintf(fid, '%s\n',x);
x = '<!--       </sequence>                                                     -->';
fprintf(fid, '%s\n',x);
x = '<!-- 		<attributeGroup ref="mpeg7:audioSpectrumAttributeGrp"/>          -->';
fprintf(fid, '%s\n',x);
x = '<!--   </extension>                                                        -->';
fprintf(fid, '%s\n',x);
x = '<!--  </complexContent>                                                    -->';
fprintf(fid, '%s\n',x);
x = '<!-- </complexType>                                                        -->';                                             
fprintf(fid, '%s\n',x);
x = '<AudioSegment idref="102">';
fprintf(fid, '%s\n',x);
x = ' <!-- MediaTime, etc. -->';
fprintf(fid, '%s\n',x);
x = '	<AudioSpectrumEnvelope>';
fprintf(fid, '%s\n',x);
x = '<!-- SOVUnscaledType if scale ratio == 1 -->';
fprintf(fid, '%s\n',x);
x = '<!-- SOVFixedScaleType, If it is fixed and a power of two -->';
fprintf(fid, '%s\n',x);
x = '<!-- SOVFreeScaleType, otherwise -->';
fprintf(fid, '%s\n',x);
x = strcat('   <HopSize timeunit="', num2str(hopSize*1000), ' ms">1</HopSize>');
fprintf(fid,'%s\n',x);
x = strcat('    <loEdge="',num2str(loEdge),'" hiEdge="',num2str(hiEdge),...
   '" resolution="', num2str(resolution),'">');
fprintf(fid,'%s\n',x);
x = strcat('   <Value totalSampleNum="', num2str(totalSampleNum), '">');
fprintf(fid,'%s\n',x);
%scaleRatio
%elementNum
%frameNum
amplFft_seriesOfScalar = zeros(1, fftSize);
powerData = zeros(1,fftSize/2);
sumElement = sum(elementNum);
Spectrum_SeriesOfVector(frameNum,sumElement);

for i = 1:frameNum
   if i==1
      signal = auData(1:sampleNum);
   else
   	signal = auData(1+(i-1)*(sampleNum - analysisStep):i*sampleNum - (i-1)*analysisStep);
   end   
   amplFft_SeriesOfScalar = amplFFT(signal, window);
   
   % === calculate power, resample to log scale ===
   x = amplFft_SeriesOfScalar.^2;
   
   powerData(1:fftSize/2) = x(2:fftSize/2+1)';
   %powerData
   Spectrum_SeriesOfVector(i,:) = Mean_SeriesOfScalar(powerData, scalingRatio, elementNum, weight_flag,weight, write_flag);

end      
   
  
%% Close file 
x = strcat(' </Value>');
fprintf(fid,'%s\n',x);

x = '	</AudioSpectrumEnvelope>';
fprintf(fid, '%s\n',x);
x = strcat('</AudioSegment>');
fprintf(fid, '%s \n',x);

fclose(fid);
