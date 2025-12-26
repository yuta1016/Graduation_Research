/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int CPdecChan(const char String, double Gain[MAXNCI], double *Offset)

Purpose:
  Decode channel gain specifications

Description:
  This routine decodes gain factor strings. The gain factor string takes the
  form
    [+/-] [gain *] chan +/- [gain *] chan +/- gain ...)
  where gain is a number or ratio and chan is a, b, c, ... or A, B, C, ... .

Parameters:
  <-  int CPdecChan
      Maximum input channel number with a gain set
   -> const char String[]
      Input string
   <-> double Gain[MAXNCI]
      Vector of input channel gains, the gains appropriate for the current
      output channel are updated
  <-> double *Offset
      Offset, updated if appropriate
Author / revision:
  P. Kabal
  $Revision: 1.32 $  $Date: 2023/04/05 13:37:52 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include <libtsp/nucleus.h>
#include "../CopyAudio.h"

#define SVCODE_0x7 7   /* V1 is unconstrained;
                          V2 must be positive, without an explicit + sign */
#define WS_TRIM    1

#define DEFAULT_GAIN_VALUE 0
#define GAIN_VALUE         1
#define OFFSET_VALUE       2

#define MAXNC 80

#define MAXV(a, b)  (((a) > (b)) ? (a) : (b))
#define NELEM(array)  ((int) ((sizeof array) / (sizeof array[0])))
#define BADCHANEX(String) \
                  UThalt("%s: %s: \"%s\"", PROGRAM, CPM_BadChanExp, String)

static int
CP_decChanID(char String[]);
static int
CP_decGain(char String[], double *gain);

int
CPdecChan(const char String[], double Gain[MAXNCI], double *Offset)

{
  int nc, sign, nsign, NCI, status;
  int Times, m, n;
  const char *p;
  char *q, *token, *Vs, *Cs;
  char cbuf[MAXNC+1];
  double value, gain, offs;

  /* Allocate temporary space */
  nc = (int) strlen(String);
  if (nc == 0)
    UThalt("%s: %s", PROGRAM, CPM_EmptyChan);
  if (nc > MAXNC)
    token = (char *) UTmalloc(nc + 1);
  else
    token = cbuf;

  NCI = 0;

/* Loop over subexpressions, delimited by +/- signs */
  VRdZero(Gain, MAXNCI);
  *Offset = 0.0;
  p = String;
  nsign = 1;
  m = 0;
  while (p != NULL) {

    /* Separate the string at +/- signs, p points to past the delimiter */
    p = STfindToken(p, "+-", "", token, WS_TRIM, nc);

    /* Set the sign from the previous pass */
    sign = nsign;

    /* Save the sign of the delimiter for the next pass */
    nsign = 1;
    if (p != NULL && *(p-1) == '-')
      nsign = -1;

 /* An empty token string indicates 2 adjacent signs (except for m=1) */
    m = m + 1;
    if (token[0] == '\0') {
      if (m == 1)
        continue;     /* Next pass */
      else
        BADCHANEX(String);
    }

 /* Set up the pointers to the gain and channel strings */
    q = strchr(token, '*');
    Times = (q != NULL);
    if (Times) {
      *q = '\0';    /* Overwrite the '*' in token */
      Vs = token;   /* Value string */
      Cs = q+1;     /* Channel ID string */
      if (Vs[0] == '\0')
        BADCHANEX(String);
    }

    if (Times) {
      /* Decode the channel */
      n = CP_decChanID(Cs);
      if (n < 0)
        BADCHANEX(String);

      /* Decode the value */
      status = CP_decGain(Vs, &value);
      if (status)
        BADCHANEX(String);
      gain = sign*value;
      offs = 0;
      if (n >= 0)
        NCI = MAXV(NCI, n+1);
    }

    if (!Times) {
      /* Trial: decode the channel */
      n = CP_decChanID(token);
      if (n >= 0) {
        NCI = MAXV(NCI, n+1);
        value = 1;
        gain = sign * value;
        offs = 0;
      }
      else {
        status = CP_decGain(token, &value);
        if (status)
          BADCHANEX(String);
        gain = 0;
        offs = sign * value;
      }
    }

    /* Update gain and offset */
    if (n >= 0)
      Gain[n] += gain;
    *Offset += offs;
  }

  /* Deallocate storage */
  if (token != cbuf)
    UTfree((void *) token);

  /* Error if no input channels have been specified */
  if (NCI == 0 )
    BADCHANEX(String);

  return NCI;
}

static const char *Chtab[] = {
  "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
  "k", "l", "m", "n", "o", "p", "q", "r", "s", "t",
  "u", "v", "w", "x", "y", "z",  NULL };

/* Return the input channel number for the expression (0 to MAXNCI);  -1 for an
   error.
*/

static int
CP_decChanID(char String[])

{
  char *cp;
  int n;

  assert(MAXNCI == NELEM(Chtab) - 1);

  cp = STtrimIws(String);
  n = STkeyMatch(cp, Chtab);

  return n;
}

/* Decode the gain expression */


static int
CP_decGain(char String[], double *gain)

{
  double Dval[2];
  int status;

  Dval[0] = 1;     /* Used when the gain expression is empty */
  Dval[1] = 1;
  status = 0;
  if (String != NULL)
    status = STdecDfrac(String, SVCODE_0x7, Dval);
  *gain =  Dval[0] / Dval[1];

  return status;
}
