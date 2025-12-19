function [X,maxenv,numFrames] = AudioSpectrumEnvelopeExtract(...
	sfxfile, BPO, USE_ENV, NORMALIZE)

% AudioSpectrumEnvelopeExtract - take a list of wav files and extract
% 		AudioSpectrumEnvelope.
% 
% X=AudioSpectrumEnvelopeExtract( soundfile, NumBands)
%
% MPEG-7 AudioSpectrumEnvelope FFT implementation
%
% soundfile = filename of WAV file
% NumBands = bands per octave resolution
%
% Options:
%    AudioSpectrumEnvelopeExtract(soundfile,NumBands,USE_ENV,NORMALIZE)
%
%    USE_ENV = 1, normalize spectrum frames by power
%    USE_ENV = 2, normalize spectrum frames by amplitude
%    NORMALIZE = 1, normalize soundfile [-1:1 range].
%
% Copyright (C) Michael A. Casey, November 2000, Mitsubishi Electric Research Labs,
%                                                        All Rights Reserved

if(nargin < 2)
BPO=4;    % Approximately critical band spacing
end

[x,SR]=wavread(sfxfile,[1 1]);
DB_FLAG=1; % Use log amplitude spectrum

if nargin < 3
    USE_ENV = 0
end

if nargin<4
    NORMALIZE=0;
end

[N,W,H,Bands]=AudioSpectrumEnvelopeAttributes(SR,BPO);
numChannels = length(Bands);

    sampleLength = wavread(sfxfile,'size');
    numSamples(i) = sampleLength(1);
    numFrames(i) = 1+fix((numSamples(i)-N)/H);


% Make AudioSpectrumEnvelope Matrix
fprintf('AudioSpectrumEnvelope [%d frames, %d bands, dB=%d]...', ...
    sum(numFrames), numChannels, DB_FLAG)

X = zeros(sum(numFrames), numChannels);
startIndex=1;
    
    endIndex=startIndex+numFrames(i)-1;
    
    % Read WAV file
    x = wavread(sfxfile);

    % Extract AudioSpectrumEnvelope
    [XX,sumX,sumXX]=AudioSpectrumEnvelope(x', SR, BPO, DB_FLAG);

    % Assign datum to data matrix
    X(startIndex:endIndex,:) = XX';

% Normalize individual sequences
if(NORMALIZE==1)
    startIndex=1;
    fprintf('Normalize...');
	endIndex=startIndex+numFrames(i)-1;
	% Perform initial data scaling
	XX = X(startIndex:endIndex,:);
	mxx(i) = max(max(abs(XX))) ;
	XX = XX ./ mxx(i) ;    % Individual datum scale
	X(startIndex:endIndex,:) = XX; % Individual datum offset
end

if(USE_ENV)
    fprintf('Env...')
    % Make observation matrix by including normalized envelope
    % Normalize spectral slices by power env = sum(X')';
    if(USE_ENV==1)
	env = sum(X')'; % Norm by total power
    elseif(USE_ENV==2)
	if(DB_FLAG)
	    env = sum(X')'; % Norm by log power
	else
	    env = (1/size(X,2))*sum(X')'% Norm mean power
	end
    end
    
    maxenv =  max(abs(env));
    normenv = env/maxenv;
    
    if((USE_ENV==1)|(USE_ENV==2))
	% normalize spectral data and energy envelope
	X = X ./ (env*ones(1,numChannels));
	X = [normenv X]; 
    end	    
else
    maxenv=1.0;
end
function [X,sumX,sumXX]=AudioSpectrumEnvelope(x, SR,BPO, DB_FLAG)

%MPEG-7 AudioSpectrumEnvelope
%
% FFT-based implementation of MPEG-7 AudioSpectrumEnvelope Extraction
%
% Inputs
% x    - a sampled sequence
% SR   - sample rate of sampled sequence 
% BPO  - Spectrum Resolution in Bands Per Octave [2, 4, 8, 16 or 32]
%
% Output coefficients correspond to  |0<=freq| |62.5<= freq <=16000| |freq<=SR/2|
%
% Copyright (C) November 2000, Michael A. Casey, MERL, All Rights Reserved

% Get AudioSpectrumEnvelopeAttribute parameters
[N,W,H,Bands,LoEdge,HiEdge]=AudioSpectrumEnvelopeAttributes(SR,BPO);

if nargin<4
    DB_FLAG=0;
end

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
    X = dB(X.^0.5); % Convert power to log Magnitude
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
d = zeros((1+f/2),1+fix((s-f)/h));
for b = 0:h:(s-f)
  u = win.*x((b+1):(b+f));
  t = fft(u);
  d(:,c) = t(1:(1+f/2))';
  c = c+1;
end;
function [N,W,H,Bands,LoEdge,HiEdge]=AudioSpectrumEnvelopeAttributes(SR,BPO)

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

WinLen  = 0.030;  % 30 ms
HopSize = 0.010; % 10 ms

W = round(SR*WinLen);
H = round(SR*HopSize);
N = 2^ceil(log2(W));
