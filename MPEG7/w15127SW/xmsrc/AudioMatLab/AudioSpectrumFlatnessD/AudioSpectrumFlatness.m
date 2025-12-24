function AudioSpectrumFlatness = mpeg7ASF(fftout,standvar,numbands)

% This function describes the spectral flatness measure of the audio signal
% The frequency range is divided into numbands logarithmically spaced bands
% of 1/4 octave width, starting at 250Hz


% Written by Melanie Jackson & Juergen Herre
% Version 2.0 25 July 2001

minval = 250;
fs = standvar.fs;
maxval = min(6300,fs/2);
N = standvar.FFTsize;
overlap = 0.05;
for k = 1:numbands

        f_lo = minval * (2^((k-1)/4)) * (1-overlap);
        f_hi = minval * (2^((k  )/4)) * (1+overlap);
        i_lo = round( f_lo/(fs/N) ) + 1;
        i_hi = round( f_hi/(fs/N) ) + 1;
    
        if (i_hi > fs/2)  break;  end
        
        % Rounding of upper index according due to coefficient grouping
        if (k > 8)
        grpsize = 2^floor( (k-5)/4 );   
                i_hi = round((i_hi-i_lo+1)/grpsize)*grpsize + i_lo-1 ;
        else
                grpsize = 1;
        end
        
        tmp = fftout(i_lo:i_hi,:) .^ 2;         % PSD coefficients
        ncoeffs = i_hi - i_lo + 1;

        % Coefficient grouping
        if (k > 8)
                tmp2 = tmp(1:grpsize:ncoeffs,:);
                for g=2:grpsize
                        tmp2 = tmp2 + tmp(g:grpsize:ncoeffs,:) ;
                end
                tmp = tmp2;
        end

        % Actual calculation
        ncoeffs = ncoeffs/grpsize ;
        tmp = tmp + 1e-50;                                              % avoid underflow for zero
signals
        gm(k,:) = exp( sum(log(tmp))/ncoeffs ); % log processing avoids overflow
        am(k,:) = sum(tmp) / ncoeffs;
        
end

AudioSpectrumFlatness = gm./am;
