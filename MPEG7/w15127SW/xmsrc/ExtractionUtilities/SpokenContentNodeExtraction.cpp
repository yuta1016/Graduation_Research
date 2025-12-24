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
// Applicable File Name:  SpokenContentNodeExtraction.cpp
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


const UUID SpokenContentNodeExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *SpokenContentNodeExtractionInterface::myName = "Streaming Component Control Interface";

const UUID SpokenContentNodeExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentNodeExtractionTool::myName = "SpokenContentNode Type Descriptor Extractor";

const UUID SpokenContentNodeExtractionInterfaceABC::myID = UUID();

//=============================================================================
SpokenContentNodeExtractionInterfaceABC::SpokenContentNodeExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpokenContentNodeExtractionInterface::SpokenContentNodeExtractionInterface(
  SpokenContentNodeExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentNodeExtractionInterface::~SpokenContentNodeExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpokenContentNodeExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentNodeExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SpokenContentNodeExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentNodeExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
SpokenContentNodeDescriptorInterfaceABC* SpokenContentNodeExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentNodeExtractionInterface::
SetDescriptorInterface(SpokenContentNodeDescriptorInterfaceABC
		       *aSpokenContentNodeDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aSpokenContentNodeDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentNodeExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long SpokenContentNodeExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
SpokenContentNodeExtractionTool::SpokenContentNodeExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpokenContentNodeExtractionTool::
SpokenContentNodeExtractionTool(SpokenContentNodeDescriptorInterfaceABC *SpokenContentNode):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!SpokenContentNode) {
	  SpokenContentNodeDescriptor *descriptor = new SpokenContentNodeDescriptor();
	  SpokenContentNode=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(SpokenContentNode);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentNodeExtractionTool::~SpokenContentNodeExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool SpokenContentNodeExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpokenContentNodeExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentNodeExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
SpokenContentNodeDescriptorInterfaceABC* SpokenContentNodeExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentNodeExtractionTool::
SetDescriptorInterface(SpokenContentNodeDescriptorInterfaceABC
		       *aSpokenContentNodeDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aSpokenContentNodeDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aSpokenContentNodeDescriptorInterface;
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
unsigned long SpokenContentNodeExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long SpokenContentNodeExtractionTool::StartExtracting(void)
{
  // To help debugging
  const char* routine="SpokenContentNodeExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "SpokenContentNode Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------

  // Get the attributes
  string tmp;
  if(m_Media->GetTextAttribute(string("num"),tmp) != 0) {
    fprintf(stderr,"%s Omitted num\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetNum(static_cast<unsigned short> (atoi(tmp.c_str())));

  if(m_Media->GetTextAttribute(string("timeOffset"),tmp) != 0) {
    fprintf(stderr,"%s Omitted timeOffset\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetTimeOffset(static_cast<unsigned short> (atoi(tmp.c_str())));

  if(m_Media->GetTextAttribute(string("speakerInfoRef"),tmp) != 0) {
    m_DescriptorInterface->SetSpeaker(NULL);
  }
  else {m_DescriptorInterface->SetSpeaker(tmp.c_str());}

  // --------------------- Now handle the elements -----------------------

  // Do the word links 
  vector<GenericDS> list;
  unsigned char i;
  list=m_Media->GetAllDescriptions(string("WordLink"));
  m_DescriptorInterface->SetNWordLinks(list.size());
  for(i=0;i<list.size();i++) {
    SpokenContentWordLinkDescriptor* theWlkDescriptor;
    SpokenContentWordLinkDescriptorInterfaceABC* theWlkDescriptorInterface;
    SpokenContentWordLinkExtractionTool* theWlkExtractionTool;
    SpokenContentWordLinkExtractionInterfaceABC* theWlkExtractionInterface;
    theWlkDescriptor = new SpokenContentWordLinkDescriptor();
    theWlkDescriptor->addref();
    theWlkDescriptorInterface = theWlkDescriptor->GetInterface();
    theWlkExtractionTool = new SpokenContentWordLinkExtractionTool(NULL);
    theWlkExtractionInterface = theWlkExtractionTool->GetInterface();

    theWlkExtractionInterface->SetSourceMedia(&(list[i]));
    theWlkExtractionInterface->SetDescriptorInterface(theWlkDescriptorInterface);
    theWlkExtractionInterface->StartExtracting();
    m_DescriptorInterface->SetWordLink(i,theWlkDescriptor);
    theWlkDescriptor->release();
  }

  list=m_Media->GetAllDescriptions(string("PhoneLink"));
  m_DescriptorInterface->SetNPhoneLinks(list.size());
  for(i=0;i<list.size();i++) {
    SpokenContentPhoneLinkDescriptor* thePlkDescriptor;
    SpokenContentPhoneLinkDescriptorInterfaceABC* thePlkDescriptorInterface;
    SpokenContentPhoneLinkExtractionTool* thePlkExtractionTool;
    SpokenContentPhoneLinkExtractionInterfaceABC* thePlkExtractionInterface;
    thePlkDescriptor = new SpokenContentPhoneLinkDescriptor();
    thePlkDescriptor->addref();
    thePlkDescriptorInterface = thePlkDescriptor->GetInterface();
    thePlkExtractionTool = new SpokenContentPhoneLinkExtractionTool(NULL);
    thePlkExtractionInterface = thePlkExtractionTool->GetInterface();

    thePlkExtractionInterface->SetSourceMedia(&(list[i]));
    thePlkExtractionInterface->SetDescriptorInterface(thePlkDescriptorInterface);
    thePlkExtractionInterface->StartExtracting();

    m_DescriptorInterface->SetPhoneLink(i,thePlkDescriptor);
    thePlkDescriptor->release();
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentNodeExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentNodeExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentNodeExtractionInterfaceABC *SpokenContentNodeExtractionTool::GetInterface(void)
{ return &m_Interface; }

