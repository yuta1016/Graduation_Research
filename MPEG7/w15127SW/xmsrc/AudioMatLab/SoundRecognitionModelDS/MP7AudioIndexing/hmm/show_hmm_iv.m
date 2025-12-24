function show_hmm_iv(filename,Means,iCovariances,Data,Transitions,occ,scale,rescale,colors)
%show_hmm_iv(filename,Means,iCovariances,Data,Transitions,occ,scale,rescale,colors)
% Generate Inventor code giving a 3D visualization of an Gaussian-output HMM.
%
% Means -- centers of Gaussian outputs
% iCovariances -- inverse covariances of Gaussian outputs
% Data -- data points
% Transitions -- state transition matrix
% scale -- how to scale points and arrows
% rescale -- scaling coefficients on the dimensions prior to PCA
% filename OR fid -- output file (may be name of file or file ID; if name,
%                    then show_hmm_iv will open and close the file)
%
% Transition probabilities are shown as an arrow protruding from the center
% of each cluster.  Self-transition probabilities are depicted as a sphere
% at the center of the cluster.  The thicker the line, the higher the
% conditional probability.  Covariances are shown as transparent 2std
% ellipsoids.  The data is shown as a scatterplot of dots.  If the model
% has more than 3 dimensions, it will be reduced to 3 via PCA.  You may
% rescale the dimensions prior to PCA to emphasize dimensions with smaller
% amounts of variation.

  if (size(filename,2) > 1)
    [fid,message]=fopen(filename,'wt');
    
    if (fid < 0)
      error(['unable to open file ' filename ': ' message]);
    end
    
    ownfile = 1;
  else
    fid = filename;
    ownfile = 0;
  end

