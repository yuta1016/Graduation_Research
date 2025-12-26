/*-------------- Telecom ht munications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdecLlimits(const char String[], long int Lim[2], int Mode)

Purpose:
  Decode a pair of limits for long integer values

Description:
  This routine decodes a string specifying a pair of long integer values. The
  limits are specified in the form "n" or "n:m", for example "-23 : 45".
  Optional white-space (as defined by isspace) can surround the values. Default
  values for the limits can be given.

  The table below shows how the processing carried out in the different modes.
  The initial values of Lim[0]:Lim[1] are indicated across the top. Table
  entries marked with an X are not allowed and generate an error.
                        Mode
              0       1       2       3
    input    x:x     x:x     L:U     L:U
    -------------------------------------
    "   "     X       X       X       X
    "n  "     X      n:n    L:L+n-1   X
    " : "     X       X       X       X
    "n: "     X       X      n:U     n:U
    " :m"     X       X       X      L:m
    "n:m"    n:m     n:m     n:m     n:m

  Mode 0: Only an input of the form n:m is allowed.

  Mode 1: The special case of an input of the form "n" (no colon or second
  value) is interpreted to be "n:n".

  Mode 2: The special case of an input of the form "n" (no colon or second
  value) is interpreted as L:L+n-1, where L is the initial value of Lim[0]. An
  input of the form "n:" is interpreted as "n:U" where U is the initial value of
  Lim[1]. In practice U can be a special
  value that indicates that the upper limit is yet to be determined.

  Mode 3: The initial values of Lim are used. An input of the form "n:" is
  interpreted as "n:U" where U is the initial value of Lim[1]. An input of the
  form ":m" is interpreted as "L:m". Both L and U can be a special value that
  indicates that these limits have yet to be determined.

  In all modes, an input of the form "n:m" ignores the initIal values set in
  Lim.

Parameters:
  <-  int STdecLimits
        0 - No error
        1 - Error. An error message is printed either by this routine or by
            STdec1val.
   -> const char String[]
      Input string
  <-> long int Lim[2]
      Initial limits modified by the decoded limits. If an error is detected,
      the initial limits are not changed.
   -> int Mode
      This flag determines if and how the initial values of Lim[] are used to
      determine the output values.

Author / revision:
  P. Kabal
  $Revision: 1.12 $  $Date: 2023/04/04 19:53:27 $

-------------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/STmsg.h>

#define MAXC  22

#define DP_BADV  -2
#define DP_BADF  -1
#define DP_EMPTY  0
#define DP_LVAL   1
#define DP_DELIM  2
#define DP_RVAL   4

#define LIM_NM        0   /* "n:m"                           */
#define LIM_NNxNM     1   /* "n" -> "n:n"                    */
#define LIM_LNxNUxNM  2   /* "n" -> "L:L+n-1"; "n:" -> "n:U" */
#define LIM_NUxLMxNM  3   /* "n:" -> "n:U"; ":m" -> "L:m"    */

#define NCOLUMN(array)  ((int) ((sizeof array) / (sizeof array[0][0])))

/*                              Mode
                      0       1       2       3
    status  input    x:x     x:x     L:U     L:U
    ----------------------------------------------
       0    "   "     X       X       X       X
       1    "n  "     X      n:n   L:L+n-1    X
       2    " : "     X       X       X       X
       3    "n: "     X       X      n:U     n:U
       4              X       X       X       X
       5              X       X       X       X
       6    " :m"     X       X       X      L:m
       7    "n:m"    n:m     n:m     n:m     n:m

    status: DP_LVAL + DP_DELIM + DP_RVAL
    X codes to 0, normal cases to 1, special cases to 2 and 3
*/

/* Table of allowed cases                 0    +     1   +     2    +     4   */
static int Tallow[8][4] = /* status = DP_EMPTY + DP_LVAL + DP_DELIM + DP_RVAL */
        { {0, 0, 0, 0},   /* 0 -> DP_EMPTY */
          {0, 2, 3, 0},   /* 1 -> DP_LVAL  */
          {0, 0, 0, 0},   /* 2 -> DP_DLIM  */
          {0, 0, 1, 1},   /* 3 -> DP_LVAL + DP_DELIM */
          {0, 0, 0, 0},   /* 4 -> DP_RVAL  */
          {0, 0, 0, 0},   /* 5 -> DP_LVAL + DP_RVAL  */
          {0, 0, 0, 1},   /* 6 -> DP_DELIM + DP_RVAL */
          {1, 1, 1, 1} }; /* 7 -> DP_LVAL + DP_DELIM + DP_RVAL */

#define SVCODE_0x0   0   /* no constraints */


int
STdecLlimits(const char String[], long int Lim[2], int Mode)

{
  int status, Allow;
  long int DLim[2];
  void *Vp[2] = {(void *) &DLim[0], (void *) &DLim[1]};

  /* Save the initial values of Lim */
  DLim[0] = Lim[0];
  DLim[1] = Lim[1];

  assert(Mode >= 0 && Mode <= 3);

  /* Decode the range values */
  status = STdecPair(String, ":", 'L', SVCODE_0x0, Vp);
  if (status < 0) {
    if (status != DP_BADV)
      UTwarn("STdecLlimits - %s: \"%s\"", STM_DataErr, STstrDots(String, MAXC));
    return 1;
  }

  /* Index into the allowed table */
  assert(status >= 0 && status < NCOLUMN(Tallow) / 3);
  Allow = Tallow[status][Mode];

  switch (Allow) {
    case 1:
      Lim[0] = DLim[0];
      Lim[1] = DLim[1];
      break;
    case 2:
      Lim[0] = DLim[0];
      Lim[1] = DLim[0];
      break;
    case 3:
      Lim[1] = Lim[0] + DLim[0] - 1;
      break;
    default:
      UTwarn("STdecLlimits - %s: \"%s\"", STM_DataErr, STstrDots(String, MAXC));
      return 1;
      break;
  }

  return 0;
}
