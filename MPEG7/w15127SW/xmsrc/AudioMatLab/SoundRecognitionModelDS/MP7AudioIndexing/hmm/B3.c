/*
 * MATLAB Compiler: 2.0.1
 * Date: Tue Dec 14 02:02:28 1999
 * Arguments: "-x" "B3" 
 */
#include "B3.h"
#include "mahalonobis.h"

extern mxArray * _persistent_B3__CONSTANTS_INITIALIZED;
extern mxArray * _persistent_B3__EPSILON;
extern mxArray * _persistent_B3__REALMAX;
extern mxArray * _persistent_B3__REALMIN;
extern mxArray * _persistent_B3__TWOPI;

/*
 * The function "MB3" is the implementation version of the "B3" M-function from
 * file "/tmp_mnt/projects/puppets/matlab/B3.m" (lines 1-50). It contains the
 * actual compiled code for that M-function. It is a static function and must
 * only be called from one of the interface functions, appearing below.
 */
/*
 * %B3 compute multivariate Gaussian probabilities
 * % function probs = B3(X,icovariance,mean)
 * %
 * % multivariate Gaussian; X is a row vector(s)
 * % takes pains to return a positive value for ill-conditioned covariance matrix
 * 
 * function probs = B3(X,icovariance,mean)
 */
static mxArray * MB3(int nargout_,
                     mxArray * X,
                     mxArray * icovariance,
                     mxArray * mean) {
    mxArray * probs = mclGetUninitializedArray();
    mxArray * T = mclGetUninitializedArray();
    mxArray * bad = mclGetUninitializedArray();
    mxArray * d = mclGetUninitializedArray();
    mxArray * idtr = mclGetUninitializedArray();
    mxArray * nargin_ = mclGetUninitializedArray();
    mlfAssign(&nargin_, mlfNargin(0, X, icovariance, mean, NULL));
    mclValidateInputs("B3", 3, &X, &icovariance, &mean);
    mclCopyArray(&X);
    mclCopyArray(&icovariance);
    mclCopyArray(&mean);
    /*
     * 
     * persistent CONSTANTS_INITIALIZED REALMIN REALMAX EPSILON TWOPI
     * if isempty(CONSTANTS_INITIALIZED),
     */
    if (mlfTobool(mlfIsempty(_persistent_B3__CONSTANTS_INITIALIZED))) {
        /*
         * REALMIN = realmin;
         */
        mlfAssign(&_persistent_B3__REALMIN, mlfRealmin());
        /*
         * REALMAX = realmax;
         */
        mlfAssign(&_persistent_B3__REALMAX, mlfRealmax());
        /*
         * EPSILON = 1e-50;
         */
        mlfAssign(&_persistent_B3__EPSILON, mlfScalar(1e-50));
        /*
         * TWOPI= 2*pi;
         */
        mlfAssign(&_persistent_B3__TWOPI, mlfMtimes(mlfScalar(2.0), mlfPi()));
        /*
         * CONSTANTS_INITIALIZED = 1;
         */
        mlfAssign(&_persistent_B3__CONSTANTS_INITIALIZED, mlfScalar(1.0));
    /*
     * end;
     */
    }
    /*
     * 
     * bad=find(~isfinite(diag(icovariance)));	% remove dimensions w/infinite variance
     */
    mlfAssign(
      &bad,
      mlfFind(NULL, NULL, mlfNot(mlfIsfinite(mlfDiag(icovariance, NULL)))));
    /*
     * if ~isempty(bad)
     */
    if (mlfTobool(mlfNot(mlfIsempty(bad)))) {
        /*
         * icovariance(bad,:)=[]; 
         */
        mlfIndexDelete(&icovariance, "(?,?)", bad, mlfCreateColonIndex());
        /*
         * if isempty(icovariance), error('B3: entire covariance matrix is bad'); end;
         */
        if (mlfTobool(mlfIsempty(icovariance))) {
            mlfError(mxCreateString("B3: entire covariance matrix is bad"));
        }
        /*
         * icovariance(:,bad)=[]; X(:,bad)=[]; mean(:,bad)=[]; 
         */
        mlfIndexDelete(&icovariance, "(?,?)", mlfCreateColonIndex(), bad);
        mlfIndexDelete(&X, "(?,?)", mlfCreateColonIndex(), bad);
        mlfIndexDelete(&mean, "(?,?)", mlfCreateColonIndex(), bad);
    /*
     * end;
     */
    }
    /*
     * 
     * % Compute determinant, taking care of near-flat dimensions
     * idtr=det(icovariance);
     */
    mlfAssign(&idtr, mlfDet(icovariance));
    /*
     * if isinf(idtr)| idtr<=0, idtr=min(REALMAX,prod(max(EPSILON,eig(icovariance)))); end;
     */
    {
        mxArray * a_ = mclInitialize(mlfIsinf(idtr));
        if (mlfTobool(a_)
            || mlfTobool(mlfOr(a_, mlfLe(idtr, mlfScalar(0.0))))) {
            mxDestroyArray(a_);
            mlfAssign(
              &idtr,
              mlfMin(
                NULL,
                _persistent_B3__REALMAX,
                mlfProd(
                  mlfMax(
                    NULL,
                    _persistent_B3__EPSILON,
                    mlfEig(NULL, icovariance, NULL),
                    NULL),
                  NULL),
                NULL));
        } else {
            mxDestroyArray(a_);
        }
    }
    /*
     * 
     * 
     * [T,d] = size(X);
     */
    mlfSize(mlfVarargout(&T, &d, NULL), X, NULL);
    /*
     * 
     * if d~=size(mean,2)
     */
    if (mlfTobool(
          mlfNe(d, mlfSize(mclValueVarargout(), mean, mlfScalar(2.0))))) {
        /*
         * disp('B3: error, data and mean do not agree in dimensionality, press any key'); 
         */
        mlfDisp(
          mxCreateString(
            "B3: error, data and mean do not agree"
            " in dimensionality, press any key"));
        /*
         * pause;
         */
        mlfPause(NULL);
    /*
     * end
     */
    }
    /*
     * 
     * if nargin>2, 
     */
    if (mlfTobool(mlfGt(nargin_, mlfScalar(2.0)))) {
        /*
         * probs = ...
         */
        mlfAssign(
          &probs,
          mlfRdivide(
            mlfExp(
              mlfTimes(mlfScalar(-.5), mlfMahalonobis(X, icovariance, mean))),
            mlfSqrt(
              mlfRdivide(
                mlfMpower(_persistent_B3__TWOPI, d),
                mlfPlus(_persistent_B3__REALMIN, idtr)))));
    /*
     * exp( -.5 .* mahalonobis(X,icovariance,mean)) ...
     * ./ sqrt(TWOPI^d ./ (REALMIN+idtr)) ...
     * ;
     * else
     */
    } else {
        /*
         * probs = ...
         */
        mlfAssign(
          &probs,
          mlfRdivide(
            mlfExp(
              mlfTimes(mlfScalar(-.5), mlfMahalonobis(X, icovariance, NULL))),
            mlfSqrt(
              mlfRdivide(
                mlfMpower(_persistent_B3__TWOPI, d),
                mlfPlus(_persistent_B3__REALMIN, idtr)))));
    /*
     * exp( -.5 .* mahalonobis(X,icovariance)) ...
     * ./ sqrt(TWOPI^d ./ (REALMIN+idtr)) ...
     * ;
     * end;
     */
    }
    mclValidateOutputs("B3", 1, nargout_, &probs);
    mxDestroyArray(T);
    mxDestroyArray(X);
    mxDestroyArray(bad);
    mxDestroyArray(d);
    mxDestroyArray(icovariance);
    mxDestroyArray(idtr);
    mxDestroyArray(mean);
    mxDestroyArray(nargin_);
    /*
     * %probs=min(REALMAX, max(REALMIN, probs));
     * 
     */
    return probs;
}

