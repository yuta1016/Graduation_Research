function showClassifierHMMs(Y, SFXnames, d1, d2, SVD_FLAG, DIR, ...
    mp7ASEfile, MAXOBS)

if nargin < 8
    MAXOBS=2000;
end

if nargin <7
    mp7ASEfile='';
end

if nargin <6
    DIR='';
end


if nargin< 5
    SVD_FLAG=1;
end

if nargin< 4
    d2=2;
end

if nargin< 3
    d1=1;
end

figure;

for k=1:length(Y)
    
    X = makesfxdata3(SFXnames{k}, 1, 0, fullfile(DIR,Y{k}.soundName,mp7ASEfile));
    YX = [X(:,1) X(:,2:end)*Y{k}.V] * Y{k}.scale;
    
    fprintf('\n');

    subplot(2,max(1,fix(length(Y))/2),k);

    rndindx=randperm(size(YX,1));
    if(SVD_FLAG)
	% Find principal directions in the data
	fprintf('Visualization SVD...\n');
	[U,S,V] = svd(YX,0);
    
	% Project everything onto these new coordinates
	M=Y{k}.M*V;
	clear C;
	C = zeros(size(Y{k}.C));
	for j=1:size(Y{k}.C,3);
	    C(:,:,j)=inv(inv(Y{k}.C(:,:,j))*V); % Rotate covariance matrices
	end
    
	YX=YX*V;
    
	show_hmm(M,C,YX(rndindx(1:MAXOBS),:),Y{k}.T,d1,d2);
    else	
	show_hmm(Y{k}.M,Y{k}.C,YX(rndindx(1:MAXOBS),:),Y{k}.T,d1,d2);
    end

    title(Y{k}.soundName);
    thesisify;

    drawnow;
end
    
    
    