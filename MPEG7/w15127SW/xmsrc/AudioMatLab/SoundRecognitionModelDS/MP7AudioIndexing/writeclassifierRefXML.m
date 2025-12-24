function writeclassifierXMLRef(Y, filename)

% WRITE CLASSIFIER XML
%
% writeclassifierXML(Y, filename)
%
% MPEG-7 XML version of writeclassifier

[nClasses, nModelTypes] = size(Y);

fid = fopen(filename, 'wt');

if(fid<0)
    error('Could mp7 not open file');
end

fprintf(fid, '<AudioDescriptionScheme xsi:type="SoundClassificationModelType">\n');
for k=1:nClasses
    fprintf(fid, '\t<SoundModel SoundModelRef="ID%s"/>\n',Y{k}.soundName);
end
fprintf(fid, '</AudioDescriptionScheme>\n');
