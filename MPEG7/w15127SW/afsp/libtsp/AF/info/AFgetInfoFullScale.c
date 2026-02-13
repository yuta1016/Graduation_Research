/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double AFgetInfoFullScale(const struct AF_info *AFInfo)

Purpose:
  Find and decode an information record specifying file data full scale value

Description:
  This routine searches the information record structure for a record specifying
  the file data full scale value.

  Bits/sample information record ID:
      "full_scale:"
  The information following the identifier consists of a float value or ratio
  of float values. Examples are
      "full_scale: 256/2"
      "full_scale: 32768"

Parameters:
  <-  double AFgetInfoFullScale
      Full scale value determined from the information record.  This is set to
      AF_FULLSCALE_UNDEF if the record was not found or a decoding error
      occurred.
   -> const struct AF_info *AFInfo
      AFsp information structure

Author / revision:
  P. Kabal
  $Revision: 1.7 $  $Date: 2023/04/04 14:19:56 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>

#define SVCODE_7x7  119

/* Sampling rate keywords */
static const char *RecID[] = AF_INFOID_FULLSCALE;


double
AFgetInfoFullScale(const struct AF_info *AFInfo)

{
  const char *data;
  double Val[2], FullScale;

/* Search for named records in an AFsp information structure */
  data = AFgetInfoRec(RecID, AFInfo);

  FullScale = AF_FULLSCALE_UNDEF;
  if (data == NULL)        /* Record not found */
    return FullScale;

  /* Check the decoded values */
  if (STdecDfrac(data, SVCODE_7x7, Val))
    UTwarn("AFgetInfoFullScale - %s", AFM_Info_DecFullScale);
  else
    FullScale = Val[0]/Val[1];

  return FullScale;
}
