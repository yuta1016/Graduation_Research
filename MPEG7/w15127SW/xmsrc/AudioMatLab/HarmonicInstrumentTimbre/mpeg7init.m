function standvar = mpeg7init(fs,hopsize,windowsize,window,FFTsize)

% This function creates a structure of the default values
% to be used throughout the descriptors

% Written by Melanie Jackson

% Version 1 15th March 2001

if nargin == 0
    Error = 'need to specify the sampling frequency: standvar = mpeg7init(fs)'
    standvar = [];
    return 
end

if ~exist('hopsize')	% variable undefined
    hopsize = floor(fs*10e-3);
elseif ~hopsize			% variable defined but no value
    hopsize = floor(fs*10e-3);
end
if ~exist('windowsize')	% variable undefined
    windowsize = 3*hopsize;
elseif ~windowsize			% variable defined but no value
    windowsize = 3*hopsize;
end
if ~exist('window')	% variable undefined
    window = hamming(windowsize);
elseif ~windowsize			% variable defined but no value
    window = hamming(windowsize);
end

if ~exist('FFTsize')	% variable undefined
   FFTsize = 2^nextpow2(windowsize);
elseif ~loedge			% variable defined but no value
   FFTsize = 2^nextpow2(windowsize);
end

standvar = struct('fs',fs,'hopsize',hopsize,'windowsize',windowsize,...
    'window',window,'FFTsize',FFTsize);
return