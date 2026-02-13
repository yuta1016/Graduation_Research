/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void GToptions(int argc, const char *argv[], struct FT_Sine *Sine,
                 struct GT_FOpar *FO)

Purpose:
  Decode options for GenTone

Description:
  This routine decodes options for GenTone.

Parameters:
   -> int argc
      Number of command line arguments
   -> const char *argv[]
      Array of pointers to argument strings
  <-  struct GT_Sine *Sine
      Sine wave parameters
  <-  struct GT_FOpar *FO
      Output file parameters

Author / revision:
  P. Kabal
  $Revision: 1.17 $  $Date: 2023/04/11 15:38:19 $

----------------------------------------------------------------------*/

#include <assert.h>
#include <math.h>

#include <libtsp.h>
#include <AFpar.h>
#include <AO.h>
#include "GenTone.h"

#define SVCODE_0x7  7   /* V1 is unconstrained;
                           V2 must be positive, without an explicit + sign */
#define SVCODE_6x7  103 /* V1 must be non-negative, without an explicit + sign;
                           V2 must be positive, without an explicit + sign */
#define PI  3.14159265358979323846
#define DEG_RAD   (PI / 180.0)

#define ERRSTOP(text,par) UThalt("%s: %s: \"%s\"", PROGRAM, text, par)

/* Option table */
static const char *OptTable[] = {
  "-f#", "--freq*uency=",
  "-r#", "--rms=",
  "-a#", "--amp*litude=",
  "-p#", "--phase=",
  NULL
};


void
GToptions(int argc, const char *argv[], struct GT_Sine *Sine,
          struct GT_FOpar *FO)

{
  const char *OptArg;
  int nF, n;
  double Dval[2];

/* Output file defaults */
  FOpar_INIT(FO);
  FO->Sfreq = AF_SFREQ_DEFAULT;

/* Defaults */
  Sine->Freq = AF_SFREQ_DEFAULT / 8.0;
  Sine->Ampl = AMPL_DEF;
  Sine->Phase = 0;

/* Initialization */
  UTsetProg(PROGRAM);
  nF = 0;

/* Decode options */
  AOinitOpt(argc, argv);
  while (1) {

    /* Decode output file options */
    n = AOdecFO(FO);
    if (n >= 1)
      continue;

    /* Decode help options */
    n = AOdecHelp(VERSION, GTMF_Usage);
    if (n >= 1)
      continue;

    /* Decode program options */
    n = AOdecOpt(OptTable, &OptArg);
    if (n == -1)
      break;

    switch (n) {
    case -2:
      UThalt(GTMF_Usage, PROGRAM);
      break;
    case 0:
      /* Filename argument */
      ++nF;
      if (nF > 1)
        UThalt("%s: %s", PROGRAM, GTM_XFName);
      STcopyMax(OptArg, FO->Fname, FILENAME_MAX-1);
      break;
    case 1:
    case 2:
      /* frequency (allow zero frequency) */
      if (STdecDfrac(OptArg, SVCODE_6x7, Dval))
        ERRSTOP(GTM_BadFreq, OptArg);
      Sine->Freq = Dval[0] / Dval[1];
      break;
    case 3:
    case 4:
      /* rms value */
      if (STdecDfrac(OptArg, SVCODE_6x7, Dval) || Dval[1] <= 0)
        ERRSTOP(GTM_BadRMS, OptArg);
      Sine->Ampl = SQRT2 * Dval[0] / Dval[1];
      break;
    case 5:
    case 6:
      /* amplitude */
      if (STdecDfrac(OptArg, SVCODE_6x7, Dval) || Dval[1] <= 0)
        ERRSTOP(GTM_BadAmpl, OptArg);
      Sine->Ampl = Dval[0] / Dval[1];
      break;
    case 7:
    case 8:
      /* phase */
      if (STdecDfrac(OptArg, SVCODE_0x7, Dval) || Dval[1] <= 0)
        ERRSTOP(GTM_BadPhase, OptArg);
      Sine->Phase =  DEG_RAD * Dval[0] / Dval[1]; /* Radians */
      break;
    default:
      assert(0);
      break;
    }
  }

/* Checks */
  if (nF < 1)
    UThalt("%s: %s", PROGRAM, GTM_NoFName);
  if (FO->Nframe == AF_NFRAME_UNDEF)
    UThalt("%s: %s", PROGRAM, GTM_NoNsamp);

/* Process file type; check compatibility of data format */
  AOsetDFormat(FO, NULL, 0);
}
