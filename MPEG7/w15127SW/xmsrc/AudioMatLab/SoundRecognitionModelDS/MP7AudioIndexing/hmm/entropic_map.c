/*
 * MATLAB Compiler: 2.0
 * Date: Tue Jul 20 10:54:30 1999
 * Arguments: "-x" "-v" "entropic_map" 
 */
#include "entropic_map.h"
#include "loglambertw0.h"
#include "loglambertwn1.h"
#include "stoch.h"
#include "vadd.h"

extern mxArray * _persistent_entropic_map__CONSTANTS_INITIALIZED;
extern mxArray * _persistent_entropic_map__NPHI;
extern mxArray * _persistent_entropic_map__PHI;
extern mxArray * _persistent_entropic_map__REALMAX;
extern mxArray * _persistent_entropic_map__REALMIN;

static double __Array0_r[1] = { 1.0 };

/*
 * The function "Mentropic_map" is the implementation version of the
 * "entropic_map" M-function from file
 * "/projects/puppets/matlab/hmmtrim/entropic_map.m" (lines 1-255). It contains
 * the actual compiled code for that M-function. It is a static function and
 * must only be called from one of the interface functions, appearing below.
 */
/*
 * function [theta, loglike] = entropic_map(omega, z, kappa, bias)
 */
static mxArray * Mentropic_map(mxArray * * loglike,
                               int nargout_,
                               mxArray * omega,
                               mxArray * z,
                               mxArray * kappa,
                               mxArray * bias) {
    mxArray * theta = mclGetUninitializedArray();
    mxArray * ans = mclInitializeAns();
    mxArray * dddtheta = mclGetUninitializedArray();
    mxArray * ddtheta = mclGetUninitializedArray();
    mxArray * delta = mclGetUninitializedArray();
    mxArray * dlambda = mclGetUninitializedArray();
    mxArray * dloglike = mclGetUninitializedArray();
    mxArray * dtheta = mclGetUninitializedArray();
    mxArray * factor = mclGetUninitializedArray();
    mxArray * fix = mclGetUninitializedArray();
    mxArray * jacobian = mclGetUninitializedArray();
    mxArray * kappaz = mclGetUninitializedArray();
    mxArray * l = mclGetUninitializedArray();
    mxArray * lambda = mclGetUninitializedArray();
    mxArray * logomegaz_pkz = mclGetUninitializedArray();
    mxArray * logtheta = mclGetUninitializedArray();
    mxArray * maxlambda = mclGetUninitializedArray();
    mxArray * meankappa = mclGetUninitializedArray();
    mxArray * minlambda = mclGetUninitializedArray();
    mxArray * nargin_ = mclGetUninitializedArray();
    mxArray * nargout = mclInitialize(mlfScalar(nargout_));
    mxArray * nm1 = mclGetUninitializedArray();
    mxArray * nonm1 = mclGetUninitializedArray();
    mxArray * ok = mclGetUninitializedArray();
    mxArray * olddlambda = mclGetUninitializedArray();
    mxArray * olddloglike = mclGetUninitializedArray();
    mxArray * oldlambda = mclUnassigned();
    mxArray * oldloglike = mclGetUninitializedArray();
    mxArray * oldtheta = mclGetUninitializedArray();
    mxArray * omegaz = mclGetUninitializedArray();
    mxArray * problem = mclGetUninitializedArray();
    mxArray * q = mclGetUninitializedArray();
    mxArray * ratio = mclGetUninitializedArray();
    mxArray * signz = mclGetUninitializedArray();
    mxArray * u = mclGetUninitializedArray();
    mxArray * w = mclGetUninitializedArray();
    mxArray * where = mclGetUninitializedArray();
    mlfAssign(&nargin_, mlfNargin(0, omega, z, kappa, bias, NULL));
    mclValidateInputs("entropic_map", 4, &omega, &z, &kappa, &bias);
    mclCopyArray(&omega);
    mclCopyArray(&z);
    mclCopyArray(&kappa);
    mclCopyArray(&bias);
    /*
     * %ENTROPIC_MAP Compute MLE values of theta given omega.
     * %
     * %   Copyright 1997-9 Matthew Brand, Mitsubishi Electric Research Labs
     * %   This is a beta version, NOT FOR DISTRIBUTION.
     * %
     * %   [THETA, LOGLIKE] = ENTROPIC_MAP(OMEGA, Z, KAPPA, BIAS) uses a fixpoint derived
     * %   from the Lagrangian of the log-likelihood to compute theta given omega.  The
     * %   inverse of the fixpoint is used to estimate theta, theta is normalized, and
     * %   then the fixpoint equation is used to see how far we are from the solution.
     * %   Convergence usually takes just a few iterations.  The output arguments
     * %   returned are the optimal theta vector and the final log-likelihood value
     * %
     * %       loglike = sum(  (omega+bias) .* log(theta)
     * %                       + z .* theta .* log(theta)
     * %                       + theta .* kappa)
     * %
     * %   theta -- multinomial parameters
     * %   omega -- evidence vector (default: 1)
     * %   z     -- exponent on prior (default: 1)
     * %   kappa -- auxiliary entropy (default: 0)
     * %   bias  -- bias on the evidence (default: 0)
     * %            e.g., Dirichlet exponents or Jeffrey's prior (bias=-1/2)
     * %
     * %   Warning: this is not numerically stable for sum(omega) < 1 or large z > 0.
     * %
     * %   $Id: entropic_map.c,v 1.1.1.1 2002/03/05 12:53:42 karol2000 Exp $
     * 
     * % Initialize and check arguments
     * error(nargchk(1, 4, nargin));
     */
    mlfError(mlfNargchk(mlfScalar(1.0), mlfScalar(4.0), nargin_));
    /*
     * if any(omega < 0), 
     */
    if (mlfTobool(mlfAny(mlfLt(omega, mlfScalar(0.0)), NULL))) {
        /*
         * warning('entropic_map: ignoring negative evidence values'); 
         */
        mclAssignAns(
          &ans,
          mlfWarning(
            NULL,
            mxCreateString("entropic_map: ignoring negative evidence values")));
        /*
         * omega=max(0,omega);
         */
        mlfAssign(&omega, mlfMax(NULL, mlfScalar(0.0), omega, NULL));
    /*
     * end;
     */
    }
    /*
     * if nargin < 2 | isempty(z),     z = 1;     end;
     */
    {
        mxArray * a_ = mclInitialize(mlfLt(nargin_, mlfScalar(2.0)));
        if (mlfTobool(a_) || mlfTobool(mlfOr(a_, mlfIsempty(z)))) {
            mxDestroyArray(a_);
            mlfAssign(&z, mlfScalar(1.0));
        } else {
            mxDestroyArray(a_);
        }
    }
    /*
     * if nargin < 3 | isempty(kappa), kappa = 0; end;
     */
    {
        mxArray * a_ = mclInitialize(mlfLt(nargin_, mlfScalar(3.0)));
        if (mlfTobool(a_) || mlfTobool(mlfOr(a_, mlfIsempty(kappa)))) {
            mxDestroyArray(a_);
            mlfAssign(&kappa, mlfScalar(0.0));
        } else {
            mxDestroyArray(a_);
        }
    }
    /*
     * if nargin < 4 | isempty(bias),  
     */
    {
        mxArray * a_ = mclInitialize(mlfLt(nargin_, mlfScalar(4.0)));
        if (mlfTobool(a_) || mlfTobool(mlfOr(a_, mlfIsempty(bias)))) {
            mxDestroyArray(a_);
            /*
             * bias = 0;  
             */
            mlfAssign(&bias, mlfScalar(0.0));
        /*
         * else 
         */
        } else {
            mxDestroyArray(a_);
            /*
             * omega = max(0,omega + bias); 
             */
            mlfAssign(
              &omega, mlfMax(NULL, mlfScalar(0.0), mlfPlus(omega, bias), NULL));
            /*
             * if all(0==omega), theta=omega+1./length(omega); return; end;
             */
            if (mlfTobool(mlfAll(mlfEq(mlfScalar(0.0), omega), NULL))) {
                mlfAssign(
                  &theta,
                  mlfPlus(omega, mlfRdivide(mlfScalar(1.0), mlfLength(omega))));
                goto return_;
            }
        }
    /*
     * end;
     */
    }
    /*
     * 
     * 
     * % Constants
     * persistent CONSTANTS_INITIALIZED
     * persistent PHI NPHI REALMAX REALMIN
     * if isempty(CONSTANTS_INITIALIZED),
     */
    if (mlfTobool(
          mlfIsempty(_persistent_entropic_map__CONSTANTS_INITIALIZED))) {
        /*
         * CONSTANTS_INITIALIZED = 1;
         */
        mlfAssign(
          &_persistent_entropic_map__CONSTANTS_INITIALIZED, mlfScalar(1.0));
        /*
         * PHI     = (sqrt(5)-1)./2;
         */
        mlfAssign(
          &_persistent_entropic_map__PHI,
          mlfRdivide(
            mlfMinus(mlfSqrt(mlfScalar(5.0)), mlfScalar(1.0)), mlfScalar(2.0)));
        /*
         * NPHI    = 1-PHI;
         */
        mlfAssign(
          &_persistent_entropic_map__NPHI,
          mlfMinus(mlfScalar(1.0), _persistent_entropic_map__PHI));
        /*
         * REALMAX = realmax;
         */
        mlfAssign(&_persistent_entropic_map__REALMAX, mlfRealmax());
        /*
         * REALMIN = realmin;
         */
        mlfAssign(&_persistent_entropic_map__REALMIN, mlfRealmin());
    /*
     * end;
     */
    }
    /*
     * 
     * % Special case: heat death
     * if z >= REALMAX,			% construct uniform distribution
     */
    if (mlfTobool(mlfGe(z, _persistent_entropic_map__REALMAX))) {
        /*
         * theta = omega > 0; 
         */
        mlfAssign(&theta, mlfGt(omega, mlfScalar(0.0)));
        /*
         * q = sum(theta);
         */
        mlfAssign(&q, mlfSum(theta, NULL));
        /*
         * if q > 0, 
         */
        if (mlfTobool(mlfGt(q, mlfScalar(0.0)))) {
            /*
             * theta = theta./q;		
             */
            mlfAssign(&theta, mlfRdivide(theta, q));
        /*
         * else 
         */
        } else {
            /*
             * q = length(omega); theta = ones(size(omega))./q; 
             */
            mlfAssign(&q, mlfLength(omega));
            mlfAssign(
              &theta,
              mlfRdivide(
                mlfOnes(mlfSize(mclValueVarargout(), omega, NULL), NULL), q));
        /*
         * end;
         */
        }
        /*
         * loglike = z.*log(q);			% posterior dominated by prior
         */
        mlfAssign(loglike, mlfTimes(z, mlfLog(q)));
        /*
         * return;
         */
        goto return_;
    /*
     * end;
     */
    }
    /*
     * 
     * % Special case: zero exponent and zero kappa
     * if ~any(z) & ~any(kappa),
     */
    {
        mxArray * a_ = mclInitialize(mlfNot(mlfAny(z, NULL)));
        if (mlfTobool(a_)
            && mlfTobool(mlfAnd(a_, mlfNot(mlfAny(kappa, NULL))))) {
            mxDestroyArray(a_);
            /*
             * theta = stoch(omega);
             */
            mlfAssign(&theta, mlfStoch(omega));
            /*
             * if nargout >= 2, loglike = sum(omega .* log(theta)); end;
             */
            if (mlfTobool(mlfGe(nargout, mlfScalar(2.0)))) {
                mlfAssign(
                  loglike, mlfSum(mlfTimes(omega, mlfLog(theta)), NULL));
            }
            /*
             * return;
             */
            goto return_;
        } else {
            mxDestroyArray(a_);
        }
    /*
     * end;
     */
    }
    /*
     * 
     * % Check for values in omega that are very close to zero
     * % TODO: With z < 0 | any(kappa), we might want to handle zeros in omega diffly.
     * ok = omega > eps.^2;
     */
    mlfAssign(&ok, mlfGt(omega, mlfPower(mlfEps(), mlfScalar(2.0))));
    /*
     * if any(ok == 0),
     */
    if (mlfTobool(mlfAny(mlfEq(ok, mlfScalar(0.0)), NULL))) {
        /*
         * q = sum(ok);
         */
        mlfAssign(&q, mlfSum(ok, NULL));
        /*
         * if q > 1,
         */
        if (mlfTobool(mlfGt(q, mlfScalar(1.0)))) {
            /*
             * % Two or more nonzero parameters -- skip those that are zero
             * if length(kappa) > 1, kappa = kappa(ok); end;
             */
            if (mlfTobool(mlfGt(mlfLength(kappa), mlfScalar(1.0)))) {
                mlfAssign(&kappa, mlfIndexRef(kappa, "(?)", ok));
            }
            /*
             * if length(z    ) > 1, z     = z    (ok); end;
             */
            if (mlfTobool(mlfGt(mlfLength(z), mlfScalar(1.0)))) {
                mlfAssign(&z, mlfIndexRef(z, "(?)", ok));
            }
            /*
             * [fix, loglike] = entropic_map(omega(ok), z, kappa);
             */
            mlfAssign(
              &fix,
              mlfNEntropic_map(
                2, loglike, mlfIndexRef(omega, "(?)", ok), z, kappa, NULL));
            /*
             * theta = ok; theta(ok) = stoch(fix); % TODO: why stoch? isn't it redundant?
             */
            mlfAssign(&theta, ok);
            mlfIndexAssign(&theta, "(?)", ok, mlfStoch(fix));
        /*
         * elseif q == 1,
         */
        } else if (mlfTobool(mlfEq(q, mlfScalar(1.0)))) {
            /*
             * % Only one nonzero parameter -- return spike distribution
             * theta = ok;
             */
            mlfAssign(&theta, ok);
            /*
             * loglike = 0;
             */
            mlfAssign(loglike, mlfScalar(0.0));
        /*
         * else
         */
        } else {
            /*
             * % Everything is zero -- return uniform distribution
             * theta = repmat(1/prod(size(omega)), size(omega));
             */
            mlfAssign(
              &theta,
              mlfRepmat(
                mlfMrdivide(
                  mlfScalar(1.0),
                  mlfProd(mlfSize(mclValueVarargout(), omega, NULL), NULL)),
                mlfSize(mclValueVarargout(), omega, NULL),
                NULL));
            /*
             * loglike = 0;
             */
            mlfAssign(loglike, mlfScalar(0.0));
        /*
         * end;
         */
        }
        /*
         * return
         */
        goto return_;
    /*
     * end;
     */
    }
    /*
     * 
     * % Trivial case -- only one parameter
     * if length(omega) == 1,
     */
    if (mlfTobool(mlfEq(mlfLength(omega), mlfScalar(1.0)))) {
        /*
         * theta = [1];
         */
        mlfAssign(&theta, mlfDoubleMatrix(1, 1, __Array0_r, NULL));
        /*
         * loglike = 1;
         */
        mlfAssign(loglike, mlfScalar(1.0));
        /*
         * return
         */
        goto return_;
    /*
     * end;
     */
    }
    /*
     * 
     * % Fixpoint loop
     * loglike         = -REALMAX;
     */
    mlfAssign(loglike, mlfUminus(_persistent_entropic_map__REALMAX));
    /*
     * dloglike        = 0;
     */
    mlfAssign(&dloglike, mlfScalar(0.0));
    /*
     * theta           = omega ./ sum(omega) + REALMIN;
     */
    mlfAssign(
      &theta,
      mlfPlus(
        mlfRdivide(omega, mlfSum(omega, NULL)),
        _persistent_entropic_map__REALMIN));
    /*
     * if z == 0,
     */
    if (mlfTobool(mlfEq(z, mlfScalar(0.0)))) {
        /*
         * meankappa     = mean(kappa);
         */
        mlfAssign(&meankappa, mlfMean(kappa, NULL));
        /*
         * minlambda     = max(kappa);
         */
        mlfAssign(&minlambda, mlfMax(NULL, kappa, NULL, NULL));
        /*
         * maxlambda     = +Inf;
         */
        mlfAssign(&maxlambda, mlfUplus(mlfInf()));
        /*
         * lambda        = sum(omega) + meankappa;
         */
        mlfAssign(&lambda, mlfPlus(mlfSum(omega, NULL), meankappa));
    /*
     * else
     */
    } else {
        /*
         * omegaz        = omega./z;
         */
        mlfAssign(&omegaz, mlfRdivide(omega, z));
        /*
         * kappaz        = kappa./z;
         */
        mlfAssign(&kappaz, mlfRdivide(kappa, z));
        /*
         * logomegaz_pkz = log(omega) - log(abs(z)) + kappaz;
         */
        mlfAssign(
          &logomegaz_pkz,
          mlfPlus(mlfMinus(mlfLog(omega), mlfLog(mlfAbs(z))), kappaz));
        /*
         * if z < 0,
         */
        if (mlfTobool(mlfLt(z, mlfScalar(0.0)))) {
            /*
             * minlambda   = max(logomegaz_pkz) - 700;
             */
            mlfAssign(
              &minlambda,
              mlfMinus(
                mlfMax(NULL, logomegaz_pkz, NULL, NULL), mlfScalar(700.0)));
        /*
         * else
         */
        } else {
            /*
             * % For z > 0, we need to restrict minlambda so that the argument to
             * % loglambertwn1 below stays within range
             * minlambda   = max(logomegaz_pkz) + 2;
             */
            mlfAssign(
              &minlambda,
              mlfPlus(mlfMax(NULL, logomegaz_pkz, NULL, NULL), mlfScalar(2.0)));
            /*
             * minlambda   = minlambda * (1 + eps * sign(minlambda));
             */
            mlfAssign(
              &minlambda,
              mlfMtimes(
                minlambda,
                mlfPlus(
                  mlfScalar(1.0), mlfMtimes(mlfEps(), mlfSign(minlambda)))));
        /*
         * end;
         */
        }
        /*
         * maxlambda     = min(logomegaz_pkz) + 700;
         */
        mlfAssign(
          &maxlambda,
          mlfPlus(mlfMin(NULL, logomegaz_pkz, NULL, NULL), mlfScalar(700.0)));
        /*
         * lambda        = sum(omegaz) + 1 + mean(kappaz) + max(log(theta));
         */
        mlfAssign(
          &lambda,
          mlfPlus(
            mlfPlus(
              mlfPlus(mlfSum(omegaz, NULL), mlfScalar(1.0)),
              mlfMean(kappaz, NULL)),
            mlfMax(NULL, mlfLog(theta), NULL, NULL)));
    /*
     * end;
     */
    }
    /*
     * lambda          = min(maxlambda, max(minlambda, lambda));
     */
    mlfAssign(
      &lambda,
      mlfMin(NULL, maxlambda, mlfMax(NULL, minlambda, lambda, NULL), NULL));
    /*
     * dlambda         = REALMAX;
     */
    mlfAssign(&dlambda, _persistent_entropic_map__REALMAX);
    /*
     * signz           = sign(z);
     */
    mlfAssign(&signz, mlfSign(z));
    /*
     * 
     * % Iterate fixpoint until numerical error intrudes.
     * if minlambda < maxlambda,
     */
    if (mlfTobool(mlfLt(minlambda, maxlambda))) {
        /*
         * while 1,
         */
        while (mlfTobool(mlfScalar(1.0))) {
            /*
             * 
             * % Store previous values
             * oldtheta    = theta;
             */
            mlfAssign(&oldtheta, theta);
            /*
             * oldloglike  = loglike;
             */
            mlfAssign(&oldloglike, *loglike);
            /*
             * olddloglike = dloglike;
             */
            mlfAssign(&olddloglike, dloglike);
            /*
             * olddlambda  = dlambda;
             */
            mlfAssign(&olddlambda, dlambda);
            /*
             * 
             * % Step theta (inverse fixpoint)
             * if z == 0,
             */
            if (mlfTobool(mlfEq(z, mlfScalar(0.0)))) {
                /*
                 * theta = max(omega ./ (lambda - kappa), 0);
                 */
                mlfAssign(
                  &theta,
                  mlfMax(
                    NULL,
                    mlfRdivide(omega, mlfMinus(lambda, kappa)),
                    mlfScalar(0.0),
                    NULL));
            /*
             * elseif z < 0,
             */
            } else if (mlfTobool(mlfLt(z, mlfScalar(0.0)))) {
                /*
                 * theta = max(omega ./ loglambertw0((lambda - 1) - logomegaz_pkz), 0);
                 */
                mlfAssign(
                  &theta,
                  mlfMax(
                    NULL,
                    mlfRdivide(
                      omega,
                      mlfLoglambertw0(
                        mlfMinus(
                          mlfMinus(lambda, mlfScalar(1.0)), logomegaz_pkz))),
                    mlfScalar(0.0),
                    NULL));
            /*
             * else
             */
            } else {
                /*
                 * theta = max(omega ./ loglambertwn1((lambda - 1) - logomegaz_pkz), 0);
                 */
                mlfAssign(
                  &theta,
                  mlfMax(
                    NULL,
                    mlfRdivide(
                      omega,
                      mlfLoglambertwn1(
                        mlfMinus(
                          mlfMinus(lambda, mlfScalar(1.0)), logomegaz_pkz))),
                    mlfScalar(0.0),
                    NULL));
            /*
             * end;
             */
            }
            /*
             * theta = theta ./ sum(theta) + REALMIN; % normalize
             */
            mlfAssign(
              &theta,
              mlfPlus(
                mlfRdivide(theta, mlfSum(theta, NULL)),
                _persistent_entropic_map__REALMIN));
            /*
             * logtheta = log(theta);
             */
            mlfAssign(&logtheta, mlfLog(theta));
            /*
             * 
             * % Compute new entropic MLE log-likelihood
             * loglike = sum(omega .* logtheta + z .* theta .* logtheta + theta .* kappa);
             */
            mlfAssign(
              loglike,
              mlfSum(
                mlfPlus(
                  mlfPlus(
                    mlfTimes(omega, logtheta),
                    mlfTimes(mlfTimes(z, theta), logtheta)),
                  mlfTimes(theta, kappa)),
                NULL));
            /*
             * dloglike = loglike - oldloglike;
             */
            mlfAssign(&dloglike, mlfMinus(*loglike, oldloglike));
            /*
             * 
             * % Compare and save
             * if dloglike == 0,
             */
            if (mlfTobool(mlfEq(dloglike, mlfScalar(0.0)))) {
                /*
                 * if signz ~= sign(dlambda), break; end;
                 */
                if (mlfTobool(mlfNe(signz, mlfSign(dlambda)))) {
                    break;
                }
                /*
                 * % Back up half a step
                 * theta   = oldtheta;
                 */
                mlfAssign(&theta, oldtheta);
                /*
                 * loglike = oldloglike;
                 */
                mlfAssign(loglike, oldloglike);
                /*
                 * dlambda = dlambda ./ 2;
                 */
                mlfAssign(&dlambda, mlfRdivide(dlambda, mlfScalar(2.0)));
                /*
                 * lambda  = lambda - dlambda;
                 */
                mlfAssign(&lambda, mlfMinus(lambda, dlambda));
            /*
             * elseif dloglike < 0,
             */
            } else if (mlfTobool(mlfLt(dloglike, mlfScalar(0.0)))) {
                /*
                 * % Golden mean
                 * theta = oldtheta;
                 */
                mlfAssign(&theta, oldtheta);
                /*
                 * if olddloglike + dloglike <= 0,
                 */
                if (mlfTobool(
                      mlfLe(mlfPlus(olddloglike, dloglike), mlfScalar(0.0)))) {
                    /*
                     * loglike = oldloglike;
                     */
                    mlfAssign(loglike, oldloglike);
                    /*
                     * logtheta = log(theta);
                     */
                    mlfAssign(&logtheta, mlfLog(theta));
                    /*
                     * break;
                     */
                    break;
                /*
                 * end;
                 */
                }
                /*
                 * loglike = oldloglike;
                 */
                mlfAssign(loglike, oldloglike);
                /*
                 * lambda = NPHI .* lambda + PHI .* oldlambda;
                 */
                mlfAssign(
                  &lambda,
                  mlfPlus(
                    mlfTimes(_persistent_entropic_map__NPHI, lambda),
                    mlfTimes(_persistent_entropic_map__PHI, oldlambda)));
                /*
                 * dlambda = lambda - oldlambda;
                 */
                mlfAssign(&dlambda, mlfMinus(lambda, oldlambda));
                /*
                 * olddlambda = REALMAX;
                 */
                mlfAssign(&olddlambda, _persistent_entropic_map__REALMAX);
            /*
             * else
             */
            } else {
                /*
                 * % Improvement
                 * oldlambda = lambda;
                 */
                mlfAssign(&oldlambda, lambda);
                /*
                 * if z == 0,
                 */
                if (mlfTobool(mlfEq(z, mlfScalar(0.0)))) {
                    /*
                     * lambda = mean(omega./theta) + meankappa;
                     */
                    mlfAssign(
                      &lambda,
                      mlfPlus(
                        mlfMean(mlfRdivide(omega, theta), NULL), meankappa));
                /*
                 * else
                 */
                } else {
                    /*
                     * lambda = 1 + mean(omegaz./theta) + mean(logtheta);
                     */
                    mlfAssign(
                      &lambda,
                      mlfPlus(
                        mlfPlus(
                          mlfScalar(1.0),
                          mlfMean(mlfRdivide(omegaz, theta), NULL)),
                        mlfMean(logtheta, NULL)));
                /*
                 * end;
                 */
                }
                /*
                 * lambda = min(maxlambda, max(minlambda, lambda));
                 */
                mlfAssign(
                  &lambda,
                  mlfMin(
                    NULL,
                    maxlambda,
                    mlfMax(NULL, minlambda, lambda, NULL),
                    NULL));
                /*
                 * dlambda = lambda - oldlambda;
                 */
                mlfAssign(&dlambda, mlfMinus(lambda, oldlambda));
            /*
             * end;
             */
            }
        /*
         * end;
         */
        }
    /*
     * 
     * else
     */
    } else {
        /*
         * % The range of logomegaz_pkz seems totally out of whack -- what the heck,
         * % let's just skip the fixpoint loop
         * logtheta = log(theta);
         */
        mlfAssign(&logtheta, mlfLog(theta));
    /*
     * end;
     */
    }
    /*
     * 
     * % Very close now; polish up with 2nd order Newton-Raphson with bisection.
     * nm1 = length(theta) - 1;        % n-1
     */
    mlfAssign(&nm1, mlfMinus(mlfLength(theta), mlfScalar(1.0)));
    /*
     * nonm1 = 1 + 1/nm1;              % n/(n-1)
     */
    mlfAssign(
      &nonm1, mlfPlus(mlfScalar(1.0), mlfMrdivide(mlfScalar(1.0), nm1)));
    /*
     * loglike = sum(omega .* logtheta + z .* theta .* logtheta + theta .* kappa);
     */
    mlfAssign(
      loglike,
      mlfSum(
        mlfPlus(
          mlfPlus(
            mlfTimes(omega, logtheta), mlfTimes(mlfTimes(z, theta), logtheta)),
          mlfTimes(theta, kappa)),
        NULL));
    /*
     * while 1,
     */
    while (mlfTobool(mlfScalar(1.0))) {
        /*
         * 
         * % Store previous values
         * oldloglike = loglike;
         */
        mlfAssign(&oldloglike, *loglike);
        /*
         * oldtheta = theta;
         */
        mlfAssign(&oldtheta, theta);
        /*
         * 
         * % Function we want root of
         * ratio = omega ./ theta;
         */
        mlfAssign(&ratio, mlfRdivide(omega, theta));
        /*
         * dtheta = ratio + z .* logtheta + kappa;
         */
        mlfAssign(
          &dtheta, mlfPlus(mlfPlus(ratio, mlfTimes(z, logtheta)), kappa));
        /*
         * dtheta = dtheta - mean(dtheta);
         */
        mlfAssign(&dtheta, mlfMinus(dtheta, mlfMean(dtheta, NULL)));
        /*
         * ddtheta = (z - ratio) ./ theta; % 1st derivative of dtheta
         */
        mlfAssign(&ddtheta, mlfRdivide(mlfMinus(z, ratio), theta));
        /*
         * 
         * % 1st order Newton-Raphson via Jacobian
         * jacobian = vadd(-ddtheta ./ nm1, diag(ddtheta .* nonm1));
         */
        mlfAssign(
          &jacobian,
          mlfVadd(
            mlfRdivide(mlfUminus(ddtheta), nm1),
            mlfDiag(mlfTimes(ddtheta, nonm1), NULL)));
        /*
         * [l,u] = lu(jacobian);
         */
        mlfAssign(&l, mlfLu(&u, NULL, jacobian, NULL));
        /*
         * if rcond(u) > 0,
         */
        if (mlfTobool(mlfGt(mlfRcond(u), mlfScalar(0.0)))) {
            /*
             * w = warning; warning off; delta=-(u\(l\(-dtheta')))'; warning(w);
             */
            mlfAssign(&w, mlfWarning(NULL, NULL));
            mclAssignAns(&ans, mlfWarning(NULL, mxCreateString("off")));
            mlfAssign(
              &delta,
              mlfUminus(
                mlfCtranspose(
                  mlfMldivide(
                    u, mlfMldivide(l, mlfUminus(mlfCtranspose(dtheta)))))));
            mclAssignAns(&ans, mlfWarning(NULL, w));
        /*
         * else
         */
        } else {
            /*
             * dddtheta = (2.*ratio - z) ./ (theta.^2); % 2nd derivative of dtheta
             */
            mlfAssign(
              &dddtheta,
              mlfRdivide(
                mlfMinus(mlfTimes(mlfScalar(2.0), ratio), z),
                mlfPower(theta, mlfScalar(2.0))));
            /*
             * factor = ddtheta.^2 - 2.*dtheta.*dddtheta;
             */
            mlfAssign(
              &factor,
              mlfMinus(
                mlfPower(ddtheta, mlfScalar(2.0)),
                mlfTimes(mlfTimes(mlfScalar(2.0), dtheta), dddtheta)));
            /*
             * if factor >= 0,
             */
            if (mlfTobool(mlfGe(factor, mlfScalar(0.0)))) {
                /*
                 * delta = (ddtheta + sqrt(factor)) ./ dddtheta; % 2nd order Newton-Raphson
                 */
                mlfAssign(
                  &delta,
                  mlfRdivide(mlfPlus(ddtheta, mlfSqrt(factor)), dddtheta));
                /*
                 * if delta > theta | sum(abs(delta)) == 0,
                 */
                {
                    mxArray * a_ = mclInitialize(mlfGt(delta, theta));
                    if (mlfTobool(a_)
                        || mlfTobool(
                             mlfOr(
                               a_,
                               mlfEq(
                                 mlfSum(mlfAbs(delta), NULL),
                                 mlfScalar(0.0))))) {
                        mxDestroyArray(a_);
                        /*
                         * delta = (ddtheta - sqrt(factor)) ./ dddtheta; % 2nd order Newton-Raphson
                         */
                        mlfAssign(
                          &delta,
                          mlfRdivide(
                            mlfMinus(ddtheta, mlfSqrt(factor)), dddtheta));
                    } else {
                        mxDestroyArray(a_);
                    }
                /*
                 * end;
                 */
                }
                /*
                 * if delta > theta | sum(abs(delta)) == 0,
                 */
                {
                    mxArray * a_ = mclInitialize(mlfGt(delta, theta));
                    if (mlfTobool(a_)
                        || mlfTobool(
                             mlfOr(
                               a_,
                               mlfEq(
                                 mlfSum(mlfAbs(delta), NULL),
                                 mlfScalar(0.0))))) {
                        mxDestroyArray(a_);
                        /*
                         * ddtheta(ddtheta == 0) = REALMAX;
                         */
                        mlfIndexAssign(
                          &ddtheta,
                          "(?)",
                          mlfEq(ddtheta, mlfScalar(0.0)),
                          _persistent_entropic_map__REALMAX);
                        /*
                         * delta = dtheta ./ ddtheta; % 1st order Newton-Raphson
                         */
                        mlfAssign(&delta, mlfRdivide(dtheta, ddtheta));
                    } else {
                        mxDestroyArray(a_);
                    }
                /*
                 * end;
                 */
                }
            /*
             * else
             */
            } else {
                /*
                 * ddtheta(ddtheta == 0) = REALMAX;
                 */
                mlfIndexAssign(
                  &ddtheta,
                  "(?)",
                  mlfEq(ddtheta, mlfScalar(0.0)),
                  _persistent_entropic_map__REALMAX);
                /*
                 * delta = dtheta ./ ddtheta; % 1st order Newton-Raphson
                 */
                mlfAssign(&delta, mlfRdivide(dtheta, ddtheta));
            /*
             * end;
             */
            }
        /*
         * end;
         */
        }
        /*
         * 
         * % If (omitted) higher-order terms are significant, must scale down delta
         * [problem, where] = max(delta ./ theta);
         */
        mlfAssign(
          &problem, mlfMax(&where, mlfRdivide(delta, theta), NULL, NULL));
        /*
         * if problem > 1, delta = delta .* (NPHI ./ problem); end;
         */
        if (mlfTobool(mlfGt(problem, mlfScalar(1.0)))) {
            mlfAssign(
              &delta,
              mlfTimes(
                delta, mlfRdivide(_persistent_entropic_map__NPHI, problem)));
        }
        /*
         * theta = max(theta - delta, 0);
         */
        mlfAssign(
          &theta, mlfMax(NULL, mlfMinus(theta, delta), mlfScalar(0.0), NULL));
        /*
         * theta = theta ./ sum(theta) + REALMIN;
         */
        mlfAssign(
          &theta,
          mlfPlus(
            mlfRdivide(theta, mlfSum(theta, NULL)),
            _persistent_entropic_map__REALMIN));
        /*
         * logtheta = log(theta);
         */
        mlfAssign(&logtheta, mlfLog(theta));
        /*
         * loglike = sum(omega .* logtheta + z .* theta .* logtheta + theta .* kappa);
         */
        mlfAssign(
          loglike,
          mlfSum(
            mlfPlus(
              mlfPlus(
                mlfTimes(omega, logtheta),
                mlfTimes(mlfTimes(z, theta), logtheta)),
              mlfTimes(theta, kappa)),
            NULL));
        /*
         * if loglike <= oldloglike,
         */
        if (mlfTobool(mlfLe(*loglike, oldloglike))) {
            /*
             * theta = theta .* NPHI + oldtheta .* PHI;
             */
            mlfAssign(
              &theta,
              mlfPlus(
                mlfTimes(theta, _persistent_entropic_map__NPHI),
                mlfTimes(oldtheta, _persistent_entropic_map__PHI)));
            /*
             * logtheta = log(theta);
             */
            mlfAssign(&logtheta, mlfLog(theta));
            /*
             * loglike = sum(omega .* logtheta + z .* theta .* logtheta + theta .* kappa);
             */
            mlfAssign(
              loglike,
              mlfSum(
                mlfPlus(
                  mlfPlus(
                    mlfTimes(omega, logtheta),
                    mlfTimes(mlfTimes(z, theta), logtheta)),
                  mlfTimes(theta, kappa)),
                NULL));
            /*
             * if loglike <= oldloglike,
             */
            if (mlfTobool(mlfLe(*loglike, oldloglike))) {
                /*
                 * theta = oldtheta;
                 */
                mlfAssign(&theta, oldtheta);
                /*
                 * loglike = oldloglike;
                 */
                mlfAssign(loglike, oldloglike);
                /*
                 * break;
                 */
                break;
            /*
             * end;
             */
            }
        /*
         * end;
         */
        }
    /*
     * end;
     */
    }
    return_:
    mclValidateOutputs("entropic_map", 2, nargout_, &theta, loglike);
    mxDestroyArray(ans);
    mxDestroyArray(bias);
    mxDestroyArray(dddtheta);
    mxDestroyArray(ddtheta);
    mxDestroyArray(delta);
    mxDestroyArray(dlambda);
    mxDestroyArray(dloglike);
    mxDestroyArray(dtheta);
    mxDestroyArray(factor);
    mxDestroyArray(fix);
    mxDestroyArray(jacobian);
    mxDestroyArray(kappa);
    mxDestroyArray(kappaz);
    mxDestroyArray(l);
    mxDestroyArray(lambda);
    mxDestroyArray(logomegaz_pkz);
    mxDestroyArray(logtheta);
    mxDestroyArray(maxlambda);
    mxDestroyArray(meankappa);
    mxDestroyArray(minlambda);
    mxDestroyArray(nargin_);
    mxDestroyArray(nargout);
    mxDestroyArray(nm1);
    mxDestroyArray(nonm1);
    mxDestroyArray(ok);
    mxDestroyArray(olddlambda);
    mxDestroyArray(olddloglike);
    mxDestroyArray(oldlambda);
    mxDestroyArray(oldloglike);
    mxDestroyArray(oldtheta);
    mxDestroyArray(omega);
    mxDestroyArray(omegaz);
    mxDestroyArray(problem);
    mxDestroyArray(q);
    mxDestroyArray(ratio);
    mxDestroyArray(signz);
    mxDestroyArray(u);
    mxDestroyArray(w);
    mxDestroyArray(where);
    mxDestroyArray(z);
    return theta;
}

