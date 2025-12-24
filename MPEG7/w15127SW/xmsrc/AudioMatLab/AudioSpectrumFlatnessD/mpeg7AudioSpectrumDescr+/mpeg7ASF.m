function AudioSpectrumFlatness = mpeg7ASF(fftout,standvar,loedge,hiedge)

% This function describes the spectral flatness measure of the audio signal
% The frequency range is divided into numbands logarithmically spaced bands
% of 1/4 octave width

% Written by Melanie Jackson & Juergen Herre
% Version 2.1 12 October 2001


% If loedge or hiedge is not specified (left blank or []) they will be assigned default values
% It is assumed that hiedge will be a loedge*2^m where m is a natural number.
if ~exist('hiedge')	% variable undefined
   hiedge = 16000;
elseif ~hiedge			% variable defined but no value
   hiedge = 16000;
end

if ~exist('loedge')	% variable undefined
   loedge = 250;
elseif ~loedge			% variable defined but no value
   loedge = 250;
end

fs = standvar.fs;
N = standvar.FFTsize;
num_frames = size(fftout,2);
t_overlap = round(standvar.windowsize/standvar.hopsize);
f_overlap = 0.05;
for k = 1:10000

	f_lo_nom = loedge * (2^((k-1)/4));
	f_hi_nom = loedge * (2^((k  )/4));
	f_lo = loedge * (2^((k-1)/4)) * (1-f_overlap);
	f_hi = loedge * (2^((k  )/4)) * (1+f_overlap);
	i_lo = round( f_lo/(fs/N) ) + 1;
	i_hi = round( f_hi/(fs/N) ) + 1;
    
	if (f_lo_nom >= hiedge)  break;  end
	if (f_hi > fs/2)  break;  end
	
	% Rounding of upper index according due to coefficient grouping
	if (f_lo_nom >= 1000)
        grpsize = 2^floor( log2(f_lo_nom/500) );
		i_hi = round((i_hi-i_lo+1)/grpsize)*grpsize + i_lo-1 ;
	else
		grpsize = 1;
	end
	
	tmp = fftout(i_lo:i_hi,1:t_overlap:num_frames) .^ 2;   	% PSD, no overlap
	ncoeffs = i_hi - i_lo + 1;

	% Coefficient grouping
	if (f_lo_nom >= 1000)
		tmp2 = tmp(1:grpsize:ncoeffs,:);
		for g=2:grpsize
			tmp2 = tmp2 + tmp(g:grpsize:ncoeffs,:) ;
		end
		tmp = tmp2;
	end

	% Actual calculation
	ncoeffs = ncoeffs/grpsize ;
	tmp = tmp + 1e-50;						% avoid underflow for zero signals
	gm(k,:) = exp( sum(log(tmp))/ncoeffs );	% log processing avoids overflow
	am(k,:) = sum(tmp) / ncoeffs;
	
end

AudioSpectrumFlatness = gm./am;
