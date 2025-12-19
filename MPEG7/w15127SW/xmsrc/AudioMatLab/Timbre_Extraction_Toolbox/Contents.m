% Matlab functions for Timbre Description
% =======================================
%
% Target:   MP7-XM version
% Author:   CUIDADO/IRCAM/ G. Peeters 
% LastEdit: 2001/03/12
%
% 1) Functions list
% =================
%
% InstrumentTimbre
%
% Hspectre
% Hharmo (Fevalbp)
% -> Fharmopic
% HharmoiParam
% -> Fihsc
% -> Fihsd (Fspecenv)
% -> Fihss
% -> Fihsv
%
% Fenergy
% -> Flat
% -> Ftc
%
%
% 2) DataTypeDefinition
% =====================
%
% F*.m : denotes a function, all function start with capital F
%    
% *_m      : matrix data
% *_v      : vector data
% *_iv     : complex vector data
% *_struct : structure
% *_bp     : breakpoint function
%
% *_sec_* : units in seconds
% *_n_*   : units in samples
% *_hz_*  : units in Hz
% *_lin_* : linear scale
% *_dB_*  : logarithmic scale
%