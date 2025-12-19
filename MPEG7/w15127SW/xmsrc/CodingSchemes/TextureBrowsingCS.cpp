///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Kapil Chhabra, Peng Wu, Xinding Sun, Dr. Hyundoo Shin, Prof. B.S. Manjunath
// University of California, Santa Barbara.
// Samsung, Yun Ju Yu, Yang Lim Choi.
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
// Applicable File Name:  TextureBrowsingCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef _VISUALC_
#include <netinet/in.h>
#endif
 

using namespace XM;

const UUID TextureBrowsingCSInterface::myID = UUID("");
const char *TextureBrowsingCSInterface::myName =
                         "TextureBrowsingCodingSchemeInterface";

// create a UUID for your type
const UUID TextureBrowsingCS::myID = UUID("to be created with xx");
const char *TextureBrowsingCS::myName = "TextureBrowsingCodingScheme";

const UUID TextureBrowsingCS::valID = UUID("");
const char *TextureBrowsingCS::valName = "";

const UUID TextureBrowsingCSInterfaceABC::myID = UUID("");

TextureBrowsingCSInterfaceABC::TextureBrowsingCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
TextureBrowsingCSInterface::TextureBrowsingCSInterface(TextureBrowsingCS *aTextureBrowsing):
m_TextureBrowsingCS(aTextureBrowsing)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TextureBrowsingCSInterface::~TextureBrowsingCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& TextureBrowsingCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TextureBrowsingCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void TextureBrowsingCSInterface::destroy(void)
{ delete m_TextureBrowsingCS; }

