//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Yong-Sung Kim - Hanyang University, Korea
// (contributing organizations names)
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
// Applicable File Name:  RegionShapeExtraction.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <inter.h>
#include <intra.h>
#include <fifo.h>
#include <math.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"


//=============================================================================
using namespace XM;


const UUID RegionShapeExtractionInterface::myID = UUID("");
const char *RegionShapeExtractionInterface::myName = "RegionShape Region Shape Extractor Interface";

const UUID RegionShapeExtractionTool::myID = UUID("");
const char *RegionShapeExtractionTool::myName = "RegionShape Region Shape Extractor";

const UUID RegionShapeExtractionInterfaceABC::myID = UUID();


//=============================================================================
// Static variables

double	RegionShapeExtractionTool::m_pBasisR[ART_ANGULAR][ART_RADIAL][ART_LUT_SIZE][ART_LUT_SIZE];	// real-value of RegionShape basis function
double	RegionShapeExtractionTool::m_pBasisI[ART_ANGULAR][ART_RADIAL][ART_LUT_SIZE][ART_LUT_SIZE];	// imaginary-value of RegionShape basis function
bool	RegionShapeExtractionTool::m_bLUTInit = false;


//=============================================================================
RegionShapeExtractionInterfaceABC::RegionShapeExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}


//=============================================================================
RegionShapeExtractionInterface::RegionShapeExtractionInterface(RegionShapeExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}


//----------------------------------------------------------------------------
RegionShapeExtractionInterface::~RegionShapeExtractionInterface()
{
}


//----------------------------------------------------------------------------
const UUID& RegionShapeExtractionInterface::GetInterfaceID(void)
{ return myID; }


//----------------------------------------------------------------------------
const char *RegionShapeExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void RegionShapeExtractionInterface::destroy(void)
{
	delete m_ExtractionTool;
}

//----------------------------------------------------------------------------
int RegionShapeExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC
						   *media)
{
	return m_ExtractionTool->SetSourceMedia(media);
}


