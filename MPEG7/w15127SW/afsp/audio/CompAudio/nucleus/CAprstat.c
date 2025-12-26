/*------------- Telecommunications & Signal Processing Lab --------------
                          McGill University
Routine:
  void CAprstat(int Nfiles, long int Nchan, long int Ndiff, double ScaleF[],
                struct Stats_F *StatsFP[])
Purpose:
  Print statistics for an audio file

Description:
  This routine prints statistics for audio file data. The standard information
  printed includes the mean, standard deviation, maximum value and minimum
  value. Additional counts (if nonzero) are reported. These are the number of
  overloads and the number of anomalous transitions.

Parameters:
   -> int Nfiles
      Number of files (one or two)
   -> long int Nchan
      Number of effective channels for each file
   -> long int Ndiff
      Number of differing samples for two input files
   -> double ScaleF[]
      Scale factor for each file. This is the scale factor before a user
      supplied gain is applied.
   -> const struct Stats_F *StatsFP[]
      Structure containing the file statistics for each file

Author / revision:
  P. Kabal
  $Revision: 1.34 $  $Date: 2023/04/05 13:26:25 $

-----------------------------------------------------------------------*/

#include <math.h>   /* sqrt, modf */

#include "../CompAudio.h"

#define MAXV(a, b)  (((a) > (b)) ? (a) : (b))
#define SQRV(x)   ((x) * (x))

static void
CA_prstat(const struct Stats_F StatsF[], long int Nchan, double ScaleF);


void
CAprstat(int Nfiles, long int Nchan, long int Ndiff, double ScaleF[],
         struct Stats_F *StatsFP[])
{
  printf("\n");
  if (Nfiles == 1)
    CA_prstat(StatsFP[0], Nchan, ScaleF[0]);

  else if (Ndiff == 0 && ScaleF[0] == ScaleF[1])
    /* Data identical in two files, print the statistics for only one file */
    CA_prstat(StatsFP[0], Nchan, ScaleF[0]);

  else {
    /* Data identical with different scale factors, or data differ */
    printf(CAMF_FileA);
    CA_prstat(StatsFP[0], Nchan, ScaleF[0]);
    printf(CAMF_FileB);
    CA_prstat(StatsFP[1], Nchan, ScaleF[1]);
  }
}

static void
CA_prstat(const struct Stats_F StatsF[], long int Nchan, double ScaleF)
{
  double SFI, sd, sd2, sm, ActFactor;
  int i, PrV;
  long int N;

  SFI = 1.0 / ScaleF;
  PrV = (SFI >= 128 && SFI <= 32768);

  for (i = 0; i < Nchan; ++i) {

    if (Nchan > 1)
      printf(CAMF_Channel, i+1);
    N = StatsF[i].N;
    if (N != 0) {
      printf(CAMF_NumSamp, N);
      if (StatsF[i].Vmax == StatsF[i].Vmin)
        sd = 0.0; /* Calculation below may give sqrt(-eps) */
      else {
        sd2 = (StatsF[i].Sxx - SQRV(StatsF[i].Sx) / N) / MAXV(N-1, 1);
        sd = sqrt(sd2);
      }
  /* sd^2 is an unbiased estimate of the variance (assuming independent
     samples), but sd itself is not unbiased. However, the formula is the one
     usually used for estimating the standard deviation.
  */
      sm = StatsF[i].Sx / N;
      if (PrV)
        printf(CAMF_SDevMeanV, SFI*sd, 100*sd, SFI*sm, 100*sm);
      else
        printf(CAMF_SDevMeanP, 100*sd, 100*sm);
      if (PrV)
        printf(CAMF_MaxMinV, SFI * StatsF[i].Vmax, 100*StatsF[i].Vmax,
               SFI*StatsF[i].Vmin, 100*StatsF[i].Vmin);
      else
        printf(CAMF_MaxMinP, 100*StatsF[i].Vmax, 100*StatsF[i].Vmin);

      if (StatsF[i].Novload > 0) {
        if (StatsF[i].Nrun == 0 || StatsF[i].Nrun == StatsF[i].Novload)
          printf(CAMF_NumOvld, StatsF[i].Novload);
        else if (StatsF[i].Nrun == 1)               /* Overloads in 1 run */
          printf(CAMF_NumOvld1, StatsF[i].Novload);
        else
          printf(CAMF_NumOvldN, StatsF[i].Novload, StatsF[i].Nrun);
      }

      if (StatsF[i].Nanomal > 0)
        printf(CAMF_NumAnom, StatsF[i].Nanomal);
      if (StatsF[i].NbSCount > 0)
        printf(CAMF_NZNbS);

      if (StatsF[i].ActLev > 0) {
        ActFactor = (StatsF[i].Sxx / N) / SQRV(StatsF[i].ActLev);
        if (PrV)
          printf(CAMF_ActLevelV, SFI*StatsF[i].ActLev, 100*StatsF[i].ActLev,
                 100*ActFactor);
        else
          printf(CAMF_ActLevelP, 100*StatsF[i].ActLev, 100*ActFactor);
      }
    }
  }
}
