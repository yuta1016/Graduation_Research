///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jiuhuai Lu
// Tektronix, Inc.
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
// Applicable File Name:  MediaQualityExtraction.h
//

#ifndef __MEDIAQUALITYEXTRACTION_H__
#define __MEDIAQUALITYEXTRACTION_H__

#include "BasicArchitecture/XMInterfaces.h"   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"      /* Naming Convention */

#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "momusys.h"

#include <stdio.h>
#include <math.h>
//ifdef XYUVDISP
//#include <xyuv.h>
//#endif



namespace XM
{

// Forward Declarations:
class MediaQualityExtractionTool;

//=============================================================================
// This is an abstract base class to provide an interface to the extraction
// utilities.
class MediaQualityExtractionInterfaceABC: public I_InterfaceABC
{
public:
  MediaQualityExtractionInterfaceABC();
  virtual ~MediaQualityExtractionInterfaceABC() {};

  virtual void destroy()=0;

  //sth we have to find a solution for this
  virtual int SetSourceMedia(MomVop* media_comp, MomVop* media_ref) = 0;

  // This is used to inform the top level module where the results
  // are written to
  virtual GenericDSInterfaceABC* GetDescriptionInterface(void) = 0;

  // This informs the extraction were to write the results
  virtual int SetDescriptionInterface(GenericDSInterfaceABC
				     *aMediaQualityDescriptionInterface) = 0;

  // This is executed before frame based /time point) extraction, e.g.,
  // reset description or read extraction parameters
  virtual unsigned long InitExtracting
  	(MediaDB *RamDB_comp, MediaDB *RamDB_ref) = 0;
  //sth what is this for?

  // This is the per frame (time point/ period) extraction
  virtual unsigned long StartExtracting(void) = 0;

  // This is executed after the frame based (time point) extraction
  virtual unsigned long PostExtracting(void) = 0;

  static const UUID myID;
};

//=============================================================================
class MediaQualityExtractionInterface: 
public MediaQualityExtractionInterfaceABC
{
public:

  MediaQualityExtractionInterface(MediaQualityExtractionTool* aTool);
  // desctructor
  virtual ~MediaQualityExtractionInterface();

  // UUID for this extraction class
  virtual const UUID& GetInterfaceID(void);
  // for checking for right element type
  virtual const char *GetName(void);

  // destructs MediaQualityExtractionTool
  virtual void destroy();

  virtual int SetSourceMedia(MomVop* media_comp, MomVop* media_ref);

  virtual GenericDSInterfaceABC* GetDescriptionInterface(void);

  virtual int SetDescriptionInterface(GenericDSInterfaceABC
				     *aMediaQualityDescriptionInterface);

  virtual unsigned long InitExtracting
  	(MediaDB *RamDB_comp, MediaDB *RamDB_ref);

  virtual unsigned long StartExtracting(void);
  virtual unsigned long PostExtracting(void);

  static const UUID myID;
  static const char *myName;

private:
  MediaQualityExtractionTool *m_ExtractionTool;
};

#define MAXSTRINGLEN 100
#define MAXNUMRATINGS 100
#define MAXPERCEPTIBLEDEFECTS	20
#define MAXNUMOFFRAMES	1800

//=============================================================================
class MediaQualityExtractionTool: public DescriptorExtractor
{
  friend class MediaQualityExtractionInterface;
public:
  // Null constructor
  MediaQualityExtractionTool();

  // Also connects the Description (result memnory) to the extraction
  // If set to "0" it automatically creates the descriptor
  MediaQualityExtractionTool(GenericDSInterfaceABC
			  *MediaQuality);

  // ID of object type
  virtual const UUID& GetObjectID(void);

  // Object type name
  virtual const char *GetName(void);

  // Don't know what this does
  virtual bool SupportsPush(void);
  // Don't know what this does
  virtual bool SupportsPull(void);

  // This informs the extractor where the source data comes from
  virtual int SetSourceMedia(MomVop* media_comp, MomVop* media_ref);

