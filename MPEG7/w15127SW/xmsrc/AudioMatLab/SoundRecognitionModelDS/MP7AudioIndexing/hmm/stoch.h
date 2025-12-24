/*
 * MATLAB Compiler: 2.0.1
 * Date: Thu Oct 12 17:01:35 2000
 * Arguments: "-x" "map_puppet" 
 */

#ifndef MLF_V2
#define MLF_V2 1
#endif

#ifndef __stoch_h
#define __stoch_h 1

#include "matlab.h"

extern mxArray * mlfStoch(mxArray * p_);
extern void mlxStoch(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]);

#endif
