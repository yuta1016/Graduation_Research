///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric R&D Centre Europe
// P. Brasnett, W. Price
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
// Applicable File Name:  ImageSignatureExtraction.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include <momusys.h>
#include <inter.h>
#include <intra.h>
#include <fifo.h>
#include <cv.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <list>
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include <functional>

#include <assert.h>


#ifndef M_PI
#define M_PI (3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067)
#endif

#define EPSILON 1e-9
#define IF1 1
#define IF3 3
#define IF6 6
#define IFS8 99
#define IFS16 100
#define IFS32 101
#define IFS64 102
#define IFS128 103
#define IFX8 499
#define IFX16 500
#define IFX32 501
#define IFX64 502
#define IFX128 503
#define NUM_DIAMETRIC_FUNCS 12
#define NUM_DIAMETRIC_FUNCS_FT 8

#define CONE_STRIP_LIMITS_FT 0.4
#define CONE_STRIP_LIMITS_FT2 4
#define CONE_STRIP_LIMITS_FT3 5
#define CONE_STRIP_LIMITS_FT4 7
#define CONE_STRIP_LIMITS_FT5 8

#define CONE_STRIP_LIMITS 1.5
#define CONE_STRIP_LIMITS4 17.5
#define CONE_STRIP_LIMITS5 18.5
#define CONE_STRIP_LIMITS6 33.5
#define CONE_STRIP_LIMITS7 34.5
#define CONE_STRIP_LIMITS8 50.8
#define CONE_STRIP_LIMITS9 51.5
#define CONE_STRIP_LIMITS2 67.5
#define CONE_STRIP_LIMITS3 68.5

#define MAX_FEATURES 80
#define SIGMA_IS 0.90
#define HARRIS_ALPHA 0.04
#define KAPPA 0.6
#define BASE_REGION_SIZE 20
#define LEVELS 12
#define SCALE_FACTOR 1.18
#define INTER_FEATURE_DISTANCE 10

using namespace XM;
using namespace std;

class featurePoint{
	int x;
	int y;
	int scale;
	int angle;
	double score;
public:
	featurePoint(int loc_x, int loc_y, int scale, int angle, double score_in);
	void print();
	
	int get_x(){ return x; }
	int get_y(){ return y; }
	int get_scale() { return scale; }	
	double get_score(){ return score; }
	int get_angle(){ return angle; }
	void set_x( int x_ ) { x=x_;}
	void set_y( int y_ ) { y=y_;}
	void set_scale( int scale_ ) { scale=scale_; }
	void set_score( double score_ ) { score=score_; }
	void set_angle( int angle_ ) { angle=angle_; }
	
	int getRegion()
	{
		int region=0;
		if( x>0 && x<96 )
		{
			if( y>0 && y<96)
			{
				region=1;
			}
			else
			{
				region=2;
			}
		}
		else
		{
			if( y>0 && y<96)
			{
				region=3;
			}
			else
			{
				region=4;
			}
		}
		return region;
	}
	
	
	bool operator<(const featurePoint fp2) const
	{
		return (score < fp2.score ) ? true : false;
	}
	
	bool operator>(const featurePoint fp2) const
	{
		return (score > fp2.score ) ? true : false;
	}
	
	bool near_border(int width, int height)
	{
		//int distance=(int)cvRound(BASE_REGION_SIZE*pow((double)(SCALE_FACTOR),(double)(scale))/2.0);
		int lscale=MIN(scale+1,LEVELS);
		int distance=(int)cvRound(BASE_REGION_SIZE*pow((double)(SCALE_FACTOR),(double)(lscale))/2.0);
		if(x<distance || y<distance || x>(width-distance) || y>(height-distance))
			return true;
		else 
			return false;
	}
	double distance( featurePoint fp2)
	{
		int dx=x-fp2.x;
		int dy=y-fp2.y;
		return sqrt((double)(dx*dx+dy*dy));
	}
	
};

featurePoint::featurePoint(int loc_x, int loc_y, int scale_, int angle_, double score_in)
{
	x=loc_x;
	y=loc_y;
	scale=scale_;
	angle=angle_;
	score=score_in;
}



//=============================================================================

const UUID ImageSignatureExtractionInterface::myID = UUID();
const char *ImageSignatureExtractionInterface::myName = "Streaming Component Control Interface";

const UUID ImageSignatureExtractionTool::myID = UUID();
const char *ImageSignatureExtractionTool::myName = "Image Signature Descriptor Extractor";

const UUID ImageSignatureExtractionInterfaceABC::myID = UUID();

//=============================================================================
ImageSignatureExtractionInterfaceABC::ImageSignatureExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
ImageSignatureExtractionInterface::ImageSignatureExtractionInterface(ImageSignatureExtractionTool* aTool)
: m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ImageSignatureExtractionInterface::~ImageSignatureExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& ImageSignatureExtractionInterface::GetInterfaceID(void)
{
	return myID;
}

//----------------------------------------------------------------------------
const char *ImageSignatureExtractionInterface::GetName(void)
{
	return myName;
}

//----------------------------------------------------------------------------
void ImageSignatureExtractionInterface::destroy(void)
{
	delete m_ExtractionTool;
}

