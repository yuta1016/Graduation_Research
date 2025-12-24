function d = dB(x)
% D = dB(X)          Convert X to dB i.e. D = 20 * log10(X).  Clips at -100 dB
ax = abs(x);
minval = 0.0000101;	% nothing smaller than -100dB returned
mm = ax < minval;
% >> 20/log(10)
% ans =
%   8.68588963806504
d = 8.68588963806504*log(ax.*(1-mm)+mm*minval);
