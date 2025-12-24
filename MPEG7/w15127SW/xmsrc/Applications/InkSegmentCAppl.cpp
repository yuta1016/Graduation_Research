///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Zon-Yin Shae and Xiping Wang	- IBM T. J. Watson Research Center
//
// in the course of development of the MPEG-7 Experimentation InkSegment.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation Package tools as specified by the MPEG-7 Requirements.
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  InkSegmentCAppl.cpp
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
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"        /* Naming Convention */
#include "Utilities/Utilities.h"				/* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Decoders/Decoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */

#include <iostream>
#include <fstream>


//=============================================================================

using namespace XM;

const UUID InkSegmentClient::myID = UUID("");
const char *InkSegmentClient::myName = "InkSegment Client";

const int	InkSegmentClient::STRING_MAXSIZE= 4096;	/* maximum size of the strings */


//=============================================================================
InkSegmentClient::InkSegmentClient() : 
m_InkSegmentfile(0),
m_Descriptionfile(0),
m_theInkSegmentDS(0)
{
}

//----------------------------------------------------------------------------
InkSegmentClient::~InkSegmentClient()
{
	delete [] m_InkSegmentfile;
	delete [] m_Descriptionfile;
	delete m_theInkSegmentDS;
}

//----------------------------------------------------------------------------
// InkSegmentFile = MPEG-7 InkSegment Description
// DescriptionFile = MPEG-7 Description

unsigned long InkSegmentClient::Open(char *InkSegmentFile, char *DescriptionFile)
{
  //sth this does not process a database of files

	std::cerr << "Executing InkSegmentClient:" << std::endl;
	std::cerr << "MP7 InkSegment description = " << InkSegmentFile << std::endl;
	std::cerr << "MPEG description = " << DescriptionFile << std::endl;

	delete [] m_InkSegmentfile;
	if (InkSegmentFile) {
		m_InkSegmentfile = new char[strlen(InkSegmentFile)+1];
		strcpy(m_InkSegmentfile, InkSegmentFile);
	}
	else m_InkSegmentfile = 0;

	delete [] m_Descriptionfile;
	if (DescriptionFile) {
		m_Descriptionfile = new char[strlen(DescriptionFile)+1];
		strcpy(m_Descriptionfile, DescriptionFile);
	}
	else m_Descriptionfile = 0;

	{	/** Get description from DescriptionFile **/

 		GenericDSInterfaceABC *theDescriptionInterface;
		GenericDSCS *theCS;
		GenericDSCSInterfaceABC *theCSInterface;

		/* Connect Interfaces and Get coding scheme interface */
		theCS = new GenericDSCS;
		theCSInterface = theCS->GetInterface();
		m_theDescriptionDS = new GenericDS(GenericDS::CreateDescriptionRoot());

		/* Connect description file to coding scheme */
		theCSInterface->SetDecoderFileName(m_Descriptionfile);
		theDescriptionInterface = m_theDescriptionDS->GetInterface();;

		/* Connect Generic DS to coding schemes and Decode Generic DS */
		theCSInterface->SetDescriptionInterface(theDescriptionInterface);
		theCSInterface->StartDecode();
		theCSInterface->destroy();
	}

	{	/** Get description from DescriptionFile **/

 		GenericDSInterfaceABC *theDescriptionInterface;
		GenericDSCS *theCS;
		GenericDSCSInterfaceABC *theCSInterface;

		/* Connect Interfaces and Get coding scheme interface */
		theCS = new GenericDSCS;
		theCSInterface = theCS->GetInterface();
		m_theInkSegmentDS = new GenericDS(GenericDS::CreateDescriptionRoot());
		
		/* Connect description file to coding scheme */
		theCSInterface->SetDecoderFileName(m_InkSegmentfile);
		theDescriptionInterface = m_theInkSegmentDS->GetInterface();

		/* Connect Generic DS to coding schemes and Decode Generic DS */
		theCSInterface->SetDescriptionInterface(theDescriptionInterface);
		theCSInterface->StartDecode();
		theCSInterface->destroy();
	}

	return 0;
}

//----------------------------------------------------------------------------
// Query gives parameters of transcoding goal, i.e., bandwidth, ...
// Ignores NoOfMatches

unsigned long InkSegmentClient::Start(char *Query, int NoOfMatches)
{
	//fprintf( stderr, "InkSegmentClient::Start( %s, %d)\n", Query, NoOfMatches);

	/* Get description interface */
	GenericDSInterfaceABC *theDescriptionInterface;
 	theDescriptionInterface = m_theDescriptionDS->GetInterface();

	/* Get package interface */
	GenericDSInterfaceABC *theInkSegmentInterface;
 	theInkSegmentInterface = m_theInkSegmentDS->GetInterface();

	/* Search Tool */
	InkSegmentSearchTool *theSearch;
	InkSegmentSearchInterfaceABC *theSearchInterface;

	/* Connect DS to SearchTool */
	theSearch = new InkSegmentSearchTool(theInkSegmentInterface, theDescriptionInterface);
	theSearchInterface = theSearch->GetInterface();

	//theSearchInterface->SetDescriptionInterface(theDescriptionInterface);
	theSearchInterface->SetInkSegmentParms(m_InkSegmentfile, m_Descriptionfile);
	theSearchInterface->StartPackaging();
	//sth not of key application type interface, what kind of key application is this?

	return(0);
	
}

//----------------------------------------------------------------------------
unsigned long InkSegmentClient::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long InkSegmentClient::Close()
{
	return 0;
}
	
//----------------------------------------------------------------------------
const UUID& InkSegmentClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *InkSegmentClient::GetName(void)
{ return myName; }










				



