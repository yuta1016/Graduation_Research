/*
 * MATLAB Compiler: 2.0.1
 * Date: Thu Oct 12 17:01:35 2000
 * Arguments: "-x" "map_puppet" 
 */

#ifndef MLF_V2
#define MLF_V2 1
#endif

#ifndef __vadd_h
#define __vadd_h 1

#include "matlab.h"

extern mxArray * mlfVadd(mxArray * a, mxArray * b);
extern void mlxVadd(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]);

#endif