/*
 * The function "mlfNEntropic_map" contains the nargout interface for the
 * "entropic_map" M-function from file
 * "/projects/puppets/matlab/hmmtrim/entropic_map.m" (lines 1-255). This
 * interface is only produced if the M-function uses the special variable
 * "nargout". The nargout interface allows the number of requested outputs to
 * be specified via the nargout argument, as opposed to the normal interface
 * which dynamically calculates the number of outputs based on the number of
 * non-NULL inputs it receives. This function processes any input arguments and
 * passes them to the implementation version of the function, appearing above.
 */
mxArray * mlfNEntropic_map(int nargout,
                           mxArray * * loglike,
                           mxArray * omega,
                           mxArray * z,
                           mxArray * kappa,
                           mxArray * bias) {
    mxArray * theta = mclGetUninitializedArray();
    mxArray * loglike__ = mclGetUninitializedArray();
    mlfEnterNewContext(1, 4, loglike, omega, z, kappa, bias);
    theta = Mentropic_map(&loglike__, nargout, omega, z, kappa, bias);
    mlfRestorePreviousContext(1, 4, loglike, omega, z, kappa, bias);
    if (loglike != NULL) {
        mclCopyOutputArg(loglike, loglike__);
    } else {
        mxDestroyArray(loglike__);
    }
    return mlfReturnValue(theta);
}

