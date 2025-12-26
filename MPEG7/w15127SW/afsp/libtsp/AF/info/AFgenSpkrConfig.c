/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFgenSpkrConfig(const char String[], unsigned char *SpkrConfig, int MaxN)

Purpose:
  Generate loudspeaker spatial position configuration codes.

Description:
  This routine codes a list of loudspeaker locations. The spatial positions
  of the loudspeakers are specified as a list of comma and/or white-space
  separated locations. These are mapped to a list of speaker indices. See the
  description of routine AFsetSpeaker for more information.

Parameters:
  <-  int AFgenSpkrConfig
      Error flag, zero for no error
   -> const char String[]
      String containing the list comma and/or white-space separated list of
      speaker positions and or configuration names.
  <-  unsigned char *SpkrConfig
      Null-terminated list of speaker location codes. Code values are integer
      values starting at 1. SpkrConfig must be of size MaxN+1. In case of an
      error, SpkrConfig is empty.
   -> int MaxN
      Maximum number of speaker locations. This value should normally be equal
      to or larger than the number of channels.

Author / revision:
  P. Kabal
  $Revision: 1.36 $  $Date: 2023/03/14 22:00:12 $

-------------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS)
#  define _CRT_SECURE_NO_WARNINGS     /* Allow strcpy */
#endif

#include <assert.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>

#define NCBUF     100
#define WS_DELIM  2

#define NELEM(array)  ((int) ((sizeof array) / (sizeof array[0])))

/* Speaker configurations */
const char *AF_Spkr_Config_Key[] = {
  "1.0*-mono",
  "2.0*-stereo",
  "4.0*-quad",
  "5.1*-surround",
  "5.1+4*-surround",
  "7.1*-surround",
  "7.1+4*-surround",
  NULL
};

/* Speaker keywords and synonyms */
const char *AF_Spkr_Key[] = {
  "FL",             "FR",
  "FC",             "LF*E1",
  "BL",             "BR",
  "FLc",   "FLC",   "FRc",   "FRC",
  "BC",             "LFE2",
  "SiL",   "SL",    "SiR",   "SR",
  "TpFL",  "TFL",   "TpFR",  "TFR",
  "TpFC",  "TFC",   "TpC",   "TC",
  "TpBL",  "TBL",   "TpBR",  "TBR",
  "TpSiL",          "TpSiR",
  "TpBC",  "TBC",   "BtFC",
  "BtFL",           "BtFR",
  "FLw",            "FRw",
  "LS",             "RS",
  "LSd",            "RSd",
  "TpLS",           "TpRS",
  "*-",
  NULL
};

/* Map from keyword index to speaker configuration */
/* Speaker configuration codes are given by the index-1 */
const char *AF_Spkr_Config_KeyVal[] = {
  "\x03",                                              /* 1.0-mono       */
  "\x01\x02",                                          /* 2.0-stereo     */
  "\x01\x02\x05\x06",                                  /* 4.0-quad       */
  "\x01\x02\x03\x04\x05\x06",                          /* 5.1-surround   */
  "\x01\x02\x03\x04\x05\x06\x0D\x0E\x11\x12",          /* 5.1+4-surround */
  "\x01\x02\x03\x04\x05\x06\x0B\x0C",                  /* 7.1-surround   */
  "\x01\x02\x03\x04\x05\x06\x0B\x0C\x0D\x0E\x11\x12",  /* 7.1+4-surround */
  NULL
};

const char *AF_Spkr_KeyVal[] = {
  "\x01",         "\x02",             /* FL          FR        */
  "\x03",         "\x04",             /* FC          LFE1      */
  "\x05",         "\x06",             /* BL          BR        */
  "\x07", "\x07", "\x08", "\x08",     /* FLc   FLC   FRc   FRC */
  "\x09",         "\x0A",             /* BC          LFE2      */
  "\x0B", "\x0B", "\x0C", "\x0C",     /* SiL   SL    SiR   SR  */
  "\x0D", "\x0D", "\x0E", "\x0E",     /* TpFL  TFL   TpFR  TFR */
  "\x0F", "\x0F", "\x10", "\x10",     /* TpFC  TFC   TpC   TC  */
  "\x11", "\x11", "\x12", "\x12",     /* TpBL  TBL   TpBR  TBR */
  "\x13",         "\x14",             /* TpSiL       TpSiR     */
  "\x15", "\x15", "\x16",             /* TpBC  TBC   BtFC      */
  "\x17",         "\x18",             /* BtFL        BtFR      */
  "\x19",         "\x1A",             /* FLw         FRw       */
  "\x1B",         "\x1C",             /* LS          RS        */
  "\x1D",         "\x1E",             /* LSd         RSd       */
  "\x1F",         "\x20",             /* TpLS        TpRS      */
  "\x21",                             /* AF_SPKR_AUX           */
};


int
AFgenSpkrConfig(const char String[], unsigned char *SpkrConfig, int MaxN)

{
  char cbuf[NCBUF+1];
  int SpkrCodeCount[AF_N_SPKR_NAMES] = {0};
  const char *p;
  char *token;
  int k, m, n, nc, L, Err;

  assert(NELEM(AF_Spkr_Config_KeyVal) == NELEM(AF_Spkr_Config_Key));
  assert(NELEM(AF_Spkr_KeyVal) == NELEM(AF_Spkr_Key) - 1);

/* Set up the token buffer */
  nc = (int) strlen(String);
  if (nc <= NCBUF)
    token = cbuf;
  else
    token = (char *) UTmalloc(nc + 1);

  Err = 0;
  SpkrConfig[0] = '\0';

  /* Check against speaker configuration names */
  /* Assumption: Configurations do not contain duplicate locations */
  STtrim(String, token);
  n = STkeyMatch(token, AF_Spkr_Config_Key);
  if (n >= 0) {
    L = (int) strlen(AF_Spkr_Config_KeyVal[n]);
    if (L <= MaxN)
      strcpy((char *) SpkrConfig, AF_Spkr_Config_KeyVal[n]); /* adds NUL */
    else {
      Err = 1;
      UTwarn("AFgenSpkrConfig - %s", AFM_XSpkr);
    }
  }
  else if (token[0] != '\0') {      /* Check for an empty string */
    /* Check against speaker names */
    p = String;
    k = 0;
    while (p != NULL) {
      p = STfindToken(p, ",", "", token, WS_DELIM, nc);
      n = STkeyMatch(token, AF_Spkr_Key);
      if (n < 0) {
        Err = 1;
        UTwarn("AFgenSpkrConfig - %s: \"%.10s\"", AFM_BadSpkr, token);
        break;
      }

      /* Check for duplicate speaker positions */
      m = (int) AF_Spkr_KeyVal[n][0];     /* Values are single bytes */
      assert(m > 0 && m <= AF_N_SPKR_NAMES);
      if (m != AF_SPKR_AUX && SpkrCodeCount[m] > 0) {
        Err = 1;
        UTwarn("AFgenSpkrConfig - %s: \"%.10s\"", AFM_DupSpkr,
                AF_Spkr_Names[m-1]);
        break;
      }
      ++SpkrCodeCount[m];

      /* Check that there is enough space */
      if (k < MaxN) {
        strcpy((char *) &SpkrConfig[k], AF_Spkr_KeyVal[n]);  /* adds NUL */
        k = k + 1;
      }
      else {
        Err = 1;
        UTwarn("AFgenSpkrConfig - %s", AFM_XSpkr);
        break;
      }
    }
  }

/* Deallocate the buffer */
  if (nc > NCBUF)
    UTfree((void *) token);

  if (Err)
    SpkrConfig[0] = '\0';

  return Err;
}
