///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Dulce Ponceleon (Almaden Research Center, IBM, USA)
// Jan H. Pieper   (Almaden Research Center, IBM, USA)
//
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
// Applicable File Name:  SeqSummaryKeywordsExtraction.cpp
//
#ifndef __SEQSUMMARYKEYWORDSEXTRACTION_H__
#define __SEQSUMMARYKEYWORDSEXTRACTION_H__


#include "BasicArchitecture/XMInterfaces.h"               /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                  /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "momusys.h"
#include "intra.h"

#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"

#include <fstream>


namespace XM
{

// Forward Declarations:
class SeqSummaryKeywordsExtractionTool;

//=============================================================================
class SeqSummaryKeywordsExtractionInterfaceABC: public I_InterfaceABC
{
public:
	SeqSummaryKeywordsExtractionInterfaceABC();
	virtual ~SeqSummaryKeywordsExtractionInterfaceABC() {};

	virtual void destroy() = 0;

	//virtual int SetSourceMedia(MomVop *media) = 0;
	//virtual int SetSourceMediaFrameNumber(unsigned long FrameNo) = 0;
	virtual int SetSourceMediaFilename(char *aFilename) = 0;

	virtual int SetDescriptorInterface(GenericDSInterfaceABC
					   *ParentDSInterface) = 0;

	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(void) = 0;
	virtual unsigned long NewSequence(void) = 0;


	static const UUID myID;
};

//=============================================================================
class SeqSummaryKeywordsExtractionInterface: 
  public SeqSummaryKeywordsExtractionInterfaceABC
{
public:
	SeqSummaryKeywordsExtractionInterface(SeqSummaryKeywordsExtractionTool* aTool);
	virtual ~SeqSummaryKeywordsExtractionInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	//virtual int SetSourceMedia(MomVop *media);
	//virtual int SetSourceMediaFrameNumber(unsigned long FrameNo);
	virtual int SetSourceMediaFilename(char *aFilename);

	virtual int SetDescriptorInterface(GenericDSInterfaceABC
					   *ParentDSInterface);

	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long NewSequence(void);

	static const UUID myID;
	static const char *myName;

private:
	SeqSummaryKeywordsExtractionTool *m_ExtractionTool;
};

//=============================================================================
class SeqSummaryKeywordsExtractionTool: public DescriptorExtractor
{
friend class SeqSummaryKeywordsExtractionInterface;
public:
	SeqSummaryKeywordsExtractionTool();
	SeqSummaryKeywordsExtractionTool(GenericDSInterface *theParent);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	//virtual int SetSourceMedia(MomVop *media);
	//virtual int SetSourceMediaFrameNumber(unsigned long FrameNo);
	virtual int SetSourceMediaFilename(char *aFilename);
                           // for fileIO with preprossessed preliminary results
	virtual int SetDescriptorInterface(GenericDSInterfaceABC
					   *ParentDSInterface);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long NewSequence(void);

	virtual SeqSummaryKeywordsExtractionInterfaceABC *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~SeqSummaryKeywordsExtractionTool();
	void ms2time(int, char*);

	/* private functions*/

	/* interface of this*/
	SeqSummaryKeywordsExtractionInterface m_Interface;

	/* extraction parameters*/

	/* description*/
	GenericDSInterfaceABC *m_DescriptorInterface;
	GenericDS m_SequentialSummary;
	GenericDSInterfaceABC *m_SequentialSummaryInterface;

	/* input media*/
	char *m_MediaFilename;
	char *m_stopwordFilename;
	int m_printAllTimestamps;
	int m_numTopK;

};

};

#endif //__SUMMARIZATIONEXTRACTION_H__
