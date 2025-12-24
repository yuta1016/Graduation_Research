function writebasisXML(V, filename);

%WRITEBASISXML - write XML basis functions

fid = fopen(filename, 'wt');
if(fid<0)
    error('cannot open file');
end

[a,b]=size(V);

fprintf(fid, '<AudioDescriptor xsi:type="AudioSpectrumBasis "');
fprintf(fid, 'loEdge="62.5" hiEdge="8000" octaveResolution="1/4">\n');
fprintf(fid,'\t<SeriesOfVector totalNumOfSamples="1" vectorSize="%d %d">\n',a,b);
fprintf(fid, '\t\t<Raw dim="%d %d">\n', a, b);

for i = 1:a
    fprintf(fid, '\t\t\t');
    fprintf(fid, '%6.3f ', V(i,:));
    fprintf(fid, '\n');
end    
fprintf(fid, '\t\t</Raw>\n');
fprintf(fid, '\t</SeriesOfVector>\n');
fprintf(fid, '</AudioDescriptor>\n');
