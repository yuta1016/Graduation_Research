///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann
// Munich University of technology
// based on the code from
// A. Mufit Ferman - University of Rochester
// Vincent Hsu, June 11, 2001
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
// Copyright (c) 1998-2002.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  GoFGoPColorExtraction.h
//
//
#ifndef __GOFGOPCOLOREXTRACTION_H__
#define __GOFGOPCOLOREXTRACTION_H__

#include "BasicArchitecture/XMInterfaces.h"   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"      /* Naming Convention */

#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"

struct gofhistlist_struct{
  unsigned long *histogram;
  struct gofhistlist_struct *next;
};

typedef struct gofhistlist_struct TGOFHISTLIST;

namespace XM
{

// Forward Declarations:
class GoFGoPColorExtractionTool;
class ScalableColorExtractionInterfaceABC;

//=============================================================================
// This is an abstract base class to provide an interface to the extraction
// utilities.
class GoFGoPColorExtractionInterfaceABC: public I_InterfaceABC
{
public:
  GoFGoPColorExtractionInterfaceABC();
  virtual ~GoFGoPColorExtractionInterfaceABC() {};

  virtual void destroy()=0;

  // This informs the extractor where the source data will come from
  virtual int SetSourceMedia(MultiMediaInterfaceABC* media) = 0;

  // This is used to inform the top level module where the results
  // are written to
  virtual GoFGoPColorDescriptorInterfaceABC*
    GetDescriptorInterface(void) = 0;
  // This informs the extraction were to write the results
  virtual int SetDescriptorInterface(GoFGoPColorDescriptorInterfaceABC
				     *aGoFGoPColorDescriptorInterface) = 0;

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
class GoFGoPColorExtractionInterface: 
public GoFGoPColorExtractionInterfaceABC
{
public:
  // used when DummyExtraction tool is created
  GoFGoPColorExtractionInterface(GoFGoPColorExtractionTool* aTool);
  // desctructor
  virtual ~GoFGoPColorExtractionInterface();

  // UUID for this extraction class
  virtual const UUID& GetInterfaceID(void);
  // for checking for right element type
  virtual const char *GetName(void);

  // destructs GoFGoPColorExtractionTool
  virtual void destroy();

  virtual int SetSourceMedia(MultiMediaInterfaceABC* media);

  virtual GoFGoPColorDescriptorInterfaceABC*
    GetDescriptorInterface(void);
  virtual int SetDescriptorInterface(GoFGoPColorDescriptorInterfaceABC
				     *aGoFGoPColorDescriptorInterface);

  virtual unsigned long InitExtracting(void);
  virtual unsigned long StartExtracting(void);
  virtual unsigned long PostExtracting(void);

  static const UUID myID;
  static const char *myName;

private:
  GoFGoPColorExtractionTool *m_ExtractionTool;
};
//=============================================================================
class GoFGoPColorExtractionTool: public DescriptorExtractor
{
  friend class GoFGoPColorExtractionInterface;
public:
  // Null constructor
  GoFGoPColorExtractionTool();

  // Also connects the Descriptor (result memnory) to the extraction
  // If set to "0" it automatically creates the descriptor
  GoFGoPColorExtractionTool(GoFGoPColorDescriptorInterfaceABC
			  *GoFGoPColor);

  // ID of object type
  virtual const UUID& GetObjectID(void);
  // Object type name
  virtual const char *GetName(void);

  // Don't know what this does
  virtual bool SupportsPush(void);
  // Don't know what this does
  virtual bool SupportsPull(void);

  // This informs the extractor where the source data comes from
  virtual int SetSourceMedia(MultiMediaInterfaceABC* media);

  virtual GoFGoPColorDescriptorInterfaceABC*
    GetDescriptorInterface(void);
  virtual int SetDescriptorInterface(GoFGoPColorDescriptorInterfaceABC
				     *aGoFGoPColorDescriptorInterface);
	
  virtual unsigned long InitExtracting(void);
  virtual unsigned long StartExtracting(void);
  virtual unsigned long PostExtracting(void);

  // Extraction object must no be used, only its interface is allowd to
  // to be used. This function is to get the interface
  virtual GoFGoPColorExtractionInterfaceABC *GetInterface(void);

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
  virtual ~GoFGoPColorExtractionTool();

  GoFGoPColorExtractionInterface m_Interface;
  GoFGoPColorDescriptorInterfaceABC *m_DescriptorInterface;
  MultiMediaInterfaceABC* m_Media;
  int m_Aggregation;
  int m_NoOfHistograms;
  TGOFHISTLIST *m_HistogramList;

  ScalableColorExtractionInterfaceABC *m_ScalableColorExtraction;

}; // End class

}; // End namespace


#endif //__GOFGOPCOLOREXTRACTION_H__
