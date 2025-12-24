function makeSoundDataLists(DIR, ALT_NAME)

%makeSoundDataLists - compile training and testing data lists
%
%  makeSoundDataLists(DIR, ALT_NAME)
%
% DIR = full pathname of experiment directory
% ALT_NAME = name of experiment trial
%
% Copyright (C) 2001 Michael Casey, MERL, All Rights Reserved.

if(nargin<2)
    ALT_NAME = 'Default';
end

fprintf('%s\n',DIR);
% Get directory structures for each sound class
D = dir(DIR);k=0;
for i=1:length(D)
    if(D(i).isdir & ~(D(i).name(1) == '.'))
	k=k+1;
	soundDir(k)=D(i);
	soundName = fullfile( DIR, soundDir(k).name) ;
	crossValidateLists(soundName, ALT_NAME);
    end
end

