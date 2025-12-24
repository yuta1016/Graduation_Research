/*
 * MATLAB Compiler: 2.0
 * Date: Mon Jul 19 18:48:59 1999
 * Arguments: "-x" "baumwelch_all" 
 */

#ifndef MLF_V2
#define MLF_V2 1
#endif

#ifndef __fb_analysis_h
#define __fb_analysis_h 1

#include "matlab.h"

extern mxArray * mlfNFb_analysis(int nargout,
                                 mxArray * * forward,
                                 mxArray * * backward,
                                 mxArray * * occupation,
                                 mxArray * * scale,
                                 mxArray * Transitions,
                                 mxArray * Starts,
                                 mxArray * P_obs_giv_state,
                                 mxArray * frames,
                                 mxArray * first);
extern mxArray * mlfFb_analysis(mxArray * * forward,
                                mxArray * * backward,
                                mxArray * * occupation,
                                mxArray * * scale,
                                mxArray * Transitions,
                                mxArray * Starts,
                                mxArray * P_obs_giv_state,
                                mxArray * frames,
                                mxArray * first);
extern void mlfVFb_analysis(mxArray * Transitions,
                            mxArray * Starts,
                            mxArray * P_obs_giv_state,
                            mxArray * frames,
                            mxArray * first);
extern void mlxFb_analysis(int nlhs,
                           mxArray * plhs[],
                           int nrhs,
                           mxArray * prhs[]);

#endif
