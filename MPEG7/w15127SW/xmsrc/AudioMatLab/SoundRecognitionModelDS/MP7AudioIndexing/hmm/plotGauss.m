function [] = plotGauss(mu1,mu2,var1,var2,covar,color,width)
%plotGauss(mu1,mu2,var1,var2,covar)
%
%PLOT A 2D Gaussian
%This function plots the given 2D gaussian on the current plot.

if nargin<6, color = 'y'; end;
if nargin<7, width = 0.5; end;
t = -pi:.01:pi;
k = length(t);
x = sin(t);
y = cos(t);

R = [var1 covar; covar var2];

[vv,dd] = eig(R);
A = real((vv*sqrt(dd))');
%A = real((vv*dd)');
z = [x' y']*A;

hold on;
if width>0, plot(mu1,mu2,'X','Color',color,'LineWidth',width);end;
plot(z(:,1)+mu1,z(:,2)+mu2,'Color',color);
