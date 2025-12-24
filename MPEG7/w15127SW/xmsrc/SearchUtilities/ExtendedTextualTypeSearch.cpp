////////////////////////////////////////////////////////////////////////
//
// ExtendedTextualTypeSearch.cpp
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


const UUID ExtendedTextualTypeSearchInterface::myID = UUID("");
const char *ExtendedTextualTypeSearchInterface::myName = "Phonetic Translation Hints Search Interface";

const UUID ExtendedTextualTypeSearchTool::myID = UUID("");
const char *ExtendedTextualTypeSearchTool::myName = "Phonetic Translation Hints Search Tool";

const UUID ExtendedTextualTypeSearchInterfaceABC::myID = UUID();

//=============================================================================
ExtendedTextualTypeSearchInterfaceABC::ExtendedTextualTypeSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
ExtendedTextualTypeSearchInterface::ExtendedTextualTypeSearchInterface(ExtendedTextualTypeSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ExtendedTextualTypeSearchInterface::~ExtendedTextualTypeSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& ExtendedTextualTypeSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ExtendedTextualTypeSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void ExtendedTextualTypeSearchInterface::destroy()
{
  delete m_SearchTool;
}

//----------------------------------------------------------------------------
int ExtendedTextualTypeSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC
		       *aRefGenericDSInterface)

{
  return m_SearchTool->
    SetRefDescriptionInterface(aRefGenericDSInterface);
}

//----------------------------------------------------------------------------
int ExtendedTextualTypeSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQueryGenericDSInterface)

{
  return m_SearchTool->
    SetQueryDescriptionInterface(aQueryGenericDSInterface);
}

//----------------------------------------------------------------------------
int ExtendedTextualTypeSearchInterface::
SetQuery(char *aQuery)

{
  return m_SearchTool->SetQuery(aQuery);
}

//----------------------------------------------------------------------------
void ExtendedTextualTypeSearchInterface::GetNodes(NodeList &m_NodeList)

{
  m_SearchTool->GetNodes(m_NodeList);
}


//=============================================================================
ExtendedTextualTypeSearchTool::ExtendedTextualTypeSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ExtendedTextualTypeSearchTool::ExtendedTextualTypeSearchTool(GenericDSInterfaceABC *aDescription):
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
    SetQueryDescriptionInterface(aDescription);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ExtendedTextualTypeSearchTool::~ExtendedTextualTypeSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& ExtendedTextualTypeSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ExtendedTextualTypeSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool ExtendedTextualTypeSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool ExtendedTextualTypeSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
ExtendedTextualTypeSearchInterfaceABC *ExtendedTextualTypeSearchTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
int ExtendedTextualTypeSearchTool::
SetRefDescriptionInterface(GenericDSInterfaceABC
		       *aRefGenericDSInterface)

{
	m_RefDescriptionInterface = aRefGenericDSInterface;
	return 0;
}

//----------------------------------------------------------------------------
// Attempt to build a full query from the Query XML file input.
//----------------------------------------------------------------------------
int  ExtendedTextualTypeSearchTool::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQueryGenericDSInterface)
{
	m_QueryDescriptionInterface = aQueryGenericDSInterface;
	return 0;
}

//----------------------------------------------------------------------------
// Get Tagname and PhoneticTranscription from the Query input string.
//----------------------------------------------------------------------------
int  ExtendedTextualTypeSearchTool::SetQuery(char *aQuery)
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
void ExtendedTextualTypeSearchTool::GetNodes(NodeList &m_NodeList)
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
	

	// Get relative SearchPath 
	if (!m_tagname.compare("Title")) 
		path2="/Audio/CreationInformation/Creation/Title";
	else if (!m_tagname.compare("Genre"))
		path2="/Audio/CreationInformation/Classification/Genre";
	else if (!m_tagname.compare("Name"))
		path2="/Audio/CreationInformation/Creation/Creator/PersonGroup/Name";
		
	counter = 0;
	counter2 = 0;
	GenericDS testDS;
	GenericDS testDS2;
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
				if (Item.GetTextAttribute("PhoneticDescription",content)==0) {
					if (!m_phtranscription.compare(content)) {
						m_NodeList.Append(SongContent,"");
					}
				}
			counter2++;
			SongContent = DOMNav.GetChildByTagName(AlbumContent,"Content",counter2);	
		}
		counter2=0;
		counter++;
		AlbumContent = DOMNav.GetChildByTagName(testDS2,"Collection",counter);
	}
}

