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
// Applicable File Name:  SpokenContentCS.cpp
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

const UUID SpokenContentCSInterface::myID = UUID("");
const char *SpokenContentCSInterface::myName =
                         "SpokenContent Type Coding Engine Interface";

const UUID SpokenContentCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentCS::myName = "SpokenContent Type Coding Scheme";

const UUID SpokenContentCS::valID = UUID("");
const char *SpokenContentCS::valName = "";

const UUID SpokenContentCSInterfaceABC::myID = UUID("");

SpokenContentCSInterfaceABC::SpokenContentCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentCSInterface::SpokenContentCSInterface(SpokenContentCS
						     *aSpokenContent):
m_SpokenContentCS(aSpokenContent)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentCSInterface::~SpokenContentCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentCSInterface::destroy(void)
{ delete m_SpokenContentCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentDescriptorInterfaceABC
*SpokenContentCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentCSInterface::
SetDescriptorInterface(SpokenContentDescriptorInterfaceABC
		       *aSpokenContentDescriptorInterface)
{
        return m_SpokenContentCS->
	  SetDescriptorInterface(aSpokenContentDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentCSInterface::StartEncode(void)
{
        return m_SpokenContentCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentCSInterface::StartDecode(void)
{
        return m_SpokenContentCS->StartDecode();
}
//=============================================================================

SpokenContentCS::SpokenContentCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentCS::SpokenContentCS(SpokenContentDescriptorInterfaceABC* SpokenContent):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContent) {
	  SpokenContentDescriptor *descriptor =
	    new SpokenContentDescriptor();
	  SpokenContent=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContent);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentCS::~SpokenContentCS()
{
  // release descriptor
  if (m_SpokenContentDescriptorInterface) {
    m_SpokenContentDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentDescriptorInterfaceABC *SpokenContentCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentCS::
SetDescriptorInterface(SpokenContentDescriptorInterfaceABC
		       *aSpokenContentDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentDescriptorInterface == 
	    aSpokenContentDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentDescriptorInterface) 
	  m_SpokenContentDescriptorInterface->release();

	// add new interface
	m_SpokenContentDescriptorInterface = 
	  aSpokenContentDescriptorInterface;
	if (m_SpokenContentDescriptorInterface) {
	  m_SpokenContentDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // Handle the dimension
  unsigned long nl=GetDescriptorInterface()->GetNLattice();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&nl),
                           8*sizeof(unsigned long));

  // Now write out the header
  SpokenContentHeaderDescriptor* header;
  SpokenContentHeaderDescriptorInterfaceABC* header_abc;
  SpokenContentHeaderCS* header_cs;
  header=GetDescriptorInterface()->GetHeader();
  header_abc=header->GetInterface(); 
  header_cs=new SpokenContentHeaderCS();
  header_cs->SetDescriptorInterface(header_abc);
  header_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
  header_cs->StartEncode();

  // Now write out the lattice components
  for(unsigned short j=0;j<nl;j++) {
    SpokenContentLatticeDescriptor* lattice;
    SpokenContentLatticeDescriptorInterfaceABC* lattice_abc;
    SpokenContentLatticeCS* lattice_cs;
    lattice=GetDescriptorInterface()->GetLattice(j);
    lattice_abc=lattice->GetInterface(); 
    lattice_cs=new SpokenContentLatticeCS();
    lattice_cs->SetDescriptorInterface(lattice_abc);
    lattice_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
    lattice_cs->StartEncode();
  } 

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int SpokenContentCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Get the dimensions
  unsigned long nl;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&nl),
                           8*sizeof(unsigned long));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
  GetDescriptorInterface()->SetNLattice(nl);

  // Handle the header
  SpokenContentHeaderDescriptor*   header;
  SpokenContentHeaderDescriptorInterfaceABC* header_abc;
  SpokenContentHeaderCS* header_cs;
  header=new SpokenContentHeaderDescriptor();
  header_abc=header->GetInterface();
  header_cs=new SpokenContentHeaderCS(header_abc);
  header_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
  header_cs->StartDecode();
  GetDescriptorInterface()->SetHeader(header);

  // Handle the lattice
  for(unsigned long j=0;j<nl;j++) {
    SpokenContentLatticeDescriptor*   lattice;
    SpokenContentLatticeDescriptorInterfaceABC* lattice_abc;
    SpokenContentLatticeCS* lattice_cs;
    lattice=new SpokenContentLatticeDescriptor();
    lattice_abc=lattice->GetInterface();
    lattice_cs=new SpokenContentLatticeCS(lattice_abc);
    lattice_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
    lattice_cs->StartDecode();
    GetDescriptorInterface()->SetLattice(j,lattice);
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
SpokenContentCSInterfaceABC *SpokenContentCS::GetInterface(void)
{ return &m_Interface; }



