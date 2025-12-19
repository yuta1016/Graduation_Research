/*
 * MATLAB Compiler: 2.0.1
 * Date: Tue Oct 10 16:03:17 2000
 * Arguments: "-x" "wcov" 
 */

#ifndef MLF_V2
#define MLF_V2 1
#endif

#ifndef __wcov_h
#define __wcov_h 1

#include "matlab.h"

extern mxArray * mlfNWcov(int nargout,
                          mxArray * * m,
                          mxArray * X,
                          mxArray * w,
                          mxArray * m_,
                          mxArray * flag);
extern mxArray * mlfWcov(mxArray * * m,
                         mxArray * X,
                         mxArray * w,
                         mxArray * m_,
                         mxArray * flag);
extern void mlfVWcov(mxArray * X, mxArray * w, mxArray * m_, mxArray * flag);
extern void mlxWcov(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]);

#endif
