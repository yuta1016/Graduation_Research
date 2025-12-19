///////////////////////////////////////////////////////////////////////////////
//
// this software module was originally developed by
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ColorLayoutCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef WIN32
#include <netinet/in.h>
#endif

using namespace XM;

const UUID ColorLayoutCSInterface::myID = UUID("");
const char *ColorLayoutCSInterface::myName =
                         "ColorLayoutCodingEngineInterface";

const UUID ColorLayoutCS::myID = UUID("");
const char *ColorLayoutCS::myName = "ColorLayoutCodingScheme";

const UUID ColorLayoutCS::valID = UUID("");
const char *ColorLayoutCS::valName = "";

const UUID ColorLayoutCSInterfaceABC::myID = UUID("");

ColorLayoutCSInterfaceABC::ColorLayoutCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
ColorLayoutCSInterface::ColorLayoutCSInterface(ColorLayoutCS
						     *aColorLayout):
m_ColorLayoutCS(aColorLayout)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ColorLayoutCSInterface::~ColorLayoutCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& ColorLayoutCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorLayoutCSInterface::GetName(void)
{ return myName; }

// start modification for version 3.0
//----------------------------------------------------------------------------
void ColorLayoutCSInterface::destroy(void)
{ delete m_ColorLayoutCS; }
// end modification for version 3.0

