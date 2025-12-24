/*
 * MATLAB Compiler: 2.0.1
 * Date: Tue Dec 14 02:02:28 1999
 * Arguments: "-x" "B3" 
 */

#ifndef MLF_V2
#define MLF_V2 1
#endif

#include "matlab.h"
#include "mahalonobis.h"
#include "B3.h"

mxArray * _persistent_B3__CONSTANTS_INITIALIZED = NULL;

mxArray * _persistent_B3__EPSILON = NULL;

mxArray * _persistent_B3__REALMAX = NULL;

mxArray * _persistent_B3__REALMIN = NULL;

mxArray * _persistent_B3__TWOPI = NULL;

static mlfFunctionTableEntry function_table[1] = { { "B3", mlxB3, 3, 1 } };

/*
 * The function mccAtExitFunction is a Compiler-generated mexAtExit function.
 * Either mexFunction or mdlOutputs (one of which appears below) registers
 * mccAtExitFunction just after it initializes any persistent variables.
 * Because of this registration, MATLAB calls mccAtExitFunction before it
 * unloads this MEX-function from memory. When called, mccAtExitFunction cleans
 * up the persistent variables that were initialized when it was registered.
 * Note that if there are no persistent variables in the original M-function,
 * this function is not generated.
 */
static void mccAtExitFunction(void) {
    mxDestroyArray(_persistent_B3__CONSTANTS_INITIALIZED);
    mxDestroyArray(_persistent_B3__EPSILON);
    mxDestroyArray(_persistent_B3__REALMAX);
    mxDestroyArray(_persistent_B3__REALMIN);
    mxDestroyArray(_persistent_B3__TWOPI);
}

static mxArray * Mmahalonobis(int nargout_,
                              mxArray * x,
                              mxArray * icov,
                              mxArray * mean) {
    mxArray * distance = mclGetUninitializedArray();
    mclFevalCallMATLAB(
      mclNVarargout(nargout_, 0, &distance, NULL),
      "mahalonobis",
      x, icov, mean, NULL);
    return distance;
}

/*
 * The function "mexFunction" is a Compiler-generated mex wrapper, suitable for
 * building a MEX-function. It initializes any persistent variables as well as
 * a function table for use by the feval function. It then calls the function
 * "mlxB3". Finally, it clears the feval table and exits.
 */
void mexFunction(int nlhs, mxArray * * plhs, int nrhs, mxArray * * prhs) {
    mlfTry {
        mlfFunctionTableSetup(1, function_table);
        mclImportGlobal(0, NULL);
        if (_persistent_B3__CONSTANTS_INITIALIZED == NULL) {
            mlfAssign(
              &_persistent_B3__CONSTANTS_INITIALIZED, mclCreateGlobal());
            mlfAssign(&_persistent_B3__EPSILON, mclCreateGlobal());
            mlfAssign(&_persistent_B3__REALMAX, mclCreateGlobal());
            mlfAssign(&_persistent_B3__REALMIN, mclCreateGlobal());
            mlfAssign(&_persistent_B3__TWOPI, mclCreateGlobal());
            mexAtExit(mccAtExitFunction);
        }
        mlxB3(nlhs, plhs, nrhs, prhs);
        mlfFunctionTableTakedown(1, function_table);
    } mlfCatch {
        mlfFunctionTableTakedown(1, function_table);
        mclMexError();
    } mlfEndCatch
}

void mlxMahalonobis(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]) {
    mxArray * mprhs[3];
    mxArray * mplhs[1];
    int i;
    if (nlhs > 1) {
        mlfError(
          mxCreateString(
            "Run-time Error: File: mahalonobis Line: 1 Column"
            ": 0 The function \"mahalonobis\" was called with"
            " more than the declared number of outputs (1)"));
    }
    if (nrhs > 3) {
        mlfError(
          mxCreateString(
            "Run-time Error: File: mahalonobis Line: 1 Column"
            ": 0 The function \"mahalonobis\" was called with"
            " more than the declared number of inputs (3)"));
    }
    for (i = 0; i < 1; ++i) {
        mplhs[i] = NULL;
    }
    for (i = 0; i < 3 && i < nrhs; ++i) {
        mprhs[i] = prhs[i];
    }
    for (; i < 3; ++i) {
        mprhs[i] = NULL;
    }
    mlfEnterNewContext(0, 3, mprhs[0], mprhs[1], mprhs[2]);
    mplhs[0] = Mmahalonobis(nlhs, mprhs[0], mprhs[1], mprhs[2]);
    mlfRestorePreviousContext(0, 3, mprhs[0], mprhs[1], mprhs[2]);
    plhs[0] = mplhs[0];
}

mxArray * mlfMahalonobis(mxArray * x, mxArray * icov, mxArray * mean) {
    int nargout = 1;
    mxArray * distance = mclGetUninitializedArray();
    mlfEnterNewContext(0, 3, x, icov, mean);
    distance = Mmahalonobis(nargout, x, icov, mean);
    mlfRestorePreviousContext(0, 3, x, icov, mean);
    return mlfReturnValue(distance);
}