  virtual GenericDSInterfaceABC* GetDescriptionInterface(void);

  virtual int SetDescriptionInterface(GenericDSInterfaceABC
				     *aMediaQualityDescriptionInterface);
	
  virtual unsigned long InitExtracting(MediaDB *RamDB_comp, MediaDB *RamDB_ref);
  virtual unsigned long StartExtracting(void);
  virtual unsigned long PostExtracting(void);

  // Extraction object must no be used, only its interface is allowd to
  // to be used. This function is to get the interface
  virtual MediaQualityExtractionInterfaceABC *GetInterface(void);

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
  virtual ~MediaQualityExtractionTool();

  MediaQualityExtractionInterface  m_Interface;
  GenericDSInterfaceABC           *m_DescriptionInterface;
  GenericDS                        m_MediaQuality;
  MomVop                          *m_Media_ref;
  MomVop                          *m_Media_comp;

  // List of parameters expected from parameter file 
  char                            m_RatingSource[MAXSTRINGLEN];
  char                            m_RatingInfoLocator[MAXSTRINGLEN];
  char m_PerceptibleDefectsVisualL  [MAXPERCEPTIBLEDEFECTS][MAXSTRINGLEN];
  // char m_PerceptibleDefectsVisualCSN[MAXPERCEPTIBLEDEFECTS][MAXSTRINGLEN];
  // char m_PerceptibleDefectsVisualCSL[MAXPERCEPTIBLEDEFECTS][MAXSTRINGLEN];
  char m_PerceptibleDefectsVisualCST[MAXPERCEPTIBLEDEFECTS][MAXSTRINGLEN];
  int  m_NoOfVisDefects;
  char m_PerceptibleDefectsAudioL   [MAXPERCEPTIBLEDEFECTS][MAXSTRINGLEN];
  // char m_PerceptibleDefectsAudioCSN [MAXPERCEPTIBLEDEFECTS][MAXSTRINGLEN];
  // char m_PerceptibleDefectsAudioCSL [MAXPERCEPTIBLEDEFECTS][MAXSTRINGLEN];
  char m_PerceptibleDefectsAudioCST [MAXPERCEPTIBLEDEFECTS][MAXSTRINGLEN];
  int  m_NoOfAudDefects;
  int                              m_NumAudDefects;
  int                              m_NumVidDefects;
  char                             m_isRatingMaxDefined;
  char                             m_isRatingMinDefined;
  int                              m_RatingMax; 
  int                              m_RatingMin; 
  char                             m_RatingStyle  [MAXSTRINGLEN];
  char                             m_Ratingtype   [MAXSTRINGLEN];
  float                            m_RatingValueY; 
  float                            m_RatingValueU;
  float                            m_RatingValueV;
  char                             m_RatingType [MAXSTRINGLEN];
  char                             m_RatingSchemeL   [3][MAXSTRINGLEN];
  //char                             m_RatingSchemeCSN [MAXSTRINGLEN];
  //char                             m_RatingSchemeCSL [MAXSTRINGLEN];
  char                             m_RatingSchemeCST [3][MAXSTRINGLEN];
 
  float                            *m_RatingPerframe;
  int                              m_NoOfSchemes;  
  unsigned int                    m_FrameCnt;
  static unsigned int              m_noframes;

  float psnr_per_frame_Y[MAXNUMOFFRAMES];
  float psnr_per_frame_U[MAXNUMOFFRAMES];
  float psnr_per_frame_V[MAXNUMOFFRAMES];

  void Psnr_ColorImage_YUV
  (MomVop* imgdata, MomVop* refdata, float& psnry, float& psnru, float& psnrv);

  float  psnr_image (UChar *i1, UChar *i2, const int size );

  int  ReadParameterfile ();
  int	VideoFileConversion ();


}; // End class

}; // End namespace


#endif //__MEDIAQUALITYEXTRACTION_H__
