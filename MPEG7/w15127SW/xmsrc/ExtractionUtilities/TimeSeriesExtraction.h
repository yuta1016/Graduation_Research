///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Takehiro FUJITA
// Hitachi, Ltd.
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  TimeSeriesExtraction.h
//

#ifndef __TIMESERIESEXTRACTION_H__
#define __TIMESERIESEXTRACTION_H__

#include "BasicArchitecture/XMInterfaces.h"   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"      /* Naming Convention */

#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"

namespace XM
{

// Forward Declarations:
class TimeSeriesExtractionTool;

//=============================================================================
// This is an abstract base class to provide an interface to the extraction
// utilities.
class TimeSeriesExtractionInterfaceABC: public I_InterfaceABC
{
public:
  TimeSeriesExtractionInterfaceABC();
  virtual ~TimeSeriesExtractionInterfaceABC() {};

  virtual void destroy()=0;

  // This informs the extractor where the source data will come from
  virtual int SetSourceMedia(MultiMediaInterfaceABC* media) = 0;

  // This is used to inform the top level module where the results
  // are written to
  virtual TimeSeriesDescriptorInterfaceABC*
    GetDescriptorInterface(void) = 0;
  // This informs the extraction were to write the results
  virtual int SetDescriptorInterface(TimeSeriesDescriptorInterfaceABC
				     *aTimeSeriesDescriptorInterface) = 0;

  // This is executed before frame based /time point) extraction, e.g.,
  // reset description or read extraction parameters
  virtual unsigned long InitExtracting(void) = 0;
  // This is the per frame (time point/ period) extraction
//  virtual unsigned long StartExtracting(void) = 0;
  virtual unsigned long StartExtracting(MediaDB *ramDB) = 0;
  // This is executed after the frame based (time point) extraction
  virtual unsigned long PostExtracting(void) = 0;

  static const UUID myID;
};

//=============================================================================
class TimeSeriesExtractionInterface: 
public TimeSeriesExtractionInterfaceABC
{
public:
  // used when DummyExtraction tool is created
  TimeSeriesExtractionInterface(TimeSeriesExtractionTool* aTool);
  // desctructor
  virtual ~TimeSeriesExtractionInterface();

  // UUID for this extraction class
  virtual const UUID& GetInterfaceID(void);
  // for checking for right element type
  virtual const char *GetName(void);

  // destructs TimeSeriesExtractionTool
  virtual void destroy();

  virtual int SetSourceMedia(MultiMediaInterfaceABC* media);

  virtual TimeSeriesDescriptorInterfaceABC*
    GetDescriptorInterface(void);
  virtual int SetDescriptorInterface(TimeSeriesDescriptorInterfaceABC
				     *aTimeSeriesDescriptorInterface);

  virtual unsigned long InitExtracting(void);
//  virtual unsigned long StartExtracting(void);
  virtual unsigned long StartExtracting(MediaDB *ramDB);
  virtual unsigned long PostExtracting(void);

  static const UUID myID;
  static const char *myName;

private:
  TimeSeriesExtractionTool *m_ExtractionTool;
};
//=============================================================================
class TimeSeriesExtractionTool: public DescriptorExtractor
{
  friend class TimeSeriesExtractionInterface;
public:
  // Null constructor
  TimeSeriesExtractionTool();

  // Also connects the Descriptor (result memnory) to the extraction
  // If set to "0" it automatically creates the descriptor
  TimeSeriesExtractionTool(TimeSeriesDescriptorInterfaceABC
			  *TimeSeries);

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

  virtual TimeSeriesDescriptorInterfaceABC*
    GetDescriptorInterface(void);
  virtual int SetDescriptorInterface(TimeSeriesDescriptorInterfaceABC
				     *aTimeSeriesDescriptorInterface);
	
  virtual unsigned long InitExtracting(void);
//  virtual unsigned long StartExtracting(void);
  virtual unsigned long StartExtracting(MediaDB *ramDB);
  virtual unsigned long PostExtracting(void);

  // Extraction object must no be used, only its interface is allowd to
  // to be used. This function is to get the interface
  virtual TimeSeriesExtractionInterfaceABC *GetInterface(void);

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
  virtual ~TimeSeriesExtractionTool();

  virtual unsigned long StartColorLayoutExtracting(MediaDB *ramDB);
  virtual void CopyColorLayoutDescriptor(ColorLayoutDescriptor *theD, ColorLayoutDescriptorInterfaceABC *theDInterface);
  virtual void CopyColorLayoutDescriptor(ColorLayoutDescriptor *dst, ColorLayoutDescriptor *src);

  virtual unsigned long StartIrregularSeriesColorLayoutExtracting(void);
  virtual float CompareDescriptors(ColorLayoutDescriptor *d1, ColorLayoutDescriptor *d2);
  TimeSeriesExtractionInterface m_Interface;
  TimeSeriesDescriptorInterfaceABC *m_DescriptorInterface;
  MultiMediaInterfaceABC* m_Media;
  unsigned long m_FrameCnt;

  int m_DummyExtractionParameter;
  
//--------- contained descriptors --------
	/* VisualDescriptors */
	ColorLayoutExtractionInterface	*m_ColorLayoutExtractionInterface;
	// other descriptors will be added in the future.

}; // End class

}; // End namespace


#endif //__TIMESERIESEXTRACTION_H__