//----------------------------------------------------------------------------
BitBuffer *TextureBrowsingCSInterface::GetEncoderStreamBuffer(void)
{
  return m_TextureBrowsingCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int TextureBrowsingCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_TextureBrowsingCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *TextureBrowsingCSInterface::GetDecoderStreamBuffer(void)
{
  return m_TextureBrowsingCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int TextureBrowsingCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_TextureBrowsingCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
TextureBrowsingDescriptorInterfaceABC
*TextureBrowsingCSInterface::GetDescriptorInterface(void)

{
  return m_TextureBrowsingCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int TextureBrowsingCSInterface::
SetDescriptorInterface(TextureBrowsingDescriptorInterfaceABC
		       *aTextureBrowsingDescriptorInterface)
{
  return m_TextureBrowsingCS->
    SetDescriptorInterface(aTextureBrowsingDescriptorInterface);
}

//----------------------------------------------------------------------------
int TextureBrowsingCSInterface::StartEncode(void)
{
  return m_TextureBrowsingCS->StartEncode();
}

//----------------------------------------------------------------------------
int TextureBrowsingCSInterface::StartDecode(void)
{
  return m_TextureBrowsingCS->StartDecode();
}
//=============================================================================
TextureBrowsingCS::TextureBrowsingCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TextureBrowsingCS::TextureBrowsingCS(TextureBrowsingDescriptorInterfaceABC* aTextureBrowsing):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL)
{
  /* create descriptor if it does not exist*/
  if (!aTextureBrowsing) {
    TextureBrowsingDescriptor *descriptor = 
      new TextureBrowsingDescriptor;
    aTextureBrowsing=descriptor->GetInterface();
  }

  /* connect descritors with CS*/
  SetDescriptorInterface(aTextureBrowsing);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TextureBrowsingCS::~TextureBrowsingCS()
{
  /* release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();

}

//----------------------------------------------------------------------------
const UUID& TextureBrowsingCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TextureBrowsingCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& TextureBrowsingCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* TextureBrowsingCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool TextureBrowsingCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * TextureBrowsingCS::GetEncoderStreamBuffer(void)
{
  return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int TextureBrowsingCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_EncoderBuffer = aBuffer;

  return 0;
}

//----------------------------------------------------------------------------
BitBuffer * TextureBrowsingCS::GetDecoderStreamBuffer(void)
{
  return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int TextureBrowsingCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_DecoderBuffer = aBuffer;

  return 0;
}

//----------------------------------------------------------------------------
TextureBrowsingDescriptorInterfaceABC *TextureBrowsingCS::
GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int TextureBrowsingCS::
SetDescriptorInterface(TextureBrowsingDescriptorInterfaceABC
		       *aTextureBrowsingDescriptorInterface)
{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == 
      aTextureBrowsingDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) 
    m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = 
    aTextureBrowsingDescriptorInterface;
  if (m_DescriptorInterface)
    m_DescriptorInterface->addref();
  else
    return -1;

  return 0;
}

//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int TextureBrowsingCS::StartEncode(void)
{
  //long v;

  // Is the file IO set up? If not return


  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"TextureBrowsingCS::StartEncode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"TextureBrowsingCS::StartEncode: Interface not initialised\n");
    return -1;
  }

  /* style for puting the bits interpreted as some kind of data*/
  /* m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&v),8*sizeof(long));*/

  //---yjyu - 010218
  // 2013/07/26 - KW fixed
	long* temp;
	int component_number_flag, *pbc;
  
	temp =new long [5];
    
	component_number_flag = m_DescriptorInterface->GetComponentNumberFlag();
	pbc = m_DescriptorInterface->GetBrowsing_Component();

	temp[0] = (long)(pbc[0]-1);
	temp[1] = (long)pbc[1];	
	temp[2] = (long)(pbc[2]-1);	
	temp[3] = (long)pbc[3];	
	temp[4] = (long)(pbc[4]-1);

	//Storing Coponent Numer Flag if 0, 3 components; if 1, 5 components;
	if (component_number_flag != 0) 
	  m_EncoderBuffer->PutBitsLong((unsigned long)1,1UL);
	else 
	  m_EncoderBuffer->PutBitsLong((unsigned long)0,1UL);

	//the scheme stores the five components of the PBC(12 bits)
    m_EncoderBuffer->PutBitsLong(temp[0],2);//storing 2 bits of 1st component
	m_EncoderBuffer->PutBitsLong(temp[1],3);//storing 3 bits of 2nd component
	m_EncoderBuffer->PutBitsLong(temp[2],2);//storing 2 bits of 3rd component
	if (component_number_flag!=0) {
		m_EncoderBuffer->PutBitsLong(temp[3],3);//storing 3 bits of 4th component
		m_EncoderBuffer->PutBitsLong(temp[4],2);//storing 2 bits of 5th component
	}

	delete [] temp;

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int TextureBrowsingCS::StartDecode(void)
{
  //long v;

  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"TextureBrowsingCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"TextureBrowsingCS::StartDecode: Interface not initialised\n");
    return -1;
  }

//---yjyu - 010218 ychoi - 010601
	int *PBC;
	int flag ;
	long temp;
//	unsigned char NUM_CONFI;


	if (!m_DecoderBuffer || !m_DescriptorInterface) {
	  return -1;
	}

	PBC=new int [5];

	// 2013/07/26 - fix
    //flag = m_DecoderBuffer->GetBit(0)>>7;
    flag = m_DecoderBuffer->GetBit(0);
    m_DecoderBuffer->TakeBits(1);
	m_DescriptorInterface->SetComponentNumberFlag(flag);

    m_DecoderBuffer->GetBitsLong(&temp,2);
    m_DecoderBuffer->TakeBits(2);
	PBC[0] =(int)temp+1;

    m_DecoderBuffer->GetBitsLong(&temp,3);
    m_DecoderBuffer->TakeBits(3);
    PBC[1] = (int)temp;

    m_DecoderBuffer->GetBitsLong(&temp,2);
    m_DecoderBuffer->TakeBits(2);
    PBC[2] = (int)temp+1;

	if (flag != 0) {
	  m_DecoderBuffer->GetBitsLong(&temp,3);
      m_DecoderBuffer->TakeBits(3);
      PBC[3] = (int)temp;

	  m_DecoderBuffer->GetBitsLong(&temp,2);
      m_DecoderBuffer->TakeBits(2);
      PBC[4] = (int)temp+1;
	}
    else 
	  PBC[3]=PBC[4]=0;

	m_DescriptorInterface->SetBrowsing_Component(PBC);
    delete [] PBC;

  // All done
  return 0;
}

//----------------------------------------------------------------------------
TextureBrowsingCSInterfaceABC *TextureBrowsingCS::GetInterface(void)
{ return &m_Interface; }


