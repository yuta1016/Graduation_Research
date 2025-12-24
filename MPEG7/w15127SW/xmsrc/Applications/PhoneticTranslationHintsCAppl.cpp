////////////////////////////////////////////////////////////////////////
//
// PhoneticTranslationHintsCAppl.cpp
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
#include <momusys.h>
//#include <vopio.h>
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif


#include "Applications/ClientApplications.h"
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"        /* Naming Convention */
#include "Utilities/Utilities.h"				/* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Decoders/Decoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */

#include <iostream>
#include <fstream>

#include <iostream>
#include <string>



using namespace XM;
using namespace std;

const UUID PhoneticTranslationHintsClient::myID = UUID("");
const char *PhoneticTranslationHintsClient::myName = "Phonetic Translation Hints DS Client";

//=============================================================================
PhoneticTranslationHintsClient::PhoneticTranslationHintsClient():
m_RamDB(),
NoOfElements(0),
m_DBDescription(0)
{
}

//----------------------------------------------------------------------------
PhoneticTranslationHintsClient::~PhoneticTranslationHintsClient()
{
}

//----------------------------------------------------------------------------
unsigned long PhoneticTranslationHintsClient::Open(char *ListFile)
{

	// ---------------------------------------------------------------------------------------
	// Declarations of variables
	// ---------------------------------------------------------------------------------------
	// Generic Description Scheme Interface
 	GenericDSInterfaceABC *theDescriptionInterface;

	// Generic Coding Scheme
	GenericDSCS *theCS;
	GenericDSCSInterfaceABC *theCSInterface;

	// ---------------------------------------------------------------------------------------
	// Connect Interfaces 
	// ---------------------------------------------------------------------------------------

	// Get coding scheme interface
	theCS = new GenericDSCS;
	theCSInterface = theCS->GetInterface();

	// The DS interface (theInterface) is set within the loop


	// ---------------------------------------------------------------------------------------
	// Read database in ListFile
	// ---------------------------------------------------------------------------------------

	// Read filename data base
	m_RamDB.CreateDatabase(ListFile);
	NoOfElements=m_RamDB.GetNoOfElements();

	// Create in memory decompressed database
	// allocate memories
	m_DBDescription =
	  new GenericDS [NoOfElements];

	for (int i=0; i<NoOfElements; i++)
	{

		// Set the description file
		char XMLFile[MAXMEDIANAMELEN];		// Description XML File
		strcpy(XMLFile, m_RamDB.GotoEntry(i)->GetFilename());

		// Connect description file to coding scheme
		theCSInterface->SetDecoderFileName(XMLFile);

		// Get ProgramDS interface
		theDescriptionInterface = m_DBDescription[i].GetInterface();

		// Connect Generic DS to coding schemes
		theCSInterface->
		  SetDescriptionInterface(theDescriptionInterface);

		// Decode Generic DS
		theCSInterface->StartDecode();
		if ( theDescriptionInterface->isNull() )
			cerr << "Could not decode " << XMLFile << endl;
	}

	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long PhoneticTranslationHintsClient::Start(char *Query)
{

  //sth bad contrast to function delimiter. Its hard for me to navigate
	// ---------------------------------------------------------------------------------------
	// Declarations of variables
	// ---------------------------------------------------------------------------------------

	// Descriptors Schemes for query and elements in database
	GenericDS theDescription;
 	GenericDSInterfaceABC *theDescriptionInterface;
 	GenericDSInterfaceABC *DBDescriptionInterface;

	// Generic Coding Scheme for dictionary only
	GenericDSCS *theCS;
	GenericDSCSInterfaceABC *theCSInterface;

	// Search Tool
	PhoneticTranslationHintsSearchTool *theSearchTool;
	PhoneticTranslationHintsSearchInterfaceABC *theSearchInterface;


	// ---------------------------------------------------------------------------------------
	// Connect Interfaces 
	// ---------------------------------------------------------------------------------------
	
	// Get description scheme interface
 	theDescriptionInterface = theDescription.GetInterface();

	// Get coding scheme interface
	theCS = new GenericDSCS(theDescriptionInterface);
	theCSInterface = theCS->GetInterface();
	char *dictionary = getpara("dictionary","");
	if (!(dictionary && *dictionary)) {
		cerr << "Unable to find Dictionary DS: "<< endl;
		exit(-1);
	}

	theCSInterface->SetDecoderFileName(dictionary);
	theCSInterface->StartDecode();
	if ( theDescriptionInterface->isNull() ) {
			cerr << "Could not decode " << dictionary << endl;
			exit(-1);
	}
	
	
	// Connect DS to SearchTool
	theSearchTool = new PhoneticTranslationHintsSearchTool(theDescriptionInterface);
	theSearchInterface = theSearchTool->GetInterface();
	theSearchInterface->SetQuery(Query);


	// ---------------------------------------------------------------------------------------
	// Build query
	// ---------------------------------------------------------------------------------------

	// ---------------------------------------------------------------------------------------
	// Match query with each item of database and print results
	// ---------------------------------------------------------------------------------------
	{
		int j;
		int i;
		GenericDS testDS;
		GenericDS testDS2;
		string attribute;
		NodeList m_NodeList;
		char *path;
		string content;
				

		for (j=0; j<NoOfElements; j++) {
			
			// Connect reference description scheme to search tool
			DBDescriptionInterface=m_DBDescription[j].GetInterface();
			theSearchInterface->SetRefDescriptionInterface(DBDescriptionInterface);
			// Initialize Nodelist
			m_NodeList.Clear();
			theSearchInterface->GetNodes(m_NodeList);

			// Print results

			std::cout << std::endl << std::endl << "Database " << j + 1;
			if (m_NodeList.GetLength()==0) {
				std::cout << "No Matches found " << std::endl;
			}

			//sth unknown application type
			for (i=0; i<m_NodeList.GetLength(); i++) {
				testDS = m_NodeList.Item(i);
				DOMNavigator DOMNav;
				path = "/Audio/CreationInformation/Creation/Title";
				testDS2 = DOMNav.GetNode(testDS,path,attribute,"");
				testDS2.GetTextValue(content);
				std::cout << std::endl << std::endl << "Match "<< i+1;
				std::cout << std::endl << "Title: " << content << std::endl;
				path = "/Audio/CreationInformation/Creation/Creator/PersonGroup/Name";
				testDS2 = DOMNav.GetNode(testDS,path,attribute,"");
				testDS2.GetTextValue(content);
				std::cout << "Group Name: " << content << std::endl;
				path = "/Audio/CreationInformation/Classification/Genre";
				testDS2 = DOMNav.GetNode(testDS,path,attribute,"");
				testDS2.GetTextValue(content);
				std::cout << "Genre: " << content << std::endl;
				path = "/Audio/CreationInformation/Classification/Releasedate";
				testDS2 = DOMNav.GetNode(testDS,path,attribute,"");
				testDS2.GetTextValue(content);
				std::cout << "Releasedate: " << content << std::endl << std::endl;
			}	
		}
	}
	

	theCSInterface->destroy();
	theSearchInterface->destroy();

	return(0);
	
}

//----------------------------------------------------------------------------
unsigned long PhoneticTranslationHintsClient::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long PhoneticTranslationHintsClient::Close()
{

	// Free memories
	m_RamDB.FreeDatabase();
	if (m_DBDescription) delete [] m_DBDescription;
	
	NoOfElements = 0;
	m_DBDescription = 0;

	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& PhoneticTranslationHintsClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PhoneticTranslationHintsClient::GetName(void)
{ return myName; }










				