//----------------------------------------------------------------------------
int ImageSignatureExtractionInterface::
SetSourceMedia(MultiMediaInterfaceABC *media)
{
	return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
ImageSignatureDescriptorInterfaceABC *ImageSignatureExtractionInterface::
GetDescriptorInterface(void)

{
	return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ImageSignatureExtractionInterface::
SetDescriptorInterface(ImageSignatureDescriptorInterfaceABC
					   *aImageSignatureDescriptorInterface)
					   
{
	return m_ExtractionTool->SetDescriptorInterface(aImageSignatureDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long ImageSignatureExtractionInterface::InitExtracting(void)
{
	return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long ImageSignatureExtractionInterface::StartExtracting(void)
{
	return m_ExtractionTool->StartExtracting();
}


//=============================================================================
ImageSignatureExtractionTool::ImageSignatureExtractionTool()
: m_Interface(this)
, m_DescriptorInterface(0)
, m_Media(0)
, m_ImageMedia(0)  
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ImageSignatureExtractionTool::
ImageSignatureExtractionTool(ImageSignatureDescriptorInterfaceABC
							 *contourshape)
							 : m_Interface(this)
							 , m_DescriptorInterface(0)
							 , m_Media(0)
							 , m_ImageMedia(0)  
{
#ifdef VERBOSE
	fprintf(stderr, "Connect descriptor\n");
#endif
	
	if (!contourshape)
	{
		ImageSignatureDescriptor *descriptor = new ImageSignatureDescriptor();
		contourshape = descriptor->GetInterface();
	}
	
	SetDescriptorInterface(contourshape);
	
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ImageSignatureExtractionTool::~ImageSignatureExtractionTool()
{
	if (m_DescriptorInterface)
		m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool ImageSignatureExtractionTool::SupportsPush(void)
{
	return true;
}

//----------------------------------------------------------------------------
bool ImageSignatureExtractionTool::SupportsPull(void)
{
	return false;
}

//----------------------------------------------------------------------------
int ImageSignatureExtractionTool::SetSourceMedia(MultiMediaInterfaceABC *media)
{
	if (!media) return -1;
	
	m_Media=media;
	return 0;
}


//----------------------------------------------------------------------------
ImageSignatureDescriptorInterfaceABC *ImageSignatureExtractionTool::
GetDescriptorInterface(void)
{
	return m_DescriptorInterface;
}


//----------------------------------------------------------------------------
int ImageSignatureExtractionTool::
SetDescriptorInterface(ImageSignatureDescriptorInterfaceABC
					   *aImageSignatureDescriptorInterface)
					   
{
	if (m_DescriptorInterface ==
		aImageSignatureDescriptorInterface) return 0;
	
	if (m_DescriptorInterface)
		m_DescriptorInterface->release();
	
	m_DescriptorInterface = aImageSignatureDescriptorInterface;
	if (m_DescriptorInterface)
	{
		m_DescriptorInterface->addref();
	}
	else
	{
		return -1;
	}
	
	return 0;
}


//----------------------------------------------------------------------------
unsigned long ImageSignatureExtractionTool::InitExtracting(void)
{
	return 0UL;
}


//----------------------------------------------------------------------------
unsigned long ImageSignatureExtractionTool::StartExtracting(void)
{
	if (!m_DescriptorInterface)
		return (unsigned long)-1;
	
	if (strcmp(m_DescriptorInterface->GetName(),
		"Image Signature Description Interface") != 0)
		return (unsigned long)-1;
	
	if (m_Media==NULL) return (unsigned long)-1;
	m_ImageMedia=m_Media->GetImage();
	if (m_ImageMedia==NULL) return (unsigned long)-1;
	
	ExtractSignatures();
	
	return 0;
}
//----------------------------------------------------------------------------
void ImageSignatureExtractionTool::subCircle( IplImage* src, IplImage* dst )
{
	CvSize size = cvGetSize(src);
	double cx=((size.width-1)/2.0);
	double cy=((size.height-1)/2.0);
	double radius=MIN(cx,cy);
	//int d_int;
	double dx, dy, d, dist, buf;
	for( int i=0; i<size.width; i++ )
    {
		for( int j=0; j<size.height; j++ )
		{
			dx=cx-i;
			dy=cy-j;
			d=(sqrt((dx*dx)+(dy*dy)));
			if( d>(radius) )
				cvSet2D( dst, j, i, cvScalar(0,0,0,0) );
			else
			{
				dist=(radius-d)/7;
				if(dist<0.0)
					dist=0.0;
				if(dist>1.0)
					dist=1.0;
				
				buf=dist*(cvGet2D(src, j, i).val[0]);
				cvSet2D( dst, j, i, cvScalar(buf,0,0,0) );
			}
		}
    }
	
}

void ImageSignatureExtractionTool::subCircleFT( CvMat* src, CvMat* dst )
{
	CvSize size = cvGetSize(src);
	double cx=((size.width-1)/2.0);
	double cy=((size.height-1)/2.0);
	double radius=MIN(cx,cy);
	double dx, dy, d, dist, buf;
	for( int i=0; i<size.width; i++ )
    {
		for( int j=0; j<size.height; j++ )
		{
			dx=cx-i;
			dy=cy-j;
			d=(sqrt((dx*dx)+(dy*dy)));
			if( d>(radius) )
				cvmSet(dst,j,i,0);
			else
			{
				dist=(radius-d)/3;
				if(dist<0.0)
					dist=0.0;
				if(dist>1.0)
					dist=1.0;

				buf=dist*(cvmGet(src,j,i));
				cvmSet(dst,j,i,buf);
			}
		}
    }
}

//----------------------------------------------------------------------------
void ImageSignatureExtractionTool::coneFPFT( CvMat *center_strip, IplImage *cone, int T, int cone_size, int type )
{
	int num_strips=center_strip->rows;
	cvZero(cone);
	int j;
	
	for( int i=0; i<num_strips; i++ )
    {
		CvMat* cone_strip = cvCreateMat( 1, T/cone_size, CV_32F );
		
		for( j=0;j<T;j+=cone_size)
		{
			double temp=0.0;
			for(int k=0; k<cone_size; k++ )
			{
				if( type==1 )
					temp+=cvmGet( center_strip,i, j+k );
				
			}
			cvmSet( cone_strip, 0, j/cone_size, temp );
		}
		IplImage* cone_stripFFT = cvCreateImage( cvSize((T/cone_size),1), IPL_DEPTH_32F, 1 );
		IplImage* magnitude = cvCreateImage( cvSize(((T/cone_size)+1)/2,1), IPL_DEPTH_32F, 1 );		
		
		
		cvDFT(cone_strip, cone_stripFFT, CV_DXT_FORWARD, 0);
		
		vilModulus(cone_stripFFT, magnitude);		
		cvReleaseImage(&cone_stripFFT);	
		cvReleaseMat( &cone_strip );
		
		// Take the log of the magnitude
		cvLog(magnitude, magnitude);
		
		IplImage* magnitude2 = cvCreateImage( cvSize(((T/cone_size)+1)/2,1), IPL_DEPTH_32F, 1 );
		vilGrad(magnitude,magnitude2);
		cvZero(magnitude);
		
		IplImage* magnitude3 = cvCreateImage( cvSize(((T/cone_size)+1)/2,1), IPL_DEPTH_32F, 1 );
		
		double avg=mean(magnitude2, 8, 1);
		cvSubS( magnitude2, cvScalar(avg,0,0,0), magnitude, NULL );
		cvThreshold( magnitude, magnitude3, 0, 1, CV_THRESH_BINARY);
		cvReleaseImage(&magnitude2);
		cvReleaseImage(&magnitude);
		int numbits=8;
		
		for( j=0; j<numbits; j++ )
		{
			double temp=cvGet2D(magnitude3, 0, j+1).val[0];			
			cvSet2D( cone, 0, j+(i*numbits), cvScalar(temp,0,0,0));
		}
		
		cvReleaseImage(&magnitude3);
    }
}

void ImageSignatureExtractionTool::coneFP( CvMat *center_strip, IplImage *cone, int T, int cone_size, int type )
{
	int num_strips=center_strip->rows;
	cvZero(cone);
	int j;
	
	for( int i=0; i<num_strips; i++ )
    {
		CvMat* cone_strip = cvCreateMat( 1, T/cone_size, CV_32F );
		
		for( j=0;j<T;j+=cone_size)
		{
			double temp=0.0;
			for(int k=0; k<cone_size; k++ )
			{
				if( type==1 )
					temp+=cvmGet( center_strip,i, j+k );
			}
			cvmSet( cone_strip, 0, j/cone_size, temp );
		}
		
		IplImage* cone_stripFFT = cvCreateImage( cvSize((T/cone_size),1), IPL_DEPTH_32F, 1 );
		IplImage* magnitude = cvCreateImage( cvSize(((T/cone_size)+1)/2,1), IPL_DEPTH_32F, 1 );
		IplImage* magnitude2 = cvCreateImage( cvSize(((T/cone_size)+1)/2,1), IPL_DEPTH_32F, 1 );
		
		cvDFT(cone_strip, cone_stripFFT, CV_DXT_FORWARD, 0);
		
		vilModulus(cone_stripFFT, magnitude);		
		
		// Take the log of the magnitude
		cvLog(magnitude, magnitude);
		
		vilGrad(magnitude,magnitude2);

		double avg=mean(magnitude2, ((T/cone_size)+1)/2, 1);
		cvSubS( magnitude2, cvScalar(avg,0,0,0), magnitude2, NULL );
		cvThreshold( magnitude2, magnitude2, 0, 1, CV_THRESH_BINARY);
		
		int numbits=8*((((T/cone_size)/2+7)>>3)-1);

		for( j=0; j<numbits; j++ )
		{
			double temp=cvGet2D(magnitude2, 0, j+1).val[0];			
			cvSet2D( cone, 0, j+(i*numbits), cvScalar(temp,0,0,0));
		}
		
		cvReleaseImage(&cone_stripFFT);
		cvReleaseImage(&magnitude);
		cvReleaseImage(&magnitude2);
		cvReleaseMat( &cone_strip );

    }
}

void ImageSignatureExtractionTool::vilGrad(IplImage *in, IplImage *out)
{
	CvSize size=cvGetSize(in);
	for(int i=0; i<(size.width-1);i++)
	{
		double buf=cvGet2D(in, 0, i+1).val[0]-cvGet2D(in, 0, i).val[0];
		cvSet2D(out, 0, i, cvScalar(buf, 0,0,0));
	}
	cvSet2D(out,0,(size.width-1), cvScalar(0, 0,0,0));
}

//----------------------------------------------------------------------------
unsigned long ImageSignatureExtractionTool::ExtractSignatures( )
{
	int i;
	ImageSignatureGlobalBits bits;
	for( i=0; i<IMAGESIGNATURE_BYTES_GLOBAL; i++ )
		bits[i]=0;
	ImageSignatureGlobalBits a_bits;
	for( i=0; i<IMAGESIGNATURE_BYTES_GLOBAL; i++ )
		a_bits[i]=0;
	ImageSignatureBits f_bits;
	for( i=0; i<IMAGESIGNATURE_BYTES; i++ )
		f_bits[i]=0;
	ExtractSignature(bits);
	m_DescriptorInterface->SetGlobalSignature(bits);

	ExtractFeatureSignature( f_bits);
	m_DescriptorInterface->SetFeatureSignature(f_bits);
	
	ExtractARCSignature( a_bits);
	m_DescriptorInterface->SetAspectSignature(a_bits);

	return 0;
}

unsigned long ImageSignatureExtractionTool::ExtractSignature( ImageSignatureGlobalBits &bits )
{
	// rgb is vyu in MomVop!!
	int sig_head=0;
	MomImage *imbuf_r, *imbuf_g, *imbuf_b;
	imbuf_r=m_ImageMedia->v_chan;
	imbuf_g=m_ImageMedia->y_chan;
	imbuf_b=m_ImageMedia->u_chan;
	// OpenCV stores colour images as BGR
	IplImage *im_original=cvCreateImage( cvSize(imbuf_r->x,imbuf_r->y),IPL_DEPTH_8U, 3);
	MomImageToIplImage( imbuf_r, imbuf_g, imbuf_b, im_original );
	
	IplImage *im = cvCreateImage( cvSize( imbuf_r->x, imbuf_r->y), IPL_DEPTH_8U, 1);
	
	cvCvtColor( im_original, im, CV_BGR2GRAY);
	
	cvReleaseImage( &im_original );
	int tracefunc=IF1;
	int diametricfunc[NUM_DIAMETRIC_FUNCS]={IF3, IF6, IFS8, IFS16, IFS32, IFS64, IFS128, IFX8, IFX16, IFX32, IFX64, IFX128};	
	
	int M=192;
	int T=600;
	int R=384;
	int df;
	int i;
	unsigned char select_bits[1008]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,0,1,1,0,0,1,1,1,0,1,0,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,0,1,1,0,0,0,0,1,1,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,0,1,1,1,0,1,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
0,0,0,0,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0};	

	if (!im)
		return false;
	
	CvSize size = cvGetSize(im);	
	
	float scale;
	scale=M/MIN((float)size.height, (float)size.width);
	
	int imwidth=(int)floor(scale*size.width+0.5);
	int imheight=(int)floor(scale*size.height+0.5);

	IplImage* im2;
	im2 = cvCreateImage( cvSize(imwidth,imheight), IPL_DEPTH_8U, 1 ); 

	
	if(scale<1)
	{
		CvMat *filter=lanczos( scale );
		cvFilter2D( im, im, filter );
		cvReleaseMat( &filter );
		cvResize( im, im2, CV_INTER_LINEAR );

	}
	else
	{
		cvResize( im, im2, CV_INTER_CUBIC );
	}
	cvReleaseImage(&im);

	subCircle( im2, im2 );
	cvSmooth( im2, im2, CV_GAUSSIAN, 3, 3 );

	IplImage* im2a = cvCreateImage( cvSize(M,M), IPL_DEPTH_8U, 1 );
	cvGetRectSubPix( im2, im2a, cvPoint2D32f((imwidth/2.0)-0.5,(imheight/2.0)-0.5) );

	cvReleaseImage( &im2 );
	// Convert the image to 32 bit floating point numbers
	IplImage* im3 = cvCreateImage( cvSize(M,M), IPL_DEPTH_32F, 1 );
	cvConvert(im2a, im3 );	
	cvReleaseImage( &im2a );
	
	// Create the output (destination) image
	CvMat* traceMatrix = cvCreateMat( R, T, CV_32F );
	// Create a buffer
	CvMat matbuf;
	// Create the source image as a matrix
	CvMat* matim;
	matim=cvGetMat(im3, &matbuf, NULL, 0);
	
	CvMat* bands = cvCreateMat( 8, T, CV_32F );
	cvSetZero(bands);
	CvMat* circles = cvCreateMat( 1, 195, CV_32F );

	cvSetZero(circles);

	trace( matim, traceMatrix, bands, circles, tracefunc, 1 );

	cvReleaseImage( &im3 );

	int cone_bytes=8*((((T/6)/2+7)>>3)-1);
	
	IplImage* cone = cvCreateImage( cvSize(cone_bytes*8,1), IPL_DEPTH_32F, 1 );
	
	coneFP( bands, cone, T, 6, 1 );
	cvReleaseMat( &bands );	


	for( i=0;i<(cone_bytes*8);i++)
	{
		if(select_bits[i]==1)
		{			
			double gidbuf=cvGetReal2D( cone, 0, i);
			if( gidbuf >0 )
				bits[sig_head>>3]|=1<<(7-(sig_head&0x7));
			sig_head++;
			
		}
	}
	cvReleaseImage( &cone );

	IplImage* circular = cvCreateImage( cvSize(1*136,1), IPL_DEPTH_32F, 1 );
	coneFP( circles, circular, 195, 1, 1 );
	cvReleaseMat( &circles );
	for( i=0;i<48;i++)
	{
		if(select_bits[cone_bytes*8+i]==1)
		{			
			double gidbuf=cvGetReal2D( circular, 0, i);
			if( gidbuf >0 )
				bits[sig_head>>3]|=1<<(7-(sig_head&0x7));
			sig_head++;
		}
	}	
	cvReleaseImage( &circular );


	int diametricfunc_cnt=NUM_DIAMETRIC_FUNCS;
	int grandIDsize=MIN(48, (int)(M));

	IplImage* grandID = cvCreateImage( cvSize(diametricfunc_cnt*grandIDsize,1), IPL_DEPTH_32F, 1 );	

	
	for(df=0;df<diametricfunc_cnt ;df++ )
	{		
		CvMat* circusVector = cvCreateMat( 1, T, CV_32F );

		diametrical( traceMatrix, circusVector, diametricfunc[df] );
		
		IplImage *circusVectorImage=cvGetImage( circusVector, cvCreateImageHeader(cvSize(T,1), IPL_DEPTH_32F,1) );
		
		IplImage* circusVectorFFT = cvCreateImage( cvSize(T,1), IPL_DEPTH_32F, 1 );
		cvDFT(circusVectorImage, circusVectorFFT, CV_DXT_FORWARD, 0);
		// HACK: OpenCV's memory management doesn't label an IplImage
		// constructed from a CvMat properly, so the data pointer will
		// be attempted to be deallocated incorrectly. We kludge these
		// members here to avoid this during deletion of the image
		circusVectorImage->imageDataOrigin = circusVectorImage->imageData = NULL;
		cvReleaseImage( &circusVectorImage );
		
		cvReleaseMat( &circusVector );	

		IplImage* magnitude = cvCreateImage( cvSize((T+1)/2,1), IPL_DEPTH_32F, 1 );		
		vilModulus(circusVectorFFT, magnitude);
		cvReleaseImage( &circusVectorFFT );
		
		// Take the log of the magnitude
		cvLog(magnitude,magnitude);	
		

		IplImage* magnitude2 = cvCreateImage( cvSize((T+1)/2,1), IPL_DEPTH_32F, 1 );
		vilGrad(magnitude,magnitude2);
		cvReleaseImage( &magnitude );

		double avg=mean(magnitude2, 48, 1);
		
		cvSubS( magnitude2, cvScalar(avg,0,0,0), magnitude2, NULL );
		
		cvThreshold( magnitude2, magnitude2, 0, 1, CV_THRESH_BINARY);
		
		// Pack up the identifier
		for( int gcnt=0; gcnt<(grandIDsize) && gcnt<((T+1)/2); ++gcnt )
		{
			if( select_bits[(cone_bytes*8)+(grandIDsize*df)+gcnt+48]==1 )
			{
				double gidbuf=cvGetReal2D( magnitude2, 0, gcnt+1);
				if( gidbuf > 0 )
					bits[sig_head>>3]|=1<<(7-(sig_head&0x7));
				sig_head++;
			}
		}	
		cvReleaseImage( &magnitude2 );
	}

	cvReleaseImage( &grandID );
	cvReleaseMat( &traceMatrix );

	return 0;
}

unsigned long ImageSignatureExtractionTool::ExtractARCSignature( ImageSignatureGlobalBits &bits )
{

	// rgb is vyu in MomVop!!
	int sig_head=0;
	MomImage *imbuf_r, *imbuf_g, *imbuf_b;
	imbuf_r=m_ImageMedia->v_chan;
	imbuf_g=m_ImageMedia->y_chan;
	imbuf_b=m_ImageMedia->u_chan;
	// OpenCV stores colour images as BGR
	IplImage *im_original=cvCreateImage( cvSize(imbuf_r->x,imbuf_r->y),IPL_DEPTH_8U, 3);
	MomImageToIplImage( imbuf_r, imbuf_g, imbuf_b, im_original );
	
	IplImage *im = cvCreateImage( cvSize( imbuf_r->x, imbuf_r->y), IPL_DEPTH_8U, 1);
	
	cvCvtColor( im_original, im, CV_BGR2GRAY);
	
	cvReleaseImage( &im_original );
	int tracefunc=IF1;
	int diametricfunc[NUM_DIAMETRIC_FUNCS]={IF3, IF6, IFS8, IFS16, IFS32, IFS64, IFS128, IFX8, IFX16, IFX32, IFX64, IFX128};	
	
	int M=192;
	int T=600;
	int R=384;
	int df;
	int i;
	unsigned char select_bits[1008]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,0,1,1,0,0,1,1,1,0,1,0,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,0,1,1,0,0,0,0,1,1,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,0,1,1,1,0,1,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
0,0,0,0,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0};	
	
	if (!im)
		return false;
	
	CvSize size = cvGetSize(im);	
	
	float scale;
	scale=M/MAX((float)size.height, (float)size.width);
	
	int imwidth=M;
	int imheight=M;

	IplImage* im2;
	im2 = cvCreateImage( cvSize(imwidth,imheight), IPL_DEPTH_8U, 1 ); 

	
	if(scale<1)
	{
		CvMat *filter=lanczos( scale );
		cvFilter2D( im, im, filter );
		cvReleaseMat( &filter );
		cvResize( im, im2, CV_INTER_LINEAR );

	}
	else
	{
		cvResize( im, im2, CV_INTER_CUBIC );
	}
	cvReleaseImage(&im);

	subCircle( im2, im2 );
	cvSmooth( im2, im2, CV_GAUSSIAN, 3, 3 );

	
	// Convert the image to 32 bit floating point numbers
	IplImage* im3 = cvCreateImage( cvSize(M,M), IPL_DEPTH_32F, 1 );
	cvConvert(im2, im3 );	

	cvReleaseImage( &im2 );
	// Create the output (destination) image
	CvMat* traceMatrix = cvCreateMat( R, T, CV_32F );
	// Create a buffer
	CvMat matbuf;
	// Create the source image as a matrix
	CvMat* matim;
	matim=cvGetMat(im3, &matbuf, NULL, 0);
	
	CvMat* bands = cvCreateMat( 8, T, CV_32F );
	cvSetZero(bands);
	CvMat* circles = cvCreateMat( 1, 195, CV_32F );

	cvSetZero(circles);

	trace( matim, traceMatrix, bands, circles, tracefunc, 1 );
	

	cvReleaseImage( &im3 );

	int cone_bytes=8*((((T/6)/2+7)>>3)-1);
	
	IplImage* cone = cvCreateImage( cvSize(cone_bytes*8,1), IPL_DEPTH_32F, 1 );
	
	coneFP( bands, cone, T, 6, 1 );
	cvReleaseMat( &bands );	

	for( i=0;i<(cone_bytes*8);i++)
	{
		if(select_bits[i]==1)
		{			
			double gidbuf=cvGetReal2D( cone, 0, i);
			if( gidbuf >0 )
				bits[sig_head>>3]|=1<<(7-(sig_head&0x7));
			sig_head++;
		}
	}
	cvReleaseImage( &cone );

	IplImage* circular = cvCreateImage( cvSize(1*136,1), IPL_DEPTH_32F, 1 );
	coneFP( circles, circular, 195, 1, 1 );
	cvReleaseMat( &circles );
	for( i=0;i<48;i++)
	{
		if(select_bits[cone_bytes*8+i]==1)
		{			
			double gidbuf=cvGetReal2D( circular, 0, i);
			if( gidbuf >0 )
				bits[sig_head>>3]|=1<<(7-(sig_head&0x7));
			sig_head++;
		}
	}	
	cvReleaseImage( &circular );


	int diametricfunc_cnt=NUM_DIAMETRIC_FUNCS;
	int grandIDsize=MIN(48, (int)(M));

	IplImage* grandID = cvCreateImage( cvSize(diametricfunc_cnt*grandIDsize,1), IPL_DEPTH_32F, 1 );	

	
	for(df=0;df<diametricfunc_cnt ;df++ )
	{		
		CvMat* circusVector = cvCreateMat( 1, T, CV_32F );

		diametrical( traceMatrix, circusVector, diametricfunc[df] );
		
		IplImage *circusVectorImage=cvGetImage( circusVector, cvCreateImageHeader(cvSize(T,1), IPL_DEPTH_32F,1) );
		
		IplImage* circusVectorFFT = cvCreateImage( cvSize(T,1), IPL_DEPTH_32F, 1 );
		cvDFT(circusVectorImage, circusVectorFFT, CV_DXT_FORWARD, 0);
		// HACK: OpenCV's memory management doesn't label an IplImage
		// constructed from a CvMat properly, so the data pointer will
		// be attempted to be deallocated incorrectly. We kludge these
		// members here to avoid this during deletion of the image
		circusVectorImage->imageDataOrigin = circusVectorImage->imageData = NULL;
		cvReleaseImage( &circusVectorImage );
		
		cvReleaseMat( &circusVector );	

		IplImage* magnitude = cvCreateImage( cvSize((T+1)/2,1), IPL_DEPTH_32F, 1 );		
		vilModulus(circusVectorFFT, magnitude);
		cvReleaseImage( &circusVectorFFT );
		
		// Take the log of the magnitude
		cvLog(magnitude,magnitude);	
		

		IplImage* magnitude2 = cvCreateImage( cvSize((T+1)/2,1), IPL_DEPTH_32F, 1 );
		vilGrad(magnitude,magnitude2);
		cvReleaseImage( &magnitude );

		double avg=mean(magnitude2, 48, 1);
		
		cvSubS( magnitude2, cvScalar(avg,0,0,0), magnitude2, NULL );
		
		cvThreshold( magnitude2, magnitude2, 0, 1, CV_THRESH_BINARY);
		
		// Pack up the identifier
		for( int gcnt=0; gcnt<(grandIDsize) && gcnt<((T+1)/2); ++gcnt )
		{
			if( select_bits[(cone_bytes*8)+(grandIDsize*df)+gcnt+48]==1 )
			{
				double gidbuf=cvGetReal2D( magnitude2, 0, gcnt+1);
				if( gidbuf > 0 )
					bits[sig_head>>3]|=1<<(7-(sig_head&0x7));
				sig_head++;
			}
		}	
		cvReleaseImage( &magnitude2 );
	}

	cvReleaseImage( &grandID );
	cvReleaseMat( &traceMatrix );

	return 0;
}

unsigned long ImageSignatureExtractionTool::ExtractFeatureSignature( ImageSignatureBits &bits )
{
	ImageSignatureRegionBits fbits;
	int sig_head=0;
	// rgb is vyu in MomVop!!
	MomImage *imbuf_r, *imbuf_g, *imbuf_b;
	imbuf_r=m_ImageMedia->v_chan;
	imbuf_g=m_ImageMedia->y_chan;
	imbuf_b=m_ImageMedia->u_chan;
	// OpenCV stores colour images as BGR
	IplImage *im_original=cvCreateImage( cvSize(imbuf_r->x,imbuf_r->y),IPL_DEPTH_8U, 3);
	MomImageToIplImage( imbuf_r, imbuf_g, imbuf_b, im_original );
	
	IplImage *im = cvCreateImage( cvSize( imbuf_r->x, imbuf_r->y), IPL_DEPTH_8U, 1);
	
	cvCvtColor( im_original, im, CV_BGR2GRAY);
	
	cvReleaseImage( &im_original );
	
	int M=256;
	int i;
	unsigned char select_bits[112]={1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	

	for( i=0; i<14; i++ )
		fbits[i]=0;
	
	if (!im)
		return false;
	
	CvSize size = cvGetSize(im);	
	
	float scale;
	scale=M/MIN((float)size.height, (float)size.width);
	
	int imwidth=(int)floor(scale*size.width+0.5);
	int imheight=(int)floor(scale*size.height+0.5);
	
	
	IplImage* im2;
	im2 = cvCreateImage( cvSize(imwidth,imheight), IPL_DEPTH_8U, 1 ); 
	
	if(scale<1)
	{
		CvMat *filter=lanczos( scale );
		cvFilter2D( im, im, filter );
		cvReleaseMat( &filter );
		cvResize( im, im2, CV_INTER_LINEAR );
	}
	else
	{
		cvResize( im, im2, CV_INTER_CUBIC );
	}
	cvReleaseImage(&im);
	
	cvSmooth( im2, im2, CV_GAUSSIAN, 3, 3 );
	
	
	// Convert the image to 32 bit floating point numbers
	IplImage* im3 = cvCreateImage( cvSize(imwidth,imheight), IPL_DEPTH_32F, 1 );

	cvConvert(im2, im3 );
	cvReleaseImage( &im2 );	

	CvMat *response=cvCreateMat(imheight, imwidth,CV_32FC1);

	IplImage* im_smooth = cvCreateImage( cvSize(imwidth,imheight), IPL_DEPTH_32F, 1 );
	CvMat *harris_response1=cvCreateMat(imheight, imwidth,CV_32FC1);
	CvMat *harris_response2=cvCreateMat(imheight, imwidth,CV_32FC1);	


	std::list<featurePoint> feature_points;
	double min, max;
	cvMinMaxLoc( response, &min, &max );
	double average=(double)cvAvg( response ).val[0];
	
	IplImage *direction=cvCreateImage(cvSize(imwidth,imheight),IPL_DEPTH_32F, 1);
	IplImage *direction_buf=cvCreateImage(cvSize(imwidth,imheight),IPL_DEPTH_32F, 1);
	cvZero(direction);
	IplImage *magnitude_buf=cvCreateImage(cvSize(imwidth,imheight),IPL_DEPTH_32F, 1);
	IplImage *scales=cvCreateImage(cvSize(imwidth,imheight),IPL_DEPTH_32F, 1);
	cvZero(scales);
	IplImage *magnitudes=cvCreateImage(cvSize(imwidth,imheight),IPL_DEPTH_32F, 1);
	cvZero(magnitudes);

	int border_size=cvRound(BASE_REGION_SIZE/2.0);

	for(int level=0;level<LEVELS;level++)
	{
		cvZero(magnitude_buf);

		cvSmooth( im3, im_smooth, CV_GAUSSIAN, 0, 0, KAPPA*pow(SCALE_FACTOR,level) );
		cvCornerHarris(im_smooth,response,5,-1);

		findFeaturesLaplacian( im_smooth, magnitude_buf, direction_buf, level );
		
		for(int j=border_size;j<(imheight-border_size);j++)
		{
			for(i=border_size;i<(imwidth-border_size);i++)
			{
				if( cvGet2D( magnitudes, j, i ).val[0] < cvGet2D( magnitude_buf, j, i ).val[0] && cvGet2D( response, j, i ).val[0] > 0.10)
				{
					cvSet2D(magnitudes,j,i,cvScalar(cvGet2D( magnitude_buf, j, i ).val[0],0,0,0));
					cvSet2D(direction,j,i,cvScalar(cvGet2D( direction_buf, j, i ).val[0],0,0,0));
					cvSet2D(scales,j,i,cvScalar(level,0,0,0));
				}				
			}
			
		}
		
	}
	cvReleaseImage(&im_smooth);
	cvReleaseImage(&magnitude_buf);
	cvReleaseImage(&direction_buf);
	
	for(i=border_size;i<(imwidth-border_size);i++)
	{
		for(int j=border_size;j<(imheight-border_size);j++)
		{
			int scale_val=cvGet2D( scales, j, i ).val[0];
			if(cvGet2D( magnitudes, j, i ).val[0]>0.0)
			{			
				featurePoint buf(i,j,cvGet2D( scales, j, i ).val[0],(int)cvFloor(cvGet2D( direction, j, i ).val[0]/22.50), cvGet2D( magnitudes, j, i ).val[0]);
				feature_points.push_back(buf);				
			}
		}
		
	}
	cvReleaseImage(&magnitudes);
	cvReleaseImage(&scales);
	cvReleaseImage(&direction);

	cvReleaseMat(&harris_response1);
	cvReleaseMat(&harris_response2);
	cvReleaseMat(&response);
	
	feature_points.sort(std::greater<featurePoint>());
	std::list<featurePoint>::iterator p=feature_points.begin();
	std::list<featurePoint> feature_points_accepted;
	
	p=feature_points.begin();
	
	int cnt=80;
	int global_cnt=80;
	while(p!=feature_points.end() && cnt>0)
	{
		bool print=true;
		std::list<featurePoint>::iterator q=feature_points_accepted.begin();
		while(q!=feature_points_accepted.end())
		{
			if(p->distance(*q)<INTER_FEATURE_DISTANCE || p->near_border(imwidth,imheight))
			{
				print=false;
				break;
			}
			q++;
		}
		if(print)
		{
			feature_points_accepted.push_back(*p);
			CvMat *submat=cvCreateMat(33, 33,CV_32FC1);
			int scale_size=(int)cvRound(BASE_REGION_SIZE*pow(SCALE_FACTOR,p->get_scale()));

			CvMat *submat_buf=cvCreateMat(scale_size, scale_size,CV_32FC1);
			
			CvPoint2D32f corners=cvPoint2D32f( (double)p->get_x(), (double)p->get_y() );
			IplImage *im8=cvCreateImage(cvGetSize(im3),8,1);
			cvConvert(im3, im8);
			cvFindCornerSubPix( im8, &corners,1, cvSize(3,3), cvSize(-1,-1), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ) );
			
			cvGetRectSubPix( im3, submat_buf, corners );
			cvResize( submat_buf, submat, CV_INTER_CUBIC );
			subCircleFT( submat, submat );
												
			cvReleaseImage( &im8 );
			
			for( i=0; i<8; i++ )
				fbits[i]=0;
			
			FindSignature(submat, fbits, select_bits);
			unsigned int addr=(11*(MAX_FEATURES-global_cnt));
			bits[addr]=(unsigned char)cvRound(255.0*corners.x/imwidth);
			bits[addr+1]=(unsigned char)cvRound(255.0*corners.y/imheight);
			bits[addr+2]=(unsigned char)p->get_angle();
			for( i=0; i<8; i++ )
			{
				bits[addr+3+i]=(unsigned char)fbits[i];
			}

			cnt--;
			global_cnt--;

			cvReleaseMat(&submat);
			cvReleaseMat(&submat_buf);
		}				
		p++;
	}

	while(cnt>0)
	{
		unsigned int addr=(11*(MAX_FEATURES-global_cnt));
		bits[addr]=(unsigned char)255;
		bits[addr+1]=(unsigned char)255;
		bits[addr+2]=(unsigned char)0;
		
		for( i=0; i<8; i++ )
			bits[addr+3+i]=0;
		cnt--;
		global_cnt--;
	}
	
	
	cvReleaseImage(&im3);
	
	feature_points.clear();
	feature_points_accepted.clear();

	return 0;
}


int findFeaturesLaplacian( IplImage *src, IplImage *dst, IplImage *direction, int scale )
{
  CvSize size = cvGetSize(src);

  CvMat kernelx;
  double a[3]={1.0,0.0,-1.0};
  double b[3]={3.0,10.0,3.0};  
  cvInitMatHeader( &kernelx, 1, 3, CV_64FC1, b );
  CvMat kernelx2;
  cvInitMatHeader( &kernelx2, 1, 3, CV_64FC1, a );
  IplImage* diff_x=cvCreateImage( size, IPL_DEPTH_32F, 1 );
  cvFilter2D( src, diff_x,&kernelx);
  cvFilter2D( diff_x, diff_x,&kernelx2);

  IplImage* diff_xx=cvCreateImage( size, IPL_DEPTH_32F, 1 );
  cvFilter2D( diff_x, diff_xx,&kernelx);
  cvFilter2D( diff_xx, diff_xx,&kernelx2);
  cvSmooth( diff_x, diff_x, CV_GAUSSIAN, 0, 0, pow(SCALE_FACTOR,scale) );
  cvSmooth( diff_xx, diff_xx, CV_GAUSSIAN, 0, 0, pow(SCALE_FACTOR,scale) );

  CvMat kernely;
  cvInitMatHeader( &kernely, 3, 1, CV_64FC1, a );
  CvMat kernely2;
  cvInitMatHeader( &kernely2, 3, 1, CV_64FC1, b );
  IplImage* diff_y=cvCreateImage( size, IPL_DEPTH_32F, 1 );
  cvFilter2D( src, diff_y,&kernely);
  cvFilter2D( diff_y, diff_y,&kernely2);

  IplImage* diff_yy=cvCreateImage( size, IPL_DEPTH_32F, 1 );
  cvFilter2D( diff_y, diff_yy,&kernely);
  cvFilter2D( diff_yy, diff_yy,&kernely2);
  cvSmooth( diff_y, diff_y, CV_GAUSSIAN, 0, 0, pow(SCALE_FACTOR,scale) );
  cvSmooth( diff_yy, diff_yy, CV_GAUSSIAN, 0, 0, pow(SCALE_FACTOR,scale) );

  int border_size=BASE_REGION_SIZE/2.0;

  for(int i=border_size;i<(size.width-border_size);i++)
  {
    for(int j=border_size;j<(size.height-border_size);j++)
    {
      double angle=cvFastArctan( (float)cvGet2D(diff_y,j,i).val[0], (float)cvGet2D(diff_x,j,i).val[0] );      
      cvSet2D(direction,j,i,cvScalar(angle,0,0,0));
    }
  }


  IplImage* diff=cvCreateImage( size, IPL_DEPTH_32F, 1 );
  cvAdd(diff_xx,diff_yy,diff);
  
  cvReleaseImage(&diff_x);
  cvReleaseImage(&diff_xx);
  cvReleaseImage(&diff_y);
  cvReleaseImage(&diff_yy);


  double sigmad=KAPPA*pow(SCALE_FACTOR,SIGMA_IS*scale);
  cvScale(diff, dst, sigmad*sigmad);
  cvReleaseImage(&diff);
  return 0;

}

void ImageSignatureExtractionTool::FindSignature( CvMat *im, ImageSignatureRegionBits bits, unsigned char *select_bits )
{
	int i;
	int T=100;
	int R=64;
	int tracefunc=IF1;
	int diametricfunc[NUM_DIAMETRIC_FUNCS_FT]={IF3, IF6, IFS8, IFS16, IFS32, IFX8, IFX16, IFX32};
	int df;
	CvMat* traceMatrix = cvCreateMat( R, T, CV_32F );
	CvMat* bands = cvCreateMat( 5, T, CV_32F );
	cvSetZero(bands);
	CvMat* circles = cvCreateMat( 1, 47, CV_32F );
	
	cvSetZero(circles);
	int cone_bytes=8;
	trace_FT( im, traceMatrix, bands, circles, tracefunc, 1 );

	IplImage* cone = cvCreateImage( cvSize(cone_bytes*5,1), IPL_DEPTH_32F, 1 );
	coneFPFT( bands, cone, T, 2, 1 );
	
	
	cvReleaseMat( &bands );	
	
	int sig_head_ft=0;
	for( i=0;i<(cone_bytes*5);i++)
	{
		if(select_bits[i]==1)
		{			
			double gidbuf=cvGetReal2D( cone, 0, i);
			if( gidbuf >0 )
			{
				bits[sig_head_ft>>3]|=1<<(7-(sig_head_ft&0x7));
			}
			sig_head_ft++;
		}
	}
	cvReleaseImage( &cone );
	
	
	IplImage* circular = cvCreateImage( cvSize(1*136,1), IPL_DEPTH_32F, 1 );
	coneFP( circles, circular, 47, 1, 1 );
	
	
	cvReleaseMat( &circles );
	for( i=0;i<8;i++)
	{
		if(select_bits[cone_bytes*5+i]==1)
		{			
			double gidbuf=cvGetReal2D( circular, 0, i);
			if( gidbuf >0 )
				bits[sig_head_ft>>3]|=1<<(7-(sig_head_ft&0x7));
			sig_head_ft++;
		}
	}	
	cvReleaseImage( &circular );
	
	
	int diametricfunc_cnt=NUM_DIAMETRIC_FUNCS_FT;
	int grandIDsize=8;
	
	IplImage* grandID = cvCreateImage( cvSize(diametricfunc_cnt*grandIDsize,1), IPL_DEPTH_32F, 1 );	
	
	
	for(df=0;df<diametricfunc_cnt ;df++ )
	{		
		CvMat* circusVector = cvCreateMat( 1, T, CV_32F );
		
		diametrical( traceMatrix, circusVector, diametricfunc[df] );
		
		IplImage *circusVectorImage=cvGetImage( circusVector, cvCreateImageHeader(cvSize(T,1), IPL_DEPTH_32F,1) );
		
		IplImage* circusVectorFFT = cvCreateImage( cvSize(T,1), IPL_DEPTH_32F, 1 );
		cvDFT(circusVectorImage, circusVectorFFT, CV_DXT_FORWARD, 0);
		// HACK: OpenCV's memory management doesn't label an IplImage
		// constructed from a CvMat properly, so the data pointer will
		// be attempted to be deallocated incorrectly. We kludge these
		// members here to avoid this during deletion of the image
		circusVectorImage->imageDataOrigin = circusVectorImage->imageData = NULL;
		cvReleaseImage( &circusVectorImage );
		
		cvReleaseMat( &circusVector );	
		
		IplImage* magnitude = cvCreateImage( cvSize((T+1)/2,1), IPL_DEPTH_32F, 1 );		
		vilModulus(circusVectorFFT, magnitude);
		cvReleaseImage( &circusVectorFFT );
		
		// Take the log of the magnitude
		cvLog(magnitude,magnitude);	
		
		
		IplImage* magnitude2 = cvCreateImage( cvSize((T+1)/2,1), IPL_DEPTH_32F, 1 );
		vilGrad(magnitude,magnitude2);
		cvReleaseImage( &magnitude );
		
		double avg=mean(magnitude2, 8, 1);
		
		cvSubS( magnitude2, cvScalar(avg,0,0,0), magnitude2, NULL );
		
		IplImage* magnitude3 = cvCreateImage( cvSize((T+1)/2,1), IPL_DEPTH_32F, 1 );
		cvThreshold( magnitude2, magnitude3, 0, 1, CV_THRESH_BINARY);
		cvReleaseImage( &magnitude2 );
		// Pack up the identifier
		for( int gcnt=0; gcnt<(grandIDsize) && gcnt<((T+1)/2); ++gcnt )
		{
			if( select_bits[(cone_bytes*5)+(grandIDsize*df)+gcnt+8]==1 )
			{
				double gidbuf=cvGetReal2D( magnitude3, 0, gcnt+1);
				if( gidbuf > 0 )
					bits[sig_head_ft>>3]|=1<<(7-(sig_head_ft&0x7));
				sig_head_ft++;
			}
		}	
		cvReleaseImage( &magnitude3 );
		
	}
	
	cvReleaseImage( &grandID );
	cvReleaseMat( &traceMatrix );
}

//----------------------------------------------------------------------------
void ImageSignatureExtractionTool::MomImageToIplImage(MomImage *im_r, MomImage *im_g, MomImage *im_b, IplImage *dst)
{
	for(int r=0; r< im_r->y; r++ )
	{
		for(int c=0;c<im_r->x;c++)
		{
			UChar pix_r = *xydata(im_r, c, r);
			UChar pix_g = *xydata(im_g, c, r);
			UChar pix_b = *xydata(im_b, c, r);
			cvSet2D( dst, r, c, cvScalar(pix_b, pix_g, pix_r, 0 ));
		}
	}
}
//----------------------------------------------------------------------------
// The Trace Transform
void ImageSignatureExtractionTool::trace( CvMat *src, CvMat *dst, CvMat *center_strip, CvMat *circles, int func, int type)
{
	CvSize size = cvGetSize(src);
	int N=size.width, M=size.height;
	int T=600;
	int R=384;
	double xmin, thetamin=0.0, rhomin, dx, dtheta, drho, fabsinvcostheta, fabsinvdxcostheta, costheta, sintheta, alpha, beta, rhooffset, invalpha, invcostheta, invsintheta, invdxcostheta, invdxsintheta; 
	double halfpi, pos;
	int mmin, mmax, nmin, nmax, loc, n, m, n_step, t, address;
	unsigned long sum;
	double halfN=0.5*N, halfM=0.5*M;
	
	halfpi=CV_PI/(double)2.0;
	
	dx=1.0;
	xmin=(double)(-((M-1)/(double)2.0)*dx);
	drho=(double)(dx/sqrt((double)2.0));
	rhomin=(double)(-((R-1)/(double)2.0)*drho);
	dtheta=(double)(M_PI/(double)T);
	
	cvZero(dst);

	register double *theta=new double[T];
	
	for(t=0; t<T; ++t)
	{
		theta[t]=thetamin+(dtheta*t);		
	}

	register double *rho=new double[R];
	for( int r=0; r<R; ++r) 
	{
		rho[r]=rhomin+(r*drho);	
	}
	
	register unsigned char *im=new unsigned char[N*M];
	for( n=0; n<N; ++n)
	{
		for( m=0; m<M; ++m)
		{
			im[(n*M)+m ]=(unsigned char)cvmGet( src, n, m );
		}
	}
	
	double onesqrttwo=(1.0/sqrt((double)2.0));
	double buf0=-1.0;
	double buf1=-1.0, buf2=-1.0, buf3=-1.0, buf4=-1.0, buf5=-1.0, buf6=-1.0, buf7=-1.0, buf8=-1.0, buf9=-1.0;
	
	for(t=0; t<T; t++ )
	{
		register double th = theta[t];
		costheta=cos(th);
		invcostheta=(double)(1.0/costheta);
		invdxcostheta=(double)(1.0/(dx*costheta));
		sintheta=sin(th);
		invsintheta=(double)(1.0/sintheta);
		invdxsintheta=(double)(1.0/(dx*sintheta));
		rhooffset=xmin*(costheta+sintheta);
		fabsinvcostheta=fabs(invcostheta);
		fabsinvdxcostheta=fabs(invdxcostheta);
		
		if( sintheta>onesqrttwo )
		{
			alpha=-costheta*invsintheta;
			invalpha=1.0/alpha;
			int alphai=(int)(alpha*2097151);
			
			for( int r=0; r<R; r++ )
			{				
				beta = (rho[r]-rhooffset)*invdxsintheta;
				double frhor=fabs(rho[r]);
				
				if(alpha>EPSILON)
				{
					mmin=MAX(0, 1+(int)((EPSILON-beta)*invalpha));
					mmax=MIN(M, 1+(int)floor((N-beta-1.0-EPSILON)*invalpha));
					
				}
				else if(alpha<-EPSILON)
				{
					mmin=MAX(0, 1+(int)((N-beta-1.0-EPSILON)*invalpha));
					mmax=MIN(M,1+(int)((EPSILON-beta)*invalpha));
				}
				else	
				{
					if(beta>1.0 && beta <(M-2.0))	
					{
						mmin=0;			
						mmax=M;
					}	
					else
					{
						mmin=0;
						mmax=-1;
					}
				}
				sum=0;
				pos=alpha*mmin+beta;
				int posi=(int)(pos*2097151);
				int circle_bit=(frhor<0.8 );
				double dmin=0.0;
				if(circle_bit)
				{				
					dmin=((mmin-halfN)+(rho[r]*costheta))*(invsintheta);
					for( m=mmin; m<mmax && pos>0.0; ++m )
					{					
						loc=(posi>>21);					
						int a=(posi)&(2097151);
						int b=2097152-a;
						address=(loc*M)+m;
						int val=(b*im[address])+(a*im[address+M]);
						sum+=(val)>>21; 
						double tempcs=0.0;
						int dmin_shift=(int)cvRound(dmin+97.0);
						if(dmin_shift>=0 && dmin_shift<=194)
						{
							tempcs=cvmGet(circles, 0, dmin_shift);
							tempcs+=(val>>21)*invsintheta;
							cvmSet( circles, 0, dmin_shift, tempcs );
						}
						if(fabs(dmin)>0.0 && fabs(dmin)<40.0)
						{							  
						  tempcs=cvmGet(center_strip, 5, t);
						  tempcs+=(val>>21)*invsintheta;
						  cvmSet( center_strip, 5, t, tempcs );
						}
						else if(fabs(dmin)>40.0 && fabs(dmin)<80.0)
						{							  
						  tempcs=cvmGet(center_strip, 6, t);
						  tempcs+=(val>>21)*invsintheta;
						  cvmSet( center_strip, 6, t, tempcs );
						}
						else if(fabs(dmin)>80.0 && fabs(dmin)<120.0)
						{
						  tempcs=cvmGet(center_strip, 7, t);
						  tempcs+=(val>>21)*invsintheta;
						  cvmSet( center_strip, 7, t, tempcs );
						}
						dmin+=invsintheta*dx;
						posi+=alphai;
					}
				}
				else
				{
					for( m=mmin; m<mmax && pos>0.0; ++m )
					{
						loc=(posi>>21);	
						int a=(posi)&(2097151);
						int b=2097152-a;
						address=(loc*M)+m;
						int val=(b*im[address])+(a*im[address+M]);
						sum+=(val)>>21; 
						posi+=alphai;
					}
				}
				if( frhor>=CONE_STRIP_LIMITS8 && frhor<CONE_STRIP_LIMITS9)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip,4,t);
						tempcs+=invsintheta*sum;
						cvmSet( center_strip, 4, t, tempcs );
					}
					else
					{
						if( buf8<0 )
						{
							buf8=invsintheta*sum;
						}
						else
						{
							double tempcs=cvmGet(center_strip, 4, t);
							tempcs+=fabs((invsintheta*sum)-buf8);
							cvmSet(center_strip, 4, t, tempcs );
							buf8=invsintheta*sum;
						}
					}
				}
				else if( frhor>=CONE_STRIP_LIMITS6 && frhor<CONE_STRIP_LIMITS7)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip,3,t);
						tempcs+=invsintheta*sum;
						cvmSet( center_strip, 3, t, tempcs );
					}
					else
					{
						if( buf6<0 )
						{
							buf6=invsintheta*sum;
						}
						else
						{
							double tempcs=cvmGet(center_strip, 3, t);
							tempcs+=fabs((invsintheta*sum)-buf6);
							cvmSet(center_strip, 3, t, tempcs );
							buf6=invsintheta*sum;
						}
					}
				}
				else if( frhor>=CONE_STRIP_LIMITS4 && frhor<CONE_STRIP_LIMITS5)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip,2,t);
						tempcs+=invsintheta*sum;
						cvmSet( center_strip, 2, t, tempcs );
					}
					else
					{
						if( buf4 <0 )
						{
							buf4=invsintheta*sum;
						}
						else
						{
							double tempcs=cvmGet(center_strip, 2, t);
							tempcs+=fabs((invsintheta*sum)-buf4);
							cvmSet(center_strip, 2, t, tempcs );
							buf4=invsintheta*sum;
						}
					}
				}
				else if(frhor>=CONE_STRIP_LIMITS2 && frhor<CONE_STRIP_LIMITS3)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip,1,t);
						tempcs+=invsintheta*sum;
						cvmSet( center_strip, 1, t, tempcs );
					}
					else
					{
						if( buf2 < 0 )
						{
							buf2=invsintheta*sum;
						}
						else
						{
							double tempcs=cvmGet(center_strip,1,t);
							tempcs+=fabs((invsintheta*sum)-buf2);
							cvmSet( center_strip, 1, t, tempcs );
							buf2=invsintheta*sum;
						}
					}
				}
				else if(frhor<CONE_STRIP_LIMITS)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip,0,t);
						tempcs+=invsintheta*sum;
						cvmSet( center_strip, 0, t, tempcs );
					}
					else
					{
						if( buf0 < 0)
						{
							buf0=invsintheta*sum;
						}
						else
						{
							double tempcs=cvmGet(center_strip,0,t);
							tempcs+=fabs((invsintheta*sum)-buf0);
							cvmSet( center_strip, 0, t, tempcs );
							buf0=invsintheta*sum;
						}
					}
				}
				cvmSet( dst, r, t, (float)invsintheta*sum );
			}
		}
		else 
		{			
			alpha=-sintheta*invcostheta;
			invalpha=1.0/alpha;
			int alphai=(int)(alpha*2097151);
			
			for( int r=0;r<(R);r++)
			{				
				beta=(rho[r]-rhooffset)*invdxcostheta;
				double frhor=fabs(rho[r]);
				
				if(alpha>EPSILON)
				{
					nmin=MAX(0, 1+(int)((EPSILON-beta)*invalpha));
					nmax=MIN(N, 1+(int)((M-beta-1.0-EPSILON)*invalpha));
				}
				else if(alpha<-EPSILON)
				{
					nmin=MAX(0, 1+(int)((M-beta-1.0-EPSILON)*invalpha));
					nmax=MIN(N,1+(int)((EPSILON-beta)*invalpha));
				}
				else
				{				
					
					if(beta>1.0 && beta<(N-2.0))	
					{
						nmin=0;	
						nmax=N;
					}	
					else
					{
						nmin=0;
						nmax=-1; 
					}
				}
				
				sum=0;
				
				pos=alpha*nmin+beta;
				int posi=(int)(pos*2097151);
				n_step=N*nmin;
				int circle_bit=(frhor<0.8 );
				double dmin=0.0;
				if(circle_bit )
				{				
					dmin=((nmin-halfM)+(rho[r]*sintheta))*(-invcostheta);
					if(circle_bit)
					{
						for( n=nmin; n<nmax && pos>0.0; ++n )
						{
							loc=((posi)>>21);
							
							int a=posi&(2097151);
							int b=2097152-a;
							address=loc+n_step;			
		
							int val=(b*im[address])+(a*im[address+1]);
							sum+=(val)>>21; 
							double tempcs=0.0;
							int dmin_shift=(int)cvRound(dmin+97.0);					
							if(dmin_shift>=0 && dmin_shift<=194)
							{
								tempcs=cvmGet(circles, 0, dmin_shift);
								if(th<1.4)						
									tempcs+=(val>>21)*invcostheta;
								else
									tempcs-=(val>>21)*invcostheta;
								cvmSet( circles, 0, dmin_shift, tempcs );	
							}
							if(fabs(dmin)>0.0 && fabs(dmin)<40.0)
							{
							  tempcs=cvmGet(center_strip, 5, t);
							  if(th<1.4)
							    tempcs+=(val>>21)*invcostheta;
							  else
							    tempcs-=(val>>21)*invcostheta;
							  cvmSet( center_strip, 5, t, tempcs );
							}
							if(fabs(dmin)>40.0 && fabs(dmin)<80.0)
							{
							  tempcs=cvmGet(center_strip, 6, t);
							  if(th<1.4)
							    tempcs+=(val>>21)*invcostheta;
							  else
							    tempcs-=(val>>21)*invcostheta;
							  cvmSet( center_strip, 6, t, tempcs );
							}
							if(fabs(dmin)>80.0 && fabs(dmin)<120.0)
							{							 
							  tempcs=cvmGet(center_strip, 7, t);
							  if(th<1.4)
							    tempcs+=(val>>21)*invcostheta;
							  else
							    tempcs-=(val>>21)*invcostheta;
							  cvmSet( center_strip, 7, t, tempcs );
							}
							dmin-=invcostheta*dx;						
							posi+=alphai;
							n_step+=N;
						}
					}
				}
				else
				{
					for( n=nmin; n<nmax && pos>0.0; ++n )
					{
						loc=((posi)>>21);
						
						int a=posi&(2097151);
						int b=2097152-a;
						address=loc+n_step;			
						int val=(b*im[address])+(a*im[address+1]);
						sum+=(val)>>21;
						posi+=alphai;
						n_step+=N;
					}
				}
				if(frhor>=CONE_STRIP_LIMITS8 && frhor<CONE_STRIP_LIMITS9)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip, 4, t);
						tempcs+=sum*fabsinvdxcostheta;
						cvmSet( center_strip, 4, t, tempcs );
					}
					else
					{
						if( buf9 < 0)
						{
							buf9=sum*fabsinvdxcostheta;
						}
						else
						{
							double tempcs=cvmGet(center_strip,4,t);
							tempcs+=fabs((sum*fabsinvdxcostheta)-buf9);
							cvmSet( center_strip, 4, t, tempcs );
							buf9=sum*fabsinvdxcostheta;
						}
					}
				}
				else if(frhor>=CONE_STRIP_LIMITS6 && frhor<CONE_STRIP_LIMITS7)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip, 3, t);
						tempcs+=sum*fabsinvdxcostheta;
						cvmSet( center_strip, 3, t, tempcs );
					}
					else
					{
						if( buf7 < 0)
						{
							buf7=sum*fabsinvdxcostheta;
						}
						else
						{
							double tempcs=cvmGet(center_strip,3,t);
							tempcs+=fabs((sum*fabsinvdxcostheta)-buf7);
							cvmSet( center_strip, 3, t, tempcs );
							buf7=sum*fabsinvdxcostheta;
						}
					}
				}
				else if(frhor>=CONE_STRIP_LIMITS4 && frhor<CONE_STRIP_LIMITS5)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip, 2, t);
						tempcs+=sum*fabsinvdxcostheta;
						cvmSet( center_strip, 2, t, tempcs );
					}
					else
					{
						if( buf5 < 0)
						{
							buf5=sum*fabsinvdxcostheta;
						}
						else
						{
							double tempcs=cvmGet(center_strip,2,t);
							tempcs+=fabs((sum*fabsinvdxcostheta)-buf5);
							cvmSet( center_strip, 2, t, tempcs );
							buf5=sum*fabsinvdxcostheta;
						}
					}
				}
				else if(frhor>=CONE_STRIP_LIMITS2 && frhor< CONE_STRIP_LIMITS3)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip,1,t);
						tempcs+=sum*fabsinvdxcostheta;
						cvmSet( center_strip, 1, t, tempcs );
					}
					else
					{
						if( buf3 < 0)
						{
							buf3=sum*fabsinvdxcostheta;
						}
						else
						{
							double tempcs=cvmGet(center_strip,1,t);
							tempcs+=fabs((sum*fabsinvdxcostheta)-buf3);
							cvmSet( center_strip, 1, t, tempcs );
							buf3=sum*fabsinvdxcostheta;
						}
					}
				}
				else if(frhor<CONE_STRIP_LIMITS)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip,0,t);
						tempcs+=sum*fabsinvdxcostheta;
						cvmSet( center_strip, 0, t, tempcs );
					}
					else
					{
						if( buf1 < 0)
						{
							buf1=sum*fabsinvdxcostheta;
						}
						else
						{
							double tempcs=cvmGet(center_strip,0,t);
							tempcs+=fabs((sum*fabsinvdxcostheta)-buf1);
							cvmSet( center_strip, 0, t, tempcs );
							buf1=sum*fabsinvdxcostheta;
						}
					}
				}
				cvmSet( dst, r, t, (float)sum*fabsinvdxcostheta );
			}
		}
	}	
	delete [] im;
	delete [] theta;
	delete [] rho;
}

