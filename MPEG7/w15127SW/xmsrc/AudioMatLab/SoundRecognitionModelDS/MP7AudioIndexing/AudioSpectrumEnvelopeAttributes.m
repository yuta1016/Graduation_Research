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

