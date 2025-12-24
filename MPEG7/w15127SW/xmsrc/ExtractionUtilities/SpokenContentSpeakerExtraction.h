///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jason Charlesworth
// Canon Research Centre (Europe) Ltd.
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
// Applicable File Name:  SpokenContentSpeakerExtraction.h
//
#ifndef __SPOKENCONTENTSPEAKERTYPEEXTRACTION_H__
#define __SPOKENCONTENTSPEAKERTYPEEXTRACTION_H__

// Only works if XML is defined
//#ifdef USEXML

#include "BasicArchitecture/XMInterfaces.h"   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"      /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */

namespace XM
{

// Forward Declarations:
class SpokenContentSpeakerExtractionTool;

//=============================================================================
// This is an abstract base class to provide an interface to the extraction
// utilities.
class SpokenContentSpeakerExtractionInterfaceABC: public I_InterfaceABC
{
public:
        // Null constructor
	SpokenContentSpeakerExtractionInterfaceABC();

        // Null destructor
	virtual ~SpokenContentSpeakerExtractionInterfaceABC() {};

        // Don't know what this does
	virtual void destroy()=0;

        // This informs the extractor where the source data will come from
	virtual int SetSourceMedia(GenericDS* media) = 0;

        // Don't know what this does
	virtual SpokenContentSpeakerDescriptorInterfaceABC*
	  GetDescriptorInterface(void) = 0;

        // Don't know what this does
	virtual int SetDescriptorInterface(SpokenContentSpeakerDescriptorInterfaceABC*
                  aSpokenContentSpeakerDescriptorInterface) = 0;

        // Don't know what this does
	virtual unsigned long InitExtracting(void) = 0;

        // Don't know what this does
	virtual unsigned long StartExtracting(void) = 0;

	static const UUID myID;
};

//=============================================================================
class SpokenContentSpeakerExtractionInterface: 
  public SpokenContentSpeakerExtractionInterfaceABC
{
public:
        // Don't know what this does
	SpokenContentSpeakerExtractionInterface(SpokenContentSpeakerExtractionTool* aTool);

        // Don't know what this does
	virtual ~SpokenContentSpeakerExtractionInterface();
	
        // Don't know what this does
	virtual const UUID& GetInterfaceID(void);

        // Don't know what this does
	virtual const char *GetName(void);

        // Don't know what this does
	virtual void destroy();

        // This informs the extractor where the source data comes frome
	virtual int SetSourceMedia(GenericDS* media);

        // Don't know what this does
	virtual SpokenContentSpeakerDescriptorInterfaceABC*
	  GetDescriptorInterface(void);

        // Don't know what this does
	virtual int SetDescriptorInterface(SpokenContentSpeakerDescriptorInterfaceABC
		       *aSpokenContentSpeakerDescriptorInterface);

        // Don't know what this does
	virtual unsigned long InitExtracting(void);

        // Don't know what this does
	virtual unsigned long StartExtracting(void);

	static const UUID myID;
	static const char *myName;

private:
	SpokenContentSpeakerExtractionTool *m_ExtractionTool;
};

//=============================================================================
class SpokenContentSpeakerExtractionTool: public DescriptorExtractor
{
        friend class SpokenContentSpeakerExtractionInterface;
public:

        // Null constructor
	SpokenContentSpeakerExtractionTool();

        // Don't know what this does
        SpokenContentSpeakerExtractionTool(SpokenContentSpeakerDescriptorInterfaceABC
  				     *SpokenContentSpeaker);

        // Don't know what this does
	virtual const UUID& GetObjectID(void);

        // Don't know what this does
	virtual const char *GetName(void);

        // Don't know what this does
	virtual bool SupportsPush(void);

        // Don't know what this does
	virtual bool SupportsPull(void);

        // This informs the extractor where the source data comes from
	virtual int SetSourceMedia(GenericDS* media);

        // Don't know what this does
        virtual SpokenContentSpeakerDescriptorInterfaceABC*
	  GetDescriptorInterface(void);

        // Don't know what this does
	virtual int SetDescriptorInterface(SpokenContentSpeakerDescriptorInterfaceABC
		       *aSpokenContentSpeakerDescriptorInterface);
	
        // Don't know what this does
	virtual unsigned long InitExtracting(void);

        // Don't know what this does
	virtual unsigned long StartExtracting(void);

        // Don't know what this does
	virtual SpokenContentSpeakerExtractionInterfaceABC *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
        // Don't know what this does
	virtual ~SpokenContentSpeakerExtractionTool();

	SpokenContentSpeakerExtractionInterface  m_Interface;
	SpokenContentSpeakerDescriptorInterfaceABC *m_DescriptorInterface;

  //	MomVop *m_Media;
  //	aud_Media* m_Media;
        GenericDS* m_Media;
	
}; // End class

}; // End namespace


//#endif // USEXML
#endif //__SPOKENCONTENTSPEAKERTYPEEXTRACTION_H__
