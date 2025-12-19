/*
 * MATLAB Compiler: 2.0
 * Date: Tue Jul 20 09:13:59 1999
 * Arguments: "-x" "-v" "entropic_map" 
 */

#ifndef MLF_V2
#define MLF_V2 1
#endif

#ifndef __entropic_map_h
#define __entropic_map_h 1

#include "matlab.h"

extern mxArray * mlfNEntropic_map(int nargout,
                                  mxArray * * loglike,
                                  mxArray * omega,
                                  mxArray * z,
                                  mxArray * kappa,
                                  mxArray * bias);
extern mxArray * mlfEntropic_map(mxArray * * loglike,
                                 mxArray * omega,
                                 mxArray * z,
                                 mxArray * kappa,
                                 mxArray * bias);
extern void mlfVEntropic_map(mxArray * omega,
                             mxArray * z,
                             mxArray * kappa,
                             mxArray * bias);
extern void mlxEntropic_map(int nlhs,
                            mxArray * plhs[],
                            int nrhs,
                            mxArray * prhs[]);

#endif