/*
 * The function "mlfB3" contains the normal interface for the "B3" M-function
 * from file "/tmp_mnt/projects/puppets/matlab/B3.m" (lines 1-50). This
 * function processes any input arguments and passes them to the implementation
 * version of the function, appearing above.
 */
mxArray * mlfB3(mxArray * X, mxArray * icovariance, mxArray * mean) {
    int nargout = 1;
    mxArray * probs = mclGetUninitializedArray();
    mlfEnterNewContext(0, 3, X, icovariance, mean);
    probs = MB3(nargout, X, icovariance, mean);
    mlfRestorePreviousContext(0, 3, X, icovariance, mean);
    return mlfReturnValue(probs);
}

/*
 * The function "mlxB3" contains the feval interface for the "B3" M-function
 * from file "/tmp_mnt/projects/puppets/matlab/B3.m" (lines 1-50). The feval
 * function calls the implementation version of B3 through this function. This
 * function processes any input arguments and passes them to the implementation
 * version of the function, appearing above.
 */
void mlxB3(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]) {
    mxArray * mprhs[3];
    mxArray * mplhs[1];
    int i;
    if (nlhs > 1) {
        mlfError(
          mxCreateString(
            "Run-time Error: File: B3 Line: 7 Column: 0 The function \"B3\" "
            "was called with more than the declared number of outputs (1)"));
    }
    if (nrhs > 3) {
        mlfError(
          mxCreateString(
            "Run-time Error: File: B3 Line: 7 Column: 0 The function \"B3\" "
            "was called with more than the declared number of inputs (3)"));
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
    mplhs[0] = MB3(nlhs, mprhs[0], mprhs[1], mprhs[2]);
    mlfRestorePreviousContext(0, 3, mprhs[0], mprhs[1], mprhs[2]);
    plhs[0] = mplhs[0];
}
