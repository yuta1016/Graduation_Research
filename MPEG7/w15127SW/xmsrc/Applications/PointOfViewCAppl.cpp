//////////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Toshihiko Munetsugu 
// Katsunao Takahashi
//        (Matsushita Electric Industrial Co., Ltd./
//         Matsushita Research Institute Tokyo, Inc.)
//
// Ver4.0 for FDIS, 20010928
// Ver3.0 for FCD,  20010824
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938).
// This software module is an implementation of a part of one or more
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard (ISO/IEC 15938) free licence
// to this software module or modifications thereof for use in hardware or
// software products claiming confermance to the MPEG-7 standard (ISO/IEC 15938).
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents.
// THE ORIGINAL DEVELOPER OF THIS SOFTWARE MODULE AND HIS/HER COMPANY,
// THE SUBSEQUENT EDITORS AND THEIR COMPANIES, AND ISO/IEC HAVE NO 
// LIABILITY FOR USE OF THIS SOFTWARE MODULE OR MODIFICATIONS THEROF.
// No license to this software module is granted for non MPEG-7 standard
// (ISO/IEC 15938) comforming products.
// Matsushita Electric Industrial Co., Ltd. and Matsushita Research Institute
// Tokyo, Inc. retain full right to use the software module for their own
// purpose, assign or donate the software module to a third party and 
// to inhibit third parties from using the code for non MPEG-7 standard
// (ISO/IEC 15938) conforming products.
//
// Copyright (c) 2000-2001.
//
// This copyright notice must be included in all copies or derivative works
// of the software module.
//
//
//////////////////////////////////////////////////////////////////////////////////
/*********************************
 * PointOfViewCAppl.cpp          *
 *********************************/


#include <stdio.h>
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif


#include "Applications/ClientApplications.h"
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"        /* Naming Convention */
#include "Utilities/Utilities.h"              /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Decoders/Decoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */


using namespace XM;
using namespace std;

const UUID PointOfViewClient::myID = UUID("");
const char *PointOfViewClient::myName = "PointOfViewClient";

//=============================================================================
PointOfViewClient::PointOfViewClient()
{
	m_NoOfElements = 0 ;
}

//----------------------------------------------------------------------------
PointOfViewClient::~PointOfViewClient()
{
}

//----------------------------------------------------------------------------
unsigned long PointOfViewClient::Open(char *ListFile, char *DescriptionFile)
// ListFile : name of the file written input Segment information(Segment DS) filenames in
// DescriptionFile : Dummy
{
	// Generic Description Scheme Interface
 	GenericDSInterfaceABC *theSegmentDSInterface ;

	// Generic Coding Scheme
	GenericDSCS * theSegmentDSCS ;
	GenericDSCSInterfaceABC *theSegmentDSCSInterface ;

	// Create coding scheme class instance
	theSegmentDSCS = new GenericDSCS ;
	// Get coding scheme interface
	theSegmentDSCSInterface = theSegmentDSCS->GetInterface() ;

	
	// Read filename database
	m_RamDB.CreateDatabase(ListFile) ;
	m_NoOfElements = m_RamDB.GetNoOfElements() ;

	m_DBSegmentDS = new GenericDS[m_NoOfElements] ;

	// XML filename associated MPEG data
	char *xmlFilename = getpara("InputXML", "xmlinputfile.xml") ;

	for(int i = 0 ; i < m_NoOfElements ; i++)
	{
		// Set the description file
		char  XMLFile[MAXMEDIANAMELEN] ;		// Description XML File
		// XML filename is "mediafilename".dir/*xmlfilename
		strcpy(XMLFile, m_RamDB.GotoEntry(i)->GetFilename()) ;
		strcat(XMLFile, ".dir/") ;
		strcat(XMLFile, xmlFilename) ;

		// Connect description file to coding scheme
		theSegmentDSCSInterface->SetDecoderFileName(XMLFile) ;

		// Get SegmentDS interface
		theSegmentDSInterface = m_DBSegmentDS[i].GetInterface() ;

		// Connect Generic DS to coding schemes
		theSegmentDSCSInterface->SetDescriptionInterface(theSegmentDSInterface) ;

		// Decode Generic DS
		theSegmentDSCSInterface->StartDecode() ;
	}
	theSegmentDSCSInterface->destroy() ;

	return(0) ;
}

