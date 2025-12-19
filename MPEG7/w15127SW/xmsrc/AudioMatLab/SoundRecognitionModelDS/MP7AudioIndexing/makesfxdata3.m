function [X,maxenv,numFrames] = makesfxdata3( sfxfiles, USE_ENV,FIX_LENGTHS, mp7ASEfile)

% MAKESFXDATA3 - take a list of wav files and convert into HMM data
% 
% makesfxdata3( sfxfiles, [USE_ENV, FIX_LENGTHS, mp7ASEfile])
%
% MPEG-7 AudioSpectrumEnvelope (version 3.0) FFT implementation
%
% Use logartithmic frequency power spectrum approximation.
% Normalize each spectral slice by its L2-norm, i.e. output unit vectors.
%
% Copyright (C) Michael A. Casey, November 2000, Mitsubishi Electric Research Labs,
%                                                                  All Rights Reserved

numSfxFiles = length(sfxfiles);
numSamples=zeros(numSfxFiles, 1);
sampStart=ones(numSfxFiles, 1);
numFrames=zeros(numSfxFiles, 1);

% Psychoacoustic parameters...
BPO=4;    % Approximately critical band spacing
SR=16000; % Assume 16kHz for now
DB_FLAG=1; % Use dB amplitude spectrum
N = .010; % 10ms spectral frames

if nargin<4
    mp7ASEfile='';
    mp7exists=[];
end

if nargin < 3
    FIX_LENGTHS=0;
end
% MIN_LENGTH=1; % Restrict sound files to >=1s
if(FIX_LENGTHS)
    MAX_LENGTH=300; % Restrict sound files to <= 5min
else
    MAX_LENGTH=60000;
end


if nargin < 2
    USE_ENV = 0;
end

if(~isempty(mp7ASEfile))
    mp7exists=dir(mp7ASEfile);
end

if(size(mp7exists,1))
    fprintf('Loading MP7 ASE file...');
    load(mp7ASEfile, '-mat');
