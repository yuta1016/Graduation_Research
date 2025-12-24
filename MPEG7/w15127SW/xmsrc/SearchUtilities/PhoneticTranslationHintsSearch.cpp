////////////////////////////////////////////////////////////////////////
//
// PhoneticTranslationHintsSearch.cpp
//
// This software module was originally developed by
//
// Andre Mengel, Bosch GmbH, Hildesheim
// 
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938). 
// This software module is an implementation of a part of one or more 
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this 
// software module or modifications thereof for use in hardware or 
// software products claiming conformance to the MPEG-7 standard. 
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents. 
// The original developer of this software module and his/her company, 
// the subsequent editors and their companies, and ISO/IEC have no 
// liability for use of this software module or modifications thereof 
// in an application. No license to this software is granted for non 
// MPEG-7 conforming products. 
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module 
// to a third party and to inhibit third parties from using the software 
// module for non MPEG-7 conforming products. 
//
// Copyright (c) 2001
//
// This copyright notice must be included in all copies or derivative 
// works of this software module.
//
////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <math.h>
#include <assert.h>
#ifdef _VISUALC_
#include <sstream>
#endif
#include "DescriptionSchemes/DescriptionSchemes.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
	

using namespace XM;
using namespace std;


const UUID PhoneticTranslationHintsSearchInterface::myID = UUID("");
const char *PhoneticTranslationHintsSearchInterface::myName = "Phonetic Translation Hints Search Interface";//sth please remove blanks

const UUID PhoneticTranslationHintsSearchTool::myID = UUID("");
const char *PhoneticTranslationHintsSearchTool::myName = "Phonetic Translation Hints Search Tool";

const UUID PhoneticTranslationHintsSearchInterfaceABC::myID = UUID();

//=============================================================================
PhoneticTranslationHintsSearchInterfaceABC::PhoneticTranslationHintsSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
PhoneticTranslationHintsSearchInterface::PhoneticTranslationHintsSearchInterface(PhoneticTranslationHintsSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
PhoneticTranslationHintsSearchInterface::~PhoneticTranslationHintsSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& PhoneticTranslationHintsSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PhoneticTranslationHintsSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void PhoneticTranslationHintsSearchInterface::destroy()
{
  delete m_SearchTool;
}

//----------------------------------------------------------------------------
int PhoneticTranslationHintsSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC
		       *aRefGenericDSInterface)

{
  return m_SearchTool->
    SetRefDescriptionInterface(aRefGenericDSInterface);
}

//----------------------------------------------------------------------------
int PhoneticTranslationHintsSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQueryGenericDSInterface)

{
  return m_SearchTool->
    SetQueryDescriptionInterface(aQueryGenericDSInterface);
}

//----------------------------------------------------------------------------
int PhoneticTranslationHintsSearchInterface::
SetQuery(char *aQuery)

{
  return m_SearchTool->SetQuery(aQuery);
}

//----------------------------------------------------------------------------
void PhoneticTranslationHintsSearchInterface::GetNodes(NodeList &m_NodeList)

{
  m_SearchTool->GetNodes(m_NodeList);
}


//=============================================================================
PhoneticTranslationHintsSearchTool::PhoneticTranslationHintsSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
PhoneticTranslationHintsSearchTool::PhoneticTranslationHintsSearchTool(GenericDSInterfaceABC *aDescription):
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
    SetQueryDescriptionInterface(aDescription);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
PhoneticTranslationHintsSearchTool::~PhoneticTranslationHintsSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& PhoneticTranslationHintsSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PhoneticTranslationHintsSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool PhoneticTranslationHintsSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool PhoneticTranslationHintsSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
PhoneticTranslationHintsSearchInterfaceABC *PhoneticTranslationHintsSearchTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
int PhoneticTranslationHintsSearchTool::
SetRefDescriptionInterface(GenericDSInterfaceABC
		       *aRefGenericDSInterface)

{
	m_RefDescriptionInterface = aRefGenericDSInterface;
	return 0;
}

//----------------------------------------------------------------------------
// Attempt to build a full query from the Query XML file input.
//----------------------------------------------------------------------------
int  PhoneticTranslationHintsSearchTool::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQueryGenericDSInterface)
{
	m_QueryDescriptionInterface = aQueryGenericDSInterface;
	return 0;
}

//----------------------------------------------------------------------------
// Get Tagname and PhoneticTranscription from the Query input string.
//----------------------------------------------------------------------------
int  PhoneticTranslationHintsSearchTool::SetQuery(char *aQuery)
{
	char *pointer;
	int counter;
	pointer = aQuery;
	counter=0;
	while( *pointer != ':' ) {
		pointer++;
		counter++;
	}	
	m_tagname = string(aQuery,counter);
	
	pointer++;
	aQuery = pointer;
	counter=0;
	while( *pointer != '\0' ) {
		pointer++;
		counter++;
	}	
	m_phtranscription = string(aQuery,counter);
	return 0;
}

//----------------------------------------------------------------------------
// Evaluate the query object on the reference DOM document.
//----------------------------------------------------------------------------
void PhoneticTranslationHintsSearchTool::GetNodes(NodeList &m_NodeList)
{
	int counter = 0;
	int counter2;
	int numberofchilds;
	char *path;
	char *path2;
	string attribute;
	string ptattribute;
	string content;
	DOMNavigator DOMNav;
	// Use Dictionary to Get appropriate string from m_phtranscription
	GenericDS testDS = m_QueryDescriptionInterface->GetChild((int)0);
	GenericDS testDS2;
	numberofchilds=testDS.GetNumberOfChildren();
	
	// Compare phtranscription with every entry in Dictionary
	do {
		counter++;
		if (counter > numberofchilds) {
			cerr << "Could not find phonetic transcription" << endl;
			exit(-1);
		}
		testDS2 = testDS.GetChild(counter-1);
		testDS2.GetTextAttribute("Phonetic_Translation",ptattribute);
	} while (ptattribute.compare(m_phtranscription));
	testDS2.GetTextValue(m_content);
	

	// Get relative SearchPath 
	if (!m_tagname.compare("Title")) 
		path2="/Audio/CreationInformation/Creation/Title";
	else if (!m_tagname.compare("Genre"))
		path2="/Audio/CreationInformation/Classification/Genre";
	else if (!m_tagname.compare("Name"))
		path2="/Audio/CreationInformation/Creation/Creator/PersonGroup/Name";
		
	counter = 0;
	counter2 = 0;
	GenericDS AlbumContent;
	GenericDS SongContent;
	GenericDS Item;
	testDS = m_RefDescriptionInterface->GetChild((int)0);
	path = "/ContentDescription/AudioVisualContent/ContentCollection";
	testDS2 = DOMNav.GetNode(testDS,path,attribute,"");
	AlbumContent = DOMNav.GetChildByTagName(testDS2,"Collection",counter);
	// loop within Collection
	while (! AlbumContent.isNull()) {
		SongContent = DOMNav.GetChildByTagName(AlbumContent,"Content",counter2);	
		// loop within Content
		while (! SongContent.isNull()) {
			// matching 
			Item = DOMNav.GetNode(SongContent,path2,attribute,"");
			Item.GetTextValue(content);
			if (!m_content.compare(content)) {
				m_NodeList.Append(SongContent,"");
			}

			counter2++;
			SongContent = DOMNav.GetChildByTagName(AlbumContent,"Content",counter2);	
		}
		counter2=0;
		counter++;
		AlbumContent = DOMNav.GetChildByTagName(testDS2,"Collection",counter);
	}
}

