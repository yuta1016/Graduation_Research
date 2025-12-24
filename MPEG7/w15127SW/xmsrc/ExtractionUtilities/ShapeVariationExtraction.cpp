//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Minseok Choi, Hanyang University, Korea
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ShapeVariationExtraction.cpp
//

#include <stdio.h>
#include <math.h>
#ifndef _VISUALC_
#define _hypot hypot
#endif
#include "AddressLib/momusys.h"
#include "AddressLib/inter.h"
#include "AddressLib/intra.h"
#include "AddressLib/fifo.h"
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/readparam.h"

//static int initextractionwascalled=0;


//=============================================================================
	

using namespace XM;


const UUID ShapeVariationExtractionInterface::myID = UUID("");
const char *ShapeVariationExtractionInterface::myName =
"ShapeVariationDescriptorExtractorInterface";

const UUID ShapeVariationExtractionTool::myID = UUID("");
const char *ShapeVariationExtractionTool::myName = "ShapeVariationDescriptorExtractor";

const UUID ShapeVariationExtractionInterfaceABC::myID = UUID();

//=============================================================================
// Static variables

double	ShapeVariationExtractionTool::m_BasisR[ANGULAR][RADIAL][LUT_SIZE][LUT_SIZE];	// real-value of ART basis function
double	ShapeVariationExtractionTool::m_BasisI[ANGULAR][RADIAL][LUT_SIZE][LUT_SIZE];	// imaginary-value of ART basis function
bool	ShapeVariationExtractionTool::m_bLUTInit = false;

//=============================================================================
ShapeVariationExtractionInterfaceABC::ShapeVariationExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
ShapeVariationExtractionInterface::
ShapeVariationExtractionInterface(ShapeVariationExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ShapeVariationExtractionInterface::~ShapeVariationExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& ShapeVariationExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeVariationExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void ShapeVariationExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc.
int ShapeVariationExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC** media, int frame)
{
	return m_ExtractionTool->SetSourceMedia(media, frame);
}

