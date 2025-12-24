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
// Applicable File Name:  SpokenContentBlockExtraction.cpp
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


const UUID SpokenContentBlockExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *SpokenContentBlockExtractionInterface::myName = "Streaming Component Control Interface";

const UUID SpokenContentBlockExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentBlockExtractionTool::myName = "SpokenContentBlock Type Descriptor Extractor";

const UUID SpokenContentBlockExtractionInterfaceABC::myID = UUID();

//=============================================================================
SpokenContentBlockExtractionInterfaceABC::SpokenContentBlockExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpokenContentBlockExtractionInterface::SpokenContentBlockExtractionInterface(
  SpokenContentBlockExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentBlockExtractionInterface::~SpokenContentBlockExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpokenContentBlockExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentBlockExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SpokenContentBlockExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentBlockExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
SpokenContentBlockDescriptorInterfaceABC* SpokenContentBlockExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentBlockExtractionInterface::
SetDescriptorInterface(SpokenContentBlockDescriptorInterfaceABC
		       *aSpokenContentBlockDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aSpokenContentBlockDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentBlockExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long SpokenContentBlockExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
SpokenContentBlockExtractionTool::SpokenContentBlockExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpokenContentBlockExtractionTool::
SpokenContentBlockExtractionTool(SpokenContentBlockDescriptorInterfaceABC *SpokenContentBlock):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!SpokenContentBlock) {
	  SpokenContentBlockDescriptor *descriptor = new SpokenContentBlockDescriptor();
	  SpokenContentBlock=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(SpokenContentBlock);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentBlockExtractionTool::~SpokenContentBlockExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool SpokenContentBlockExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpokenContentBlockExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentBlockExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
SpokenContentBlockDescriptorInterfaceABC* SpokenContentBlockExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentBlockExtractionTool::
SetDescriptorInterface(SpokenContentBlockDescriptorInterfaceABC
		       *aSpokenContentBlockDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aSpokenContentBlockDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aSpokenContentBlockDescriptorInterface;
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
unsigned long SpokenContentBlockExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long SpokenContentBlockExtractionTool::StartExtracting(void)
{
  // To help debugging
  const char* routine="SpokenContentBlockExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "SpokenContentBlock Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------

  // ------------------- Get the attributes --------------------------------
  string tmp;
  vector<GenericDS> list;

  // Num is required and should be a ushort
  if(m_Media->GetTextAttribute(string("num"),tmp) != 0) {
    fprintf(stderr,"%s num omitted\n",routine);
    return(failure);
  }
  long l=static_cast<unsigned short> (atol(tmp.c_str()));
  if((l<0) || (l>65535)) {
    fprintf(stderr,"%s num out of range\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetNum(static_cast<unsigned long> (l));

  // defaultSpeaker is required
  if(m_Media->GetTextAttribute(string("defaultSpeakerInfoRef"),tmp) != 0) {
    fprintf(stderr,"%s defaultSpeakerInfoRef omitted\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetDefaultSpeaker(tmp.c_str());

  // Get the audio - must be 'unknown', 'speech', 'noise' or 'noisySpeech'.
  // Default is "speech"
  if(m_Media->GetTextAttribute(string("audio"),tmp) != 0) {
    tmp="speech";
  }
  else if((tmp != "unknown") && (tmp != "speech") && (tmp != "noise") &&
     (tmp != "noisySpeech")) {
    fprintf(stderr,"%s audio not enumerated selection\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetAudio(tmp.c_str());

  // Handle the media time - currently treat as a string - sort out later
  list=m_Media->GetAllDescriptions(string("MediaTime"));
  if(list.size() != 1) {
    fprintf(stderr,"%s Incorrect number of media times\n",routine);
    return(failure);
  }
  if(list[0].GetTextValue(tmp) != 0) {
    fprintf(stderr,"%s Media time omitted\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetMediaTime(tmp.c_str());

  // Get the nodes
  list=m_Media->GetAllDescriptions(string("Node"));
  if(list.size()==0) {
    fprintf(stderr,"%s Nodes omitted\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetNNodes(list.size());
  for(unsigned int i=0;i<list.size();i++) {
    SpokenContentNodeDescriptor* theNodeDescriptor;
    SpokenContentNodeDescriptorInterfaceABC* theNodeDescriptorInterface;
    SpokenContentNodeExtractionTool* theNodeExtractionTool;
    SpokenContentNodeExtractionInterfaceABC* theNodeExtractionInterface;
    theNodeDescriptor = new SpokenContentNodeDescriptor();
    theNodeDescriptor->addref();
    theNodeDescriptorInterface = theNodeDescriptor->GetInterface();
    theNodeExtractionTool = new SpokenContentNodeExtractionTool(NULL);
    theNodeExtractionInterface = theNodeExtractionTool->GetInterface();

    theNodeExtractionInterface->SetSourceMedia(&(list[i]));
    theNodeExtractionInterface->SetDescriptorInterface(theNodeDescriptorInterface);
    theNodeExtractionInterface->StartExtracting();

    m_DescriptorInterface->SetNode(i,theNodeDescriptor);
    theNodeDescriptor->release();
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentBlockExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentBlockExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentBlockExtractionInterfaceABC *SpokenContentBlockExtractionTool::GetInterface(void)
{ return &m_Interface; }

