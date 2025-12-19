function writeclassifier(Y, filename)
% WRITE CLASSIFIER
%
% writeclassifier(Y, filename)

[nClasses, nModelTypes] = size(Y);

% Make little-endian for WINDOZE
fid = fopen(filename, 'w', 'l');

if(fid<0)
    error('Could mp7 not open file');
end


% Define TOKENS
ClassifierBegin=1000; 
nStatesBegin=1001;
nBasisBegin=1002;
nDimBegin=1003;
ClassStrBegin=1004;
iDetBegin=1005;
nTicksBegin=1006;
nClassesBegin=1007;
		
HMMBegin=1011; 
MeansBegin=1012;
iCovBegin=1013;
StartsBegin=1014;
TransBegin=1015;
BasisBegin=1016;

ClassifierEnd=2000; 
nStatesEnd=2001;
nBasisEnd=2002;
nDimEnd=2003;
ClassStrEnd=2004;
iDetEnd=2005;
nTicksEnd=2006;
nClassesEnd=2007;

HMMEnd=2011; 
MeansEnd=2012;
iCovEnd=2013;
StartsEnd=2014;
TransEnd=2015;
BasisEnd=2016;

writetoken(fid, ClassifierBegin);

writetoken(fid, nClassesBegin);
writeint(fid, nClasses);
writetoken(fid, nClassesEnd);

for k=1:nClasses
    writetoken(fid, HMMBegin);

    % nStates
    writetoken(fid, nStatesBegin);
    writeint(fid, size(Y{k}.T,1));
    writetoken(fid, nStatesEnd);
    
    % nBasis
    writetoken(fid, nBasisBegin);
    writeint(fid, size(Y{k}.V,2));
    writetoken(fid, nBasisEnd);
    
    % nDim
    writetoken(fid, nDimBegin);
    writeint(fid, size(Y{k}.V,1));
    writetoken(fid, nDimEnd);
    
    % nTicks
    writetoken(fid, nTicksBegin);
    writeint(fid, mean(Y{k}.numFrames));
    writetoken(fid, nTicksEnd);

    % classStr
    writetoken(fid, ClassStrBegin);
    writestr(fid, Y{k}.soundName);
    writetoken(fid, ClassStrEnd);

    % M, C
    for j=1:size(Y{k}.T,1)
	% Means j
	writetoken(fid, MeansBegin);	
	writedouble(fid, Y{k}.M(j,:) );
	writetoken(fid, MeansEnd);	

	% Covariance j
	writetoken(fid, iCovBegin);	
	writedouble(fid, Y{k}.C(:,:,j) );
	writetoken(fid, iCovEnd);

	% iDet j
	writetoken(fid, iDetBegin);
	writedouble(fid, det(Y{k}.C(:,:,j)) );
	writetoken(fid, iDetEnd);
	
    end

    % S
    writetoken(fid, StartsBegin);
    writedouble(fid, Y{k}.S);
    writetoken(fid, StartsEnd);    
 
    % T
    writetoken(fid, TransBegin);    
    writedouble(fid, Y{k}.T);
    writetoken(fid, TransEnd);    
    
    % V
    writetoken(fid, BasisBegin);    
    writedouble(fid, Y{k}.V);
    writetoken(fid, BasisEnd);        
    
    writetoken(fid, HMMEnd);
end
writetoken(fid, ClassifierEnd);

    
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
