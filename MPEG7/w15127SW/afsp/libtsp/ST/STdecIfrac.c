/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdecIfrac(const char String[], int SVCode, int Ival[2])

Purpose:
  Decode a ratio specification of two integer values

Description:
  This routine decodes a string specifying a ratio of integer values. The ratio
  is specified in the form "V" or "V1/V2", for example "-23 / 45".  Optional
  white-space (as defined by isspace) can surround the values. For the case of a
  single value V, this is equivalent to the ratio "V/1". If an error is
  encountered, neither output value is set.

Parameters:
  <-  int STdecIfrac
        0 - No error
        1 - Error. An error message is printed either by this routine or by
            STdec1val.
   -> const char String[]
      Input string
   -> int SVCode
      This code determines whether an explicit -/+ sign or an implicit sign can
      appear in the first / second value and whether the values can be zero.
      See STdecPair for details. Some sample codes:
        SVCODE_0x0=0   V1 is unconstrained;
                       V2 is unconstrained
        SVCODE_0x6=6   V1 is unconstrained;
                       V2 must be non-negative without an explicit + sign
        SVCODE_2x6=38  V1 must be non-negative (possibly an explicit + sign);
                       V2 must be non-negative without an explicit + sign
        SVCODE_6x6=102 V1 must be non-negative without an explicit + sign;
                       V2 must be non-negative without an explicit + sign
        SVCODE_0x7=7   V1 is unconstrained;
                       V2 must be positive, without an explicit + sign
        SVCODE_2x7=39  V1 must be non-negative (possibly an explicit + sign);
                       V2 must be positive, without an explicit + sign
        SVCODE_6x7=103 V1 must be non-negative, without an explicit + sign;
                       V2 must be positive, without an explicit + sign
        SVCODE_3x7=55  V1 must be positive (possibly an explicit + sign);
                       V2 must be positive, without an explicit + sign
        SVCODE_7x7=119 V1 must be positive, without an explicit + sign;
                       V2 must be positive, without an explicit + sign
  <-  int Ival[2]
      Values for the first and second parts of the fraction. These values are
      unchanged in case of an error.

Author / revision:
  P. Kabal
  $Revision: 1.18 $  $Date: 2023/03/23 20:17:01 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/STmsg.h>

#define MAXC  23

#define DP_BADV  -2
#define DP_BADF  -1   /* Error message in STdecPair */
#define DP_EMPTY  0
#define DP_LVAL   1
#define DP_DELIM  2
#define DP_RVAL   4


int
STdecIfrac(const char String[], int SVCode, int Ival[2])

{
  int status;
  int Val[2];
  void *Vp[2] = {(void *) &Val[0], (void *) &Val[1]};

  /* Decode the ratio values */
  status = STdecPair(String, "/", 'I', SVCode, Vp);

  if (status == DP_LVAL) {
    Ival[0] = Val[0];
    Ival[1] = 1;
    status = 0;
  }
  else if (status == (DP_LVAL + DP_DELIM + DP_RVAL)) {
    Ival[0] = Val[0];
    Ival[1] = Val[1];
    status = 0;
  }
  else if (status != DP_BADF)
    status = DP_BADV;

  if (status < 0 && status != DP_BADF)
    UTwarn("STdecIfrac - %s: \"%s\"", STM_DataErr, STstrDots(String, MAXC));

  return (status < 0);
}
