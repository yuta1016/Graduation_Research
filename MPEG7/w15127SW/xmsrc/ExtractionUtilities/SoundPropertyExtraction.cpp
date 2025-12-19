//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// KDDI R&D Laboratories Inc., Masaru Sugano, Yasuyuki Nakajima
// (contributing organizations names)
//
// in the course of development of the MPEG-7 Experimentation Model.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation Model tools as specified by the MPEG-7 Requirements.
//
// ISO/IEC gives users of MPEG-7 free license to this software module or
// modifications thereof for use in hardware or software products claiming
// conformance to MPEG-7.
//
// Those intending to use this software module in hardware or software products
// are advised that its use may infringe existing patents. The original
// developer of this software module and his/her company, the subsequent
// editors and their companies, and ISO/IEC have no liability for use of this
// software module or modifications thereof in an implementation.
//
// Copyright is not released for non MPEG-7 conforming products. The
// organizations named above retain full right to use the code for their own
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SoundPropertyExtraction.cpp
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <math.h> 
#include <ctype.h>

#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"
#include "Utilities/readparam.h"

static int GSQCounts;
// Window table
double wndtbl[HAN_SIZE]=
      { 0.000000000, -0.000000477, -0.000000477, -0.000000477,
       -0.000000477, -0.000000477, -0.000000477, -0.000000954,
       -0.000000954, -0.000000954, -0.000000954, -0.000001431,
       -0.000001431, -0.000001907, -0.000001907, -0.000002384,
       -0.000002384, -0.000002861, -0.000003338, -0.000003338,
       -0.000003815, -0.000004292, -0.000004768, -0.000005245,
       -0.000006199, -0.000006676, -0.000007629, -0.000008106,
       -0.000009060, -0.000010014, -0.000011444, -0.000012398,
       -0.000013828, -0.000014782, -0.000016689, -0.000018120,
       -0.000019550, -0.000021458, -0.000023365, -0.000025272,
       -0.000027657, -0.000030041, -0.000032425, -0.000034809,
       -0.000037670, -0.000040531, -0.000043392, -0.000046253,
       -0.000049591, -0.000052929, -0.000055790, -0.000059605,
       -0.000062943, -0.000066280, -0.000070095, -0.000073433,
       -0.000076771, -0.000080585, -0.000083923, -0.000087261,
       -0.000090599, -0.000093460, -0.000096321, -0.000099182,
        0.000101566,  0.000103951,  0.000105858,  0.000107288,
        0.000108242,  0.000108719,  0.000108719,  0.000108242,
        0.000106812,  0.000105381,  0.000102520,  0.000099182,
        0.000095367,  0.000090122,  0.000084400,  0.000077724,
        0.000069618,  0.000060558,  0.000050545,  0.000039577,
        0.000027180,  0.000013828, -0.000000954, -0.000017166,
       -0.000034332, -0.000052929, -0.000072956, -0.000093937,
       -0.000116348, -0.000140190, -0.000165462, -0.000191212,
       -0.000218868, -0.000247478, -0.000277042, -0.000307560,
       -0.000339031, -0.000371456, -0.000404358, -0.000438213,
       -0.000472546, -0.000507355, -0.000542164, -0.000576973,
       -0.000611782, -0.000646591, -0.000680923, -0.000714302,
       -0.000747204, -0.000779152, -0.000809669, -0.000838757,
       -0.000866413, -0.000891685, -0.000915051, -0.000935555,
       -0.000954151, -0.000968933, -0.000980854, -0.000989437,
       -0.000994205, -0.000995159, -0.000991821, -0.000983715,
        0.000971317,  0.000953674,  0.000930786,  0.000902653,
        0.000868797,  0.000829220,  0.000783920,  0.000731945,
        0.000674248,  0.000610352,  0.000539303,  0.000462532,
        0.000378609,  0.000288486,  0.000191689,  0.000088215,
       -0.000021458, -0.000137329, -0.000259876, -0.000388145,
       -0.000522137, -0.000661850, -0.000806808, -0.000956535,
       -0.001111031, -0.001269817, -0.001432419, -0.001597881,
       -0.001766682, -0.001937389, -0.002110004, -0.002283096,
       -0.002457142, -0.002630711, -0.002803326, -0.002974033,
       -0.003141880, -0.003306866, -0.003467083, -0.003622532,
       -0.003771782, -0.003914356, -0.004048824, -0.004174709,
       -0.004290581, -0.004395962, -0.004489899, -0.004570484,
       -0.004638195, -0.004691124, -0.004728317, -0.004748821,
       -0.004752159, -0.004737377, -0.004703045, -0.004649162,
       -0.004573822, -0.004477024, -0.004357815, -0.004215240,
       -0.004049301, -0.003858566, -0.003643036, -0.003401756,
        0.003134727,  0.002841473,  0.002521515,  0.002174854,
        0.001800537,  0.001399517,  0.000971317,  0.000515938,
        0.000033379, -0.000475883, -0.001011848, -0.001573563,
       -0.002161503, -0.002774239, -0.003411293, -0.004072189,
       -0.004756451, -0.005462170, -0.006189346, -0.006937027,
       -0.007703304, -0.008487225, -0.009287834, -0.010103703,
       -0.010933399, -0.011775017, -0.012627602, -0.013489246,
       -0.014358521, -0.015233517, -0.016112804, -0.016994476,
       -0.017876148, -0.018756866, -0.019634247, -0.020506859,
       -0.021372318, -0.022228718, -0.023074150, -0.023907185,
       -0.024725437, -0.025527000, -0.026310921, -0.027073860,
       -0.027815342, -0.028532982, -0.029224873, -0.029890060,
       -0.030526638, -0.031132698, -0.031706810, -0.032248020,
       -0.032754898, -0.033225536, -0.033659935, -0.034055710,
       -0.034412861, -0.034730434, -0.035007000, -0.035242081,
       -0.035435200, -0.035586357, -0.035694122, -0.035758972,
        0.035780907,  0.035758972,  0.035694122,  0.035586357,
        0.035435200,  0.035242081,  0.035007000,  0.034730434,
        0.034412861,  0.034055710,  0.033659935,  0.033225536,
        0.032754898,  0.032248020,  0.031706810,  0.031132698,
        0.030526638,  0.029890060,  0.029224873,  0.028532982,
        0.027815342,  0.027073860,  0.026310921,  0.025527000,
        0.024725437,  0.023907185,  0.023074150,  0.022228718,
        0.021372318,  0.020506859,  0.019634247,  0.018756866,
        0.017876148,  0.016994476,  0.016112804,  0.015233517,
        0.014358521,  0.013489246,  0.012627602,  0.011775017,
        0.010933399,  0.010103703,  0.009287834,  0.008487225,
        0.007703304,  0.006937027,  0.006189346,  0.005462170,
        0.004756451,  0.004072189,  0.003411293,  0.002774239,
        0.002161503,  0.001573563,  0.001011848,  0.000475883,
       -0.000033379, -0.000515938, -0.000971317, -0.001399517,
       -0.001800537, -0.002174854, -0.002521515, -0.002841473,
        0.003134727,  0.003401756,  0.003643036,  0.003858566,
        0.004049301,  0.004215240,  0.004357815,  0.004477024,
        0.004573822,  0.004649162,  0.004703045,  0.004737377,
        0.004752159,  0.004748821,  0.004728317,  0.004691124,
        0.004638195,  0.004570484,  0.004489899,  0.004395962,
        0.004290581,  0.004174709,  0.004048824,  0.003914356,
        0.003771782,  0.003622532,  0.003467083,  0.003306866,
        0.003141880,  0.002974033,  0.002803326,  0.002630711,
        0.002457142,  0.002283096,  0.002110004,  0.001937389,
        0.001766682,  0.001597881,  0.001432419,  0.001269817,
        0.001111031,  0.000956535,  0.000806808,  0.000661850,
        0.000522137,  0.000388145,  0.000259876,  0.000137329,
        0.000021458, -0.000088215, -0.000191689, -0.000288486,
       -0.000378609, -0.000462532, -0.000539303, -0.000610352,
       -0.000674248, -0.000731945, -0.000783920, -0.000829220,
       -0.000868797, -0.000902653, -0.000930786, -0.000953674,
        0.000971317,  0.000983715,  0.000991821,  0.000995159,
        0.000994205,  0.000989437,  0.000980854,  0.000968933,
        0.000954151,  0.000935555,  0.000915051,  0.000891685,
        0.000866413,  0.000838757,  0.000809669,  0.000779152,
        0.000747204,  0.000714302,  0.000680923,  0.000646591,
        0.000611782,  0.000576973,  0.000542164,  0.000507355,
        0.000472546,  0.000438213,  0.000404358,  0.000371456,
        0.000339031,  0.000307560,  0.000277042,  0.000247478,
        0.000218868,  0.000191212,  0.000165462,  0.000140190,
        0.000116348,  0.000093937,  0.000072956,  0.000052929,
        0.000034332,  0.000017166,  0.000000954, -0.000013828,
       -0.000027180, -0.000039577, -0.000050545, -0.000060558,
       -0.000069618, -0.000077724, -0.000084400, -0.000090122,
       -0.000095367, -0.000099182, -0.000102520, -0.000105381,
       -0.000106812, -0.000108242, -0.000108719, -0.000108719,
       -0.000108242, -0.000107288, -0.000105858, -0.000103951,
        0.000101566,  0.000099182,  0.000096321,  0.000093460,
        0.000090599,  0.000087261,  0.000083923,  0.000080585,
        0.000076771,  0.000073433,  0.000070095,  0.000066280,
        0.000062943,  0.000059605,  0.000055790,  0.000052929,
        0.000049591,  0.000046253,  0.000043392,  0.000040531,
        0.000037670,  0.000034809,  0.000032425,  0.000030041,
        0.000027657,  0.000025272,  0.000023365,  0.000021458,
        0.000019550,  0.000018120,  0.000016689,  0.000014782,
        0.000013828,  0.000012398,  0.000011444,  0.000010014,
        0.000009060,  0.000008106,  0.000007629,  0.000006676,
        0.000006199,  0.000005245,  0.000004768,  0.000004292,
        0.000003815,  0.000003338,  0.000003338,  0.000002861,
        0.000002384,  0.000002384,  0.000001907,  0.000001907,
        0.000001431,  0.000001431,  0.000000954,  0.000000954,
        0.000000954,  0.000000954,  0.000000477,  0.000000477,
        0.000000477,  0.000000477,  0.000000477,  0.000000477};

