function varargout = mpeg7ASFunc(audiosignal,fs,varargin)

% This function will return the specified AudioSpectrum descriptors
% where audiosignal contains the raw data to be analysed
% fs is the sampling frequency of this data
% vargin contains a variable length list of abbreviations to the 
% decriptors (e.g. ASE for AudioSpectrumEnvelope, ASS for AudioSPectrumSpread etc.)
%
% The output arguments are returned in the same order as specified in varargin.
% An error will arise if there are too many or insufficient output variables.

% Written by Melanie Jackson
% Version 1 16th March 2001
% Modified 17th March - completed to a function

% assume audiosignal contains the signal to be processed
% fs is the sampling frequency
% Eventually this should be a variable selection or text input box gui

numin = size(varargin,2);
numout = nargout;
if numin~=numout
    Error = 'Incompatible number of input and output arguments'
    return
end
argin = upper(char(varargin)); % convert inputs to an array of strings
ASfound = sum(strmatch('AS',argin)); % Determine if any of the descriptors are AudioSpectrumDecriptors
SCfound = sum(strmatch('SC',argin));
if ~ASfound
    'No spectral descriptors requested'
    return
end

ivar = mpeg7init(fs);
% ensure audiosignal in column format
if size(audiosignal,2)~=1
    audiosignal = audiosignal';
end
num_frames = floor((length(audiosignal)-2*ivar.hopsize)/ivar.hopsize);

stframe = 1;
finframe = ivar.windowsize;
fftout=[];
for frame = 1:num_frames
   frame_data = ivar.window.*audiosignal(stframe:finframe);
   % fft frame with windowing
   fftout(:,frame) = fft(frame_data,ivar.FFTsize);
   stframe= stframe+ivar.hopsize; % 50% overlap
   finframe = finframe+ivar.hopsize;
end

fftout=abs(fftout(1:(ivar.FFTsize/2+1),:)); 



for p = 1:numin
    switch argin(p,:)
    case 'ASE' % Determine AudioSpectrumEnvelope
        varargout(p) = {mpeg7ASE(fftout,ivar,'1/4 octave')};
    case 'ASS' % Determine AudioSPectrumSpread
        varargout(p) = {mpeg7ASS(fftout,ivar)};
    case 'ASC' % Determine AudioSpectrumCentroid
        varargout(p) = {mpeg7ASC(fftout,ivar)};
    case 'ASF' % Determine AudioSpectrumFlatness
        varargout(p) = {mpeg7ASF(fftout,ivar,4)};
    case 'SC ' % Determine the frames based Spectral Centroid
        varargout(p) = {mpeg7SC(fftout.^2,0,ivar)};
    otherwise
        varargout(p) = {'invalid type'};
    end
end




