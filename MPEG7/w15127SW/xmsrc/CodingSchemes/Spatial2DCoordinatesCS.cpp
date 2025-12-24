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
// Applicable File Name:  Spatial2DCoordinatesCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <sys/types.h>
#ifndef WIN32
#include <netinet/in.h>
#endif
 

using namespace XM;

const UUID Spatial2DCoordinatesCSInterface::myID = UUID("");
const char *Spatial2DCoordinatesCSInterface::myName = "Spatial2DCoordinatestemCodingEngineInterface";

const UUID Spatial2DCoordinatesCS::myID = UUID("");
const char *Spatial2DCoordinatesCS::myName = "Spatial2DCoordinatestemCodingScheme";

const UUID Spatial2DCoordinatesCS::valID = UUID("");
const char *Spatial2DCoordinatesCS::valName = "";

const UUID Spatial2DCoordinatesCSInterfaceABC::myID = UUID("");

Spatial2DCoordinatesCSInterfaceABC::Spatial2DCoordinatesCSInterfaceABC()

{
  SetInterfaceID(myID);
}

//=============================================================================
Spatial2DCoordinatesCSInterface::Spatial2DCoordinatesCSInterface(Spatial2DCoordinatesCS *aSpatial2DCoordinates):
m_Spatial2DCoordinatesCS(aSpatial2DCoordinates)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
Spatial2DCoordinatesCSInterface::~Spatial2DCoordinatesCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& Spatial2DCoordinatesCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Spatial2DCoordinatesCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
BitBuffer *Spatial2DCoordinatesCSInterface::GetEncoderStreamBuffer(void)
{
        return m_Spatial2DCoordinatesCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int Spatial2DCoordinatesCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_Spatial2DCoordinatesCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *Spatial2DCoordinatesCSInterface::GetDecoderStreamBuffer(void)
{
        return m_Spatial2DCoordinatesCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int Spatial2DCoordinatesCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_Spatial2DCoordinatesCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
Spatial2DCoordinatesDescriptorInterfaceABC *Spatial2DCoordinatesCSInterface::
GetDescriptorInterface(void)
{
        return m_Spatial2DCoordinatesCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int Spatial2DCoordinatesCSInterface::
SetDescriptorInterface(Spatial2DCoordinatesDescriptorInterfaceABC
		       *aDescriptorInterface)
{
        return m_Spatial2DCoordinatesCS->
	  SetDescriptorInterface(aDescriptorInterface);
}

//----------------------------------------------------------------------------
int Spatial2DCoordinatesCSInterface::StartEncode(void)
{
        return m_Spatial2DCoordinatesCS->StartEncode();
}

//----------------------------------------------------------------------------
int Spatial2DCoordinatesCSInterface::StartDecode(void)
{
        return m_Spatial2DCoordinatesCS->StartDecode();
}


//----------------------------------------------------------------------------
void Spatial2DCoordinatesCSInterface::destroy(void)
{ delete m_Spatial2DCoordinatesCS; }

//=============================================================================

Spatial2DCoordinatesCS::Spatial2DCoordinatesCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
Spatial2DCoordinatesCS::Spatial2DCoordinatesCS(Spatial2DCoordinatesDescriptorInterfaceABC *Spatial2DCoordinates):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{
	/* create descriptor if it does not exist*/
	if (!Spatial2DCoordinates) {
	  Spatial2DCoordinatesDescriptor *descriptor =
	    new Spatial2DCoordinatesDescriptor();
	  Spatial2DCoordinates=descriptor->GetInterface();
	}

	/* connect descritor with coding schemes*/
	SetDescriptorInterface(Spatial2DCoordinates);
	
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
Spatial2DCoordinatesCS::~Spatial2DCoordinatesCS()
{
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& Spatial2DCoordinatesCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Spatial2DCoordinatesCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& Spatial2DCoordinatesCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* Spatial2DCoordinatesCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool Spatial2DCoordinatesCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * Spatial2DCoordinatesCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int Spatial2DCoordinatesCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * Spatial2DCoordinatesCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int Spatial2DCoordinatesCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
Spatial2DCoordinatesDescriptorInterfaceABC *Spatial2DCoordinatesCS::GetDescriptorInterface()
{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int Spatial2DCoordinatesCS::
SetDescriptorInterface(Spatial2DCoordinatesDescriptorInterfaceABC
	      *aDescriptorInterface)
{
    /* check if new value is different from old value*/
    if (m_DescriptorInterface == 
	    aDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_DescriptorInterface) 
	  m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = 
	  aDescriptorInterface;
	if (m_DescriptorInterface) {
	  m_DescriptorInterface->addref();
	}
	else {
	  return -1;
	}

	return 0;

}

//----------------------------------------------------------------------------
int Spatial2DCoordinatesCS::StartEncode(void)
{
    unsigned long a, b, e, g, i, j, type;
	double c;
	char *d;
	float fl;
	unsigned long xR,yR;

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_EncoderBuffer || !m_DescriptorInterface) {
	  return -1;
	}

	d = m_DescriptorInterface->Getid();
	m_EncoderBuffer->PutBits(d, 255*8);
	xR = m_DescriptorInterface->GetxRepr();
	m_EncoderBuffer->PutBitsLong(xR,8);
	yR = m_DescriptorInterface->GetyRepr();
	m_EncoderBuffer->PutBitsLong(yR,8);
	a = m_DescriptorInterface->GetxSrcSize();
	m_EncoderBuffer->PutBitsLong(a,8);
	a = m_DescriptorInterface->GetySrcSize();
	m_EncoderBuffer->PutBitsLong(a,8);

	a = m_DescriptorInterface->GetUnitDefined();
	m_EncoderBuffer->PutBitsLong(a,1);
	if (a != 0) {
		b = m_DescriptorInterface->GetUnit();
		m_EncoderBuffer->PutBitsLong(b,3);
	}
	
	a = m_DescriptorInterface->GetLocalCoordinatesDefined();
	m_EncoderBuffer->PutBitsLong(a,1);
	if (a != 0) {
		b = m_DescriptorInterface->GetNameLength();
		m_EncoderBuffer->PutBitsLong(b,8);
		d = m_DescriptorInterface->Getname();
		m_EncoderBuffer->PutBits(d, b*8);
		b = m_DescriptorInterface->GetDataSetDefined();
		m_EncoderBuffer->PutBitsLong(b,1);
		if (b != 0) {
			b = m_DescriptorInterface->GetDataSetLength();
			m_EncoderBuffer->PutBitsLong(b,8);
			d = m_DescriptorInterface->GetdataSet();
			m_EncoderBuffer->PutBits(d, b*8);
		}
		b = m_DescriptorInterface->GetNumOfPixels();
		m_EncoderBuffer->PutBitsLong(b,2);
		for (i=0; i<b; i++) {
			e = m_DescriptorInterface->GetsrcPixelX(i);
			m_EncoderBuffer->PutBitsLong(e,xR);
			e = m_DescriptorInterface->GetsrcPixelY(i);
			m_EncoderBuffer->PutBitsLong(e,yR);
			e = m_DescriptorInterface->GetCoord(i);
			m_EncoderBuffer->PutBitsLong(e,1);
			if (e == 0) {
				g = m_DescriptorInterface->GetPixelX(i);
				m_EncoderBuffer->PutBitsLong(g,16);
				g = m_DescriptorInterface->GetPixelY(i);
				m_EncoderBuffer->PutBitsLong(g,16);
			} else {
				c = m_DescriptorInterface->GetCoordPointX(i);
				fl=(float)c;
				m_EncoderBuffer->PutBits((char *)(&fl),8*sizeof(float));
				c = m_DescriptorInterface->GetCoordPointY(i);
				fl=(float)c;
				m_EncoderBuffer->PutBits((char *)(&fl),8*sizeof(float));
			}
		}
		b = m_DescriptorInterface->GetNumOfMappingFuncts();
		m_EncoderBuffer->PutBitsLong(b,2);
		for (i=0; i<b; i++) {
			e = m_DescriptorInterface->GetMappingFunctLength(i);
			m_EncoderBuffer->PutBitsLong(e,8);
			d = m_DescriptorInterface->GetMappingFunct(i);
			m_EncoderBuffer->PutBits(d, e*8);
		}
	}
	
	a = m_DescriptorInterface->GetIntegratedCoordinatesDefined();
	m_EncoderBuffer->PutBitsLong(a,1);
	if (a != 0) {
		type = m_DescriptorInterface->GetmodelType();
		m_EncoderBuffer->PutBitsLong(type,4);
		c = m_DescriptorInterface->GetxOrigin();
		fl=(float)c;
		m_EncoderBuffer->PutBits((char *)(&fl),8*sizeof(float));
		c = m_DescriptorInterface->GetyOrigin();
		fl=(float)c;
		m_EncoderBuffer->PutBits((char *)(&fl),8*sizeof(float));
		b = m_DescriptorInterface->GetnumOfMotionParameterSets();
		m_EncoderBuffer->PutBitsLong(b,16);
		for (i=0; i<b; i++) {
			e = m_DescriptorInterface->GetTimeIncr(i);
			m_EncoderBuffer->PutBitsLong(e,16);
			for (j=0; j<type; j++) {
				c = m_DescriptorInterface->GetMotionParameters(i,j);
				fl=(float)c;
				m_EncoderBuffer->PutBits((char *)(&fl),8*sizeof(float));
			}
		}
	}

	return 0;
}

//----------------------------------------------------------------------------
int Spatial2DCoordinatesCS::StartDecode(void)
{
    long a, b, e, i, j, type, xR, yR;
	float c;
	char d[256];

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_DescriptorInterface) {
	  return -1;
	}

	b=255;
	memset(d, 0, b+1);
	m_DecoderBuffer->GetBits(d,b*8);
	m_DecoderBuffer->TakeBits(b*8);
	m_DescriptorInterface->Setid(d);

	m_DecoderBuffer->GetBitsLong(&xR,8);
	m_DecoderBuffer->TakeBits(8);
	m_DescriptorInterface->SetxRepr(xR);
	m_DecoderBuffer->GetBitsLong(&yR,8);
	m_DecoderBuffer->TakeBits(8);
	m_DescriptorInterface->SetyRepr(yR);

	m_DecoderBuffer->GetBitsLong(&a,8);
	m_DecoderBuffer->TakeBits(8);
	m_DescriptorInterface->SetxSrcSize(a);
	m_DecoderBuffer->GetBitsLong(&a,8);
	m_DecoderBuffer->TakeBits(8);
	m_DescriptorInterface->SetySrcSize(a);

	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	m_DescriptorInterface->SetUnitDefined(a);
	if (a != 0) {
		m_DecoderBuffer->GetBitsLong(&b,3);
		m_DecoderBuffer->TakeBits(3);
		m_DescriptorInterface->SetUnit(b);
	}		
	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	m_DescriptorInterface->SetLocalCoordinatesDefined(a);
	if (a != 0) {
		m_DecoderBuffer->GetBitsLong(&b,8);
		m_DecoderBuffer->TakeBits(8);
		m_DescriptorInterface->SetNameLength(b);
		memset(d, 0, b+1);
		m_DecoderBuffer->GetBits(d,b*8);
		m_DecoderBuffer->TakeBits(b*8);
		m_DescriptorInterface->Setname(d);
		m_DecoderBuffer->GetBitsLong(&b,1);
		m_DecoderBuffer->TakeBits(1);
		m_DescriptorInterface->SetDataSetDefined(b);
		if (b != 0) {
			m_DecoderBuffer->GetBitsLong(&b,8);
			m_DecoderBuffer->TakeBits(8);
			m_DescriptorInterface->SetDataSetLength(b);
			memset(d, 0, b+1);
			m_DecoderBuffer->GetBits(d,b*8);
			m_DecoderBuffer->TakeBits(b*8);
			m_DescriptorInterface->SetdataSet(d);
		}
		m_DecoderBuffer->GetBitsLong(&b,2);
		m_DecoderBuffer->TakeBits(2);
		m_DescriptorInterface->SetNumOfPixels(b);
		for (i=0; i<b; i++) {
			m_DecoderBuffer->GetBitsLong(&e,xR);
			m_DecoderBuffer->TakeBits(xR);
			m_DescriptorInterface->SetsrcPixelX(i,e);
			m_DecoderBuffer->GetBitsLong(&e,yR);
			m_DecoderBuffer->TakeBits(yR);
			m_DescriptorInterface->SetsrcPixelY(i,e);
			m_DecoderBuffer->GetBitsLong(&e,1);
			m_DecoderBuffer->TakeBits(1);
			m_DescriptorInterface->SetCoord(i,e);
			if (e == 0) {
				m_DecoderBuffer->GetBitsLong(&e,16);
				m_DecoderBuffer->TakeBits(16);
				m_DescriptorInterface->SetPixelX(i,e);
				m_DecoderBuffer->GetBitsLong(&e,16);
				m_DecoderBuffer->TakeBits(16);
				m_DescriptorInterface->SetPixelY(i,e);
			} else {
				c = 0.0;
				m_DecoderBuffer->GetBits((char *)(&c),8*sizeof(float));
				m_DecoderBuffer->TakeBits(8*sizeof(float));
				m_DescriptorInterface->SetCoordPointX(i,c);
				c = 0.0;
				m_DecoderBuffer->GetBits((char *)(&c),8*sizeof(float));
				m_DecoderBuffer->TakeBits(8*sizeof(float));
				m_DescriptorInterface->SetCoordPointY(i,c);
			}
		}
		m_DecoderBuffer->GetBitsLong(&b,2);
		m_DecoderBuffer->TakeBits(2);
		m_DescriptorInterface->SetNumOfMappingFuncts(b);
		for (i=0; i<b; i++) {
			m_DecoderBuffer->GetBitsLong(&e,8);
			m_DecoderBuffer->TakeBits(8);
			m_DescriptorInterface->SetMappingFunctLength(i,e);
			memset(d, 0, e+1);
			m_DecoderBuffer->GetBits(d,e*8);
			m_DecoderBuffer->TakeBits(e*8);
			m_DescriptorInterface->SetMappingFunct(i,d);
		}
	}

	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	m_DescriptorInterface->SetIntegratedCoordinatesDefined(a);
	if (a != 0) {
		m_DecoderBuffer->GetBitsLong(&type,4);
		m_DecoderBuffer->TakeBits(4);
		m_DescriptorInterface->SetmodelType(type);
		c = 0.0;
		m_DecoderBuffer->GetBits((char *)(&c),8*sizeof(float));
		m_DecoderBuffer->TakeBits(8*sizeof(float));
		m_DescriptorInterface->SetxOrigin(c);
		c = 0.0;
		m_DecoderBuffer->GetBits((char *)(&c),8*sizeof(float));
		m_DecoderBuffer->TakeBits(8*sizeof(float));
		m_DescriptorInterface->SetyOrigin(c);
		m_DecoderBuffer->GetBitsLong(&b,16);
		m_DecoderBuffer->TakeBits(16);
		m_DescriptorInterface->SetnumOfMotionParameterSets(b);
		for (i=0; i<b; i++) {
			m_DecoderBuffer->GetBitsLong(&e,16);
			m_DecoderBuffer->TakeBits(16);
			m_DescriptorInterface->SetTimeIncr(i,e);
			for (j=0; j<type; j++) {
				c = 0.0;
				m_DecoderBuffer->GetBits((char *)(&c),8*sizeof(float));
				m_DecoderBuffer->TakeBits(8*sizeof(float));
				m_DescriptorInterface->SetMotionParameters(i,j,c);
			}
		}
	}
	return 0;
}

//----------------------------------------------------------------------------
Spatial2DCoordinatesCSInterfaceABC *Spatial2DCoordinatesCS::GetInterface(void)
{ return &m_Interface; }


