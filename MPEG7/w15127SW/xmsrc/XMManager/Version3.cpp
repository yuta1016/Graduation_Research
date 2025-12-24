///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann, Munich University of Technology
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Version3.cpp


#include <stdio.h>
#include <string.h>

#include "XMManager/ComponentDB.h"
#include "Utilities/readparam.h"
#include "Applications/ServerApplications.h"
#include "Applications/ClientApplications.h"
/*#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"*/
#include "EvaluationTools/EvaluationTools.h"
#include "Utilities/Utilities.h"
#include "XMManager/Version3.h"

//#define BITSTREAMTEST
#ifdef BITSTREAMTEST
#include "Encoders/Encoders.h"
#include "Decoders/Decoders.h"
#endif

const char *MPEG7_AMD3_NS="urn:mpeg:mpeg7:schema:2004";
const char *MPEG7_AMD3_SCHEMA_LOCATION="urn:mpeg:mpeg7:schema:2004 mpeg7-v3.xsd";



//---------------------------------------------------------------------------
int select_application3 (TXMApplicationParameterSet * ParameterSet)
{
  if (!ParameterSet)
    return 1;

  if (!ParameterSet->Application)
    return 1;

 /* init components */
//  theComponentDatabase.InitDB();
//  theComponentDatabase.CreateDDL();
//  theComponentDatabase.CreateClient();
//  theComponentDatabase.CreateServer();
//  theComponentDatabase.CreateCS();
//  theComponentDatabase.CreateDecoder();
//  theComponentDatabase.CreateDS();
//  theComponentDatabase.CreateD();
//  theComponentDatabase.CreateEncoder();
//  theComponentDatabase.CreateET();
//  theComponentDatabase.CreateExtractor();
//  theComponentDatabase.CreateParser();
//  theComponentDatabase.CreateSearch();
//  theComponentDatabase.CreateSystem();

//  theComponentDatabase.GetClientApps();
//  theComponentDatabase.GetServerApps();

 /* parse application database and select application */

 /* print list of possible applications */
  if (!strcmp (ParameterSet->Application, "PrintApplications")) {
#ifdef USEIS
    printf ("ImageSignatureServer\n");
    printf ("ImageSignatureClient\n");
#endif
    return 0;
  }

#ifdef USEEXAMPLE
 /* DummyType database extraction */
  if (!strcmp (ParameterSet->Application, "ExampleServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::ExampleServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* DummyType query retrieval */
  if (!strcmp (ParameterSet->Application, "ExampleClient")) {
    XM::ExampleClient theClient;

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream && ParameterSet->Query
	&& ParameterSet->NoOfMatches))
      return 1;
    else {
      theClient.Open (ParameterSet->ListFile, ParameterSet->Bitstream);
      theClient.Start (ParameterSet->Query, ParameterSet->NoOfMatches);
      theClient.Stop ();
      theClient.Close ();

      return 0;
    }
  }
#endif


#ifdef USEIS
 /* image signature database extraction */
  if (!strcmp (ParameterSet->Application, "ImageSignatureServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::ImageSignatureServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* image signature query retrieval */
  if (!strcmp (ParameterSet->Application, "ImageSignatureClient")) {
    XM::ImageSignatureClient theClient;

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream && ParameterSet->Query
	&& ParameterSet->NoOfMatches))
      return 1;
    else {
      theClient.Open (ParameterSet->ListFile, ParameterSet->Bitstream);
      theClient.Start (ParameterSet->Query, ParameterSet->NoOfMatches);
      theClient.Stop ();
      theClient.Close ();

      return 0;
    }
  }
#endif


  return -1;
}
