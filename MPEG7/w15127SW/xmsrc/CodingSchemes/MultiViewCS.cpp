///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// HHI
// K. Mueller
//
// Mitsubishi Electric - Infomation Technology Europe - Visual Information Laboratory
// James Cooper
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
// Copyright (c) 2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MultiViewCS.cpp
//
///////////////////////////////////////////////////////////////////////////////


#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <sys/types.h>
#ifndef _VISUALC_
#include <netinet/in.h>
#endif

using namespace XM;

const int COEFFSIZE=16;

const XM::UUID MultiViewCSInterface::myID = XM::UUID("");
//sth please remove spaces in strings
const char *MultiViewCSInterface::myName = "MultiView Coding Engine Interface";

const XM::UUID MultiViewCS::myID = XM::UUID("");
const char *MultiViewCS::myName = "MultiView Coding Scheme";

const XM::UUID MultiViewCS::valID = XM::UUID("");
const char *MultiViewCS::valName = "MultiView Coding Scheme";

const XM::UUID MultiViewCSInterfaceABC::myID = XM::UUID("");

//sth delimiters :-(
//#===---------------------------------
//#===--- MultiViewCSInterfaceABC

MultiViewCSInterfaceABC::MultiViewCSInterfaceABC()
{
  SetInterfaceID(myID);
}

//#===---------------------------------
MultiViewCSInterfaceABC *MultiViewCS::GetInterface(void)
{
  return &m_Interface;
}

//#===------------------------------
//#===--- MultiViewCSInterface

//#===---------------------------------
MultiViewCSInterface::~MultiViewCSInterface()
{
}

//#===---------------------------------
MultiViewCSInterface::MultiViewCSInterface(MultiViewCS *pMultiView) :
m_pMultiViewCS(pMultiView)
{
  SetInterfaceID(myID);
}

//#===---------------------------------
const XM::UUID& MultiViewCSInterface::GetInterfaceID(void)
{
  return myID;
}

//#===---------------------------------
const char *MultiViewCSInterface::GetName(void)
{
  return myName;
}

//----------------------------------------------------------------------------
void MultiViewCSInterface::destroy(void)
{ delete m_pMultiViewCS; }

//#===---------------------------------
BitBuffer *MultiViewCSInterface::GetEncoderStreamBuffer()
{
  return m_pMultiViewCS->GetEncoderStreamBuffer();
}

//#===---------------------------------
int MultiViewCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_pMultiViewCS->SetEncoderStreamBuffer(aBuffer);
}

//#===---------------------------------
BitBuffer *MultiViewCSInterface::GetDecoderStreamBuffer()
{
  return m_pMultiViewCS->GetDecoderStreamBuffer();
}

//#===---------------------------------
int MultiViewCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_pMultiViewCS->SetDecoderStreamBuffer(aBuffer);
}

//#===---------------------------------
MultiViewDescriptorInterfaceABC *MultiViewCSInterface::GetDescriptorInterface(void)
{
  return m_pMultiViewCS->GetDescriptorInterface();
}

//#===---------------------------------
int MultiViewCSInterface::SetDescriptorInterface(MultiViewDescriptorInterfaceABC 
																							   *pMultiViewDescriptorInterface)
{
  return m_pMultiViewCS->SetDescriptorInterface(pMultiViewDescriptorInterface);
}

//----------------------------------------------------------------------------
SUB_CS_INTERFACE_ABC *MultiViewCSInterface::Get2DCSInterface()
{
        return m_pMultiViewCS->Get2DCSInterface();
}

//----------------------------------------------------------------------------
void MultiViewCSInterface::Set2DCSInterface(SUB_CS_INTERFACE_ABC *a2DCSInterface)
{
  m_pMultiViewCS->Set2DCSInterface(a2DCSInterface);
}

//----------------------------------------------------------------------------
int MultiViewCSInterface::StartEncode(void)
{
        return m_pMultiViewCS->StartEncode();
}
//----------------------------------------------------------------------------
int MultiViewCSInterface::StartDecode(void)
{
        return m_pMultiViewCS->StartDecode();
}


//#===---------------------------------
//#===---------------------
//#===--- MultiViewCS
MultiViewCS::MultiViewCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_pMultiViewDescriptorInterface(0),
m_pShape2DCSInterface(0)
{	
	SUB_CS *m_2DCS;

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	m_2DCS = new SUB_CS();
	// connect sub coding schemes with 3D coding scheme
	if (m_2DCS)
		Set2DCSInterface(m_2DCS->GetInterface());

  SetExposedInterface(&m_Interface);

}
//=============================================================================

