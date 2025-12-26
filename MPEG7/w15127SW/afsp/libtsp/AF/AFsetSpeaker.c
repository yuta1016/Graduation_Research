/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFsetSpeaker(const char String[])

Purpose:
  Set loudspeaker spatial positions

Description:
  This routine sets the mapping of output channels to speaker positions. The
  spatial positions of the loudspeakers are specified as a list of comma and/or
  white-space separated locations from the list below.
             Name                    IEC 62574  AES 22.2  WAVE Ext
     FL    - Front Left                1        1/1     1     0x1
     FR    - Front Right               2        1/2     2     0x2
     FC    - Front Center              3        2/1     3     0x4
     LFE1  - Low Frequency Effects-1   4        2/2     4     0x8
     BL    - Back Left                 5        3/1     5    0x10
     BR    - Back Right                6        3/2     6    0x20
     FLc   - Front Left center         7        4/1     7    0x40
     FRc   - Front Right center        8        4/2     8    0x80
     BC    - Back Center               9        5/1     9   0x100
     LFE2  - Low Frequency Effects-2  10        5/2
     SiL   - Side Left                11        6/1    10   0x200
     SiR   - Side Right               12        6/2    11   0x400
     TpFL  - Top Front Left           13        7/1    12   0x800
     TpFR  - Top Front Right          14        7/2    13  0x1000
     TpFC  - Top Front Center         15        8/1    14  0x2000
     TpC   - Top Center               16        8/2    15  0x4000
     TpBL  - Top Back Left            17        9/1    16  0x8000
     TpBR  - Top Back Right           18        9/2    17 0x10000
     TpSiL - Top Side Left            19       10/1
     TpSiR - Top Side Right           20       10/2
     TpBC  - Top Back Center          21       11/1    18 0x20000
     BtFC  - Bottom Front Center      22       11/2
     BtFL  - Bottom Front Left        23       12/1
     BtFR  - Bottom Front Right       24       12/2
     FLw   - Front Left wide          25
     FRw   - Front Right wide         26
     LS    - Left Surround            27
     RS    - Right Surround           28
     LSd   - Left Surround direct     29
     RSd   - Right Surround direct    30
     TpLS  - Top Left Surround        31
     TpRS  - Top Right Surround       32
     -     - No speaker assigned

  A speaker position can be associated with only one channel. For compatibility
  with previous usage, some synonyms are applicable. For instance
     LF       -> LFE1
     SL/SR    -> SiL/SiR
     TFL/TFR  -> TpFL/TpFR
     TBL/TFR -> TpBL/TpBR

  The following names can be used to set a speaker location configuration.
    "1.0-mono"       -> "FC"
    "2.0-stereo"     -> "FL FR"
    "4.0-quad"       -> "FL FR BL BR"
    "5.1-surround"   -> "FL FR FC LFE1 BL BR"
    "5.1+4-surround" -> "FL FR FC LFE1 BL BR TpFL TpFR TpBL TpBR"
    "7.1-surround"   -> "FL FR FC LFE1 BL BR SiL SiR"
    "7.1+4-surround" -> "FL FR FC LFE1 BL BR SiL SiR TpFL TpFR TpBL TpBR"

  WAVE extensible files:
  These files can store the speaker locations in the file header if certain
  constraints are in place.
    Available speaker locations:
      "FL FR FC LFE1 BL BR FLc FRc BC SiL SiR TpC TpFL TpFC TpFR,
      TpBL, TpBC, TpBR"
    Channels:
      If M speaker locations are specified, they must be associated with the
      first M audio channels.
    Speaker Location Order:
      The speaker locations must be in the same order as the list, but locations
      can be skipped. As an example, the order "FL FR TpC" is valid, but
      "FL FC FR" is not.

  AIFF-C files:
  Speaker locations are implicit in AIFC-C audio files.
     Stereo: "FL FR"
     3 channel: "FL FR FC"
     Quadraphonic" "FR FR BL BR"
     4 channel surround: "FL FC FR BC"
     6 channel surround: "FL FLc FC FR FRc BC"

  AU, AIFF, AIFF-C, and WAVE (non-extensible):
  Speaker locations appear as an information record of the form,
     "loudspeakers: FL FR LFE1"
  This speaker location record is one of the standard information records which
  appear in the file header.

Parameters:
  <-  int AFsetSpeaker
      Error flag, zero for no error
   -> const char String[]
      String containing the comma and/or white-space separated list of speaker
      positions

Author / revision:
  P. Kabal
  $Revision: 1.29 $  $Date: 2023/03/23 20:21:01 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFinfo.h>


int
AFsetSpeaker(const char String[])

{
  unsigned char SpkrConfig[AF_MAXN_SPKR+1];
  int Err, Nspkr;

 /* Set the parameters */
  Err = AFgenSpkrConfig(String, SpkrConfig, AF_MAXN_SPKR);
  if (!Err)
    Nspkr = (int) strlen((const char *) SpkrConfig);
  else
    Nspkr = 0;

  UTfree(AFopt.SpkrConfig);
  AFopt.SpkrConfig = NULL;
  if (!Err && Nspkr > 0) {
    AFopt.SpkrConfig = (unsigned char *) UTmalloc(Nspkr + 1);
    STcopyMax((const char *) SpkrConfig, (char *) AFopt.SpkrConfig, Nspkr);
  }

  return Err;

/*
 Next AFopnWrite will call AFpresetWpar to process AFopt.SpkrConfig. After
 opening the output file, AFoptions is called to recover the space allocated to
 AFopt.SpkrConfig.
*/
}
