function [FundFreq, weight] = autocorrPitch(signal,samplingRate,window,loLimit,hiLimit)
%% File: autocorrPitch.m
%%
%% The fundamental frequency is the frequency that best explains 
%% the periodicity of a signal. While numerous methods have been 
%% proposed in order to estimate it, one can simply compute the 
%% local normalized auto-correlation function of the signal and 
%% take its first maximum in order to estimate the local 
%% fundamental period. The local fundamental frequency is then 
%% estimated by the inverse of the time corresponding to the 
%% position of this maximum.
%%
%% Copyright (c), IME, All Rights Reserved
%%
%% Author: Dong Yan Huang
%% Version: 1.0  Time: 28 October 2000 (N3489)
%% Last Modified: 27 Mar 2001 (N3704)

sampleNum = length(signal);
sn_win = signal.*window;
%sn_win = s;
sn_autocorr=xcorr(sn_win);
y3 = sn_autocorr(sampleNum:length(sn_autocorr)); 
maxy3=max(y3);
y3_norm=y3/maxy3;
len = length(y3_norm);
%weight =[];
%pitch =[];
position =[];
max_y3=[];
for i=2:len-1
  if (y3_norm(i) - y3_norm(i-1) >0 & y3_norm(i) - y3_norm(i+1) > 0)
   max_y3 = [max_y3, y3_norm(i)];
   position=[position, i];
 end
end
R=1-(1:len)/len;
max_len = length(max_y3);
max_selected = [];
pos_selected = [];
for i=1:max_len
   j=position(i);
  if ( R(j) - max_y3(i) < 0.2) %80%
   max_selected = [max_selected, max_y3(i)];
   pos_selected = [pos_selected,j];
 end
end
%max_selected
pitch1 = pos_selected(1)-1;

len1 = length(pos_selected);
j = 1;
pitch = pitch1;
for i = 2:len1-1
   x = pos_selected(i+1)-pos_selected(i);
   if abs(x - pitch1) <= 2
      pitch = pitch + x;
      j = j+1;
   end
end
pitch = pitch/j;
FundFreq = 1/pitch*samplingRate;
if FundFreq < loLimit | FundFreq > hiLimit
   weight = 0;
else
   weight = 1;
end













