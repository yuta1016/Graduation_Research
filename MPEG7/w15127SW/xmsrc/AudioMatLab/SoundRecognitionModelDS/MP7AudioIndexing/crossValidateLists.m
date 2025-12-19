function crossValidateLists(soundDir, ALT_NAME)

% CROSSVALIDATELISTS - make cross validation training and testing set ...
%    from an ALL_TestList.txt file
%
% crossValidateData(soundDir)
%
% Makes symbolic links into Train and Test subdirectories by randomly
% selecting a 70/30 split of the available data in soundDir.
%
% Copyright (C) October 2000, Michael A. Casey, MERL, All Rights Reserved

if nargin < 2
    ALT_NAME = [];
end    

AllListName = fullfile(soundDir,'ALL_TestList.txt');
TrainListName = fullfile(soundDir,[ALT_NAME '_TrainList.txt']);
TestListName =  fullfile(soundDir,[ALT_NAME '_TestList.txt']);

All=dir(AllListName);
Trn=dir(TrainListName);
Tst=dir(TestListName);

if(isempty(All) & (isempty(Trn)|isempty(Tst)) )
   TL=dir([soundDir '/*.wav']);
   for k=1:length(TL)
      AllList{k}=fullfile(soundDir,TL(k).name);
   end
   textwrite(AllList, AllListName);
   % error(['ALL_TestList.txt does not exist for directory ' soundDir]);
end

if(isempty(Trn) | isempty(Tst))
    s = textread(AllListName, '%s\n');
    numFiles=length(s);
    p = randperm(numFiles);
    trainIndx=round(.7*numFiles); % Use 70% of data for training, 30% for testing

    trainers=p(1:trainIndx);
    testers=p(trainIndx+1:end);

    fprintf('%s #Train: %d, #Test %d\n', soundDir, length(trainers), length(testers)); 
    
    textwrite({s{trainers}}, TrainListName);
    textwrite({s{testers}}, TestListName);
end



