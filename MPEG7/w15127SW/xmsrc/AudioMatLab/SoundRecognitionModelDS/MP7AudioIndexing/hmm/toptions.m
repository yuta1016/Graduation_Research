function optsout = toptions(optsin)
%TOPTIONS Default option settings used by the entropic training routines.
%   The OPTIONS input argument to MIXTURE and HMM is a cell array or structure
%   argument specifying additional control parameters.  If OPTIONS is a cell
%   array, each pair of elements is treated as the name and value of an option.
%   If OPTIONS is a structure, each field is treated as an option specification.
%   If OPTIONS is an empty array, default values are used for all options;
%   otherwise, the option values specified override default values.
%
%   The 'epsilon' option is the smallest allowable gain in the posterior between
%   successive epochs before training is terminated, as a fraction of the gain
%   in the posterior between the first and current epochs.  The default is
%   0.0001.
%
%   The 'itmax' option is the maximum number of epochs allowed before training
%   is terminated.  It defaults to +Inf (i.e., no termination due to number of
%   epochs).
%
%   The 'z' option (a vector of length 2) and the 'y' option (a square matrix of
%   size 2) are constants the log posterior probability distribution function,
%   namely
%
%       post = (probability of observations given model)
%            - zz * (entropy of mixture prior probabilities [mixture]
%                    or state transition and starting probabilities [hmm])
%            - y1 * (total entropy of Gaussians [mixture or continuous hmm]
%                    or output probabilities [discrete hmm], _unscaled_ by
%                    mixture priors [mixture] or state occupancy [hmm])
%            - y2 * (total entropy of Gaussians [mixture or continuous hmm]
%                    or output probabilities [discrete hmm], _scaled_ by
%                    mixture priors [mixture] or state occupancy [hmm])
%            - temperature^2/2,
%
%   in which
%
%       zz = z(  1) + z(  2) * temperature,
%       y1 = y(1,1) + y(1,2) * temperature,
%       y2 = y(2,1) + y(2,2) * temperature,
%
%   and temperature is the deterministic annealing temperature.  Special cases
%   of the 'z' and 'y' options include:
%
%       (a) 'z' = [0 0], 'y' = [0 0; 0 0]: maximum likelihood.
%       (b) 'z' = [1 0], 'y' = [0 0; 0 0]: minimum entropy of mixture priors
%           [mixture] or state transitions [hmm].
%       (c) 'z' = [1 0], 'y' = [1 0; 0 0]: minimum entropy of model desciption.
%       (d) 'z' = [1 0], 'y' = [0 0; 1 0]: minimum entropy of prob distribution.
%       (e) 'z' = [barrier 1/sqrt(scale)], 'y' = [barrier 1/sqrt(scale); 0 0]:
%           similar to (c), plus annealing with a schedule determined from the
%           model description entropy by the driving function
%               log(delta(zz)) = -scale*(zz-barrier)^2/2 = -temperature^2/2.
%       (f) 'z' = [barrier 1/sqrt(scale)], 'y' = [barrier 0; 0 1/sqrt(scale)]:
%           similar to (e), but with an annealing schedule determined from the
%           probability distribution entropy instead.
%
%   The default for 'z' and 'y' is special case (c) above.
%
%   The 'zmax' option specifies a maximum value for zz.  If the annealing
%   temperature causes
%
%       zz = z(1) + z(2) * temperature
%
%   to rise above 'zmax', the temperature is clamped to fix zz at 'zmax'.  The
%   default for 'zmax' is 1; it can also be set to +Inf.
%
%   By default, the temperature is estimated every epoch to maximize the
%   posterior, just like every other parameter.  If the 'schedule' option is
%   specified to be a non-empty vector, it is used as a fixed annealing
%   schedule.  Each entry in 'schedule' is taken as the temperature for the
%   corresponding epoch.  When the epoch number exceeds the length of
%   'schedule', the temperature is set to the last entry in 'schedule'.
%
%   The 'trimperiod' option (a non-negative integer) and the 'trimratio' option
%   (a non-negative real number) determine whether and how often to trim.
%   Trimming is performed whenever one of two criteria is satisfied:  (1)
%   'trimperiod' is nonzero, and the current epoch is 'trimperiod' epochs since
%   the last epoch when trimming took place.  (2) 'trimratio' is nonzero, and
%   the posterior increase since the previous epoch is less than 'trimratio'
%   times the posterior increase since the last epoch when trimming took place.
%   Both of these options default to zero.
%
%   The 'trimepsilon' option (a non-negative number) restricts trimming to
%   parameter values (e.g., transition probabilities) that are smaller than or
%   equal to 'trimepsilon'.  The default is +Inf.  This option is currently only
%   implemented by HMM, for which it should probably be set to something like
%   1e-6.  For HMM, if 'trimepsilon' is set to a vector of length 2 or 3, the
%   entries of the vector are taken as separate 'trimepsilon' settings for
%   trimming starting probabilities, transition probabilities, and (for discrete
%   models only) output probabilities.
%
%   The 'trimcycles' option (a positive integer) is the maximum number of
%   trimming cycles in each epoch, i.e., the maximum number of times trimming
%   can be performed before moving on to the M-step.  The default is +Inf.
%
%   The 'status' option controls how status messages are printed:
%
%       1 for one line per epoch (the default),
%       0 for every epoch on the same line unless something special happens,
%       -1 for no status messages;
%
%   The 'plotting' boolean option determines whether the status of epoch should
%   be plotted.  The default is no.  If this option is set to true and the
%   global variable Movie is not empty, the status of each epoch is stored in
%   the Movie variable as consecutive movie frames.  This parameter is currently
%   only implemented by HMM.
%
%   The 'estoutput' boolean option determines whether the output parameters
%   (i.e., MEANS and COVARIANCES) are reestimated.  The default is yes.  This
%   option is currently only implemented by HMM.
%
%   The 'iccabort' boolean option determines whether the training procedure
%   should simply abort if it has repeatedly tried and failed to fix an
%   ill-conditioned covariance matrix.  The default is no.
%
%   The 'TtransitionsN', 'TstartsN', 'TmeansN', 'TmeansD', 'TcovariancesN'
%   options for HMM specify a transformation to be applied to the results of
%   forward-backward analysis before reestimating model parameters.  Each
%   transformation is specified as a square matrix whose size is the number of
%   HMM states; the corresponding variable is multiplied by the square matrix on
%   the left.
%
%   $Id: toptions.m,v 1.1.1.1 2002/03/05 12:53:34 karol2000 Exp $

if isempty(optsin), optsin = {}; end;

optsout = struct_params(optsin,         ...
    'epsilon',          0.0001,         ...
    'itmax',            +Inf,           ...
    'z',                [1 0],          ...
    'y',                [1 0; 0 0],     ...
    'zmax',             1,              ...
    'schedule',         [],             ...
    'trimperiod',       0,              ...
    'trimratio',        0,              ...
    'trimepsilon',      +Inf,           ...
    'trimcycles',       +Inf,           ...
    'status',           1,              ...
    'plotting',         0,              ...
    'estoutput',        1,              ...
    'iccabort',         0,              ...
    'starts_bias',      [],             ...
    'transitions_bias', [],             ...
    'discrete_bias',    [],             ...
    'TtransitionsN',    [],             ...
    'TstartsN',         [],             ...
    'TmeansN',          [],             ...
    'TmeansD',          [],             ...
    'TcovariancesN',    []);
