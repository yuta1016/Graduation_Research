///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Zon-Yin Shae and Xiping Wang		- IBM T. J. Watson Research Center 
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
// Applicable File Name:  InkSegmentSAppl.cpp
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


#include <iostream>
#include <fstream>


//=============================================================================

using namespace XM;


const UUID InkSegmentServer::myID = UUID("");
const char *InkSegmentServer::myName = "InkSegment DS Server";

const int	InkSegmentServer::STRING_MAXSIZE= 4096;		// maximum size of the strings


//=============================================================================
InkSegmentServer::InkSegmentServer()
{
}

//----------------------------------------------------------------------------
InkSegmentServer::~InkSegmentServer()
{
}

//----------------------------------------------------------------------------
unsigned long InkSegmentServer::Open()
{
	/* This server doesn't handle pull */
	return 0;
}

//----------------------------------------------------------------------------
unsigned long InkSegmentServer::Start(char *ListFile, char *DescriptionFile)
{
	/* ------------------------------------------------------
	   Create Interfaces 
	   ----------------------------------------------------*/

	/* Descriptors Schemes */
	GenericDS theDescription = GenericDS::CreateDescriptionRoot();
	GenericDSInterfaceABC *theDescriptionInterface;

	/* Extraction Tools */
	InkSegmentExtractionTool *theExtraction;
	InkSegmentExtractionInterfaceABC
	  *theExtractionInterface;

	/* Generic Coding Scheme */
	GenericDSCS *theCS;
	GenericDSCSInterfaceABC *theCSInterface;


	/* -------------------------------------------------------
	   Connect Interfaces 
	   -----------------------------------------------------*/

	/* Get DS Interface */
	theDescriptionInterface = theDescription.GetInterface();

	/* Connect DS to ExtractionTool */
	theExtraction =
	  new InkSegmentExtractionTool(theDescriptionInterface);
	theExtractionInterface = theExtraction->GetInterface();

	/* Connect descriptor to coding scheme */
	theCS = new GenericDSCS(theDescriptionInterface);
	theCSInterface = theCS->GetInterface();

	/* Connect output file to coding scheme */

	/* Start Extraction */
	theExtractionInterface->StartExtracting(ListFile, DescriptionFile);
	//sth interface violation!! Extraction is done per media file
	// (and per frame of the media file if applicable)
	// individual interface cannot be reused!!

	theExtractionInterface->destroy();
	theCSInterface->destroy();


	return(0);

}

//----------------------------------------------------------------------------
unsigned long InkSegmentServer::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long InkSegmentServer::Close()
{
	return 0;
}
	
//----------------------------------------------------------------------------
const UUID& InkSegmentServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *InkSegmentServer::GetName(void)
{ return myName; }










				