if nargin<4, Data = []; end;
if nargin<5, Transitions=[]; end;
if nargin<6, occ=[];end;
if nargin<7, scale = 1; end;
if nargin<8, rescale = []; end;
if nargin<9, colors = []; end;
if isempty(fid), fid=2;end;
if min(size(occ))>1, [junk,occ]=max(occ'); clear junk; end;
[n,d]=size(Means);
if rescale,
  if ~isempty(Data), Data=vprod(rescale,Data); end;
  if ~isempty(Means), Means=vprod(rescale,Means); end;
  if ~isempty(iCovariances), 
    rescale=1./rescale;
    for i=1:n,
      iCovariances(:,:,i)=vprod(rescale',vprod(rescale,iCovariances(:,:,i))); 
    end;
  end;
end;

if d<3, warning('show_hmm_iv: not enough dimensions'); return; end;

[a,b,c] = size(iCovariances);

if any([a b c] ~= [d d n])
  warning('show_hmm_iv: mean and icovs dimensions disagree');
  if ~isempty(Data),    return;    end
end

for i=1:n
  [v,d] = eig(iCovariances(:,:,i));
  if (any(diag(d) < 0))
    warning(['show_hmm_iv: inverse covariance has negative eigenvalues,' ...
	     ' inverting']);
    iCovariances(:,:,i) = v*abs(d)*v';
  end
end

if d>3,
  % project HMM onto 3D subspace
  if ~isempty(Data),
    if (size(Data,2) ~= d), 
      warning('show_hmm_iv: mean and data dimensions disagree');
      return;
    end
    
    [eigval,dataMean,vals]=pca(Data);
    Data=pca_encode(Data,eigval,dataMean,3);
    Means=pca_encode(Means,eigval,dataMean,3);
    newiCovs=zeros(3,3,n);
    for i=1:n,
      [c_eig,c_val]=eig(iCovariances(:,:,i));
      c_eig=real(vprod(diag(c_val)'.^(-1/2),-c_eig));
      c_eig=pca_encode(c_eig',eigval,[0 0 0 0],3);
      newiCovs(:,:,i)=inv(c_eig'*c_eig);
    end;
    iCovariances=newiCovs;
  end;
end;

if ~isempty(occ) & ~isempty(Data) & length(occ) ~= size(Data,1)
  warning('show_hmm_iv: data and occupancy dimensions do not agree');
  return
end
  

if isempty(colors), 
  st=rand('state');rand('state',50);
  if isempty(occ), q=n; else q=max(n,max(occ)); end;
  colors=(rand(q,3)).^(2/3); 
  rand('state',st);clear st;
end;

fprintf(fid,'#Inventor V2.0 ascii\nSeparator { ShapeHints { creaseAngle 2 }\nDirectionalLight { direction -0.1 -0.9 0.0}\n');

fprintf(fid,'DEF COLOR%04d Material { ambientColor %.4g %.4g %.4g diffuseColor %.4g %.4g %.4g }\n',[(1:size(colors,1))',colors,colors]');
fprintf(fid,'DEF TCOLOR%04d Material { ambientColor %.4g %.4g %.4g diffuseColor %.4g %.4g %.4g transparency 0.6}\n',[(1:size(colors,1))',colors,colors]');

if ~isempty(Data),
  fprintf(fid,'Separator { DrawStyle { pointSize 3 }\n');
  if isempty(occ),
    fprintf(fid,'Material {\n ambientColor 0.7 0.4 0.8\n diffuseColor 0.4 0.7 0.8 transparency 0 }\nCoordinate3 { point [\n');
    fprintf(fid,'%.4g %.4g %.4g,\n', Data'); 
    fprintf(fid,'] } PointSet { numPoints -1 startIndex 0 }');
  else
    for i=1:max(occ);
      pts=Data(find(occ==i),:);
      if ~isempty(pts),
	fprintf(fid,'USE COLOR%04d\nCoordinate3 { point [\n', i);
	fprintf(fid,'%.4g %.4g %.4g,\n', pts'); 
	fprintf(fid,'] } PointSet { numPoints -1 startIndex 0 }');
      end;
    end;
  end;
  fprintf(fid,' }\n');
end;

first=1;
if ~isempty(Means),
  %cla
  if ~isempty(Transitions),
    fprintf(fid,'Separator {\nMaterial {\n ambientColor 0.7 0.8 0.4\n diffuseColor 0.4 0.8 0.7 transparency 0\n}\n');
    for i=1:n,
      from = Means(i,1:3);
      for j=1:n,
	to = mean([from;Means(j, 1:3)]);
	width = Transitions(i,j).*scale;
	if width>0, 
	  fprintf(fid,'USE COLOR%04d\n', j);
	  first=arrow(fid,from,to,width,first); 
	end;
      end;
    end;
    fprintf(fid,'}\n');
  end;
  if ~isempty(iCovariances),
    fprintf(fid,'Separator {\nMaterial {\n ambientColor 0.4 0.7 0.8\n diffuseColor 0.4 0.7 0.8 transparency 0.6\n}\n');
    for i = 1:n,
      fprintf(fid,'USE TCOLOR%04d\n', i);
      ellipse(fid,Means(i,1:3),iCovariances(1:3,1:3,i),i);
    end;
    fprintf(fid,'}\n');
  end;
end;

fprintf(fid,'}\n');

if ownfile
  fclose(fid);
end

function ellipse(fid,mn,icv,num)
[v,d]=eig((icv));
d=real(diag(d))';
rot = [[real(vprod(1./sqrt(d),-(v))),[0;0;0]];[0,0,0,1]];
%rot = [[real(vprod(sqrt(d),-v)),[0;0;0]];[0,0,0,1]];
fprintf(fid,'Separator { Transform { translation %.4g %.4g %.4g }\n MatrixTransform { matrix ',mn);
fprintf(fid,' %.4g %.4g %.4g %.4g\n', rot);
%fprintf(fid,'} Transform { scaleFactor %.4g %.4g %.4g } Sphere { radius 1 } Transform { translation 0.3 0.3 0.3 } USE COLOR%04d Text2 { string "%d" } }\n',1./d,num,num);
fprintf(fid,'} Sphere { radius 1 } Transform { translation 0.3 0.3 0.3 } USE COLOR%04d Text2 { string "%d" } }\n',num,num);

function first=arrow(fid,from,to,width,first)
if nargin<5, first=1; end;
d=to-from;
n=norm(d);
if n>0.0,
  fprintf(fid,['Separator { Transform { translation %.4g %.4g %.4g ',...
	'scaleFactor %.4g %.4g %.4g rotation %.4g %.4g %.4g %.4g }\n '],...
      from,[width,norm(d)./2.5,width],cross([0 1 0],d),acos(d(2)./norm(d)));
  if first,
    first=0;
    fprintf(fid,['DEF ARROW Group { Transform { translation 0 1 0 } ',...
	'Cylinder { radius 1 }\nTransform { translation 0 1.25 0 ',...
	'scaleFactor 1.5 0.25 1.5 } Cone { bottomRadius 1 }\n }\n']);
    else
      fprintf(fid,'USE ARROW\n');
    end;
  fprintf(fid,'}\n');
else
  fprintf(fid,['Separator { Transform { translation %.4g %.4g %.4g ',...
      '} Sphere { radius %.4g }}\n'], from, width);
end;








