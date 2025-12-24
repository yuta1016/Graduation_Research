//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Yoshiaki Shibata (Sony Corporation)
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
// Applicable File Name:  AffectiveSegmentFilterCApp.cpp
//

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
#include "CodingSchemes/CodingSchemes.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "SearchUtilities/SearchUtilities.h"        /* Naming Convention */

using namespace XM;

const UUID AffectiveSegmentFilterClient::myID = UUID("");
const char *AffectiveSegmentFilterClient::myName = "Cliant for Affective Segment Filtering";

//=============================================================================
AffectiveSegmentFilterClient::AffectiveSegmentFilterClient()
{
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterClient::~AffectiveSegmentFilterClient()
{
}

//----------------------------------------------------------------------------
unsigned long AffectiveSegmentFilterClient::Open(char *ListFile, char *DescriptionFile)
// ListFile : input description (Scene decomposition + Affective DS instance)
// DescriptionFile : output description (based on Summary DS)
{
        // create Generic DS instance and its interface
        GenericDSInterfaceABC *theGenericDSInterface = theGenericDS.GetInterface();

        // create coding scheme instance with theGenericDS and its interface
        GenericDSCS *theGenericDSCS = new GenericDSCS;
        GenericDSCSInterfaceABC *theGenericDSCSInterface = theGenericDSCS->GetInterface();

	    //sth only one file in data base?
        // set GenericDS instance and input file to the coding scheme
        theGenericDSCSInterface->SetDescriptionInterface(theGenericDSInterface) ;
        theGenericDSCSInterface->SetDecoderFileName( ListFile );

        // start XML decoding
        theGenericDSCSInterface->StartDecode();

        // set output file
        m_OutputDescriptionFile = DescriptionFile;

        return( 0 );
}

//----------------------------------------------------------------------------
unsigned long AffectiveSegmentFilterClient::Start(void)
{
  //sth what kind of key application is this?
        // create the filtering tool and its interface
        AffectiveSegmentFilterTool
	  *theAffectiveSegmentFilterTool = new AffectiveSegmentFilterTool;
        AffectiveSegmentFilterInterfaceABC
	  *theAffectiveSegmentFilterInterface =
	  theAffectiveSegmentFilterTool->GetInterface();

        // connect theGenericDS to the filtering tool
        theAffectiveSegmentFilterInterface->SetGenericDSInterface( theGenericDS.GetInterface() );

        // execute the affective segment filtering
        if( !theAffectiveSegmentFilterInterface->ExecuteAffectiveFiltering() ){
                std::cerr << "Error: ExecuteAffectiveFiltering() failed" << std::endl;
                return(0);
        };

        // create Generic DS interface
        GenericDSInterfaceABC *theGenericDSInterface = theGenericDS.GetInterface();

        // create coding scheme instance with theGenericDS and its interface
        GenericDSCS *theGenericDSCS = new GenericDSCS;
        GenericDSCSInterfaceABC *theGenericDSCSInterface = theGenericDSCS->GetInterface();

        // set GenericDS instance and output file to the coding scheme
        theGenericDSCSInterface->
	  SetDescriptionInterface(theGenericDSInterface) ;
        theGenericDSCSInterface->
	  SetEncoderFileName(m_OutputDescriptionFile);

        // start XML encoding
        theGenericDSCSInterface->StartEncode();

        return(0);
}

//----------------------------------------------------------------------------
unsigned long AffectiveSegmentFilterClient::Stop()
{
        return( 0 );
}

//----------------------------------------------------------------------------
unsigned long AffectiveSegmentFilterClient::Close()
{
        return( 0 );
}


//----------------------------------------------------------------------------
const UUID &AffectiveSegmentFilterClient::GetObjectID()
{
        return( myID );
}

//----------------------------------------------------------------------------
const char *AffectiveSegmentFilterClient::GetName()
{
        return( myName );
}
