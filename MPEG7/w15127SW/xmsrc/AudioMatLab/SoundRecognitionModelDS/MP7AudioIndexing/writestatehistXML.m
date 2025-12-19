function writestatehistXML(H,fname)

fid = fopen(fname, 'wt');
if(fid<0)
    error('Could not open file\n');
end

a=length(H);
fprintf(fid,'<AudioDescriptor xsi:type="SoundModelStatePathType">\n');
for k=1:a
    fprintf(fid, '\t<StateRef>State%d</StateRef>\n',k);
    fprintf(fid, '\t<RelativeFrequency>%6.3f</RelativeFrequency>\n', H(k));    
end
fprintf(fid,'\t<SoundModelRef>%s</SoundModelRef>\n', 'ID6');
fprintf(fid,'</AudioDescriptor>');
fclose(fid);