//----------------------------------------------------------------------------
// The Trace Transform
void ImageSignatureExtractionTool::trace_FT( CvMat *src, CvMat *dst, CvMat *center_strip, CvMat *circles, int func, int type)
{
	CvSize size = cvGetSize(src);
	int N=size.width, M=size.height;
	register unsigned char *im=new unsigned char[N*M];
	for( int nn=0; nn<N; nn++)
	{
		for( int m=0; m<M; m++)
		{
			im[(nn*M)+m ]=(unsigned char)cvmGet( src, nn, m );
		}
	}
	int T=100;
	int R=64;
	double xmin, thetamin=0.0, rhomin, dx, dtheta, drho, fabsinvcostheta, fabsinvdxcostheta, costheta, sintheta, alpha, beta, rhooffset, invalpha, invcostheta, invsintheta, invdxcostheta, invdxsintheta; 
	double halfpi, pos;
	int mmin, mmax, nmin, nmax, loc, n, m, n_step, t, address;
	unsigned long sum;
	double halfN=0.5*N, halfM=0.5*M;
	
	halfpi=CV_PI/(double)2.0;
	
	dx=1.0;
	xmin=(double)(-((M-1)/(double)2.0)*dx);
	drho=(double)(dx/sqrt((double)2.0));
	rhomin=(double)(-((R-1)/(double)2.0)*drho);
	dtheta=(double)(M_PI/(double)T);
	
	cvZero(dst);
	cvZero(circles);
	
	register double *theta=new double[T];
	
	for(t=0; t<T; ++t)
	{
		theta[t]=thetamin+(dtheta*t);		
	}
	
	register double *rho=new double[R];
	for( int r=0; r<R; ++r) 
	{
		rho[r]=rhomin+(r*drho);	
	}
	
	
	
	double onesqrttwo=(1.0/sqrt((double)2.0));
	double buf0=-1.0;
	double buf1=-1.0, buf2=-1.0, buf3=-1.0, buf4=-1.0, buf5=-1.0, buf6=-1.0, buf7=-1.0, buf8=-1.0, buf9=-1.0;
	
	for(t=0; t<T; t++ )
	{
		register double th = theta[t];
		costheta=cos(th);
		invcostheta=(double)(1.0/costheta);
		invdxcostheta=(double)(1.0/(dx*costheta));
		sintheta=sin(th);
		invsintheta=(double)(1.0/sintheta);
		invdxsintheta=(double)(1.0/(dx*sintheta));
		rhooffset=xmin*(costheta+sintheta);
		fabsinvcostheta=fabs(invcostheta);
		fabsinvdxcostheta=fabs(invdxcostheta);
		
		if( sintheta>onesqrttwo )
		{
			alpha=-costheta*invsintheta;
			invalpha=1.0/alpha;
			int alphai=(int)(alpha*2097151);
			
			for( int r=0; r<R; r++ )
			{				
				beta = (rho[r]-rhooffset)*invdxsintheta;
				double frhor=fabs(rho[r]);
				
				if(alpha>EPSILON)
				{
					mmin=MAX(0, 1+(int)((EPSILON-beta)*invalpha));
					mmax=MIN(M, 1+(int)floor((N-beta-1.0-EPSILON)*invalpha));
					
				}
				else if(alpha<-EPSILON)
				{
					mmin=MAX(0, 1+(int)((N-beta-1.0-EPSILON)*invalpha));
					mmax=MIN(M,1+(int)((EPSILON-beta)*invalpha));
				}
				else	
				{
					if(beta>1.0 && beta <(M-2.0))	
					{
						mmin=0;			
						mmax=M;
					}	
					else
					{
						mmin=0;
						mmax=-1;
					}
				}
				sum=0;
				pos=alpha*mmin+beta;
				int posi=(int)(pos*2097151);
				int circle_bit=(frhor<0.8 );
				double dmin=0.0;
				if(circle_bit)
				{				
					dmin=((mmin-halfN)+(rho[r]*costheta))*(invsintheta);
					for( m=mmin; m<mmax && pos>0.0; ++m )
					{					
						loc=(posi>>21);					
						int a=(posi)&(2097151);
						int b=2097152-a;
						address=(loc*M)+m;
						if((address<0) || ((address+M)>1088))
						{
							cout << "*****ERROR*****" << endl;
						}
						int val=(b*im[address])+(a*im[address+M]);
						sum+=(val)>>21; 									
						double tempcs=cvmGet(circles, 0, (int)cvRound(dmin+23.0));
						tempcs+=((val>>21)*invsintheta);
						cvmSet( circles, 0, (int)cvRound(dmin+23.0), tempcs );
						if(fabs(dmin)>0.0 && fabs(dmin)<15.0)
						{							  
							tempcs=cvmGet(center_strip, 3, t);
							tempcs+=(val>>21)*invsintheta;
							cvmSet( center_strip, 3, t, tempcs );
						}
						else if(fabs(dmin)>15.0 && fabs(dmin)<30.0)
						{							  
							tempcs=cvmGet(center_strip, 4, t);
							tempcs+=(val>>21)*invsintheta;
							cvmSet( center_strip, 4, t, tempcs );
						}
						dmin+=invsintheta*dx;
						posi+=alphai;
					}
				}
				else
				{
					for( m=mmin; m<mmax && pos>0.0; ++m )
					{
						loc=(posi>>21);	
						int a=(posi)&(2097151);
						int b=2097152-a;
						address=(loc*M)+m;
						if((address<0) || ((address+M)>1088))
						{
							cout << "*****ERROR*****" << endl;
						}
						int val=(b*im[address])+(a*im[address+M]);
						sum+=(val)>>21; 
						posi+=alphai;
					}
				}

				if( frhor>=CONE_STRIP_LIMITS_FT4 && frhor<CONE_STRIP_LIMITS_FT5)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip,2,t);
						tempcs+=invsintheta*sum;
						cvmSet( center_strip, 2, t, tempcs );
					}
					else
					{
						if( buf4 <0 )
						{
							buf4=invsintheta*sum;
						}
						else
						{
							double tempcs=cvmGet(center_strip, 2, t);
							tempcs+=fabs((invsintheta*sum)-buf4);
							cvmSet(center_strip, 2, t, tempcs );
							buf4=invsintheta*sum;
						}
					}
				}
				else if(frhor>=CONE_STRIP_LIMITS_FT2 && frhor<CONE_STRIP_LIMITS_FT3)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip,1,t);
						tempcs+=invsintheta*sum;
						cvmSet( center_strip, 1, t, tempcs );
					}
					else
					{
						if( buf2 < 0 )
						{
							buf2=invsintheta*sum;
						}
						else
						{
							double tempcs=cvmGet(center_strip,1,t);
							tempcs+=fabs((invsintheta*sum)-buf2);
							cvmSet( center_strip, 1, t, tempcs );
							buf2=invsintheta*sum;
						}
					}
				}
				else if(frhor<CONE_STRIP_LIMITS_FT)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip,0,t);
						tempcs+=invsintheta*sum;
						cvmSet( center_strip, 0, t, tempcs );
					}
					else
					{
						if( buf0 < 0)
						{
							buf0=invsintheta*sum;
						}
						else
						{
							double tempcs=cvmGet(center_strip,0,t);
							tempcs+=fabs((invsintheta*sum)-buf0);
							cvmSet( center_strip, 0, t, tempcs );
							buf0=invsintheta*sum;
						}
					}
				}
				cvmSet( dst, r, t, (float)invsintheta*sum );
			}
		}
		else 
		{			
			alpha=-sintheta*invcostheta;
			invalpha=1.0/alpha;
			int alphai=(int)(alpha*2097151);
			
			for( int r=0;r<(R);r++)
			{
				beta=(rho[r]-rhooffset)*invdxcostheta;
				double frhor=fabs(rho[r]);
				
				if(alpha>EPSILON)
				{
					nmin=MAX(0, 1+(int)((EPSILON-beta)*invalpha));
					nmax=MIN(N, 1+(int)((M-beta-1.0-EPSILON)*invalpha));
				}
				else if(alpha<-EPSILON)
				{
					nmin=MAX(0, 1+(int)((M-beta-1.0-EPSILON)*invalpha));
					nmax=MIN(N,1+(int)((EPSILON-beta)*invalpha));
				}
				else
				{				
					
					if(beta>1.0 && beta<(N-2.0))	
					{
						nmin=0;	
						nmax=N;
					}	
					else
					{
						nmin=0;
						nmax=-1; 
					}
				}
				
				sum=0;
				
				pos=alpha*nmin+beta;
				int posi=(int)(pos*2097151);
				n_step=N*nmin;
				int circle_bit=(frhor<0.8 );
				double dmin=0.0;
				if(circle_bit )
				{				
					dmin=((nmin-halfM)+(rho[r]*sintheta))*(-invcostheta);
					if(circle_bit)
					{
						for( n=nmin; n<nmax && pos>0.0; ++n )
						{
							loc=((posi)>>21);
							
							int a=posi&(2097151);
							int b=2097152-a;
							address=loc+n_step;			
							if((address<0) || ((address+1)>1088))
							{
								cout << "*****ERROR*****" << endl;
							}
							int val=(b*im[address])+(a*im[address+1]);
							sum+=(val)>>21; 

							double tempcs=cvmGet(circles, 0, (int)cvRound(dmin+23.0));
							if(th<1.4)						
								tempcs+=((val>>21)*invcostheta);
							else
								tempcs-=((val>>21)*invcostheta);
							cvmSet( circles, 0, (int)cvRound(dmin+23.0), tempcs );
							if(fabs(dmin)>0.0 && fabs(dmin)<15.0)
							{
								tempcs=cvmGet(center_strip, 3, t);
								if(th<1.4)
									tempcs+=(val>>21)*invcostheta;
								else
									tempcs-=(val>>21)*invcostheta;
								cvmSet( center_strip, 3, t, tempcs );
							}
							if(fabs(dmin)>15.0 && fabs(dmin)<30.0)
							{
								tempcs=cvmGet(center_strip, 4, t);
								if(th<1.4)
									tempcs+=(val>>21)*invcostheta;
								else
									tempcs-=(val>>21)*invcostheta;
								cvmSet( center_strip, 4, t, tempcs );
							}							
							dmin-=invcostheta*dx;						
							posi+=alphai;
							n_step+=N;
						}
					}
				}
				else
				{
					for( n=nmin; n<nmax && pos>0.0; ++n )
					{
						loc=((posi)>>21);
						
						int a=posi&(2097151);
						int b=2097152-a;
						address=loc+n_step;	
						if((address<0) || ((address+1)>1088))
						{
							cout << "*****ERROR*****" << endl;
						}
						int val=(b*im[address])+(a*im[address+1]);
						sum+=(val)>>21;
						posi+=alphai;
						n_step+=N;
					}
				}				
				if(frhor>=CONE_STRIP_LIMITS_FT4 && frhor<CONE_STRIP_LIMITS_FT5)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip, 2, t);
						tempcs+=sum*fabsinvdxcostheta;
						cvmSet( center_strip, 2, t, tempcs );
					}
					else
					{
						if( buf5 < 0)
						{
							buf5=sum*fabsinvdxcostheta;
						}
						else
						{
							double tempcs=cvmGet(center_strip,2,t);
							tempcs+=fabs((sum*fabsinvdxcostheta)-buf5);
							cvmSet( center_strip, 2, t, tempcs );
							buf5=sum*fabsinvdxcostheta;
						}
					}
				}
				else if(frhor>=CONE_STRIP_LIMITS_FT2 && frhor< CONE_STRIP_LIMITS_FT3)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip,1,t);
						tempcs+=sum*fabsinvdxcostheta;
						cvmSet( center_strip, 1, t, tempcs );
					}
					else
					{
						if( buf3 < 0)
						{
							buf3=sum*fabsinvdxcostheta;
						}
						else
						{
							double tempcs=cvmGet(center_strip,1,t);
							tempcs+=fabs((sum*fabsinvdxcostheta)-buf3);
							cvmSet( center_strip, 1, t, tempcs );
							buf3=sum*fabsinvdxcostheta;
						}
					}
				}
				else if(frhor<CONE_STRIP_LIMITS_FT)
				{
					if( type==1)
					{
						double tempcs=cvmGet(center_strip,0,t);
						tempcs+=sum*fabsinvdxcostheta;
						cvmSet( center_strip, 0, t, tempcs );
					}
					else
					{
						if( buf1 < 0)
						{
							buf1=sum*fabsinvdxcostheta;
						}
						else
						{
							double tempcs=cvmGet(center_strip,0,t);
							tempcs+=fabs((sum*fabsinvdxcostheta)-buf1);
							cvmSet( center_strip, 0, t, tempcs );
							buf1=sum*fabsinvdxcostheta;
						}
					}
				}
				cvmSet( dst, r, t, (float)sum*fabsinvdxcostheta );
			}
		}
	}	
	delete [] im;
	delete [] theta;
	delete [] rho;
}

