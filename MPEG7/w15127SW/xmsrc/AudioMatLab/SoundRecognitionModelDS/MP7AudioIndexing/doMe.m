function doMe(DIR, ModelName)

%doMe - does all MPEG-7 stuff for a directory
%
% doMe(DIR, ModelName)
%
% Train Hidden Markov Models for sub-directories
% Index all WAV files in subdirectories

Nstates=10;
Ncomps=10;

Y=TrainSoundModels(DIR, Nstates, Ncomps, ModelName);
IndexIndividualModels(DIR, Y);