//----------------------------------------------------------------------------
BitBuffer *ColorLayoutCSInterface::GetEncoderStreamBuffer(void)
{
        return m_ColorLayoutCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ColorLayoutCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ColorLayoutCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *ColorLayoutCSInterface::GetDecoderStreamBuffer(void)
{
        return m_ColorLayoutCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ColorLayoutCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ColorLayoutCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
ColorLayoutDescriptorInterfaceABC
*ColorLayoutCSInterface::GetDescriptorInterface(void)

{
        return m_ColorLayoutCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ColorLayoutCSInterface::
SetDescriptorInterface(ColorLayoutDescriptorInterfaceABC
		       *aColorLayoutDescriptorInterface)
{
        return m_ColorLayoutCS->
	  SetDescriptorInterface(aColorLayoutDescriptorInterface);
}

//----------------------------------------------------------------------------
int ColorLayoutCSInterface::StartEncode(void)
{
        return m_ColorLayoutCS->StartEncode();
}
//----------------------------------------------------------------------------
int ColorLayoutCSInterface::StartDecode(void)
{
        return m_ColorLayoutCS->StartDecode();
}
//=============================================================================

ColorLayoutCS::ColorLayoutCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_ColorLayoutDescriptorInterface(0)
{
	static int ctable[8]={ 1, 3, 6, 10, 15, 21, 28, 64 };
	for(int i=0; i<8; i++) code_table[i]=ctable[i];
	SetExposedInterface(&m_Interface);
}
//----------------------------------------------------------------------------
// start modification for version 3.0
ColorLayoutCS::ColorLayoutCS(ColorLayoutDescriptorInterfaceABC
				   *colorlayout):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_ColorLayoutDescriptorInterface(0)
{
	/* create descriptor if it does not exist*/
	if (!colorlayout) {
	  ColorLayoutDescriptor *descriptor =
	    new ColorLayoutDescriptor();
	  colorlayout=descriptor->GetInterface();
	}

	/* connect descritors with coding schemes*/
	SetDescriptorInterface(colorlayout);

	//sth was this missing here?
	static int ctable[8]={ 1, 3, 6, 10, 15, 21, 28, 64 };
	for(int i=0; i<8; i++) code_table[i]=ctable[i];

	SetExposedInterface(&m_Interface);
}
// end modification for version 3.0
//----------------------------------------------------------------------------
ColorLayoutCS::~ColorLayoutCS()
{
	// start modification for version 3.0
	/* release descriptor*/
	if (m_ColorLayoutDescriptorInterface)
	  m_ColorLayoutDescriptorInterface->release();
	// end modification for version 3.0
}

//----------------------------------------------------------------------------
const UUID& ColorLayoutCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorLayoutCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ColorLayoutCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ColorLayoutCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ColorLayoutCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * ColorLayoutCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int ColorLayoutCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * ColorLayoutCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int ColorLayoutCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
ColorLayoutDescriptorInterfaceABC *ColorLayoutCS::
GetDescriptorInterface(void)
{
       return m_ColorLayoutDescriptorInterface;
}

//----------------------------------------------------------------------------
int ColorLayoutCS::
SetDescriptorInterface(ColorLayoutDescriptorInterfaceABC
		       *aColorLayoutDescriptorInterface)
{
	// start modification for version 3.0
        /* check if new value is different from old value*/
        if (m_ColorLayoutDescriptorInterface == 
	    aColorLayoutDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_ColorLayoutDescriptorInterface) 
	  m_ColorLayoutDescriptorInterface->release();

	/* add new interface*/
	m_ColorLayoutDescriptorInterface = 
	  aColorLayoutDescriptorInterface;
	if (m_ColorLayoutDescriptorInterface) {
	  m_ColorLayoutDescriptorInterface->addref();
	}
	else {
	  /* if no descriptor, release also descriptor interfaces from
	     sub coding schemes*/
	  return -1;
	}
	
	
//        if (!aColorLayoutDescriptorInterface) return -1;
//
//        m_ColorLayoutDescriptorInterface =
//	  aColorLayoutDescriptorInterface;
	return 0;
}

//----------------------------------------------------------------------------
int ColorLayoutCS::StartEncode(void)
{
    int a, b;
    unsigned long ax, bx;
    if (!m_EncoderBuffer) {
        return -1;
    }

    /* check if descriptor and stream buffer are linked
       to coding scheme*/
    if (!m_EncoderBuffer || !m_ColorLayoutDescriptorInterface) {
        return -1;
    }

    a=m_ColorLayoutDescriptorInterface->GetNumberOfYCoeff();
    b=m_ColorLayoutDescriptorInterface->GetNumberOfCCoeff();
    if((a==6)&&(b==3)){
	ax=2; bx=1;
        m_EncoderBuffer->PutBitsLong(0, 1UL);
// start change 2000/10/27 for CD
    } else if((a==6)&&(b==6)){
      ax=2; bx=2;
      m_EncoderBuffer->PutBitsLong(2, 2UL);
      // end change 2000/10/27 for CD
    } else {
        m_EncoderBuffer->PutBitsLong(3, 2UL);
        if(a>28) ax=7;
	else if(a>21) ax=6;
	else if(a>15) ax=5;
	else if(a>10) ax=4;
	else if(a>6) ax=3;
	else if(a>3) ax=2;
	else ax=1;
	if(b>28) bx=7;
	else if(b>21) bx=6;
	else if(b>15) bx=5;
	else if(b>10) bx=4;
	else if(b>6) bx=3;
	else if(b>3) bx=2;
	else bx=1;
	m_EncoderBuffer->PutBitsLong(ax, 3UL);
	m_EncoderBuffer->PutBitsLong(bx, 3UL);
    }

    int i;
    int *y, *cb, *cr;

    y=m_ColorLayoutDescriptorInterface->GetYCoeff();
    cb=m_ColorLayoutDescriptorInterface->GetCbCoeff();
    cr=m_ColorLayoutDescriptorInterface->GetCrCoeff();

    m_EncoderBuffer->PutBitsLong((long)(y[0]), 6UL);
    m_EncoderBuffer->PutBitsLong((long)(cb[0]), 6UL);
    m_EncoderBuffer->PutBitsLong((long)(cr[0]), 6UL);

    for(i=1; i<a; i++) m_EncoderBuffer->PutBitsLong((long)(y[i]), 5UL);
    for(; i<code_table[ax]; i++) m_EncoderBuffer->PutBitsLong(16, 5UL);

    for(i=1; i<b; i++) m_EncoderBuffer->PutBitsLong((long)(cb[i]), 5UL);
    for(; i<code_table[bx]; i++) m_EncoderBuffer->PutBitsLong(16, 5UL);

    for(i=1; i<b; i++) m_EncoderBuffer->PutBitsLong((long)(cr[i]), 5UL);
    for(; i<code_table[bx]; i++) m_EncoderBuffer->PutBitsLong(16, 5UL);

    return 0;
}

//----------------------------------------------------------------------------
int ColorLayoutCS::StartDecode(void)
{
    long a;
    int ax, bx;
	int *YCoeff;
	int *CbCoeff;
	int *CrCoeff;

    if (!m_DecoderBuffer) {
	return -1;
    }

    /* check if descriptor and stream buffer are linked
	to coding scheme*/
    if (!m_DecoderBuffer || !m_ColorLayoutDescriptorInterface) {
	return -1;
    }

    m_DecoderBuffer->GetBitsLong(&a,1UL);
    m_DecoderBuffer->TakeBits(1UL);
    if(a==0){
	m_ColorLayoutDescriptorInterface->SetNumberOfYCoeff(ax=6);
	m_ColorLayoutDescriptorInterface->SetNumberOfCCoeff(bx=3);
// start change 2000/10/27 for CD
    } else {
      m_DecoderBuffer->GetBitsLong(&a, 1UL);
      m_DecoderBuffer->TakeBits(1UL);
      if(a==0){
	m_ColorLayoutDescriptorInterface->SetNumberOfYCoeff(ax=6);
	m_ColorLayoutDescriptorInterface->SetNumberOfCCoeff(bx=6);
// end change 2000/10/27 for CD
      } else {
	m_DecoderBuffer->GetBitsLong(&a,3UL);
	m_DecoderBuffer->TakeBits(3UL);
	m_ColorLayoutDescriptorInterface->SetNumberOfYCoeff(ax=code_table[a]);
	m_DecoderBuffer->GetBitsLong(&a,3UL);
	m_DecoderBuffer->TakeBits(3UL);
	m_ColorLayoutDescriptorInterface->SetNumberOfCCoeff(bx=code_table[a]);
      }
// start change 2000/10/27 for CD
    }
// end change 2000/10/27 for CD
// fprintf(stderr, "StartDecode:: NumberOfCoeff=%d %d\n", ax, bx);

    YCoeff= new int [ax];
    CbCoeff= new int [bx];
    CrCoeff= new int [bx];

    m_DecoderBuffer->GetBitsLong(&a,6UL);
    m_DecoderBuffer->TakeBits(6UL);
    YCoeff[0]=a;
    m_DecoderBuffer->GetBitsLong(&a,6UL);
    m_DecoderBuffer->TakeBits(6UL);
    CbCoeff[0]=a;
    m_DecoderBuffer->GetBitsLong(&a,6UL);
    m_DecoderBuffer->TakeBits(6UL);
    CrCoeff[0]=a;

    {
      int i;
      for(i=1; i<ax; i++){
	m_DecoderBuffer->GetBitsLong(&a,5UL);
	m_DecoderBuffer->TakeBits(5UL);
	YCoeff[i]=a;
      }
    }	
    m_ColorLayoutDescriptorInterface->SetYCoeff(YCoeff);

    {
      int i;
      for(i=1; i<bx; i++){
	m_DecoderBuffer->GetBitsLong(&a,5UL);
	m_DecoderBuffer->TakeBits(5UL);
	CbCoeff[i]=a;
      }
    }
    m_ColorLayoutDescriptorInterface->SetCbCoeff(CbCoeff);

    {
      int i;
      for(i=1; i<bx; i++){
	m_DecoderBuffer->GetBitsLong(&a,5UL);
	m_DecoderBuffer->TakeBits(5UL);
	CrCoeff[i]=a;
      }
    }
    m_ColorLayoutDescriptorInterface->SetCrCoeff(CrCoeff);
    
    delete [] YCoeff;
    delete [] CbCoeff;
    delete [] CrCoeff;
	 
    return 0;
}

//----------------------------------------------------------------------------
ColorLayoutCSInterfaceABC *ColorLayoutCS::GetInterface(void)
{ return &m_Interface; }



