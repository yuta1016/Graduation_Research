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
// Applicable File Name:  SpokenContentWordLinkCS.cpp
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

const UUID SpokenContentWordLinkCSInterface::myID = UUID("");
const char *SpokenContentWordLinkCSInterface::myName =
                         "SpokenContentWordLink Type Coding Engine Interface";

const UUID SpokenContentWordLinkCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordLinkCS::myName = "SpokenContentWordLink Type Coding Scheme";

const UUID SpokenContentWordLinkCS::valID = UUID("");
const char *SpokenContentWordLinkCS::valName = "";

const UUID SpokenContentWordLinkCSInterfaceABC::myID = UUID("");

SpokenContentWordLinkCSInterfaceABC::SpokenContentWordLinkCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentWordLinkCSInterface::SpokenContentWordLinkCSInterface(SpokenContentWordLinkCS
						     *aSpokenContentWordLink):
m_SpokenContentWordLinkCS(aSpokenContentWordLink)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentWordLinkCSInterface::~SpokenContentWordLinkCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentWordLinkCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordLinkCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentWordLinkCSInterface::destroy(void)
{ delete m_SpokenContentWordLinkCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentWordLinkCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentWordLinkCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentWordLinkCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentWordLinkCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentWordLinkCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentWordLinkCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentWordLinkCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentWordLinkCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentWordLinkDescriptorInterfaceABC
*SpokenContentWordLinkCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentWordLinkCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentWordLinkCSInterface::
SetDescriptorInterface(SpokenContentWordLinkDescriptorInterfaceABC
		       *aSpokenContentWordLinkDescriptorInterface)
{
        return m_SpokenContentWordLinkCS->
	  SetDescriptorInterface(aSpokenContentWordLinkDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentWordLinkCSInterface::StartEncode(void)
{
        return m_SpokenContentWordLinkCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentWordLinkCSInterface::StartDecode(void)
{
        return m_SpokenContentWordLinkCS->StartDecode();
}
//=============================================================================

SpokenContentWordLinkCS::SpokenContentWordLinkCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentWordLinkDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentWordLinkCS::SpokenContentWordLinkCS(SpokenContentWordLinkDescriptorInterfaceABC* SpokenContentWordLink):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentWordLinkDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContentWordLink) {
	  SpokenContentWordLinkDescriptor *descriptor =
	    new SpokenContentWordLinkDescriptor();
	  SpokenContentWordLink=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContentWordLink);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentWordLinkCS::~SpokenContentWordLinkCS()
{
  // release descriptor
  if (m_SpokenContentWordLinkDescriptorInterface) {
    m_SpokenContentWordLinkDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentWordLinkCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordLinkCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentWordLinkCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentWordLinkCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentWordLinkCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentWordLinkCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentWordLinkCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentWordLinkCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentWordLinkCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentWordLinkDescriptorInterfaceABC *SpokenContentWordLinkCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentWordLinkDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentWordLinkCS::
SetDescriptorInterface(SpokenContentWordLinkDescriptorInterfaceABC
		       *aSpokenContentWordLinkDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentWordLinkDescriptorInterface == 
	    aSpokenContentWordLinkDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentWordLinkDescriptorInterface) 
	  m_SpokenContentWordLinkDescriptorInterface->release();

	// add new interface
	m_SpokenContentWordLinkDescriptorInterface = 
	  aSpokenContentWordLinkDescriptorInterface;
	if (m_SpokenContentWordLinkDescriptorInterface) {
	  m_SpokenContentWordLinkDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentWordLinkCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentWordLinkCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentWordLinkDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // Handle the word - assume all words are less that 255 characters in length
  char* w=const_cast<char*>(GetDescriptorInterface()->GetWord());
  unsigned char wl=strlen(w);
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&wl),
                           8*sizeof(unsigned char));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (w),
                           8*static_cast<long> (wl)*sizeof(char));

  // Now write out the Link component
  SpokenContentLinkDescriptor* lnk;
  SpokenContentLinkDescriptorInterfaceABC* lnk_abc;
  SpokenContentLinkCS* lnk_cs;
  lnk=GetDescriptorInterface()->GetLink();
  lnk_abc=lnk->GetInterface();
  lnk_cs=new SpokenContentLinkCS();
  lnk_cs->SetDescriptorInterface(lnk_abc);
  lnk_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
  lnk_cs->StartEncode();
  
  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int SpokenContentWordLinkCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentWordLinkCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentWordLinkDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentWordLinkCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Handle the word - assume all words are less that 255 characters in length
  char *b=new char[256];
  unsigned char wl;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&wl),
                             8*sizeof(unsigned char));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                             8*static_cast<long> (wl)*sizeof(char));
  m_DecoderBuffer->TakeBits(8*static_cast<long> (wl)*sizeof(char));
  b[wl]='\0';
  GetDescriptorInterface()->SetWord(b);
  delete [] b;

  // Now do the Link
  SpokenContentLinkDescriptor*   lnk;
  SpokenContentLinkDescriptorInterfaceABC* lnk_abc;
  SpokenContentLinkCS* lnk_cs;
  lnk=new SpokenContentLinkDescriptor();
  lnk_abc=lnk->GetInterface();
  lnk_cs=new SpokenContentLinkCS(lnk_abc);
  lnk_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
  lnk_cs->StartDecode();
  GetDescriptorInterface()->SetLink(lnk);

  // All done
  return 0;
}

//----------------------------------------------------------------------------
SpokenContentWordLinkCSInterfaceABC *SpokenContentWordLinkCS::GetInterface(void)
{ return &m_Interface; }



