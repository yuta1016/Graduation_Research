///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Ana B. Benitez		- Columbia University.
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
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  StructuredCollectionSAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif


#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */


#include <iostream> //LINUX_2011
#include <fstream>


//=============================================================================

using namespace XM;


const UUID StructuredCollectionServer::myID = UUID("");
const char *StructuredCollectionServer::myName = "StructuredCollectionServer";

const int	StructuredCollectionServer::STRING_MAXSIZE= 4096;		// maximum size of the strings


//=============================================================================
StructuredCollectionServer::StructuredCollectionServer()
{
}

//----------------------------------------------------------------------------
StructuredCollectionServer::~StructuredCollectionServer()
{
}

//----------------------------------------------------------------------------
unsigned long StructuredCollectionServer::Open()
{
	/* This server doesn't handle pull */
	return 0;
}

//----------------------------------------------------------------------------
unsigned long StructuredCollectionServer::Start(char *ListFile, char *DescriptionFile)
{

	// NOTES Start() method of the StructuredCollectionDS Server
	// 
	// ListFile is a list of text files which describe of a collection of images in
	// multiple semantic classes. The path of the images in ListFile is relative
	// the directory where ListFile is kept.
	// 
	// The xml output descriptions will be stored in the base directory of each class
	// text file with name DescriptionFile.

	
	/* ---------------------------------------------------------------------------------------
	   Declarations of variables
	   --------------------------------------------------------------------------------------- */

	/* Files on disk */
	char pcBaseDir[STRING_MAXSIZE];		/* Base Folder */
	char pcClassName[STRING_MAXSIZE];	/* Class file name */
	char pcImageName[STRING_MAXSIZE];	/* Image file name */

	const char IMAGEFILE[] = "%s";		/* Name for image file name */

	char XMLFile[STRING_MAXSIZE];

	int NoOfElements;					/* Number of elements in DB */


	/* image data base */
	MediaDB RamDB;

	/* Extraction Tools */
	StructuredCollectionExtractionTool *theExtraction;
	StructuredCollectionExtractionInterfaceABC *theExtractionInterface;
	
	/* Generic Coding Scheme */
	GenericDSCS *theCS;
	GenericDSCSInterfaceABC *theCSInterface;


	/* ---------------------------------------------------------------------------------------
	   Connect Interfaces
	   --------------------------------------------------------------------------------------- */

	/* Connect DS to ExtractionTool */
	theExtraction = new StructuredCollectionExtractionTool();
	theExtractionInterface = theExtraction->GetInterface();

	/* Connect descriptor to coding scheme */
	theCS = new GenericDSCS();
	theCSInterface = theCS->GetInterface();


	/* ---------------------------------------------------------------------------------------
	   Start Extraction for each database in ListFile
	   --------------------------------------------------------------------------------------- */

	/* Read filename data base */
	RamDB.CreateDatabase(ListFile);
	NoOfElements = RamDB.GetNoOfElements();

	for (int i=0; i<NoOfElements; i++) {

		/* Read class name and set the base directory */
		strcpy ( pcClassName, RamDB.GotoEntry(i)->GetFilename());
		strcpy( pcBaseDir, pcClassName);
		fprintf(stderr, "\n\nClass File: %s\n", pcClassName);
		strcat( pcBaseDir, ".dir/");

		/* Set image names */
		strcpy( pcImageName, pcBaseDir);
		strcat( pcImageName, IMAGEFILE);
		
		/* Output file name */
		strcpy(XMLFile, pcBaseDir);
		strcat(XMLFile, DescriptionFile);

		/* Descriptors Schemes */
		GenericDS theDescription = GenericDS::CreateDescriptionRoot();
	 	GenericDSInterfaceABC *theDescriptionInterface;
	
		/* Get DS Interface */
	 	theDescriptionInterface = theDescription.GetInterface();

		/* Connect DS to ExtractionTool */
		theExtractionInterface->SetDescriptionInterface(theDescriptionInterface);

		/* Connect descriptor to coding scheme */
		theCSInterface->SetDescriptionInterface(theDescriptionInterface);

		/* Connect output file to coding scheme */
		theCSInterface->SetEncoderFileName(XMLFile);

		/* Set Data to StructuredCollectionExtraction */
		theExtractionInterface->SetSourceMedia(pcClassName, pcImageName);

		/* Start Extraction */
		theExtractionInterface->StartExtracting();

		/* Output each segment tree description in a separate file */
		theCSInterface->StartEncode();

	} /* for each database (FS image + Tree file) */

	theExtractionInterface->destroy(); 
	theCSInterface->destroy();

	RamDB.FreeDatabase();

	return(0);	
}

//----------------------------------------------------------------------------
unsigned long StructuredCollectionServer::Stop()
{
	fprintf(stderr,"WARNING(StructuredCollectionServer): Stop() member function not Implemented.\n");
	return 0;
}

//----------------------------------------------------------------------------
unsigned long StructuredCollectionServer::Close()
{
	fprintf(stderr,"WARNING(StructuredCollectionServer): Close() member function not Implemented.\n");
	return 0;
}
	
//----------------------------------------------------------------------------
const UUID& StructuredCollectionServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *StructuredCollectionServer::GetName(void)
{ return myName; }

