function varargout = mpeg7ASFunc(audiosignal,fs,varargin)
% varargout = mpeg7ASFunc(audiosignal,fs,varargin)
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

% Modified 12th October 2001 by Juergen Herre
% - adapted call to 'ASF' function
% - accepts non-integer hopsize now
% - corrected num_frame calculation

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
num_frames = floor((length(audiosignal)-ivar.windowsize)/ivar.hopsize)+1;

fftout=[];
for frame = 1:num_frames	
   stframe = round((frame-1)*ivar.hopsize)+1;
   frame_data = ivar.window.*audiosignal(stframe:stframe+ivar.windowsize-1);
   % fft frame with windowing
   fftout(:,frame) = fft(frame_data,ivar.FFTsize);
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
        varargout(p) = {mpeg7ASF(fftout,ivar)};
    case 'SC ' % Determine the frames based Spectral Centroid
        varargout(p) = {mpeg7SC(fftout.^2,0,ivar)};
    otherwise
        varargout(p) = {'invalid type'};
    end
end




