/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdecPair(const char String[], const char Delim[], int Type, int SVCode,
                void *Vp[2])

Purpose:
  Decode a data specification for a pair of numeric values

Description:
  This routine decodes a string specifying a pair of integer values. The pair
  is specified as "V", "V1:", ":V2", or "V1:V2", for example "-23 : 45". The
  delimiter is shown as ":" in the example, but a general delimiter string can
  be specified in the argument list. Optional white-space (as defined by
  isspace) can surround the values.

Parameters:
  <-  int STdecPair
      Return code
       -2 - Error in decoding a number, error message printed by routine
            STdec1val
       -1 - Error in value (wrong sign or zero) or format, no error message is
            issued
        0 - Empty or whitespace string (both Val1 and Val2 unchanged)
        1 - Left value (no delimiter) (only Val1 changed)
        2 - Delimiter alone (both Val1 and Val2 unchanged)
        3 - Left value + delimiter (only Val1 changed)
        6 - Delimiter + right value (only Val2 changed)
        7 - Left value + delimiter + right value (Val1 and Val2 changed)
   -> const char String[]
      Input string
   -> const char Delim[];
      String with delimiter characters. These delimiters are passed to function
      STfindToken. Any of the delimiter characters can separate the two values.
   -> int SVCode
      Data values can include an explicit Negative value (- sign), and explicit
      Positive value (+ sign), or an Implicit positive value (sign is absent).
      Denote the case with the binary values N, P, or I where a binary value of
      1 indicates that the N, P, or I are NOT allowed. The sign code for the
      first value V1 is a mask of the form
        SignCode1 = N1 + 2 P1 + 4 I2.
      Data values can be allowed to take on the value zero or can be constrained
      to be non-zero. A binary of value of 1 indicates that the value must be
      non-zero. The value code for the first value V1 is
        ValueCode1 = Z
      The sign code and the value code for V2 are constructed in a similar
      manner. The final code is
        SVCode = [Z2 + 2(N2 + 2 P2 + 4 I2)] + 16 [Z1 + 2(N1 + 2 P1 + 4 I1)]
      With no constraints on either V1 or V2, SVCode = 0.
      However some cases it is undesirable to have a zero value for the second
      value (V2), giving Z2 = 1. It may also be undesirable that V2 has an
      explicit positive or negative sign, giving N2 = 1 and P2 = 1.
      Some sample codes:
        SVCODE_0x0=0   V1 is unconstrained;
                       V2 is unconstrained
        SVCODE_0x6=6   V1 is unconstrained;
                       V2 must be non-negative without an explicit + sign
        SVCODE_0x7=7   V1 is unconstrained;
                       V2 must be positive, without an explicit + sign
        SVCODE_2x6=38  V1 must be non-negative (possibly an explicit + sign);
                       V2 must be non-negative without an explicit + sign
        SVCODE_2x7=39  V1 must be non-negative (possibly an explicit + sign);
                       V2 must be positive, without an explicit + sign
        SVCODE_3x7=55  V1 must be positive (possibly an explicit + sign);
                       V2 must be positive, without an explicit + sign
        SVCODE_6x6=102 V1 must be non-negative without an explicit + sign;
                       V2 must be non-negative without an explicit + sign
        SVCODE_6x7=103 V1 must be non-negative, without an explicit + sign;
                       V2 must be positive, without an explicit + sign
        SVCODE_7x7=119 V1 must be positive, without an explicit + sign;
                       V2 must be positive, without an explicit + sign
   -> int Type
      Character code for the data type: 'D' for double, 'F' for float, 'I' for
      int, 'L' for long int, 'U' for unsigned long int
  <-  void *Vp[2]
      Void pointers to the two data values to be updated. These will be updated
      if the corresponding data value is correctly decoded.

Author / revision:
  P. Kabal
  $Revision: 1.27 $  $Date: 2023/04/11 15:34:45 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>

#define NCBUF     80

#define DP_BADV  -2
#define DP_BADF  -1    /* Error message in STdec1val */
#define DP_EMPTY  0
#define DP_LVAL   1
#define DP_DELIM  2
#define DP_RVAL   4

