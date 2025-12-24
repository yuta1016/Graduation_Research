/*
 * Viterbi dynamic programming for hidden Markov models.
 *
 * $Id: viterbi_lite.c,v 1.1.1.1 2002/03/05 12:53:46 karol2000 Exp $
 */

#include <matrix.h>
#include <math.h>
#include <stdio.h>

#define NEGINF          (-1.0/0.0)

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    const mxArray *aTransitions;
    const double *pdTransitions;
    const mxArray *aStarts;
    const double *pdStarts;
    const mxArray *aProbs;
    const double *pdProbs;
    int nStates;
    int nTicks;

    mxArray *aPath;
    double *pdPath;
    mxArray *aLogLike;
    double *pdLogLike;

    double *pdLogTransitions;
    double *pdDeltaCurr;
    double *pdDeltaPrev;
    int *piPsi;

    /* Check number of arguments */
    if (nrhs != 3) mexErrMsgTxt("takes 3 input arguments.");
    if (nlhs >  2) mexErrMsgTxt("outputs 2 results.");

    /* Grab and check input arguments */
    aTransitions = prhs[0];
    pdTransitions = mxGetPr(aTransitions);
    nStates = mxGetM(aTransitions);
    if (!mxIsDouble(aTransitions) ||
        mxIsSparse(aTransitions) ||
        mxIsComplex(aTransitions) ||
        nStates != mxGetN(aTransitions))
        mexErrMsgTxt("1st arg must be a real non-sparse square matrix.");
    if (nStates <= 0)
        mexErrMsgTxt("There must be at least one state.");
    aStarts = prhs[1];
    pdStarts = mxGetPr(aStarts);
    if (!mxIsDouble(aStarts) ||
        mxIsSparse(aStarts) ||
        mxIsComplex(aStarts))
        mexErrMsgTxt("2nd arg must be a real non-sparse vector.");
    if (nStates != mxGetM(aStarts) * mxGetN(aStarts))
        mexErrMsgTxt("1st and 2nd arguments must match in size.");
    aProbs = prhs[2];
    pdProbs = mxGetPr(aProbs);
    nTicks = mxGetM(aProbs);
    if (!mxIsDouble(aProbs) ||
        mxIsSparse(aProbs) ||
        mxIsComplex(aProbs))
        mexErrMsgTxt("3rd arg must be a real non-sparse matrix.");
    if (nStates != mxGetN(aProbs))
        mexErrMsgTxt("1st and 3rd arguments must match in size.");
    if (nTicks <= 0)
        mexErrMsgTxt("There must be at least one time step.");

    /* Create output arguments */
    aPath = mxCreateDoubleMatrix(nTicks, 1, mxREAL);
    pdPath = mxGetPr(aPath);
    aLogLike = mxCreateDoubleMatrix(1, 1, mxREAL);
    pdLogLike = mxGetPr(aLogLike);

    /* Allocate temporary storage */
    pdLogTransitions = mxMalloc(sizeof(double) * nStates * nStates);
    pdDeltaCurr = mxMalloc(sizeof(double) * nStates);
    pdDeltaPrev = mxMalloc(sizeof(double) * nStates);
    piPsi = mxMalloc(sizeof(int) * (nTicks - 1) * nStates);

    /* Take the log of aTransitions to get pdLogTransitions */
    {
        register int i;
        for (i = 0; i < nStates * nStates; ++i)
        {
            register const double dTransition = pdTransitions[i];
            pdLogTransitions[i] = dTransition > 0 ? log(dTransition) : NEGINF;
        }
    }

    /* Initialize forward propagation */
    {
        register int iState;
        for (iState = 0; iState < nStates; ++iState)
        {
            register const double dProb = pdProbs[iState * nTicks];
            register const double dStart = pdStarts[iState];
            pdDeltaCurr[iState]
                = (dProb  > 0 ? log(dProb ) : NEGINF)
                + (dStart > 0 ? log(dStart) : NEGINF);
        }
    }

    /* Forward propagation */
    {
        register int iTick;
        for (iTick = 1; iTick < nTicks; ++iTick)
        {
            /* The current deltas have become the previous deltas */
            {
                register double *const pdDeltaTemp = pdDeltaCurr;
                pdDeltaCurr = pdDeltaPrev;
                pdDeltaPrev = pdDeltaTemp;
            }
            /* Pick the best source state for each target state */
            {
                register int iStateT;
                for (iStateT = 0; iStateT < nStates; ++iStateT)
                {
                    register int iPsi = 0;
                    register double dMaxLike = NEGINF;
                    {
                        register int iStateF;
                        for (iStateF = 0; iStateF < nStates; ++iStateF)
                        {
                            register const double dLike = pdDeltaPrev[iStateF]
                                + pdLogTransitions[iStateF + iStateT * nStates];
                            if (dMaxLike < dLike)
                            {
                                dMaxLike = dLike;
                                iPsi = iStateF;
                            }
                        }
                    }
                    piPsi[(iTick - 1) * nStates + iStateT] = iPsi;
                    {
                        register const double dProb
                            = pdProbs[iTick + iStateT * nTicks];
                        pdDeltaCurr[iStateT] = dMaxLike
                            + (dProb > 0 ? log(dProb) : NEGINF);
                    }
                }
            }
        }
    }

    /* Backward propagation */
    {
        register int iPsi = 0;

        /* Last time step */
        {
            register double dMaxLike = NEGINF;
            {
                register int iStateFrom;
                for (iStateFrom = 0; iStateFrom < nStates; ++iStateFrom)
                {
                    if (dMaxLike < pdDeltaCurr[iStateFrom])
                    {
                        dMaxLike = pdDeltaCurr[iStateFrom];
                        iPsi = iStateFrom;
                    }
                }
            }
            *pdLogLike = dMaxLike;
            pdPath[nTicks - 1] = iPsi + 1;
        }

        /* Backtrack */
        {
            register int iTick;
            for (iTick = nTicks - 2; iTick >= 0; --iTick)
            {
                iPsi = piPsi[iTick * nStates + iPsi];
                pdPath[iTick] = iPsi + 1;
            }
        }
    }

    /* Deallocate temporary storage */
    mxFree(piPsi);
    mxFree(pdDeltaPrev);
    mxFree(pdDeltaCurr);
    mxFree(pdLogTransitions);

    /* Return output arguments */
    if (nlhs > 1)
        plhs[1] = aLogLike;
    else
        mxDestroyArray(aLogLike);
    plhs[0] = aPath;
}
