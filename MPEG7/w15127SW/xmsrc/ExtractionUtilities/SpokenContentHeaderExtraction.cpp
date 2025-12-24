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
// Applicable File Name:  SpokenContentHeaderExtraction.cpp
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


const UUID SpokenContentHeaderExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *SpokenContentHeaderExtractionInterface::myName = "Streaming Component Control Interface";

const UUID SpokenContentHeaderExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentHeaderExtractionTool::myName = "SpokenContentHeader Type Descriptor Extractor";

const UUID SpokenContentHeaderExtractionInterfaceABC::myID = UUID();

//=============================================================================
SpokenContentHeaderExtractionInterfaceABC::SpokenContentHeaderExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpokenContentHeaderExtractionInterface::SpokenContentHeaderExtractionInterface(
  SpokenContentHeaderExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentHeaderExtractionInterface::~SpokenContentHeaderExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpokenContentHeaderExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentHeaderExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SpokenContentHeaderExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentHeaderExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
SpokenContentHeaderDescriptorInterfaceABC* SpokenContentHeaderExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentHeaderExtractionInterface::
SetDescriptorInterface(SpokenContentHeaderDescriptorInterfaceABC
		       *aSpokenContentHeaderDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aSpokenContentHeaderDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentHeaderExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long SpokenContentHeaderExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
SpokenContentHeaderExtractionTool::SpokenContentHeaderExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpokenContentHeaderExtractionTool::
SpokenContentHeaderExtractionTool(SpokenContentHeaderDescriptorInterfaceABC *SpokenContentHeader):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!SpokenContentHeader) {
	  SpokenContentHeaderDescriptor *descriptor = new SpokenContentHeaderDescriptor();
	  SpokenContentHeader=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(SpokenContentHeader);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentHeaderExtractionTool::~SpokenContentHeaderExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool SpokenContentHeaderExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpokenContentHeaderExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentHeaderExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
SpokenContentHeaderDescriptorInterfaceABC* SpokenContentHeaderExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentHeaderExtractionTool::
SetDescriptorInterface(SpokenContentHeaderDescriptorInterfaceABC
		       *aSpokenContentHeaderDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aSpokenContentHeaderDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aSpokenContentHeaderDescriptorInterface;
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
unsigned long SpokenContentHeaderExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long SpokenContentHeaderExtractionTool::StartExtracting()
{
  // To help debugging
  const char* routine="SpokenContentHeaderExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "SpokenContentHeader Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------
  unsigned long i;

  // Do the speakers - there must be at least one speaker
  vector<GenericDS> list=m_Media->GetAllDescriptions(string("SpeakerInfo"));
  if(list.size()==0) {
    fprintf(stderr,"%s no speakers defined\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetNSpeaker(list.size());
  for(i=0;i<list.size();i++) {
    SpokenContentSpeakerDescriptor* theSpkDescriptor;
    SpokenContentSpeakerDescriptorInterfaceABC* theSpkDescriptorInterface;
    SpokenContentSpeakerExtractionTool* theSpkExtractionTool;
    SpokenContentSpeakerExtractionInterfaceABC* theSpkExtractionInterface;
    theSpkDescriptor = new SpokenContentSpeakerDescriptor();
    theSpkDescriptor->addref();
    theSpkDescriptorInterface = theSpkDescriptor->GetInterface();
    theSpkExtractionTool = new SpokenContentSpeakerExtractionTool(NULL);
    theSpkExtractionInterface = theSpkExtractionTool->GetInterface();

    theSpkExtractionInterface->SetSourceMedia(&(list[i]));
    theSpkExtractionInterface->SetDescriptorInterface(theSpkDescriptorInterface);
    theSpkExtractionInterface->StartExtracting();

    m_DescriptorInterface->SetSpeaker(i,theSpkDescriptor);
    theSpkDescriptor->release();
  }

  // Do the word lexicons
  list=m_Media->GetAllDescriptions(string("WordLexicon"));
  m_DescriptorInterface->SetNWordLexicon(list.size());
  for(i=0;i<list.size();i++) {
    SpokenContentWordLexiconDescriptor* theWLxDescriptor;
    SpokenContentWordLexiconDescriptorInterfaceABC* theWLxDescriptorInterface;
    SpokenContentWordLexiconExtractionTool* theWLxExtractionTool;
    SpokenContentWordLexiconExtractionInterfaceABC* theWLxExtractionInterface;
    theWLxDescriptor = new SpokenContentWordLexiconDescriptor();
    theWLxDescriptor->addref();
    theWLxDescriptorInterface = theWLxDescriptor->GetInterface();
    theWLxExtractionTool = new SpokenContentWordLexiconExtractionTool(NULL);
    theWLxExtractionInterface = theWLxExtractionTool->GetInterface();

    theWLxExtractionInterface->SetSourceMedia(&(list[i]));
    theWLxExtractionInterface->SetDescriptorInterface(theWLxDescriptorInterface);
    theWLxExtractionInterface->StartExtracting();

    m_DescriptorInterface->SetWordLexicon(i,theWLxDescriptor);
    theWLxDescriptor->release();
  }

  // Do the phone lexicons
  list=m_Media->GetAllDescriptions(string("PhoneLexicon"));
  m_DescriptorInterface->SetNPhoneLexicon(list.size());
  for(i=0;i<list.size();i++) {
    SpokenContentPhoneLexiconDescriptor* thePLxDescriptor;
    SpokenContentPhoneLexiconDescriptorInterfaceABC* thePLxDescriptorInterface;
    SpokenContentPhoneLexiconExtractionTool* thePLxExtractionTool;
    SpokenContentPhoneLexiconExtractionInterfaceABC* thePLxExtractionInterface;
    thePLxDescriptor = new SpokenContentPhoneLexiconDescriptor();
    thePLxDescriptor->addref();
    thePLxDescriptorInterface = thePLxDescriptor->GetInterface();
    thePLxExtractionTool = new SpokenContentPhoneLexiconExtractionTool(NULL);
    thePLxExtractionInterface = thePLxExtractionTool->GetInterface();

    thePLxExtractionInterface->SetSourceMedia(&(list[i]));
    thePLxExtractionInterface->SetDescriptorInterface(thePLxDescriptorInterface);
    thePLxExtractionInterface->StartExtracting();

    m_DescriptorInterface->SetPhoneLexicon(i,thePLxDescriptor);
    thePLxDescriptor->release();
  }

  // Do the confusion info
  list=m_Media->GetAllDescriptions(string("ConfusionInfo"));
  if(list.size() != 0) m_DescriptorInterface->SetNConfusions(list.size());
  for(i=0;i<list.size();i++) {
    SpokenContentConfusionInfoDescriptor* theCIDescriptor;
    SpokenContentConfusionInfoDescriptorInterfaceABC* theCIDescriptorInterface;
    SpokenContentConfusionInfoExtractionTool* theCIExtractionTool;
    SpokenContentConfusionInfoExtractionInterfaceABC* theCIExtractionInterface;
    theCIDescriptor = new SpokenContentConfusionInfoDescriptor();
    theCIDescriptor->addref();
    theCIDescriptorInterface = theCIDescriptor->GetInterface();
    theCIExtractionTool = new SpokenContentConfusionInfoExtractionTool(NULL);
    theCIExtractionInterface = theCIExtractionTool->GetInterface();

    theCIExtractionInterface->SetSourceMedia(&(list[i]));
    theCIExtractionInterface->SetDescriptorInterface(theCIDescriptorInterface);
    theCIExtractionInterface->StartExtracting();

    m_DescriptorInterface->SetConfusion(i,theCIDescriptor);
    theCIDescriptor->release();
  }

  // Sanity check. Must have at least one word or phone lexicon
  assert((m_DescriptorInterface->GetNPhoneLexicon() != 0) ||
         (m_DescriptorInterface->GetNWordLexicon() != 0));

  // All done
  return 0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentHeaderExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentHeaderExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentHeaderExtractionInterfaceABC *SpokenContentHeaderExtractionTool::GetInterface(void)
{ return &m_Interface; }