#define SVCODE_0x0   0   /* no constraints */
#define SVCODE_7x0   7   /* V2 must be positive, without an explicit + sign */
#define SVCODE_7x2  39   /* V2 must be positive, without an explicit + sign; V1
                            is non-negative (possibly an explicit + sign) */
#define SVCODE_7x3  55   /* V2 must be positive, without an explicit + sign; V1
                            is positive (possibly an explicit + sign) */
#define SVCODE_7x7 119   /* V2 must be positive, without an explicit + sign; V1
                            is positive, without an explicit + sign */

#define SCODE1(svcode) ((svcode/32) & (1+2+4))
#define SCODE2(svcode) ((svcode/2)  & (1+2+4))
#define ZCODE1(svcode) ((svcode/16) & 1)
#define ZCODE2(svcode) (svcode & 1)

#define WS_DELIM  2

static int
ST_checkSign(char c, int DMask);
static int
ST_checkZero(int Type, void *Val);


int
STdecPair(const char String[], const char Delim[], int Type, int SVCode,
          void *Vp[2])

{
  char *token;
  const char *p;
  char cbuf[NCBUF+1];
  int nc, status, SignCode1, SignCode2, ZeroCode1, ZeroCode2;

  /* Allocate temporary string space */
  nc = (int) strlen(String);
  token = cbuf;
  if (nc > NCBUF)
    token = (char *) UTmalloc(nc + 1);

  /* Separate the codes for V1 and V2 */
  SignCode1 = SCODE1(SVCode);
  SignCode2 = SCODE2(SVCode);
  ZeroCode1 = ZCODE1(SVCode);
  ZeroCode2 = ZCODE2(SVCode);

  status = DP_EMPTY;

  /* Separate the input into two parts; token has left part, p the right part */
  p = STfindToken(String, Delim, "", token, WS_DELIM, nc);
  if (p != NULL && strchr(Delim, *(p-1)) == NULL)   /* Whitespace delimiter */
    status = DP_BADV;

  /* Pick up the first value */
  if (status >= 0 && token[0] != '\0') {
    status += DP_LVAL;          /* Left value present */
    if (ST_checkSign(token[0], SignCode1))
      status = DP_BADV;
    else if (STdec1val(token, Type, Vp[0]))
      status = DP_BADF;
    else if (ZeroCode1 && ST_checkZero(Type, Vp[0]))
      status = DP_BADV;
  }

  if (status >= 0 && p != NULL) {   /* Delimiter present  */
    status += DP_DELIM;
    /* Pick up the next token */
    p = STfindToken(p, Delim, "", token, WS_DELIM, nc);
    if (p != NULL)
      status = DP_BADV;         /* Trailing characters */
  }

  /* Pick up the second value */
  if (status >= DP_DELIM && token[0] != '\0') {
    status += DP_RVAL;            /* Right value present */
    if (ST_checkSign(token[0], SignCode2))
      status = DP_BADV;
    else if (STdec1val(token, Type, Vp[1]))
      status = DP_BADF;
    else if (ZeroCode2 && ST_checkZero(Type, Vp[1]))
      status = DP_BADV;
  }

  if (nc > NCBUF)
    UTfree((void *) token);

  return status;
}

/* Check for forbidden sign values */


static int
ST_checkSign(char c, int DMask)

{
  int cs;

  switch (c) {
    case '+':
      cs = 2 & DMask;
     break;
    case '-':
      cs = 1 & DMask;
      break;
    default:
     cs = 4 & DMask;
  }
  return cs;
}

/* Check for zero values */


static int
ST_checkZero(int Type, void *Val)

{
  int status;

  switch (Type) {
  case 'D':
    status = (*(double *) Val == 0);
    break;
  case 'F':
    status = (*(float *) Val == 0);
    break;
  case 'I':
    status = (*(int *) Val == 0);
    break;
  case 'L':
    status = (*(long int *) Val == 0);
    break;
  case 'U':
    status = (*(unsigned long int *) Val == 0);
    break;
  default:
    assert(0);
    break;
  }

  return status;
}
