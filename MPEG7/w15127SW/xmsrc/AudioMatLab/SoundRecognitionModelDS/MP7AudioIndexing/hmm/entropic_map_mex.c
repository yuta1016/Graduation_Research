/*
 * MATLAB Compiler: 2.0
 * Date: Tue Jul 20 10:54:30 1999
 * Arguments: "-x" "-v" "entropic_map" 
 */

#ifndef MLF_V2
#define MLF_V2 1
#endif

#include "matlab.h"
#include "vadd.h"
#include "stoch.h"
#include "loglambertwn1.h"
#include "loglambertw0.h"
#include "entropic_map.h"

mxArray * _persistent_entropic_map__CONSTANTS_INITIALIZED = NULL;

mxArray * _persistent_entropic_map__NPHI = NULL;

mxArray * _persistent_entropic_map__PHI = NULL;

mxArray * _persistent_entropic_map__REALMAX = NULL;

mxArray * _persistent_entropic_map__REALMIN = NULL;

static mlfFunctionTableEntry function_table[1]
  = { { "entropic_map", mlxEntropic_map, 4, 2 } };

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
    mxDestroyArray(_persistent_entropic_map__CONSTANTS_INITIALIZED);
    mxDestroyArray(_persistent_entropic_map__NPHI);
    mxDestroyArray(_persistent_entropic_map__PHI);
    mxDestroyArray(_persistent_entropic_map__REALMAX);
    mxDestroyArray(_persistent_entropic_map__REALMIN);
}

static mxArray * Mvadd(int nargout_, mxArray * a, mxArray * b) {
    mxArray * res = mclGetUninitializedArray();
    mclFevalCallMATLAB(
      mclNVarargout(nargout_, 0, &res, NULL), "vadd", a, b, NULL);
    return res;
}

static mxArray * Mstoch(int nargout_, mxArray * p_) {
    mxArray * p = mclGetUninitializedArray();
    mclCopyInputArg(&p, p_);
    mclFevalCallMATLAB(mclNVarargout(nargout_, 0, &p, NULL), "stoch", p_, NULL);
    return p;
}

static mxArray * Mloglambertwn1(int nargout_, mxArray * yy) {
    mxArray * xx = mclGetUninitializedArray();
    mclFevalCallMATLAB(
      mclNVarargout(nargout_, 0, &xx, NULL), "loglambertwn1", yy, NULL);
    return xx;
}

static mxArray * Mloglambertw0(int nargout_, mxArray * y) {
    mxArray * x = mclGetUninitializedArray();
    mclFevalCallMATLAB(
      mclNVarargout(nargout_, 0, &x, NULL), "loglambertw0", y, NULL);
    return x;
}

/*
 * The function "mexFunction" is a Compiler-generated mex wrapper, suitable for
 * building a MEX-function. It initializes any persistent variables as well as
 * a function table for use by the feval function. It then calls the function
 * "mlxEntropic_map". Finally, it clears the feval table and exits.
 */
void mexFunction(int nlhs, mxArray * * plhs, int nrhs, mxArray * * prhs) {
    mlfTry {
        mlfFunctionTableSetup(1, function_table);
        mclImportGlobal(0, NULL);
        if (_persistent_entropic_map__CONSTANTS_INITIALIZED == NULL) {
            mlfAssign(
              &_persistent_entropic_map__CONSTANTS_INITIALIZED,
              mclCreateGlobal());
            mlfAssign(&_persistent_entropic_map__NPHI, mclCreateGlobal());
            mlfAssign(&_persistent_entropic_map__PHI, mclCreateGlobal());
            mlfAssign(&_persistent_entropic_map__REALMAX, mclCreateGlobal());
            mlfAssign(&_persistent_entropic_map__REALMIN, mclCreateGlobal());
            mexAtExit(mccAtExitFunction);
        }
        mlxEntropic_map(nlhs, plhs, nrhs, prhs);
        mlfFunctionTableTakedown(1, function_table);
    } mlfCatch {
        mlfFunctionTableTakedown(1, function_table);
        mclMexError();
    } mlfEndCatch
}

void mlxVadd(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]) {
    mxArray * mprhs[2];
    mxArray * mplhs[1];
    int i;
    if (nlhs > 1) {
        mlfError(
          mxCreateString(
            "Run-time Error: File: vadd Line: 1 Column: 0 The function \"vadd\""
            " was called with more than the declared number of outputs (1)"));
    }
    if (nrhs > 2) {
        mlfError(
          mxCreateString(
            "Run-time Error: File: vadd Line: 1 Column: 0 The function \"vadd"
            "\" was called with more than the declared number of inputs (2)"));
    }
    for (i = 0; i < 1; ++i) {
        mplhs[i] = NULL;
    }
    for (i = 0; i < 2 && i < nrhs; ++i) {
        mprhs[i] = prhs[i];
    }
    for (; i < 2; ++i) {
        mprhs[i] = NULL;
    }
    mlfEnterNewContext(0, 2, mprhs[0], mprhs[1]);
    mplhs[0] = Mvadd(nlhs, mprhs[0], mprhs[1]);
    mlfRestorePreviousContext(0, 2, mprhs[0], mprhs[1]);
    plhs[0] = mplhs[0];
}

