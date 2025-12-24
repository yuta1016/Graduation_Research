/*
 * MATLAB Compiler: 2.0.1
 * Date: Tue Dec 14 02:02:28 1999
 * Arguments: "-x" "B3" 
 */

#ifndef MLF_V2
#define MLF_V2 1
#endif

#ifndef __B3_h
#define __B3_h 1

#include "matlab.h"

extern mxArray * mlfB3(mxArray * X, mxArray * icovariance, mxArray * mean);
extern void mlxB3(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]);

#endif
