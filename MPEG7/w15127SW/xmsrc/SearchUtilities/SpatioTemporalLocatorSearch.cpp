//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// K.Masukura (R&D Center Toshiba, Japan)
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SpatioTemporalLocatorSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <cstring>

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"


//=============================================================================

#define MAXNVertices 30
struct CPolygon {
public:
	int N_Frame;					// Max Frame
	int VNumberOfRefernceRegions;			// NumberOfRefernceRegions of Vertex
	int Frame;					// Refernce No.
	double X[MAXNVertices+1];		// List of X coordinate
	double Y[MAXNVertices+1];		// List of Y coordinate
};

using namespace XM;


const UUID SpatioTemporalLocatorSearchInterface::myID = UUID("");
const char *SpatioTemporalLocatorSearchInterface::myName = "SpatioTemporalLocatorMatchingInterface";

const UUID SpatioTemporalLocatorSearchTool::myID = UUID("");
const char *SpatioTemporalLocatorSearchTool::myName = "SpatioTemporalLocatorMatchingTool";

const UUID SpatioTemporalLocatorSearchInterfaceABC::myID = UUID();

//=============================================================================
SpatioTemporalLocatorSearchInterfaceABC::SpatioTemporalLocatorSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpatioTemporalLocatorSearchInterface::SpatioTemporalLocatorSearchInterface(SpatioTemporalLocatorSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorSearchInterface::~SpatioTemporalLocatorSearchInterface()
{
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorSearchInterface::
SetRefDescriptorInterface(SpatioTemporalLocatorDescriptorInterfaceABC
		       *RefSpatioTemporalLocatorDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefSpatioTemporalLocatorDescriptorInterface);
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorDescriptorInterfaceABC* SpatioTemporalLocatorSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorSearchInterface::
SetQueryDescriptorInterface(SpatioTemporalLocatorDescriptorInterfaceABC
		       *QuerySpatioTemporalLocatorDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QuerySpatioTemporalLocatorDescriptorInterface);
}

//----------------------------------------------------------------------------
double SpatioTemporalLocatorSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//----------------------------------------------------------------------------
const UUID& SpatioTemporalLocatorSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpatioTemporalLocatorSearchInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
int SpatioTemporalLocatorSearchInterface::SetQueryPoint(MultiMediaInterfaceABC *media)
{
  return m_SearchTool->SetQueryPoint(media);
}

//----------------------------------------------------------------------------
void SpatioTemporalLocatorSearchInterface::destroy(void)
{ delete m_SearchTool; }

//=============================================================================
SpatioTemporalLocatorSearchTool::SpatioTemporalLocatorSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorSearchTool::
SpatioTemporalLocatorSearchTool(SpatioTemporalLocatorDescriptorInterfaceABC
				*aQueryDescriptionInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	/* create descriptor if it does not exist*/
	if (!aQueryDescriptionInterface) {
	  SpatioTemporalLocatorDescriptor *descriptor =
	    new SpatioTemporalLocatorDescriptor(0,0);
	  aQueryDescriptionInterface=descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetQueryDescriptorInterface(aQueryDescriptionInterface);

	/* create sub coding schemes and
	   connect them with descriptor if availabe*/
	/* no sub matching tools (see how ColorHistCS if needed)*/

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorSearchTool::~SpatioTemporalLocatorSearchTool()
{
	/* release descriptor*/
	if (m_RefDescriptorInterface)
	  m_RefDescriptorInterface->release();
	if (m_QueryDescriptorInterface)
	  m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& SpatioTemporalLocatorSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpatioTemporalLocatorSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool SpatioTemporalLocatorSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool SpatioTemporalLocatorSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int SpatioTemporalLocatorSearchTool::
SetRefDescriptorInterface(SpatioTemporalLocatorDescriptorInterfaceABC
		       *aRefDescriptorInterface)

{
    /* check if new value is different from old value*/
    if (m_RefDescriptorInterface == 
	    aRefDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_RefDescriptorInterface) 
	  m_RefDescriptorInterface->release();

	/* add new interface*/
	m_RefDescriptorInterface = 
	  aRefDescriptorInterface;
	if (m_RefDescriptorInterface) {
	  m_RefDescriptorInterface->addref();

	  /* connect sub descriptors with sub search tools*/
	  /* not available*/
	}
	else {
	  /* if no descriptor, release also descriptor interfaces from
	     sub search tools*/
	  /* not available*/
	  return -1;
	}
		
	return 0;
}

//----------------------------------------------------------------------------

SpatioTemporalLocatorDescriptorInterfaceABC* SpatioTemporalLocatorSearchTool::
GetQueryDescriptorInterface(void)
{
        return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  SpatioTemporalLocatorSearchTool::
SetQueryDescriptorInterface(SpatioTemporalLocatorDescriptorInterfaceABC
		       *aQueryDescriptorInterface)

{
    /* check if new value is different from old value*/
    if (m_QueryDescriptorInterface == 
	    aQueryDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_QueryDescriptorInterface) 
	  m_QueryDescriptorInterface->release();

	/* add new interface*/
	m_QueryDescriptorInterface = 
	  aQueryDescriptorInterface;
	if (m_QueryDescriptorInterface) {
	  m_QueryDescriptorInterface->addref();

	  /* conect sub descriptors with sub coding schemes*/
	  /* not available*/
	}
	else {
	  /* if no descriptor, release also descriptor interfaces from
	     sub extraction tools schemes*/
	  /* not available*/
	  return -1;
	}
	
	return 0;
}

//----------------------------------------------------------------------------
double SpatioTemporalLocatorSearchTool::GetDistance(void)

{
    int i,j;

    /* check for descriptors*/
    if (!m_RefDescriptorInterface) return DBL_MAX;

	if (strcmp(m_RefDescriptorInterface->GetName(),
		   "SpatioTemporalLocatorDescriptorInterface") != 0) {
	  return DBL_MAX;
	}

	// Extraction of vertices at a given time
	CPolygon Poly;
	int Type, N;

	TemporalInterpolationDescriptorInterfaceABC *TIDI;
	RegionLocatorDescriptorInterfaceABC *RLDI;

	double x, y;
	double rad, in_pro, a, b, det;
	double vec0_x, vec0_y, vec1_x, vec1_y;
	double ta, tb, fa, fb, aa;

	Type = m_RefDescriptorInterface->GetTypeOfTrajectory(0);
	Poly.Frame = -1;

	double mtime = m_RefDescriptorInterface->GetMediaTimePoint(0);

	if (Type != 1) {			// FigureTrajectory
		Poly.VNumberOfRefernceRegions = m_RefDescriptorInterface->Gettype(0);
		for (i=0; i<Poly.VNumberOfRefernceRegions; i++) {
			TIDI = m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface(0,i);
			N = TIDI->GetNumOfKeyPoint();

				for (j=N-2; j>=0; j--) {
					if ((TIDI->GetMediaTimePoint(j)+mtime <= m_Time) 
						&& (TIDI->GetMediaTimePoint(j+1)+mtime >= m_Time)) { 
						Poly.Frame = j;
						break; 
					}
				}
			}
//			if (Poly.Frame == -1) { return  DBL_MAX; }
			if (Poly.Frame == -1) { return  10.0; }//sth why 10.0 and not DBL_MAX


			ta = TIDI->GetMediaTimePoint(Poly.Frame)+mtime;
			tb = TIDI->GetMediaTimePoint(Poly.Frame+1)+mtime;
			fa = TIDI->GetKeyValue(0,Poly.Frame);
			fb = TIDI->GetKeyValue(0,Poly.Frame+1);
			aa = TIDI->GetParam(0,Poly.Frame);
			Poly.X[i] = GetAtT(ta,tb,fa,fb,aa,m_Time);
			fa = TIDI->GetKeyValue(1,Poly.Frame);
			fb = TIDI->GetKeyValue(1,Poly.Frame+1);
			aa = TIDI->GetParam(1,Poly.Frame);
			Poly.Y[i] = GetAtT(ta,tb,fa,fb,aa,m_Time);

	  }
	else {				// ParameterTrajectory
		double RefX[4], RefY[4], AffineParam[6];
		Poly.VNumberOfRefernceRegions = 4;	
		// RegionLocator
		RLDI = m_RefDescriptorInterface->GetRegionLocatorDescriptorInterface(0);
		struct loc locator;
	    locator=RLDI->GetLocator(0,0);
        RefX[0] = locator.Vertices[0];			RefY[0] = locator.Vertices[1];		
        RefX[1] = locator.Vertices[2];			RefY[1] = locator.Vertices[3];		
        RefX[2] = locator.Vertices[4];			RefY[2] = locator.Vertices[5];		
		RefX[3] = RefX[0]+ RefX[2]-RefX[1];		RefY[3] = RefY[0]+ RefY[2]-RefY[1];	

		// TI (Only Affine)
		TIDI = m_RefDescriptorInterface->
		  GetTemporalInterpolationDescriptorInterface(0,0);
		N = TIDI->GetNumOfKeyPoint();
		if (TIDI->GetConstantTimeInterval()==1) {
			j = (int)(m_Time / TIDI->GetMediaDuration());
			if (j <= N-1) { Poly.Frame = j; }
		} else {
			for (j=N-2; j>=0; j--) {
				if ((TIDI->GetMediaTimePoint(j)+mtime <= m_Time) 
					&& (TIDI->GetMediaTimePoint(j+1)+mtime >= m_Time)) { 
					Poly.Frame = j;
					break; 
				}
			}
		}
//		if (Poly.Frame == -1) { return  DBL_MAX; }
		if (Poly.Frame == -1) { return  10.0; }//sth

		ta = TIDI->GetMediaTimePoint(Poly.Frame)+mtime;
		tb = TIDI->GetMediaTimePoint(Poly.Frame+1)+mtime;
		for (i=0; i<6; i++) {												
			fa = TIDI->GetKeyValue(i,Poly.Frame);
			fb = TIDI->GetKeyValue(i,Poly.Frame+1);
			aa = TIDI->GetParam(i,Poly.Frame);
			AffineParam[i] = GetAtT(ta,tb,fa,fb,aa,m_Time);
		}

		for (j=0; j<4; j++) {															
			Poly.X[j] = AffineParam[0]*RefX[j] + AffineParam[1]*RefY[j] + AffineParam[2];
			Poly.Y[j] = AffineParam[3]*RefX[j] + AffineParam[4]*RefY[j] + AffineParam[5];
		}																				
	}

	// IsInside
	x = m_X; y = m_Y;

	vec0_x = Poly.X[Poly.VNumberOfRefernceRegions-1]-x;
	vec0_y = Poly.Y[Poly.VNumberOfRefernceRegions-1]-y;
	vec1_x = Poly.X[0]-x;
	vec1_y = Poly.Y[0]-y;
	in_pro = vec0_x*vec1_x + vec0_y*vec1_y;
	a = sqrt( vec0_x*vec0_x + vec0_y*vec0_y );
	b = sqrt( vec1_x*vec1_x + vec1_y*vec1_y );
	det = vec0_x*vec1_y - vec1_x*vec0_y;
	if( det > 0.0 ) rad = acos( in_pro/(a*b) );
	else rad = -acos( in_pro/(a*b) );
	for( i=0; i<Poly.VNumberOfRefernceRegions-1; i++ ){
		vec0_x = Poly.X[i]-x;
		vec0_y = Poly.Y[i]-y;
		vec1_x = Poly.X[i+1]-x;
		vec1_y = Poly.Y[i+1]-y;
		in_pro = vec0_x*vec1_x + vec0_y*vec1_y;
		a = sqrt( vec0_x*vec0_x + vec0_y*vec0_y );
		b = sqrt( vec1_x*vec1_x + vec1_y*vec1_y );
		det = vec0_x*vec1_y - vec1_x*vec0_y;
		if( det > 0.0 ) rad += acos( in_pro/(a*b) );
		else rad -= acos( in_pro/(a*b) );
	}
	if (rad<=3.14) return 1.0;

	// Depth
	if ((m_RefDescriptorInterface->GetDepthFlag(0)==1) && (m_FlagDepth==1)) {
	  TIDI = m_RefDescriptorInterface->
	    GetTemporalInterpolationDescriptorInterface(0,
					      MAXNRefrence*MAXNVertices+1);
	  N = TIDI->GetNumOfKeyPoint();
	  for (j=N-2; j>=0; j--) {
	    if ((TIDI->GetMediaTimePoint(j)+mtime <= m_Time) 
		&& (TIDI->GetMediaTimePoint(j+1)+mtime >= m_Time)) { 
	      Poly.Frame = j;
	      break; 
	    }
	  }
	  if (Poly.Frame == -1) { return  1.0; }

	  ta = TIDI->GetMediaTimePoint(Poly.Frame)+mtime;
	  tb = TIDI->GetMediaTimePoint(Poly.Frame+1)+mtime;
	  fa = TIDI->GetKeyValue(0,Poly.Frame);
	  fb = TIDI->GetKeyValue(0,Poly.Frame+1);
	  aa = TIDI->GetParam(0,Poly.Frame);
	  double depth = GetAtT(ta,tb,fa,fb,aa,m_Time);

	  if ((depth<m_NearDepth) || (depth>m_FarDepth)) {
	    return 1.0;
	  }
	}

	return 0.0;
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorSearchInterfaceABC *SpatioTemporalLocatorSearchTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
int SpatioTemporalLocatorSearchTool::SetQueryPoint(MultiMediaInterfaceABC *media)
{
	media->GotoKeyPoint(0);

	m_Time = media->GetKeyPoint()->time; 
	m_X = media->GetKeyPoint()->p[0];
	m_Y = media->GetKeyPoint()->p[1];

	if (media->GetKeyPoint()->dim <= 3) {	m_FlagDepth = 0; }
	else { 
		m_FlagDepth = 1;
		m_NearDepth = media->GetKeyPoint()->p[2];
		m_FarDepth = media->GetKeyPoint()->p[3];	
	}

	return 0;
}

//----------------------------------------------------------------------------
double SpatioTemporalLocatorSearchTool::GetAtT
	(double ta,double tb,double fa,double fb,double a,double t )
{
	int i;
	double Coefficient[3], va, aa;
	int Function_ID=1; 

	if( t < ta || tb < t ) return 0.0; 

	a *= 2;		

	switch( Function_ID ){
	case -1:
		for( i=0; i<3; i++ ) Coefficient[i] = 0.0;
		break;
	case 0:
		va = (fb-fa)/(tb-ta);
		Coefficient[0] = fa-va*ta;
		Coefficient[1] = va;
		Coefficient[2] = 0.0;
		break;
	case 1:
		aa = a;
		va = (fb-fa)/(tb-ta)-aa*(tb-ta)/2.0;
		Coefficient[0] = fa-va*ta+aa*ta*ta/2.0;
		Coefficient[1] = va-aa*ta;
		Coefficient[2] = aa/2.0;
		break;
	default:
		return 0.0;
	}

	return Coefficient[2]*t*t+Coefficient[1]*t+Coefficient[0];
}
