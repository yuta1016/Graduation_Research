function writeobsXML(X, filename);

%WRITEOBSXML - write XML row-wise observations to a file

fid = fopen(filename, 'wt');
if(fid<0)
    error('cannot open file');
end

[a,b]=size(X);

fprintf(fid, '<AudioDescriptor xsi:type="AudioSpectrumProjection">\n');
fprintf(fid, ...
    '\t<SeriesOfVector hopSize="PT10N1000F" totalNumOfSamples="%d" vectorSize="%d">\n', ...
    a,b);
fprintf(fid, '\t\t<Raw dim="%d %d">\n', a, b);

for i = 1:a
    fprintf(fid, '\t\t\t');
    fprintf(fid, '%6.3f ', X(i,:));
    fprintf(fid, '\n');
end    
fprintf(fid, '\t\t</Raw>\n');
fprintf(fid, '\t</SeriesOfVector>\n');
fprintf(fid, '</AudioDescriptor>\n');
