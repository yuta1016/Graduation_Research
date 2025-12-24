function IndexIndividualModels(DIR, Y);
% IndexIndividualModels(DIR, Y);

for k=1:length(Y)
    
    soundName = fullfile( DIR, Y{k}.soundName ) ;
    fprintf('\nIndexing sounds: %s ...', Y{k}.soundName);

    % Architecture independent filename construction
    listFile = fullfile(soundName, 'ALL_TestList.txt');

    soundList = textread(listFile, '%s\n');    
    IndexSoundData(soundList, {Y{k}}, ...
	fullfile(soundName,'IndexFile.mp7'));
   
end

