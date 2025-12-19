function textwrite(S,filename)

%TEXTWRITE write cell array matrix to text file
%
% Each cell is considered a separate line

fid = fopen(filename,'w');
if(fid<0)
    error('Cannot open file');
end

numFiles=length(S);

for k=1:numFiles
    fprintf(fid, '%s\n',S{k});
end

fclose(fid);

