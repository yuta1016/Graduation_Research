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
// Applicable File Name:  SpokenContentNodeCS.cpp
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

const UUID SpokenContentNodeCSInterface::myID = UUID("");
const char *SpokenContentNodeCSInterface::myName =
                         "SpokenContentNode Type Coding Engine Interface";

const UUID SpokenContentNodeCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentNodeCS::myName = "SpokenContentNode Type Coding Scheme";

const UUID SpokenContentNodeCS::valID = UUID("");
const char *SpokenContentNodeCS::valName = "";

const UUID SpokenContentNodeCSInterfaceABC::myID = UUID("");

SpokenContentNodeCSInterfaceABC::SpokenContentNodeCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentNodeCSInterface::SpokenContentNodeCSInterface(SpokenContentNodeCS
						     *aSpokenContentNode):
m_SpokenContentNodeCS(aSpokenContentNode)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentNodeCSInterface::~SpokenContentNodeCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentNodeCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentNodeCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentNodeCSInterface::destroy(void)
{ delete m_SpokenContentNodeCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentNodeCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentNodeCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentNodeCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentNodeCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentNodeCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentNodeCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentNodeCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentNodeCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentNodeDescriptorInterfaceABC
*SpokenContentNodeCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentNodeCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentNodeCSInterface::
SetDescriptorInterface(SpokenContentNodeDescriptorInterfaceABC
		       *aSpokenContentNodeDescriptorInterface)
{
        return m_SpokenContentNodeCS->
	  SetDescriptorInterface(aSpokenContentNodeDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentNodeCSInterface::StartEncode(void)
{
        return m_SpokenContentNodeCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentNodeCSInterface::StartDecode(void)
{
        return m_SpokenContentNodeCS->StartDecode();
}
//=============================================================================

SpokenContentNodeCS::SpokenContentNodeCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentNodeDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentNodeCS::SpokenContentNodeCS(SpokenContentNodeDescriptorInterfaceABC* SpokenContentNode):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentNodeDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContentNode) {
	  SpokenContentNodeDescriptor *descriptor =
	    new SpokenContentNodeDescriptor();
	  SpokenContentNode=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContentNode);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentNodeCS::~SpokenContentNodeCS()
{
  // release descriptor
  if (m_SpokenContentNodeDescriptorInterface) {
    m_SpokenContentNodeDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentNodeCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentNodeCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentNodeCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentNodeCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentNodeCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentNodeCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentNodeCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentNodeCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentNodeCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentNodeDescriptorInterfaceABC *SpokenContentNodeCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentNodeDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentNodeCS::
SetDescriptorInterface(SpokenContentNodeDescriptorInterfaceABC
		       *aSpokenContentNodeDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentNodeDescriptorInterface == 
	    aSpokenContentNodeDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentNodeDescriptorInterface) 
	  m_SpokenContentNodeDescriptorInterface->release();

	// add new interface
	m_SpokenContentNodeDescriptorInterface = 
	  aSpokenContentNodeDescriptorInterface;
	if (m_SpokenContentNodeDescriptorInterface) {
	  m_SpokenContentNodeDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentNodeCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentNodeCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentNodeDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // Put out the simple data types
  unsigned char uc;
  unsigned short us;
  uc=GetDescriptorInterface()->GetNWordLinks();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&uc),
                           8*sizeof(unsigned char));
  uc=GetDescriptorInterface()->GetNPhoneLinks();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&uc),
                           8*sizeof(unsigned char));
  us=GetDescriptorInterface()->GetNum();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&us),
                           8*sizeof(unsigned short));
  us=GetDescriptorInterface()->GetTimeOffset();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&us),
                           8*sizeof(unsigned short));

  // Put out the speaker - assume that the idref is less than 255 chars
  char* s=const_cast<char*>(GetDescriptorInterface()->GetSpeaker());
  if(s==NULL) m_EncoderBuffer->PutBit(0);
  else {
    m_EncoderBuffer->PutBit(1);
    uc=strlen(s);
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&uc),
                             8*sizeof(unsigned char));
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (s),
                             8*static_cast<long> (uc)*sizeof(char));
  }

  // Now handle the word links
  unsigned char i;
  for(i=0;i<GetDescriptorInterface()->GetNWordLinks();i++) {
    SpokenContentWordLinkDescriptor* wlx;
    SpokenContentWordLinkDescriptorInterfaceABC* wlx_abc;
    SpokenContentWordLinkCS* wlx_cs;
    wlx=GetDescriptorInterface()->GetWordLink(i);
    wlx_abc=wlx->GetInterface();
    wlx_cs=new SpokenContentWordLinkCS();
    wlx_cs->SetDescriptorInterface(wlx_abc);
    wlx_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
    wlx_cs->StartEncode();
  }

  // Handle the phone links
  for(i=0;i<GetDescriptorInterface()->GetNPhoneLinks();i++) {
    SpokenContentPhoneLinkDescriptor* plx;
    SpokenContentPhoneLinkDescriptorInterfaceABC* plx_abc;
    SpokenContentPhoneLinkCS* plx_cs;
    plx=GetDescriptorInterface()->GetPhoneLink(i);
    plx_abc=plx->GetInterface();
    plx_cs=new SpokenContentPhoneLinkCS();
    plx_cs->SetDescriptorInterface(plx_abc);
    plx_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
    plx_cs->StartEncode();
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int SpokenContentNodeCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentNodeCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentNodeDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentNodeCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Get the simple data types
  unsigned char uc;
  unsigned short us;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&uc),
                           8*sizeof(unsigned char));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
  GetDescriptorInterface()->SetNWordLinks(uc);

  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&uc),
                           8*sizeof(unsigned char));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
  GetDescriptorInterface()->SetNPhoneLinks(uc);

  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&us),
                           8*sizeof(unsigned short));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned short));
  GetDescriptorInterface()->SetNum(us);

  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&us),
                           8*sizeof(unsigned short));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned short));
  GetDescriptorInterface()->SetTimeOffset(us);

  // Get the speaker - assume that the idref is less than 255 chars
  bool flag=(m_DecoderBuffer->GetBit(0)!=0);
  m_DecoderBuffer->TakeBits(1);
  if(flag) {
    char *s=new char[256];
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&uc),
                             8*sizeof(unsigned char));
    m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (s),
                             8*static_cast<long> (uc)*sizeof(char));
    m_DecoderBuffer->TakeBits(8*static_cast<long> (uc)*sizeof(char));
    s[uc]='\0';
    GetDescriptorInterface()->SetSpeaker(s);
    delete [] s;
  }
  else {
    GetDescriptorInterface()->SetSpeaker(NULL);
  }

  // Get the word links
  unsigned char i;
  for(i=0;i<GetDescriptorInterface()->GetNWordLinks();i++) {
    SpokenContentWordLinkDescriptor*             wlx;
    SpokenContentWordLinkDescriptorInterfaceABC* wlx_abc;
    SpokenContentWordLinkCS*                     wlx_cs;
    wlx=new SpokenContentWordLinkDescriptor();
    wlx_abc=wlx->GetInterface();
    wlx_cs=new SpokenContentWordLinkCS(wlx_abc);
    wlx_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
    wlx_cs->StartDecode();
    GetDescriptorInterface()->SetWordLink(i,wlx);
  }

  // Get the phone links
  for(i=0;i<GetDescriptorInterface()->GetNPhoneLinks();i++) {
    SpokenContentPhoneLinkDescriptor*   plx;
    SpokenContentPhoneLinkDescriptorInterfaceABC* plx_abc;
    SpokenContentPhoneLinkCS* plx_cs;
    plx=new SpokenContentPhoneLinkDescriptor();
    plx_abc=plx->GetInterface();
    plx_cs=new SpokenContentPhoneLinkCS(plx_abc);
    plx_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
    plx_cs->StartDecode();
    GetDescriptorInterface()->SetPhoneLink(i,plx);
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
SpokenContentNodeCSInterfaceABC *SpokenContentNodeCS::GetInterface(void)
{ return &m_Interface; }



