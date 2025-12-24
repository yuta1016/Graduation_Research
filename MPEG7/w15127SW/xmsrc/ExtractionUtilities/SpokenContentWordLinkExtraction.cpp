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
// Applicable File Name:  SpokenContentWordLinkExtraction.cpp
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


const UUID SpokenContentWordLinkExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordLinkExtractionInterface::myName = "Streaming Component Control Interface";

const UUID SpokenContentWordLinkExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordLinkExtractionTool::myName = "SpokenContentWordLink Type Descriptor Extractor";

const UUID SpokenContentWordLinkExtractionInterfaceABC::myID = UUID();

//=============================================================================
SpokenContentWordLinkExtractionInterfaceABC::SpokenContentWordLinkExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpokenContentWordLinkExtractionInterface::SpokenContentWordLinkExtractionInterface(
  SpokenContentWordLinkExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentWordLinkExtractionInterface::~SpokenContentWordLinkExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpokenContentWordLinkExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordLinkExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SpokenContentWordLinkExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentWordLinkExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
SpokenContentWordLinkDescriptorInterfaceABC* SpokenContentWordLinkExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentWordLinkExtractionInterface::
SetDescriptorInterface(SpokenContentWordLinkDescriptorInterfaceABC
		       *aSpokenContentWordLinkDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aSpokenContentWordLinkDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentWordLinkExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long SpokenContentWordLinkExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
SpokenContentWordLinkExtractionTool::SpokenContentWordLinkExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpokenContentWordLinkExtractionTool::
SpokenContentWordLinkExtractionTool(SpokenContentWordLinkDescriptorInterfaceABC *SpokenContentWordLink):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!SpokenContentWordLink) {
	  SpokenContentWordLinkDescriptor *descriptor = new SpokenContentWordLinkDescriptor();
	  SpokenContentWordLink=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(SpokenContentWordLink);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentWordLinkExtractionTool::~SpokenContentWordLinkExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool SpokenContentWordLinkExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpokenContentWordLinkExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentWordLinkExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
SpokenContentWordLinkDescriptorInterfaceABC* SpokenContentWordLinkExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentWordLinkExtractionTool::
SetDescriptorInterface(SpokenContentWordLinkDescriptorInterfaceABC
		       *aSpokenContentWordLinkDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aSpokenContentWordLinkDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aSpokenContentWordLinkDescriptorInterface;
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
unsigned long SpokenContentWordLinkExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long SpokenContentWordLinkExtractionTool::StartExtracting(void)
{
  // To help debugging
  const char* routine="SpokenContentWordLinkExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "SpokenContentWordLink Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------

  // Get the word attribute. This is required
  string wrd;
  if(m_Media->GetTextAttribute(string("word"),wrd) != 0) {
    fprintf(stderr,"%s Omitted word\n",routine);
    return(failure);
  }
  long wordIndex = atol(wrd.c_str());
  if(wordIndex == -1)
  {
	  fprintf(stderr, "%s word \"%s\" is not a valid index", routine, wrd.c_str());
	  return failure;
  }
  m_DescriptorInterface->SetWordIndex(wordIndex);
  
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
const UUID& SpokenContentWordLinkExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordLinkExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentWordLinkExtractionInterfaceABC *SpokenContentWordLinkExtractionTool::GetInterface(void)
{ return &m_Interface; }

