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
// Applicable File Name:  SpokenContentPhoneIndexEntryExtraction.cpp
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


const UUID SpokenContentPhoneIndexEntryExtractionInterface::myID = UUID("290d7900-db6e-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneIndexEntryExtractionInterface::myName = "Streaming Component Control Interface";

const UUID SpokenContentPhoneIndexEntryExtractionTool::myID = UUID("bff2b340-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneIndexEntryExtractionTool::myName = "SpokenContentPhoneIndexEntry Type Descriptor Extractor";

const UUID SpokenContentPhoneIndexEntryExtractionInterfaceABC::myID = UUID();

//=============================================================================
SpokenContentPhoneIndexEntryExtractionInterfaceABC::SpokenContentPhoneIndexEntryExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpokenContentPhoneIndexEntryExtractionInterface::SpokenContentPhoneIndexEntryExtractionInterface(
  SpokenContentPhoneIndexEntryExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryExtractionInterface::~SpokenContentPhoneIndexEntryExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneIndexEntryExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneIndexEntryExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentPhoneIndexEntryExtractionInterface::SetSourceMedia(GenericDS* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryDescriptorInterfaceABC* SpokenContentPhoneIndexEntryExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentPhoneIndexEntryExtractionInterface::
SetDescriptorInterface(SpokenContentPhoneIndexEntryDescriptorInterfaceABC
		       *aSpokenContentPhoneIndexEntryDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(
                                           aSpokenContentPhoneIndexEntryDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentPhoneIndexEntryExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long SpokenContentPhoneIndexEntryExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//=============================================================================
SpokenContentPhoneIndexEntryExtractionTool::SpokenContentPhoneIndexEntryExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpokenContentPhoneIndexEntryExtractionTool::
SpokenContentPhoneIndexEntryExtractionTool(SpokenContentPhoneIndexEntryDescriptorInterfaceABC *SpokenContentPhoneIndexEntry):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	// create descriptor if it does not exist*/
	if(!SpokenContentPhoneIndexEntry) {
	  SpokenContentPhoneIndexEntryDescriptor *descriptor = new SpokenContentPhoneIndexEntryDescriptor();
	  SpokenContentPhoneIndexEntry=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes*/
	SetDescriptorInterface(SpokenContentPhoneIndexEntry);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryExtractionTool::~SpokenContentPhoneIndexEntryExtractionTool()
{
	// release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool SpokenContentPhoneIndexEntryExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpokenContentPhoneIndexEntryExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. Here
// it's just a dom node
int SpokenContentPhoneIndexEntryExtractionTool::SetSourceMedia(GenericDS* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryDescriptorInterfaceABC* SpokenContentPhoneIndexEntryExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentPhoneIndexEntryExtractionTool::
SetDescriptorInterface(SpokenContentPhoneIndexEntryDescriptorInterfaceABC
		       *aSpokenContentPhoneIndexEntryDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == aSpokenContentPhoneIndexEntryDescriptorInterface) {
          return 0;
	}

	/* release old interface*/
	if (m_DescriptorInterface) m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aSpokenContentPhoneIndexEntryDescriptorInterface;
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
unsigned long SpokenContentPhoneIndexEntryExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the dummyXML
unsigned long SpokenContentPhoneIndexEntryExtractionTool::StartExtracting(void)
{
  // To help debugging
  const char* routine="SpokenContentPhoneIndexEntryExtractionTool::StartExtracting:";

  // I do not understand this return value at all!
  const unsigned long failure=(unsigned long)-1;

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface null\n",routine);
    return(failure);
  }

  if(strcmp(m_DescriptorInterface->GetName(),
	    "SpokenContentPhoneIndexEntry Type Description Interface") != 0) {
    fprintf(stderr,"%s Name is wrong\n",routine);
    return(failure);
  }

  if(m_Media==NULL) {
    fprintf(stderr,"%s media is null\n",routine);
    return(failure);
  }

  // ------------------------- Finished the checks -------------------------

  // Get the key attribute
  string key;
  if(m_Media->GetTextAttribute(string("key"),key) != 0) {
    fprintf(stderr,"%s omitted key\n",routine);
    return(failure);
  }
  unsigned char nk=0;
  char *b=new char[strlen(key.c_str())+1];
  strcpy(b,key.c_str());
  char *p=strtok(b," \n\t");
  
  while(p != NULL) {
    nk++;
    p=strtok(NULL," \n\t");
  }
  if(nk==0) {
    fprintf(stderr,"%s key must contain at least one token\n",routine);
    return(failure);
  }

  m_DescriptorInterface->SetNKey(nk);
  nk=0;
  strcpy(b,key.c_str());
  p=strtok(b," \n\t");
  while(p != NULL) {
    long phoneIndex = atol(p);
    if(phoneIndex == -1)
      {
	fprintf(stderr, "%s: invalid phone index \"%s\" in key.\n", routine, p);
	return failure;
      }
    m_DescriptorInterface->SetKeyIndex(nk++, phoneIndex);
    //m_DescriptorInterface->SetKey(nk++, p);
    p=strtok(NULL," \n\t");
  }
  delete [] b;

  // Get the entries
  vector<GenericDS> list=m_Media->GetAllDescriptions(string("IndexEntry"));
  if(list.size()==0) {
    fprintf(stderr,"%s must contain at least one entry\n",routine);
    return(failure);
  }

  m_DescriptorInterface->SetNEntries(list.size());
  for(unsigned int i=0;i<list.size();i++) {
    SpokenContentIndexEntryDescriptor* theDescriptor;
    SpokenContentIndexEntryDescriptorInterfaceABC* theDescriptorInterface;
    SpokenContentIndexEntryExtractionTool* theExtractionTool;
    SpokenContentIndexEntryExtractionInterfaceABC* theExtractionInterface;
    theDescriptor = new SpokenContentIndexEntryDescriptor();
    theDescriptor->addref();
    theDescriptorInterface = theDescriptor->GetInterface();
    theExtractionTool = new SpokenContentIndexEntryExtractionTool(NULL);
    theExtractionInterface = theExtractionTool->GetInterface();

    theExtractionInterface->SetSourceMedia(&(list[i]));
    theExtractionInterface->SetDescriptorInterface(theDescriptorInterface);
    theExtractionInterface->StartExtracting();

    m_DescriptorInterface->SetEntry(i,theDescriptor);
    theDescriptor->release();
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneIndexEntryExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneIndexEntryExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryExtractionInterfaceABC *SpokenContentPhoneIndexEntryExtractionTool::GetInterface(void)
{ return &m_Interface; }

