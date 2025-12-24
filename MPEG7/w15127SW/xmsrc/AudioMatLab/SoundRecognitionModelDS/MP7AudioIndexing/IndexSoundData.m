function [Indexes] = IndexSoundData(SoundList, Classifier, DBName)

%IndexSoundData - index sound data with MPEG-7 HMM classifier
%
% IndexSoundData(SoundList, Classifier, DataBaseName)
%
% SoundList   - cell array of absolute filenames of sound database.
% Classifier  - cell array of HMMs forming a classifier.
% DataBaseName - Filename for MPEG-7 database index file
%
% Copyright (C) June-August 2001 Michael A. Casey, MERL, All Rights Reserved

if(nargin ~=3)
    error('All 3 input arguments are required');
end

SR=16000;   % Default Sample rate
NumSounds=length(SoundList);
NumModels=length(Classifier);
USEENV=1;
NORMALIZE=1;

% Define TOKENS
ClassifierIndexesBegin=3000; 
ClassifierIndexesEnd=4000; 

NumModelsBegin=3001;
NumModelsEnd=4001;
NumSoundsBegin=3002;
NumSoundsEnd=4002;

fprintf('Indexing %d sounds...\n', NumSounds);

% Initialize index cell array
Indexes = cell(NumSounds, NumModels);

% Initialize Likelihood matrix
LLM = zeros(NumModels,1);

IndexName = DBName;

fid = fopen(IndexName, 'w');
if(fid<0)
    error(['Could not open ' IndexName ' for writing.']);
end

writetoken(fid, ClassifierIndexesBegin);

writetoken(fid, NumModelsBegin);
writeint(fid, NumModels);
writetoken(fid, NumModelsEnd);

writetoken(fid, NumSoundsBegin);
writeint(fid, NumSounds);
writetoken(fid, NumSoundsEnd);

% Index each sound in the list of sounds
for i=1:NumSounds 
    fprintf('\r%04d](%s) ', i, SoundList{i});
    [SoundPath, SoundName, SoundExt] = fileparts(SoundList{i});
    indx=find(SoundPath==filesep);
    
    % Generate spectrum envelope of current SoundList item
    % FIX LENGTHS TO 5 MINUTES
    [X, maxenv, numFrames] = makesfxdata3({SoundList{i}}, USEENV, 1); 

    % Perform Viterbi decode for sound {i} sounds against all models    
    for j=1:NumModels
	Indexes{i,j} = ViterbiDecode(Classifier{j}, X, numFrames, USEENV, NORMALIZE) ; 
    end

    % Convert cell array to matrix
    for j = 1:NumModels % model class
	LLM(j)=Indexes{i,j}.LLM;
    end

    % Find maximum likelihood and class
    [Likelihood, SoundClass]=max(LLM);

    % Write index file of SoundClass and StatePath
    WriteMP7IndexFile(SoundClass, Indexes{i, SoundClass}.Path{1}, fid, ...
	[SoundName SoundExt], ...
	Classifier{SoundClass}.soundName, Likelihood, size(Classifier{SoundClass}.T,1));

end

writetoken(fid, ClassifierIndexesEnd);

if(fclose(fid)<0)
    error('Could not close index file\n');
end

function WriteMP7IndexFile(Class, Path, fid, soundName, className, ...
    likelihood, NStates)

% Define sound index tokens

WavFileNameBegin=3003;
WavFileNameEnd=4003;
ClassIndexBegin=3004;
ClassIndexEnd=4004;
ClassNameBegin=3005;
ClassNameEnd=4005;
PathLengthBegin=3006;
PathLengthEnd=4006;
NStatesBegin=3007;
NStatesEnd=4007;
StatePathBegin=3008;
StatePathEnd=4008;
HistStatePathBegin=3009;
HistStatePathEnd=4009;
LikelihoodBegin=3010;
LikelihoodEnd=4010;

writetoken(fid, WavFileNameBegin);
writestr(fid, soundName);
writetoken(fid, WavFileNameEnd);

writetoken(fid, ClassIndexBegin);
writeint(fid, Class);
writetoken(fid, ClassIndexEnd);

writetoken(fid, ClassNameBegin);
writestr(fid, className);
writetoken(fid, ClassNameEnd);

writetoken(fid, PathLengthBegin);
writeint(fid, length(Path));
writetoken(fid, PathLengthEnd);

writetoken(fid, NStatesBegin);
writeint(fid, NStates);
writetoken(fid, NStatesEnd);

writetoken(fid, StatePathBegin);
writeint(fid, Path);
writetoken(fid, StatePathEnd);

H=hist(Path,1:NStates);
H=H./length(Path);

writetoken(fid, HistStatePathBegin);
writeint(fid, H);
writetoken(fid, HistStatePathEnd);

writetoken(fid, LikelihoodBegin);
writedouble(fid, likelihood);
writetoken(fid, LikelihoodEnd);


function writetoken(fid, x)
fwrite(fid, x, 'int');

function writeint(fid, x)
fwrite(fid, x, 'int');

function writedouble(fid, x)
fwrite(fid, x, 'double');

function writestr(fid, x)
fwrite(fid, x, 'char');
% Write string terminator !!!
fwrite(fid, [0], 'char');


function WriteIndexFile(Class, Path, fid, soundName, className, likelihood)

%	<!-- ##################################################################### -->
%	<!-- Definition of SoundModelStatePathType                                 -->
%	<!-- ##################################################################### -->
%	<complexType name="SoundModelStatePathType">
%		<complexContent>
%			<extension base="mpeg7:AudioDSType">
%				<sequence>
%				<element name="SoundEffectModelRef" type="uriReference"/>
%				<element name="StatePath" type="mpeg7:AudioLLDScalarType"/>
%				</sequence>
%			</extension>
%		</complexContent>
%	</complexType>

fprintf(fid, '<!--  SoundModelStatePath for FILE: %s -->\n', soundName); 
fprintf(fid, '<element name="SoundModelStatePath" type="SoundModelStatePathType"/>\n');
fprintf(fid, '<SoundModelStatePath confidence="%f">\n', log(-likelihood));
fprintf(fid, '\t<SoundModelRef>ID:%s</SoundModelRef>\n', className);
fprintf(fid, '\t<StatePath>\n');
fprintf(fid, '\t\t<SeriesOfScalar hopSize="PN10F1000">\n');
fprintf(fid, '\t\t\t<Raw>');
fprintf(fid, '%d ', Path);
fprintf(fid, '</Raw>\n');
fprintf(fid, '\t\t</SeriesOfScalar>\n');
fprintf(fid, '\t</StatePath>\n');
fprintf(fid, '</SoundModelStatePath>\n');


