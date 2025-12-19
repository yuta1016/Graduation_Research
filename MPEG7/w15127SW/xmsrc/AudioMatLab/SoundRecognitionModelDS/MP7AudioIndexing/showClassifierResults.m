function showClassifierResults(DIR, MODEL_NAME, ALT_SUFFIX, MICROSOFT);
%
% showClassifierResutls(DIR, MODEL_NAME, ALT_SUFFIX)
%
% Set ALT_SUFFIX to '*' to write results for all
% versions of MODEL_NAME in DIR

if nargin<4
    MICROSOFT=0;
end
if nargin<3
    ALT_SUFFIX='';
end

if nargin<2
    error('need directory and model name');
end

if (strncmp(ALT_SUFFIX,'*',1))
    LOOP_ME=1;
else
    LOOP_ME=0;
end

TrainModelSearch=fullfile(DIR, [MODEL_NAME '_TRAIN_' ALT_SUFFIX '.mat']);
TrainList = dir(TrainModelSearch);
for z=1:size(TrainList,1)
    TrainModelName=fullfile(DIR,TrainList(z).name);

    q1=findstr(TrainModelName, '_TRAIN_');
    D=dir(TrainModelName);
    if(~length(D))
	error(['Could not find specified file(s):' TrainModelName ' in directory']);
    end
    ALT_SUFFIX=TrainModelName(q1(1)+length('_TRAIN_'):end-4);
    TestModelName=[TrainModelName(1:q1(1)-1) '_TEST_' ALT_SUFFIX '.mat'];
    D=dir(TestModelName);
    if(~length(D))
	error(['Could not find specified file(s):' TestModelName ...
		' in directory']);
    end
    
    % OK, we've checked everything, now load all the variables
    load(TrainModelName);
    load(TestModelName);
    
    N_SOUNDS=size(Y,1);
    N_MODELS=size(Y,2);

    if(computer=='PCWIN')
	outFile=fullfile(DIR, [MODEL_NAME '_' ALT_SUFFUX '.txt']);
	fid=fopen(outFile,'w') ;
    else
	outFile=fullfile(DIR, [MODEL_NAME '_' ALT_SUFFIX '.txt']);
	fid=fopen(outFile,'w') ;
    end
    if(~fid)
	fprintf('Couldn''t open result file: %s\n',outFile);
    else

	fprintf(fid,'\n\t\t *** MODEL SCORES *** \n\n')
	if(size(Y{1,1}.V,2)>20)
	    Y{1,1}.modelName='Direct Spectrum'
	end
	fprintf(fid, 'ModelName = %s\n', TrainModelName);

	for l=1:N_MODELS
	    fprintf(fid, 'Features Type = %s\n', Y{1,l}.modelName);
	    BWTest=1;
	    for k=1:size(Y{1}.C,3)-1
		BWTest=BWTest*all(all((Y{1}.C(:,:,k) == Y{1}.C(:,:,k+1))));
	    end
	    if(BWTest)
		TrainMethod='Conventional Baum-Welch';
	    else
		TrainMethod='Entropic Maximum A-Posteriori (MAP)';
	    end

	    fprintf(fid, '\nTraining Method = %s\n', TrainMethod);
	    fprintf(fid, 'Normalize Maxenv= %d\n', Y{1,1}.maxenv);
	    fprintf(fid, 'Normalize Scale = %d\n', Y{1,1}.scale);
	    fprintf(fid, 'Temperature T0  = %d\n', Y{1,1}.T0);

	    titleStr=sprintf('\nResult matrix for %s\n', Y{1,l}.modelName);
	    fprintf(fid,titleStr);
	    fprintf(fid,underlinestr(titleStr));
	    fprintf(fid,'\n\n%15s\t\t%s\t%s', 'Sound Class', 'Hit', ... 
		'Miss');
	    fprintf(fid,'\n\n');
	    TotalSounds=0;
	    TotalCorrect=0;
	    for k=1:N_SOUNDS
		SoundsInClass=length(CC{k,1});
		TotalSounds=TotalSounds+SoundsInClass;
		CorrectInClass=length(find(CC{k,l}==k));
		TotalCorrect=TotalCorrect+CorrectInClass;
		if(~MICROSOFT)
		    fprintf(fid, '%15s\t\t', Y{k}.soundName);
		    fprintf(fid, ...
			'%3d\t%3d\n',CorrectInClass,...
			SoundsInClass-CorrectInClass);
		else
		    fprintf(fid, '%s ', Y{k}.soundName);
		    fprintf(fid, '%d %d\n', CorrectInClass,...
			SoundsInClass-CorrectInClass);

		end
	    end
	    fprintf(fid,'\n');
	    fprintf(fid,'Mean Recognition=%7.2f\n', 100*TotalCorrect/TotalSounds);
	    fprintf(fid,'\n\n');
	end
	fclose(fid);
	type(outFile);
	fprintf('Results written to file: %s\n\n', outFile);
    end
end

function s=underlinestr(str,chr)

% = 61, - 45

l=length(str);
s=char(ones(1,l)*61);
