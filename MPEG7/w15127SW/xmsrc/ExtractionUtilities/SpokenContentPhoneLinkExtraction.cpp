//////////////////////////////////////////////////////////////////////////////
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SpokenContentPhoneLinkExtraction.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <inter.h>
#include <intra.h>
#include <fifo.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"

//static int initextractionwascalled=0;


//=============================================================================
	

using namespace XM;


const UUID SpokenContentPhoneLinkExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneLinkExtractionInterface::myName = "Streaming Component Control Interface";

const UUID SpokenContentPhoneLinkExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneLinkExtractionTool::myName = "SpokenContentPhoneLink Type Descriptor Extractor";

const UUID SpokenContentPhoneLinkExtractionInterfaceABC::myID = UUID();

//=============================================================================
SpokenContentPhoneLinkExtractionInterfaceABC::SpokenContentPhoneLinkExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpokenContentPhoneLinkExtractionInterface::SpokenContentPhoneLinkExtractionInterface(
  SpokenContentPhoneLinkExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLinkExtractionInterface::~SpokenContentPhoneLinkExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLinkExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneLinkExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SpokenContentPhoneLinkExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentPhoneLinkExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLinkDescriptorInterfaceABC* SpokenContentPhoneLinkExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLinkExtractionInterface::
SetDescriptorInterface(SpokenContentPhoneLinkDescriptorInterfaceABC
		       *aSpokenContentPhoneLinkDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aSpokenContentPhoneLinkDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentPhoneLinkExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long SpokenContentPhoneLinkExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
SpokenContentPhoneLinkExtractionTool::SpokenContentPhoneLinkExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpokenContentPhoneLinkExtractionTool::
SpokenContentPhoneLinkExtractionTool(SpokenContentPhoneLinkDescriptorInterfaceABC *SpokenContentPhoneLink):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!SpokenContentPhoneLink) {
	  SpokenContentPhoneLinkDescriptor *descriptor = new SpokenContentPhoneLinkDescriptor();
	  SpokenContentPhoneLink=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(SpokenContentPhoneLink);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLinkExtractionTool::~SpokenContentPhoneLinkExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool SpokenContentPhoneLinkExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpokenContentPhoneLinkExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentPhoneLinkExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
SpokenContentPhoneLinkDescriptorInterfaceABC* SpokenContentPhoneLinkExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLinkExtractionTool::
SetDescriptorInterface(SpokenContentPhoneLinkDescriptorInterfaceABC
		       *aSpokenContentPhoneLinkDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aSpokenContentPhoneLinkDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aSpokenContentPhoneLinkDescriptorInterface;
	if (m_DescriptorInterface) {
	  m_DescriptorInterface->addref();

	  /* conect sub descriptors with sub coding schemes*/
	  /* not available*/
	}
	else {
	  /* if no descriptor, release also descriptor interfaces from
	     sub extraction tools schemes*/
	  /* not available*/
	  return -1;
	}
	
	
	return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long SpokenContentPhoneLinkExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long SpokenContentPhoneLinkExtractionTool::StartExtracting(void)
{
  // To help debugging
  const char* routine="SpokenContentPhoneLinkExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "SpokenContentPhoneLink Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------

  // Get the Phone attribute. This is required
  string wrd;
  if(m_Media->GetTextAttribute(string("phone"),wrd) != 0) {
    fprintf(stderr,"%s Omitted phone\n",routine);
    return(failure);
  }
  long index = atol(wrd.c_str());
  if(index == -1)
  {
	  fprintf(stderr, "%s: phone \"%s\" is not a valid index\n", routine, wrd.c_str());
	  return failure;
  }
  m_DescriptorInterface->SetPhoneIndex(index);
  //m_DescriptorInterface->SetPhone(wrd.c_str());
  
  // Now do the link component
  SpokenContentLinkDescriptor* theDescriptor;
  SpokenContentLinkDescriptorInterfaceABC* theDescriptorInterface;
  SpokenContentLinkExtractionTool* theExtractionTool;
  SpokenContentLinkExtractionInterfaceABC* theExtractionInterface;
  theDescriptor = new SpokenContentLinkDescriptor();
  theDescriptor->addref();
  theDescriptorInterface = theDescriptor->GetInterface();
  theExtractionTool = new SpokenContentLinkExtractionTool(NULL);
  theExtractionInterface = theExtractionTool->GetInterface();

  theExtractionInterface->SetSourceMedia(m_Media);
  theExtractionInterface->SetDescriptorInterface(theDescriptorInterface);
  theExtractionInterface->StartExtracting();

  m_DescriptorInterface->SetLink(theDescriptor);
  theDescriptor->release();

  // All done
  return 0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLinkExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneLinkExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentPhoneLinkExtractionInterfaceABC *SpokenContentPhoneLinkExtractionTool::GetInterface(void)
{ return &m_Interface; }

