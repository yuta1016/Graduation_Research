/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  unsigned char *AFgetInfoSpkr(const struct AF_info *AFInfo, int MaxN)

Purpose:
  Find and decode an information record specifying loudspeaker locations

Description:
  This routine searches the information record structure for a record specifying
  loudspeaker configuration.

  Bits/sample information record ID:
      "loudspeakers:""

  The information following is a comma or whitespace separated list of the
  loudspeaker locations. See AFsetSpeaker for a list of available speaker
  location names. Examples are
      "loudspeakers: FL FR"
      "loudspeakers: FL FR FC"

Parameters:
  <-  unsigned char *AFgetInfoSpkr
      Pointer to a loudspeaker configuration string. This is a null terminated
      list of loudspeaker positions coded as unsigned bytes. This is a pointer
      to a static storage area of length AF_MAXN_SPKR+1.
   -> const struct AF_info *AFInfo
      AFsp information structure
   -> int MaxN
      Maximum number of speaker positions (number of channels). Any list of
      speaker positions longer than MaxN is truncated to MaxN positions. If the
      list is truncated, a warning message is printed.

Author / revision:
  P. Kabal
  $Revision: 1.7 $  $Date: 2023/03/14 22:00:34 $

-------------------------------------------------------------------------*/

#include <libtsp/AFinfo.h>

#define MINV(a, b)  (((a) < (b)) ? (a) : (b))

/* Loudspeakers keyword */
static const char *RecID[] = AF_INFOID_SPKR;


unsigned char *
AFgetInfoSpkr(const struct AF_info *AFInfo, int MaxN)

{
  const char *SpkrNames;
  static unsigned char SpkrConfig[AF_MAXN_SPKR+1];

/* Search for named records in an AFsp information structure */
  SpkrNames = AFgetInfoRec(RecID, AFInfo);

  SpkrConfig[0] = '\0';
  if (SpkrNames == NULL)
    return SpkrConfig;

/* Convert the string to loudspeaker position codes */
  AFgenSpkrConfig(SpkrNames, SpkrConfig, MINV(MaxN, AF_MAXN_SPKR));

  return SpkrConfig;
}
