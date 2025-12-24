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