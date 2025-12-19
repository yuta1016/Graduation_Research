/*
 * MATLAB Compiler: 2.0
 * Date: Tue Jul 20 09:13:59 1999
 * Arguments: "-x" "-v" "entropic_map" 
 */

#ifndef MLF_V2
#define MLF_V2 1
#endif

#ifndef __loglambertwn1_h
#define __loglambertwn1_h 1

#include "matlab.h"

extern mxArray * mlfLoglambertwn1(mxArray * yy);
extern void mlxLoglambertwn1(int nlhs,
                             mxArray * plhs[],
                             int nrhs,
                             mxArray * prhs[]);

#endif
