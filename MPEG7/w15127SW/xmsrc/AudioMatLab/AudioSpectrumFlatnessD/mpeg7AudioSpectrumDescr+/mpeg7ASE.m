function AudioSpectrumEnvelope = mpeg7ASE(fftout,standvar,resolution,loedge,hiedge)
%
% This function determines the mpeg7ASE.- AudioSpectrumEnvelope
% 
%
%


% Written by Melanie Jackson
% Based on ISO/IEC WD 15938-4
%
% Version 1.0 15/12/2000
% Modified 18/12/2000 - Debugging
% Modified 9/1/2001 - Complete function description
% Modified 16th March 2001 - Removed common sepectrum analysis to generic function

% If loedge or hiedge is not specified (left blank or []) they will be assigned default values
% It is assumed that hiedge will be a loedge*2^m where m is a natural number.
if ~exist('hiedge')	% variable undefined
   hiedge = 16000;
elseif ~hiedge			% variable defined but no value
   hiedge = 16000;
end

if ~exist('loedge')	% variable undefined
   loedge = 62.5;
elseif ~loedge			% variable defined but no value
   loedge = 62.5;
end

AudioSpectrumEnvelope = [];

% Now determine resolution.

% Resolution will be given as a string

switch lower(resolution)
case('1/16 octave');
   res = 1/16;
case('1/8 octave');
   res = 1/8;
case('1/4 octave');
   res = 1/4;
case('1/2 octave');
   res = 1/2;
case('1 octave');
   res = 1;
case('2 octave');
   res = 2;
case('4 octave');
   res = 4;
case('8 octave');
   res = 8;
otherwise
   disp('Undefined resolution. Default to 1 octave')
   res = 1;
end


% This section maps the fft spectrum to the log freq spectrum with specified resolution
N = standvar.FFTsize;
fs = standvar.fs;
f_fft = (0:N/2)*fs/N; % The frequencies of the fft samples
DF = fs/N;

edge = [];
num_bands = floor(log2(hiedge/loedge))/res+2;  % add the band from 0-loedge and hiedge-fs/2.

edge = [0];
newedge = loedge;
for band = 1:num_bands-1
   edge = [edge newedge];
   newedge = newedge*2^res;
end
edge = [edge fs/2]; % 35 edges -> 34 bands for 1/4 octave example

e_index = 2;
f_fft_index = [];
bin_index = [];
value = [];
for f_index = 1:(N/2+1)
   if f_fft(f_index)>(edge(end-1)+DF/2)
      % outside range 
      f_fft_index = [f_fft_index; f_index];
      bin_index = [bin_index; num_bands];
      value = [value; 1];
  elseif f_fft(f_index)<(edge(2)-DF/2)
      % outside range 
      f_fft_index = [f_fft_index; f_index];
      bin_index = [bin_index; 1];
      value = [value; 1];
  elseif f_fft(f_index)<(edge(e_index)-DF/2)
      f_fft_index = [f_fft_index; f_index];
      bin_index = [bin_index; e_index-1];
      value = [value; 1];
  elseif f_fft(f_index)<=edge(e_index)
      f_fft_index = [f_fft_index; f_index; f_index];
      bin_index = [bin_index; e_index-1; e_index];
      dif = edge(e_index)-f_fft(f_index);
      prop = 1/2+dif/DF;
      if prop <0
          prop = 0;
      elseif prop>1
          prop = 1;
      end
      value = [value; prop; 1-prop];
      e_index = e_index+1;
  
  else
       %f_fft(f_index)>edge(e_index)
      % Since f_fft elements fall every DF that means that the 
      % edge must be within DF/2 to the fft sample
      % So thus needs to be proportioned as above
      f_fft_index = [f_fft_index; f_index; f_index];
      bin_index = [bin_index; e_index; e_index+1];
      
      dif = f_fft(f_index)-edge(e_index); % the difference in freq between fft point and bin edge
      prop = 1/2-dif/DF;
      if prop <0
          prop = 0;
      elseif prop>1
          prop = 1;
      end
      value = [value; prop; 1-prop]; 
      e_index = e_index+1;  
   end
end
bins = sparse(bin_index,f_fft_index,value);

   % now if the output of the fft has been converted to power values which can be added
   % then to resample to frequency scale just premultiply by the bins matrix
   % as long as the fft output is a column vector.

num_frames = size(fftout,2);
powers = fftout.^2;
for frame = 1:num_frames
   AudioSpectrumEnvelope(:,frame) = bins*powers(:,frame);
end
