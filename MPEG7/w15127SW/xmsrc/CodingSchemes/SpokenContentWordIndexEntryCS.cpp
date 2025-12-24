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
// Applicable File Name:  SpokenContentWordIndexEntryCS.cpp
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

const UUID SpokenContentWordIndexEntryCSInterface::myID = UUID("");
const char *SpokenContentWordIndexEntryCSInterface::myName =
                         "SpokenContentWordIndexEntry Type Coding Engine Interface";

const UUID SpokenContentWordIndexEntryCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordIndexEntryCS::myName = "SpokenContentWordIndexEntry Type Coding Scheme";

const UUID SpokenContentWordIndexEntryCS::valID = UUID("");
const char *SpokenContentWordIndexEntryCS::valName = "";

const UUID SpokenContentWordIndexEntryCSInterfaceABC::myID = UUID("");

SpokenContentWordIndexEntryCSInterfaceABC::SpokenContentWordIndexEntryCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentWordIndexEntryCSInterface::SpokenContentWordIndexEntryCSInterface(SpokenContentWordIndexEntryCS
						     *aSpokenContentWordIndexEntry):
m_SpokenContentWordIndexEntryCS(aSpokenContentWordIndexEntry)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentWordIndexEntryCSInterface::~SpokenContentWordIndexEntryCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentWordIndexEntryCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordIndexEntryCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryCSInterface::destroy(void)
{ delete m_SpokenContentWordIndexEntryCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentWordIndexEntryCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentWordIndexEntryCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentWordIndexEntryCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentWordIndexEntryCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentWordIndexEntryCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentWordIndexEntryCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentWordIndexEntryCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentWordIndexEntryCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentWordIndexEntryDescriptorInterfaceABC
*SpokenContentWordIndexEntryCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentWordIndexEntryCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentWordIndexEntryCSInterface::
SetDescriptorInterface(SpokenContentWordIndexEntryDescriptorInterfaceABC
		       *aSpokenContentWordIndexEntryDescriptorInterface)
{
        return m_SpokenContentWordIndexEntryCS->
	  SetDescriptorInterface(aSpokenContentWordIndexEntryDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentWordIndexEntryCSInterface::StartEncode(void)
{
        return m_SpokenContentWordIndexEntryCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentWordIndexEntryCSInterface::StartDecode(void)
{
        return m_SpokenContentWordIndexEntryCS->StartDecode();
}
//=============================================================================

SpokenContentWordIndexEntryCS::SpokenContentWordIndexEntryCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentWordIndexEntryDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentWordIndexEntryCS::SpokenContentWordIndexEntryCS(SpokenContentWordIndexEntryDescriptorInterfaceABC* SpokenContentWordIndexEntry):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentWordIndexEntryDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContentWordIndexEntry) {
	  SpokenContentWordIndexEntryDescriptor *descriptor =
	    new SpokenContentWordIndexEntryDescriptor();
	  SpokenContentWordIndexEntry=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContentWordIndexEntry);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentWordIndexEntryCS::~SpokenContentWordIndexEntryCS()
{
  // release descriptor
  if (m_SpokenContentWordIndexEntryDescriptorInterface) {
    m_SpokenContentWordIndexEntryDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentWordIndexEntryCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordIndexEntryCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentWordIndexEntryCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentWordIndexEntryCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentWordIndexEntryCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentWordIndexEntryCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentWordIndexEntryCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentWordIndexEntryCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentWordIndexEntryCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentWordIndexEntryDescriptorInterfaceABC *SpokenContentWordIndexEntryCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentWordIndexEntryDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentWordIndexEntryCS::
SetDescriptorInterface(SpokenContentWordIndexEntryDescriptorInterfaceABC
		       *aSpokenContentWordIndexEntryDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentWordIndexEntryDescriptorInterface == 
	    aSpokenContentWordIndexEntryDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentWordIndexEntryDescriptorInterface) 
	  m_SpokenContentWordIndexEntryDescriptorInterface->release();

	// add new interface
	m_SpokenContentWordIndexEntryDescriptorInterface = 
	  aSpokenContentWordIndexEntryDescriptorInterface;
	if (m_SpokenContentWordIndexEntryDescriptorInterface) {
	  m_SpokenContentWordIndexEntryDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentWordIndexEntryCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentWordIndexEntryCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentWordIndexEntryDescriptorInterface==NULL) {
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
int SpokenContentWordIndexEntryCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentWordIndexEntryCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentWordIndexEntryDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentWordIndexEntryCS::StartDecode: Interface not initialised\n");
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
SpokenContentWordIndexEntryCSInterfaceABC *SpokenContentWordIndexEntryCS::GetInterface(void)
{ return &m_Interface; }



