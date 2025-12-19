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
// Applicable File Name:  ImageSignatureSearch.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <float.h>
#include <string.h>
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"					 // Naming Convention
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
// _isnan is defined in Windows library float.h and isnan is defined by GCC 
// in math.h. This ensures compatibility of the code between the two compilers.

#ifdef WIN32
int isnan(double x) { return _isnan(x); }
#endif // WIN32

//=============================================================================
using namespace XM;


const UUID ImageSignatureSearchInterface::myID = UUID("");
const char *ImageSignatureSearchInterface::myName = "Image Signature Matching Interface";

const UUID ImageSignatureSearchTool::myID = UUID("");
const char *ImageSignatureSearchTool::myName = "Image Signature Matching Tool";

const UUID ImageSignatureSearchInterfaceABC::myID = UUID();

//=============================================================================
ImageSignatureSearchInterfaceABC::ImageSignatureSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
ImageSignatureSearchInterface::ImageSignatureSearchInterface(ImageSignatureSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ImageSignatureSearchInterface::~ImageSignatureSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& ImageSignatureSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ImageSignatureSearchInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void ImageSignatureSearchInterface::destroy(void)
{
	delete m_SearchTool;
}

//----------------------------------------------------------------------------
int ImageSignatureSearchInterface::
SetRefDescriptorInterface(ImageSignatureDescriptorInterfaceABC
						  *RefImageSignatureDescriptorInterface)
						  
{
	return m_SearchTool->
		SetRefDescriptorInterface(RefImageSignatureDescriptorInterface);
}

//----------------------------------------------------------------------------
int ImageSignatureSearchInterface::
SetQueryDescriptorInterface(ImageSignatureDescriptorInterfaceABC
							*QueryImageSignatureDescriptorInterface)
							
{
	return m_SearchTool->
		SetQueryDescriptorInterface(QueryImageSignatureDescriptorInterface);
}

//----------------------------------------------------------------------------
double ImageSignatureSearchInterface::GetDistance(void)

{
	return m_SearchTool->GetDistance();
}



//=============================================================================
ImageSignatureSearchTool::ImageSignatureSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ImageSignatureSearchTool::
ImageSignatureSearchTool(ImageSignatureDescriptorInterfaceABC
						 *aQueryDescriptorInterface)
						 : m_Interface(this)
						 , m_RefDescriptorInterface(0)
						 , m_QueryDescriptorInterface(0)
{
#ifdef VERBOSE
	fprintf(stderr, "Connect descriptor\n");
#endif
	
	if (aQueryDescriptorInterface)
	{
		ImageSignatureDescriptor *descriptor = new ImageSignatureDescriptor();
		aQueryDescriptorInterface = descriptor->GetInterface();
	}
	
	SetQueryDescriptorInterface(aQueryDescriptorInterface);
	
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ImageSignatureSearchTool::~ImageSignatureSearchTool()
{
	if (m_RefDescriptorInterface)
		m_RefDescriptorInterface->release();
	if (m_QueryDescriptorInterface)
		m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& ImageSignatureSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ImageSignatureSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool ImageSignatureSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool ImageSignatureSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int ImageSignatureSearchTool::
SetRefDescriptorInterface(ImageSignatureDescriptorInterfaceABC
						  *aRefImageSignatureDescriptorInterface)
						  
{
	if (m_RefDescriptorInterface ==
		aRefImageSignatureDescriptorInterface) return 0;
	
	if (m_RefDescriptorInterface)
		m_RefDescriptorInterface->release();
	
	m_RefDescriptorInterface = aRefImageSignatureDescriptorInterface;
	if (m_RefDescriptorInterface)
	{
		m_RefDescriptorInterface->addref();
	}
	else
	{
		return -1;
	}
	
	return 0;
}

//----------------------------------------------------------------------------
int  ImageSignatureSearchTool::
SetQueryDescriptorInterface(ImageSignatureDescriptorInterfaceABC
							*aQueryImageSignatureDescriptorInterface)
							
{
	if (m_QueryDescriptorInterface ==
		aQueryImageSignatureDescriptorInterface) return 0;
	
	if (m_QueryDescriptorInterface)
		m_QueryDescriptorInterface->release();
	
	m_QueryDescriptorInterface = aQueryImageSignatureDescriptorInterface;
	if (m_QueryDescriptorInterface)
	{
		m_QueryDescriptorInterface->addref();
	}
	else
	{
		return -1;
	}
	
	return 0;
}

//----------------------------------------------------------------------------

FeatureSignature::FeatureSignature(unsigned char x, unsigned char y, unsigned char angle, ImageSignatureFeatureBits bits, unsigned char *hash_mask)
{
	int i;
	x_ = x;
	y_ = y;
	angle_ = angle;
	memcpy(bits_ , bits, sizeof(bits_));

	for(i=0;i<IMAGESIGNATURE_BYTES_HASH_2;i++)
    hash_[i]=0;

	int head=0;

	for(i=0;i< IMAGESIGNATURE_BITS_FEATURE;i++)
	{
		if((hash_mask[i>>3]&(1<<(0x07-(i&0x07))))>0)
		{
			if( (bits_[i>>3]&(1<<(0x07-(i&0x07))))>0 )
			{
				hash_[head>>3]|=(1<<(0x07-(head&0x07)));
			}
			head++;
		}		
	}
}

FeatureSignature::FeatureSignature()
{
	x_ = 255;
	y_ = 255;
	angle_ = 0;
	memset(bits_, 0, sizeof(bits_));
	memset(hash_,0,sizeof(hash_));
}


void FeatureSignature::print()
{
	std::cout << x_ << "\t" << y_ << std::endl;
}

//----------------------------------------------------------------------------


void intvectobit( int* vec, unsigned char* bitimage, int size )
{
  int i;
  for(i=0; i<(size>>3); i++) 
    bitimage[i]=0;
  for(i=0; i<size;++i )
  {		
    if(vec[i]>0 )
    {			
      bitimage[i>>3]|=1<<(0x07-(i&0x07));						
    }
  }
}

int GlobalSignatureHash(ImageSignatureGlobalBits bits, unsigned char *hash_mask, ImageSignatureGlobalHash _hash)
{
  int i;
  int head = 0;
  for(i=0;i<IMAGESIGNATURE_BYTES_HASH;i++)
    _hash[i]=0;

  for( i = 0; i<IMAGESIGNATURE_BITS_GLOBAL; i++)
  {
    if((hash_mask[i>>3]&(1<<(0x07-(i&0x07))))>0)
    {
      if( (bits[i>>3]&(1<<(0x07-(i&0x07))))>0 )
      {
        //std::cout << (bits_[i>>3]&(1<<(0x07-(i&0x07)))) << ",";
        _hash[head>>3]|=(1<<(0x07-(head&0x07)));
      }
      head++;
    }		

  }

  return 0;
}

//----------------------------------------------------------------------------
double ImageSignatureSearchTool::GetDistance(void)

{
	if (!m_RefDescriptorInterface) return DBL_MAX;
	if (!m_QueryDescriptorInterface) return DBL_MAX;
	
	if (strcmp(m_RefDescriptorInterface->GetName(),
		"Image Signature Description Interface") != 0)
	{
		return DBL_MAX;
	}
	
	if (strcmp(m_QueryDescriptorInterface->GetName(),
		"Image Signature Description Interface") != 0)
	{
		return DBL_MAX;
	}
	
	ImageSignatureGlobalBits refBitsGlobal, queryBitsGlobal, refBitsAspect, queryBitsAspect;
	ImageSignatureBits refBitsFeature, queryBitsFeature;

	m_RefDescriptorInterface->GetGlobalSignature(refBitsGlobal);
	m_QueryDescriptorInterface->GetGlobalSignature(queryBitsGlobal);
	
	m_RefDescriptorInterface->GetAspectSignature(refBitsAspect);
	m_QueryDescriptorInterface->GetAspectSignature(queryBitsAspect);
	
	m_RefDescriptorInterface->GetFeatureSignature(refBitsFeature);
	m_QueryDescriptorInterface->GetFeatureSignature(queryBitsFeature);

	int _signature_hash_mask[IMAGESIGNATURE_BITS_GLOBAL]={0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	unsigned char *signature_hash_mask=new unsigned char[IMAGESIGNATURE_BITS_GLOBAL];
	intvectobit( _signature_hash_mask, signature_hash_mask, IMAGESIGNATURE_BITS_GLOBAL );
	
	int _feature_hash_mask[IMAGESIGNATURE_BITS_FEATURE]={0,1,0,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	unsigned char *feature_hash_mask=new unsigned char[IMAGESIGNATURE_BITS_FEATURE];
	intvectobit( _feature_hash_mask, feature_hash_mask, IMAGESIGNATURE_BITS_FEATURE );
	
	int cost = 0;
	unsigned int i;
	for (i = 0; i < IMAGESIGNATURE_BYTES_GLOBAL; i++)
		cost += costTable[refBitsGlobal[i]^queryBitsGlobal[i]];
	if(cost<147)
	{
		cost=0;
	}
	else
	{
		cost=0;
		for (i = 0; i < IMAGESIGNATURE_BYTES_GLOBAL; i++)
			cost += costTable[refBitsAspect[i]^queryBitsAspect[i]];
		if(cost<169)
		{
			cost=0;
		}
		else
		{
			FeatureSignatures reffs;
			for (i = 0; i < 80; i++)
			{
				unsigned char x, y, angle;
				ImageSignatureFeatureBits bits;
				int addr=(i*11);
				x=refBitsFeature[addr];
				y=refBitsFeature[addr+1];
				angle=refBitsFeature[addr+2];
				memcpy( bits, &refBitsFeature[addr+3], sizeof(ImageSignatureFeatureBits));
				reffs.push_back(FeatureSignature(x,y,angle,bits,feature_hash_mask));
			}
			FeatureSignatures queryfs;
			for (i = 0; i < 80; i++)
			{
				unsigned char x, y, angle;
				ImageSignatureFeatureBits bits;
				int addr=(i*11);
				x=queryBitsFeature[addr];
				y=queryBitsFeature[addr+1];
				angle=queryBitsFeature[addr+2];
				memcpy( bits, &queryBitsFeature[addr+3], sizeof(ImageSignatureFeatureBits));
				queryfs.push_back(FeatureSignature(x,y,angle,bits,feature_hash_mask));
			}

			std::vector<Pair> scores;
			
			std::vector<FeatureSignature>::iterator f1=reffs.begin();
			
			int dist_threshold=15;
			bool ham2=false;
			int dist=0;
			int max = 0;
			int cnt=0;
			for ( int l = 32; l-- ; ++f1 )
			{
				//ImageSignatureBits bits;
				int reffsX=f1->getX();
				
				if (reffsX!=255)
				{
					std::vector<FeatureSignature>::iterator f2=queryfs.begin();
					for ( int m = 32; m-- ; ++f2)
					{
						dist=0;
						
						if( f2->getX()==255 ) 
							dist=IMAGESIGNATURE_BITS_FEATURE;
						else
						{			   
							dist = f1->distance(*f2);
						}
						if(dist<dist_threshold)
						{
							scores.push_back(Pair(*f1,*f2,dist,l,m));
						}
						
					}
				}		  
			}
			
			std::sort(scores.begin(), scores.end());
			
			cost=4*IMAGESIGNATURE_BITS_FEATURE;
			
			int scoresize=(int)scores.size();
			int done=0;
			
			if(scoresize>3)
			{
				if(scoresize>18)
					scoresize=18;
				for(int ii=0; (ii<scoresize) && (done == 0);ii++ )
				{
					int a0=scores[ii].get_a_angle();
					int b0=scores[ii].get_b_angle();
					for(int jj=(ii+1);(jj<scoresize) && (done == 0);jj++)
					{
						int a1=scores[jj].get_a_angle();			  									
						int b1=scores[jj].get_b_angle();

						for(int kk=(jj+1); (kk<scoresize) && (done == 0);kk++)
						{
							if(scores[ii].get_idxa()!=scores[jj].get_idxa() && scores[ii].get_idxa()!=scores[kk].get_idxa() && scores[jj].get_idxa()!=scores[kk].get_idxa() \
								&& scores[ii].get_idxb()!=scores[jj].get_idxb() && scores[ii].get_idxb()!=scores[kk].get_idxb() && scores[jj].get_idxb()!=scores[kk].get_idxb() )
							{
								cost=scores[ii].getDistance() + scores[jj].getDistance() + scores[kk].getDistance();
								
								if( cost < 38 )
								{									
									int angle_a1=a0-a1; 			 
									int angle_b1=b0-b1;
									int R3=abs(angle_a1-angle_b1);
									int R3a=abs(angle_a1+angle_b1);
									
									if( R3<2 || (R3>14 && R3 <18) || R3a<2 || (R3a>14 && R3a <18) )
									{
										int a2=scores[kk].get_a_angle();
										int b2=scores[kk].get_b_angle();			  
										int angle_a2=a1-a2;
										int angle_b2=b1-b2;
										int R4=abs(angle_a2-angle_b2);
										int R4a=abs(angle_a2+angle_b2);
										int angle_a3=a2-a0;
										int angle_b3=b2-b0;
										int R5=abs(angle_a3-angle_b3);
										int R5a=abs(angle_a3+angle_b3);
										if( R4<2 || (R4>14 && R4 <18) || R4a<2 || (R4a>14 && R4a <18))
										{
											if( R5<2 || (R5>14 && R5 <18) || R5a<2 || (R5a>14 && R5a <18))
											{
												
												double buf1=(scores[ii].get_a_x()-scores[jj].get_a_x());
												double buf2=(scores[ii].get_a_y()-scores[jj].get_a_y());
												double d1=sqrt(buf1*buf1+buf2*buf2);
												buf1=(scores[ii].get_a_x()-scores[kk].get_a_x());
												buf2=(scores[ii].get_a_y()-scores[kk].get_a_y());
												double d2=sqrt(buf1*buf1+buf2*buf2);
												double geom_ratio1=d1/d2;
												
												buf1=(scores[ii].get_b_x()-scores[jj].get_b_x());
												buf2=(scores[ii].get_b_y()-scores[jj].get_b_y());
												double d3=sqrt(buf1*buf1+buf2*buf2);
												buf1=(scores[ii].get_b_x()-scores[kk].get_b_x());
												buf2=(scores[ii].get_b_y()-scores[kk].get_b_y());
												double d4=sqrt(buf1*buf1+buf2*buf2);				  
												double geom_ratio4=d3/d4;
												static double tolerance2=0.06;
												double R1=fabs(geom_ratio1-geom_ratio4)/(geom_ratio1+geom_ratio4);
												
												if((R1)<=tolerance2)
												{				   
													double tolerance1=0.1;
													buf1=(scores[jj].get_a_x()-scores[kk].get_a_x());
													buf2=(scores[jj].get_a_y()-scores[kk].get_a_y());
													double d5=sqrt(buf1*buf1+buf2*buf2);
													double geom_ratio2=d2/d5;
													double geom_ratio3=d5/d1;
													
													buf1=(scores[jj].get_b_x()-scores[kk].get_b_x());
													buf2=(scores[jj].get_b_y()-scores[kk].get_b_y());
													double d6=sqrt(buf1*buf1+buf2*buf2);  
													double geom_ratio5=d4/d6;
													double geom_ratio6=d6/d3;
													double R2=fabs(geom_ratio2-geom_ratio5)/(geom_ratio2+geom_ratio5);
													double R3=fabs(geom_ratio3-geom_ratio6)/(geom_ratio3+geom_ratio6);
													
													if((R2)<=tolerance2 && (R3)<=tolerance2 && (R1+R2)<=tolerance1 && (R1+R3)<=tolerance1 && (R2+R3)<=tolerance1)
													{						 
														while((int)scores.size()>scoresize)
															scores.pop_back();
														if(!ham2)
														{
															for ( int l = 0;l<80; l++ )
															{
																//ImageSignatureBits bits;
																int reffsX=reffs[l].getX();
																if (reffsX!=255)
																{
																	for ( int m = 0; m<80; m++ )
																	{
																		if(!(l<32 && m<32))
																		{
																			int dist2=0;
																			if( queryfs[m].getX()==255 ) 
																				dist2=IMAGESIGNATURE_BITS_FEATURE;
																			else
																			{			   
																				dist2 = reffs[l].distance(queryfs[m]);
																			}
																			if(dist2<dist_threshold)
																				scores.push_back(Pair(reffs[l],queryfs[m],dist2,l,m));
																		}
																	}		  
																}
															}
															ham2=true;
														}
														int scoresize2=(int)scores.size();
														
														for(int ll=(kk+1); (ll<scoresize2) && (done == 0);ll++)
														{
															if(scores[ii].get_idxa()!=scores[ll].get_idxa() && scores[jj].get_idxa()!=scores[ll].get_idxa() && scores[kk].get_idxa()!=scores[ll].get_idxa() \
																&& scores[ii].get_idxb()!=scores[ll].get_idxb() && scores[jj].get_idxb()!=scores[ll].get_idxb() && scores[kk].get_idxb()!=scores[ll].get_idxb() )
															{
																cost=scores[ii].getDistance() + scores[jj].getDistance() + scores[kk].getDistance()+scores[ll].getDistance();
																if( cost < 39 )
																{
																	int a4=scores[ll].get_a_angle();
																	int b4=scores[ll].get_b_angle();			  
																	int angle_a4=a4-a1;
																	int angle_b4=b4-b1;
																	int R6=abs(angle_a4-angle_b4);
																	int R6a=abs(angle_a4+angle_b4);
																	if( R6<2 || (R6>14 && R6 <18) || R6a<2 || (R6a>14 && R6a <18)) 
																	{
																		buf1=(scores[ii].get_a_x()-scores[ll].get_a_x());
																		buf2=(scores[ii].get_a_y()-scores[ll].get_a_y());
																		double d7=sqrt(buf1*buf1+buf2*buf2);
																		buf1=(scores[jj].get_a_x()-scores[ll].get_a_x());
																		buf2=(scores[jj].get_a_y()-scores[ll].get_a_y());
																		double d8=sqrt(buf1*buf1+buf2*buf2);
																		buf1=(scores[kk].get_a_x()-scores[ll].get_a_x());
																		buf2=(scores[kk].get_a_y()-scores[ll].get_a_y());
																		double d9=sqrt(buf1*buf1+buf2*buf2);
																		buf1=(scores[ii].get_b_x()-scores[ll].get_b_x());
																		buf2=(scores[ii].get_b_y()-scores[ll].get_b_y());
																		double d10=sqrt(buf1*buf1+buf2*buf2);  
																		buf1=(scores[jj].get_b_x()-scores[ll].get_b_x());
																		buf2=(scores[jj].get_b_y()-scores[ll].get_b_y());
																		double d11=sqrt(buf1*buf1+buf2*buf2); 
																		buf1=(scores[kk].get_b_x()-scores[ll].get_b_x());
																		buf2=(scores[kk].get_b_y()-scores[ll].get_b_y());
																		double d12=sqrt(buf1*buf1+buf2*buf2);
																		
																		double s=(d1+d5+d2)*0.5;
																		double TA1=sqrt(s*(s-d1)*(s-d5)*(s-d2));								 
																		s=(d1+d8+d7)*0.5;
																		double TA2=sqrt(s*(s-d1)*(s-d8)*(s-d7));
																		s=(d8+d9+d5)*0.5;
																		double TA3=sqrt(s*(s-d8)*(s-d9)*(s-d5));
																		s=(d9+d2+d7)*0.5;
																		double TA4=sqrt(s*(s-d9)*(s-d2)*(s-d7));
																		if(TA1!=0.0 && !isnan(TA1) && TA2!=0.0 && !isnan(TA2) && TA3!=0.0 && !isnan(TA3) && TA4!=0.0&& !isnan(TA4) )
																		{
																			double TRA1=TA1/TA2;
																			double TRA2=TA2/TA3;
																			double TRA3=TA3/TA4;
																			double TRA4=TA4/TA1;
																			
																			s=(d3+d6+d4)*0.5;
																			double TB1=sqrt(s*(s-d3)*(s-d6)*(s-d4));
																			s=(d3+d11+d10)*0.5;
																			double TB2=sqrt(s*(s-d3)*(s-d11)*(s-d10));
																			s=(d11+d12+d6)*0.5;
																			double TB3=sqrt(s*(s-d11)*(s-d12)*(s-d6));
																			s=(d12+d4+d10)*0.5;
																			double TB4=sqrt(s*(s-d12)*(s-d4)*(s-d10));
																			
																			double TRB1=TB1/TB2;
																			double TRB2=TB2/TB3;
																			double TRB3=TB3/TB4;
																			double TRB4=TB4/TB1;
																			if(TB1!=0.0 && !isnan(TB1) && TB2!=0.0 && !isnan(TB2) && TB3!=0.0 && !isnan(TB3) && TB4!=0.0 && !isnan(TB4))
																			{
																				
																				double T1=fabs(TRA1-TRB1)/(TRA1+TRB1);
																				double T2=fabs(TRA2-TRB2)/(TRA2+TRB2);
																				double T3=fabs(TRA3-TRB3)/(TRA3+TRB3);
																				double T4=fabs(TRA4-TRB4)/(TRA4+TRB4);
																				double tolerance3=0.2;
																				double tolerance4=0.55;
																				if(T1<tolerance3 && T2<tolerance3 && T3<tolerance3 && T4<tolerance3 && (T1+T2+T3+T4)<tolerance4)
																				{
																					done=1;
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			
			if(done==0)
				cost=4*IMAGESIGNATURE_BITS_FEATURE;
			scores.clear();
		}
	}

	return (double)cost / (double)(4*IMAGESIGNATURE_BITS_FEATURE);
}

//----------------------------------------------------------------------------
ImageSignatureSearchInterfaceABC *ImageSignatureSearchTool::GetInterface(void)
{ return &m_Interface; }

