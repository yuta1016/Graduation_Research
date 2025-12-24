function writestatepathXML(P,fname)

fid = fopen(fname, 'wt');
if(fid<0)
    error('Could not open file\n');
end

a=length(P);
fprintf(fid,'<AudioDescriptor xsi:type="SoundModelStatePathType">\n');
fprintf(fid,'\t<StatePath><SeriesOfScalar totalNumOfSamples="%d" hopSize="PT10N1000F">\n', ...
    a);
fprintf(fid,'\t\t');
fprintf(fid,'%d ', P);
fprintf(fid,'\n\t</SeriesOfScalar></StatePath>\n');
fprintf(fid,'\t<SoundModelRef>%s</SoundModelRef>\n', 'ID6');
fprintf(fid,'</AudioDescriptor>');
fclose(fid);