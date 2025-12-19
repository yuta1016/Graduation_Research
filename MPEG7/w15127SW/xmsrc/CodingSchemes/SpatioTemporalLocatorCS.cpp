///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// K.Masukura (R&D Center Toshiba, Japan)
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
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SpatioTemporalLocatorCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <sys/types.h>
#ifndef _VISUALC_
#include <netinet/in.h>
#endif
 

using namespace XM;

const UUID SpatioTemporalLocatorCSInterface::myID = UUID("");
const char *SpatioTemporalLocatorCSInterface::myName = "SpatioTemporalLocator Coding Engine Interface";

const UUID SpatioTemporalLocatorCS::myID = UUID("");
const char *SpatioTemporalLocatorCS::myName = "SpatioTemporalLocatorCodingScheme";

const UUID SpatioTemporalLocatorCS::valID = UUID("");
const char *SpatioTemporalLocatorCS::valName = "";

const UUID SpatioTemporalLocatorCSInterfaceABC::myID = UUID("");

SpatioTemporalLocatorCSInterfaceABC::SpatioTemporalLocatorCSInterfaceABC()

{
  SetInterfaceID(myID);
}

//=============================================================================
SpatioTemporalLocatorCSInterface::SpatioTemporalLocatorCSInterface(SpatioTemporalLocatorCS *aSpatioTemporalLocator):
m_SpatioTemporalLocatorCS(aSpatioTemporalLocator)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorCSInterface::~SpatioTemporalLocatorCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpatioTemporalLocatorCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpatioTemporalLocatorCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
BitBuffer *SpatioTemporalLocatorCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpatioTemporalLocatorCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpatioTemporalLocatorCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpatioTemporalLocatorCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpatioTemporalLocatorCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpatioTemporalLocatorCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorDescriptorInterfaceABC *SpatioTemporalLocatorCSInterface::
GetDescriptorInterface(void)
{
        return m_SpatioTemporalLocatorCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorCSInterface::
SetDescriptorInterface(SpatioTemporalLocatorDescriptorInterfaceABC
		       *aSpatioTemporalLocatorDescriptorInterface)
{
        return m_SpatioTemporalLocatorCS->
	  SetDescriptorInterface(aSpatioTemporalLocatorDescriptorInterface);
}

//----------------------------------------------------------------------------
RegionLocatorCSInterfaceABC 
*SpatioTemporalLocatorCSInterface::GetRegionLocatorCSInterface(void)
{
  return m_SpatioTemporalLocatorCS->GetRegionLocatorCSInterface();
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorCSInterface::
SetRegionLocatorCSInterface(RegionLocatorCSInterfaceABC
			    *aRegionLocatorCSInterface)
{
  return m_SpatioTemporalLocatorCS->
    SetRegionLocatorCSInterface(aRegionLocatorCSInterface);
}

//----------------------------------------------------------------------------
TemporalInterpolationCSInterfaceABC 
*SpatioTemporalLocatorCSInterface::GetTemporalInterpolationCSInterface(void)
{
  return m_SpatioTemporalLocatorCS->GetTemporalInterpolationCSInterface();
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorCSInterface::
SetTemporalInterpolationCSInterface(TemporalInterpolationCSInterfaceABC
			    *aTemporalInterpolationCSInterface)
{
  return m_SpatioTemporalLocatorCS->
    SetTemporalInterpolationCSInterface(aTemporalInterpolationCSInterface);
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorCSInterface::StartEncode(void)
{
        return m_SpatioTemporalLocatorCS->StartEncode();
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorCSInterface::StartDecode(void)
{
        return m_SpatioTemporalLocatorCS->StartDecode();
}

//----------------------------------------------------------------------------
void SpatioTemporalLocatorCSInterface::destroy(void)
{ delete m_SpatioTemporalLocatorCS; }

//=============================================================================

SpatioTemporalLocatorCS::SpatioTemporalLocatorCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpatioTemporalLocatorDescriptorInterface(0),
m_RegionLocatorCSInterface(0),
m_TemporalInterpolationCSInterface(0)
{
	/* create sub CS and
		connect them with descriptor if availabe*/
	RegionLocatorCS *m_RegionLocatorCS= new RegionLocatorCS();
	TemporalInterpolationCS *m_TemporalInterpolationCS= new TemporalInterpolationCS();

	/* connect sub CS with SpatioTemporalLocatorCS*/
	if (m_RegionLocatorCS)
		SetRegionLocatorCSInterface(m_RegionLocatorCS->GetInterface());
	if (m_TemporalInterpolationCS)
		SetTemporalInterpolationCSInterface(m_TemporalInterpolationCS->GetInterface());

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorCS::
SpatioTemporalLocatorCS(SpatioTemporalLocatorDescriptorInterfaceABC
			*SpatioTemporalLocator):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpatioTemporalLocatorDescriptorInterface(0),
m_RegionLocatorCSInterface(0),
m_TemporalInterpolationCSInterface(0)
{
  RegionLocatorCS *m_RegionLocatorCS;
  TemporalInterpolationCS *m_TemporalInterpolationCS;

  if (!SpatioTemporalLocator) {
    SpatioTemporalLocatorDescriptor *descriptor =
      new SpatioTemporalLocatorDescriptor(0,0);
    SpatioTemporalLocator = descriptor->GetInterface();
  }

  SetDescriptorInterface(SpatioTemporalLocator);

#ifdef VERBOSE
  fprintf(stderr,"Connect sub descriptors\n");
#endif

  /* create sub CS and
     connect them with descriptor if availabe*/
  if (SpatioTemporalLocator->GetRegionLocatorDescriptorInterface(0)) {
    m_RegionLocatorCS= new
       RegionLocatorCS(SpatioTemporalLocator->
       GetRegionLocatorDescriptorInterface(0));
  }
  else { 
    m_RegionLocatorCS= new RegionLocatorCS();
  }

  if (SpatioTemporalLocator->
      GetTemporalInterpolationDescriptorInterface(0,0)) {
    m_TemporalInterpolationCS= new
      TemporalInterpolationCS(SpatioTemporalLocator->
			  GetTemporalInterpolationDescriptorInterface(0,0));
  }
  else { 
    m_TemporalInterpolationCS= new TemporalInterpolationCS();
  }

  if (m_RegionLocatorCS)
    SetRegionLocatorCSInterface(m_RegionLocatorCS->GetInterface());
  if (m_TemporalInterpolationCS)
    SetTemporalInterpolationCSInterface(m_TemporalInterpolationCS->
					GetInterface());

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorCS::~SpatioTemporalLocatorCS()
{
  if (m_SpatioTemporalLocatorDescriptorInterface)
    m_SpatioTemporalLocatorDescriptorInterface->release();

  if (m_RegionLocatorCSInterface) m_RegionLocatorCSInterface->destroy();
  if (m_TemporalInterpolationCSInterface)
    m_TemporalInterpolationCSInterface->destroy();

}

//----------------------------------------------------------------------------
const UUID& SpatioTemporalLocatorCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpatioTemporalLocatorCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpatioTemporalLocatorCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpatioTemporalLocatorCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpatioTemporalLocatorCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpatioTemporalLocatorCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_EncoderBuffer = aBuffer;

  if (m_RegionLocatorCSInterface)
    m_RegionLocatorCSInterface->SetEncoderStreamBuffer(aBuffer);
  if (m_TemporalInterpolationCSInterface)
    m_TemporalInterpolationCSInterface->SetEncoderStreamBuffer(aBuffer);
	
  return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpatioTemporalLocatorCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	if (m_RegionLocatorCSInterface)
	  m_RegionLocatorCSInterface->SetDecoderStreamBuffer(aBuffer);
	if (m_TemporalInterpolationCSInterface)
	  m_TemporalInterpolationCSInterface->SetDecoderStreamBuffer(aBuffer);

	return 0;
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorDescriptorInterfaceABC *SpatioTemporalLocatorCS::GetDescriptorInterface()
{
        return m_SpatioTemporalLocatorDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorCS::
SetDescriptorInterface(SpatioTemporalLocatorDescriptorInterfaceABC
	      *aSpatioTemporalLocatorDescriptorInterface)
{
  /* check if new value is different from old value*/
  if (m_SpatioTemporalLocatorDescriptorInterface == 
      aSpatioTemporalLocatorDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_SpatioTemporalLocatorDescriptorInterface) 
    m_SpatioTemporalLocatorDescriptorInterface->release();

  /* add new interface*/
  m_SpatioTemporalLocatorDescriptorInterface = 
    aSpatioTemporalLocatorDescriptorInterface;
  if (m_SpatioTemporalLocatorDescriptorInterface) {
    m_SpatioTemporalLocatorDescriptorInterface->addref();

    /* conect sub descriptors with sub CS*/
    if ((m_SpatioTemporalLocatorDescriptorInterface->
	 GetRegionLocatorDescriptorInterface(0)) &&
	m_RegionLocatorCSInterface) {
      m_RegionLocatorCSInterface->
	SetDescriptorInterface(m_SpatioTemporalLocatorDescriptorInterface->
			       GetRegionLocatorDescriptorInterface(0));
    }
    if ((m_SpatioTemporalLocatorDescriptorInterface->
	 GetTemporalInterpolationDescriptorInterface(0,0)) &&
	m_TemporalInterpolationCSInterface) {
      m_TemporalInterpolationCSInterface->
	SetDescriptorInterface(m_SpatioTemporalLocatorDescriptorInterface->
		       GetTemporalInterpolationDescriptorInterface(0,0));
    }
  }
  else {
    m_RegionLocatorCSInterface->SetDescriptorInterface(0);
    m_TemporalInterpolationCSInterface->SetDescriptorInterface(0);

    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
RegionLocatorCSInterfaceABC *SpatioTemporalLocatorCS::
GetRegionLocatorCSInterface(void)
{
  return m_RegionLocatorCSInterface;
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorCS::
SetRegionLocatorCSInterface(RegionLocatorCSInterfaceABC
			    *aRegionLocatorCSInterface)
{
  if (!aRegionLocatorCSInterface) return -1;

  m_RegionLocatorCSInterface=aRegionLocatorCSInterface;

  return 0;
}

//----------------------------------------------------------------------------
TemporalInterpolationCSInterfaceABC *SpatioTemporalLocatorCS::GetTemporalInterpolationCSInterface(void)
{
  return m_TemporalInterpolationCSInterface;
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorCS::
SetTemporalInterpolationCSInterface(TemporalInterpolationCSInterfaceABC
			    *aTemporalInterpolationCSInterface)
{
  if (!aTemporalInterpolationCSInterface) return -1;

  m_TemporalInterpolationCSInterface=aTemporalInterpolationCSInterface;

  return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorCS::GetVluimsbf()
{
	long n, a;
	unsigned long ul;

	n=0;
	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	while( a == 1 ){
		n ++;
		m_DecoderBuffer->GetBitsLong(&a,1);
		m_DecoderBuffer->TakeBits(1);
	}
	m_DecoderBuffer->GetBitsLong((long*)&ul,(n+1)*8);
	m_DecoderBuffer->TakeBits((n+1)*8);
	return ul;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorCS::PutVluimsbf(unsigned long value)
{
	int i, n;

	for (n=3;n>0;n--) {
		if( (value>>(n*8)) != 0 ) break;
	}
	for(i=0;i<n;i++) m_EncoderBuffer->PutBitsLong(1,1);
	m_EncoderBuffer->PutBitsLong(0,1);
	m_EncoderBuffer->PutBitsLong(value,(n+1)*8);
	
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorCS::EncodeTimePoint(double t)
{
	long a, time;
	time = (long)t;
	a = 2;
	m_EncoderBuffer->PutBitsLong(a,3);	// Representation
	a = 1;
	m_EncoderBuffer->PutBitsLong(a,1);	// Sign
	a = time/3600;
	m_EncoderBuffer->PutBitsLong(a,5);	// Hours
	time -= 3600*a;
	a = time/60;
	m_EncoderBuffer->PutBitsLong(a,6);	// Minutes
	time -= 60*a;
	a = time;
	m_EncoderBuffer->PutBitsLong(a,6);	// Seconds
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorCS::EncodeDuration(double t)
{
	long a, time, h, m, s, r;

	time = (long)t;
	h = time/3600;
	time -= 3600*h;
	m = time/60;
	time -= 60*m;
	s = time;

	r = 0;
	if(h>0){ r=14; }
	else if(m>0) { r=6; }
	else if(s>0) { r=2; }

	m_EncoderBuffer->PutBitsLong(r,5);	// Representation
	a = 1;
	m_EncoderBuffer->PutBitsLong(a,1);	// Sign
	if(r&8) m_EncoderBuffer->PutBitsLong(h,10);	// Hours
	if(r&4) m_EncoderBuffer->PutBitsLong(m,10);	// Minutes
	if(r&2) m_EncoderBuffer->PutBitsLong(s,10);	// Seconds
	return 0;
}

//----------------------------------------------------------------------------
double SpatioTemporalLocatorCS::DecodeTimePoint()
{
	long a, r, s, n, l;
	double time;

	m_DecoderBuffer->GetBitsLong(&r,3);
	m_DecoderBuffer->TakeBits(3);
	m_DecoderBuffer->GetBitsLong(&s,1);
	m_DecoderBuffer->TakeBits(1);

	time = 0;
	if(r&2){
		m_DecoderBuffer->GetBitsLong(&a,5);
		m_DecoderBuffer->TakeBits(5);
		time += a*3600;
		m_DecoderBuffer->GetBitsLong(&a,6);
		m_DecoderBuffer->TakeBits(6);
		time += a*60;
		m_DecoderBuffer->GetBitsLong(&a,6);
		m_DecoderBuffer->TakeBits(6);
		time += a;
	}
	if(r&1){
		n = 0;
		do {
			m_DecoderBuffer->GetBitsLong(&a,8);
			m_DecoderBuffer->TakeBits(8);
			n += a;
			m_DecoderBuffer->GetBitsLong(&a,1);
			m_DecoderBuffer->TakeBits(1);
		} while(a==1);
		l = (long)ceil(log((double)n)/log(2.0));
		m_DecoderBuffer->GetBitsLong(&a,l);
		m_DecoderBuffer->TakeBits(l);
		time += (double)n/(double)a;
	}
	if(s>0) return time;
	else return -time;
}

//----------------------------------------------------------------------------
double SpatioTemporalLocatorCS::DecodeDuration()
{
	long a, r, s, n, l;
	double time;

	m_DecoderBuffer->GetBitsLong(&r,5);
	m_DecoderBuffer->TakeBits(5);
	m_DecoderBuffer->GetBitsLong(&s,1);
	m_DecoderBuffer->TakeBits(1);

	time = 0;
	if(r&8){
		m_DecoderBuffer->GetBitsLong(&a,10);
		m_DecoderBuffer->TakeBits(10);
		time += a*3600;
	}
	if(r&4){
		m_DecoderBuffer->GetBitsLong(&a,10);
		m_DecoderBuffer->TakeBits(10);
		time += a*60;
	}
	if(r&2){
		m_DecoderBuffer->GetBitsLong(&a,10);
		m_DecoderBuffer->TakeBits(10);
		time += a;
	}
	if(r&1){
		n = 0;
		do {
			m_DecoderBuffer->GetBitsLong(&a,8);
			m_DecoderBuffer->TakeBits(8);
			n += a;
			m_DecoderBuffer->GetBitsLong(&a,1);
			m_DecoderBuffer->TakeBits(1);
		} while(a==1);
		l = (long)ceil(log((double)n)/log(2.0));
		m_DecoderBuffer->GetBitsLong(&a,l);
		m_DecoderBuffer->TakeBits(l);
		time += (double)n/(double)a;
	}
	if(s>0) return time;
	else return -time;
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorCS::StartEncode(void)
{
	unsigned long a, b, N;
	double c;
	/* check if Descriptor and stream buffers are linked
	to coding scheme*/
	if (!m_EncoderBuffer || !m_SpatioTemporalLocatorDescriptorInterface) {
		return -1;
	}
	
	a = m_SpatioTemporalLocatorDescriptorInterface->GetCoordFlag();
	m_EncoderBuffer->PutBitsLong(a,1UL);
	if (a!=0) {
		b = m_SpatioTemporalLocatorDescriptorInterface->GetRef();
		m_EncoderBuffer->PutBitsLong(b,8UL);
		b = m_SpatioTemporalLocatorDescriptorInterface->GetspatialRef();
		m_EncoderBuffer->PutBitsLong(a,1UL);		
	}
	
	N = m_SpatioTemporalLocatorDescriptorInterface->GetNumberOfRefernceRegions();
//	m_EncoderBuffer->PutBitsLong(N,8UL);
	PutVluimsbf(N);
	
	int i,j;
	for (i=0; i<N; i++) {
		a = m_SpatioTemporalLocatorDescriptorInterface->GetTypeOfTrajectory(i);
		m_EncoderBuffer->PutBitsLong(a,1UL);
		
		if (a == 0) {		//  FigureTrajectory
			
			b = m_SpatioTemporalLocatorDescriptorInterface->GetMediaTimePoint(i);
			EncodeTimePoint((double)b);
			b = m_SpatioTemporalLocatorDescriptorInterface->GetMediaDuration(i);
			EncodeDuration((double)b);
			
			b = m_SpatioTemporalLocatorDescriptorInterface->Gettype(i);
			m_EncoderBuffer->PutBitsLong(b,6UL);
			
			int Nv = 0;
			if (b==1 || b==2) { Nv = 3; }
			else if (b>=3) { Nv = b; }
			
			for (j=0; j<Nv; j++) {	// TI
				m_TemporalInterpolationCSInterface->
					SetDescriptorInterface(m_SpatioTemporalLocatorDescriptorInterface->
					GetTemporalInterpolationDescriptorInterface(i,j));
				m_TemporalInterpolationCSInterface->StartEncode();
			}
			
			// 3D
			a = m_SpatioTemporalLocatorDescriptorInterface->GetDepthFlag(i);
			m_EncoderBuffer->PutBitsLong(a,1UL);
			if (a == 1) {
				m_TemporalInterpolationCSInterface->
					SetDescriptorInterface(m_SpatioTemporalLocatorDescriptorInterface->
					GetTemporalInterpolationDescriptorInterface(i,
					MAXNRefrence*MAXNVertices+1));
				m_TemporalInterpolationCSInterface->StartEncode();
			}
			
		} else if( a==1 ){			//  ParameterTrajectory
	
			b = m_SpatioTemporalLocatorDescriptorInterface->GetMediaTimePoint(i);
			EncodeTimePoint((double)b);
			b = m_SpatioTemporalLocatorDescriptorInterface->GetMediaDuration(i);
			EncodeDuration((double)b);
			
			b = m_SpatioTemporalLocatorDescriptorInterface->GetmotionModel(i);
			m_EncoderBuffer->PutBitsLong(b,3UL);
			
			c = m_SpatioTemporalLocatorDescriptorInterface->GetellipseFlag(i);
			m_EncoderBuffer->PutBitsLong(c,1UL);
			
			// RegionLocator
			m_RegionLocatorCSInterface->
				SetDescriptorInterface(m_SpatioTemporalLocatorDescriptorInterface->
				GetRegionLocatorDescriptorInterface(i));
			m_RegionLocatorCSInterface->StartEncode();
			
			// TI
			if(m_SpatioTemporalLocatorDescriptorInterface->GetmotionModel(i)>0){
				m_TemporalInterpolationCSInterface->
					SetDescriptorInterface(m_SpatioTemporalLocatorDescriptorInterface->
					GetTemporalInterpolationDescriptorInterface(i,0));
				m_TemporalInterpolationCSInterface->StartEncode();
			}
			
			// 3D
			a = m_SpatioTemporalLocatorDescriptorInterface->GetDepthFlag(i);
			m_EncoderBuffer->PutBitsLong(a,1UL);
			if (a == 1) {
				m_TemporalInterpolationCSInterface->
					SetDescriptorInterface(m_SpatioTemporalLocatorDescriptorInterface->
					GetTemporalInterpolationDescriptorInterface(i,
					MAXNRefrence*MAXNVertices+1));
				m_TemporalInterpolationCSInterface->StartEncode();
			}
			
		} else {	// MediaTime

			b = m_SpatioTemporalLocatorDescriptorInterface->GetMediaTimePoint(i);
			EncodeTimePoint((double)b);
			b = m_SpatioTemporalLocatorDescriptorInterface->GetMediaDuration(i);
			EncodeDuration((double)b);
			
		}
		
	}
	
	return 0;
}

//----------------------------------------------------------------------------
int SpatioTemporalLocatorCS::StartDecode(void)
{
	long a, b, N;
	
	/* check if Descriptor and stream buffers are linked
	to coding scheme*/
	if (!m_DecoderBuffer || !m_SpatioTemporalLocatorDescriptorInterface) {
		return -1;
	}
	
	m_DecoderBuffer->GetBitsLong(&a,1UL);
	m_DecoderBuffer->TakeBits(1UL);
	m_SpatioTemporalLocatorDescriptorInterface->SetCoordFlag(a);
	if (a!=0) {
		m_DecoderBuffer->GetBitsLong(&b,8UL);
		m_DecoderBuffer->TakeBits(8UL);
		m_SpatioTemporalLocatorDescriptorInterface->SetRef(b);
		m_DecoderBuffer->GetBitsLong(&b,1UL);
		m_DecoderBuffer->TakeBits(1UL);
		m_SpatioTemporalLocatorDescriptorInterface->SetspatialRef(b);
	}
	
//	m_DecoderBuffer->GetBitsLong(&N,8UL);
//	m_DecoderBuffer->TakeBits(8UL);
	N = GetVluimsbf();
	m_SpatioTemporalLocatorDescriptorInterface->SetNumberOfRefernceRegions(N);
	
	long i,j;
	for (i=0; i<N; i++) {
		m_DecoderBuffer->GetBitsLong(&a,1UL);
		m_DecoderBuffer->TakeBits(1UL);
		m_SpatioTemporalLocatorDescriptorInterface->SetTypeOfTrajectory(i,a);
		
		if (a == 0) {		//  FigureTrajectory

			b = (unsigned long)DecodeTimePoint();
			m_SpatioTemporalLocatorDescriptorInterface->SetMediaTimePoint(i,b);
			b = (unsigned long)DecodeDuration();
			m_SpatioTemporalLocatorDescriptorInterface->SetMediaDuration(i,b);
			
			m_DecoderBuffer->GetBitsLong(&b,6UL);
			m_DecoderBuffer->TakeBits(6UL);
			m_SpatioTemporalLocatorDescriptorInterface->Settype(i,b);
			
			int Nv = 0;
			if (b==1 || b==2) { Nv = 3; }
			else if (b>=3) { Nv = b; }
			
			for (j=0; j<Nv; j++) {	// TI
				m_TemporalInterpolationCSInterface->
					SetDescriptorInterface(m_SpatioTemporalLocatorDescriptorInterface->
					GetTemporalInterpolationDescriptorInterface(i,j));
				m_TemporalInterpolationCSInterface->StartDecode();
			}
			
			// 3D
			m_DecoderBuffer->GetBitsLong(&a,1UL);
			m_DecoderBuffer->TakeBits(1UL);
			m_SpatioTemporalLocatorDescriptorInterface->SetDepthFlag(i,a);
			if (a==1) {
				m_TemporalInterpolationCSInterface->
					SetDescriptorInterface(m_SpatioTemporalLocatorDescriptorInterface->
					GetTemporalInterpolationDescriptorInterface(i,
					MAXNRefrence*MAXNVertices+1));
				m_TemporalInterpolationCSInterface->StartDecode();
			}
			
		} else if( a==1 ) {			//  ParameterTrajectory

			b = (unsigned long)DecodeTimePoint();
			m_SpatioTemporalLocatorDescriptorInterface->SetMediaTimePoint(i,b);
			b = (unsigned long)DecodeDuration();
			m_SpatioTemporalLocatorDescriptorInterface->SetMediaDuration(i,b);
			
			m_DecoderBuffer->GetBitsLong(&b,3UL);
			m_DecoderBuffer->TakeBits(3UL);
			m_SpatioTemporalLocatorDescriptorInterface->SetmotionModel(i,b);
			
			m_DecoderBuffer->GetBitsLong(&b,1UL);
			m_DecoderBuffer->TakeBits(1UL);
			m_SpatioTemporalLocatorDescriptorInterface->SetellipseFlag(i,b);
			
			// RegionLocator
			m_RegionLocatorCSInterface->
				SetDescriptorInterface(m_SpatioTemporalLocatorDescriptorInterface->
				GetRegionLocatorDescriptorInterface(i));
			m_RegionLocatorCSInterface->StartDecode();
			
			// TI
			if( m_SpatioTemporalLocatorDescriptorInterface->GetmotionModel(i)>0 ){
				m_TemporalInterpolationCSInterface->
					SetDescriptorInterface(m_SpatioTemporalLocatorDescriptorInterface->
					GetTemporalInterpolationDescriptorInterface(i,0));
				m_TemporalInterpolationCSInterface->StartDecode();
			}
			
			// 3D
			m_DecoderBuffer->GetBitsLong(&a,1UL);
			m_DecoderBuffer->TakeBits(1UL);
			m_SpatioTemporalLocatorDescriptorInterface->SetDepthFlag(i,a);
			if (a==1) {
				m_TemporalInterpolationCSInterface->
					SetDescriptorInterface(m_SpatioTemporalLocatorDescriptorInterface->
					GetTemporalInterpolationDescriptorInterface(i,
					MAXNRefrence*MAXNVertices+1));
				m_TemporalInterpolationCSInterface->StartDecode();
			}
			
		} else {	// MediaTime

			b = (unsigned long)DecodeTimePoint();
			m_SpatioTemporalLocatorDescriptorInterface->SetMediaTimePoint(i,b);
			b = (unsigned long)DecodeDuration();
			m_SpatioTemporalLocatorDescriptorInterface->SetMediaDuration(i,b);

		}
		
	}
	
	return 0;
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorCSInterfaceABC *SpatioTemporalLocatorCS::GetInterface(void)
{ return &m_Interface; }