mxArray * mlfVadd(mxArray * a, mxArray * b) {
    int nargout = 1;
    mxArray * res = mclGetUninitializedArray();
    mlfEnterNewContext(0, 2, a, b);
    res = Mvadd(nargout, a, b);
    mlfRestorePreviousContext(0, 2, a, b);
    return mlfReturnValue(res);
}

void mlxStoch(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]) {
    mxArray * mprhs[1];
    mxArray * mplhs[1];
    int i;
    if (nlhs > 1) {
        mlfError(
          mxCreateString(
            "Run-time Error: File: stoch Line: 1 Column: 0 The function \"stoch"
            "\" was called with more than the declared number of outputs (1)"));
    }
    if (nrhs > 1) {
        mlfError(
          mxCreateString(
            "Run-time Error: File: stoch Line: 1 Column: 0 The function \"stoch"
            "\" was called with more than the declared number of inputs (1)"));
    }
    for (i = 0; i < 1; ++i) {
        mplhs[i] = NULL;
    }
    for (i = 0; i < 1 && i < nrhs; ++i) {
        mprhs[i] = prhs[i];
    }
    for (; i < 1; ++i) {
        mprhs[i] = NULL;
    }
    mlfEnterNewContext(0, 1, mprhs[0]);
    mplhs[0] = Mstoch(nlhs, mprhs[0]);
    mlfRestorePreviousContext(0, 1, mprhs[0]);
    plhs[0] = mplhs[0];
}

mxArray * mlfStoch(mxArray * p_) {
    int nargout = 1;
    mxArray * p = mclGetUninitializedArray();
    mlfEnterNewContext(0, 1, p_);
    p = Mstoch(nargout, p_);
    mlfRestorePreviousContext(0, 1, p_);
    return mlfReturnValue(p);
}

void mlxLoglambertwn1(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]) {
    mxArray * mprhs[1];
    mxArray * mplhs[1];
    int i;
    if (nlhs > 1) {
        mlfError(
          mxCreateString(
            "Run-time Error: File: loglambertwn1 Line: 1 Column"
            ": 0 The function \"loglambertwn1\" was called with"
            " more than the declared number of outputs (1)"));
    }
    if (nrhs > 1) {
        mlfError(
          mxCreateString(
            "Run-time Error: File: loglambertwn1 Line: 1 Colum"
            "n: 0 The function \"loglambertwn1\" was called wi"
            "th more than the declared number of inputs (1)"));
    }
    for (i = 0; i < 1; ++i) {
        mplhs[i] = NULL;
    }
    for (i = 0; i < 1 && i < nrhs; ++i) {
        mprhs[i] = prhs[i];
    }
    for (; i < 1; ++i) {
        mprhs[i] = NULL;
    }
    mlfEnterNewContext(0, 1, mprhs[0]);
    mplhs[0] = Mloglambertwn1(nlhs, mprhs[0]);
    mlfRestorePreviousContext(0, 1, mprhs[0]);
    plhs[0] = mplhs[0];
}

mxArray * mlfLoglambertwn1(mxArray * yy) {
    int nargout = 1;
    mxArray * xx = mclGetUninitializedArray();
    mlfEnterNewContext(0, 1, yy);
    xx = Mloglambertwn1(nargout, yy);
    mlfRestorePreviousContext(0, 1, yy);
    return mlfReturnValue(xx);
}

void mlxLoglambertw0(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]) {
    mxArray * mprhs[1];
    mxArray * mplhs[1];
    int i;
    if (nlhs > 1) {
        mlfError(
          mxCreateString(
            "Run-time Error: File: loglambertw0 Line: 1 Column"
            ": 0 The function \"loglambertw0\" was called with"
            " more than the declared number of outputs (1)"));
    }
    if (nrhs > 1) {
        mlfError(
          mxCreateString(
            "Run-time Error: File: loglambertw0 Line: 1 Column"
            ": 0 The function \"loglambertw0\" was called with"
            " more than the declared number of inputs (1)"));
    }
    for (i = 0; i < 1; ++i) {
        mplhs[i] = NULL;
    }
    for (i = 0; i < 1 && i < nrhs; ++i) {
        mprhs[i] = prhs[i];
    }
    for (; i < 1; ++i) {
        mprhs[i] = NULL;
    }
    mlfEnterNewContext(0, 1, mprhs[0]);
    mplhs[0] = Mloglambertw0(nlhs, mprhs[0]);
    mlfRestorePreviousContext(0, 1, mprhs[0]);
    plhs[0] = mplhs[0];
}

mxArray * mlfLoglambertw0(mxArray * y) {
    int nargout = 1;
    mxArray * x = mclGetUninitializedArray();
    mlfEnterNewContext(0, 1, y);
    x = Mloglambertw0(nargout, y);
    mlfRestorePreviousContext(0, 1, y);
    return mlfReturnValue(x);
}
