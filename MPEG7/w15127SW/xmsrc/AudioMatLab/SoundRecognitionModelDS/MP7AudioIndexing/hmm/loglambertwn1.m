function xx = loglambertwn1(yy)
%LOGLAMBERTWN1 Branch -1 of Lambert's W function (log).
%   X = LOGLAMBERTWN1(Y) computes x = -W(-exp(-y)), where W is branch -1 of
%   Lambert's W function, such that x * exp(-x) = exp(-y).
%
%   $Id: loglambertwn1.m,v 1.1.1.1 2002/03/05 12:53:28 karol2000 Exp $

siz = size(yy);
xx = zeros(siz);
for i = 1:prod(siz)
  y = yy(i);
  if y > 1,
    if y < 20,
      xx(i) = -lambertwn1(-exp(-y));
    elseif y == Inf,
      xx(i) = Inf;
    else
      % make a first guess
      if y < 1.5,
        x = sqrt(2.*(y-1)); % branch point series
        x = (((((((1/204120.*x-139/5443200).*x+1/17010) ...
            .*x+1/4320).*x-1/270).*x+1/36).*x+1/3).*x+1).*x+1;
      else
        x = y + log(y);
      end;
      oldx = 0;
      while abs(oldx - x) > realmin,
        oldx = x;
        x = y + log(x);
      end;
      xx(i) = x;
    end;
  elseif y == 1,
    xx(i) = 1;
  else
    warning(sprintf('loglambertwn1 argument out of range: %f', y));
    xx(i) = NaN;
  end;
end;
