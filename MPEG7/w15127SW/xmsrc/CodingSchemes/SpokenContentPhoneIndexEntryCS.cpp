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
// Applicable File Name:  SpokenContentPhoneIndexEntryCS.cpp
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

const UUID SpokenContentPhoneIndexEntryCSInterface::myID = UUID("");
const char *SpokenContentPhoneIndexEntryCSInterface::myName =
                         "SpokenContentPhoneIndexEntry Type Coding Engine Interface";

const UUID SpokenContentPhoneIndexEntryCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneIndexEntryCS::myName = "SpokenContentPhoneIndexEntry Type Coding Scheme";

const UUID SpokenContentPhoneIndexEntryCS::valID = UUID("");
const char *SpokenContentPhoneIndexEntryCS::valName = "";

const UUID SpokenContentPhoneIndexEntryCSInterfaceABC::myID = UUID("");

SpokenContentPhoneIndexEntryCSInterfaceABC::SpokenContentPhoneIndexEntryCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentPhoneIndexEntryCSInterface::SpokenContentPhoneIndexEntryCSInterface(SpokenContentPhoneIndexEntryCS
						     *aSpokenContentPhoneIndexEntry):
m_SpokenContentPhoneIndexEntryCS(aSpokenContentPhoneIndexEntry)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryCSInterface::~SpokenContentPhoneIndexEntryCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneIndexEntryCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneIndexEntryCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryCSInterface::destroy(void)
{ delete m_SpokenContentPhoneIndexEntryCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentPhoneIndexEntryCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentPhoneIndexEntryCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentPhoneIndexEntryCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentPhoneIndexEntryCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentPhoneIndexEntryCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentPhoneIndexEntryCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentPhoneIndexEntryCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentPhoneIndexEntryCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryDescriptorInterfaceABC
*SpokenContentPhoneIndexEntryCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentPhoneIndexEntryCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentPhoneIndexEntryCSInterface::
SetDescriptorInterface(SpokenContentPhoneIndexEntryDescriptorInterfaceABC
		       *aSpokenContentPhoneIndexEntryDescriptorInterface)
{
        return m_SpokenContentPhoneIndexEntryCS->
	  SetDescriptorInterface(aSpokenContentPhoneIndexEntryDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentPhoneIndexEntryCSInterface::StartEncode(void)
{
        return m_SpokenContentPhoneIndexEntryCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentPhoneIndexEntryCSInterface::StartDecode(void)
{
        return m_SpokenContentPhoneIndexEntryCS->StartDecode();
}
//=============================================================================

SpokenContentPhoneIndexEntryCS::SpokenContentPhoneIndexEntryCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentPhoneIndexEntryDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentPhoneIndexEntryCS::SpokenContentPhoneIndexEntryCS(SpokenContentPhoneIndexEntryDescriptorInterfaceABC* SpokenContentPhoneIndexEntry):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentPhoneIndexEntryDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContentPhoneIndexEntry) {
	  SpokenContentPhoneIndexEntryDescriptor *descriptor =
	    new SpokenContentPhoneIndexEntryDescriptor();
	  SpokenContentPhoneIndexEntry=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContentPhoneIndexEntry);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryCS::~SpokenContentPhoneIndexEntryCS()
{
  // release descriptor
  if (m_SpokenContentPhoneIndexEntryDescriptorInterface) {
    m_SpokenContentPhoneIndexEntryDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneIndexEntryCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneIndexEntryCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneIndexEntryCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentPhoneIndexEntryCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentPhoneIndexEntryCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentPhoneIndexEntryCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentPhoneIndexEntryCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentPhoneIndexEntryCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentPhoneIndexEntryCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryDescriptorInterfaceABC *SpokenContentPhoneIndexEntryCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentPhoneIndexEntryDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentPhoneIndexEntryCS::
SetDescriptorInterface(SpokenContentPhoneIndexEntryDescriptorInterfaceABC
		       *aSpokenContentPhoneIndexEntryDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentPhoneIndexEntryDescriptorInterface == 
	    aSpokenContentPhoneIndexEntryDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentPhoneIndexEntryDescriptorInterface) 
	  m_SpokenContentPhoneIndexEntryDescriptorInterface->release();

	// add new interface
	m_SpokenContentPhoneIndexEntryDescriptorInterface = 
	  aSpokenContentPhoneIndexEntryDescriptorInterface;
	if (m_SpokenContentPhoneIndexEntryDescriptorInterface) {
	  m_SpokenContentPhoneIndexEntryDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentPhoneIndexEntryCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentPhoneIndexEntryCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentPhoneIndexEntryDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // Handle the dimension
  unsigned long ne=GetDescriptorInterface()->GetNEntries();
  unsigned char nk=GetDescriptorInterface()->GetNKey();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&ne),
                           8*sizeof(unsigned long));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&nk),
                           8*sizeof(unsigned char));

  // Handle the key
  for(unsigned char i=0;i<nk;i++) {
    // Assume each key is of length 255 chars or less
    const char* k=GetDescriptorInterface()->GetKey(i);
    unsigned char kl=strlen(k);
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&kl),
                           8*sizeof(unsigned char));
    m_EncoderBuffer->PutBits(const_cast<char*> (k),
                           8*static_cast<long> (kl)*sizeof(char));
  }

  // Now write out the entry components
  for(unsigned long j=0;j<ne;j++) {
    SpokenContentIndexEntryDescriptor* entry;
    SpokenContentIndexEntryDescriptorInterfaceABC* entry_abc;
    SpokenContentIndexEntryCS* entry_cs;
    entry=GetDescriptorInterface()->GetEntry(j);
    entry_abc=entry->GetInterface(); 
    entry_cs=new SpokenContentIndexEntryCS();
    entry_cs->SetDescriptorInterface(entry_abc);
    entry_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
    entry_cs->StartEncode();
  } 

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int SpokenContentPhoneIndexEntryCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentPhoneIndexEntryCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentPhoneIndexEntryDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentPhoneIndexEntryCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Get the dimensions
  unsigned long ne;
  unsigned char nk;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&ne),
                           8*sizeof(unsigned long));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&nk),
                           8*sizeof(unsigned char));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
  GetDescriptorInterface()->SetNKey(nk);
  GetDescriptorInterface()->SetNEntries(ne);

  // Get the keys
  char *b=new char[256];
  unsigned char l;
  for(unsigned char i=0;i<nk;i++) {
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&l),
                             8*sizeof(unsigned char));
    m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                             8*static_cast<long> (l)*sizeof(char));
    m_DecoderBuffer->TakeBits(8*static_cast<long> (l)*sizeof(char));
    b[l]='\0';
    GetDescriptorInterface()->SetKey(i,b);
  }
  delete [] b;

  // Handle the entries
  for(unsigned long j=0;j<ne;j++) {
    SpokenContentIndexEntryDescriptor*   entry;
    SpokenContentIndexEntryDescriptorInterfaceABC* entry_abc;
    SpokenContentIndexEntryCS* entry_cs;
    entry=new SpokenContentIndexEntryDescriptor();
    entry_abc=entry->GetInterface();
    entry_cs=new SpokenContentIndexEntryCS(entry_abc);
    entry_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
    entry_cs->StartDecode();
    GetDescriptorInterface()->SetEntry(j,entry);
  }
  // All done
  return 0;
}

//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryCSInterfaceABC *SpokenContentPhoneIndexEntryCS::GetInterface(void)
{ return &m_Interface; }