//----------------------------------------------------------------------------
RegionShapeDescriptorInterfaceABC *RegionShapeExtractionInterface::
	GetDescriptorInterface(void)
{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int RegionShapeExtractionInterface::
SetDescriptorInterface(RegionShapeDescriptorInterfaceABC
		       *aRegionShapeDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(aRegionShapeDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long RegionShapeExtractionInterface::StartExtracting(void)
{
  return m_ExtractionTool->StartExtracting();
}


//=============================================================================
RegionShapeExtractionTool::RegionShapeExtractionTool():
m_Interface(this),
m_DescriptorInterface(0),
m_Media(0)

{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
RegionShapeExtractionTool::
RegionShapeExtractionTool(RegionShapeDescriptorInterfaceABC *art) :
m_Interface(this),
m_DescriptorInterface(0),
m_Media(0)
{
#ifdef VERBOSE
  fprintf(stderr, "Connect descriptor\n");
#endif

  if (!art)
  {
    RegionShapeDescriptor *descriptor = new RegionShapeDescriptor();
    art = descriptor->GetInterface();
  }

  SetDescriptorInterface(art);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
RegionShapeExtractionTool::~RegionShapeExtractionTool()
{
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool RegionShapeExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool RegionShapeExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int RegionShapeExtractionTool::SetSourceMedia(MultiMediaInterfaceABC *media)
{
  if (!media) return -1;

  m_MultiMedia=media;
  if (m_MultiMedia) {
    m_Media=m_MultiMedia->GetImage();
  }
  else {
    m_Media=0;
  }
  return 0;
}


//----------------------------------------------------------------------------
int RegionShapeExtractionTool::
SetDescriptorInterface(RegionShapeDescriptorInterfaceABC
		       *aRegionShapeDescriptorInterface)

{
  if (m_DescriptorInterface == aRegionShapeDescriptorInterface) 
    return 0;

	if (m_DescriptorInterface) m_DescriptorInterface->release();
	
	m_DescriptorInterface = aRegionShapeDescriptorInterface;
	
	if (m_DescriptorInterface)
		m_DescriptorInterface->addref();
	else
		return -1;

	return 0;
}

//----------------------------------------------------------------------------
RegionShapeExtractionInterfaceABC *RegionShapeExtractionTool::GetInterface(void)
{
  return &m_Interface;
}

//----------------------------------------------------------------------------
RegionShapeDescriptorInterfaceABC *RegionShapeExtractionTool::
GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
unsigned long RegionShapeExtractionTool::StartExtracting(void)
{
	// check for descriptor
	if (!m_DescriptorInterface) return (unsigned long)-1;

	if (strcmp(m_DescriptorInterface->GetName(),
		   "Region Shape Descriptor Interface") != 0)
	  return (unsigned long)-1;

	/* check for media*/
	if (!m_MultiMedia) return (unsigned long)-1;
	m_Media=m_MultiMedia->GetImage();
	if (!m_Media) return (unsigned long)-1;

	GenerateBasisLUT();
	Reset();
	FindCenterOfMass();
	FindRadius();
	ExtractCoefficients();
	Save();

	return 0;
}

//----------------------------------------------------------------------------
const UUID& RegionShapeExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RegionShapeExtractionTool::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
inline void RegionShapeExtractionTool::GenerateBasisLUT()
{
	if(m_bLUTInit) return;

	double angle, temp, radius;
	int p, r, x, y;
	int maxradius;

	maxradius = ART_LUT_SIZE/2;

	for(y=0 ; y<ART_LUT_SIZE ; y++)
	for(x=0 ; x<ART_LUT_SIZE ; x++)
	{
		radius = HYPOT((double)x-maxradius, (double)y-maxradius);
		if(radius < maxradius)
		{
			angle = atan2((double)y-maxradius, (double)x-maxradius);

			for(p=0 ; p<ART_ANGULAR ; p++)
			for(r=0 ; r<ART_RADIAL ; r++)
			{
				temp = cos(radius*M_PI*r/maxradius);
				m_pBasisR[p][r][x][y] = temp*cos(angle*p);
				m_pBasisI[p][r][x][y]  = temp*sin(angle*p);
			}
		}
		else
		{
			for(p=0 ; p<ART_ANGULAR ; p++)
			for(r=0 ; r<ART_RADIAL ; r++)
			{
				m_pBasisR[p][r][x][y] = 0;
				m_pBasisI[p][r][x][y]  = 0;
			}
		}
	}

	m_bLUTInit = true;
}

//----------------------------------------------------------------------------
inline void RegionShapeExtractionTool::FindCenterOfMass()
{
	unsigned int x, y;
	unsigned int m10=0, m01=0;
	
	unsigned char *pImage = m_Media->y_chan->data->u;
	unsigned char size = datasize(m_Media->y_chan);

	m_mass = 0;

	for (y=0 ; y<m_Media->y_chan->y ; y++)
	for (x=0 ; x<m_Media->y_chan->x ; x++)
	{
		if( *pImage < 128 )
		{
			m_mass++;
			m10 += x;
			m01 += y;
		}
		pImage += size;
	}

	m_centerX = (double)m10/(double)m_mass;
	m_centerY = (double)m01/(double)m_mass;
}

//----------------------------------------------------------------------------
inline void RegionShapeExtractionTool::FindRadius()
{
	unsigned int x, y;
	double temp_radius;
	
	unsigned char *pImage = m_Media->y_chan->data->u;
	unsigned char size = datasize(m_Media->y_chan);

	for (y=0 ; y<m_Media->y_chan->y ; y++)
	for (x=0 ; x<m_Media->y_chan->x ; x++)
	{
		if( *pImage < 128 )
		{
			temp_radius = HYPOT(x-m_centerX, y-m_centerY);
			if(temp_radius > m_radius) m_radius = temp_radius;
		}
		pImage += size;
	}
}

//----------------------------------------------------------------------------
inline void RegionShapeExtractionTool::ExtractCoefficients()
{
	unsigned int x, y;
	int p, r;
	double dx, dy, tx, ty;
	
	unsigned char *pImage = m_Media->y_chan->data->u;
	unsigned char size = datasize(m_Media->y_chan);

	for (y=0 ; y<m_Media->y_chan->y ; y++)
	for (x=0 ; x<m_Media->y_chan->x ; x++)
	{
		if( *pImage < 128 )
		{
			// map image coordinate (x,y) to basis function coordinate (tx,ty)
			dx = x - m_centerX;
			dy = y - m_centerY;
			tx = ((dx*ART_LUT_RADIUS) / m_radius) + ART_LUT_RADIUS;
			ty = ((dy*ART_LUT_RADIUS) / m_radius) + ART_LUT_RADIUS;

			// summation of basis function
			if(tx >= 0 && tx < ART_LUT_SIZE && ty >= 0 && ty < ART_LUT_SIZE)
			
			for(p=0 ; p<ART_ANGULAR ; p++)
			for(r=0 ; r<ART_RADIAL ; r++)
			{
				m_pCoeffR[p][r] += GetReal(p, r, tx, ty);
				m_pCoeffI[p][r] -= GetImg(p, r, tx, ty);
			}
		}
		pImage += size;
	}
}

//----------------------------------------------------------------------------
inline double RegionShapeExtractionTool::GetReal(int p, int r, double dx, double dy)
{
	int x = (int)dx;
	int y = (int)dy;

	double ix = dx - x;
	double iy = dy - y;

	double x1 = m_pBasisR[p][r][x][y] + (m_pBasisR[p][r][x+1][y]-m_pBasisR[p][r][x][y]) * ix;
	double x2 = m_pBasisR[p][r][x][y+1] + (m_pBasisR[p][r][x+1][y+1]-m_pBasisR[p][r][x][y+1]) * ix;

	return (x1 + (x2-x1) * iy);
}

//----------------------------------------------------------------------------
inline double RegionShapeExtractionTool::GetImg(int p, int r, double dx, double dy)
{
	int x = (int)dx;
	int y = (int)dy;

	double ix = dx - x;
	double iy = dy - y;

	double x1 = m_pBasisI[p][r][x][y] + (m_pBasisI[p][r][x+1][y]-m_pBasisI[p][r][x][y]) * ix;
	double x2 = m_pBasisI[p][r][x][y+1] + (m_pBasisI[p][r][x+1][y+1]-m_pBasisI[p][r][x][y+1]) * ix;

	return (x1 + (x2-x1) * iy);
}

//----------------------------------------------------------------------------
inline void RegionShapeExtractionTool::Reset()
{
	for(int p=0 ; p<ART_ANGULAR ; p++)
	for(int r=0 ; r<ART_RADIAL ; r++)
	{
		m_pCoeffR[p][r] = 0;
		m_pCoeffI[p][r] = 0;
	}

	m_radius = 0;
}

//----------------------------------------------------------------------------
inline void RegionShapeExtractionTool::Save()
{
	for(int r=0 ; r<ART_RADIAL ; r++)
	for(int p=0 ; p<ART_ANGULAR ; p++)		
	{
		m_DescriptorInterface->SetElement(p, r, HYPOT(m_pCoeffR[p][r]/m_mass, m_pCoeffI[p][r]/m_mass));
	}
}
