///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann, Munich University of Technology
// (contributing organizations names)
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
// Version4.cpp


#include <stdio.h>
#include <string.h>

#include "XMManager/ComponentDB.h"
#include "Utilities/readparam.h"
#include "Applications/ServerApplications.h"
#include "Applications/ClientApplications.h"
#include "EvaluationTools/EvaluationTools.h"
#include "Utilities/Utilities.h"
#include "XMManager/Version4.h"

//#define BITSTREAMTEST
#ifdef BITSTREAMTEST
#include "Encoders/Encoders.h"
#include "Decoders/Decoders.h"
#endif

const char *MPEG7_AMD4_NS="urn:mpeg:mpeg7:schema:2004";
const char *MPEG7_AMD4_SCHEMA_LOCATION="urn:mpeg:mpeg7:schema:2004 mpeg7-v3.xsd";


//---------------------------------------------------------------------------
int select_application4 (TXMApplicationParameterSet * ParameterSet)
{
  if (!ParameterSet)
    return 1;

  if (!ParameterSet->Application)
    return 1;


 /* parse application database and select application */

 /* print list of possible applications */
  if (!strcmp (ParameterSet->Application, "PrintApplications")) {
#ifdef USEVS
    printf ("VideoSignatureServer\n");
    printf ("VideoSignatureClient\n");
#endif
    return 0;
  }



#ifdef USEVS
 /* video signature database extraction */
  if (!strcmp (ParameterSet->Application, "VideoSignatureServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::VideoSignatureServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* image signature query retrieval */
  if (!strcmp (ParameterSet->Application, "VideoSignatureClient")) {
    XM::VideoSignatureClient theClient;

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
