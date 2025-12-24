/*
 * MATLAB Compiler: 2.0.1
 * Date: Tue Dec 14 02:02:28 1999
 * Arguments: "-x" "B3" 
 */

#ifndef MLF_V2
#define MLF_V2 1
#endif

#ifndef __mahalonobis_h
#define __mahalonobis_h 1

#include "matlab.h"

extern mxArray * mlfMahalonobis(mxArray * x, mxArray * icov, mxArray * mean);
extern void mlxMahalonobis(int nlhs,
                           mxArray * plhs[],
                           int nrhs,
                           mxArray * prhs[]);

#endif
