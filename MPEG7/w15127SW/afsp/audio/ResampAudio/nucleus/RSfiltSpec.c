/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void RSfiltSpec(const char String[], struct Fspec_T *Fspec)

Purpose:
  Decode interpolation filter specifications

Description:
  This routine sets interpolation filter specifications from values specified in
  an input string.

Parameters:
   -> const char String[]
      String containing the list of filter specifications
  <-> struct Fspec_T *Fspec
      Structure with the decoded filter specifications

Author / revision:
  P. Kabal
  $Revision: 1.22 $  $Date: 2023/04/05 15:52:45 $

-------------------------------------------------------------------------*/

#include <math.h>
#include <string.h>

#include "../ResampAudio.h"

#define WS_STRIP  1

#define SVCODE_0x7  7   /* V1 is unconstrained;
                           V2 must be positive, without an explicit + sign */

#define ERRSTOP(text,par) UThalt("%s: %s: \"%s\"", PROGRAM, text, par)

static const char *keytable [] = {
  "file",
  "write",
  "ratio",
  "del*ay",
  "g*ain",
  "cut*off",
  "atten*uation",
  "alpha",
  "N*cof",
  "span",
  "offset",
  NULL
};


void
RSfiltSpec(const char String[], struct Fspec_T *Fspec)

{
  int ind, nt, nc;
  const char *p;
  char *token;
  double Dval[2], atten;

/* Allocate temporary storage */
  nt = (int) strlen(String);
  token = (char *) UTmalloc(nt + 1);

/* Separate the parameters */
  p = String;
  while (p != NULL) {
    p = STfindToken(p, ",", "\"\"", token, WS_STRIP, nt);
    if (token[0] != '\0') {

      /* Decode the parameter values */
      ind = STkeyXpar(token, "=", "\"\"", keytable, token);
      if (ind < 0)
        ERRSTOP(RSM_BadKey, token);

      switch (ind) {

      /* file = */
      case 0:
        UTfree((void *) Fspec->FFile);
        nc = (int) strlen(token);
        if (nc <= 0)
          UThalt("%s: %s", PROGRAM, RSM_NoCoef);
        Fspec->FFile = (char *) UTmalloc(nc + 1);
        STcopyMax(token, Fspec->FFile, nc);
        break;

      /* write = */
      case 1:
        UTfree((void *) Fspec->WFile);
        nc = (int) strlen(token);
        if (nc <= 0)
          UThalt("%s: %s", PROGRAM, RSM_NoFName);
        Fspec->WFile = (char *) UTmalloc(nc + 1);
        STcopyMax(token, Fspec->WFile, nc);
        break;

      /* ratio = */
      case 2:
        if (STdec1int(token, &Fspec->Ir) || Fspec->Ir <= 0)
          ERRSTOP(RSM_BadInterp, token);
        break;

      /* delay = */
      case 3:
        if (STdecDfrac(token, SVCODE_0x7, Dval))
          ERRSTOP(RSM_BadDelay, token);
        Fspec->Del = Dval[0] / Dval[1];
        break;

      /* gain = */
      case 4:
        if (STdecDfrac(token, SVCODE_0x7, Dval))
          ERRSTOP(RSM_BadFGain, token);
        Fspec->Gain = Dval[0] / Dval[1];
        break;

      /* cutoff = */
      case 5:
        if (STdec1double(token, &Fspec->Fc) || Fspec->Fc <= 0.0 ||
                                               Fspec->Fc > 0.5)
          ERRSTOP(RSM_BadFCutoff, token);
        break;

      /* attenuation = */
      case 6:
        if (STdec1double(token, &atten) || atten < 21.0)
          ERRSTOP(RSM_BadAtten, token);
        Fspec->alpha = RSKattenXalpha(atten);
        break;

      /*  alpha = */
      case 7:
        if (STdec1double(token, &Fspec->alpha) || Fspec->alpha < 0.0)
          ERRSTOP(RSM_BadWinPar, token);
        break;

      /* N = */
      case 8:
      if (STdec1int(token, &Fspec->Ncof) || Fspec->Ncof <= 0)
        ERRSTOP(RSM_BadNCoef, token);
      break;

      /* span = */
      case 9:
      if (STdec1double(token, &Fspec->Wspan) || Fspec->Wspan <= 0.0)
        ERRSTOP(RSM_BadWinSpan, token);
      break;

      /* offset = */
      case 10:
      if (STdec1double(token, &Fspec->Woffs))
        ERRSTOP(RSM_BadWinOffs, token);
      break;

      }
    }
  }
  UTfree((void *) token);
}
