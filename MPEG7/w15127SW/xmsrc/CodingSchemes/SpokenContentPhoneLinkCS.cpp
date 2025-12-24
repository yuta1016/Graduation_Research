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
// Applicable File Name:  SpokenContentPhoneLinkCS.cpp
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

const UUID SpokenContentPhoneLinkCSInterface::myID = UUID("");
const char *SpokenContentPhoneLinkCSInterface::myName =
                         "SpokenContentPhoneLink Type Coding Engine Interface";

const UUID SpokenContentPhoneLinkCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneLinkCS::myName = "SpokenContentPhoneLink Type Coding Scheme";

const UUID SpokenContentPhoneLinkCS::valID = UUID("");
const char *SpokenContentPhoneLinkCS::valName = "";

const UUID SpokenContentPhoneLinkCSInterfaceABC::myID = UUID("");

SpokenContentPhoneLinkCSInterfaceABC::SpokenContentPhoneLinkCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentPhoneLinkCSInterface::SpokenContentPhoneLinkCSInterface(SpokenContentPhoneLinkCS
						     *aSpokenContentPhoneLink):
m_SpokenContentPhoneLinkCS(aSpokenContentPhoneLink)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLinkCSInterface::~SpokenContentPhoneLinkCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLinkCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneLinkCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentPhoneLinkCSInterface::destroy(void)
{ delete m_SpokenContentPhoneLinkCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentPhoneLinkCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentPhoneLinkCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLinkCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentPhoneLinkCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentPhoneLinkCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentPhoneLinkCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLinkCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentPhoneLinkCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLinkDescriptorInterfaceABC
*SpokenContentPhoneLinkCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentPhoneLinkCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLinkCSInterface::
SetDescriptorInterface(SpokenContentPhoneLinkDescriptorInterfaceABC
		       *aSpokenContentPhoneLinkDescriptorInterface)
{
        return m_SpokenContentPhoneLinkCS->
	  SetDescriptorInterface(aSpokenContentPhoneLinkDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLinkCSInterface::StartEncode(void)
{
        return m_SpokenContentPhoneLinkCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentPhoneLinkCSInterface::StartDecode(void)
{
        return m_SpokenContentPhoneLinkCS->StartDecode();
}
//=============================================================================

SpokenContentPhoneLinkCS::SpokenContentPhoneLinkCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentPhoneLinkDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentPhoneLinkCS::SpokenContentPhoneLinkCS(SpokenContentPhoneLinkDescriptorInterfaceABC* SpokenContentPhoneLink):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentPhoneLinkDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContentPhoneLink) {
	  SpokenContentPhoneLinkDescriptor *descriptor =
	    new SpokenContentPhoneLinkDescriptor();
	  SpokenContentPhoneLink=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContentPhoneLink);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLinkCS::~SpokenContentPhoneLinkCS()
{
  // release descriptor
  if (m_SpokenContentPhoneLinkDescriptorInterface) {
    m_SpokenContentPhoneLinkDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLinkCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneLinkCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLinkCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentPhoneLinkCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentPhoneLinkCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentPhoneLinkCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLinkCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentPhoneLinkCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLinkCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentPhoneLinkDescriptorInterfaceABC *SpokenContentPhoneLinkCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentPhoneLinkDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLinkCS::
SetDescriptorInterface(SpokenContentPhoneLinkDescriptorInterfaceABC
		       *aSpokenContentPhoneLinkDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentPhoneLinkDescriptorInterface == 
	    aSpokenContentPhoneLinkDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentPhoneLinkDescriptorInterface) 
	  m_SpokenContentPhoneLinkDescriptorInterface->release();

	// add new interface
	m_SpokenContentPhoneLinkDescriptorInterface = 
	  aSpokenContentPhoneLinkDescriptorInterface;
	if (m_SpokenContentPhoneLinkDescriptorInterface) {
	  m_SpokenContentPhoneLinkDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentPhoneLinkCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentPhoneLinkCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentPhoneLinkDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // Handle the phone - assume all words are less that 255 characters in length
  char* w=const_cast<char*>(GetDescriptorInterface()->GetPhone());
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
int SpokenContentPhoneLinkCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentPhoneLinkCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentPhoneLinkDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentPhoneLinkCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Handle the phone - assume all phones are less that 255 characters in length
  char *b=new char[256];
  unsigned char wl;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&wl),
                             8*sizeof(unsigned char));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                             8*static_cast<long> (wl)*sizeof(char));
  m_DecoderBuffer->TakeBits(8*static_cast<long> (wl)*sizeof(char));
  b[wl]='\0';
  GetDescriptorInterface()->SetPhone(b);
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
SpokenContentPhoneLinkCSInterfaceABC *SpokenContentPhoneLinkCS::GetInterface(void)
{ return &m_Interface; }



