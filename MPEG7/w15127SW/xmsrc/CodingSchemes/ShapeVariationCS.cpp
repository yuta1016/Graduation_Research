///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Minseok Choi, Hanyang University, Korea
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
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ShapeVariationCS.cpp
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

const UUID ShapeVariationCSInterface::myID = UUID("");
const char *ShapeVariationCSInterface::myName =
"ShapeVariationCodingSchemeInterface";

// create a UUID for your type
const UUID ShapeVariationCS::myID = UUID("to be created with xx");
const char *ShapeVariationCS::myName = "ShapeVariationCodingScheme";

const UUID ShapeVariationCS::valID = UUID("");
const char *ShapeVariationCS::valName = "";

const UUID ShapeVariationCSInterfaceABC::myID = UUID("");

ShapeVariationCSInterfaceABC::ShapeVariationCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
ShapeVariationCSInterface::ShapeVariationCSInterface(ShapeVariationCS *aShapeVariation):
m_ShapeVariationCS(aShapeVariation)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ShapeVariationCSInterface::~ShapeVariationCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& ShapeVariationCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeVariationCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void ShapeVariationCSInterface::destroy(void)
{ delete m_ShapeVariationCS; }

//----------------------------------------------------------------------------
BitBuffer *ShapeVariationCSInterface::GetEncoderStreamBuffer(void)
{
	return m_ShapeVariationCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ShapeVariationCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
	return m_ShapeVariationCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *ShapeVariationCSInterface::GetDecoderStreamBuffer(void)
{
	return m_ShapeVariationCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ShapeVariationCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
	return m_ShapeVariationCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
ShapeVariationDescriptorInterfaceABC
*ShapeVariationCSInterface::GetDescriptorInterface(void)

{
	return m_ShapeVariationCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ShapeVariationCSInterface::
SetDescriptorInterface(ShapeVariationDescriptorInterfaceABC
					   *aShapeVariationDescriptorInterface)
{
	return m_ShapeVariationCS->
		SetDescriptorInterface(aShapeVariationDescriptorInterface);
}

//----------------------------------------------------------------------------
int ShapeVariationCSInterface::StartEncode(void)
{
	return m_ShapeVariationCS->StartEncode();
}

//----------------------------------------------------------------------------
int ShapeVariationCSInterface::StartDecode(void)
{
	return m_ShapeVariationCS->StartDecode();
}
//=============================================================================
ShapeVariationCS::ShapeVariationCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ShapeVariationCS::ShapeVariationCS(ShapeVariationDescriptorInterfaceABC* aShapeVariation):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL)
{
	/* create descriptor if it does not exist*/
	if (!aShapeVariation) {
		ShapeVariationDescriptor *descriptor = new ShapeVariationDescriptor;
		aShapeVariation=descriptor->GetInterface();
	}	
	
	/* connect descritors with CS*/
	SetDescriptorInterface(aShapeVariation);
	
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ShapeVariationCS::~ShapeVariationCS()
{
	/* release descriptor*/
	if (m_DescriptorInterface)
		m_DescriptorInterface->release();		
}

//----------------------------------------------------------------------------
const UUID& ShapeVariationCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeVariationCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ShapeVariationCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ShapeVariationCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ShapeVariationCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * ShapeVariationCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int ShapeVariationCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
	if (!aBuffer) return -1;
	
	m_EncoderBuffer = aBuffer;	
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * ShapeVariationCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int ShapeVariationCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
	if (!aBuffer) return -1;
	
	m_DecoderBuffer = aBuffer;		
	return 0;
}

//----------------------------------------------------------------------------
ShapeVariationDescriptorInterfaceABC *ShapeVariationCS::
GetDescriptorInterface(void)
{
	return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int ShapeVariationCS::
SetDescriptorInterface(ShapeVariationDescriptorInterfaceABC
					   *aShapeVariationDescriptorInterface)
{
	/* check if new value is different from old value*/
	if (m_DescriptorInterface == 
		aShapeVariationDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_DescriptorInterface) 
		m_DescriptorInterface->release();
	
	/* add new interface*/
	m_DescriptorInterface = 
		aShapeVariationDescriptorInterface;
	if (m_DescriptorInterface) {
		m_DescriptorInterface->addref();		
	}
	else {	
		return -1;
	}
	
	return 0;
}

//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int ShapeVariationCS::StartEncode(void)
{
	//long v;
	
	// Is the file IO set up? If not return
	if(m_EncoderBuffer==NULL) {
		fprintf(stderr,"ShapeVariationCS::StartEncode: Buffer not initialised\n");
		return(-1);
	}
	
	// Is the interface set up?
	if(m_DescriptorInterface==NULL) {
		fprintf(stderr,"ShapeVariationCS::StartEncode: Interface not initialised\n");
		return -1;
	}
			

	char p, r;
	long value;
	
	for( p = 0 ; p < ANGULAR ; p++ )
	{
		for( r = 0 ; r < RADIAL ; r++ )
		{
			if( p !=0 || r != 0 ) 
			{
				value = m_DescriptorInterface->GetLVMElement(p,r);
				m_EncoderBuffer->PutBitsLong(value, 4UL);
			}
		}
	}
	for( p = 0 ; p < ANGULAR ; p++ )
	{
		for( r = 0 ; r < RADIAL ; r++ )		
		{
			if( p !=0 || r != 0 ) 
			{
				value = m_DescriptorInterface->GetHVMElement(p,r);
				m_EncoderBuffer->PutBitsLong(value, 4UL);
			}
		}
	}
	for( p = 0 ; p < ANGULAR ; p++ )
	{
		for( r = 0 ; r < RADIAL ; r++ )			
		{
			if( p !=0 || r != 0 ) 
			{
				value = m_DescriptorInterface->GetSTDElement(p,r);
				m_EncoderBuffer->PutBitsLong(value, 4UL);
			}
		}
	}
	
	// All done
	return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int ShapeVariationCS::StartDecode(void)
{
	//long v;
	
	// Is the file IO set up? If not return
	if(m_DecoderBuffer==NULL) {
		fprintf(stderr,"ShapeVariationCS::StartDecode: Buffer not initialised\n");
		return(-1);
	}
	
	// Is the interface set up?
	if(m_DescriptorInterface==NULL) {
		fprintf(stderr,"ShapeVariationCS::StartDecode: Interface not initialised\n");
		return -1;
	}
	
	char p, r;
	long value;

	for( p = 0 ; p < ANGULAR ; p++ )
	{
		for( r = 0 ; r < RADIAL ; r++ )			
		{
			if( p != 0 || r != 0 )
			{
				m_DecoderBuffer->GetBitsLong(&value, 4UL);
				m_DecoderBuffer->TakeBits(4UL);
				m_DescriptorInterface->SetLVMElement( p, r, (char)value);
			}
		}
	}

	for( p = 0 ; p < ANGULAR ; p++ )	
	{
		for( r = 0 ; r < RADIAL ; r++ )
		{
			if( p != 0 || r != 0 )
			{
				m_DecoderBuffer->GetBitsLong(&value, 4UL);
				m_DecoderBuffer->TakeBits(4UL);
				m_DescriptorInterface->SetHVMElement( p, r, (char)value);
			}
		}
	}	

	for( p = 0 ; p < ANGULAR ; p++ )
	{		
		for( r = 0 ; r < RADIAL ; r++ )
		{
			if( p != 0 || r != 0 )
			{
				m_DecoderBuffer->GetBitsLong(&value, 4UL);
				m_DecoderBuffer->TakeBits(4UL);
				m_DescriptorInterface->SetSTDElement( p, r, (char)value);
			}
		}
	}	
	
	// All done
	return 0;
}

//----------------------------------------------------------------------------
ShapeVariationCSInterfaceABC *ShapeVariationCS::GetInterface(void)
{ return &m_Interface; }