void ImageSignatureExtractionTool::imagetobit( IplImage *image, char *bitimage )
{
	CvSize size=cvGetSize(image);
	int k=0;
	for( int i=0; i<size.height; i++)
    {
		for( int j=0; j<size.width; j++)
		{
			if(cvGet2D( image, i, j ).val[0]>0 )
				bitimage[k>>3]|=1<<(7-(k&0x7));
			k++;
		}
    }
}

void ImageSignatureExtractionTool::diametrical( const CvMat* src, CvMat* dst, int func )
{
	CvSize size = cvGetSize(src);
	int N=size.width, M=size.height;
	double *vec=new double[1024], diametricval;
	
	for(int n=0;n<N;n++)
	{
		for(int m=0;m<M;m++)
		{					
			vec[m] = (double)cvmGet( src, m, n );	
		}
		diametricval = tracevector(vec, M, func);
		cvmSet( dst, 0, n, (float)diametricval );
	}
	delete [] vec;
}

double ImageSignatureExtractionTool::mean(IplImage* src, int ncols, int nrows)
{
	double val=0.0;
	for(int i=0;i<nrows;i++)
	{
		for(int j=0; j<ncols; j++)
		{
			val+=cvGet2D( src, i, j ).val[0];
		}
	}
	return val/(ncols*nrows);
}

