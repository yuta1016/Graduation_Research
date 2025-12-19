function params = struct_params(varargin)
%STRUCT_PARAMS Flexible parsing of function input arguments.
%   PARAMS = STRUCT_PARAMS(...) is similar to STRUCT, but it constructs
%   only one structure, and handles cell arrays and structures specially
%   as follows.  Structures are merged into PARAMS as keyword arguments.
%   Cell arrays are parsed as if they are interpolated into the argument
%   list.  If the same field name is specified twice, the first value is
%   kept.  This allows optional defaults to be specified and overridden.
%
%   $Id: struct_params.m,v 1.1.1.1 2002/03/05 12:53:34 karol2000 Exp $

params = [];
iargin = nargin;
while iargin > 0
    if iscell(varargin{iargin})
        ncell = prod(size(varargin{iargin}));
        if mod(ncell, 2)
            error('Cell array argument contains an odd number of elements.');
        end
        for icell = ncell : -2 : 2
            params = setfield(params, ...
                varargin{iargin}{icell-1}, varargin{iargin}{icell});
        end
        iargin = iargin - 1;
    elseif isstruct(varargin{iargin})
        for field = fieldnames(varargin{iargin})'
            params = setfield(params, ...
                field{1}, getfield(varargin{iargin}, field{1}));
        end
        iargin = iargin - 1;
    else
        if iargin == 1
            error('Odd number of non-cell-array, non-structure arguments.');
        end
        params = setfield(params, varargin{iargin-1}, varargin{iargin});
        iargin = iargin - 2;
    end
end