MultiViewCS::MultiViewCS(MultiViewDescriptorInterfaceABC *MultiView):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_pMultiViewDescriptorInterface(0),
m_pShape2DCSInterface(0)
{

	SUB_CS *m_2DCS;

#ifdef VERBOSE
        fprintf(stderr,"Connect descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!MultiView) {
	  MultiViewDescriptor *descriptor =  new MultiViewDescriptor(0);
	  MultiView = descriptor->GetInterface();
	}

	// connect descritors with coding schemes
	SetDescriptorInterface(MultiView);

	/* create sub coding schemes and
	   connect them with descriptor if availabe*/
	if (MultiView->Get2DDescriptorInterface(0)) 
		// create sub coding schemes
	  //sth (0) expected in else section
		m_2DCS = new SUB_CS(MultiView->Get2DDescriptorInterface(0));
	else 
		m_2DCS = new SUB_CS();

	// connect sub coding scheme with 3D coding scheme
	if (m_2DCS)
		Set2DCSInterface(m_2DCS->GetInterface());
	

	SetExposedInterface(&m_Interface);

}

//#===---------------------------------
MultiViewCS::~MultiViewCS()
{
  // destroy sub coding schemes
	if (m_pShape2DCSInterface) m_pShape2DCSInterface->destroy();

	/* release descriptor*/
	if (m_pMultiViewDescriptorInterface)  
		m_pMultiViewDescriptorInterface->release();

}

//#===---------------------------------
const XM::UUID& MultiViewCS::GetObjectID(void)
{
  return myID;
}

//#===---------------------------------
const char *MultiViewCS::GetName(void)
{
  return myName;
}

//#===---------------------------------
const XM::UUID& MultiViewCS::GetValueID(void)
{
  return valID;
}

//#===---------------------------------
const char* MultiViewCS::GetValueName(void)
{
  return valName;
}

//#===---------------------------------
bool MultiViewCS::IsProprietary(void)
{
  return m_isProprietary;
}

//#===---------------------------------
BitBuffer *MultiViewCS::GetEncoderStreamBuffer()
{
  return m_EncoderBuffer;
}

//#===---------------------------------
int MultiViewCS::SetEncoderStreamBuffer(BitBuffer *pBuffer)
{
  if (!pBuffer) return -1;
	// if sub coding schemes exist apply stream also to them
	for( int i = 0; i < TotNumberOfViews; i++ ) 
	{
		m_pShape2DCSInterface->SetEncoderStreamBuffer(pBuffer);
	}

  m_EncoderBuffer = pBuffer;
  return 0;
}

//#===---------------------------------
BitBuffer *MultiViewCS::GetDecoderStreamBuffer()
{
  return m_DecoderBuffer;
}

//#===---------------------------------
int MultiViewCS::SetDecoderStreamBuffer(BitBuffer *pBuffer)
{
  if (!pBuffer) return -1;
	// if sub coding schemes exist apply stream also to them
	for( int i = 0; i < TotNumberOfViews; i++ ) 
	{
		m_pShape2DCSInterface->SetDecoderStreamBuffer(pBuffer);
	}

  m_DecoderBuffer = pBuffer;
  return 0;
}

//#===---------------------------------
MultiViewDescriptorInterfaceABC *MultiViewCS::GetDescriptorInterface(void)
{
  return m_pMultiViewDescriptorInterface;
}

//#===---------------------------------
int MultiViewCS::SetDescriptorInterface(MultiViewDescriptorInterfaceABC
																				 *aMultiViewDescriptorInterface)
{
	// check if new value is different from old value
	if (m_pMultiViewDescriptorInterface == aMultiViewDescriptorInterface) 
		return 0;
	
	// release old interface
	if (m_pMultiViewDescriptorInterface) 
		m_pMultiViewDescriptorInterface->release();
	
	// add new interface
	m_pMultiViewDescriptorInterface = aMultiViewDescriptorInterface;
	if (m_pMultiViewDescriptorInterface)
	{
		m_pMultiViewDescriptorInterface->addref();
		
		// conect sub descriptors with sub coding scheme
		if ((m_pMultiViewDescriptorInterface->Get2DDescriptorInterface(0)) &&	m_pShape2DCSInterface)
			m_pShape2DCSInterface->
				SetDescriptorInterface(m_pMultiViewDescriptorInterface->Get2DDescriptorInterface(0));
	}
	else 
	{
	  // if no descriptor, release also descriptor interfaces from sub coding schemes
		for( int i = 0; i < TotNumberOfViews; i++ )
			m_pShape2DCSInterface->SetDescriptorInterface(0);

		return -1;
	}
	
	
	return 0;
}

