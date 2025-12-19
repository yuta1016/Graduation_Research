function show_hmm(Means,iCovariances,Data,Transitions,d1,d2)
% Visualize the state of an hmm.
% Transition probabilities are shown as a half-line runnng into each
% cluster.  The thicker the line, the higher the conditional probability.
% Output probabilities are shown as Gaussian density isocontours.
% The data is shown as a scatterplot of dots.  You can choose which
% dimensions to project onto the plane (defaults to 1 & 2).

if nargin<6, d1=1; end;
if nargin<5, d2=2; end;
if nargin<4, Transitions=[]; end;
if nargin<3, Data=[]; end;
[n,d]=size(Means);
colors = get(gca,'ColorOrder');
nc=size(colors,1);
offset=(max(Means(:,d1))-min(Means(:,d1)))./20;
for i=1:size(iCovariances,3); iCovariances(:,:,i)=inv(iCovariances(:,:,i)); end;

if ~isempty(Data), set(plot(Data(:,d1),Data(:,d2), 'k.'), 'MarkerSize', 1); hold on;
if ~isempty(Means),
  %cla
  if ~isempty(Transitions),
    for i=1:n,
      from = Means(i,[d1 d2]);
      for j=1:n,
	if ~(j==i),
	  to = Means(j, [d1 d2]);
	  to = [from; mean([from;to])];
          if Transitions(i,j),
	    width = min(10, Transitions(i,j).*20 + 1);
          else
            width = 0;
          end;
	  %width = max(width,1);		% TEMPORARY
	  if width>=1,
	    line(to(:,1), to(:,2), ...
		'LineWidth', width, 'Color', colors(1+rem(j-1,nc),:));
	  elseif width>0,
	    line(to(:,1), to(:,2), ...
		'LineStyle', ':', 'Color', colors(1+rem(j-1,nc),:));
	  end;
	  hold on;
	end;
      end;
    end;
  end;
  if ~isempty(iCovariances),
    for i = 1:n,
      if isempty(Transitions), center=0; 
      else center =  max(0.5,Transitions(i,i).*20); end;
      plotGauss(Means(i,d1),Means(i,d2),iCovariances(d1,d1,i), ...
	  iCovariances(d2,d2,i),iCovariances(d1,d2,i), ...
	  colors(1+rem(i-1,nc),:),center);
      hold on;
    end;
  end;
  c=0.1.*offset;
  %for i = 1:n,
  %  text(Means(i,d1)+offset+c,Means(i,d2)-c,sprintf('%d',i),'color','white');
  %  text(Means(i,d1)+offset,Means(i,d2),sprintf('%d',i),'color','black');
  %end;
end;
axis([min(Data(:,d1)), max(Data(:,d1)), min(Data(:,d2)), max(Data(:,d2))]);
end;
axis('off');
drawnow;
hold off;




