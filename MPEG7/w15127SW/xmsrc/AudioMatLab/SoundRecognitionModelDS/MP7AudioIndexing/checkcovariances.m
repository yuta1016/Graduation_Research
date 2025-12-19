function checkcovariances(Y,PLOT_FLAG)

% CHECKCOVARIANCES - test covariance matrices

if nargin<2
    PLOT_FLAG=0;
end

[c,d,e] = size(Y.C);

if(PLOT_FLAG)
    figure;
else
    fprintf('\n');
end

for k=1:e
    COVK = Y.C(:,:,k);
    if (PLOT_FLAG)
	subplot(e, 2, 2*k-1);	    
	imagesc(COVK)
	title(['I-Det=' num2str(det(COVK))])
	subplot(e, 2, 2*k);
	imagesc(inv(COVK))
	title(['Det=' num2str(det(inv(COVK)))])
    else
	fprintf('Det=%f, IDet=%f\n', det(inv(COVK)), ...
	    det(COVK));
    end
end

if(PLOT_FLAG)
    drawnow;
else
    fprintf('\n');
end