double ImageSignatureExtractionTool::tracevector(double* vec, int vc, int traceflag)
{
	double retval;
	int i;	
	double val, val2;
	
	double sum=0.0, maxval;
	
	switch( traceflag )
	{
	case IF1:
		sum=0.0;
		for(i=0;i<vc;++i)
		{
			sum+=vec[i];
		}
		retval=sum;
		break;
	case IF3:
		for( i=0;i<(vc-1);++i)
		{
			sum+=fabs(vec[i]-vec[i+1]);
		}
		retval=sum;
		break;
	case IF6:
		maxval=-999.9;
		for( i=0;i<vc;++i)
		{
			if(vec[i]>maxval)
				maxval=vec[i];
		}
		retval=maxval;
		break;
	case IFS8:
		retval=0.0;
		if(vc>8)
		{
			val=0;
			for( int j=0; j<8; j++ )
			{
				val+=vec[j];
			}
			for( i=8; i<(vc-7); i=i+8 )
			{
				val2=0;
				for( int j=0; j<8; j++ )
				{
					val2+=vec[i+j];
				}					
				sum+=fabs(val-val2);
				val=val2;
			}
		}
		retval=sum;
		break;
	case IFS16:
		retval=0.0;
		if(vc>16)
		{
			val=0;
			for( int j=0; j<16; j++ )
			{
				val+=vec[j];
			}
			for( i=16; i<(vc-15); i=i+16 )
			{
				val2=0;
				for( int j=0; j<16; j++ )
				{
					val2+=vec[i+j];
				}					
				sum+=fabs(val-val2);
				val=val2;
			}
		}
		retval=sum;
		break;
	case IFS32:
		retval=0.0;
		if(vc>32)
		{
			val=0;
			for( int j=0; j<32; j++ )
			{
				val+=vec[j];
			}
			for( i=32; i<(vc-31); i=i+32 )
			{
				val2=0;
				for( int j=0; j<32; j++ )
				{
					val2+=vec[i+j];
				}					
				sum+=fabs(val-val2);
				val=val2;
			}
		}
		retval=sum;
		break;
	case IFS64:
		retval=0.0;
		if(vc>64)
		{
			val=0;
			for( int j=0; j<64; j++ )
			{
				val+=vec[j];
			}
			for( i=64; i<(vc-63); i=i+64 )
			{
				val2=0;
				for( int j=0; j<64; j++ )
				{
					val2+=vec[i+j];
				}					
				sum+=fabs(val-val2);
				val=val2;
			}
		}
		retval=sum;
		break;
	case IFS128:
		retval=0.0;
		if(vc>128)
		{
			val=0;
			for( int j=0; j<128; j++ )
			{
				val+=vec[j];
			}
			for( i=128; i<(vc-127); i=i+128 )
			{
				val2=0;
				for( int j=0; j<128; j++ )
				{
					val2+=vec[i+j];
				}					
				sum+=fabs(val-val2);
				val=val2;
			}
		}
		retval=sum;
		break;
	case IFX8:
		retval=0.0;			
		if(vc>8)
		{
			maxval=0;
			for( int j=0; j<8; j++ )
			{
				maxval+=vec[j];
			}
			for( i=8; i<(vc-7); i=i+8 )
			{
				val=0;
				for( int j=0; j<8; j++ )
				{
					val+=vec[i+j];
				}					
				if(val>maxval)
					maxval=val;
			}
		}
		retval=maxval;
		break;
	case IFX16:
		retval=0.0;
		if(vc>16)
		{
			maxval=0;
			for( int j=0; j<16; j++ )
			{
				maxval+=vec[j];
			}
			for( i=16; i<(vc-15); i=i+16 )
			{
				val=0;
				for( int j=0; j<16; j++ )
				{
					val+=vec[i+j];
				}					
				if(val>maxval)
					maxval=val;
			}
		}
		retval=maxval;
		break;
	case IFX32:
		retval=0.0;
		if(vc>32)
		{
			maxval=0;
			for( int j=0; j<32; j++ )
			{
				maxval+=vec[j];
			}
			for( i=32; i<(vc-31); i=i+32 )
			{
				val=0;
				for( int j=0; j<32; j++ )
				{
					val+=vec[i+j];
				}					
				if(val>maxval)
					maxval=val;
				
			}
		}
		retval=maxval;
		break;
	case IFX64:
		retval=0.0;
		if(vc>64)
		{
			maxval=0;
			for( int j=0; j<64; j++ )
			{
				maxval+=vec[j];
			}
			for( i=64; i<(vc-63); i=i+64 )
			{
				val=0;
				for( int j=0; j<64; j++ )
				{
					val+=vec[i+j];
				}					
				if(val>maxval)
					maxval=val;
			}
		}
		retval=maxval;
		break;
	case IFX128:
		retval=0.0;
		if(vc>128)
		{
			maxval=0;
			for( int j=0; j<128; j++ )
			{
				maxval+=vec[j];
			}
			for( i=128; i<(vc-127); i=i+128 )
			{
				val=0;
				for( int j=0; j<128; j++ )
				{
					val+=vec[i+j];
				}					
				if(val>maxval)
					maxval=val;
			}
		}
		retval=maxval;
		break;
	default:
		cout << "Warning: Undefined functional (" << traceflag << ")" << endl;
		retval=0.0;
	}
	return retval;
}