//----------------------------------------------------------------------------
ShapeVariationDescriptorInterfaceABC* ShapeVariationExtractionInterface::
GetDescriptorInterface(void)
{
	return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ShapeVariationExtractionInterface::
SetDescriptorInterface(ShapeVariationDescriptorInterfaceABC
					   *aShapeVariationDescriptorInterface)
					   
{
	return m_ExtractionTool->
		SetDescriptorInterface(aShapeVariationDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long ShapeVariationExtractionInterface::InitExtracting(void)
{
	return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long ShapeVariationExtractionInterface::StartExtracting(void)
{
	return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long ShapeVariationExtractionInterface::PostExtracting(void)
{
	return m_ExtractionTool->PostExtracting();
}

//=============================================================================

ShapeVariationExtractionTool::ShapeVariationExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL),
m_FrameCnt(0)
{
	m_FrameCnt=0;
	m_pMap = NULL;
	m_Media = NULL;
	SetExposedInterface(&m_Interface);
}

//=============================================================================
ShapeVariationExtractionTool::
ShapeVariationExtractionTool(ShapeVariationDescriptorInterfaceABC
							*aShapeVariation):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL),
m_FrameCnt(0)
{
	/* create descriptor if it does not exist*/
	if (!aShapeVariation) {
		ShapeVariationDescriptor *descriptor = new ShapeVariationDescriptor;
		aShapeVariation=descriptor->GetInterface();
	}
		
	/* connect descritors with Extraction*/
	SetDescriptorInterface(aShapeVariation);
	
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ShapeVariationExtractionTool::~ShapeVariationExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
		m_DescriptorInterface->release();

	if( m_Media ) delete [] m_Media;
	
}

//----------------------------------------------------------------------------
bool ShapeVariationExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool ShapeVariationExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int ShapeVariationExtractionTool::SetSourceMedia(MultiMediaInterfaceABC** media, int frame)
{
	if(media==NULL) return -1;
	if(media[0]==NULL) return -1;

	if( m_Media ) delete [] m_Media;

	m_Media = new MultiMediaInterfaceABC*[frame];
	
	for( int i = 0; i < frame ; i++ )
		m_Media[i] = media[i];
  
	m_FrameCnt = frame;

	MomVop *ImageMedia = m_Media[0]->GetImage();

	int w = ImageMedia->y_chan->x;
	int h = ImageMedia->y_chan->y;
	
	m_mapWidth = (int)(w*1.5);
	m_mapHeight = (int)(h*1.5);

	return 0;
}


//----------------------------------------------------------------------------
ShapeVariationDescriptorInterfaceABC* ShapeVariationExtractionTool::
GetDescriptorInterface(void)

{
	return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int ShapeVariationExtractionTool::
SetDescriptorInterface(ShapeVariationDescriptorInterfaceABC
					   *aShapeVariationDescriptorInterface)
					   
{
	/* check if new value is different from old value*/
	if (m_DescriptorInterface == aShapeVariationDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();
	
	/* add new interface*/
	m_DescriptorInterface = aShapeVariationDescriptorInterface;
	if (m_DescriptorInterface) {
		m_DescriptorInterface->addref();
	}
	else {	
		return -1;
	}
	
	return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long ShapeVariationExtractionTool::InitExtracting(void)
{
	
	m_FrameCnt=0;//TOBEMODIFIED in you Application
	
	return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the 
unsigned long ShapeVariationExtractionTool::StartExtracting(void)

{
	// select media type you require and remove comment
	// MomVop *ImageMedia;
	// AudioType *AudioMedia;  // type of audio media still  needs to be defined
	
	// First check that it's all correctly initialised
	if (m_DescriptorInterface==NULL) return (unsigned long)-1;
	if(strcmp(m_DescriptorInterface->GetName(),
		"ShapeVariationDescriptorInterface") != 0) return (unsigned long)-1;
		
	if (m_Media==NULL) return (unsigned long)-1;
	if (m_Media[0]==NULL) return (unsigned long)-1;
	
	//ImageMedia=m_Media[0]->GetImage();
	//if (ImageMedia==NULL) return (unsigned long)-1;

	GenerateBasisLUT();
	
	// map & data initialize
	Init();
	// make low variation map
	MakeLVM();
	// extract shape of low variation map
	ExtractLVMCoefficients();
	// make high variation map
	MakeHVM();
	// extract shape of high variation map
	ExtractHVMCoefficients();
	// extract standard deviation 
	ExtractSTDCoefficients();
	// store coefficients
	Save();
	// reset map;
	Reset();
	
	// All done
	return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long ShapeVariationExtractionTool::PostExtracting(void)

{

	// In this specific case you could als write the number of frames here
	//m_DescriptorInterface->SetDummyContents(m_FrameCnt*
	//					  m_DummyExtractionParameter);
	
	return(0UL);
}

//----------------------------------------------------------------------------
const UUID& ShapeVariationExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeVariationExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
ShapeVariationExtractionInterfaceABC *ShapeVariationExtractionTool::GetInterface(void)
{ return &m_Interface; }


//----------------------------------------------------------------------------
void ShapeVariationExtractionTool::GenerateBasisLUT()
{
	if(m_bLUTInit) return;

	double angle, temp;
	int p, r, x, y;
	int maxradius, radius;

	maxradius = LUT_SIZE/2;

	for( y = 0 ; y < LUT_SIZE ; y++ )
	for( x = 0 ; x < LUT_SIZE ; x++ )
	{
		radius = (int)HYPOT((double)(x-maxradius), (double)(y-maxradius));
		if(radius < maxradius)
		{
			angle = atan2((double)(y-maxradius), (double)(x-maxradius));

			for( p = 0 ; p < ANGULAR ; p++ )
			for( r = 0 ; r < RADIAL ; r++ )
			{
				temp = cos(radius*M_PI*r/maxradius);
				m_BasisR[p][r][x][y] = temp*cos(angle*p);
				m_BasisI[p][r][x][y]  = temp*sin(angle*p);
			}
		}
		else
		{
			for( p = 0 ; p < ANGULAR ; p++)
			for( r = 0 ; r < RADIAL ; r++)
			{
				m_BasisR[p][r][x][y] = 0.;
				m_BasisI[p][r][x][y] = 0.;
			}
		}
	}
	m_bLUTInit = true;

}

//----------------------------------------------------------------------------	
void ShapeVariationExtractionTool::Init()
{
	for( int p = 0 ; p < ANGULAR ; p++ )
	for( int r = 0 ; r < RADIAL ; r++ )
	{
		m_LVMCoeffR[p][r] = 0.;
		m_LVMCoeffI[p][r] = 0.;
		m_HVMCoeffR[p][r] = 0.;
		m_HVMCoeffI[p][r] = 0.;
		m_STDCoeffR[p][r] = 0.;
		m_STDCoeffI[p][r] = 0.;
	}
	m_radius = 0;

	m_pMap = new short*[m_mapHeight];
	for( int i = 0 ; i < m_mapHeight ; i++ )
	{
		m_pMap[i] = new short[m_mapWidth];
		memset( m_pMap[i], 0, m_mapWidth*sizeof(short) );
	}
}

//----------------------------------------------------------------------------	
void ShapeVariationExtractionTool::Reset()
{
	for( int i = 0 ; i < m_mapHeight ; i++ )
	{
		delete m_pMap[i];
	}
	delete [] m_pMap;
	m_pMap = NULL;
}

//----------------------------------------------------------------------------
void ShapeVariationExtractionTool::MakeLVM()
{
	int i;
	for( i = 0 ; i < m_FrameCnt ; i++ )
	{
		m_currentMedia = m_Media[i];
		if( !m_currentMedia ) return;
		AddFrame();
	}
}


//----------------------------------------------------------------------------
void ShapeVariationExtractionTool::AddFrame()
{
	int mx = m_mapWidth/2;
	int my = m_mapHeight/2;
	int w,h;

	MomVop *ImageMedia = m_currentMedia->GetImage();

	w = ImageMedia->y_chan->x;
	h = ImageMedia->y_chan->y;

	FindCenter();
	
	unsigned char *pImage = ImageMedia->y_chan->data->u;
	unsigned char size = datasize(ImageMedia->y_chan);

	int x,y;
	int cx,cy;
	cx = (int)m_centerX;
	cy = (int)m_centerY;

	// object region accumulation of the image
	for( y = 0 ; y < h ; y++ )
	{
		for( x = 0 ; x < w ; x++ )
		{
			if( *pImage < 200 )
			{				
				m_pMap[my+(y-cy)][mx+(x-cx)]++;
			}
			pImage += size;
		}		
	}
}

//----------------------------------------------------------------------------
void ShapeVariationExtractionTool::MakeHVM()
{
	int x,y;

	for( y = 0 ; y < m_mapHeight ; y++ )
	{
		for( x = 0 ; x < m_mapWidth ; x++ )
		{
			if( m_pMap[y][x] )
				m_pMap[y][x] = m_FrameCnt-m_pMap[y][x];
		}
	}
}

//----------------------------------------------------------------------------
void ShapeVariationExtractionTool::FindCenter()
{
	unsigned int x, y;
	unsigned int m10=0, m01=0;
	
	MomVop *ImageMedia = m_currentMedia->GetImage();

	unsigned char *pImage = ImageMedia->y_chan->data->u;
	unsigned char size = datasize(ImageMedia->y_chan);
	int w = ImageMedia->y_chan->x;
	int h = ImageMedia->y_chan->y;

	m_mass = 0;

	for( y = 0 ; y < h ; y++ )
	{
		for( x = 0 ; x < w ; x++ )
		{
			if( *pImage < 200 )
			{
				m_mass++;
				m10 += x;
				m01 += y;
			}			
			pImage += size;
		}		
	}

	m_centerX = (double)m10/(double)m_mass;
	m_centerY = (double)m01/(double)m_mass;
}


//----------------------------------------------------------------------------
void ShapeVariationExtractionTool::ExtractLVMCoefficients()
{
	unsigned int x, y;
	int p, r;
	double dx, dy, tx, ty;
	double radius, maxradius=0;
	double fcount;
	double cx = m_mapWidth/2.;
	double cy = m_mapHeight/2.;
	
	for( y = 0 ; y < m_mapHeight ; y++ )
	{
		for( x = 0 ; x < m_mapWidth ; x++ )
		{
			if( m_pMap[y][x] )
			{	
				radius = _hypot(x-cx, y-cy);
				if(maxradius < radius) maxradius = radius;
			}
		}
	}

	m_lmass = 0;

	fcount = m_FrameCnt;

	for( y = 0 ; y < m_mapHeight ; y++)
	{
		for( x = 0 ; x < m_mapWidth ; x++)
		{
			if( m_pMap[y][x] )
			{
				// map image coordinate (x,y) to basis function coordinate (tx,ty)
				dx = x - cx;
				dy = y - cy;
				tx = ((dx*LUT_RADIUS)/maxradius) + LUT_RADIUS;
				ty = ((dy*LUT_RADIUS)/maxradius) + LUT_RADIUS;

				// summation of basis function
				if( tx >= 0 && tx < LUT_SIZE && ty >= 0 && ty < LUT_SIZE)
				{										
					for( p = 0 ; p < ANGULAR ; p++)
					for( r = 0 ; r < RADIAL ; r++)
					{
						m_LVMCoeffR[p][r] += (m_pMap[y][x]/fcount*GetReal(p, r, tx, ty));
						m_LVMCoeffI[p][r] -= (m_pMap[y][x]/fcount*GetImg(p, r, tx, ty));
					}
					m_lmass++;
				}
			}	
		}
	}
}

//----------------------------------------------------------------------------
void ShapeVariationExtractionTool::ExtractHVMCoefficients()
{
	unsigned int x, y;
	int p, r;
	double dx, dy, tx, ty;
	double radius, maxradius=0;
	double fcount;
	double cx = m_mapWidth/2.;
	double cy = m_mapHeight/2.;
	
	for( y = 0 ; y < m_mapHeight ; y++ )
	{
		for( x = 0 ; x < m_mapWidth ; x++ )
		{
			if( m_pMap[y][x] )
			{	
				radius = _hypot(x-cx, y-cy);
				if(maxradius < radius) maxradius = radius;
			}
		}
	}

	m_hmass = 0;

	fcount = m_FrameCnt;

	for( y = 0 ; y < m_mapHeight ; y++)
	{
		for( x = 0 ; x < m_mapWidth ; x++)
		{
			if( m_pMap[y][x] )
			{
				// map image coordinate (x,y) to basis function coordinate (tx,ty)
				dx = x - cx;
				dy = y - cy;
				tx = ((dx*LUT_RADIUS)/maxradius) + LUT_RADIUS;
				ty = ((dy*LUT_RADIUS)/maxradius) + LUT_RADIUS;

				// summation of basis function
				if( tx >= 0 && tx < LUT_SIZE && ty >= 0 && ty < LUT_SIZE)
				{										
					for( p = 0 ; p < ANGULAR ; p++)
					for( r = 0 ; r < RADIAL ; r++)
					{
						m_HVMCoeffR[p][r] += (m_pMap[y][x]/fcount*GetReal(p, r, tx, ty));
						m_HVMCoeffI[p][r] -= (m_pMap[y][x]/fcount*GetImg(p, r, tx, ty));
					}
					m_hmass++;
				}
			}	
		}
	}
}

//----------------------------------------------------------------------------
void ShapeVariationExtractionTool::ExtractSTDCoefficients()
{
	double*** mag;
	double mean[ANGULAR][RADIAL] = { {0,}, };
	
	int i, p, r, k;
	
	// memory allocation
	mag = new double**[m_FrameCnt];		
	for( i = 0 ; i < m_FrameCnt ; i++ )
	{
		mag[i] = new double*[ANGULAR];
		for( p = 0 ; p < ANGULAR ; p++ )
			mag[i][p] = new double[RADIAL];
	}
	
	// extract ART coefficient from each frame
	for( i = 0 ; i < m_FrameCnt ; i++ )
	{
		m_currentMedia = m_Media[i];
		FindCenter();
		ExtractBinaryCoefficients();

		for( p = 0 ; p < ANGULAR ; p++)
		{		
			for( r=0 ; r < RADIAL ; r++)
			{				
				mag[i][p][r] = HYPOT(m_STDCoeffR[p][r], m_STDCoeffI[p][r])/m_mass;
			}
		}
	}

	// calculate mean of the each coefficient along time(frame)
	for( i = 0 ; i < m_FrameCnt ; i++ )
	{
		for( p = 0 ; p < ANGULAR ; p++)
		{		
			for( r=0 ; r < RADIAL ; r++)
			{		
				mean[p][r] += mag[i][p][r];
			}
		}
	}
	
	for( p = 0 ; p < ANGULAR ; p++)
	{		
		for( r=0 ; r < RADIAL ; r++)
		{		
			mean[p][r] /= m_FrameCnt;
		}
	}
	
	// initial value assign
	for( p = 0 ; p < ANGULAR ; p++)
	{		
		for( r=0 ; r < RADIAL ; r++)
		{		
			m_STDCoeff[p][r] = 0.;
		}
	}
	
	// calculatate standard deviation
	for( i = 0 ; i < m_FrameCnt ; i++ )
	{
		for( p = 0 ; p < ANGULAR ; p++)
		{		
			for( r=0 ; r < RADIAL ; r++)
			{		
				m_STDCoeff[p][r] += (mean[p][r] - mag[i][p][r])*(mean[p][r] - mag[i][p][r]);
			}
		}			
	}
	for( p = 0 ; p < ANGULAR ; p++)
	{		
		for( r=0 ; r < RADIAL ; r++)
		{		
			m_STDCoeff[p][r] = sqrt(m_STDCoeff[p][r]/m_FrameCnt);	
		}
	}

	// memory free
	for( i = 0 ; i < m_FrameCnt ; i++ )
	{
		for( p = 0 ; p < ANGULAR ; p++)
		{				
			delete [] mag[i][p];
		}
		delete [] mag[i];
	}
	delete [] mag;
			
}

//----------------------------------------------------------------------------
void ShapeVariationExtractionTool::ExtractBinaryCoefficients()
{
	unsigned int x, y;
	int p, r;
	double dx, dy, tx, ty;
	double radius, maxradius=0;
		
	double cx = m_centerX;
	double cy = m_centerY;

	MomVop *ImageMedia = m_currentMedia->GetImage();

	unsigned char *pImage = ImageMedia->y_chan->data->u;
	unsigned char size = datasize(ImageMedia->y_chan);
	int w = ImageMedia->y_chan->x;
	int h = ImageMedia->y_chan->y;
		
	for( y = 0 ; y < h ; y++ )
	{
		for( x = 0 ; x < w ; x++ )
		{
			if( *pImage < 200 )
			{
				radius = _hypot(x-cx, y-cy);
				if(maxradius < radius) maxradius = radius;
			}			
			pImage += size;			
		}
	}

	m_mass = 0;
	pImage = ImageMedia->y_chan->data->u;

	for( p = 0 ; p < ANGULAR ; p++)
	{
		for( r=0 ; r < RADIAL ; r++)
		{
			m_STDCoeffR[p][r] = 0.f;
			m_STDCoeffI[p][r] = 0.f;
		}
	}	

	for( y = 0 ; y < h ; y++)
	{
		for( x = 0 ; x < w ; x++)
		{
			if( *pImage < 200 )
			{
				// map image coordinate (x,y) to basis function coordinate (tx,ty)
				dx = x - cx;
				dy = y - cy;
				tx = ((dx*LUT_RADIUS)/maxradius) + LUT_RADIUS;
				ty = ((dy*LUT_RADIUS)/maxradius) + LUT_RADIUS;

				// summation of basis function
				if( tx >= 0 && tx < LUT_SIZE && ty >= 0 && ty < LUT_SIZE)
				{										
					for( p = 0 ; p < ANGULAR ; p++)
					for( r=0 ; r < RADIAL ; r++)
					{
						m_STDCoeffR[p][r] += GetReal(p, r, tx, ty);
						m_STDCoeffI[p][r] -= GetImg(p, r, tx, ty);
					}
					m_mass++;
				}
			}	
			pImage += size;
		}
	}	
}

//----------------------------------------------------------------------------
void ShapeVariationExtractionTool::Save()
{
	double val;
	for( int p = 0 ; p < ANGULAR ; p++)
	{
		for( int r = 0 ; r < RADIAL ; r++)
		{
			if( p == 0 && r == 0 ) continue;
			val = HYPOT(m_LVMCoeffR[p][r], m_LVMCoeffI[p][r])/m_lmass;
			m_DescriptorInterface->SetLVMElement(p,r,val);
			val = HYPOT(m_HVMCoeffR[p][r], m_HVMCoeffI[p][r])/m_hmass;
			m_DescriptorInterface->SetHVMElement(p,r,val);			
			m_DescriptorInterface->SetSTDElement(p,r,2*m_STDCoeff[p][r]);
		}
	}
}

//----------------------------------------------------------------------------
double ShapeVariationExtractionTool::GetReal(int p, int r, double dx, double dy)
{

	int x = (int)dx;
	int y = (int)dy;

	double ix = dx - x;
	double iy = dy - y;

	double x1, x2;
	x1 = x2 = 0.;
	
	if( ix != 0 )
	{
		x1 = m_BasisR[p][r][x][y] + (m_BasisR[p][r][x+1][y]-m_BasisR[p][r][x][y]) * ix;
		if( iy != 0 )
			x2 = m_BasisR[p][r][x][y+1] + (m_BasisR[p][r][x+1][y+1]-m_BasisR[p][r][x][y+1]) * ix;
	}
	else
	{
		x1 = m_BasisR[p][r][x][y];
		if( iy != 0 )
			x2 = m_BasisR[p][r][x][y+1];
	}	

	return (x1 + (x2-x1) * iy);
}

//----------------------------------------------------------------------------
double ShapeVariationExtractionTool::GetImg(int p, int r, double dx, double dy)
{
	int x = (int)dx;
	int y = (int)dy;

	double ix = dx - x;
	double iy = dy - y;

	double x1, x2;
	x1 = x2 = 0.;

	if( ix != 0 )
	{
		x1 = m_BasisI[p][r][x][y] + (m_BasisI[p][r][x+1][y]-m_BasisI[p][r][x][y]) * ix;
		if( iy != 0 )
			x2 = m_BasisI[p][r][x][y+1] + (m_BasisI[p][r][x+1][y+1]-m_BasisI[p][r][x][y+1]) * ix;
	}
	else
	{
		x1 = m_BasisI[p][r][x][y];
		if( iy != 0 )
			x2 = m_BasisI[p][r][x][y+1];
	}

	return (x1 + (x2-x1) * iy);
}
