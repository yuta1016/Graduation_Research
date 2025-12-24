function [R,C,CC,SFXnames] = TestSoundModels(Y, DIR, USE_ENV, NORMALIZE, ALT_NAME, ...
    SHOW_RESULTS, TRAINDATA, FIX_LENGTHS, ALT_SUFFIX)

%TESTSOUNDMODELS - test individual sound sequences against HMM model
%
% [R,C,CC] = TestSoundModels(Y, DIR, USE_ENV, NORMALIZE, ALT_NAME, SHOW_RESULTS)
%
% Returns classifications, paths and likelihoods of each sound against the model
%
% Copyright (C) September 2000 Michael A. Casey, MERL, All Rights Reserved

if nargin < 3
    USE_ENV = 0;
end

if nargin < 4
    NORMALIZE = 0;
end

if nargin < 5
    ALT_NAME = [];
end

if nargin<6
    SHOW_RESULTS=1;
end

if nargin<7
    TRAINDATA=0;
end

if nargin<8
    FIX_LENGTHS=0;
end

if nargin<9
    ALT_SUFFIX='';
end

if(computer=='PCWIN')
    if(~TRAINDATA)
	FILELISTNAME = [ALT_NAME 'T.txt'];
    else
	FILELISTNAME = [ALT_NAME 'R.txt'];
    end
else
    if(~TRAINDATA)
	FILELISTNAME = [ALT_NAME '_TestList.txt'];
    else
	FILELISTNAME = [ALT_NAME '_TrainList.txt'];
    end
end

SR=16000;   % Sample rate

[N_SOUNDS,N_MODELS]=size(Y);

% Get directory structures for each sound class
% D = dir(DIR);k=0;
% for i=1:length(D)
%    if(D(i).isdir & ~(D(i).name(1) == '.'))
%	k=k+1;
%	soundDir(k)=D(i);
%    end
% end

% N_SOUNDS=k;

% SFX CE STEP III - testing
% Assume that there is one model for each sound class

fprintf('Testing %d classes...\n', N_SOUNDS) ;

% Loop through all TEST sounds and find the most probable model for each
for s=1:N_SOUNDS % sound test data classes
    % soundName = fullfile( DIR, soundDir(s).name ) ;
    soundName = fullfile( DIR, Y{s}.soundName ) ;
    fprintf('\nTesting sounds: %s ...', Y{s}.soundName);

    % Architecture independent filename construction
    fileList = fullfile( soundName, FILELISTNAME );

    % Make data matrix
    % Read specified WAV files and do frequency transform
    sfxfiles = readsfxfiles(fileList);

    % Load or calculate spectrum envelope and sequence indices
    mp7ASEfile= [fileList(1:end-4) '_ASE' num2str(USE_ENV) ...
	    num2str(FIX_LENGTHS) '.mp7'];
    [X, maxenv, numFrames] = makesfxdata3( sfxfiles, USE_ENV, ...
	FIX_LENGTHS, mp7ASEfile) ;
    fprintf('Viterbi...');
    SFXnames{s}=sfxfiles;
    for i=1:N_SOUNDS % sound model classes
	for j=1:N_MODELS % HMM model types
		R{s,i,j} = ViterbiDecode(Y{i,j}, X, numFrames, ...
		    USE_ENV, NORMALIZE) ;
	end	
    end
end

% Generate confusion matrix
fprintf('\nGenerating result matrix...\n');
for l=1:N_MODELS % model type
    for i=1:N_SOUNDS % test sound class
	clear LLM;
	for j = 1 : N_SOUNDS % model class
	    LLM(j,:)=R{i,j,l}.LLM;
	end
	[vl,mx]=max(LLM);
	CC{i,l}=mx;
	for j=1:N_SOUNDS
	    C{l}(i,j)=length(find(mx==j))./length(mx);
	end
    end
end

if(SHOW_RESULTS)
    if(computer=='PCWIN')
	outFile=fullfile(DIR, [ALT_NAME ALT_SUFFUX '.txt']);
	fid=fopen(outFile,'w') ;
    else
	outFile=fullfile(DIR, [ALT_NAME ALT_SUFFIX '.txt']);
	fid=fopen(outFile,'w') ;
    end
    if(~fid)
	fprintf('Couldn''t open result file: %s\n',outFile);
    else

	fprintf(fid,'\n\t\t *** MODEL SCORES *** \n\n')
	for l=1:N_MODELS
	    titleStr=sprintf('Result matrix for %s\n', Y{1,l}.modelName);
	    fprintf(fid,titleStr);
	    fprintf(fid,underlinestr(titleStr));
	    fprintf(fid,'\n%15s\t\t', 'Sound / Class');
	    for k=1:N_SOUNDS
		fprintf(fid,' [%d]\t', k);
	    end
	    fprintf(fid,'\n\n');
	    for k=1:N_SOUNDS
		fprintf(fid,'%15s\t[%d]\t', Y{k}.soundName, k);
		fprintf(fid,'%5.3f\t',C{l}(k,:));
		fprintf(fid,'\n');
	    end
	    fprintf(fid,'\n');
	    fprintf(fid,'Recognition rate = %5.3f\n', 100*trace(C{l})./size(C{l},1) );
	    fprintf(fid,'\n\n');
	end
	fclose(fid);
	fprintf('Results written to file: %s\n\n', outFile);
	type(outFile);
    end
end

function s=underlinestr(str,chr)

% = 61, - 45

l=length(str);
s=char(ones(1,l)*61);
