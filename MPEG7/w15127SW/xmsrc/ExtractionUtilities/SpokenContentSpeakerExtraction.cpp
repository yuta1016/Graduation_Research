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
// Applicable File Name:  SpokenContentSpeakerExtraction.cpp
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


const UUID SpokenContentSpeakerExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *SpokenContentSpeakerExtractionInterface::myName = "Streaming Component Control Interface";

const UUID SpokenContentSpeakerExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentSpeakerExtractionTool::myName = "SpokenContentSpeaker Type Descriptor Extractor";

const UUID SpokenContentSpeakerExtractionInterfaceABC::myID = UUID();

//=============================================================================
SpokenContentSpeakerExtractionInterfaceABC::SpokenContentSpeakerExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpokenContentSpeakerExtractionInterface::SpokenContentSpeakerExtractionInterface(
  SpokenContentSpeakerExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentSpeakerExtractionInterface::~SpokenContentSpeakerExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpokenContentSpeakerExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentSpeakerExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SpokenContentSpeakerExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentSpeakerExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
SpokenContentSpeakerDescriptorInterfaceABC* SpokenContentSpeakerExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentSpeakerExtractionInterface::
SetDescriptorInterface(SpokenContentSpeakerDescriptorInterfaceABC
		       *aSpokenContentSpeakerDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aSpokenContentSpeakerDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentSpeakerExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long SpokenContentSpeakerExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
SpokenContentSpeakerExtractionTool::SpokenContentSpeakerExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpokenContentSpeakerExtractionTool::
SpokenContentSpeakerExtractionTool(SpokenContentSpeakerDescriptorInterfaceABC *SpokenContentSpeaker):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!SpokenContentSpeaker) {
	  SpokenContentSpeakerDescriptor *descriptor = new SpokenContentSpeakerDescriptor();
	  SpokenContentSpeaker=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(SpokenContentSpeaker);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentSpeakerExtractionTool::~SpokenContentSpeakerExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool SpokenContentSpeakerExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpokenContentSpeakerExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentSpeakerExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
SpokenContentSpeakerDescriptorInterfaceABC* SpokenContentSpeakerExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentSpeakerExtractionTool::
SetDescriptorInterface(SpokenContentSpeakerDescriptorInterfaceABC
		       *aSpokenContentSpeakerDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aSpokenContentSpeakerDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aSpokenContentSpeakerDescriptorInterface;
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
unsigned long SpokenContentSpeakerExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long SpokenContentSpeakerExtractionTool::StartExtracting(void)
{
  // To help debugging
  const char* routine="SpokenContentSpeakerExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "SpokenContentSpeaker Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------

  // ------------------------- Do the attributes ---------------------------

  // get the id
  string id;
  if(m_Media->GetTextAttribute(string("id"),id) != 0) {
    fprintf(stderr,"%s id was omitted\n",routine);
    return(failure);
  }
  m_DescriptorInterface->SetSpeakerID(id.c_str());

  // Get the phoneLexiconId - this is optional
  string tmp;
  if(m_Media->GetTextAttribute(string("phoneLexiconRef"),tmp) == 0) {
    m_DescriptorInterface->SetPhoneLexiconID(tmp.c_str());
  }

  // Get the wordLexiconId - this is optional
  if(m_Media->GetTextAttribute(string("wordLexiconRef"),tmp) == 0) {
    m_DescriptorInterface->SetWordLexiconID(tmp.c_str());
  }

  // Get the phone confusions Id - this is optional but only allowed if phones
  if(m_Media->GetTextAttribute(string("confusionInfoRef"),tmp) == 0) {
    assert(m_DescriptorInterface->GetPhoneLexiconID() != NULL);
    m_DescriptorInterface->SetPhoneConfID(tmp.c_str());
  }

  // Get the provenance - this is required. Should be unknown, ASR, manual,
  // keyword or parsing
  if(m_Media->GetTextAttribute(string("provenance"),tmp) != 0) {
    fprintf(stderr,"%s provenance omitted\n",routine);
    return(failure);
  }
  if((tmp != "unknown") && (tmp != "ASR") && (tmp != "manual") && 
     (tmp != "keyword") && (tmp != "parsing")) {
    fprintf(stderr,"%s provenance unknown: %s\n",routine,tmp.c_str());
    return(failure);
  }
  m_DescriptorInterface->SetProvenance(tmp.c_str());

  // Get the lang - this is required
  vector<GenericDS> list=m_Media->GetAllDescriptions(string("SpokenLanguage"));
  if(list.size()==0) {
    fprintf(stderr,"%s SpokenLanguage unspecified\n",routine);
    return(failure);    
  }
  else if(list.size() != 1) {
    fprintf(stderr,"%s Too many SpokenLanguages\n",routine);
    return(failure);
  }    
  else {
    string splang;
    list[0].GetTextValue(splang);
    m_DescriptorInterface->SetLang(splang.c_str());
  }

#if 0 // PNG
  // Handle the components for individual - optional
  list=m_Media->GetAllDescriptions(string("Individual"));
  if(list.size()==0) {
    m_DescriptorInterface->SetIndividual(NULL);
  }
  else if(list.size() != 1) {
    fprintf(stderr,"%s Too many individuals\n",routine);
    return(failure);
  }    
  else {
    IndividualDescriptor* theIndDescriptor;
    IndividualDescriptorInterfaceABC* theIndDescriptorInterface;
    IndividualExtractionTool* theIndExtractionTool;
    IndividualExtractionInterfaceABC* theIndExtractionInterface;
    theIndDescriptor = new IndividualDescriptor();
    theIndDescriptor->addref();
    theIndDescriptorInterface = theIndDescriptor->GetInterface();
    theIndExtractionTool = new IndividualExtractionTool(NULL);
    theIndExtractionInterface = theIndExtractionTool->GetInterface();

    theIndExtractionInterface->SetSourceMedia(&(list[0]));
    theIndExtractionInterface->SetDescriptorInterface(theIndDescriptorInterface);
    theIndExtractionInterface->StartExtracting();

    m_DescriptorInterface->SetIndividual(theIndDescriptor);
    theIndDescriptor->release();
  }
#endif

#if 0 // PNG
  // Handle the components for extractionInfo - optional
  list=m_Media->GetAllDescriptions(string("ExtractionInfo"));
  if(list.size()==0) {
    m_DescriptorInterface->SetExtractionInfo(NULL);
  }
  else if(list.size() != 1) {
    fprintf(stderr,"%s Too many extraction infos\n",routine);
    return(failure);
  }    
  else {
    SpokenContentExtractionInfoDescriptor* theExtDescriptor;
    SpokenContentExtractionInfoDescriptorInterfaceABC* theExtDescriptorInterface;
    SpokenContentExtractionInfoExtractionTool* theExtExtractionTool;
    SpokenContentExtractionInfoExtractionInterfaceABC* theExtExtractionInterface;
    theExtDescriptor = new SpokenContentExtractionInfoDescriptor();
    theExtDescriptor->addref();
    theExtDescriptorInterface = theExtDescriptor->GetInterface();
    theExtExtractionTool = new SpokenContentExtractionInfoExtractionTool(NULL);
    theExtExtractionInterface = theExtExtractionTool->GetInterface();

    theExtExtractionInterface->SetSourceMedia(&(list[0]));
    theExtExtractionInterface->SetDescriptorInterface(theExtDescriptorInterface);
    theExtExtractionInterface->StartExtracting();

    m_DescriptorInterface->SetExtractionInfo(theExtDescriptor);
    theExtDescriptor->release();
  }
#endif

  // Get the phone index entries
  unsigned int i;
  list=m_Media->GetAllDescriptions(string("PhoneIndex"));
  if((list.size() != 0) && (list.size() != 1)) {
    fprintf(stderr,"%s only permit zero or one phone index: %d\n",routine,
            list.size());
    return(failure);
  }
  if(list.size()==1) {
    // Get the default Lattice Id attribute
    if(list[0].GetTextAttribute(string("defaultLattice"),tmp) != 0) {
      fprintf(stderr,"%s defaultLattice (phone) omitted\n",routine);
      return(failure);
    }
    m_DescriptorInterface->SetDefaultPhoneLatticeId(tmp.c_str());

    list=list[0].GetAllDescriptions(string("PhoneIndexEntry"));
    if(list.size() == 0) {
      fprintf(stderr,"%s require some content\n",routine);
      return(failure);
    }
    m_DescriptorInterface->SetNPhoneIndex(list.size());
    for(i=0;i<list.size();i++) {
      SpokenContentPhoneIndexEntryDescriptor* thePInDescriptor;
      SpokenContentPhoneIndexEntryDescriptorInterfaceABC* 
                                                    thePInDescriptorInterface;
      SpokenContentPhoneIndexEntryExtractionTool* thePInExtractionTool;
      SpokenContentPhoneIndexEntryExtractionInterfaceABC* 
                                                    thePInExtractionInterface;
      thePInDescriptor = new SpokenContentPhoneIndexEntryDescriptor();
      thePInDescriptor->addref();
      thePInDescriptorInterface = thePInDescriptor->GetInterface();
      thePInExtractionTool =new SpokenContentPhoneIndexEntryExtractionTool(NULL);
      thePInExtractionInterface = thePInExtractionTool->GetInterface();

      thePInExtractionInterface->SetSourceMedia(&(list[i]));
      thePInExtractionInterface->SetDescriptorInterface(thePInDescriptorInterface);
      thePInExtractionInterface->StartExtracting();

      m_DescriptorInterface->SetPhoneIndex(i,thePInDescriptor);
      thePInDescriptor->release();
    }
  }

  // Get the phone index entries
  list=m_Media->GetAllDescriptions(string("WordIndex"));
  if((list.size() != 0) && (list.size() != 1)) {
    fprintf(stderr,"%s only permit zero or one word index: %d\n",routine,
      list.size());
    return(failure);
  }
  if(list.size() == 1) {
    // Get the default Lattice Id attribute
    if(list[0].GetTextAttribute(string("defaultLattice"),tmp) != 0) {
      fprintf(stderr,"%s defaultLattice (word) omitted\n",routine);
      return(failure);
    }
    m_DescriptorInterface->SetDefaultWordLatticeId(tmp.c_str());

    list=list[0].GetAllDescriptions(string("WordIndexEntry"));
    if(list.size() == 0) {
      fprintf(stderr,"%s require some content\n",routine);
      return(failure);
    }
    m_DescriptorInterface->SetNWordIndex(list.size());
    for(i=0;i<list.size();i++) {
      SpokenContentWordIndexEntryDescriptor* theWInDescriptor;
      SpokenContentWordIndexEntryDescriptorInterfaceABC* 
                                                    theWInDescriptorInterface;
      SpokenContentWordIndexEntryExtractionTool* theWInExtractionTool;
      SpokenContentWordIndexEntryExtractionInterfaceABC* 
                                                    theWInExtractionInterface;
      theWInDescriptor = new SpokenContentWordIndexEntryDescriptor();
      theWInDescriptor->addref();
      theWInDescriptorInterface = theWInDescriptor->GetInterface();
      theWInExtractionTool =new SpokenContentWordIndexEntryExtractionTool(NULL);
      theWInExtractionInterface = theWInExtractionTool->GetInterface();

      theWInExtractionInterface->SetSourceMedia(&(list[i]));
      theWInExtractionInterface->SetDescriptorInterface(theWInDescriptorInterface);
      theWInExtractionInterface->StartExtracting();

      m_DescriptorInterface->SetWordIndex(i,theWInDescriptor);
      theWInDescriptor->release();
    }
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentSpeakerExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentSpeakerExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentSpeakerExtractionInterfaceABC *SpokenContentSpeakerExtractionTool::GetInterface(void)
{ return &m_Interface; }

