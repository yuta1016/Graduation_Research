///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Osamu Hori, Toshimitsu Kaneko, Koji Yamamoto
// R&D Center, TOSHIBA Corp.
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SequentialSummaryExtraction.h
//

#ifndef __SEQUENTIALSUMMARYEXTRACTION_H__
#define __SEQUENTIALSUMMARYEXTRACTION_H__

#include "BasicArchitecture/XMInterfaces.h"   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"      /* Naming Convention */

#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"

namespace XM
{

// Forward Declarations:
class SequentialSummaryExtractionTool;

//=============================================================================
// This is an abstract base class to provide an interface to the extraction
// utilities.
class SequentialSummaryExtractionInterfaceABC: public I_InterfaceABC
{
public:
  SequentialSummaryExtractionInterfaceABC();
  virtual ~SequentialSummaryExtractionInterfaceABC() {};

  virtual void destroy()=0;

  // This informs the extractor where the source data will come from
  virtual int SetSourceMedia(MultiMediaInterfaceABC *Media) = 0;

  // This is used to inform the top level module where the results
  // are written to
  virtual GenericDSInterfaceABC*
    GetDescriptionInterface(void) = 0;
  // This informs the extraction were to write the results
  virtual int SetDescriptionInterface(GenericDSInterfaceABC
				     *aSequentialSummaryDescriptionInterface) = 0;

  // This is executed before frame based /time point) extraction, e.g.,
  // reset description or read extraction parameters
  virtual unsigned long InitExtracting(void) = 0;
  // This is the per frame (time point/ period) extraction
  virtual unsigned long StartExtracting(void) = 0;
  // This is executed after the frame based (time point) extraction
  virtual unsigned long PostExtracting(void) = 0;

  static const UUID myID;
};

//=============================================================================
class SequentialSummaryExtractionInterface: 
public SequentialSummaryExtractionInterfaceABC
{
public:
  // used when DummyExtraction tool is created
  SequentialSummaryExtractionInterface(SequentialSummaryExtractionTool* aTool);
  // desctructor
  virtual ~SequentialSummaryExtractionInterface();

  // UUID for this extraction class
  virtual const UUID& GetInterfaceID(void);
  // for checking for right element type
  virtual const char *GetName(void);

  // destructs SequentialSummaryExtractionTool
  virtual void destroy();

  virtual int SetSourceMedia(MultiMediaInterfaceABC *Media);

  virtual GenericDSInterfaceABC*
    GetDescriptionInterface(void);
  virtual int SetDescriptionInterface(GenericDSInterfaceABC
				     *aSequentialSummaryDescriptionInterface);

  virtual unsigned long InitExtracting(void);
  virtual unsigned long StartExtracting(void);
  virtual unsigned long PostExtracting(void);

  static const UUID myID;
  static const char *myName;

private:
  SequentialSummaryExtractionTool *m_ExtractionTool;
};

//=============================================================================
class SequentialSummaryExtractionTool: public DescriptorExtractor
{
  friend class SequentialSummaryExtractionInterface;
public:
  // Null constructor
  SequentialSummaryExtractionTool();

  // Also connects the Description (result memnory) to the extraction
  // If set to "0" it automatically creates the descriptor
  SequentialSummaryExtractionTool(GenericDSInterfaceABC
			  *SequentialSummary);

  // ID of object type
  virtual const UUID& GetObjectID(void);
  // Object type name
  virtual const char *GetName(void);

  // Don't know what this does
  virtual bool SupportsPush(void);
  // Don't know what this does
  virtual bool SupportsPull(void);

  // This informs the extractor where the source data comes from
  virtual int SetSourceMedia(MultiMediaInterfaceABC *Media);

  virtual GenericDSInterfaceABC*
    GetDescriptionInterface(void);
  virtual int SetDescriptionInterface(GenericDSInterfaceABC
				     *aSequentialSummaryDescriptionInterface);
	
  virtual unsigned long InitExtracting(void);
  virtual unsigned long StartExtracting(void);
  virtual unsigned long PostExtracting(void);

  // Extraction object must no be used, only its interface is allowd to
  // to be used. This function is to get the interface
  virtual SequentialSummaryExtractionInterfaceABC *GetInterface(void);

  // access is allowed only by class factories for this
  // object.  This avoids having to duplicate the
  // ID definition in multiple locations.  In the future, we may
  // have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
  // IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
  static const UUID myID;
  static const char * myName;
private:

  // Destructor is private to allow creation of
  // object only by using "new"
  virtual ~SequentialSummaryExtractionTool();

  SequentialSummaryExtractionInterface m_Interface;
  GenericDSInterfaceABC *m_DescriptionInterface;
  GenericDS m_SequentialSummary;

  long m_srcVideoFrame;
  long m_videoSummaryFrame;
  MultiMediaInterfaceABC	*m_Media;
  MomVop* m_pOldImage;
  MomVop* m_pImage;
  MediaDB m_SummaryDB;
  char m_szSourceVideoName[MAXMEDIANAMELEN];
  char m_szVideoSummaryName[MAXMEDIANAMELEN];
  int m_thumbWidth;
  int m_thumbHeight;

  // Creates a SourceLocator & VideoSummaryLocator node to describe their location
  void CreateVideoLocatorNode(GenericDSInterfaceABC *parentInterface,
						char* srcVideoName, char* videoSummaryName);
  // Creates a VisualSummaryComponent node to describe the correspondence between
  // the original video and video skim
  void CreateImageLocatorVSCNode(GenericDSInterfaceABC *parentInterface,
						int ipicFrame, int skimFrame);
  // Creates a VisualSummaryComponent node to describe the FrameActivity
  void CreateFrameActivityVSCNode(GenericDSInterfaceABC *parentInterface,
						int ppicFrame, double frameActivity);
  // Shrinks the image
  void ShrinkImage(MomVop* inImage, MomVop* outImage);
  // Calculates the frame activity from the color histogram
  double CalcFrameActivity(MomVop* oldImage, MomVop* newImage);
}; // End class

}; // End namespace


#endif //__SEQUENTIALSUMMARYEXTRACTION_H__
