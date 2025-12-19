[audata,Fs,Bits]=auread('aa.au');
s=audata(1:512);
y3=xcorr(s(1:256));
maxy3=max(y3);
y3_norm=y3(256:511)/maxy3;

position =[];
max_y3=[];
len = length(y3_norm);

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
  if ( R(j) - max_y3(i) < 0.001)
   max_selected = [max_selected, max_y3(i)];
   pos_selected = [pos_selected,j];
 end
end
max_selected
pitch = pos_selected(1)-1
freqfundamental = 1/pitch*samplingRate;
if freqfundamental < 25 | freqfundamental > 4000
   weight = 0;
else
   weight = 1;
end


  













