/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void CPdecExtract(const char String, struct CP_Chgain Chgain)

Purpose:
  Decode input channel selections

Description:
  This routine decodes a list of input channel selections. The gain factor for
  the input channel associated with the output channel is set to unity. The list
  of channel inputs is of the form
    "a, b, d:e"
  The output file will have 4 channels taken from channels [a b d e] of the
  input file(s).
Parameters:
   -> const char String[]
      Input string containing a list of input channels (designated a to z) in a
      comma and/or whitespace delimited list. List items can include colon
      separated items in an increasing order (a:c) or a decreasing order (c:a).
   -> int Ichan
      Output channel number
  <-> struct CP_Chgain Chgain
      Structure with channel gains and offsets

Author / revision:
  P. Kabal
  $Revision: 1.1 $  $Date: 2023/04/05 14:01:44 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp/nucleus.h>
#include "../CopyAudio.h"

#define SVCODE_0x7 7   /* V1 is unconstrained;
                          V2 must be positive, without an explicit + sign */
#define WS_DELIM  2

#define DEFAULT_GAIN_VALUE 0
#define GAIN_VALUE         1
#define OFFSET_VALUE       2

#define MAXNC 80

#define MAXV(a, b)  (((a) > (b)) ? (a) : (b))
#define NELEM(array)  ((int) ((sizeof array) / (sizeof array[0])))
#define BADCHANEX(String) \
                  UThalt("%s: %s: \"%s\"", PROGRAM, CPM_BadChanEx, String)

static const char *Chtab[] = {
  "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
  "k", "l", "m", "n", "o", "p", "q", "r", "s", "t",
  "u", "v", "w", "x", "y", "z",  NULL };


void
CPdecExtract(const char String[], struct CP_Chgain *Gain)

{
  int nc, NCI;
  int k, m, TIndex[2];
  const char *p;
  char *token;
  char cbuf[MAXNC+1];

/* Allocate temporary space */
  nc = (int) strlen(String);
  if (nc == 0)
    UThalt("%s: %s", PROGRAM, CPM_EmptyChan);
  if (nc > MAXNC)
    token = (char *) UTmalloc(nc + 1);
  else
    token = cbuf;

  NCI = 0;
  k = 0;
  p = String;
  while (p != NULL) {

    /* Separate the string at commas or whitespace */
    p = STfindToken(p, ",", "", token, WS_DELIM, nc);

    /* Decode the input channel designator */
    if (STdecSrange(token, TIndex, (const char **) Chtab))
      UThalt("%s: %s", PROGRAM, "Invalid input channel designator");

    if (TIndex[1] >= TIndex[0]) {
      for (m = TIndex[0]; m <= TIndex[1]; ++m) {
        Gain->Gain[k][m] = 1;
        NCI = MAXV(m+1, NCI);
        ++k;
      }
    }
    else {
      for (m = TIndex[0]; m >= TIndex[1]; --m) {
        Gain->Gain[k][m] = 1;
        NCI = MAXV(m+1, NCI);
        ++k;
      }
    }
  }
  Gain->NCO = k;
  Gain->NCI = NCI;

  /* Deallocate storage */
  if (token != cbuf)
    UTfree((void *) token);

  return;
}
