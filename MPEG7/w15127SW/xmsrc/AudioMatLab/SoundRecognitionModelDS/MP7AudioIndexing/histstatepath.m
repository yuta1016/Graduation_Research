function [Q,H]=histstatepath(R, Class1, Nstates, SFXnames, Qsound, ...
    PLAY_AUDIO, SAVE_IT)

% QUERY BY EXAMPLE - ranked by order of statePathHistogram SSE
% with AUDIO results
%
% histstatepath(R, Class1, Nstates, [SFXnames, Qsound, PLAY_AUDIO, SAVE_RESULTS])
%
% Copyright (C) 2000, Michael A. Casey , MERL, All Rights Reserved

MAX_HITS=5;


if nargin<7
	SAVE_IT=0;	
end

if nargin < 6
    PLAY_AUDIO=0;
end

if nargin<5
    Qsound=1;
end

if nargin<4
    SFXnames=[];
end

if nargin < 3
    error('R, Class, Nstates required');
end

% Legacy stuff that should be removed by now!
Class2=Class1;

figure
[FilePath,FileName] = fileparts(SFXnames{Class1}{Qsound});

fprintf('%d-best matches for QUERY %s\n\n', MAX_HITS, FileName);
numSounds=length(R{Class1,Class2}.Path);

H=zeros(numSounds, Nstates);
Q=zeros(1, numSounds);
H(Qsound,:)=hist(R{Class1,Class2}.Path{Qsound},1:Nstates);
H(Qsound,:)=H(Qsound,:)./sum(H(Qsound,:));
for k=1:numSounds
    if(k~=Qsound)
	H(k,:)=hist(R{Class1,Class2}.Path{k},1:Nstates);
	H(k,:)=H(k,:)./sum(H(k,:));
	Q(k) = sum((H(Qsound,:)-H(k,:)).^2);
    end
end

[Dval,indx] = sort(Q);

% THE QUERY
subplot(1,2,1);
stem(R{Class1,Class2}.Path{indx(1)});
ax=axis;
axis([1 length(R{Class1,Class2}.Path{indx(1)}) 1 Nstates]);
title('State Path');
[dirdum,fname]=fileparts(SFXnames{Class1}{indx(1)});
ylabel(fname);
grid
        
subplot(1,2,2);
bar(1:Nstates,H(indx(1),:),'c');
ax = axis;
axis([ax(1) ax(2) ax(3) 1]);
title('State-Path Histogram');
grid
thesisify
if(SAVE_IT)
    print -djpeg SoundQuery
end
% THE RESULT LIST
figure
kk=0;
hold
subplot(1,1,1)
title(['Query Result List: ' fname]);
for k=indx(2:end)
    kk=kk+1;

    if(~isempty(SFXnames))
	[dirdum,fname]=fileparts(SFXnames{Class1}{k});
	fprintf('[%3d] [%10s] %f\n', kk, fname, Dval(kk+1)); 
    end
    subplot(MAX_HITS,2,2*(kk)-1);
    stem(R{Class1,Class2}.Path{k});
    ax=axis;
    axis([1 length(R{Class1,Class2}.Path{k}) 1 Nstates]);
    grid
    ylabel(fname);
    if(kk==1)
	title('State Paths');
    end
    if(kk==MAX_HITS)
	xlabel('Time Index')
    end

        
    subplot(MAX_HITS,2,2*kk);
    bar(1:Nstates,[H(indx(1),:);H(k,:)]');
    ax = axis;
    axis([ax(1) ax(2) ax(3) 1]);
    ylabel(['Dist=' num2str(Dval(kk+1))]);
    if(kk==1)
	title('State-Path Histograms');
    end
    if(kk==MAX_HITS)
	xlabel('State Index')
    end
    grid
    if kk>=MAX_HITS 
	break;
    end
end
colormap(cool);
thesisify;
if(SAVE_IT)
    print -djpeg ResultList
end

kk=0;
if(PLAY_AUDIO)
    for k=indx(1:MAX_HITS+1)
	kk=kk+1;
	fprintf('Playing %s\n', SFXnames{Class1}{k});
	[s,sr]=wavread(SFXnames{Class1}{k});
	rsound(s,sr);
	sil='                                           ';
	if(SAVE_IT)
	    [pth,outfilename,ext]=fileparts(SFXnames{Class1}{k});
	    wavwrite(.99*(s./max(abs(s))), 16000, [num2str(kk-1) '_' outfilename ext]); 
	end
	if(kk<(MAX_HITS+1))
	    fprintf('Hit a key for next sound %s \r',k, sil);
	    pause
	end
    end
end

fprintf('\nDone\n');