void ImageSignatureExtractionTool::vilModulus(IplImage* src, IplImage* dst)
{
	CvSize size=cvGetSize(src);
	char oddwidth=size.width%2;
	char oddheight=size.height%2;
	double im, re, val;
	
	re = cvGet2D( src, 0, 0 ).val[0];
	im=0;
	val=sqrt((im*im)+(re*re));
	cvSet2D( dst, 0, 0, cvScalar( val, 0, 0, 0 ) );
	
	for(int i=0;i<size.height-(1-oddheight);i++)
	{
		if((i%2)==1)
		{
			re=cvGet2D( src, i, 0 ).val[0]; 
			im=cvGet2D( src, i+1, 0 ).val[0];
			val=sqrt((im*im)+(re*re));
			cvSet2D( dst, (i+1)/2, 0, cvScalar( val, 0, 0, 0 ) );
		}
		for(int j=1;j<size.width-1;j++)
		{
			re = cvGet2D( src, i, j ).val[0];
			im = cvGet2D( src, i, j+1 ).val[0];
			
			val=sqrt((im*im)+(re*re));
			cvSet2D( dst, i, (j+1)/2, cvScalar( val, 0, 0, 0 ) );
			j++;			
		}
	}
}

CvMat *ImageSignatureExtractionTool::lanczos ( float scale )
{
	int i, j;
	int int_scale=(int)cvCeil(scale);
	int filter_size=int_scale*4 + 1;
	double *lookup=new double[filter_size];
	double buffer;
	
	// Create a lookup table for the 1D data
	for(i=0;i<filter_size;i++)
	{
		double val=(-(int_scale*2.0)+i)/(double)int_scale;
		
		lookup[i]=val*val;
		
	}
	
	// Now we can create the 2D filter from the 1D data
	// Note - It is assumed that the scaling is the same in both directions
	CvMat* filter = cvCreateMat(filter_size,filter_size,CV_32FC1);
	double filter_value;
	double normalising_value=0.0;
	
	for(i=0;i<filter_size;i++)
	{
		for(j=0;j<filter_size;j++)
		{
			double distance=sqrt(lookup[i]+lookup[j]);
			if(distance==0.0)
				filter_value=1.0;				
			else if(distance>2.0)
				filter_value=0.0;
			else
				filter_value=(2*sin(M_PI*distance)*(sin(M_PI*(distance/2.0)))) / ((M_PI*(distance))*(M_PI*(distance)));
			
			cvmSet(filter, i, j, (float)filter_value);
			normalising_value+=filter_value;			
		}		
	}
	
	// Normalise the filter
	for(i=0;i<filter_size;i++)
	{
		for(j=0;j<filter_size;j++)
		{
			buffer=cvmGet( filter, i, j );
			filter_value=buffer/normalising_value;
			cvmSet(filter, i, j, (float)filter_value);
		}
	}
	
	delete [] lookup;
	return filter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
const UUID& ImageSignatureExtractionTool::GetObjectID(void)
{
	return myID;
}

//----------------------------------------------------------------------------
const char *ImageSignatureExtractionTool::GetName(void)
{
	return myName;
}

//----------------------------------------------------------------------------
ImageSignatureExtractionInterfaceABC *ImageSignatureExtractionTool::GetInterface(void)
{
	return &m_Interface;
}

