function [Y,Z] = AudioFundamentalFrequency(filename, params)

%AudioFundamentalFrequency - f0 extraction
%
% filename  - WAV file to extract
%   params  - list of parameters in {'parameter',value} format
%
% outputs: [Y,Z], Y=f0 track. Z=Autocorrelation values.
%
% Default Parameters:
%
% 'LoEdge', 62.5
% 'HiEdge', 1500
% 'HopSize', .050
% 'WinLen', .1
% 'BPO', 16
%
% Copyright (C) 2001 Michael A. Casey All Rights Reserved


% Read the sound file
[x,SR] = wavread(filename);
[Samps,Channels] = size(x);
L=length(params);
params(L+1)={'SR'};
params(L+2)={SR};

if(Samps==1&x==-1)
    error(['Could not open file ' filename]);
end
    
method='AudioFundamentalFrequency');
LLD_NAMES=strvcat('AudioFundamentalFrequency');
LLD_ID = strmatch(method,LLD_NAMES);

if(isempty(LLD_ID))
    error(['No match for method: ' method]);
else
    LLD_ID=LLD_ID(1); % Select first match    
end

fprintf('Extracting %s...\n', LLD_NAMES(LLD_ID,:));

% Parse and execute correct LLD
if(LLD_ID==1)
    [Y,Z]=AudioFundamentalFrequency(x, params);
else
    error('There is a problem with LLD_ID...\n');
end

function [Y,Z] = AudioFundamentalFrequency(x, params)

[xlength,c]=size(x);
if(c>1)
    x=sum(x');
end

SR = paramSelect(params, 'SR', 16000)
LoEdge = paramSelect(params, 'LoEdge', 62.5)
HiEdge = paramSelect(params, 'HiEdge', 1500)
HopSize = paramSelect(params, 'HopSize', .050)
WinLen = paramSelect(params, 'WinLen', .1)
BPO = paramSelect(params,'BPO',16)

FreqSteps = fix(BPO*(log2(HiEdge)-log2(LoEdge)))
Freqs = LoEdge*2.^([0:FreqSteps-1]/BPO);
FrameLength=fix(SR*WinLen)
HopLength=fix(SR*HopSize)
NumFrames=fix(size(x,1)/HopLength);
if(rem(size(x,1),HopLength))
    NumFrames=NumFrames+1;
end
NumFrames
ConvLength = 2*FrameLength+1
NumPeriods=4

% Initialize output vector and comb filter matrices
Y = zeros(1,NumFrames);
Z = zeros(NumFrames,FreqSteps);

% Construct lags
k=0;
for F=Freqs
    k=k+1;
    Period=1/F;
    Delay(k)=round(Period*SR);
end
Freqs2=SR./Delay; % Calculate rounded frequencies

startIndx=1;
for k=1:NumFrames    
    fprintf('Extracting Frame %d[n=%d]...   \r', k, startIndx-1);
    endIndx=startIndx+FrameLength-1;
    if(endIndx>xlength)
	endIndx=xlength;
    end
    Tindx=startIndx:endIndx;
    % Comb filter product
    % Z(k,:) =  x(Tindx)'*C(1:length(Tindx),:);
    
    % Summary autocorrelation lag 
    for l=1:FreqSteps
	Z(k,l) =  [x(Tindx)' zeros(1,Delay(l))]*[zeros(1,Delay(l)) x(Tindx)']';
    end

    [m,i]=max(Z(k,:));
    Y(k) = Freqs2(i);
    startIndx=startIndx+HopLength;
end

fprintf('\ndone.\n');

function Val=paramSelect(params,strVal,defVal)

L=length(params);

paramIndx=find(strncmp(strVal,params,length(strVal)));
if(isempty(paramIndx))
    Val=defVal;
elseif(paramIndx(1)==L|isstr({params(paramIndx(1)+1)}))
    error('paramSelect: badly formed parameter cell array');
else
    V=params(paramIndx(1)+1);
    Val=V{1};
end


