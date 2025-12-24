function writeclassifierXML(Y, filename)

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
    fprintf(fid, '\t<SoundModel id="ID:SoundModel:%d">\n',k);
    fprintf(fid, '\t\t<SoundClassLabel><Term id="ID:SoundClass:%s">%s</Term></SoundClassLabel>\n', ...
	Y{k}.soundName,Y{k}.soundName);

    fprintf(fid, '\t\t<Initial dim="%d %d">', 1, ...
	size(Y{k}.T,1));
    fprintf(fid, '%6.3f ', Y{k}.S);
    fprintf(fid, '</Initial>\n');
    
    fprintf(fid, '\t\t<Transitions dim="%d %d">\n', size(Y{k}.T,1), ...
	size(Y{k}.T,1));

    for l=1:size(Y{k}.T,1)
	fprintf(fid, '\t\t\t');
	fprintf(fid, '%6.3f ', Y{k}.T(l,:));
	fprintf(fid, '\n');
    end
    fprintf(fid, '\t\t</Transitions>\n');

    fprintf(fid, '\t\t<DescriptorModel>\n');
    fprintf(fid, '\t\t\t<Descriptor xsi:type="mpeg7:AudioSpectrumProjectionType"/>\n');
    fprintf(fid, '\t\t\t<Field>SeriesOfVector</Field>\n');
    fprintf(fid, '\t\t</DescriptorModel>\n');

    for m=1:size(Y{k}.S,2)
	fprintf(fid, '\t\t<State>\n');
	fprintf(fid, '\t\t\t<Label><Term id="ID:ModelState:%d">State%d</Term></Label>\n',m,m);
	fprintf(fid, '\t\t</State>\n');
    
	fprintf(fid, '\t\t<ObservationDistribution xsi:type="mpeg7:GaussianDistributionType">\n');
	fprintf(fid, '\t\t\t<Mean dim="1 %d">', size(Y{k}.M,2));
	fprintf(fid, '%6.3f ', Y{k}.M(m,:) );
	fprintf(fid, '</Mean>\n');
   
	fprintf(fid, '\t\t\t<CovarianceInverse dim="%d %d">\n', size(Y{k}.C(:,:,m),1), ...
	    size(Y{k}.C(:,:,m),2));
	C = Y{k}.C(:,:,m);
	for l=1:size(C,1)
	    fprintf(fid, '\t\t\t\t'); 
	    fprintf(fid, '%6.3f ', C(l,:)); % inv(K)
	    fprintf(fid, '\n');
	end
	fprintf(fid, '\t\t\t</CovarianceInverse>\n');
	fprintf(fid, '\t\t\t<Determinant>\n');	
	fprintf(fid, '\t\t\t\t%f\n', det(inv(Y{k}.C(:,:,m)))); % determinant of K
	fprintf(fid, '\t\t\t</Determinant>\n');		
	fprintf(fid, '\t\t</ObservationDistribution>\n');
    end
    
       
    fprintf(fid, '\t\t<SpectrumBasis loEdge="62.5" hiEdge="8000" octaveResolution="1/4">\n');
    fprintf(fid, ...
	'\t\t\t<SeriesOfVector hopSize="PT10N1000F" totalNumOfSamples="1" vectorSize="%d %d">\n', ...
	size(Y{k}.V,1), size(Y{k}.V,2));
    fprintf(fid, '\t\t\t\t<Raw mpeg7:dim="%d %d">\n', size(Y{k}.V,1),size(Y{k}.V,2));

    for l=1:size(Y{k}.V,1)
	fprintf(fid,'\t\t\t\t\t');
	fprintf(fid,'%6.3f ', Y{k}.V(l,:));
	fprintf(fid,'\n');
    end
    
    fprintf(fid, '\t\t\t\t</Raw>\n');
    fprintf(fid, '\t\t\t</SeriesOfVector>\n');
    fprintf(fid, '\t\t</SpectrumBasis>\n');
    fprintf(fid, '\t</SoundModelClass>\n',k);
end
fprintf(fid, '</AudioDescriptionScheme>\n');