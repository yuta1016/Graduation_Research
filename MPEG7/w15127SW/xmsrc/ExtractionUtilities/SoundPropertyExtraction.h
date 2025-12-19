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
// Applicable File Name:  SoundPropertyExtraction.h
//

#ifndef __SOUNDPROPERTYEXTRACTION_H__
#define __SOUNDPROPERTYEXTRACTION_H__

#include <assert.h>
#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Media/Media.h"

typedef unsigned int UINT;
typedef unsigned long ULONG;

#define CP_FIpar AO_FIpar
#define CP_FOpar AO_FOpar
#define PROGRAM "SoundPropertyExtraction"
#define AFPATH_ENV	"$AUDIOPATH"

#define MYMIN(a, b)	(((a) < (b)) ? (a) : (b))
#define MYMAX(a, b)	(((a) > (b)) ? (a) : (b))

#define ICEILV(n, m)	(((n) + ((m) - 1)) / (m))

#define MAX_BUFSIZE 255

#ifndef PI
#define PI		(3.14159265358979)
#endif

#ifndef PI64
#define PI64	PI/64
#endif

#define HAN_SIZE	(512)
#define MAX_GROUP	(36)
#define MAX_SB		(32)
#define FRAME_SIZE	(1152)

#define DSCALE 32768

extern double wndtbl[HAN_SIZE];

namespace XM
{
	
//============================================================================
// PCM to subband converter structure
typedef struct pcm_sb_converter {
  double sample[36][2][MAX_SB];
  double filter[MAX_SB][64];
  int sampling_freq;
  int nch;
  unsigned int pcm_pos;
  unsigned int sample_size;
  short *pcm[2];
} PcmSbConverter;

class SoundPropertyExtractionTool;

//============================================================================
class SoundPropertyExtractionInterfaceABC: public I_InterfaceABC
{
public:
	SoundPropertyExtractionInterfaceABC();
	virtual ~SoundPropertyExtractionInterfaceABC() {};
	
	virtual void destroy(void) = 0;

	virtual int SetSourceMedia(MediaDBElement* media) = 0;
	virtual int SetDSInterface(GenericDSInterfaceABC *aGenericDSInterface) = 0;
    virtual GenericDSInterfaceABC * GetDSInterface() = 0;

	virtual ULONG InitExtracting(void) = 0;
	virtual ULONG StartExtracting(void) = 0;
	virtual ULONG PostExtracting(void) = 0;
	
	static const UUID myID;
};

//============================================================================
class SoundPropertyExtractionInterface: 
	public SoundPropertyExtractionInterfaceABC
{
public:
	SoundPropertyExtractionInterface(
		SoundPropertyExtractionTool * aTool);
	virtual ~SoundPropertyExtractionInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy(void);

	virtual int SetSourceMedia(MediaDBElement* media);
	virtual int SetDSInterface(GenericDSInterfaceABC
					   *aGenericDSInterface);
    virtual GenericDSInterfaceABC * GetDSInterface();

	virtual ULONG InitExtracting(void);
	virtual ULONG StartExtracting(void);
	virtual ULONG PostExtracting(void);

	static const UUID myID;
    static const char *myName;

private:
	SoundPropertyExtractionTool *m_ExtractionTool;

};

//============================================================================
class aSummaryList
{
public:
	aSummaryList();
	~aSummaryList();

	char OriginalFileName[MAX_BUFSIZE];
	char aSummaryFileName[MAX_BUFSIZE];
	char Title[MAX_BUFSIZE];
	int start_h;
	int start_m;
	int start_s;
	int start_nn;
	int duration_h;
	int duration_m;
	int duration_s;
	int duration_nn;
};

//============================================================================
//sth why can´t these clases be private methodes of the extarction class
//============================================================================
class ExtractAudioSummary
{
	friend class aSummaryList;
public:
	ExtractAudioSummary();
	~ExtractAudioSummary();
	int aSummaryExtracting(void);
	ULONG InitExtracting(MediaDBElement* media);
	ULONG StartExtracting(aSummaryList* m_Summarylist);
	int SQCounts;

private:
	char *audioFileName;
	aSummaryList *m_SList;
};


//============================================================================
class SoundPropertyExtractionTool : public DescriptorExtractor
{
	friend class SoundPropertyExtractionInterface;
	friend class aSummaryList;
	friend class ExtractAudioSummary; 
	friend class SoundPropertyDOM;
public:
	SoundPropertyExtractionTool();
	SoundPropertyExtractionTool(GenericDSInterfaceABC *theDSInterface);
	
	virtual ~SoundPropertyExtractionTool();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(MediaDBElement* media);

	virtual int SetDSInterface(GenericDSInterfaceABC *aGenericDSInterfaceABC);
	virtual GenericDSInterfaceABC * GetDSInterface();

	virtual ULONG InitExtracting(void);
	virtual ULONG StartExtracting(void);
	virtual ULONG PostExtracting(void);
    
	virtual SoundPropertyExtractionInterfaceABC *GetInterface(void);

private:
	bool SetASummaryList(ULONG numList);
	SoundPropertyExtractionInterface m_Interface;
	GenericDSInterfaceABC *m_DSInterface;

	ExtractAudioSummary *m_ExtractASummary;
	aSummaryList **m_aSummarylist;
	ULONG m_numList;
};

class SoundPropertyDOM
{
	friend class aSummaryList;
public:
	
	SoundPropertyDOM(aSummaryList **m2_aSummarylist, ULONG NumList);
	~SoundPropertyDOM();
	
	int listsnum;
	GenericDS CreateSummarization(GenericDSInterfaceABC *DSdoc);
	GenericDS CreateSoundProperty(GenericDS parent);

private:
	char TimeStringSrc[32];
	char TimeStringDurSrc[32];
	aSummaryList **m_bSList;
	ULONG m_bnumList;

	GenericDS CreateMediaTimePoint(GenericDS parent);
	GenericDS CreateMediaDuration(GenericDS parent);
	GenericDS CreateMediaURI(GenericDS parent, char *m_str);
	GenericDS CreateElem(GenericDS parent, const char *tag,
			     int int_val = -1);


};

//============================================================================
class SearchSummary
{
public:
	SearchSummary();
	~SearchSummary();

	PcmSbConverter *PcmSseCreateSbConverter(unsigned int ch, unsigned int Fs);
	void PcmSseDestroySbConverter(PcmSbConverter *converter);
	int PcmSseConvertSubband(PcmSbConverter *converter);

private:
	void window_subband (short *buffer, double *z, int k);
	void filter_subband(double m[MAX_SB][64], double *z, double *s);
	
};

};      // namespace XM

#endif  // __SOUNDPROPERTYEXTRACTION_H__
