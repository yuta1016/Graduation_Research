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
// Applicable File Name:  SpokenContentLatticeCS.cpp
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

const UUID SpokenContentLatticeCSInterface::myID = UUID("");
const char *SpokenContentLatticeCSInterface::myName =
                         "SpokenContentLattice Type Coding Engine Interface";

const UUID SpokenContentLatticeCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentLatticeCS::myName = "SpokenContentLattice Type Coding Scheme";

const UUID SpokenContentLatticeCS::valID = UUID("");
const char *SpokenContentLatticeCS::valName = "";

const UUID SpokenContentLatticeCSInterfaceABC::myID = UUID("");

SpokenContentLatticeCSInterfaceABC::SpokenContentLatticeCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentLatticeCSInterface::SpokenContentLatticeCSInterface(SpokenContentLatticeCS
						     *aSpokenContentLattice):
m_SpokenContentLatticeCS(aSpokenContentLattice)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentLatticeCSInterface::~SpokenContentLatticeCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentLatticeCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLatticeCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentLatticeCSInterface::destroy(void)
{ delete m_SpokenContentLatticeCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentLatticeCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentLatticeCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentLatticeCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentLatticeCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentLatticeCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentLatticeCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentLatticeCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentLatticeCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentLatticeDescriptorInterfaceABC
*SpokenContentLatticeCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentLatticeCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentLatticeCSInterface::
SetDescriptorInterface(SpokenContentLatticeDescriptorInterfaceABC
		       *aSpokenContentLatticeDescriptorInterface)
{
        return m_SpokenContentLatticeCS->
	  SetDescriptorInterface(aSpokenContentLatticeDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentLatticeCSInterface::StartEncode(void)
{
        return m_SpokenContentLatticeCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentLatticeCSInterface::StartDecode(void)
{
        return m_SpokenContentLatticeCS->StartDecode();
}
//=============================================================================

SpokenContentLatticeCS::SpokenContentLatticeCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentLatticeDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentLatticeCS::SpokenContentLatticeCS(SpokenContentLatticeDescriptorInterfaceABC* SpokenContentLattice):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentLatticeDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContentLattice) {
	  SpokenContentLatticeDescriptor *descriptor =
	    new SpokenContentLatticeDescriptor();
	  SpokenContentLattice=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContentLattice);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentLatticeCS::~SpokenContentLatticeCS()
{
  // release descriptor
  if (m_SpokenContentLatticeDescriptorInterface) {
    m_SpokenContentLatticeDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentLatticeCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLatticeCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentLatticeCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentLatticeCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentLatticeCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentLatticeCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentLatticeCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentLatticeCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentLatticeCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentLatticeDescriptorInterfaceABC *SpokenContentLatticeCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentLatticeDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentLatticeCS::
SetDescriptorInterface(SpokenContentLatticeDescriptorInterfaceABC
		       *aSpokenContentLatticeDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentLatticeDescriptorInterface == 
	    aSpokenContentLatticeDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentLatticeDescriptorInterface) 
	  m_SpokenContentLatticeDescriptorInterface->release();

	// add new interface
	m_SpokenContentLatticeDescriptorInterface = 
	  aSpokenContentLatticeDescriptorInterface;
	if (m_SpokenContentLatticeDescriptorInterface) {
	  m_SpokenContentLatticeDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentLatticeCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentLatticeCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentLatticeDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // Handle the dimension
  unsigned short nb=GetDescriptorInterface()->GetNBlocks();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&nb),
                           8*sizeof(unsigned short));

  // Handle the id - assumed 255 chars or less
  char* ii=const_cast<char*>(GetDescriptorInterface()->GetLatticeID());
  unsigned char li=strlen(ii);
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&li),
                           8*sizeof(unsigned char));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (ii),
                           8*static_cast<long> (li)*sizeof(char));

  // Now write out the entry components
  for(unsigned short j=0;j<nb;j++) {
    SpokenContentBlockDescriptor* entry;
    SpokenContentBlockDescriptorInterfaceABC* entry_abc;
    SpokenContentBlockCS* entry_cs;
    entry=GetDescriptorInterface()->GetBlock(j);
    entry_abc=entry->GetInterface(); 
    entry_cs=new SpokenContentBlockCS();
    entry_cs->SetDescriptorInterface(entry_abc);
    entry_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
    entry_cs->StartEncode();
  } 

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int SpokenContentLatticeCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentLatticeCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentLatticeDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentLatticeCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Get the dimensions
  unsigned short nb;
  unsigned char li;
  char b[256];
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&nb),
                           8*sizeof(unsigned short));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned short));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&li),
                           8*sizeof(unsigned char));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                           8*static_cast<long> (li)*sizeof(char));
  m_DecoderBuffer->TakeBits(8*static_cast<long> (li)*sizeof(char));
  b[li]='\0';
  GetDescriptorInterface()->SetNBlocks(nb);
  GetDescriptorInterface()->SetLatticeID(b);

  // Handle the blocks
  for(unsigned long j=0;j<GetDescriptorInterface()->GetNBlocks();j++) {
    SpokenContentBlockDescriptor*   entry;
    SpokenContentBlockDescriptorInterfaceABC* entry_abc;
    SpokenContentBlockCS* entry_cs;
    entry=new SpokenContentBlockDescriptor();
    entry_abc=entry->GetInterface();
    entry_cs=new SpokenContentBlockCS(entry_abc);
    entry_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
    entry_cs->StartDecode();
    GetDescriptorInterface()->SetBlock(j,entry);
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
SpokenContentLatticeCSInterfaceABC *SpokenContentLatticeCS::GetInterface(void)
{ return &m_Interface; }



