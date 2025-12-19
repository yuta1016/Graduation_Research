///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// NEC Corp., Akio Yamada, Eiji Kasutani
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
// Applicable File Name:  ColorHistExtraction.h
//
#ifndef __COLOR_LAYOUT_EXTRACTION_H__
#define __COLOR_LAYOUT_EXTRACTION_H__
#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                                      /* Naming Convention */

#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"

namespace XM
{


// Forward Declarations:
class ColorLayoutExtractionTool;

//=============================================================================
class ColorLayoutExtractionInterfaceABC: public I_InterfaceABC
{
public:
	ColorLayoutExtractionInterfaceABC();
	virtual ~ColorLayoutExtractionInterfaceABC() {};

	// start modification for version 3.0
	virtual void destroy()=0;
	// end modification for version 3.0

	virtual int SetSourceMedia(MultiMediaInterfaceABC* media) = 0;

	// start modification for version 3.0
	virtual ColorLayoutDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	// end modification for version 3.0
	virtual int SetDescriptorInterface(ColorLayoutDescriptorInterfaceABC
				    *aColorLayoutDescriptorInterface) = 0;
        // start modification for version 3.0
	virtual unsigned long InitExtracting(void) = 0;
	// end modification for version 3.0
	virtual unsigned long StartExtracting(void) = 0;
	// This is executed after the frame based (time point) extraction
	virtual unsigned long PostExtracting(void) = 0;

	static const UUID myID;
};

//=============================================================================
class ColorLayoutExtractionInterface: 
  public ColorLayoutExtractionInterfaceABC
{
public:
	ColorLayoutExtractionInterface(ColorLayoutExtractionTool* aTool);
	virtual ~ColorLayoutExtractionInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	// start modification for version 3.0
	virtual void destroy();
	// end modification for version 3.0

	virtual int SetSourceMedia(MultiMediaInterfaceABC* media);
	// start modification for version 3.0
	virtual ColorLayoutDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	// end modification for version 3.0
	virtual int SetDescriptorInterface(ColorLayoutDescriptorInterfaceABC
					  *aColorLayoutDescriptorInterface);
	// start modification for version 3.0
	virtual unsigned long InitExtracting(void);
	// end modification for version 3.0
	virtual unsigned long StartExtracting(void);
	virtual unsigned long PostExtracting(void);

	static const UUID myID;
	static const char *myName;

private:
	ColorLayoutExtractionTool *m_ExtractionTool;
};

//=============================================================================
class ColorLayoutExtractionTool: public DescriptorExtractor
{
friend class ColorLayoutExtractionInterface;
public:
	// Null constructor
	ColorLayoutExtractionTool();
        // start modification for version 3.0
        ColorLayoutExtractionTool(ColorLayoutDescriptorInterfaceABC *aDescriptorInterface);
        // end modfication for version 3.0
//	virtual ~ColorLayoutExtractionTool();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(MultiMediaInterfaceABC *media);

/*	virtual unsigned long ExtractFeature(ColorLayoutDescriptor* aDesc,
	             I_ExtractionInterfaceABC::ExtractionResult& aResult);*/
        // start modification for version 3.0
	virtual ColorLayoutDescriptorInterfaceABC *GetDescriptorInterface(void);
        // end modification for version 3.0
	virtual int SetDescriptorInterface(ColorLayoutDescriptorInterfaceABC
				   * aColorLayoutDescriptorInterface);

        // start modification for version 3.0
	virtual unsigned long InitExtracting(void);
        // end modification for version 3.0
	virtual unsigned long StartExtracting(int NumberOfYCoeff, int NumberOfCCoeff);
	virtual unsigned long StartExtracting(){ return StartExtracting(6, 3); };
	virtual unsigned long PostExtracting(void);

	virtual ColorLayoutExtractionInterfaceABC *GetInterface(void);

/*	"C" void pixeltocolorhist(TPixel *res, TPixel *in,TCoor x, TCoor y);*/

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
        // start modification for version 3.0
	virtual ~ColorLayoutExtractionTool();
        // end modification for version 3.0

	ColorLayoutExtractionInterface m_Interface;
	ColorLayoutDescriptorInterfaceABC *m_DescriptorInterface;
	MultiMediaInterfaceABC* m_Media;
	unsigned long m_FrameCnt;

	// helper functions and variables
	double c[8][8]; /* transform coefficients */
	unsigned char zigzag_scan[64]; /* Zig-Zag scan pattern  */
	void CreateSmallImage(MomVop *src, short small_img[3][64]);
	void init_fdct();
	inline void fdct(short *block);
 	inline int quant_ydc(int i);
	inline int quant_cdc(int i);
	inline int quant_ac(int i);
};

};

//extern "C" void pixeltocolorhist(TPixel *res, TPixel *in,TCoor x, TCoor y);


#endif //__COLOR_LAYOUT_EXTRACTION_H__
