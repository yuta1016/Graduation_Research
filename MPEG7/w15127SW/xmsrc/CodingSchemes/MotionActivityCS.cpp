//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Nam Kyu Kim, Dong-Gye Sim, Hae-Kwang Kim - Hyundai
// Ajay Divakaran - Mitsubishi
// Xinding Sun, B. S. Manjunath -UCSB
// Sharp corporation, Yasuaki Tokumo, Shuichi Watanabe.
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
// Applicable File Name:  MotionActivityCS.cpp
//

#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <sys/types.h>
#include <math.h> // for SLMA
//#include <netinet/in.h>

using namespace XM;

const UUID MotionActivityCSInterface::myID = UUID("");
const char *MotionActivityCSInterface::myName =
                         "Motion Activity Coding Scheme Interface";
const UUID MotionActivityCS::myID = UUID("");
const char *MotionActivityCS::myName = "Motion Activity Coding Scheme";
const UUID MotionActivityCS::valID = UUID("");
const char *MotionActivityCS::valName = "";
const UUID MotionActivityCSInterfaceABC::myID = UUID("");

MotionActivityCSInterfaceABC::MotionActivityCSInterfaceABC()

{
  SetInterfaceID(myID);
}

//=============================================================================

MotionActivityCSInterface::MotionActivityCSInterface(MotionActivityCS *aMotionActivity):
m_MotionActivityCS(aMotionActivity)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MotionActivityCSInterface::~MotionActivityCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& MotionActivityCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionActivityCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void MotionActivityCSInterface::destroy(void)
{ delete m_MotionActivityCS; }

