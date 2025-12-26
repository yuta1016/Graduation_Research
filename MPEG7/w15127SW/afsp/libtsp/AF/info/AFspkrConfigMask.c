/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
 UT_uint4_t AFspkrConfigMask(const unsigned char SpeakerConfig[], int ErrFlag)

Purpose:
  Create a channel mask for a speaker configuration.

Description:
  The WAVE extensible format for audio files uses a channel mask with 18 bits.
  Each bit in the mask corresponds to a speaker position. If the bit is one, the
  corresponding speaker is associated with an audio channel. The first audio
  channel is assigned the speaker position corresponding to the first bit
  (starting from low to high) that is is turned on. The second channel is
  associated with the next bit that is turned on.

  The speaker locations and the corresponding mask bit are as follows:
     Speaker position                   Mask
     1 FL    - Front Left     1          0x1
     2 FR    - Front Right               0x2
     3 FC    - Front Center              0x4
     4 LFE1  - Low Frequency Effects-1   0x8
     5 BL    - Back Left                0x10
     6 BR    - Back Right               0x20
     7 FLc   - Front Left center        0x40
     8 FRc   - Front Right center       0x80
     9 BC    - Back Center             0x100
    10 SiL   - Side Left               0x200
    11 SiR   - Side Right              0x400
    12 TpFL  - Top Front Left          0x800
    13 TpFR  - Top Front Right        0x1000
    14 TpFC  - Top Front Center       0x2000
    15 TpC   - Top Center             0x4000
    16 TpBL  - Top Back Left          0x8000
    17 TpBR  - Top Back Right        0x10000
    18 TpBC  - Top Back Center       0x20000
  The input to this routine is a array of speaker location indices. There are
  33 possible indices (one is a place holder: "no speaker assigned"). A map
  takes each input location and maps it 20 values. Eighteen of those are the
  speaker locations shown. The two remained values indicate an error or the
  unassigned position. The unassigned position can only appear at the end of
  the speaker location array.

Parameters:
  <-  UT_uint4_t ATspkrConfigMask
      This value is 0, if the SpkrConfig is empty or if the SpkrConfig has
      entries which are one of permissible values.
   -> const unsigned char SpkrConfig[]
      Array with speaker location identifiers in the order of the audio
      channels.
   -> int ErrFlag
      If 0, no error messages are printed; else error messages are printed.

Author / revision:
  P. Kabal
  $Revision: 1.3 $  $Date: 2023/03/17 19:31:28 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>
#include <libtsp/UTtypes.h>

#define WV_SPKR     /* Enables speaker definitions in WVpar.h */
#include <libtsp/WVpar.h>


UT_uint4_t
AFspkrConfigMask(const unsigned char SpkrConfig[], int ErrFlag)

{
  int i, n, mp, m, Nspkr;
  UT_uint4_t ChannelMask;

  if (SpkrConfig == NULL || SpkrConfig[0] == '\0')
    Nspkr = 0;
  else
    Nspkr = (int) strlen((const char *) SpkrConfig);

  ChannelMask = 0;
  mp = -1;         /* Previous mask index (to check correct ordering) */
  for (i = 0; i < Nspkr; ++i) {
    n = SpkrConfig[i];
    if (n == AF_SPKR_AUX)
      mp = WV_N_SPKR_MASK; /* If any more locations => out of order error */
    else {
      m = AF_WV_ChannelMask[n-1];  /* m is a mask index (0, ...) */
      if (m == WV_X) {
        if (ErrFlag)
          UTwarn("AFwrWVhead - %s", AFM_WV_NSSpkr);
        ChannelMask = 0;
        break;
      }
      if (m <= mp) {   /* Check ordering */
        if (ErrFlag)
          UTwarn("AFwrWVhead - %s", AFM_WV_NSSpkrO);
        ChannelMask = 0;
        break;
      }
      ChannelMask = ChannelMask | WV_Spkr_Mask[m];
      mp = m;
    }
  }

  return ChannelMask;
}