/*
 * The function "mlfEntropic_map" contains the normal interface for the
 * "entropic_map" M-function from file
 * "/projects/puppets/matlab/hmmtrim/entropic_map.m" (lines 1-255). This
 * function processes any input arguments and passes them to the implementation
 * version of the function, appearing above.
 */
mxArray * mlfEntropic_map(mxArray * * loglike,
                          mxArray * omega,
                          mxArray * z,
                          mxArray * kappa,
                          mxArray * bias) {
    int nargout = 1;
    mxArray * theta = mclGetUninitializedArray();
    mxArray * loglike__ = mclGetUninitializedArray();
    mlfEnterNewContext(1, 4, loglike, omega, z, kappa, bias);
    if (loglike != NULL) {
        ++nargout;
    }
    theta = Mentropic_map(&loglike__, nargout, omega, z, kappa, bias);
    mlfRestorePreviousContext(1, 4, loglike, omega, z, kappa, bias);
    if (loglike != NULL) {
        mclCopyOutputArg(loglike, loglike__);
    } else {
        mxDestroyArray(loglike__);
    }
    return mlfReturnValue(theta);
}

/*
 * The function "mlfVEntropic_map" contains the void interface for the
 * "entropic_map" M-function from file
 * "/projects/puppets/matlab/hmmtrim/entropic_map.m" (lines 1-255). The void
 * interface is only produced if the M-function uses the special variable
 * "nargout", and has at least one output. The void interface function
 * specifies zero output arguments to the implementation version of the
 * function, and in the event that the implementation version still returns an
 * output (which, in MATLAB, would be assigned to the "ans" variable), it
 * deallocates the output. This function processes any input arguments and
 * passes them to the implementation version of the function, appearing above.
 */
