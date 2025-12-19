///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Sanghoon Sull, Korea University, Seoul, Korea
// Keansub Lee, Korea University, Seoul, Korea
//
// in the course of development of the MPEG-7 Experimentation SpatialResolution Hint.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation SpatialResolutionHint tools as specified by the MPEG-7 Requirements.
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
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SpatialResolutionHintCAppl.cpp
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


#include "Applications/ClientApplications.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "SearchUtilities/SearchUtilities.h"
#include "Utilities/Utilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Decoders/Decoders.h"
#include "Media/Media.h"
#include "SearchUtilities/MatchList.h"

#include <iostream>
#include <fstream>


//=============================================================================

using namespace XM;
using namespace std;

const UUID SpatialResolutionHintClient::myID = UUID("");
const char *SpatialResolutionHintClient::myName = "SpatialResolutionHintDSClient";



//=============================================================================
SpatialResolutionHintClient::SpatialResolutionHintClient() : 
m_RamDB(),
NoOfElements(0),
m_DBDescription(0)
{
}

//----------------------------------------------------------------------------
SpatialResolutionHintClient::~SpatialResolutionHintClient()
{
}

//----------------------------------------------------------------------------
// ListFile = AV File
// DescriptionFile = MPEG-7 File with spatialResolutionHints

unsigned long SpatialResolutionHintClient::Open(char *ListFile, char *DescriptionFile)
{

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

	m_RamDB.CreateDatabase(ListFile);
	NoOfElements=m_RamDB.GetNoOfElements();

	// Create in memory decompressed database
	// allocate memories
	m_DBDescription = new GenericDS [NoOfElements];

	for (int i=0; i<NoOfElements; i++)
	{
		// Connect description file to coding scheme
		theCSInterface->SetDecoderFileName(DescriptionFile);

		// Get ProgramDS interface
		theDescriptionInterface = m_DBDescription[i].GetInterface();

		// Connect Generic DS to coding schemes
		theCSInterface->SetDescriptionInterface(theDescriptionInterface);

		// Decode Generic DS
		theCSInterface->StartDecode();
	}
	theCSInterface->destroy();

	return 0;
}

//-------------------------------------------------------------------------- --
// Query gives parameters of transcoding goal, i.e., the display size(in pixels) of client device ...
// Ignores NoOfMatches

unsigned long SpatialResolutionHintClient::Start(char *Query, int NoOfMatches)
{
	// ---------------------------------------------------------------------------------------
	// Declarations of variables
	// ---------------------------------------------------------------------------------------	

	// Descriptors Schemes for query and elements in database
	GenericDS theDescription;
 	GenericDSInterfaceABC *theDescriptionInterface;
 	GenericDSInterfaceABC *DBDescriptionInterface;

	// Generic Coding Scheme for query only
	GenericDSCS *theCS;
	GenericDSCSInterfaceABC *theCSInterface;

	// Search Tool
	SpatialResolutionHintSearchTool *theSearchTool;
	SpatialResolutionHintSearchInterfaceABC *theSearchInterface;

	// ---------------------------------------------------------------------------------------
	// Connect Interfaces 
	// ---------------------------------------------------------------------------------------
	
	// Get description scheme interface
 	theDescriptionInterface = theDescription.GetInterface();

	// Get coding scheme interface
	theCS = new GenericDSCS(theDescriptionInterface);
	theCSInterface = theCS->GetInterface();

	// Connect DS to SearchTool
	theSearchTool = new SpatialResolutionHintSearchTool(theDescriptionInterface);
	theSearchInterface = theSearchTool->GetInterface();


	int i;
	for (i=0; i<NoOfElements; i++)
	{
#ifdef VERBOSE
		cout << "Getting distance for : " << m_RamDB.GotoEntry(i)->GetFilename() << endl;
#endif

		// Get DS interface
		DBDescriptionInterface=m_DBDescription[i].GetInterface();

		// Connect reference description scheme to search tool
		theSearchInterface->SetDescriptionInterface(DBDescriptionInterface);
		MomVop *image = m_RamDB.LoadNamedImage(m_RamDB.GotoEntry(i)->GetFilename());

		theSearchInterface->SetSpatialResolutionHintParms(image->width, image->height);
		theSearchInterface->GetDistance();
	}
	
	theCSInterface->destroy();
	theSearchInterface->destroy();

	return(0);
}

//----------------------------------------------------------------------------
unsigned long SpatialResolutionHintClient::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatialResolutionHintClient::Close()
{
		// Free memories
	m_RamDB.FreeDatabase();
	if (m_DBDescription) delete [] m_DBDescription;
	
	NoOfElements = 0;
	m_DBDescription = 0;
	return 0;
}
	
//----------------------------------------------------------------------------
const UUID& SpatialResolutionHintClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpatialResolutionHintClient::GetName(void)
{ return myName; }










				