//----------------------------------------------------------------------------
unsigned long PointOfViewClient::Start(char *Query,int NoOfMatches)
// Query : filename of UserPreferenceDS instance
// NoOfMatches : 
{
	if(m_NoOfElements < 0)
	{
		return(1) ;
	}

	MatchList theMatchList ;
	if(NoOfMatches > m_NoOfElements)
	{
		NoOfMatches = m_NoOfElements ;
	}
	theMatchList.SetNumberOfMatches(NoOfMatches);

	// Decode Query(UserPreferenceDS)
	GenericDS    theUserPreferenceDS ; 
	GenericDSInterfaceABC * theUserPreferenceDSInterface ;
	GenericDSCS  *theUserPreferenceDSCS ;
	GenericDSCSInterfaceABC * theUserPreferenceDSCSInterface ;

	// Create coding scheme class instance
	theUserPreferenceDSCS = new GenericDSCS ;
	// Get coding shema interface
	theUserPreferenceDSCSInterface = theUserPreferenceDSCS->GetInterface() ;
	// Connect UserPreference description file to coding sheme
	theUserPreferenceDSCSInterface->SetDecoderFileName(Query) ;
	// Get UserPreferenceDS Interface
	theUserPreferenceDSInterface = theUserPreferenceDS.GetInterface() ;
	// Connect Generic DS to coding scheme
	theUserPreferenceDSCSInterface->SetDescriptionInterface(theUserPreferenceDSInterface) ;
	// Decode Generic DS
	theUserPreferenceDSCSInterface->StartDecode() ;
	theUserPreferenceDSCSInterface->destroy() ;

	// Set Outputfilename
	char *outputfile = getpara("OutputXML", "PointOfViewFiltered.xml") ;

	// Matching
	for(int i = 0 ; i < m_NoOfElements ; i++)
	{
		// Search Tool
		PointOfViewSearchTool *thePointOfViewSearchTool =
			new PointOfViewSearchTool(theUserPreferenceDSInterface) ;
		PointOfViewSearchInterfaceABC *thePointOfViewSearchToolInterface ;
		GenericDS * theFilteredSegment ;

		// Connect DS to SearchTool
		thePointOfViewSearchToolInterface = 
			thePointOfViewSearchTool->GetInterface() ;

		// Get Segment DS interface
		GenericDSInterface * DBSegmentDSInterface ;
		DBSegmentDSInterface = (GenericDSInterface *)m_DBSegmentDS[i].GetInterface() ;

		// Connect to Search Tool and do Segment Filtering
		double dist = 1.0 ;
		if((!(thePointOfViewSearchToolInterface->
			     SetRefDescriptionInterface(DBSegmentDSInterface))) &&
			(!(thePointOfViewSearchToolInterface->
			     SetQueryDescriptionInterface(theUserPreferenceDSInterface))))
		{
			thePointOfViewSearchToolInterface->SetQuery() ;
			dist = thePointOfViewSearchToolInterface->GetDistance() ;
			theFilteredSegment = thePointOfViewSearchToolInterface->GetFilteredSegment() ;
		}
		theMatchList.InsertMatch(i, dist) ;

		if(!theFilteredSegment)
		{
			// Segment filtering is failed
			cerr << "Segment Filtering is failed.\n" ;
		}
		else
		{
			FilteredSegmentToSummary theConvertSummary ;
			theConvertSummary.SetFilteredSegment(theFilteredSegment) ;
			if(theConvertSummary.ConvertToSummary()){
				GenericDS * theSummary = theConvertSummary.GetSummarization() ;
				// Descriptor Schemes for filtered Segment DS
				GenericDSInterfaceABC * theSummaryDSInterface ;
				theSummaryDSInterface = theSummary->GetInterface() ;

				// Coding Scheme class
				GenericDSCS * theSummaryDSCS = new GenericDSCS ;
				// Get Coding Scheme Interface
				GenericDSCSInterfaceABC * theSummaryDSCSInterface ;
				theSummaryDSCSInterface = theSummaryDSCS->GetInterface() ;

				// Connect result file to Summary DS coding scheme
				// output filename is "media filename".dir/
				char outFilename[MAXMEDIANAMELEN] ;
				char *inputFilename = m_RamDB.GotoEntry(i)->GetFilename() ;
				strcpy(outFilename, inputFilename) ;
				strcat(outFilename, ".dir/") ;
				strcat(outFilename, outputfile) ;
				theSummaryDSCSInterface->SetEncoderFileName(outFilename) ;

				// Connect Summry DS Interface
				theSummaryDSCSInterface->SetDescriptionInterface(theSummaryDSInterface) ;

				// Encoding
				theSummaryDSCSInterface->StartEncode() ;
				theSummaryDSCSInterface->destroy() ;
			}
			else
			{
				cerr << "Converting Filtered Segment to Summary is failed.\n" ;
			}
			thePointOfViewSearchToolInterface->destroy() ;
		}
	}

	//---------------------------------------------------------------------
	// Print results of Matching
	//---------------------------------------------------------------------
	cerr << endl ;
	cerr << "List of results: (Distance, Item)" << endl ;
	for(int j = 0 ; j < NoOfMatches ; j++)
	{
		if(theMatchList.GetDBEntryNo(j) == (unsigned long)(-1))
		{
			break ;
		}
		cout << theMatchList.GetDistanceValue(j) << "\t" ;
		cout << m_RamDB.GotoEntry(theMatchList.GetDBEntryNo(j))->GetFilename() ;
		cout << endl ;
	}
	return(0);
}

//----------------------------------------------------------------------------
unsigned long PointOfViewClient::Stop()
{
	return(0) ;
}

//----------------------------------------------------------------------------
unsigned long PointOfViewClient::Close()
{
	// Free memories
	m_RamDB.FreeDatabase() ;
	if(m_DBSegmentDS) delete[] m_DBSegmentDS ;
	
	m_NoOfElements = 0 ;
	m_DBSegmentDS = 0 ;
	
	return(0) ;
}

	
//----------------------------------------------------------------------------
const UUID & PointOfViewClient::GetObjectID(void)
{
	return(myID) ;
}

//----------------------------------------------------------------------------
const char * PointOfViewClient::GetName(void)
{
	return(myName) ;
}