//----------------------------------------------------------------------------
BitBuffer *MotionActivityCSInterface::GetEncoderStreamBuffer(void)
{
        return m_MotionActivityCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int MotionActivityCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_MotionActivityCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *MotionActivityCSInterface::GetDecoderStreamBuffer(void)
{
        return m_MotionActivityCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int MotionActivityCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_MotionActivityCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
MotionActivityDescriptorInterfaceABC *MotionActivityCSInterface::
GetDescriptorInterface(void)
{
        return m_MotionActivityCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int MotionActivityCSInterface::
SetDescriptorInterface(MotionActivityDescriptorInterfaceABC
		       *aMotionActivityDescriptorInterface)
{
        return m_MotionActivityCS->
	  SetDescriptorInterface(aMotionActivityDescriptorInterface);
}

//----------------------------------------------------------------------------
int MotionActivityCSInterface::StartEncode(void)
{
        return m_MotionActivityCS->StartEncode();
}

//----------------------------------------------------------------------------
int MotionActivityCSInterface::StartDecode(void)
{
        return m_MotionActivityCS->StartDecode();
}

//=============================================================================
MotionActivityCS::MotionActivityCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_MotionActivityDescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MotionActivityCS::MotionActivityCS(MotionActivityDescriptorInterfaceABC
				   *motionactivity):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_MotionActivityDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptors\n");
#endif

	/* create descriptor if it does not exist*/
	if (!motionactivity) {
	  MotionActivityDescriptor *descriptor =
	    new MotionActivityDescriptor();
	  motionactivity=descriptor->GetInterface();
	}

	/* connect descritors with coding schemes*/
	SetDescriptorInterface(motionactivity);


	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MotionActivityCS::~MotionActivityCS()
{
	/* release descriptor*/
	if (m_MotionActivityDescriptorInterface)
	  m_MotionActivityDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& MotionActivityCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionActivityCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& MotionActivityCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* MotionActivityCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool MotionActivityCS::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
BitBuffer * MotionActivityCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int MotionActivityCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * MotionActivityCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int MotionActivityCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
MotionActivityDescriptorInterfaceABC *MotionActivityCS::GetDescriptorInterface(void)
{
        return m_MotionActivityDescriptorInterface;
}

//----------------------------------------------------------------------------
int MotionActivityCS::
SetDescriptorInterface(MotionActivityDescriptorInterfaceABC
	      *aMotionActivityDescriptorInterface)
{
        if (m_MotionActivityDescriptorInterface == 
	    aMotionActivityDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_MotionActivityDescriptorInterface) 
	  m_MotionActivityDescriptorInterface->release();

	/* add new interface*/
	m_MotionActivityDescriptorInterface = 
	  aMotionActivityDescriptorInterface;
	if (m_MotionActivityDescriptorInterface) {
	  m_MotionActivityDescriptorInterface->addref();
	}
	else {
	  return -1;
	}

	return 0;

}

#ifndef SPALOCNUMBER
#define SPALOCNUMBER 1
#endif


//----------------------------------------------------------------------------
int MotionActivityCS::StartEncode(void)
{
	int i;
	int division_block; // for SLMA
	long a;
	long direction_flag,
		 spatial_distribution_flag,
		 spatial_localized_distribution_flag, // for SLMA
		 temporal_distribution_flag;

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_EncoderBuffer || !m_MotionActivityDescriptorInterface) {
	  return -1;
	}

	/* encode motion activity */
	/*
		Intensity;
		DirectionFlag;
		SpatialDistributionFlag;
		SpatialLocalizedDistributionFlag;
		TemporalDistributionFlag;
		if( DirectionFlag == 0 ) DominantDirection;
		if( SpatialDistributionFlag == 0 )  { m_SpatialParameters * 3; }
		if( SpatialLocalizedDistributionFlag == 0 ) {
			SpaLocNumber;
			for(i=0;i<SpaLocNumber;i++) { SparialLocalizedParameter; }
		}
		if( TemporalDistributionFlag == 0 ) { m_TemporalParameters * 5;	}
	*/

	a = m_MotionActivityDescriptorInterface->GetIntensity();
	m_EncoderBuffer->PutBitsLong(a, 3);	
	direction_flag = m_MotionActivityDescriptorInterface->GetDirectionFlag();
	m_EncoderBuffer->PutBitsLong(direction_flag, 1);	
	spatial_distribution_flag = m_MotionActivityDescriptorInterface->GetSpatialDistributionFlag();
	m_EncoderBuffer->PutBitsLong(spatial_distribution_flag, 1);	
	spatial_localized_distribution_flag = m_MotionActivityDescriptorInterface->GetSpatialLocalizedDistributionFlag(); // for SLMA
	m_EncoderBuffer->PutBitsLong(spatial_localized_distribution_flag, 1); // for SLMA
	temporal_distribution_flag = m_MotionActivityDescriptorInterface->GetTemporalDistributionFlag();
	m_EncoderBuffer->PutBitsLong(temporal_distribution_flag, 1);	

	if( direction_flag == 1 ) 
	{
		a = m_MotionActivityDescriptorInterface->GetDominantDirection();
		m_EncoderBuffer->PutBitsLong(a, 3);	
	}
	if( spatial_distribution_flag == 1 ) 
	{
		a = m_MotionActivityDescriptorInterface->GetSpatialParameters(0); // Nsr
		m_EncoderBuffer->PutBitsLong(a, 6);	
		a = m_MotionActivityDescriptorInterface->GetSpatialParameters(1); // Nmr
		m_EncoderBuffer->PutBitsLong(a, 5);	
		a = m_MotionActivityDescriptorInterface->GetSpatialParameters(2); // Nlr
		m_EncoderBuffer->PutBitsLong(a, 5);	
	}
	if( spatial_localized_distribution_flag == 1)// for SLMA
	{
		/*
		a = m_MotionActivityDescriptorInterface->GetSpaLocNumber();
		m_EncoderBuffer->PutBitsLong(a, 2);
		division_block=pow(4.0, (double)(a+1.0));
		*/
		// 2013/07/25 KW - added the value according to the specification of bitstream.
		m_EncoderBuffer->PutBitsLong((int)SPALOCNUMBER, 2);
		division_block=(int)(pow(4.0, (double)(SPALOCNUMBER+1.0)));
		for (i=0; i<division_block; i++){
			a = m_MotionActivityDescriptorInterface->GetSpatialLocalizedParameter(i);
			m_EncoderBuffer->PutBitsLong(a, 3);
		}
	}

	if( temporal_distribution_flag == 1 ) 
	{
		for( i=0; i< NUM_N0_N1_N2_N3_N4; i++ )
		{
			a = m_MotionActivityDescriptorInterface->GetTemporalParameters(i);
			m_EncoderBuffer->PutBitsLong(a, 6);	
		}
	}

	return 0;
}

//----------------------------------------------------------------------------
int MotionActivityCS::StartDecode(void)
{
	int division_block; // for SLMA
	long a;
	long direction_flag,
		 spatial_distribution_flag,
		 spatial_localized_distribution_flag, // for SLMA
		 temporal_distribution_flag;

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_MotionActivityDescriptorInterface) {
	  return -1;
	}

	/* decode motion activity */
	/*
		Intensity;
		DirectionFlag;
		SpatialDistributionFlag;
		SpatialLocalizedDistributionFlag;
		TemporalDistributionFlag;
		if( DirectionFlag == 0 ) DominantDirection;
		if( SpatialDistributionFlag == 0 )  { m_SpatialParameters * 3; }
		if( SpatialLocalizedDistributionFlag == 0 ) {
			SpaLocNumber;
			for(i=0;i<SpaLocNumber;i++) { SparialLocalizedParameter; }
		}
		if( TemporalDistributionFlag == 0 ) { m_TemporalParameters * 5;	}
	*/
	m_DecoderBuffer->GetBitsLong(&a, 3);
	m_DecoderBuffer->TakeBits(3);
	m_MotionActivityDescriptorInterface->SetIntensity(a);

	m_DecoderBuffer->GetBitsLong(&direction_flag, 1);
	m_DecoderBuffer->TakeBits(1);
	m_MotionActivityDescriptorInterface->SetDirectionFlag(direction_flag);

	m_DecoderBuffer->GetBitsLong(&spatial_distribution_flag, 1);
	m_DecoderBuffer->TakeBits(1);
	m_MotionActivityDescriptorInterface->SetSpatialDistributionFlag(spatial_distribution_flag);
	m_DecoderBuffer->GetBitsLong(&spatial_localized_distribution_flag, 1); // for SLMA

	m_DecoderBuffer->TakeBits(1); // for SLMA

	m_MotionActivityDescriptorInterface->SetSpatialLocalizedDistributionFlag(spatial_localized_distribution_flag); // for SLMA
	m_DecoderBuffer->GetBitsLong(&temporal_distribution_flag, 1);
	m_DecoderBuffer->TakeBits(1);
	m_MotionActivityDescriptorInterface->SetTemporalDistributionFlag(temporal_distribution_flag);

	if( direction_flag == 1 )
	{
		m_DecoderBuffer->GetBitsLong(&a, 3);
		m_DecoderBuffer->TakeBits(3);
		m_MotionActivityDescriptorInterface->SetDominantDirection(a);
	}
	
	if( spatial_distribution_flag == 1 )
	{
		m_DecoderBuffer->GetBitsLong(&a, 6);
		m_DecoderBuffer->TakeBits(6);
		m_MotionActivityDescriptorInterface->SetSpatialParameters(0, a); // Nsr

		m_DecoderBuffer->GetBitsLong(&a, 5);
		m_DecoderBuffer->TakeBits(5);
		m_MotionActivityDescriptorInterface->SetSpatialParameters(1, a); // Nmr

		m_DecoderBuffer->GetBitsLong(&a, 5);
		m_DecoderBuffer->TakeBits(5);
		m_MotionActivityDescriptorInterface->SetSpatialParameters(2, a); // Nlr
	}

	if( spatial_localized_distribution_flag == 1 ) // for SLMA
	{
		/*
		m_DecoderBuffer->GetBitsLong(&a, 2);
		m_DecoderBuffer->TakeBits(2);
		m_MotionActivityDescriptorInterface->SetSpaLocNumber(a);
		division_block=pow(4.0, (double)(a+1.0));
		*/
		m_DecoderBuffer->GetBitsLong(&a, 2);
		m_DecoderBuffer->TakeBits(2);
		assert(a==SPALOCNUMBER);
		division_block=(int)(pow(4.0, (double)(SPALOCNUMBER+1.0)));
		for ( int i=0; i<division_block; i++){
			m_DecoderBuffer->GetBitsLong(&a, 3);
			m_DecoderBuffer->TakeBits(3);
			m_MotionActivityDescriptorInterface->SetSpatialLocalizedParameter(i, a);
		}
	}

	if( temporal_distribution_flag == 1 )
	{
		for( int i=0; i< NUM_N0_N1_N2_N3_N4; i++ )
		{
			m_DecoderBuffer->GetBitsLong(&a, 6);
			m_DecoderBuffer->TakeBits(6);
			m_MotionActivityDescriptorInterface->SetTemporalParameters(i, a); //N0,N1,N2,N3,N4
		}
	}
	
	return 0;
}

//----------------------------------------------------------------------------
MotionActivityCSInterfaceABC *MotionActivityCS::GetInterface(void)
{ return &m_Interface; }