void mlfVEntropic_map(mxArray * omega,
                      mxArray * z,
                      mxArray * kappa,
                      mxArray * bias) {
    mxArray * theta = mclUnassigned();
    mxArray * loglike = mclUnassigned();
    mlfEnterNewContext(0, 4, omega, z, kappa, bias);
    theta = Mentropic_map(&loglike, 0, omega, z, kappa, bias);
    mlfRestorePreviousContext(0, 4, omega, z, kappa, bias);
    mxDestroyArray(theta);
    mxDestroyArray(loglike);
}

/*
 * The function "mlxEntropic_map" contains the feval interface for the
 * "entropic_map" M-function from file
 * "/projects/puppets/matlab/hmmtrim/entropic_map.m" (lines 1-255). The feval
 * function calls the implementation version of entropic_map through this
 * function. This function processes any input arguments and passes them to the
 * implementation version of the function, appearing above.
 */
void mlxEntropic_map(int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]) {
    mxArray * mprhs[4];
    mxArray * mplhs[2];
    int i;
    if (nlhs > 2) {
        mlfError(
          mxCreateString(
            "Run-time Error: File: entropic_map Line: 1 Column"
            ": 0 The function \"entropic_map\" was called with"
            " more than the declared number of outputs (2)"));
    }
    if (nrhs > 4) {
        mlfError(
          mxCreateString(
            "Run-time Error: File: entropic_map Line: 1 Column"
            ": 0 The function \"entropic_map\" was called with"
            " more than the declared number of inputs (4)"));
    }
    for (i = 0; i < 2; ++i) {
        mplhs[i] = NULL;
    }
    for (i = 0; i < 4 && i < nrhs; ++i) {
        mprhs[i] = prhs[i];
    }
    for (; i < 4; ++i) {
        mprhs[i] = NULL;
    }
    mlfEnterNewContext(0, 4, mprhs[0], mprhs[1], mprhs[2], mprhs[3]);
    mplhs[0]
      = Mentropic_map(&mplhs[1], nlhs, mprhs[0], mprhs[1], mprhs[2], mprhs[3]);
    mlfRestorePreviousContext(0, 4, mprhs[0], mprhs[1], mprhs[2], mprhs[3]);
    plhs[0] = mplhs[0];
    for (i = 1; i < 2 && i < nlhs; ++i) {
        plhs[i] = mplhs[i];
    }
    for (; i < 2; ++i) {
        mxDestroyArray(mplhs[i]);
    }
}