else	
    [N,W,H,Bands]=AudioSpectrumEnvelopeAttributes(SR,BPO, N);
    numChannels = length(Bands);

    for i=1:numSfxFiles
	sampleLength = wavread(sfxfiles{i},'size');
	sampStart(i)=1;
	numSamples(i) = min(sampleLength(1),MAX_LENGTH*SR);
	numFrames(i) = 1+fix((numSamples(i)-N)/H);
    end
    
    % Make AudioSpectrumEnvelope Matrix
    fprintf('AudioSpectrumEnvelope [%d frames, %d bands, dB=%d]...', ...
	sum(numFrames), numChannels, DB_FLAG);
    
    X = zeros(sum(numFrames), numChannels);
    startIndex=1;
    for i=1:numSfxFiles
	
	endIndex=startIndex+numFrames(i)-1;
	
	% Read WAV file
	x = wavread(sfxfiles{i},[sampStart(i) sampStart(i)+numSamples(i)-1]);
	
	% Extract AudioSpectrumEnvelope
	[XX,sumX,sumXX]=AudioSpectrumEnvelope(x', SR, BPO, DB_FLAG);
	
	% Assign datum to data matrix
	X(startIndex:endIndex,:) = XX';
	clear XX; clear x;
	startIndex=endIndex+1;
    end

    if(USE_ENV)
	fprintf('Env...');
	% Make observation matrix by norming the rows
	if(DB_FLAG)
	    env =  sqrt(sum(X'.*X')'); % 2-norm of log magnitude spectrum
	else
	    % env =  sqrt(sum(X'.*X')'); % 2-norm of power spectrum
	    env =  sqrt(sum(X')'); % 2-norm of magnitude spectrum
	    
	end
	
	
	maxenv =  max(abs(env));
	normenv = env/maxenv;
	
	% Make normed observations
	X = [normenv X ./ (env*ones(1,numChannels))];
    else
	maxenv=1.0;
    end
    
    if(~isempty(mp7ASEfile))
	save(mp7ASEfile, 'X', 'maxenv', 'numFrames');
    end
end
    
function [N,W,H,Bands,LoEdge,HiEdge]=AudioSpectrumEnvelopeAttributes(SR,BPO,N)

%MPEG-7 AudioSpectrumEnvelopeAttributes
%
% Get parameters of AudioSpectrumEnvelope Extraction
%
% Inputs
% SR   - sample rate of sampled sequence 
% BPO  - Spectrum Resolution in Bands Per Octave [2, 4, 8, 16 or 32]
%
% Outputs
% N,W,H,Bands
%
% Copyright (C) November 2000, Michael A. Casey, MERL, All Rights Reserved

LoEdge=62.5;
if(SR/2<16000)
    HiEdge=SR/2;
else
    HiEdge=16000;    
end

deltaBand=1/BPO;
bandRange=log2(HiEdge)-log2(LoEdge);

Bands = [0 LoEdge*2.^[0:deltaBand:bandRange] SR/2];

if(N>.005)
    WinLen  = N*3;  % 30 ms
    HopSize = N; % 10
else
    WinLen  = .03;  % 30 ms    
    HopSize = N; % 10
end
N = round(SR*WinLen);
W = N;
H = round(SR*HopSize); %

function [X,sumX,sumXX,Bands]=AudioSpectrumEnvelope(x, SR,BPO, DB_FLAG,N)

%MPEG-7 AudioSpectrumEnvelope
%
% FFT-based implementation of MPEG-7 AudioSpectrumEnvelope Extraction
%
% [X,sumX,sumXX]=AudioSpectrumEnvelope(x, SR,BPO, DB_FLAG)
%
% Inputs
% x    - a sampled sequence
% SR   - sample rate of sampled sequence 
% BPO  - Spectrum Resolution in Bands Per Octave [2, 4, 8, 16 or 32]
%
% Output coefficients correspond to  |0<=freq| |62.5<= freq <=16000| |freq<=SR/2|
%
% Copyright (C) November 2000, Michael A. Casey, MERL, All Rights Reserved

if nargin<5
    N=.010; % default=10ms;
end

if nargin<4
    DB_FLAG=0;
end

% Get AudioSpectrumEnvelopeAttribute parameters
[N,W,H,Bands,LoEdge,HiEdge]=AudioSpectrumEnvelopeAttributes(SR,BPO,N);

% Take STFT of x using window parameters
XX = abs(stft(x, N, W, H)).^2; % Column-major Power Spectrum

% Convert STFT representation to Log frequency
NumBands = length(Bands);
NumFrames=size(XX,2);
X=zeros(NumBands,NumFrames); % Pre-allocate output spectrogram
DF = SR/N;
FFTBands = [0:DF:SR/2]; % N/2+1 frequency bins to consider

indx = find(FFTBands<LoEdge);
X(1,:) = sumBands(indx,XX);

indx = find(FFTBands>=HiEdge);
X(NumBands,:) = sumBands(indx,XX);

for i=2:length(Bands)-1
    indx = find((FFTBands>=Bands(i))&(FFTBands<Bands(i)+DF));
    X(i, :) = X(i,:) + sumBands(indx, XX);
    indx = find((FFTBands>=Bands(i)+DF)&(FFTBands<Bands(i+1)));
    for j = indx
	BandWidth=Bands(i+1)-Bands(i);
	HiBandRatio = (FFTBands(j)-Bands(i))/BandWidth;
	LoBandRatio = 1 - HiBandRatio;
	X(i,:) = X(i,:) + LoBandRatio * XX(j, :);
	X(i+1,:) = X(i+1,:) + HiBandRatio * XX(j, :);
    end
end

sumX = sum(X);
sumXX = sum(XX);

if(DB_FLAG)
    X = dB(X.^.5); % Convert power to decibel scale
end

function Y=sumBands(indx, XX)

if(length(indx)>1)
    Y = sum(XX(indx,:)); % Sum of the columns in these rows
elseif(length(indx)==1)
    Y = XX(indx,:);
else
    Y = 0;
end

function d = stft(x, f, w, h)
% D = stft(X, F, W, H)                            Short-time Fourier transform.
%	Returns frames of short-term Fourier transform of x.  Each 
%	column of the result is one F-point fft; each successive frame is 
%	offset by H points until X is exhausted.  Data is hamm-windowed 
%	at W pts.
%	See also 'istft.m'.
% dpwe 1994may05.  Uses built-in 'fft'

s = size(x);
s = s(1)*s(2);
if rem(w, 2) == 0   % force window to be odd-len
  w = w + 1;
end
halflen = (w-1)/2;
halfwin = 0.5 * ( 1 + cos( pi * (0:halflen)/halflen));
win = zeros(1, f);
halff = f/2;   % midpoint of win
acthalflen = min(halff, halflen);
win((halff+1):(halff+acthalflen)) = halfwin(1:acthalflen);
win((halff+1):-1:(halff-acthalflen+2)) = halfwin(1:acthalflen);
c = 1;
% pre-allocating this array (rather than growing it dynamically)
% makes an *incredible* savings for very large transforms - 
% obviously, most of the time was spent realloc-ing the d array 
% and copying its old contents.  E.g. for stft(cs1(1:40000),128,64,32)
% on kew, it took 47 seconds with no d pre-allocation, and 
% under 5 seconds with pre-allocation!!! dpwe 1994may25
d = zeros((1+f/2),1+fix((s-f)/h));
%size(d)
for b = 0:h:(s-f)
  u = win.*x((b+1):(b+f));
%  if(b==0)
%	plot(u)
%  end
  t = fft(u);
  d(:,c) = t(1:(1+f/2)).';
  c = c+1;
end;
%size(d)

function d = dB(x)
% D = dB(X)          Convert X to dB i.e. D = 20 * log10(X).  Clips at -100 dB
ax = abs(x);
minval = 0.0000101;	% nothing smaller than -100dB returned
mm = ax < minval;
% >> 20/log(10)
% ans =
%   8.68588963806504
d = 8.68588963806504*log(ax.*(1-mm)+mm*minval);
