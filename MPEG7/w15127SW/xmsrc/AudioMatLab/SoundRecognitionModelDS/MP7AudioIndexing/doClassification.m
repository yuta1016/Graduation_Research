function [C,CC]=doClassification(R)

% [C,CC]=doClassification(R)
%
% Generates a class confusion matrix C and a class assignment list CC
% for the across class likelihood estimates in R.
%
% Copyright 2001 Michel A. Casey, MERL, All Rights Reserved


[N_SOUNDS, N_CLASSES, N_MODELS] = size(R);

% Generate confusion matrix
fprintf('\nGenerating result matrix...\n');
for l=1:N_MODELS % model type
    for i=1:N_SOUNDS % test sound class
	clear LLM;
	for j = 1 : N_CLASSES % model class
	    LLM(j,:)=R{i,j,l}.LLM;
	end
	[vl,mx]=max(LLM);
	CC{i,l}=mx;
	for j=1:N_SOUNDS
	    C{l}(i,j)=length(find(mx==j))./length(mx);
	end
    end
end


fid=1;
fprintf(fid,'\n\t\t *** MODEL SCORES *** \n\n')
for l=1:N_MODELS
    titleStr=sprintf('Result Matrix\n');
    fprintf(fid,titleStr);
    fprintf(fid,underlinestr(titleStr));
    fprintf(fid,'\n%15s\t\t', 'Sound / Class');
    for k=1:N_SOUNDS
	fprintf(fid,' [%d]\t', k);
    end
    fprintf(fid,'\n\n');
    for k=1:N_SOUNDS
	fprintf(fid,'%15s\t[%d]\t', '?', k);
	fprintf(fid,'%5.3f\t',C{l}(k,:));
	fprintf(fid,'\n');
    end
    fprintf(fid,'\n');
    fprintf(fid,'Recognition rate = %5.3f\n', 100*trace(C{l})./size(C{l},1) );
    fprintf(fid,'\n\n');
    figure;
    imagesc(C{1});
    title('Confusion Statistics for across-class likelihoods');
    linscale;
    thesisify;
end


function s=underlinestr(str,chr)

% = 61, - 45

l=length(str);
s=char(ones(1,l)*61);
