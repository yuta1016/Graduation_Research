///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Gang Wei and Nevenka	Dimitrova	- Philips Research.
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
// Applicable File Name:  VideoTextExtraction.h
//

#ifndef __VIDEOTEXTEXTRACTION_H__
#define __VIDEOTEXTEXTRACTION_H__

#include "BasicArchitecture/XMInterfaces.h"                  /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                     /* Naming Convention */
#include "Descriptors/Descriptors.h"                   /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"            /* Naming Convention */
#include "momusys.h"
#include "coortrans.h"
#include "VideoText_Detect.h"
namespace XM
{


// Forward Declarations:
class VideoTextExtractionTool;

//=============================================================================
class VideoTextExtractionInterfaceABC: public I_InterfaceABC
{
public:
	VideoTextExtractionInterfaceABC();
	virtual ~VideoTextExtractionInterfaceABC() {};

	virtual void destroy() = 0;

	virtual int SetSourceMedia(MomVop *VOP) = 0;
	virtual int SetSourceMedia(char * sFileName) = 0;
	virtual int SetSourceMediaFrameNumber(unsigned long FrameNo) = 0;
	virtual int SetDescriptorInterface(GenericDSInterfaceABC
					   *ParentDSInterface) = 0;

	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(void) = 0;
	virtual unsigned long StartExtracting(char * sXMLFileName) = 0;
	                      //sth what is this for? please remove this
	virtual unsigned long PostExtracting(void) = 0;
	virtual MomVop* GetVideoText(void) = 0;

	static const UUID myID;
};

//=============================================================================
class VideoTextExtractionInterface:	public VideoTextExtractionInterfaceABC
{
public:
	VideoTextExtractionInterface(
                        VideoTextExtractionTool* aTool);
	virtual ~VideoTextExtractionInterface();
	
	virtual void destroy();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetSourceMedia(MomVop *VOP);
	virtual int SetSourceMedia(char * sVideoName); //sth not needed when you use MultiMedia
	virtual int SetSourceMediaFrameNumber(unsigned long FrameNo);
	virtual int SetDescriptorInterface(
			 GenericDSInterfaceABC
			 *ParentDSInterface);

	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long StartExtracting(char * sXMLfileName);//sth
	virtual unsigned long PostExtracting(void);
	virtual MomVop* GetVideoText(void);

	static const UUID myID;
	static const char *myName;

private:
	VideoTextExtractionTool *m_ExtractionTool;
};

//=============================================================================
class VideoTextExtractionTool: public DescriptorExtractor
{
friend class VideoTextExtractionInterface;
public:
	VideoTextExtractionTool();
	VideoTextExtractionTool(GenericDSInterfaceABC *theParent);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(char * sVideoFileName);
	virtual int SetSourceMedia(MomVop *VOP);
	virtual int SetSourceMediaFrameNumber(unsigned long FrameNo);
	     /* frame number must be set AFTER Media was set!!!*/
 
	virtual int SetDescriptorInterface(
                         GenericDSInterfaceABC
			 *ParentDSInterface);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long StartExtracting(char * sXMLFileName);
	virtual unsigned long PostExtracting(void);
	virtual MomVop* GetVideoText(void); //sth not std interface

	virtual VideoTextExtractionInterfaceABC
	  *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

private:
	virtual ~VideoTextExtractionTool();
	void track_text_to_get_XML(char * sXMLFileName);
	void write_XML_header(FILE* fpXML, char* sTitle, int frmHeight, int frmWidth); //sth use GenericDSCS to write XML
	void write_XML_end(FILE* fpXML);
	void output_to_XML(FILE * fpXML, CTextFrame & textFrame, int frmInd, int larger);

	VideoTextExtractionInterface m_Interface;
	GenericDSInterfaceABC *m_DescriptorInterface;

	/* internal variables*/
	GenericDS m_videoSegmentDS;
	GenericDSInterfaceABC *m_videoSegmentDSInterface;
	char * sInVideoFileName;
	int m_frameNumber;
	int m_prevFrameNumber;
	MomVop* m_VideoTextMedia;
	CTextFrame m_curTextFrame;
	CTextFrame m_prevTextFrame;
};
};

#endif //__VIDEOTEXTEXTRACTION_H__
