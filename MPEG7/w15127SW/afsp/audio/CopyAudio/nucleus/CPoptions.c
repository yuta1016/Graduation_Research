/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void CPoptions(int argc, const char *argv[], int *Mode,
                 struct AO_FIpar FI[MAXIFILE], int *Nifiles,
                 struct AO_FOpar *FO)

Purpose:
  Decode options for CopyAudio

Description:
  This routine decodes options for CopyAudio.

Parameters:
   -> int argc
      Number of command line arguments
   -> const char *argv[]
      Array of pointers to argument strings
  <-  int *Mode
      Combine / concatenate mode
  <-  struct AO_FIpar FI[MAXIFILES]
      Input file parameters
  <-  int *Nifiles
      Number of input file names
  <-  struct AO_FOpar *FO
      Output file parameters
  <-  struct CP_Chgain *Chgain
      Channel gain matrix

Author / revision:
  P. Kabal
  $Revision: 1.91 $  $Date: 2023/04/05 13:38:29 $

----------------------------------------------------------------------*/

#include <assert.h>
#include "../CopyAudio.h"

#define CHGAIN_EXTRACT  1
#define CHGAIN_MIX      2
#define LIM_LNxNUxNM  2   /* "n" -> "L:L+n-1"; "n:" -> "n:U" */

#define MAXV(a, b)  (((a) > (b)) ? (a) : (b))
#define NELEM(array)  ((int) ((sizeof array) / (sizeof array[0])))
#define ERRSTOP(text,par) UThalt("%s: %s: \"%s\"", PROGRAM, text, par)
#define ERRHALT(text)     UThalt("%s: %s", PROGRAM, text)

/* Option table */
/* Size: 2*4 + 2*MAXNCO + 1*/
static const char *OptTable[] = {
  "-c", "--com*bine",
  "-C", "--con*catenate",
  "-l#", "--l*imits=",
  "-x#", "--ex*tract=",
  "-cA#", "--chanA=",
  "-cB#", "--chanB=",
  "-cC#", "--chanC=",
  "-cD#", "--chanD=",
  "-cE#", "--chanE=",
  "-cF#", "--chanF=",
  "-cG#", "--chanG=",
  "-cH#", "--chanH=",
  "-cI#", "--chanI=",
  "-cJ#", "--chanJ=",
  "-cK#", "--chanK=",
  "-cL#", "--chanL=",
  "-cM#", "--chanM=",
  "-cN#", "--chanN=",
  "-cO#", "--chanO=",
  "-cP#", "--chanP=",
 NULL
};

/* Channel designations for error message */
static const char Ch[] = "ABCDEFGHIJKLMNOP";


void
CPoptions(int argc, const char *argv[], int *Mode, struct AO_FIpar FI[MAXIFILE],
          int *Nifiles, struct AO_FOpar *FO, struct CP_Chgain *Chgain)

