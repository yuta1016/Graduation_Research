///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jason Charlesworth
// Canon Research Centre (Europe) Ltd.
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
// Applicable File Name:  ProbabilityVectorCS.cpp
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

const UUID ProbabilityVectorCSInterface::myID = UUID("");
const char *ProbabilityVectorCSInterface::myName =
                         "ProbabilityVector Type Coding Engine Interface";

const UUID ProbabilityVectorCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *ProbabilityVectorCS::myName = "ProbabilityVector Type Coding Scheme";

const UUID ProbabilityVectorCS::valID = UUID("");
const char *ProbabilityVectorCS::valName = "";

const UUID ProbabilityVectorCSInterfaceABC::myID = UUID("");

ProbabilityVectorCSInterfaceABC::ProbabilityVectorCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
ProbabilityVectorCSInterface::ProbabilityVectorCSInterface(ProbabilityVectorCS
						     *aProbabilityVector):
m_ProbabilityVectorCS(aProbabilityVector)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ProbabilityVectorCSInterface::~ProbabilityVectorCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& ProbabilityVectorCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ProbabilityVectorCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void ProbabilityVectorCSInterface::destroy(void)
{ delete m_ProbabilityVectorCS; }

//----------------------------------------------------------------------------
BitBuffer *ProbabilityVectorCSInterface::GetEncoderStreamBuffer(void)
{
        return m_ProbabilityVectorCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ProbabilityVectorCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ProbabilityVectorCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *ProbabilityVectorCSInterface::GetDecoderStreamBuffer(void)
{
        return m_ProbabilityVectorCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ProbabilityVectorCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ProbabilityVectorCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
ProbabilityVectorDescriptorInterfaceABC
*ProbabilityVectorCSInterface::GetDescriptorInterface(void)

{
        return m_ProbabilityVectorCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ProbabilityVectorCSInterface::
SetDescriptorInterface(ProbabilityVectorDescriptorInterfaceABC
		       *aProbabilityVectorDescriptorInterface)
{
        return m_ProbabilityVectorCS->
	  SetDescriptorInterface(aProbabilityVectorDescriptorInterface);
}

//----------------------------------------------------------------------------
int ProbabilityVectorCSInterface::StartEncode(void)
{
        return m_ProbabilityVectorCS->StartEncode();
}
//----------------------------------------------------------------------------
int ProbabilityVectorCSInterface::StartDecode(void)
{
        return m_ProbabilityVectorCS->StartDecode();
}
//=============================================================================

ProbabilityVectorCS::ProbabilityVectorCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_ProbabilityVectorDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

ProbabilityVectorCS::ProbabilityVectorCS(ProbabilityVectorDescriptorInterfaceABC* ProbabilityVector):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_ProbabilityVectorDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!ProbabilityVector) {
	  ProbabilityVectorDescriptor *descriptor =
	    new ProbabilityVectorDescriptor();
	  ProbabilityVector=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(ProbabilityVector);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ProbabilityVectorCS::~ProbabilityVectorCS()
{
  // release descriptor
  if (m_ProbabilityVectorDescriptorInterface) {
    m_ProbabilityVectorDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& ProbabilityVectorCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ProbabilityVectorCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ProbabilityVectorCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ProbabilityVectorCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ProbabilityVectorCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * ProbabilityVectorCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int ProbabilityVectorCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * ProbabilityVectorCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int ProbabilityVectorCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
ProbabilityVectorDescriptorInterfaceABC *ProbabilityVectorCS::
GetDescriptorInterface(void)
{
       return m_ProbabilityVectorDescriptorInterface;
}

//----------------------------------------------------------------------------
int ProbabilityVectorCS::
SetDescriptorInterface(ProbabilityVectorDescriptorInterfaceABC
		       *aProbabilityVectorDescriptorInterface)
{
        // check if new value is different from old value
        if (m_ProbabilityVectorDescriptorInterface == 
	    aProbabilityVectorDescriptorInterface) return 0;
	
	// release old interface
	if (m_ProbabilityVectorDescriptorInterface) 
	  m_ProbabilityVectorDescriptorInterface->release();

	// add new interface
	m_ProbabilityVectorDescriptorInterface = 
	  aProbabilityVectorDescriptorInterface;
	if (m_ProbabilityVectorDescriptorInterface) {
	  m_ProbabilityVectorDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int ProbabilityVectorCS::StartEncode(void)
{
  // To help debugging
  const char* routine="ProbabilityVectorCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_ProbabilityVectorDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // Write out the length
  unsigned long l=GetDescriptorInterface()->GetSize();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&l),
                           8*sizeof(unsigned long));

  // Write out the values
  for(unsigned long i=0;i<l;i++) {
    float p=GetDescriptorInterface()->GetValue(i);
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&p),
                             8*sizeof(float));
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int ProbabilityVectorCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"ProbabilityVectorCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_ProbabilityVectorDescriptorInterface==NULL) {
    fprintf(stderr,"ProbabilityVectorCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Get the length
  unsigned long l;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&l),
                           8*sizeof(unsigned long));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
  GetDescriptorInterface()->SetSize(l);

  // Get the data
  for(unsigned long i=0;i<l;i++) {
    float p;
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&p),
                             8*sizeof(float));
    m_DecoderBuffer->TakeBits(8*sizeof(float));
    GetDescriptorInterface()->SetValue(p,i);
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
ProbabilityVectorCSInterfaceABC *ProbabilityVectorCS::GetInterface(void)
{ return &m_Interface; }