//----------------------------------------------------------------------------
using namespace XM;

const UUID SoundPropertyExtractionInterface::myID = UUID("");
const char *SoundPropertyExtractionInterface::myName = "SoundPropertyDSExtractionInterface";

const UUID SoundPropertyExtractionInterfaceABC::myID = UUID("");

//============================================================================
SoundPropertyExtractionInterfaceABC::SoundPropertyExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SoundPropertyExtractionInterface::SoundPropertyExtractionInterface(SoundPropertyExtractionTool *aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SoundPropertyExtractionInterface::~SoundPropertyExtractionInterface()
{
}

//----------------------------------------------------------------------------
void SoundPropertyExtractionInterface::destroy()
{
	delete m_ExtractionTool;
}

//----------------------------------------------------------------------------
int SoundPropertyExtractionInterface::SetSourceMedia(MediaDBElement* media)
{
	return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
int SoundPropertyExtractionInterface::SetDSInterface(
GenericDSInterfaceABC *aGenericDSInterface)
{
	return m_ExtractionTool->SetDSInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC *SoundPropertyExtractionInterface::
GetDSInterface()
{
	return m_ExtractionTool->GetDSInterface();
}

//----------------------------------------------------------------------------
ULONG SoundPropertyExtractionInterface::InitExtracting(void)
{
	return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
ULONG SoundPropertyExtractionInterface::StartExtracting(void)
{
	return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
ULONG SoundPropertyExtractionInterface::PostExtracting(void)
{
	return m_ExtractionTool->PostExtracting();
}

//----------------------------------------------------------------------------
const UUID& SoundPropertyExtractionInterface::GetInterfaceID(void)
{ 
	return myID;
}

//----------------------------------------------------------------------------
const char *SoundPropertyExtractionInterface::GetName(void)
{ 
	return myName;
}

//----------------------------------------------------------------------------
aSummaryList::aSummaryList():
start_h(0),
start_m(0),
start_s(0),
start_nn(0),
duration_h(0),
duration_m(0),
duration_s(0),
duration_nn(0)
{
	memset((char *)OriginalFileName, 0L, sizeof(OriginalFileName));
	memset((char *)aSummaryFileName, 0L, sizeof(aSummaryFileName));
	memset((char *)Title, 0L, sizeof(Title));
}

//----------------------------------------------------------------------------
aSummaryList::~aSummaryList()
{
}

//----------------------------------------------------------------------------
ExtractAudioSummary::ExtractAudioSummary():
SQCounts(0),
m_SList(NULL)
{
	audioFileName = (char *)malloc(sizeof(char)*128);
	memset(audioFileName, 0L, sizeof(audioFileName));	
}

//----------------------------------------------------------------------------
ExtractAudioSummary::~ExtractAudioSummary()
{
}

//----------------------------------------------------------------------------
int ExtractAudioSummary::aSummaryExtracting()
{
	AFILE *AFp, *AFpO;
	FILE *fpinfo;
	struct CP_FIpar FI, FIx;
	struct CP_FOpar FO;
	float SfreqIn, Fbuf[5120];
	float first_Fbuf[2304];	// 1frame(1152) * 2ch
	double SfreqOut;
	long int Nsamp = 0, Nframe = 0, NchanIn = 0, NchanOut = 0, 
		     StartS = 0, MaxNfr = 0, offr = 0, offs = 0, Nrem = 0, Nj = 0;
	
	double W_DC = 0.1, W_VX = 0.2, W_MI = 0.7;
	double sb_dc = 0, sb_vx = 0, sb_mi = 0;
	double sbpower[2][MAX_SB], ele, frame_rate;
	double sbw_max = 0, tmp_sbw = 0;
	int Fformat, eof, Nr, Nv;
	int	frame_count = 0, prev_tsec = 0, tsec, sabi_time = 0;
	int s, sb, ch;
	int LRLen, FbufLen;
	short *temp_pcm[2];
	char *Mybuff;
	char *tempext;

	for (ch = 0; ch < 2; ch++)
		for (sb = 0; sb < MAX_SB; sb++)
			sbpower[ch][sb] = 0;

	Mybuff = (char *)malloc(sizeof(char)*MAX_BUFSIZE);
	temp_pcm[0] = (short *)malloc(sizeof(short)*4608);
	temp_pcm[1] = (short *)malloc(sizeof(short)*4608);
	memset(temp_pcm[0], 0, sizeof(temp_pcm[0]));
	memset(temp_pcm[1], 0, sizeof(temp_pcm[1]));
	memset(first_Fbuf, 0, sizeof(first_Fbuf));
	
	fpinfo = stdout;

	AFopt_NsampND = 1;
	FIpar_INIT (&FIx);
	FOpar_INIT (&FO);

	strcpy(FIx.Fname, audioFileName);
	FIx.Lim[0] = 0;
	FIx.Lim[1] = 0;
	FIx.Ftype = FT_AUTO;		// AFsp checks file format automatically
	
	FI = FIx;

	// The output (summary) file name
	string asummaryfilename;

	asummaryfilename = audioFileName;

	strcpy(m_SList->OriginalFileName, audioFileName);

	asummaryfilename.erase(asummaryfilename.rfind("."), 
		asummaryfilename.size() - asummaryfilename.rfind("."));

	sprintf(Mybuff, "_sum%d", SQCounts);
	
	asummaryfilename += Mybuff;
	
	// Open input file
	AFopt_NHpar = FI.NHpar;
	AFopt_FtypeI = FI.Ftype;
	FLpathList(FI.Fname, AFPATH_ENV, FI.Fname);
	AFp = AFopenRead(FI.Fname, &Nsamp, &NchanIn, &SfreqIn, fpinfo);
	AFp->ScaleF *= FI.Gain;

	// Analysis pcm data
	frame_rate = (double)(SfreqIn/FRAME_SIZE);	

	// Create struct for summary extraction
	SearchSummary sSum;
	PcmSbConverter * PSC = sSum.PcmSseCreateSbConverter((unsigned int)NchanIn, (unsigned int)SfreqIn);

	int AFrBuffLen;
	AFrBuffLen = FRAME_SIZE * AFp->Nchan;  

	for(int MediaDrSec = 0; MediaDrSec < (int)(AFp->Nsamp / AFrBuffLen); MediaDrSec++){

		AFreadData(AFp,  offr, first_Fbuf, AFrBuffLen);
		offr += AFrBuffLen;

		for(LRLen = 0, FbufLen = 0; FbufLen < AFrBuffLen ; LRLen++, FbufLen += AFp->Nchan){
			temp_pcm[0][LRLen] = (short)first_Fbuf[FbufLen];
			if(AFp->Nchan == 2)
				temp_pcm[1][LRLen] = (short)first_Fbuf[FbufLen + 1];
		}

		PSC->sample_size = FRAME_SIZE;

		PSC->pcm[0] = temp_pcm[0];
		if(AFp->Nchan == 2)
			PSC->pcm[1] = temp_pcm[1];

		sSum.PcmSseConvertSubband(PSC);

		// Add subband data
		for (ch = 0; ch < AFp->Nchan; ch++)
			for (sb = 0; sb < MAX_SB; sb++)
				for (s = 0; s < MAX_GROUP; s++) {
					ele = PSC->sample[s][ch][sb];
					sbpower[ch][sb] += (ele*ele);
				}

		// Calculate elapsed seconds
		frame_count++;

		tsec = (int)((double)frame_count/frame_rate);

		// Check subband power per second
		if (tsec != prev_tsec) {

			// DC
			sb_dc = sbpower[0][0];

			// Vx
			sb_vx = sbpower[0][1] + sbpower[0][2] + sbpower[0][3];

			// MI
			for (sb = 4; sb < MAX_SB; sb++)
				sb_mi += sbpower[0][sb];

			// Weight subband energy
			tmp_sbw = W_DC*sb_dc + W_VX*sb_vx + W_MI*sb_mi;

			// Reset subband energy
			for (sb = 0; sb < MAX_SB; sb++)
				sbpower[0][sb] = sbpower[1][sb] = 0;

			sb_dc = sb_vx = sb_mi = 0;

			// Update sabi time
			if (tmp_sbw >= sbw_max) {
				sabi_time = tsec - 1;
				sbw_max = tmp_sbw;
			}

			prev_tsec = tsec;	
		}

	} // MediaDrFrame
	
	sSum.PcmSseDestroySbConverter(PSC);

	if(sabi_time > 3600)	m_SList->start_h = sabi_time / 3600;
	
	if(sabi_time > 60)
		m_SList->start_m = (sabi_time - (m_SList->start_h * 3600)) / 60;
	
	m_SList->start_s = sabi_time - (m_SList->start_m * 60);

	// Duration of summary clip
	m_SList->duration_s = 15;

	// Set samples for summary
	FI.Lim[0] = (int)AFp->Sfreq * sabi_time;
	FI.Lim[1] = (int)AFp->Sfreq * (sabi_time + 15);

	AFp->ScaleF *= FI.Gain;
	NchanOut = NchanIn;
	Nframe = AOnFrame (&AFp, &FI, 1, AF_NFRAME_UNDEF);
	AFopt_Nframe = Nframe;
	SfreqOut = AFp->Sfreq;

	fprintf(fpinfo, "\n");

	// Set title
	if ((tempext = (char *)strrchr(asummaryfilename.c_str(), '\\')) == NULL &&
		(tempext = (char *)strrchr(asummaryfilename.c_str(), '/')) == NULL)
			tempext = (char *)asummaryfilename.c_str();
	else
		tempext += 1;
	
	strcpy(m_SList->Title, tempext);
	
	// Open output file
	if ((tempext = strrchr(audioFileName, '.')) != NULL)
		asummaryfilename += tempext;

	strcpy(FO.Fname, asummaryfilename.c_str());
	
	FO.Ftype = FTW_UNDEF;
	Fformat = AOsetFormat(&FO, &AFp, 1);
	AFsetHinfo(FO.Hinfo);
	AFpO = AFopenWrite(FO.Fname, Fformat, NchanOut, SfreqOut, fpinfo);
	StartS = FI.Lim[0];
	MaxNfr = MYMAX(FI.Lim[1] - StartS + 1L, 0L);
	eof = (Nframe == -1L);
	Nj = AFp->Nchan;
	Nrem = Nj * MYMIN(Nframe, MaxNfr);
	offr = 0L;

	while(Nrem > 0L){
		
		Nv = (int) MYMIN(Nrem, 5120);
	
		offs = offr + Nj * StartS;

		// Seek summary position
		Nr = AFreadData(AFp, offs, Fbuf, Nv);
		
		if (Nr < Nv && eof) {
			offs = offr + Nr;
			Nrem = Nj * ICEILV (offs, Nj) - offr;
			Nv = MYMIN (Nrem, Nv);
		}
		Nrem -= Nv;
		offr += Nv;

		// Write summary samples
		AFwriteData(AFpO, Fbuf, Nv);
	}

	// Close audio file
	AFclose(AFp);
	AFclose(AFpO);

	strcpy(m_SList->aSummaryFileName, asummaryfilename.c_str());
	//strcpy(m_SList->Title, Mybuff);

	free(temp_pcm[0]);
	temp_pcm[0] = NULL;
	free(temp_pcm[1]);
	temp_pcm[1] = NULL;
	free((char *)audioFileName);
	audioFileName = NULL;
	free((char *)Mybuff);
	Mybuff = NULL;

	return 0;
}

//----------------------------------------------------------------------------
ULONG ExtractAudioSummary::InitExtracting(MediaDBElement* media)
{
	strcpy(audioFileName, (char *)media->GetFilename());

	return true;
}

//----------------------------------------------------------------------------
ULONG ExtractAudioSummary::StartExtracting(aSummaryList* m_Summarylist)
{
	m_SList = m_Summarylist;
	this->aSummaryExtracting();

	return true;
}	

//============================================================================
SoundPropertyExtractionTool::SoundPropertyExtractionTool():
m_Interface(this),
m_DSInterface(0),
m_ExtractASummary(0),
m_aSummarylist(0),
m_numList(0)
{
	GSQCounts = 0;
	SetExposedInterface(&m_Interface);
	SetASummaryList(0);
}

//----------------------------------------------------------------------------

SoundPropertyExtractionTool::SoundPropertyExtractionTool(GenericDSInterfaceABC *theDSInterface):
m_Interface(this),
m_DSInterface(0),
m_ExtractASummary(0),
m_aSummarylist(0),
m_numList(0)
{
	GSQCounts = 0;
	SetDSInterface(theDSInterface);
	SetExposedInterface(&m_Interface);
	SetASummaryList(0);
}

//----------------------------------------------------------------------------
SoundPropertyExtractionTool::~SoundPropertyExtractionTool()
{
}

//----------------------------------------------------------------------------
int SoundPropertyExtractionTool::SetDSInterface(GenericDSInterfaceABC *aGenericDSInterfaceABC)
{
	if(!aGenericDSInterfaceABC) return -1;

	m_DSInterface = aGenericDSInterfaceABC;
	
	return 0;
}

//----------------------------------------------------------------------------
int SoundPropertyExtractionTool::SetSourceMedia(MediaDBElement* media)
{
	if(m_ExtractASummary)
		delete m_ExtractASummary;

	m_ExtractASummary = new ExtractAudioSummary();
	assert(m_ExtractASummary);

	return m_ExtractASummary->InitExtracting(media);
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC *SoundPropertyExtractionTool::GetDSInterface()
{
	return m_DSInterface;
}

//----------------------------------------------------------------------------
ULONG SoundPropertyExtractionTool::InitExtracting(void)
{
	return 0;	
}

//----------------------------------------------------------------------------
ULONG SoundPropertyExtractionTool::StartExtracting(void)
{
	m_numList = GSQCounts;
	
	SetASummaryList(m_numList+1);
	
	GSQCounts++;

	m_ExtractASummary->SQCounts = GSQCounts;

	return m_ExtractASummary->StartExtracting(m_aSummarylist[m_numList-1]);
}

//----------------------------------------------------------------------------
ULONG SoundPropertyExtractionTool::PostExtracting(void)
{
	SoundPropertyDOM generateXMLdoc(m_aSummarylist, m_numList);

	generateXMLdoc.CreateSummarization(m_DSInterface);

	return 0;
}

//----------------------------------------------------------------------------
const UUID& SoundPropertyExtractionTool::GetObjectID(void)
{ return myID;}

//----------------------------------------------------------------------------
const char *SoundPropertyExtractionTool::GetName(void)
{ return myName;}

//----------------------------------------------------------------------------
bool SoundPropertyExtractionTool::SupportsPush(void)
{
	return true;
}

//----------------------------------------------------------------------------
bool SoundPropertyExtractionTool::SupportsPull(void)
{	
	return false;
}

//----------------------------------------------------------------------------
SoundPropertyExtractionInterfaceABC *SoundPropertyExtractionTool::GetInterface(void)
{
	return &m_Interface;	
}

//----------------------------------------------------------------------------
bool SoundPropertyExtractionTool::SetASummaryList(ULONG numList)
{
	aSummaryList **aSummarylist = new aSummaryList*[numList];
	assert(aSummarylist);
	if(numList)
		memcpy(aSummarylist, m_aSummarylist, m_numList * sizeof(aSummaryList*));
	delete [] m_aSummarylist;
	m_aSummarylist = aSummarylist;

	while(m_numList < numList){
		m_aSummarylist[m_numList] = new aSummaryList();
		assert(m_aSummarylist[m_numList]);
		m_numList++;
	}

	return true;
}

//============================================================================
SoundPropertyDOM::SoundPropertyDOM(aSummaryList **m2_aSummarylist, ULONG NumList):
listsnum(0),
m_bSList(m2_aSummarylist),
m_bnumList(NumList)
{
	TimeStringSrc[0] = '\0';
	TimeStringDurSrc[0] = '\0';
}

//----------------------------------------------------------------------------
SoundPropertyDOM::~SoundPropertyDOM()
{
}

//----------------------------------------------------------------------------
GenericDS SoundPropertyDOM::CreateSummarization(GenericDSInterfaceABC *DSdoc)
{
	// <Summary>
	GenericDS RootParent = DSdoc->CreateChild("Summarization");

	//		<SequentialSummary>
	GenericDS SeqSumm = RootParent.CreateChild("SequentialSummary");
	
	//		<SequentialSummary name="test">
	//SeqSumm.SetAttribute("name","all");
	
	for(listsnum = 0; listsnum < (int)m_bnumList; listsnum++)
		CreateSoundProperty(SeqSumm);

	return RootParent;
}

//----------------------------------------------------------------------------
GenericDS SoundPropertyDOM::CreateSoundProperty(GenericDS parent)
{
		//  <SoundProperty>
		//                 -> <AudioSummaryComponent>
		GenericDS SndProp = parent.CreateChild("AudioSummaryComponent");

		//				<Title>
		GenericDS Title = SndProp.CreateChild("Title");
		//              <Title xml:lang="en">
		//Title.SetAttribute("xml:lang","en">;
		
		//              <Title xml:lang="en">??????<Title/>
		Title.SetValue(m_bSList[listsnum]->Title);
		
		// <SourceLocator>
		//                -> <AudioSourceLocator>
		GenericDS SrcLoc = SndProp.CreateChild("AudioSourceLocator");
		CreateMediaURI(SrcLoc, m_bSList[listsnum]->OriginalFileName);
		
		// <SourceTime>
		//             -> <ComponentSourceTime>
		GenericDS SrcTime = SndProp.CreateChild("ComponentSourceTime");
		CreateMediaTimePoint(SrcTime);
		CreateMediaDuration(SrcTime);
		
		//              <SyncTime> -- Not implemented
		//GenericDS SyncTime = SndProp.CreateChild("SyncTime");
		//CreateMediaTimePoint(SyncTime);
		//CreateMediaDuration(SyncTime);

		//				<SoundLocator> -- Only separated summary file supported
		GenericDS SndLoc = SndProp.CreateChild("SoundLocator");
		CreateMediaURI(SndLoc, m_bSList[listsnum]->aSummaryFileName);
		//CreateMediaURL(SndLoc, m_bSList[listsnum]->OriginalFileName);
		//CreateMediaTimePoint(SndLoc);
		//CreateMediaDuration(SndLoc);
		
		return SndProp;
}

//----------------------------------------------------------------------------
GenericDS SoundPropertyDOM::CreateMediaTimePoint(GenericDS parent)
{
	char temp[MAX_BUFSIZE];
	temp[0] = '\0';

	// <MediaRelTimePoint>
	GenericDS SrcTimeP = parent.CreateChild("MediaRelTimePoint");

	// This description for time elements is based on MDS CD.
	strcpy(TimeStringSrc, "PT");
	if(m_bSList[listsnum]->start_h)
		sprintf(TimeStringSrc, "%s%02dH", TimeStringSrc, m_bSList[listsnum]->start_h);
	if(m_bSList[listsnum]->start_m)
		sprintf(TimeStringSrc, "%s%02dM", TimeStringSrc, m_bSList[listsnum]->start_m);
	if(m_bSList[listsnum]->start_s)
		sprintf(TimeStringSrc, "%s%02dS", TimeStringSrc, m_bSList[listsnum]->start_s);
	
	SrcTimeP.SetValue((char *)TimeStringSrc);
	memset((char *)TimeStringSrc, 0L, sizeof(TimeStringSrc));

	return SrcTimeP;
}

//----------------------------------------------------------------------------
GenericDS SoundPropertyDOM::CreateMediaDuration(GenericDS parent)
{
	// <MediaDuration>
	GenericDS SrcDur = parent.CreateChild("MediaDuration");
	
	// This description for time elements is based on MDS CD.
	strcpy(TimeStringDurSrc, "PT");
	if(m_bSList[listsnum]->duration_h)
		sprintf(TimeStringDurSrc, "%s%02dH", TimeStringDurSrc, m_bSList[listsnum]->duration_h);
	if(m_bSList[listsnum]->duration_m)
		sprintf(TimeStringDurSrc, "%s%02dM", TimeStringDurSrc, m_bSList[listsnum]->duration_m);
	if(m_bSList[listsnum]->duration_s)
		sprintf(TimeStringDurSrc, "%s%02dS", TimeStringDurSrc, m_bSList[listsnum]->duration_s);
	
	SrcDur.SetValue((char *)TimeStringDurSrc);
	memset((char *)TimeStringDurSrc, 0L, sizeof(TimeStringDurSrc));

	return SrcDur;
}

//----------------------------------------------------------------------------
GenericDS SoundPropertyDOM::CreateMediaURI(GenericDS parent, char *m_str)
{
	char temp[MAX_BUFSIZE];
	temp[0] = '\0';

	// <MediaURI> file://temp </MediaURI>
	GenericDS SrcURI = parent.CreateChild("MediaURI");
	strcpy(temp, "file://");	strcat(temp, m_str);

	SrcURI.SetValue((char *)temp);

	return SrcURI;	
}

//----------------------------------------------------------------------------
GenericDS SoundPropertyDOM::CreateElem(GenericDS parent, const char *tag, int int_val)
{
	GenericDS Elem = parent.CreateChild(tag);

	if(int_val != -1)
		Elem.SetValue(int_val);

	return Elem;
}

//============================================================================
//		SearchSummary Class
//============================================================================
SearchSummary::SearchSummary()
{
}

//----------------------------------------------------------------------------
SearchSummary::~SearchSummary()
{
}

//----------------------------------------------------------------------------
PcmSbConverter *SearchSummary::PcmSseCreateSbConverter(unsigned int ch, unsigned int Fs)
{
	PcmSbConverter	*converter;
	int i, k;

	// Allocate PCM to Subband converter
	if ((converter = (PcmSbConverter *)malloc(sizeof(PcmSbConverter))) == NULL)
	  {
		fprintf(stderr, "Can't allocate %u bytes memory.\n", sizeof(PcmSbConverter));
		return(NULL);
	}

	memset((void *)converter, 0, sizeof(PcmSbConverter));

	for (i = 0; i < 32; i++)
		for (k = 0; k < 64; k++) {
			if ((converter->filter[i][k] = 1e9*cos((double)((2*i+1)*(16-k)*PI64))) >= 0)
				modf(converter->filter[i][k]+0.5, &converter->filter[i][k]);
			else
				modf(converter->filter[i][k]-0.5, &converter->filter[i][k]);
			converter->filter[i][k] *= 1e-9;
		}

	// Set channel number
	converter->nch = (int)ch;

	// Set sampling frequency
	converter->sampling_freq = (int)Fs;

	return(converter);
}

//----------------------------------------------------------------------------
void SearchSummary::PcmSseDestroySbConverter(PcmSbConverter *converter)
{
	if (converter != NULL) {
		free(converter);
	}
}

//----------------------------------------------------------------------------
int SearchSummary::PcmSseConvertSubband(PcmSbConverter *pcm_conv)
{
	double win_que[5][HAN_SIZE];
	int	i, j, k;

	if ((pcm_conv->pcm[0] == NULL) || ((pcm_conv->nch == 2) &&
	  (pcm_conv->pcm[1] == NULL))) {
		return(-1);
	}
	else if (pcm_conv->sample_size == 0) {
		return(1);
	}

	// Check shortage buffer
	if (pcm_conv->sample_size < FRAME_SIZE) {
		for (k = 0; k < pcm_conv->nch; k++)
			for (i = (int)pcm_conv->sample_size; i < FRAME_SIZE; i++)
				*(pcm_conv->pcm[k] + i) = 0;

		pcm_conv->sample_size = FRAME_SIZE;
	}

	for (i = 0; i < 3; i++)
		for (j = 0; j < 12; j++)
			for (k = 0; k < pcm_conv->nch; k++) {

				// Calculate window subband
				window_subband(&(pcm_conv->pcm[k][i*384 + j*32]),
				    &(win_que[k][0]), k);

				filter_subband(pcm_conv->filter, &(win_que[k][0]),
				  &(pcm_conv->sample)[i*12 + j][k][0]);
			}

	// Adjust buffer position
	pcm_conv->sample_size -= FRAME_SIZE;

	return(0);
}

//----------------------------------------------------------------------------
void SearchSummary::window_subband (short *buffer, double *z, int k)
{
	double x[2][HAN_SIZE];
	int i;
	static int off[14]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    for (i = 0; i < 32; i++)
		x[k][31-i+off[k]] = (double) (*buffer)++ / DSCALE;
	
	for (i = 0; i < HAN_SIZE; i++)
		z[i] = x[k][(i+off[k])&HAN_SIZE-1] * wndtbl[i];
	off[k] += 480;
	off[k] &= HAN_SIZE-1;
}

//----------------------------------------------------------------------------
void SearchSummary::filter_subband(double m[MAX_SB][64], double *z, double *s)
{
	double y[64];
	int i, k;
	double sum1, sum2;

	// Window
	for (i=0; i<64; i++)
	{
		for (k=0, sum1 = 0.0; k<8; k++)
			sum1 += z[i+64*k];
		y[i] = sum1;
	}

   // Filter
	for (i=0; i<MAX_SB/2; i++)
	{
		for (k=0, sum1=0.0, sum2=0.0; k<16;)
		{
			sum1 += m[i][k] * (y[k]+y[32-k]); 
			sum2 += m[i][k+1] * (y[k+1]+y[31-k]); 
			sum2 += m[i][k+33] * (y[k+33]-y[63-k]); 
			sum1 += m[i][k+34] * (y[k+34]-y[62-k]); 
			k+=2;
		}
		sum1 += m[i][16]*y[16] - m[i][48]*y[48]; 

		s[i]    = sum1 + sum2;
		s[31-i] = sum1 - sum2;
	}

}