{
  const char *OptArg;
  struct AO_FIpar FIx;
  int nF, k, n, NCI, mode, ChgainFlag, FIParSet;
  int Chset[MAXNCO];

/* Defaults */
  mode = M_COMB;

/* Size checks */
  assert(NELEM(OptTable) == 2*MAXNCO+9);
  assert(NELEM(Ch) == MAXNCO+1);

/* Input file defaults */
  FIParSet = 0;
  FIpar_INIT(&FIx);       /* Temp input file parameter structure */

/* Output file defaults */
  FOpar_INIT(FO);
  ChgainFlag = 0;

/* Initialize the channel gain structure */
/* If the number of output channels in Chgain is zero, it is assumed that the
   number of output channels are mapped one-to-one to the input channels */
  Chgain_INIT(Chgain);  /* Channels gains and offsets set to zero */
  VRiZero(Chset, MAXNCO);

/* Initialization */
  UTsetProg(PROGRAM);
  nF = 0;

/* Decode options */
/* For each argument try the following:
   - decode an input file option
   - if not successful, decode an output file option
   - if not successful, decode a help option
   - if not successful, decode a file name or a program option
*/
  AOinitOpt(argc, argv);
  while (1) {

    /* Decode input file options */
    n = AOdecFI(&FIx);
    if (n >= 1) {
      FIParSet = nF;    /* Flag, found input parameters FI[nF] */
      continue;
    }

    /* Decode output file options */
    n = AOdecFO(FO);
    if (n >= 1)
      continue;

    /* Decode help options */
    n = AOdecHelp(VERSION, CPMF_Usage);
    if (n >= 1)
      continue;

    /* Decode program options */
    n = AOdecOpt(OptTable, &OptArg);
    if (n == -1)
      break;

    switch (n) {
    case -2:
      UThalt(CPMF_Usage, PROGRAM);
      break;
    case 0:
      /* Filename argument */
      /* Filename tentatively assigned to both the output file and to input file
         nF-1.
         - The last file name will be the output file name
         - The other file names will be input file names (0 to nF-2)
       */
      ++nF;
      if (nF > MAXIFILE+1)
        ERRHALT(CPM_XFName);
      STcopyMax(OptArg, FO->Fname, FILENAME_MAX-1);
      STcopyMax(OptArg, FIx.Fname, FILENAME_MAX-1);
      if (nF <= MAXIFILE)
        FI[nF-1] = FIx;     /* Save input parameters */
      break;
    case 1:
    case 2:
      /* Combine mode */
      mode = M_COMB;
      break;
    case 3:
    case 4:
      /* Concatenate mode */
      mode = M_CONCAT;
      break;
    case 5:
    case 6:
      /* Limits specification */
      /* Reset the default limits - the option may be called more than once */
      FIx.Lim[0] = 0;
      FIx.Lim[1] = AO_LIM_UNDEF;
      if (STdecLlimits(OptArg, FIx.Lim, LIM_LNxNUxNM) ||
          (FIx.Lim[1] != AO_LIM_UNDEF && FIx.Lim[0] > FIx.Lim[1]))
        ERRSTOP(CPM_BadLimits, OptArg);
      break;
    case 7:
    case 8:
      /* Extract input channels */
      if (ChgainFlag == CHGAIN_MIX)
        ERRHALT(CPM_BadOptChan);

      /* Reset gains */
      if (ChgainFlag == CHGAIN_EXTRACT) {
        Chgain_INIT(Chgain);  /* Channels gains and offsets set to zero */
        VRiZero(Chset, MAXNCO);
      }
      ChgainFlag = CHGAIN_EXTRACT;
      CPdecExtract(OptArg, Chgain);
      for (k = 0; k < Chgain->NCO; ++k)
        Chset[k] = 1;   /* Flag, gain set for output channel k */
      break;
    default:
      ChgainFlag = CHGAIN_MIX;
      /* Channel gain expressions */
      /* Output channel:  n=9,10 -> k=0;  n=11,12 -> k=1; ... */
      k = (n - 9) / 2;
      NCI = CPdecChan(OptArg, Chgain->Gain[k], &Chgain->Offset[k]);
      Chgain->NCI = MAXV(NCI, Chgain->NCI);
      Chgain->NCO = MAXV(k+1, Chgain->NCO);
      Chset[k] = 1;     /* Flag, gain set for output channel k */
      break;
    }
  }

/* Checks */
  if (nF < 2)
    ERRHALT(CPM_MFName);
  for (k = 0; k < Chgain->NCO; ++k) {  /* Check output channel gains */
    if (Chset[k] == 0)
      UThalt("%s: %s %c", PROGRAM, CPM_NoChanSpec, Ch[k]);
  }
  if (FIParSet >= nF - 1)     /* Input parameters set after last input file */
    ERRHALT(CPM_LateFPar);

  /* Check for too many stdin specs */
  AOstdin(FI, nF-1);

  /* Process file type; check compatibility of data format */
  AOsetDFormat(FO, NULL, nF-1);

/* Set return values */
  *Mode = mode;
  *Nifiles = nF-1;
}
