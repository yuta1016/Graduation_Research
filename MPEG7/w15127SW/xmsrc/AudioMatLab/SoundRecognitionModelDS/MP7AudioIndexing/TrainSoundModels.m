function [Y,Z,R,C,CC] = TrainSoundModels(DIR, Nstates, Ncomps, ...
    ALT_NAME, USE_ENV, NORM_COLS, NORMALIZE, SCALE, T0, HMM_SIMPLE, MODELS)

% TrainSoundModels - MPEG-7 sound recognition model training
%                                                                    
% Y = TrainSoundModels(dir, Nstates, Ncomps, ClassifierName, [USE_ENV,
%                                          NORMCOLS, NORMALIZE, SCALE, TO])
%                                                                    
%   I. Train a classifier using HMM models
%  II. Test cross-model performance and generate confusion statistics
%                                                                    
% DIR - contains sub-directories of training data for each sound class:                   
%                                                                    
%                  -------DIR-------------                            
%                  |       |      |      |                           
%                class1  class2  ...   classN
%                  |       |             |                                    
%              file1.wav   
%              file2.wav 
%                 ...
%
%
% Nstates - number of initial model states for each HMM
% Ncomps  - number of basis functions to extract
% ClassifierName - Classifier Name to use for output files.
%
% Optional Arguments:
% USE_ENV   - 0=no envelope, 1=L2 norm envelope
% NORMCOLS  - 0=no basis norming, 1=norm basis columns
% NORMALIZE - 0=no data scaling, 1=scale data by max(X), 2=scale by max(X(:,j))
% SCALE     - scaling coefficient for normalizing data [10]
% T0        - Initial temparature for annealing schedule [100]
% HMM_SIMPLE- 0=Entropic Training ; 1=Baum-Welch
% MODELS: vector of models algorithms to use
%      1 = 'SVD'; [DEFAULT]
%      2 = 'SVD:jade';
%      3 = 'SVD:fastICA';
%      4 = 'fastICA';
%      5 = 'Direct Spectrum';
%
% Copyright (C) August 2000 Michael A. Casey, MERL, All Rights Reserved

error(nargchk(1,11,nargin));

if nargin < 11
    MODELS = [1]; % HMM Model Types to Train (different feature sets)
end

if nargin<10
    HMM_SIMPLE=0;
    FIX_LENGTHS = 0; 
else
    FIX_LENGTHS=HMM_SIMPLE;
end

if nargin < 9
    T0=100;
end

if nargin < 8
    SCALE = 10;
end

if nargin < 7
    NORMALIZE = 1;
end

if nargin < 6
    NORM_COLS=1;
end

if nargin < 5
    USE_ENV=1;
end

if nargin < 4
    ALT_NAME = ['SFX_' num2str(Nstates) num2str(Ncomps) ...
	    num2str(USE_ENV) num2str(NORM_COLS) num2str(NORMALIZE)];
end

if nargin<3
    Ncomps = 5;    
end

if nargin < 2
    Nstates = 5;
end

modelStr(1).name = 'SVD';
modelStr(2).name = 'SVD:jade';
modelStr(3).name = 'SVD:fastICA';
modelStr(4).name = 'fastICA';
modelStr(5).name = 'Direct Spectrum';

if(nargin<4)
    SAVE_SUFFIX = ['_' datestr(now,3) datestr(now,7) '_' ...
	datestr(now,13)] ;
else
    SAVE_SUFFIX ='';
end

TRAIN_LIST_NAME = [ALT_NAME SAVE_SUFFIX '_TrainList.txt'];

makeSoundDataLists(DIR, ALT_NAME);

% Get directory structures for each sound class
D = dir(DIR);k=0;
for i=1:length(D)
    if(D(i).isdir & ~(D(i).name(1) == '.'))
	k=k+1;
	soundDir(k)=D(i);
    end
end

fprintf('MODELING SOUNDS:\n\n');
for i=1:k
    fprintf('\t[%d] %s\n',i,soundDir(i).name);
end

fprintf('\nModel: %s \n',ALT_NAME) ;

if(k>0)
    
    % Loop over sound classes, train HMM models
    for i=1:k
	
	% Make training and testing lists (cross-validation data)
	soundName = fullfile( DIR, soundDir(i).name) ;
	fprintf('\nModeling sounds: %s\n', soundName);
	if(nargin < 4)
	    crossValidateData( soundName, [ALT_NAME SAVE_SUFFIX]) ;
	end

	% Architecture independent filename construction
	sfxfilename = fullfile( soundName, TRAIN_LIST_NAME );
	
	% Read specified WAV files
	fileList = readsfxfiles(sfxfilename);
	
	% Load or calculate spectrum envelope and sequence indices
	mp7ASEfile= fullfile(soundName,[TRAIN_LIST_NAME(1:end-4) ...
		'_ASE' num2str(USE_ENV) num2str(FIX_LENGTHS) '.mp7']);
	
	[ X, maxenv, numFrames] = makesfxdata3( fileList, ...
	    USE_ENV, FIX_LENGTHS, mp7ASEfile ) ;

	l=0;

	for j = MODELS % 0=No reduction, 1=SVD, 2=ICA[jade], 3=ICA[fast], 4=ICA[fast] only
	    l=l+1;
	    % STEP I - training
	    fprintf('\n%s HMM Train Model %d...',soundDir(i).name,j);
	    Y{i,l} = TrainHiddenMarkovModel ( X, maxenv, numFrames, Nstates, Ncomps, j, NORM_COLS, ...
		USE_ENV, NORMALIZE, SCALE, T0, HMM_SIMPLE ) ;
	    Y{i,l}.soundName = soundDir(i).name;
	    Y{i,l}.modelName = modelStr(j).name;
	    checkcovariances( Y{i,l} ) ;
	end 
	clear X;
    end

    TRAIN_PREFIX = [ALT_NAME '_TRAIN'];
    TEST_PREFIX = [ALT_NAME '_TEST'];
    if(nargin >= 4) % SAVE different versions of same ALT_NAME model
	SAVE_SUFFIX2 = ['_' datestr(now,3) datestr(now,7) '_' ...
		datestr(now,13)] ;
    else
	SAVE_SUFFIX2 = SAVE_SUFFIX;
    end

    fprintf('...saving HMM CLASSIFIER to %s...\n', [TRAIN_PREFIX SAVE_SUFFIX2]);
    save(fullfile(DIR,[TRAIN_PREFIX SAVE_SUFFIX2]), 'Y');
    writeclassifierXML(Y, fullfile(DIR,[TRAIN_PREFIX SAVE_SUFFIX2 '.mp7']));
    
    % Test SFX HMMs on cross-validation data
    [R,C,CC,SFXnames] = TestSoundModels(Y, DIR, USE_ENV, NORMALIZE, ...
	[ALT_NAME SAVE_SUFFIX], 1, 0, FIX_LENGTHS);
    
    fprintf('...saving TEST RESULTS to %s...\n', [TEST_PREFIX SAVE_SUFFIX2]);
    save(fullfile(DIR,[TEST_PREFIX SAVE_SUFFIX2]), 'R','C','CC', 'SFXnames');
else
    fprintf('\nBummer! No sound directories found\n');
end