//----------------------------------------------------------------------------
SUB_CS_INTERFACE_ABC *MultiViewCS::Get2DCSInterface()
{
  return m_pShape2DCSInterface;
}

//----------------------------------------------------------------------------
void MultiViewCS::
Set2DCSInterface(SUB_CS_INTERFACE_ABC  *a2DCSInterface)
{
  m_pShape2DCSInterface = a2DCSInterface;
}

//----------------------------------------------------------------------------
int MultiViewCS::StartEncode(void)
{
	int i;

	if (!m_EncoderBuffer)  return -1;

	// check if descriptor and stream buffer are linked to coding scheme
	if (!m_EncoderBuffer || !m_pMultiViewDescriptorInterface)  return -1;

	// DescriptorID = 6 for ContourShape
	m_EncoderBuffer->PutBitsLong(6 , 8);

	// output the fixed views flag
	m_EncoderBuffer->PutBitsLong((long)m_pMultiViewDescriptorInterface->GetFixedViewsFlag(), 1);

	// output the number of views
	m_EncoderBuffer->PutBitsLong((long)m_pMultiViewDescriptorInterface->GetNoOfViews()-1, 4);

	// output the 2d descritors
	for( i = 0; i < m_pMultiViewDescriptorInterface->GetNoOfViews(); i++ ) 
	{
		// check if shape 3D descriptor is linked 2D
		if (!m_pMultiViewDescriptorInterface->Get2DDescriptorInterface(i)) return -1;

		// check if 2D descriptor and stream buffer are linked to 2D coding scheme
		if(!m_pShape2DCSInterface->GetDescriptorInterface() || 
			 !m_pShape2DCSInterface->GetEncoderStreamBuffer()) return -1;
		
		// set 2D descriptor Interface to 2D coding scheme
		m_pShape2DCSInterface->SetDescriptorInterface(m_pMultiViewDescriptorInterface->
			 Get2DDescriptorInterface(i));

		// output the visible view flags
		m_EncoderBuffer->PutBitsLong((long)m_pMultiViewDescriptorInterface->GetVisibleViewFlag(i), 1);

		// encode 2D description
		m_pShape2DCSInterface->StartEncode();
	}

	return 0;
}

//----------------------------------------------------------------------------
int MultiViewCS::StartDecode(void)
{
	unsigned long iViewCount;
	unsigned long iVisFlag=0;
	int i;
	unsigned long DescriptorID;
	
	if (!m_DecoderBuffer) return -1;

	// check if descriptor and stream buffer are linked to coding scheme
	if (!m_DecoderBuffer || !m_pMultiViewDescriptorInterface)  return -1;
	
	// read in the descriptor ID
	m_DecoderBuffer->GetBitsLong((long *)&DescriptorID, 8);
	m_DecoderBuffer->TakeBits(8);

	// read in the fixed views flag	
	m_DecoderBuffer->GetBitsLong((long *)&iVisFlag, 1);
	m_DecoderBuffer->TakeBits(1);
	m_pMultiViewDescriptorInterface->SetFixedViewsFlag((bool)iVisFlag);

	// read in the number of views for this object
	m_DecoderBuffer->GetBitsLong((long *)&iViewCount, 4);
	m_DecoderBuffer->TakeBits(4);
	iViewCount += 1;
	m_pMultiViewDescriptorInterface->SetNoOfViews((unsigned char)iViewCount);

	// check values for 2D //////////////////
	for( i = 0; i < iViewCount; i++ ) 
	{
		// check if shape 3D descriptor is linked 2D
		if (!m_pMultiViewDescriptorInterface->Get2DDescriptorInterface(i)) return -1;

		// check if color space descriptor and stream buffer are linkedto color space coding scheme
		if(!m_pShape2DCSInterface->GetDescriptorInterface() || 
			 !m_pShape2DCSInterface->GetDecoderStreamBuffer()) return -1;

		// set 2D descriptor Interface to 2D coding scheme
		m_pShape2DCSInterface->SetDescriptorInterface(m_pMultiViewDescriptorInterface->
			 Get2DDescriptorInterface(i));

	// read in the visible view flags
		m_DecoderBuffer->GetBitsLong((long *)&iVisFlag, 1);
		m_DecoderBuffer->TakeBits(1);
		m_pMultiViewDescriptorInterface->SetVisibleViewFlag(i, (bool)iVisFlag);

		// decode 2D description*/
		m_pShape2DCSInterface->StartDecode();
	}

	return 0;
}

