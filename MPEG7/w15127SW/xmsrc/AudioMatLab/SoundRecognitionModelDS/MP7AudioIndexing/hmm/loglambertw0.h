/*
 * MATLAB Compiler: 2.0
 * Date: Tue Jul 20 09:13:59 1999
 * Arguments: "-x" "-v" "entropic_map" 
 */

#ifndef MLF_V2
#define MLF_V2 1
#endif

#ifndef __loglambertw0_h
#define __loglambertw0_h 1

#include "matlab.h"

extern mxArray * mlfLoglambertw0(mxArray * y);
extern void mlxLoglambertw0(int nlhs,
                            mxArray * plhs[],
                            int nrhs,
                            mxArray * prhs[]);

#endif
