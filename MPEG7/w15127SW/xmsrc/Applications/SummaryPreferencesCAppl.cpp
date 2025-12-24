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
// Copyright (c) 2000-2003.
//
// This copyright notice must be included in all copies or derivative works
// of the software module.
//
//
//////////////////////////////////////////////////////////////////////////////////
/*********************************
 * SumamryPreferencesCAppl.cpp   *
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
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "SearchUtilities/SearchUtilities.h"
#include "Utilities/Utilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Decoders/Decoders.h"
#include "Media/Media.h"
#include "SearchUtilities/MatchList.h"


using namespace XM;
using namespace std;

const UUID SummaryPreferencesClient::myID = UUID("");
const char *SummaryPreferencesClient::myName = "SummaryPreferencesClient";

//=============================================================================
SummaryPreferencesClient::SummaryPreferencesClient()
{
	m_NoOfElements = 0 ;
}

//----------------------------------------------------------------------------
SummaryPreferencesClient::~SummaryPreferencesClient()
{
}

//----------------------------------------------------------------------------
unsigned long SummaryPreferencesClient::Open(char *ListFile, char *DescriptionFile)
// ListFile : name of the file written input Segment information(Segment DS) filenames in
// DescriptionFile : Dummy
{
	// Generic Description Scheme Interface
 	GenericDSInterfaceABC	*theSegmentDSInterface;

	// Generic Coding Scheme
	GenericDSCS		* theSegmentDSCS;
	GenericDSCSInterfaceABC	*theSegmentDSCSInterface;


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

	for(int i = 0 ; i < m_NoOfElements ; i++){
		// Set the description file
		char  XMLFile[MAXMEDIANAMELEN] ;	// Description XML File
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
unsigned long SummaryPreferencesClient::Start(char *Query,int NoOfMatches)
// Query : filename of SummaryPreferencesDS instance
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

	// Decode Query(SummaryPreferencesDS)
	GenericDS    theSummaryPreferencesDS ; 
	GenericDSInterfaceABC * theSummaryPreferencesDSInterface ;
	GenericDSCS  *theSummaryPreferencesDSCS ;
	GenericDSCSInterfaceABC * theSummaryPreferencesDSCSInterface ;

	// Create coding scheme class instance
	theSummaryPreferencesDSCS = new GenericDSCS ;
	// Get coding shema interface
	theSummaryPreferencesDSCSInterface = theSummaryPreferencesDSCS->GetInterface() ;
	// Connect SummaryPreferences description file to coding sheme
	theSummaryPreferencesDSCSInterface->SetDecoderFileName(Query) ;
	// Get SummaryPreferencesDS Interface
	theSummaryPreferencesDSInterface = theSummaryPreferencesDS.GetInterface() ;
	// Connect Generic DS to coding scheme
	theSummaryPreferencesDSCSInterface->SetDescriptionInterface(theSummaryPreferencesDSInterface) ;
	// Decode Generic DS
	theSummaryPreferencesDSCSInterface->StartDecode() ;
	theSummaryPreferencesDSCSInterface->destroy() ;

	// Set Outputfilename
	char *outputfile = getpara("OutputXML", "SummaryPreferencesFiltered.xml") ;

	for (int i = 0 ; i < m_NoOfElements ; i++) {
		// Search Tool
		SummaryPreferencesSearchTool *theSummaryPreferencesDSSearchTool 
			= new SummaryPreferencesSearchTool ;
		SummaryPreferencesSearchInterfaceABC *theSummaryPreferencesSearchToolInterface ;
		GenericDS * theFilteredSegment ;

		// Connect DS to SearchTool
		theSummaryPreferencesSearchToolInterface =
			theSummaryPreferencesDSSearchTool->GetInterface() ;

		// Get Segment DS interface
		GenericDSInterface * DBSegmentDSInterface ;
		DBSegmentDSInterface = (GenericDSInterface *)m_DBSegmentDS[i].GetInterface() ;

		// Connect to Search Tool and do Segment Filtering
		double dist = 1.0 ;
		if((!(theSummaryPreferencesSearchToolInterface->
			     SetRefDescriptionInterface(DBSegmentDSInterface))) &&
			(!(theSummaryPreferencesSearchToolInterface->
			     SetQueryDescriptionInterface(theSummaryPreferencesDSInterface)))) {
			theSummaryPreferencesSearchToolInterface->SetQuery() ;
			dist = theSummaryPreferencesSearchToolInterface->GetDistance() ;
			theFilteredSegment = theSummaryPreferencesSearchToolInterface->GetFilteredSegment() ;
		}
		theMatchList.InsertMatch(i, dist) ;

		if (!theFilteredSegment) {
			// Segment filtering is failed
			cerr << "Segment Filtering is failed.\n" ;
		}
		else {
			// Descriptor Schemes for filtered Segment DS
			GenericDSInterfaceABC * theFilteredSegmentDSInterface ;
			theFilteredSegmentDSInterface = theFilteredSegment->GetInterface() ;

			// Coding Scheme class
			GenericDSCS * theFilteredSegmentDSCS = new GenericDSCS ;
			// Get Coding Scheme Interface
			GenericDSCSInterfaceABC * theFilteredSegmentDSCSInterface ;
			theFilteredSegmentDSCSInterface = theFilteredSegmentDSCS->GetInterface() ;
			// Connect result file to Filtered Segment DS coding scheme
			// output filename is "media filename".dir/
			char outFilename[MAXMEDIANAMELEN] ;
			char *inputFilename = m_RamDB.GotoEntry(i)->GetFilename() ;
			strcpy(outFilename, inputFilename) ;
			strcat(outFilename, ".dir/") ;
			strcat(outFilename, outputfile) ;
			theFilteredSegmentDSCSInterface->SetEncoderFileName(outFilename) ;

			// Connect Filtered SegmentDS Interface
			theFilteredSegmentDSCSInterface->SetDescriptionInterface(theFilteredSegmentDSInterface) ;

			// Encoding
			theFilteredSegmentDSCSInterface->StartEncode() ;
			theFilteredSegmentDSCSInterface->destroy() ;
		}
	}

	//---------------------------------------------------------------------
	// Print results of Matching
	//---------------------------------------------------------------------
	cerr << endl ;
	cerr << "List of results: (Distance, Item)" << endl ;
	for (int j = 0 ; j < NoOfMatches ; j++) {
		if (theMatchList.GetDBEntryNo(j) == (unsigned long)(-1))
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
unsigned long SummaryPreferencesClient::Stop()
{
	return(0) ;
}

//----------------------------------------------------------------------------
unsigned long SummaryPreferencesClient::Close()
{
	// Free memories
	m_RamDB.FreeDatabase() ;
	if(m_DBSegmentDS) delete[] m_DBSegmentDS ;
	
	m_NoOfElements = 0 ;
	m_DBSegmentDS = 0 ;
	
	return(0) ;
}

	
//----------------------------------------------------------------------------
const UUID & SummaryPreferencesClient::GetObjectID(void)
{
	return(myID) ;
}

//----------------------------------------------------------------------------
const char * SummaryPreferencesClient::GetName(void)
{
	return(myName) ;
}
