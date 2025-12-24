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
// Applicable File Name:  SpokenContentIndexEntryCS.cpp
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

const UUID SpokenContentIndexEntryCSInterface::myID = UUID("");
const char *SpokenContentIndexEntryCSInterface::myName =
                         "SpokenContentIndexEntry Type Coding Engine Interface";

const UUID SpokenContentIndexEntryCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentIndexEntryCS::myName = "SpokenContentIndexEntry Type Coding Scheme";

const UUID SpokenContentIndexEntryCS::valID = UUID("");
const char *SpokenContentIndexEntryCS::valName = "";

const UUID SpokenContentIndexEntryCSInterfaceABC::myID = UUID("");

SpokenContentIndexEntryCSInterfaceABC::SpokenContentIndexEntryCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentIndexEntryCSInterface::SpokenContentIndexEntryCSInterface(SpokenContentIndexEntryCS
						     *aSpokenContentIndexEntry):
m_SpokenContentIndexEntryCS(aSpokenContentIndexEntry)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentIndexEntryCSInterface::~SpokenContentIndexEntryCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentIndexEntryCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentIndexEntryCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentIndexEntryCSInterface::destroy(void)
{ delete m_SpokenContentIndexEntryCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentIndexEntryCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentIndexEntryCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentIndexEntryCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentIndexEntryCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentIndexEntryCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentIndexEntryCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentIndexEntryCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentIndexEntryCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentIndexEntryDescriptorInterfaceABC
*SpokenContentIndexEntryCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentIndexEntryCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentIndexEntryCSInterface::
SetDescriptorInterface(SpokenContentIndexEntryDescriptorInterfaceABC
		       *aSpokenContentIndexEntryDescriptorInterface)
{
        return m_SpokenContentIndexEntryCS->
	  SetDescriptorInterface(aSpokenContentIndexEntryDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentIndexEntryCSInterface::StartEncode(void)
{
        return m_SpokenContentIndexEntryCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentIndexEntryCSInterface::StartDecode(void)
{
        return m_SpokenContentIndexEntryCS->StartDecode();
}
//=============================================================================

SpokenContentIndexEntryCS::SpokenContentIndexEntryCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentIndexEntryDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentIndexEntryCS::SpokenContentIndexEntryCS(SpokenContentIndexEntryDescriptorInterfaceABC* SpokenContentIndexEntry):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentIndexEntryDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContentIndexEntry) {
	  SpokenContentIndexEntryDescriptor *descriptor =
	    new SpokenContentIndexEntryDescriptor();
	  SpokenContentIndexEntry=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContentIndexEntry);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentIndexEntryCS::~SpokenContentIndexEntryCS()
{
  // release descriptor
  if (m_SpokenContentIndexEntryDescriptorInterface) {
    m_SpokenContentIndexEntryDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentIndexEntryCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentIndexEntryCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentIndexEntryCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentIndexEntryCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentIndexEntryCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentIndexEntryCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentIndexEntryCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentIndexEntryCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentIndexEntryCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentIndexEntryDescriptorInterfaceABC *SpokenContentIndexEntryCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentIndexEntryDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentIndexEntryCS::
SetDescriptorInterface(SpokenContentIndexEntryDescriptorInterfaceABC
		       *aSpokenContentIndexEntryDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentIndexEntryDescriptorInterface == 
	    aSpokenContentIndexEntryDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentIndexEntryDescriptorInterface) 
	  m_SpokenContentIndexEntryDescriptorInterface->release();

	// add new interface
	m_SpokenContentIndexEntryDescriptorInterface = 
	  aSpokenContentIndexEntryDescriptorInterface;
	if (m_SpokenContentIndexEntryDescriptorInterface) {
	  m_SpokenContentIndexEntryDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentIndexEntryCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentIndexEntryCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentIndexEntryDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // Write out the node and block
  unsigned short n=GetDescriptorInterface()->GetNode();
  unsigned long b=GetDescriptorInterface()->GetBlock();

  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned short));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&b),
                           8*sizeof(unsigned long));

  // Write out the lattice - assume length of string less than 256 chars
  char* l=const_cast<char*>(GetDescriptorInterface()->GetLattice());
  if(l==NULL) {
    m_EncoderBuffer->PutBit(0);
  }
  else {
    m_EncoderBuffer->PutBit(1);
    unsigned char ll=strlen(l);
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&ll),
                             8*sizeof(unsigned char));
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (l),
                             8*ll*sizeof(char));
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int SpokenContentIndexEntryCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentIndexEntryCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentIndexEntryDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentIndexEntryCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Get the node and block
  unsigned short n;
  unsigned long b;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned short));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned short));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&b),
                           8*sizeof(unsigned long));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
  GetDescriptorInterface()->SetNode(n);
  GetDescriptorInterface()->SetBlock(b);

  // Now do the lattice ID
  const bool flag=(m_DecoderBuffer->GetBit(0) != 0);
  m_DecoderBuffer->TakeBits(1);
  if(flag) {
    unsigned char ll;
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&ll),
                             8*sizeof(unsigned char));
    m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
    char *t=new char[ll+1];
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (t),
                             8*ll*sizeof(char));
    m_DecoderBuffer->TakeBits(8*ll*sizeof(char));
    t[ll]='\0';
    GetDescriptorInterface()->SetLattice(t);
    delete [] t;
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
SpokenContentIndexEntryCSInterfaceABC *SpokenContentIndexEntryCS::GetInterface(void)
{ return &m_Interface; }



