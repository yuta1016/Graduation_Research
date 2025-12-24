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
// Applicable File Name:  ProbabilityMatrixCS.cpp
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

const UUID ProbabilityMatrixCSInterface::myID = UUID("");
const char *ProbabilityMatrixCSInterface::myName =
                         "ProbabilityMatrix Type Coding Engine Interface";

const UUID ProbabilityMatrixCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *ProbabilityMatrixCS::myName = "ProbabilityMatrix Type Coding Scheme";

const UUID ProbabilityMatrixCS::valID = UUID("");
const char *ProbabilityMatrixCS::valName = "";

const UUID ProbabilityMatrixCSInterfaceABC::myID = UUID("");

ProbabilityMatrixCSInterfaceABC::ProbabilityMatrixCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
ProbabilityMatrixCSInterface::ProbabilityMatrixCSInterface(ProbabilityMatrixCS
						     *aProbabilityMatrix):
m_ProbabilityMatrixCS(aProbabilityMatrix)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ProbabilityMatrixCSInterface::~ProbabilityMatrixCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& ProbabilityMatrixCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ProbabilityMatrixCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void ProbabilityMatrixCSInterface::destroy(void)
{ delete m_ProbabilityMatrixCS; }

//----------------------------------------------------------------------------
BitBuffer *ProbabilityMatrixCSInterface::GetEncoderStreamBuffer(void)
{
        return m_ProbabilityMatrixCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ProbabilityMatrixCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ProbabilityMatrixCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *ProbabilityMatrixCSInterface::GetDecoderStreamBuffer(void)
{
        return m_ProbabilityMatrixCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ProbabilityMatrixCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ProbabilityMatrixCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
ProbabilityMatrixDescriptorInterfaceABC
*ProbabilityMatrixCSInterface::GetDescriptorInterface(void)

{
        return m_ProbabilityMatrixCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ProbabilityMatrixCSInterface::
SetDescriptorInterface(ProbabilityMatrixDescriptorInterfaceABC
		       *aProbabilityMatrixDescriptorInterface)
{
        return m_ProbabilityMatrixCS->
	  SetDescriptorInterface(aProbabilityMatrixDescriptorInterface);
}

//----------------------------------------------------------------------------
int ProbabilityMatrixCSInterface::StartEncode(void)
{
        return m_ProbabilityMatrixCS->StartEncode();
}
//----------------------------------------------------------------------------
int ProbabilityMatrixCSInterface::StartDecode(void)
{
        return m_ProbabilityMatrixCS->StartDecode();
}
//=============================================================================

ProbabilityMatrixCS::ProbabilityMatrixCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_ProbabilityMatrixDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

ProbabilityMatrixCS::ProbabilityMatrixCS(ProbabilityMatrixDescriptorInterfaceABC* ProbabilityMatrix):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_ProbabilityMatrixDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!ProbabilityMatrix) {
	  ProbabilityMatrixDescriptor *descriptor =
	    new ProbabilityMatrixDescriptor();
	  ProbabilityMatrix=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(ProbabilityMatrix);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ProbabilityMatrixCS::~ProbabilityMatrixCS()
{
  // release descriptor
  if (m_ProbabilityMatrixDescriptorInterface) {
    m_ProbabilityMatrixDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& ProbabilityMatrixCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ProbabilityMatrixCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ProbabilityMatrixCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ProbabilityMatrixCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ProbabilityMatrixCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * ProbabilityMatrixCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int ProbabilityMatrixCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * ProbabilityMatrixCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int ProbabilityMatrixCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
ProbabilityMatrixDescriptorInterfaceABC *ProbabilityMatrixCS::
GetDescriptorInterface(void)
{
       return m_ProbabilityMatrixDescriptorInterface;
}

//----------------------------------------------------------------------------
int ProbabilityMatrixCS::
SetDescriptorInterface(ProbabilityMatrixDescriptorInterfaceABC
		       *aProbabilityMatrixDescriptorInterface)
{
        // check if new value is different from old value
        if (m_ProbabilityMatrixDescriptorInterface == 
	    aProbabilityMatrixDescriptorInterface) return 0;
	
	// release old interface
	if (m_ProbabilityMatrixDescriptorInterface) 
	  m_ProbabilityMatrixDescriptorInterface->release();

	// add new interface
	m_ProbabilityMatrixDescriptorInterface = 
	  aProbabilityMatrixDescriptorInterface;
	if (m_ProbabilityMatrixDescriptorInterface) {
	  m_ProbabilityMatrixDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int ProbabilityMatrixCS::StartEncode(void)
{
  // To help debugging
  const char* routine="ProbabilityMatrixCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_ProbabilityMatrixDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // Write out the row and col
  unsigned long r=GetDescriptorInterface()->GetRows(),
    c=GetDescriptorInterface()->GetCols();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&r),
                           8*sizeof(unsigned long));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&c),
                           8*sizeof(unsigned long));

  // Write out the values
  for(unsigned long i=0;i<r;i++) {
    for(unsigned long j=0;j<c;j++) {
      float p=GetDescriptorInterface()->GetValue(i,j);
      m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&p),
                               8*sizeof(float));
    }
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int ProbabilityMatrixCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"ProbabilityMatrixCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_ProbabilityMatrixDescriptorInterface==NULL) {
    fprintf(stderr,"ProbabilityMatrixCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Get the length
  unsigned long r,c;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&r),
                           8*sizeof(unsigned long));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&c),
                           8*sizeof(unsigned long));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
  GetDescriptorInterface()->SetSize(r,c);

  // Get the data
  for(unsigned long i=0;i<r;i++) {
    for(unsigned long j=0;j<c;j++) {
      float p;
      m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&p),
                               8*sizeof(float));
      m_DecoderBuffer->TakeBits(8*sizeof(float));
      GetDescriptorInterface()->SetValue(p,i,j);
    }
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
ProbabilityMatrixCSInterfaceABC *ProbabilityMatrixCS::GetInterface(void)
{ return &m_Interface; }



