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
// Applicable File Name:  SpokenContentConfusionInfoCS.cpp
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

const UUID SpokenContentConfusionInfoCSInterface::myID = UUID("");
const char *SpokenContentConfusionInfoCSInterface::myName =
                         "SpokenContentConfusionInfo Type Coding Engine Interface";

const UUID SpokenContentConfusionInfoCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentConfusionInfoCS::myName = "SpokenContentConfusionInfo Type Coding Scheme";

const UUID SpokenContentConfusionInfoCS::valID = UUID("");
const char *SpokenContentConfusionInfoCS::valName = "";

const UUID SpokenContentConfusionInfoCSInterfaceABC::myID = UUID("");

SpokenContentConfusionInfoCSInterfaceABC::SpokenContentConfusionInfoCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentConfusionInfoCSInterface::SpokenContentConfusionInfoCSInterface(SpokenContentConfusionInfoCS
						     *aSpokenContentConfusionInfo):
m_SpokenContentConfusionInfoCS(aSpokenContentConfusionInfo)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentConfusionInfoCSInterface::~SpokenContentConfusionInfoCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentConfusionInfoCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentConfusionInfoCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoCSInterface::destroy(void)
{ delete m_SpokenContentConfusionInfoCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentConfusionInfoCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentConfusionInfoCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentConfusionInfoCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentConfusionInfoCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentConfusionInfoCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentConfusionInfoCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentConfusionInfoCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentConfusionInfoCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentConfusionInfoDescriptorInterfaceABC
*SpokenContentConfusionInfoCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentConfusionInfoCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentConfusionInfoCSInterface::
SetDescriptorInterface(SpokenContentConfusionInfoDescriptorInterfaceABC
		       *aSpokenContentConfusionInfoDescriptorInterface)
{
        return m_SpokenContentConfusionInfoCS->
	  SetDescriptorInterface(aSpokenContentConfusionInfoDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentConfusionInfoCSInterface::StartEncode(void)
{
        return m_SpokenContentConfusionInfoCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentConfusionInfoCSInterface::StartDecode(void)
{
        return m_SpokenContentConfusionInfoCS->StartDecode();
}
//=============================================================================

SpokenContentConfusionInfoCS::SpokenContentConfusionInfoCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentConfusionInfoDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentConfusionInfoCS::SpokenContentConfusionInfoCS(SpokenContentConfusionInfoDescriptorInterfaceABC* SpokenContentConfusionInfo):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentConfusionInfoDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContentConfusionInfo) {
	  SpokenContentConfusionInfoDescriptor *descriptor =
	    new SpokenContentConfusionInfoDescriptor();
	  SpokenContentConfusionInfo=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContentConfusionInfo);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentConfusionInfoCS::~SpokenContentConfusionInfoCS()
{
  // release descriptor
  if (m_SpokenContentConfusionInfoDescriptorInterface) {
    m_SpokenContentConfusionInfoDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentConfusionInfoCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentConfusionInfoCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentConfusionInfoCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentConfusionInfoCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentConfusionInfoCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentConfusionInfoCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentConfusionInfoCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentConfusionInfoCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentConfusionInfoCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentConfusionInfoDescriptorInterfaceABC *SpokenContentConfusionInfoCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentConfusionInfoDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentConfusionInfoCS::
SetDescriptorInterface(SpokenContentConfusionInfoDescriptorInterfaceABC
		       *aSpokenContentConfusionInfoDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentConfusionInfoDescriptorInterface == 
	    aSpokenContentConfusionInfoDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentConfusionInfoDescriptorInterface) 
	  m_SpokenContentConfusionInfoDescriptorInterface->release();

	// add new interface
	m_SpokenContentConfusionInfoDescriptorInterface = 
	  aSpokenContentConfusionInfoDescriptorInterface;
	if (m_SpokenContentConfusionInfoDescriptorInterface) {
	  m_SpokenContentConfusionInfoDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentConfusionInfoCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentConfusionInfoCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentConfusionInfoDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // Handle the size and unconditional
  float p=GetDescriptorInterface()->GetUnconditionalInsertionProb();
  unsigned long l=GetDescriptorInterface()->GetSize();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&l),
                           8*sizeof(unsigned long));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&p),
                           8*sizeof(float));

  // Handle the id
  char* ci=const_cast<char*>(GetDescriptorInterface()->GetConfusionID());
  unsigned char il=strlen(ci);
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&il),
                           8*sizeof(unsigned char));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (ci),
                           8*static_cast<long> (il)*sizeof(char));

  ProbabilityVectorDescriptor* pvec;
  ProbabilityVectorDescriptorInterfaceABC* pvec_abc;
  ProbabilityVectorCS* pvec_cs;
  ProbabilityMatrixDescriptor* pmat;
  ProbabilityMatrixDescriptorInterfaceABC* pmat_abc;
  ProbabilityMatrixCS* pmat_cs;

  // Now write out the occurance
  pvec=GetDescriptorInterface()->GetOccuranceProb();
  pvec_abc=pvec->GetInterface();
  pvec_cs=new ProbabilityVectorCS();
  pvec_cs->SetDescriptorInterface(pvec_abc);
  pvec_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
  pvec_cs->StartEncode();
  
  // Now write out the insertion
  pvec=GetDescriptorInterface()->GetInsertionProb();
  pvec_abc=pvec->GetInterface();
  pvec_cs=new ProbabilityVectorCS();
  pvec_cs->SetDescriptorInterface(pvec_abc);
  pvec_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
  pvec_cs->StartEncode();
  
  // Now write out the omission
  pvec=GetDescriptorInterface()->GetOmissionProb();
  pvec_abc=pvec->GetInterface();
  pvec_cs=new ProbabilityVectorCS();
  pvec_cs->SetDescriptorInterface(pvec_abc);
  pvec_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
  pvec_cs->StartEncode();
  
  // Now write out the confusions
  pmat=GetDescriptorInterface()->GetConfusionProb();
  pmat_abc=pmat->GetInterface();
  pmat_cs=new ProbabilityMatrixCS();
  pmat_cs->SetDescriptorInterface(pmat_abc);
  pmat_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
  pmat_cs->StartEncode();

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int SpokenContentConfusionInfoCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentConfusionInfoCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentConfusionInfoDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentConfusionInfoCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Handle the size and unconditional prob
  unsigned long l;
  float p;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&l),
                             8*sizeof(unsigned long));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&p),
                             8*sizeof(float));
  m_DecoderBuffer->TakeBits(8*sizeof(float));
  GetDescriptorInterface()->SetSize(l);
  GetDescriptorInterface()->SetUnconditionalInsertionProb(p);

  // Handle the id
  char b[256];
  unsigned char ib;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&ib),
                           8*sizeof(unsigned char));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                             8*static_cast<long> (ib)*sizeof(char));
  m_DecoderBuffer->TakeBits(8*static_cast<long> (ib)*sizeof(char));
  b[ib]='\0';
  GetDescriptorInterface()->SetConfusionID(b);

  ProbabilityVectorDescriptor*   pvec;
  ProbabilityVectorDescriptorInterfaceABC* pvec_abc;
  ProbabilityVectorCS* pvec_cs;
  ProbabilityMatrixDescriptor*   pmat;
  ProbabilityMatrixDescriptorInterfaceABC* pmat_abc;
  ProbabilityMatrixCS* pmat_cs;

  // Now do the occurance
  pvec=new ProbabilityVectorDescriptor();
  pvec_abc=pvec->GetInterface();
  pvec_cs=new ProbabilityVectorCS(pvec_abc);
  pvec_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
  pvec_cs->StartDecode();
  GetDescriptorInterface()->SetOccuranceProb(pvec);

  // Now do the insertion
  pvec=new ProbabilityVectorDescriptor();
  pvec_abc=pvec->GetInterface();
  pvec_cs=new ProbabilityVectorCS(pvec_abc);
  pvec_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
  pvec_cs->StartDecode();
  GetDescriptorInterface()->SetInsertionProb(pvec);

  // Now do the omission
  pvec=new ProbabilityVectorDescriptor();
  pvec_abc=pvec->GetInterface();
  pvec_cs=new ProbabilityVectorCS(pvec_abc);
  pvec_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
  pvec_cs->StartDecode();
  GetDescriptorInterface()->SetOmissionProb(pvec);

  // Now do the confusion
  pmat=new ProbabilityMatrixDescriptor();
  pmat_abc=pmat->GetInterface();
  pmat_cs=new ProbabilityMatrixCS(pmat_abc);
  pmat_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
  pmat_cs->StartDecode();
  GetDescriptorInterface()->SetConfusionProb(pmat);

  // All done
  return 0;
}

//----------------------------------------------------------------------------
SpokenContentConfusionInfoCSInterfaceABC *SpokenContentConfusionInfoCS::GetInterface(void)
{ return &m_Interface; }



