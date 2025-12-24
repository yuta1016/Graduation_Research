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
// Applicable File Name:  TemporalInterpolationCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <sys/types.h>
#ifndef WIN32
#include <netinet/in.h>
#endif
 

using namespace XM;

const UUID TemporalInterpolationCSInterface::myID = UUID("");
const char *TemporalInterpolationCSInterface::myName = "Temporal Interpolation Coding Engine Interface";

const UUID TemporalInterpolationCS::myID = UUID("");
const char *TemporalInterpolationCS::myName = "Temporal Interpolation Coding Scheme";

const UUID TemporalInterpolationCS::valID = UUID("");
const char *TemporalInterpolationCS::valName = "";

const UUID TemporalInterpolationCSInterfaceABC::myID = UUID("");

TemporalInterpolationCSInterfaceABC::TemporalInterpolationCSInterfaceABC()

{
  SetInterfaceID(myID);
}

//=============================================================================
TemporalInterpolationCSInterface::TemporalInterpolationCSInterface(TemporalInterpolationCS *aTemporalInterpolation):
m_TemporalInterpolationCS(aTemporalInterpolation)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TemporalInterpolationCSInterface::~TemporalInterpolationCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& TemporalInterpolationCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TemporalInterpolationCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
BitBuffer *TemporalInterpolationCSInterface::GetEncoderStreamBuffer(void)
{
        return m_TemporalInterpolationCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int TemporalInterpolationCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_TemporalInterpolationCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *TemporalInterpolationCSInterface::GetDecoderStreamBuffer(void)
{
        return m_TemporalInterpolationCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int TemporalInterpolationCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_TemporalInterpolationCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
TemporalInterpolationDescriptorInterfaceABC *TemporalInterpolationCSInterface::
GetDescriptorInterface(void)
{
        return m_TemporalInterpolationCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int TemporalInterpolationCSInterface::
SetDescriptorInterface(TemporalInterpolationDescriptorInterfaceABC
		       *aTemporalInterpolationDescriptorInterface)
{
        return m_TemporalInterpolationCS->
	  SetDescriptorInterface(aTemporalInterpolationDescriptorInterface);
}

//----------------------------------------------------------------------------
int TemporalInterpolationCSInterface::StartEncode(void)
{
        return m_TemporalInterpolationCS->StartEncode();
}

//----------------------------------------------------------------------------
int TemporalInterpolationCSInterface::StartDecode(void)
{
        return m_TemporalInterpolationCS->StartDecode();
}

//----------------------------------------------------------------------------
void TemporalInterpolationCSInterface::destroy(void)
{ delete m_TemporalInterpolationCS; }

//=============================================================================

TemporalInterpolationCS::TemporalInterpolationCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_TemporalInterpolationDescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TemporalInterpolationCS::TemporalInterpolationCS(TemporalInterpolationDescriptorInterfaceABC *TemporalInterpolation):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_TemporalInterpolationDescriptorInterface(0)
{
	/* create descriptor if it does not exist*/
	if (!TemporalInterpolation) {
	  TemporalInterpolationDescriptor *descriptor =
	    new TemporalInterpolationDescriptor();
	  TemporalInterpolation=descriptor->GetInterface();
	}

	/* connect descritor with coding schemes*/
	SetDescriptorInterface(TemporalInterpolation);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TemporalInterpolationCS::~TemporalInterpolationCS()
{
  if (m_TemporalInterpolationDescriptorInterface)
    m_TemporalInterpolationDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& TemporalInterpolationCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TemporalInterpolationCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& TemporalInterpolationCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* TemporalInterpolationCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool TemporalInterpolationCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * TemporalInterpolationCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int TemporalInterpolationCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * TemporalInterpolationCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int TemporalInterpolationCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
TemporalInterpolationDescriptorInterfaceABC *TemporalInterpolationCS::GetDescriptorInterface()
{
        return m_TemporalInterpolationDescriptorInterface;
}

//----------------------------------------------------------------------------
int TemporalInterpolationCS::
SetDescriptorInterface(TemporalInterpolationDescriptorInterfaceABC
	      *aTemporalInterpolationDescriptorInterface)
{
    /* check if new value is different from old value*/
    if (m_TemporalInterpolationDescriptorInterface == 
	    aTemporalInterpolationDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_TemporalInterpolationDescriptorInterface) 
	  m_TemporalInterpolationDescriptorInterface->release();

	/* add new interface*/
	m_TemporalInterpolationDescriptorInterface = 
	  aTemporalInterpolationDescriptorInterface;
	if (m_TemporalInterpolationDescriptorInterface) {
	  m_TemporalInterpolationDescriptorInterface->addref();
	}
	else {
	  return -1;
	}

	return 0;
}

//----------------------------------------------------------------------------
unsigned long TemporalInterpolationCS::EncodeTimePoint(double t)
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
unsigned long TemporalInterpolationCS::EncodeDuration(double t)
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
double TemporalInterpolationCS::DecodeTimePoint()
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
double TemporalInterpolationCS::DecodeDuration()
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
int TemporalInterpolationCS::StartEncode(void)
{
    unsigned long a, b, i, j, Nkey, Dim;
	double c;
	float f;
	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_EncoderBuffer || !m_TemporalInterpolationDescriptorInterface) {
	  return -1;
	}

	Nkey = m_TemporalInterpolationDescriptorInterface->GetNumOfKeyPoint();
	m_EncoderBuffer->PutBitsLong(Nkey,16);
	a = m_TemporalInterpolationDescriptorInterface->GetConstantTimeInterval();
	m_EncoderBuffer->PutBitsLong(a,1);
	if (a==1) {		// ConstantTimeInterval=1
		a = m_TemporalInterpolationDescriptorInterface->GetWholeIntervalDataType();
		m_EncoderBuffer->PutBitsLong(a,1);	// WholeIntervalDataType
		c = m_TemporalInterpolationDescriptorInterface->GetMediaDuration();
		EncodeDuration(c);
	} else {	// ConstantTimeInterval=0
		a = m_TemporalInterpolationDescriptorInterface->GetMediaTimePointDataType();
		m_EncoderBuffer->PutBitsLong(a,2);	// KeyTimePointDataType	

		for (i=0; i<Nkey; i++) {
			c = m_TemporalInterpolationDescriptorInterface->GetMediaTimePoint(i);
			EncodeTimePoint(c);
		}
	}

	Dim = m_TemporalInterpolationDescriptorInterface->GetDimension();
	m_EncoderBuffer->PutBitsLong(Dim,4);

	for (i=0; i<Dim; i++) {
		a = m_TemporalInterpolationDescriptorInterface->GetDefaultFunction(i);
		m_EncoderBuffer->PutBitsLong(a,1);	
		if( a == 0 ){	// not default functions

			for (j=0; j<Nkey; j++) {
				if (j==0) {			// Start Point
					m_EncoderBuffer->PutBitsLong(0,2);
				} else {
					b = m_TemporalInterpolationDescriptorInterface->GetType(i,j-1);
					m_EncoderBuffer->PutBitsLong(b,2);
					if (b == 2) {
						c = m_TemporalInterpolationDescriptorInterface->GetParam(i,j-1);
						f = (float)c;
						m_EncoderBuffer->PutBits((char *)(&f),8*sizeof(float));		
					}
				}
				c = m_TemporalInterpolationDescriptorInterface->GetKeyValue(i,j);
				f = (float)c;
				m_EncoderBuffer->PutBits((char *)(&f),8*sizeof(float));
			}

		} else {		// default functions

			for (j=0; j<Nkey; j++) {
				c = m_TemporalInterpolationDescriptorInterface->GetKeyValue(i,j);
				f = (float)c;
				m_EncoderBuffer->PutBits((char *)(&f),8*sizeof(float));
			}
		}
	}

	return 0;
}

//----------------------------------------------------------------------------
int TemporalInterpolationCS::StartDecode(void)
{
    long a, b, i, j, Nkey, Dim;
	float c;

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_TemporalInterpolationDescriptorInterface) {
	  return -1;
	}

	m_DecoderBuffer->GetBitsLong(&Nkey,16);
	m_DecoderBuffer->TakeBits(16);
	m_TemporalInterpolationDescriptorInterface->SetNumOfKeyPoint(Nkey);
	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	m_TemporalInterpolationDescriptorInterface->SetConstantTimeInterval(a);
	if (a==1) {
		m_DecoderBuffer->GetBitsLong(&a,1);
		m_DecoderBuffer->TakeBits(1);
		m_TemporalInterpolationDescriptorInterface->SetWholeIntervalDataType(a);
		c = DecodeDuration();
		m_TemporalInterpolationDescriptorInterface->SetMediaDuration(c);		
	} else {
		m_DecoderBuffer->GetBitsLong(&a,2);
		m_DecoderBuffer->TakeBits(2);
		m_TemporalInterpolationDescriptorInterface->SetMediaTimePointDataType(a);
		for (i=0; i<Nkey; i++) {
			c = DecodeTimePoint();
			m_TemporalInterpolationDescriptorInterface->SetMediaTimePoint(i,c);
		}
	}

	m_DecoderBuffer->GetBitsLong(&Dim,4);
	m_DecoderBuffer->TakeBits(4);
	m_TemporalInterpolationDescriptorInterface->SetDimension(Dim);

	for (i=0; i<Dim; i++) {
		m_DecoderBuffer->GetBitsLong(&a,1);
		m_DecoderBuffer->TakeBits(1);
		m_TemporalInterpolationDescriptorInterface->SetDefaultFunction(i,a);
		if(a==0){		// not default functions

			for (j=0; j<Nkey; j++) {
				if (j==0) {			// Start Point
					m_DecoderBuffer->GetBitsLong(&b,2);
					m_DecoderBuffer->TakeBits(2);
					m_TemporalInterpolationDescriptorInterface->SetType(i,0,0);	
				} else {
					m_DecoderBuffer->GetBitsLong(&b,2);
					m_DecoderBuffer->TakeBits(2);
					m_TemporalInterpolationDescriptorInterface->SetType(i,j-1,b);	
					if (b == 2) {
						c = 0.0;
						m_DecoderBuffer->GetBits((char *)(&c),8*sizeof(float));
						m_DecoderBuffer->TakeBits(8*sizeof(float));
						m_TemporalInterpolationDescriptorInterface->SetParam(i,j-1,c);
					}
				}
				c = 0.0;
				m_DecoderBuffer->GetBits((char *)(&c),8*sizeof(float));
				m_DecoderBuffer->TakeBits(8*sizeof(float));
				m_TemporalInterpolationDescriptorInterface->SetKeyValue(i,j,c);
			}

		} else {		// default functions

			for (j=0; j<Nkey; j++) {
				if (j==0) {			// Start Point
					m_TemporalInterpolationDescriptorInterface->SetType(i,0,0);	
				} else {
					m_TemporalInterpolationDescriptorInterface->SetType(i,j-1,1);	
				}
				c = 0.0;
				m_DecoderBuffer->GetBits((char *)(&c),8*sizeof(float));
				m_DecoderBuffer->TakeBits(8*sizeof(float));
				m_TemporalInterpolationDescriptorInterface->SetKeyValue(i,j,c);
			} 

		}
	}

	return 0;
}

//----------------------------------------------------------------------------
TemporalInterpolationCSInterfaceABC *TemporalInterpolationCS::GetInterface(void)
{ return &m_Interface; }


