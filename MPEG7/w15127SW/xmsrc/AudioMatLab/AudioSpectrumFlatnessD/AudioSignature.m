function [as_mean,as_var]=AudioSignature(audiosignal,fs,decim)
% [as_mean,as_var] = AudioSignature(audiosignal,fs,decim)
% This function extracts the values of the MPEG-7 Audio AudioSignature DS
% where audiosignal contains the raw data to be analysed
% fs is the sampling frequency of this data
% decim optionally specifies the decimation factor (default: 32)
%
% s = AudioSignature(audiosignal,fs,decim) returns the data as XML in string variable s
%
% v1.0 Written 12th October 2001 by Juergen Herre

if (nargin < 3)  decim=32;  end;
if (2^round(log2(decim)) ~= decim), error('Illegal decimation factor'), end

sfm = mpeg7ASFunc(audiosignal,fs,'ASF');	% <<< still needs to set loedge/hiedge !!
as_mean = [];  as_var = [];
num_blocks = floor(size(sfm,2) / decim);
for k=1:num_blocks
	block_data = sfm(:,(k-1)*decim+1:k*decim)';
	as_mean = [as_mean mean(block_data)'];
	as_var = [as_var (std(block_data,1).^2)'];
end

if (nargout==1)		% ASCII XML output
	num_bands = size(sfm,1);
    hiedge = 250 * 2^(0.25*num_bands);

	s = sprintf('<AudioDS xsi:type="AudioSignatureType">\n');
	s = [s sprintf('  <loEdge="250"/>\n')];
	s = [s sprintf('  <hiEdge="%.5f"/>\n',hiedge)];
	s = [s sprintf('  <SeriesOfVectorBinaryType>\n')];
	s = [s sprintf('    <Value vectorSize="%.0f" totalSampleNum="%.0f">\n',num_bands,num_blocks*decim)];
	s = [s sprintf('      <Scaling ratio="%.0f" elementNum="%.0f"/>\n',decim,num_blocks)];
	s = [s sprintf('      <Mean dim="%.0f %.0f"/>\n',num_blocks,num_bands)];
	s = [s sprintf('       ')];
	s = [s sprintf(' %f',as_mean)];
	s = [s sprintf('\n      </Mean>\n')];
	s = [s sprintf('      <Variance dim="%.0f %.0f"/>\n',num_blocks,num_bands)];
	s = [s sprintf('       ')];
	s = [s sprintf(' %f',as_var)];
	s = [s sprintf('\n      </Variance>\n')];
	s = [s sprintf('    </Value>\n')];
	s = [s sprintf('  </SeriesOfVectorBinaryType>\n')];
	s = [s sprintf('</AudioDS>\n')];
	
	as_mean = s;
end
