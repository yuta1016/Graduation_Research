///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Sanghoon Sull, Korea University, Seoul, Korea
// Keansub Lee, Korea University, Seoul, Korea
//
// in the course of development of the MPEG-7 Experimentation SpatialResolutionHint.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation SpatialResolutionHint tools as specified by the MPEG-7 Requirements.
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SpatialResolutionHintSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <fstream>

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"



//=============================================================================

using namespace XM;
using namespace std;


const UUID SpatialResolutionHintSearchInterface::myID = UUID("");
const char *SpatialResolutionHintSearchInterface::myName = "SpatialResolutionHint Transcoding Interface";

const UUID SpatialResolutionHintSearchTool::myID = UUID("");
const char *SpatialResolutionHintSearchTool::myName = "SpatialResolutionHint Transcoding Tool";

const UUID SpatialResolutionHintSearchInterfaceABC::myID = UUID();


//=============================================================================
SpatialResolutionHintSearchInterfaceABC::SpatialResolutionHintSearchInterfaceABC()
{
	SetInterfaceID(myID);
}


//=============================================================================
SpatialResolutionHintSearchInterface::SpatialResolutionHintSearchInterface(SpatialResolutionHintSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpatialResolutionHintSearchInterface::~SpatialResolutionHintSearchInterface()
{
}

//----------------------------------------------------------------------------
void SpatialResolutionHintSearchInterface::destroy()
{  delete m_SearchTool; }


//----------------------------------------------------------------------------
int SpatialResolutionHintSearchInterface::SetDescriptionInterface(GenericDSInterfaceABC
														   *aGenericDSInterface)
{
  return m_SearchTool->SetDescriptionInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
int SpatialResolutionHintSearchInterface::SetSpatialResolutionHintParms(const int m_w, const int m_h ) 
{
	return m_SearchTool->SetSpatialResolutionHintParms(m_w, m_h);
}

//----------------------------------------------------------------------------
double SpatialResolutionHintSearchInterface::GetDistance(void)
{
	return m_SearchTool->GetDistance();
}

//----------------------------------------------------------------------------
const UUID& SpatialResolutionHintSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpatialResolutionHintSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
SpatialResolutionHintSearchTool::SpatialResolutionHintSearchTool():
m_Interface(this),
m_DescriptionInterface(0),
m_width(0),
m_height(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpatialResolutionHintSearchTool::SpatialResolutionHintSearchTool(GenericDSInterfaceABC
												   *theParent):
m_Interface(this),
m_DescriptionInterface(0),
m_width(0),
m_height(0)
{
	SetDescriptionInterface(theParent);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpatialResolutionHintSearchTool::~SpatialResolutionHintSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& SpatialResolutionHintSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpatialResolutionHintSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool SpatialResolutionHintSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool SpatialResolutionHintSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int SpatialResolutionHintSearchTool::SetDescriptionInterface(GenericDSInterfaceABC
													  *aGenericDSInterface)
{
  m_DescriptionInterface = aGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
int SpatialResolutionHintSearchTool::SetSpatialResolutionHintParms(const int m_w, const int m_h ) 
{
	m_width = m_w;
	m_height = m_h;

return 0;

}

//----------------------------------------------------------------------------
double SpatialResolutionHintSearchTool::GetDistance(void)
{
	int x, y, w, h, cx, cy;
	double R;
	// Read SpatialResolution Hint Description
	GenericDS StillRegionDS = m_DescriptionInterface->GetFirstChild();
	GenericDSInterfaceABC *pStillRegionDSInt = StillRegionDS.GetInterface();

	if (pStillRegionDSInt->isNull()) cerr << "Error - NULL Child" << endl;

	GenericDS BoundingBoxDS = pStillRegionDSInt->GetFirstChild();
	GenericDSInterfaceABC *pBoundingBoxDSInt = BoundingBoxDS.GetInterface();

    pBoundingBoxDSInt->GetIntAttribute("BoxWidth", w);
	pBoundingBoxDSInt->GetIntAttribute("BoxHeight", h);

	GenericDS CompositionInfoDS = pBoundingBoxDSInt->GetFirstChild();
	GenericDSInterfaceABC *pCompositionInfoDSInt = CompositionInfoDS.GetInterface();

    pCompositionInfoDSInt->GetIntAttribute("BoxCenterH", cx);
	pCompositionInfoDSInt->GetIntAttribute("BoxCenterV", cy);
	x = cx - w/2;
	y = cy - h/2;

	GenericDS MediaInformationDS = pBoundingBoxDSInt->GetNextSibling();
	GenericDSInterfaceABC *pMediaInformationDSInt = MediaInformationDS.GetInterface();

	GenericDS MediaProfileDS = pMediaInformationDSInt->GetFirstChild();
	GenericDSInterfaceABC *pMediaProfileDSInt = MediaProfileDS.GetInterface();

	GenericDS MediaTranscodingHintDS = pMediaProfileDSInt->GetFirstChild();
	GenericDSInterfaceABC *pMediaTranscodingHintDSInt = MediaTranscodingHintDS.GetInterface();
	pMediaTranscodingHintDSInt->GetRealAttribute("spatialResolutionHint", R);
	StillRegionMatching(x, y, w, h, R);

	return 0;
}


//----------------------------------------------------------------------------
SpatialResolutionHintSearchInterfaceABC *SpatialResolutionHintSearchTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
void SpatialResolutionHintSearchTool::StillRegionMatching( int x, int y, int w, int h, double r)
{
	// Perform the image transcoding with the client display size(100 x 100)  
	int wd_t, ht_t, wd1, wd2, ht1, ht2, X1, Y1, X2, Y2, clientw=100, clienth=100, imag_width = m_width, Imag_height = m_height;
	X1 = x;
	Y1 = y;
	X2 = x + w;
	Y2 = y + h;
	wd_t = (int)((clientw/r) - w);
	ht_t = (int)((clienth/r) - h);
	if(wd_t>0)
	{
		wd1 = (int)(X1 * wd_t)/(imag_width - X2 + X1);
		wd2 = (int)((imag_width - X2)*wd_t)/(imag_width - X2 + X1);
		X1 = X1 - wd1;
		X2 = X2 + wd2;
	}
	if(ht_t>0)
	{
		ht1 = (int)(Y1 * ht_t)/(Imag_height - Y2 + Y1);
		ht2 = (int)((Imag_height - Y2)*ht_t)/(Imag_height - Y2 + Y1);
		Y1 = Y1 - ht1;
		Y2 = Y2 + ht2;
	}
	if( X1 <= 0 ) X1 = 0;
	if( X1 >= imag_width ) X1 = imag_width;
	if( Y1 <= 0 ) Y1 = 0;
	if( Y1 >= Imag_height  ) Y1 = Imag_height;
	if( X2 <= 0 ) X2 = 0;
	if( X2 >= imag_width  ) X2 = imag_width;
	if( Y2 <= 0 ) Y2 = 0;
	if( Y2 >= Imag_height  ) Y2 = Imag_height; 

	//sth printf to stdout violates primary output of the XM
	printf("\n\nOriginal Image Size = %d x %d (Pixels)  Scaling Factor = 1.0", imag_width, Imag_height);  	
	printf("\nTarget Display Size = 100 x 100 (Pixels)");  	
	printf("\n\nImage Transcoding Result : \nOutput Region = (%d, %d) ~ (%d, %d) (Pixels)  \nScaling Factor = %.2f\n\n\n", X1, Y1, X2, Y2, r);  	
}
