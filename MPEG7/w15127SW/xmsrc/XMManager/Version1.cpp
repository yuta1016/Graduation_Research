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
// Version1.cpp


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
#include "XMManager/Version1.h"

//#define BITSTREAMTEST
#ifdef BITSTREAMTEST
#include "Encoders/Encoders.h"
#include "Decoders/Decoders.h"
#endif

const char *MPEG7_BASE_NS="urn:mpeg:mpeg7:schema:2001";
const char *MPEG7_XML_SCHEMA="http://www.w3.org/2001/XMLSchema-instance";
const char *MPEG7_BASE_SCHEMA_LOCATION="urn:mpeg:mpeg7:schema:2001 Mpeg7-2001.xsd";

//---------------------------------------------------------------------------
TXMApplicationParameterSet *parse_cmdline (int argc, char **argv)
{
 //   XM::ComponentDatabase  theComponentDatabase;
  static TXMApplicationParameterSet ParameterSet;
 //  char *ListFile=0;
 //char *Query=0;
 //char *ParameterFileName=0;
 //char *Bitstream=0;
 //char *Application=0;
 //int  NoOfMatches=0;
 //char *LogRedirection=0;
  char *a = 0;

#ifdef XYUVDISP
  XYUV_INIT (argc, argv);
#endif
 /* check parameters of program call */
 /* check if help will be printed */
  for (int i = 0; i < argc; i++) {
    if (!strcmp (argv[i], "-h")) {
      return 0;
    }
    if (!strcmp (argv[i], "h")) {
      return 0;
    }
    if (!strcmp (argv[i], "-?")) {
      return 0;
    }
    if (!strcmp (argv[i], "?")) {
      return 0;
    }

    /* check stdout redirection parameter */
    if (strstr (argv[i], "-o") == argv[i]) {
      ParameterSet.ResultRedirection = &(argv[i][2]);
      if (*ParameterSet.ResultRedirection) {
	freopen (ParameterSet.ResultRedirection, "a+", stdout);
#ifdef DEBUG
	fprintf (stderr,"Result Redirection to: %s\n",
		 ParameterSet.ResultRedirection);
#endif
	continue;
      }
     /* redirection name in next parameter */
      ParameterSet.ResultRedirection = 0;
      if ((i + 1) == argc) {
	return 0;
      }
      if (argv[i + 1][0] == '-') {
	return 0;
      }
      i++;
      ParameterSet.ResultRedirection = &(argv[i][0]);
      freopen (ParameterSet.ResultRedirection, "a+", stdout);
#ifdef DEBUG
      fprintf (stderr,"Result Redirection to: %s\n",
	       ParameterSet.ResultRedirection);
#endif
    }

   /* check stderr redirection parameter */
    if (strstr (argv[i], "-r") == argv[i]) {
      ParameterSet.LogRedirection = &(argv[i][2]);
      if (*ParameterSet.LogRedirection) {
	freopen (ParameterSet.LogRedirection, "a+", stderr);
#ifdef DEBUG
	fprintf (stderr,"Log Redirection to: %s\n",ParameterSet.LogRedirection);
#endif
	continue;
      }
      /* redirection name in next parameter */
      ParameterSet.LogRedirection = 0;
      if ((i + 1) == argc) {
	return 0;
      }
      if (argv[i + 1][0] == '-') {
	return 0;
      }
      i++;
      ParameterSet.LogRedirection = &(argv[i][0]);
      freopen (ParameterSet.LogRedirection, "a+", stderr);
#ifdef DEBUG
      fprintf (stderr,"Log Redirection to: %s\n",ParameterSet.LogRedirection);
#endif
    }

   /* check application parameter */
    if (strstr (argv[i], "-a") == argv[i]) {
      ParameterSet.Application = &(argv[i][2]);
      if (*ParameterSet.Application)
	continue;
     /* application name in next parameter */
      ParameterSet.Application = 0;
      if ((i + 1) == argc) {
	return 0;
      }
      if (argv[i + 1][0] == '-') {
	return 0;
      }
      i++;
      ParameterSet.Application = &(argv[i][0]);
    }

   /* check listfile parameter */
    if (strstr (argv[i], "-l") == argv[i]) {
      ParameterSet.ListFile = &(argv[i][2]);
      if (*ParameterSet.ListFile)
	continue;
     /* listfile name in next parameter */
      ParameterSet.ListFile = 0;
      if ((i + 1) == argc) {
	return 0;
      }
      if (argv[i + 1][0] == '-') {
	return 0;
      }
      i++;
      ParameterSet.ListFile = &(argv[i][0]);
    }

   /* check bitstream parameter */
    if (strstr (argv[i], "-b") == argv[i]) {
      ParameterSet.Bitstream = &(argv[i][2]);
      if (*ParameterSet.Bitstream)
	continue;
     /* bitstream name in next parameter */
      ParameterSet.Bitstream = 0;
      if ((i + 1) == argc) {
	return 0;
      }
      if (argv[i + 1][0] == '-') {
	return 0;
      }
      i++;
      ParameterSet.Bitstream = &(argv[i][0]);
    }

   /* check parammeterfile parameter */
    if (strstr (argv[i], "-p") == argv[i]) {

      a = &(argv[i][2]);
      if (!(*a)) {

       /* query name in next parameter */
	if ((i + 1) == argc) {
	  return 0;
	}
	if (argv[i + 1][0] == '-') {
	  return 0;
	}
	i++;
	a = &(argv[i][0]);
      };
      if (*a) {
       /* read parameter file */
	readparameters (a);
      }
      if (!ParameterSet.ParameterFileName)
	ParameterSet.ParameterFileName = a;
    }

   /* check query parameter */
    if (strstr (argv[i], "-q") == argv[i]) {
      ParameterSet.Query = &(argv[i][2]);
      if (*ParameterSet.Query)
	continue;
     /* query name in next parameter */
      ParameterSet.Query = 0;
      if ((i + 1) == argc) {
	return 0;
      }
      if (argv[i + 1][0] == '-') {
	return 0;
      }
      i++;
      ParameterSet.Query = &(argv[i][0]);
    }

   /* check noofmatches parameter */
    if (strstr (argv[i], "-n") == argv[i]) {
      a = &(argv[i][2]);
      if (!*a) {
       /* noofmatches name in next parameter */
	a = 0;
	if ((i + 1) == argc) {
	  return 0;
	}
	if (argv[i + 1][0] == '-') {
	  return 0;
	}
	i++;
	a = &(argv[i][0]);
      }
      ParameterSet.NoOfMatches = atoi (a);
      if (ParameterSet.NoOfMatches <= 0)
	ParameterSet.NoOfMatches = 1;
    }
  }

 /* if paramters are not set, get them from parameter file */
  if (ParameterSet.ParameterFileName) {
    if (!ParameterSet.Application)
      ParameterSet.Application = getpara ("Application", (char *) NULL);
    if (!ParameterSet.ListFile)
      ParameterSet.ListFile = getpara ("ListFile", (char *) NULL);
    if (!ParameterSet.Query)
      ParameterSet.Query = getpara ("Query", (char *) NULL);
    if (!ParameterSet.Bitstream)
      ParameterSet.Bitstream = getpara ("Bitstream", (char *) NULL);
    if (!ParameterSet.NoOfMatches)
      ParameterSet.NoOfMatches = atoi (getpara ("NoOfMatches", "10"));
  }

  return &ParameterSet;
}


//---------------------------------------------------------------------------
int select_application (TXMApplicationParameterSet * ParameterSet)
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
#ifdef USEDT
    printf ("DummyTypeServer\n");
    printf ("DummyTypeClient\n");
#endif
#ifdef USEDTDS
    printf ("DSDummyTypeServer\n");
    printf ("DSDummyTypeClient\n");
#endif
#ifdef USETS
    printf ("TimeSeriesServer\n");
    printf ("TimeSeriesClient\n");
#endif
#ifdef USECC
    printf ("CompactColorServer\n");
    printf ("CompactColorClient\n");
#endif
#ifdef USECH
    printf ("ColorHistogramServer\n");
    printf ("ColorHistogramClient\n");
#endif
#ifdef USECL
    printf ("ColorLayoutServer\n");
    printf ("ColorLayoutClient\n");
#endif
#ifdef USECT
    printf ("ColorStructureServer\n");
    printf ("ColorStructureClient\n");
#endif
#ifdef USEDC
    printf ("DominantColorServer\n");
    printf ("DominantColorClient\n");
#endif
#ifdef USEGF
    printf ("GoFGoPColorServer\n");
    printf ("GoFGoPColorClient\n");
#endif
#ifdef USESCC
    printf ("ScalableColorServer\n");
    printf ("ScalableColorlient\n");
#endif
#ifdef USEEH
    printf ("EdgeHistogramServer\n");
    printf ("EdgeHistogramClient\n");
#endif
#ifdef USEHT
    printf ("HomogeneousTextureServer\n");
    printf ("HomogeneousTextureClient\n");
#endif
#ifdef USETB
    printf ("TextureBrowsingServer\n");
    printf ("TextureBrowsingClient\n");
#endif
#ifdef USE3DSS
    printf ("3DShapeSpectrumServer\n");
    printf ("3DShapeSpectrumClient\n");
#endif
#ifdef USESR
    printf ("RegionShapeServer\n");
    printf ("RegionShapeClient\n");
#endif
#ifdef USE2D3D
    printf ("MultiViewServer\n");
    printf ("MultiViewClient3D3D\n");
    printf ("MultiViewClient3D3DVVO\n");
    printf ("MultiViewClient2D3D\n");
    printf ("MultiViewClient2D3DVVO\n");
#endif
#ifdef USEGL
    printf ("GridLayoutServer\n");
#endif // USEGL
#ifdef USESC
    printf ("ContourShapeServer\n");
    printf ("ContourShapeClient\n");
#endif
#ifdef USECM
    printf ("CameraMotionServer\n");
    printf ("CameraMotionClient\n");
#endif
#ifdef USEMA
    printf ("MotionActivityServer\n");
    printf ("MotionActivityClient\n");
#endif
#ifdef USECM
    printf ("CameraMotionServer\n");
    printf ("CameraMotionClient\n");
#endif
#ifdef USEMT
    printf ("MotionTrajectoryServer\n");
    printf ("MotionTrajectoryClient\n");
#endif
#ifdef USEPOM
    printf ("ParametricObjectMotionServer\n");
    printf ("ParametricObjectMotionClient\n");
#endif
#ifdef USEFR
    printf ("FaceRecognitionServer\n");
    printf ("FaceRecognitionClient\n");
#endif
#ifdef USEMI
    printf ("MultiImageServer\n");
    printf ("MultiImageClient\n");
#endif
#ifdef USERL
    printf ("RegionLocatorServer\n");
    printf ("RegionLocatorClient\n");
#endif
#ifdef USESTL
    printf ("SpatioTemporalLocatorServer\n");
    printf ("SpatioTemporalLocatorClient\n");
#endif
#ifdef USESPKC
    printf ("SpokenContentServer\n");
    printf ("SpokenContentClient\n");
#endif
#ifdef USEMSQ
    printf ("MelodySequenceClient\n");
#endif
#ifdef USEASF
    printf ("AffectiveSegmentFilterClient\n");
#endif
#ifdef USEDPS
    printf ("DependStructureClient\n");
#endif
#ifdef USEETT
    printf ("ExtendedTextualTypeClient\n");
#endif
#ifdef USEGCS
//      printf("GraphicalCSServer\n");
    printf ("GraphicalCSClient\n");
#endif
#ifdef USEHS
    printf ("HierarchicalKeyFrameServer\n");
//       printf("HierarchicalKeyFrameClient\n");
#endif
#ifdef USEIMH
    printf ("ImportanceHintClient\n");
#endif
#ifdef USEINK
    printf ("InkSegmentServer\n");
    printf ("InkSegmentClient\n");
#endif
#ifdef USEMH
    printf ("MatchingHintServer\n");
    printf ("MatchingHintClient\n");
#endif
#ifdef USEMM
//      printf("MediaMetaServer\n");
    printf ("MediaMetaClient\n");
#endif
#ifdef USEMQ
    printf ("MediaQualityServer\n");
//    printf("MediaQualityClient\n");
#endif
#ifdef USEMR
//      printf("MediaReviewServer\n");
    printf ("MediaReviewClient\n");
#endif
#ifdef USEMK
    printf ("MosaicServer\n");
    printf ("MosaicClient\n");
#endif
#ifdef USEODK
   /* OrderingKey */
    printf ("OrderingKeyClient\n");
#endif
#ifdef USEPACK
   /* Packages */
    printf ("PackageClient\n");
#endif
#ifdef USEPTH
    printf ("PhoneticTranslationHintsClient\n");
#endif
#ifdef ___TBC
    printf ("PointOfViewClient\n");
#endif
#ifdef USESG
    printf ("SegmentServer\n");
    printf ("SegmentClient\n");
#endif
#ifdef USESRG
    printf ("SegmentRelGraphServer\n");
    printf ("SegmentRelGraphClient\n");
#endif
#ifdef USESEM
   //printf("SemanticServer\n");
    printf ("SemanticClient\n");
#endif
#ifdef USESMS
    printf ("SeqSummaryFastMovingStoryBoardServer\n");
//      printf("SeqSummaryFastMovingStoryBoardClient\n");
#endif
#ifdef USESKW
    printf ("SeqSummaryKeywordsServer\n");
//      printf("SeqSummaryKeywordsClient\n");
#endif
#ifdef USESSZ
    printf ("SequentialSummaryServer\n");
//      printf("SequentialSummaryClient\n");
#endif
#ifdef USESP
    printf ("SoundPropertyServer\n");
//       printf("SoundPropertyClient\n");
#endif
#ifdef USESRH
    printf ("SpatialResolutionHintClient\n");
#endif
#ifdef USESTM
//      printf("StateMemberServer\n");
    printf ("StateMemberClient\n");
#endif
#ifdef USESTC
    printf ("StructuredCollectionServer\n");
    printf ("StructuredCollectionClient\n");
#endif
#ifdef USESZ
    printf ("SummarizationServer\n");
    printf ("SummarizationClient\n");
#endif
#ifdef USESPR
    printf ("SummaryPreferencesClient\n");
#endif
#ifdef USETH
    printf ("TranscodingServer\n");
   // printf("TranscodingClient\n");
#endif
#ifdef USEUP
//       printf("UserPreferencesServer\n");
    printf ("UserPreferencesClient\n");
#endif
#ifdef USEVE
    printf ("VideoEditingServer\n");
   /* printf("VideoEditingClient\n"); */
#endif
#ifdef USEVT
    printf ("VideoTextServer\n");
#endif
    return 0;
  }

#ifdef USEDT
 /* DummyType database extraction */
  if (!strcmp (ParameterSet->Application, "DummyTypeServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::DummyTypeServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* DummyType query retrieval */
  if (!strcmp (ParameterSet->Application, "DummyTypeClient")) {
    XM::DummyTypeClient theClient;

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

#ifdef USEDTDS
 /* DSDummyType database extraction */
  if (!strcmp (ParameterSet->Application, "DSDummyTypeServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::DSDummyTypeServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* DSDummyType query retrieval */
  if (!strcmp (ParameterSet->Application, "DSDummyTypeClient")) {
    XM::DSDummyTypeClient theClient;

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


#ifdef USETS
 /* if paramters are not set, get them from parameter file */
 //sth put this to application!!
 //  if (!DescriptorId) DescriptorId=(short)atoi(getpara("DescriptorId","10"));
 //  if (!AddtParamParameterSet->ListFile) AddtParamParameterSet->ListFile=getpara("AddtParamListFile",(char *)NULL);

 /* time series database extraction */
  if (!strcmp (ParameterSet->Application, "TimeSeriesServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::TimeSeriesServer theServer;

      theServer.Open ();
     //sth use required interface
     //theServer.Start(DescriptorId, ParameterSet->ListFile, ParameterSet->Bitstream, AddtParamListFile);
     theServer.Start(ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* time series query retrieval */
  if (!strcmp (ParameterSet->Application, "TimeSeriesClient")) {
    XM::TimeSeriesClient theClient;

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream && ParameterSet->Query
	&& ParameterSet->NoOfMatches))
      return 1;
    else {
     //sth use required interface
     //theClient.Open(DescriptorId, ParameterSet->ListFile, ParameterSet->Bitstream, AddtParamListFile);
      theClient.Start (ParameterSet->Query, ParameterSet->NoOfMatches);
      theClient.Stop ();
      theClient.Close ();

      return 0;
    }
  }
#endif

#ifdef USECC
 /* compact color histogram database extraction */
  if (!strcmp (ParameterSet->Application, "CompactColorServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::CompactColorServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* compact color histogram query retrieval */
  if (!strcmp (ParameterSet->Application, "CompactColorClient")) {
    XM::CompactColorClient theClient;

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


 /* color histogram database extraction */
#ifdef USECH
  if (!strcmp (ParameterSet->Application, "ColorHistogramServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::ColorHistogramServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* color histogram query retrieval */
  if (!strcmp (ParameterSet->Application, "ColorHistogramClient")) {
    XM::ColorHistogramClient theClient;

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

#ifdef USECL
 /* color layout database extraction */
  if (!strcmp (ParameterSet->Application, "ColorLayoutServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::ColorLayoutServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* color layout query retrieval */
  if (!strcmp (ParameterSet->Application, "ColorLayoutClient")) {
    XM::ColorLayoutClient theClient;

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

#ifdef USECT
 /* color structure database extraction */
  if (!strcmp (ParameterSet->Application, "ColorStructureServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::ColorStructureServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* color structure query retrieval */
  if (!strcmp (ParameterSet->Application, "ColorStructureClient")) {
    XM::ColorStructureClient theClient;

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream && ParameterSet->Query
	&& ParameterSet->NoOfMatches))
      return 1;
    else {
      theClient.Open (ParameterSet->ListFile, ParameterSet->Bitstream);
      if (strstr (ParameterSet->Query,
	  ".lst") ==
	ParameterSet->Query + strlen (ParameterSet->Query) - strlen (".lst")) {
	char singleQuery[MAXMEDIANAMELEN];
	char singleResult[MAXMEDIANAMELEN];
	FILE *fp = fopen (ParameterSet->Query, "r");
	while (fscanf (fp, "%s %s\n", singleQuery,
	    singleResult) == 2) {
	  if ((theClient.m_ResultFile = fopen (singleResult, "w"))) {
	    theClient.Start (singleQuery,
	      ParameterSet->NoOfMatches);
	    fclose (theClient.m_ResultFile);
	  }
	  else
	    fprintf (stderr, "Unable to open %s for write", singleResult);
	}
      }
      else
	theClient.Start (ParameterSet->Query, ParameterSet->NoOfMatches);
      theClient.Stop ();
      theClient.Close ();

      return 0;
    }
  }
#endif

#ifdef USEDC
 /* dominant color database extraction */
  if (!strcmp (ParameterSet->Application, "DominantColorServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::DominantColorServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* dominant color query retrieval */
  if (!strcmp (ParameterSet->Application, "DominantColorClient")) {
    XM::DominantColorClient theClient;

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

#ifdef USEGF
 /* group of frames color histogram database extraction */
  if (!strcmp (ParameterSet->Application, "GoFGoPColorServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::GoFGoPColorServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* group of frames color histogram query retrieval */
  if (!strcmp (ParameterSet->Application, "GoFGoPColorClient")) {
    XM::GoFGoPColorClient theClient;

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

#ifdef USESCC
 /* scalable color database extraction */
  if (!strcmp (ParameterSet->Application, "ScalableColorServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::ScalableColorServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* scalable color query retrieval */
  if (!strcmp (ParameterSet->Application, "ScalableColorClient")) {
    XM::ScalableColorClient theClient;

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

#ifdef USETB
 /* texture browsing database extraction */
  if (!strcmp (ParameterSet->Application, "TextureBrowsingServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::TextureBrowsingServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* texture browsing query retrieval */
  if (!strcmp (ParameterSet->Application, "TextureBrowsingClient")) {
    XM::TextureBrowsingClient theClient;

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

#ifdef USEHT
 /* homogeneous texture database extraction */
  if (!strcmp (ParameterSet->Application, "HomogeneousTextureServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::HomogeneousTextureServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* homogeneous texture query retrieval */
  if (!strcmp (ParameterSet->Application, "HomogeneousTextureClient")) {
    XM::HomogeneousTextureClient theClient;

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

#ifdef USEEH
 /* edge histogram database extraction */
  if (!strcmp (ParameterSet->Application, "EdgeHistogramServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::EdgeHistogramServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* edge histogram query retrieval */
  if (!strcmp (ParameterSet->Application, "EdgeHistogramClient")) {
    XM::EdgeHistogramClient theClient;

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

#ifdef USE3DSS
 /*  3D Shape Spectrum database extraction */
  if (!strcmp (ParameterSet->Application, "3DShapeSpectrumServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::ShapeSpectrumServer theShapeSpectrumServer;

      theShapeSpectrumServer.Open ();
      theShapeSpectrumServer.Start (ParameterSet->ListFile,
	ParameterSet->Bitstream);
      theShapeSpectrumServer.Stop ();
      theShapeSpectrumServer.Close ();

      return 0;
    }
  }
 /*  3D Shape Spectrum query retrieval */
  if (!strcmp (ParameterSet->Application, "3DShapeSpectrumClient")) {
    XM::ShapeSpectrumClient theShapeSpectrumClient;

//    ParameterSet->NoOfMatches = atoi (a);
    fprintf (stderr, "3DShapeSpectrumClient ParameterSet->NoOfMatches = %d\n",
      ParameterSet->NoOfMatches);
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream && ParameterSet->Query
	&& ParameterSet->NoOfMatches))
      return 1;
    else {
      theShapeSpectrumClient.Open (ParameterSet->ListFile,
	ParameterSet->Bitstream);
      theShapeSpectrumClient.Start (ParameterSet->Query,
	ParameterSet->NoOfMatches);
      theShapeSpectrumClient.Stop ();
      theShapeSpectrumClient.Close ();

      return 0;
    }
  }

#endif

#ifdef ___TBC
 /* bounding box database extraction */
  if (!strcmp (ParameterSet->Application, "BBoxServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::BBoxServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* bounding box query retrieval */
  if (!strcmp (ParameterSet->Application, "BBoxClient")) {
    XM::BBoxClient theClient;

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


#ifdef USESC
 /* contour based shape database extraction */
  if (!strcmp (ParameterSet->Application, "ContourShapeServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::ContourShapeServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* contour based shape query retrieval */
  if (!strcmp (ParameterSet->Application, "ContourShapeClient")) {
    XM::ContourShapeClient theClient;

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

#ifdef USESR
 /* region based shape database extraction */
  if (!strcmp (ParameterSet->Application, "RegionShapeServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::RegionShapeServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* region based shape query retrieval */
  if (!strcmp (ParameterSet->Application, "RegionShapeClient")) {
    XM::RegionShapeClient theClient;

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

#ifdef USE2D3D
// 2D/3D extraction server
  if (!strcmp (ParameterSet->Application, "MultiViewServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::MultiViewServer theServer;
      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();
      return 0;
    }
  }

// 3D -> 3D search 
  if (!strcmp (ParameterSet->Application, "MultiViewClient3D3D")) {
    XM::MultiViewClient theClient;
//    ParameterSet->NoOfMatches = atoi (a);
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream && ParameterSet->Query
	&& ParameterSet->NoOfMatches))
      return 1;
    else {
      theClient.Open (ParameterSet->ListFile, ParameterSet->Bitstream);
      theClient.Start (ParameterSet->Query, ParameterSet->NoOfMatches,
	ANYVIEWSEARCH);
      theClient.Stop ();
      theClient.Close ();
      return 0;
    }
  }

 //sth why can't this be set via parametrs in the parameterfile without
 //sth violating the interfaces
// 3D -> 3D search using only visible views within the source material
  if (!strcmp (ParameterSet->Application, "MultiViewClient3D3DVVO")) {
    XM::MultiViewClient theClient;
//    ParameterSet->NoOfMatches = atoi (a);
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream && ParameterSet->Query
	&& ParameterSet->NoOfMatches))
      return 1;
    else {
      theClient.Open (ParameterSet->ListFile, ParameterSet->Bitstream);
      theClient.Start (ParameterSet->Query, ParameterSet->NoOfMatches,
	VISIBLEVIEWONLYSEARCH);
      theClient.Stop ();
      theClient.Close ();

      return 0;
    }
  }

// 2D -> 3D search
  if (!strcmp (ParameterSet->Application, "MultiViewClient2D3D")) {
    XM::MultiViewClient theClient;
//    ParameterSet->NoOfMatches = atoi (a);
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream && ParameterSet->Query
	&& ParameterSet->NoOfMatches))
      return 1;
    else {
      theClient.Open (ParameterSet->ListFile, ParameterSet->Bitstream);
      theClient.Start2D (ParameterSet->Query, ParameterSet->NoOfMatches,
	ANYVIEWSEARCH);
      theClient.Stop ();
      theClient.Close ();

      return 0;
    }
  }

// 2D -> 3D search using only visible views within the source material
  if (!strcmp (ParameterSet->Application, "MultiViewClient2D3DVVO")) {
    XM::MultiViewClient theClient;
//    ParameterSet->NoOfMatches = atoi (a);
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream && ParameterSet->Query
	&& ParameterSet->NoOfMatches))
      return 1;
    else {
      theClient.Open (ParameterSet->ListFile, ParameterSet->Bitstream);
      theClient.Start2D (ParameterSet->Query, ParameterSet->NoOfMatches,
	VISIBLEVIEWONLYSEARCH);
      theClient.Stop ();
      theClient.Close ();
      return 0;
    }
  }
#endif /*USE2D3D */

#ifdef USEGL
  /* GridLayout Server */
  if (!strcmp (ParameterSet->Application, "GridLayoutServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
	  XM::GridLayoutServer theServer;
      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();
      return 0;
    }
  }
#endif // USEGL

#ifdef USESTL
 /* spatio temporal locator database extraction */
  if (!strcmp (ParameterSet->Application, "SpatioTemporalLocatorServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::SpatioTemporalLocatorServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* spatio temporal locator query retrieval */
  if (!strcmp (ParameterSet->Application, "SpatioTemporalLocatorClient")) {
    XM::SpatioTemporalLocatorClient theClient;

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

#ifdef USECM
 /*  camera motion database extraction */
  if (!strcmp (ParameterSet->Application, "CameraMotionServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::CameraMotionServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* camera motion query retrieval */
  if (!strcmp (ParameterSet->Application, "CameraMotionClient")) {
    XM::CameraMotionClient theClient;

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

#ifdef USEMA
 /*  motion activity database extraction */
  if (!strcmp (ParameterSet->Application, "MotionActivityServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::MotionActivityServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* motion activity query retrieval */
  if (!strcmp (ParameterSet->Application, "MotionActivityClient")) {
    XM::MotionActivityClient theClient;

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

#ifdef USERL
 /* region location extraction */
  if (!strcmp (ParameterSet->Application, "RegionLocatorServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::RegionLocatorServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* region location retrieval */
  if (!strcmp (ParameterSet->Application, "RegionLocatorClient")) {
    XM::RegionLocatorClient theClient;

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream && ParameterSet->Query
	&& ParameterSet->NoOfMatches))
      return 1;
    else {
      theClient.Open (ParameterSet->ListFile, ParameterSet->Bitstream);
      theClient.Start (ParameterSet->Query, 0);
      theClient.Stop ();
      theClient.Close ();

      return 0;
    }
  }
#endif


#ifdef USEMT
 /* object motion trajectory database extraction */
  if (!strcmp (ParameterSet->Application, "MotionTrajectoryServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::MotionTrajectoryServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* object motion trajectory query retrieval */
  if (!strcmp (ParameterSet->Application, "MotionTrajectoryClient")) {
    XM::MotionTrajectoryClient theClient;

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

#ifdef USEPOM
 /* parametric object motion database extraction */
  if (!strcmp (ParameterSet->Application, "ParametricObjectMotionServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::ParametricObjectMotionServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* parametric object motion query retrieval */
  if (!strcmp (ParameterSet->Application, "ParametricObjectMotionClient")) {
    XM::ParametricObjectMotionClient theClient;

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

#ifdef USEFR
 /* face recognition database extraction */
  if (!strcmp (ParameterSet->Application, "FaceRecognitionServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::FaceRecognitionServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* face recognition query retrieval */
  if (!strcmp (ParameterSet->Application, "FaceRecognitionClient")) {
    XM::FaceRecognitionClient theClient;

//     ParameterSet->NoOfMatches=atoi(a);
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

#ifdef USEMI
 /* Image Multi feature database extraction */
  if (!strcmp (ParameterSet->Application, "MultiImageServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::MultiImageServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* Image Multi feature query retrieval */
  if (!strcmp (ParameterSet->Application, "MultiImageClient")) {
    XM::MultiImageClient theClient;

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

#ifdef USESPKC
 // SpokenContent database extraction
  if (!strcmp (ParameterSet->Application, "SpokenContentServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::SpokenContentServer theServer;
      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 // SpokenContent query retrieval
  if (!strcmp (ParameterSet->Application, "SpokenContentClient")) {
    XM::SpokenContentClient theClient;

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

#ifdef USEMSQ
 /* MelodySequence query retrieval */
  if (!strcmp (ParameterSet->Application, "MelodySequenceClient")) {
    XM::MelodySequenceClient theClient;

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

#ifdef USEASF
  if (!strcmp (ParameterSet->Application, "AffectiveSegmentFilterClient")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::AffectiveSegmentFilterClient theAffectiveSegmentFilterClient;
      theAffectiveSegmentFilterClient.Open (ParameterSet->ListFile,
	ParameterSet->Bitstream);
      theAffectiveSegmentFilterClient.Start ();
      theAffectiveSegmentFilterClient.Stop ();
      theAffectiveSegmentFilterClient.Close ();

      return 0;
    }
  }
#endif

#ifdef USEDPS
  if (!strcmp (ParameterSet->Application, "DependStructureClient")) {
    if (!(ParameterSet->ListFile && !ParameterSet->Bitstream
	&& ParameterSet->Query))
      return 1;
    else {

      XM::DependStructureClient theDependStructureClient;
      theDependStructureClient.Open (ParameterSet->ListFile,
	ParameterSet->Bitstream);
      theDependStructureClient.Start (ParameterSet->Query,
	ParameterSet->NoOfMatches);
      theDependStructureClient.Stop ();
      theDependStructureClient.Close ();

      return 0;
    }
  }
#endif

#ifdef USEETT
 /* Extended Textual Type */
  if (!strcmp (ParameterSet->Application, "ExtendedTextualTypeCAppl")) {
    XM::ExtendedTextualTypeClient theClient;
    if (!(ParameterSet->ListFile && ParameterSet->Query))
      return 1;
    else {
      theClient.Open (ParameterSet->ListFile);
      theClient.Start (ParameterSet->Query);
      theClient.Stop ();
      theClient.Close ();

      return 0;
    }
  }
#endif

#ifdef USEGCS
 /* GraphicalClassificationScheme retrieval */
  if (!strcmp (ParameterSet->Application, "GraphicalCSClient")) {
    XM::GraphicalCSClient theClient;

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

#ifdef USEHS
 /* Hierarchical KeyFrame Summarization DS extraction */
  if (!strcmp (ParameterSet->Application, "HierarchicalKeyFrameServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::HierarchicalKeyFrameServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }
#endif

#ifdef USEIMH
 /* Importance hint transcoding */

  if (!strcmp (ParameterSet->Application, "ImportanceHintClient")) {
    XM::ImportanceHintClient theClient;

//    if (a)
//      ParameterSet->NoOfMatches = atoi (a);	//sth I guess that should disappear
//    else
//      ParameterSet->NoOfMatches = 0;
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
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

#ifdef USEINK
 //InkSegment
  if (!strcmp (ParameterSet->Application, "InkSegmentClient")) {
    XM::InkSegmentClient theClient;

//    if (a)
//      ParameterSet->NoOfMatches = atoi (a);
//    else
//      ParameterSet->NoOfMatches = 0;
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {

      theClient.Open (ParameterSet->ListFile, ParameterSet->Bitstream);
      theClient.Start (ParameterSet->Query, ParameterSet->NoOfMatches);
      theClient.Stop ();
      theClient.Close ();

      return 0;
    }
  }

  if (!strcmp (ParameterSet->Application, "InkSegmentServer")) {
    XM::InkSegmentServer theServer;

//    if (a)
//      ParameterSet->NoOfMatches = atoi (a);
//    else
//      ParameterSet->NoOfMatches = 0;
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }
#endif


#ifdef USEMH
/* MatchingHint database extraction*/
  if (!strcmp (ParameterSet->Application, "MatchingHintServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Query))
      return 1;
    else {
      XM::MatchingHintServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Query);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

/* MatchingHint query retrieval*/
  if (!strcmp (ParameterSet->Application, "MatchingHintClient")) {
    XM::MatchingHintClient theClient;

    if (!(ParameterSet->ListFile && ParameterSet->Query
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

#ifdef USEMM
 /* Meta Media extraction */
//     if (!strcmp(ParameterSet->Application,"MediaMetaServer")) {
//       if (!(ParameterSet->ListFile && ParameterSet->Bitstream)) return 1;
//       else {
//      XM::MediaMetaServer theServer;

//      theServer.Open();
//      theServer.Start(ParameterSet->ListFile, ParameterSet->Bitstream);
//      theServer.Stop();
//      theServer.Close();

//      return 0;
//       }
//     }

 /* Meta Media retrieval */
  if (!strcmp (ParameterSet->Application, "MediaMetaClient")) {
    XM::MediaMetaClient theClient;

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

#ifdef USEMQ
 /* MediaQuality extraction */
  if (!strcmp (ParameterSet->Application, "MediaQualityServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::MediaQualityServer theMediaQualityServer;
      theMediaQualityServer.Open ();
      theMediaQualityServer.Start (ParameterSet->ListFile,
	ParameterSet->Bitstream);
      theMediaQualityServer.Stop ();
      theMediaQualityServer.Close ();

      return 0;
    }
  }
#endif

#ifdef USEMR
  if (!strcmp (ParameterSet->Application, "MediaReviewClient")) {
    XM::MediaReviewClient theMediaReviewClient;

//    ParameterSet->NoOfMatches = atoi (a);
    if (!(ParameterSet->ListFile && ParameterSet->Query
	&& ParameterSet->NoOfMatches))
      return 1;
    else {
      ParameterSet->Bitstream = 0;
      theMediaReviewClient.Open (ParameterSet->ListFile, ParameterSet->Bitstream);
      theMediaReviewClient.Start (ParameterSet->Query, ParameterSet->NoOfMatches);
      theMediaReviewClient.Stop ();
      theMediaReviewClient.Close ();

      return 0;
    }
  }
#endif

#ifdef USEMK
 /* Mosaic extraction */
  if (!strcmp (ParameterSet->Application, "MosaicServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::MosaicServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* Mosaic retrieval */
  if (!strcmp (ParameterSet->Application, "MosaicClient")) {
    XM::MosaicClient theClient;

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream
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

#ifdef USEMO
 /* Model extraction */
  if (!strcmp (ParameterSet->Application, "ModelServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::ModelServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* Model retrieval */
//     if (!strcmp(ParameterSet->Application,"ModelClient")) {
//       XM::ModelClient theClient;

//       if (!(ParameterSet->ListFile && ParameterSet->Bitstream && ParameterSet->Query && ParameterSet->NoOfMatches)) return 1;
//       else {
//      theClient.Open(ParameterSet->ListFile, ParameterSet->Bitstream);
//      theClient.Start(ParameterSet->Query, ParameterSet->NoOfMatches);
//      theClient.Stop();
//      theClient.Close();

//      return 0;
//       }
//     }
#endif

#ifdef USEODK
 /* OrderingKey */

  if (!strcmp (ParameterSet->Application, "OrderingKeyClient")) {
    XM::OrderingKeyClient theClient;

//    if (a)
//      ParameterSet->NoOfMatches = atoi (a);
//    else
//      ParameterSet->NoOfMatches = 0;
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
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

#ifdef USEPACK
 /* Packages */

  if (!strcmp (ParameterSet->Application, "PackageClient")) {
    XM::PackageClient theClient;

//    if (a)
//      ParameterSet->NoOfMatches = atoi (a);
//    else
//      ParameterSet->NoOfMatches = 0;
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
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

#ifdef USEPTH
/* Phonetic Translation Hints */
  if (!strcmp (ParameterSet->Application, "PhoneticTranslationHintsCAppl")) {
    XM::PhoneticTranslationHintsClient theClient;
    if (!(ParameterSet->ListFile && ParameterSet->Query))
      return 1;
    else {
      theClient.Open (ParameterSet->ListFile);
      theClient.Start (ParameterSet->Query);
      theClient.Stop ();
      theClient.Close ();

      return 0;
    }
  }
#endif

#ifdef ___TBC
 // Add by Matsushita
  if (!strcmp (ParameterSet->Application, "PointOfViewClient")) {
    if (!(ParameterSet->ListFile && ParameterSet->Query)) {
      return 1;
    }
    else {
      XM::PointOfViewClient theClient;
      theClient.Open (ParameterSet->ListFile, ParameterSet->Bitstream);
      theClient.Start (ParameterSet->Query, ParameterSet->NoOfMatches);
      theClient.Stop ();
      theClient.Close ();

      return 0;
    }
  }
#endif

#ifdef USESG
 /* Segment extraction */
  if (!strcmp (ParameterSet->Application, "SegmentServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::SegmentServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* Segment retrieval */
  if (!strcmp (ParameterSet->Application, "SegmentClient")) {
    XM::SegmentClient theClient;

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream &&
	  ParameterSet->Query && ParameterSet->NoOfMatches))
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

#ifdef USESRG
 /* Segment Relation Graph extraction */
  if (!strcmp (ParameterSet->Application, "SegmentRelGraphServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::SegmentRelGraphServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* Segment Relation Graph retrieval */
  if (!strcmp (ParameterSet->Application, "SegmentRelGraphClient")) {
    XM::SegmentRelGraphClient theClient;

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

#ifdef USESEM
 /* Semantic extraction */
 //if (!strcmp(ParameterSet->Application,"SemanticServer")) {
 // if (!(ParameterSet->ListFile && ParameterSet->Bitstream)) return 1;
 // else {
 //XM::SemanticServer theServer;

 //theServer.Open();
 //theServer.Start(ParameterSet->ListFile, ParameterSet->Bitstream);
 //theServer.Stop();
 //theServer.Close();

 //return 0;
 //  }
 //}

 /* Sematnic retrieval */
  if (!strcmp (ParameterSet->Application, "SemanticClient")) {
    XM::SemanticClient theClient;

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

#ifdef USESMS
 /* SeqSummaryFastMovingStoryBoard extraction */
  if (!strcmp (ParameterSet->Application,
      "SeqSummaryFastMovingStoryBoardServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::SeqSummaryFastMovingStoryBoardServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }
#endif

#ifdef USESKW
 /* SeqSummaryKeywords extraction */
  if (!strcmp (ParameterSet->Application, "SeqSummaryKeywordsServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::SeqSummaryKeywordsServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }
#endif

#ifdef USESSZ
 /* SequentialSummary extraction */
  if (!strcmp (ParameterSet->Application, "SequentialSummaryServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::SequentialSummaryServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }
#endif

#ifdef USESP
 /* SoundProperty extraction */
  if (!strcmp (ParameterSet->Application, "SoundPropertyServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::SoundPropertyServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }
#endif

#ifdef USESRH
 /* SpatialResReduction hint transcoding */

  if (!strcmp (ParameterSet->Application, "SpatialResolutionHintClient")) {
    XM::SpatialResolutionHintClient theClient;

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
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

#ifdef USESTM
 /* SemanticState Membership function retrieval */
  if (!strcmp (ParameterSet->Application, "StateMemberClient")) {
    XM::StateMemberClient theClient;

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

#ifdef USESFV
 /* SpaceFrequencyView server */
  if (!strcmp (ParameterSet->Application, "SpaceFrequencyViewServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::SpaceFrequencyViewServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

#endif

#ifdef USESTX
 /* StateTransitionModel server */
  if (!strcmp (ParameterSet->Application, "StateTransitionModelServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::StateTransitionModelServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

#endif

#ifdef USESRH
 /* SpatialResolution hint transcoding */
  if (!strcmp (ParameterSet->Application, "SpatialResolutionHintClient")) {
    XM::SpatialResolutionHintClient theClient;
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
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

#ifdef USESTC
 /* collection structure database extraction */
  if (!strcmp (ParameterSet->Application, "StructuredCollectionServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::StructuredCollectionServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* collection structure query retrieval */
  if (!strcmp (ParameterSet->Application, "StructuredCollectionClient")) {
    XM::StructuredCollectionClient theClient;

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

#ifdef USESZ
 /* Summarization extraction */
  if (!strcmp (ParameterSet->Application, "SummarizationServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::SummarizationServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* Summarization retrieval */
  if (!strcmp (ParameterSet->Application, "SummarizationClient")) {
    XM::SummarizationClient theClient;

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

#ifdef USESPR
 // Add by Matsushita
  if (!strcmp (ParameterSet->Application, "SummaryPreferencesClient")) {
    if (!(ParameterSet->ListFile && ParameterSet->Query)) {
      return 1;
    }
    else {
      XM::SummaryPreferencesClient theClient;
      theClient.Open (ParameterSet->ListFile, ParameterSet->Bitstream);
      theClient.Start (ParameterSet->Query, ParameterSet->NoOfMatches);
      theClient.Stop ();
      theClient.Close ();

      return 0;
    }
  }
#endif

#ifdef USETH
 // added by Sony Japan / Peter Kuhn 
 /* Transcoding extraction */
  if (!strcmp (ParameterSet->Application, "TranscodingServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::TranscodingServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* Transcoding application */
 /*
    if (!strcmp(ParameterSet->Application,"TranscodingClient")) {
    XM::TranscodingClient theClient;

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream && ParameterSet->Query && ParameterSet->NoOfMatches)) return 1;
    else {
    theClient.Open(ParameterSet->ListFile, ParameterSet->Bitstream);
    theClient.Start(ParameterSet->Query, ParameterSet->NoOfMatches);
    theClient.Stop();
    theClient.Close();

    return 0;
    }
    }
  */
#endif

#ifdef USEUP
 /* User Preferences extraction */
//     if (!strcmp(ParameterSet->Application,"UserPreferencesServer")) {
//       if (!(ParameterSet->ListFile && ParameterSet->Bitstream)) return 1;
//       else {
//      XM::UserPreferencesServer theServer;

//      theServer.Open();
//      theServer.Start(ParameterSet->ListFile, ParameterSet->Bitstream);
//      theServer.Stop();
//      theServer.Close();

//      return 0;
//       }
//     }

 /* User Preference retrieval */
  if (!strcmp (ParameterSet->Application, "UserPreferenceClient")) {
    XM::UserPreferenceClient theClient;

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

#ifdef USEVA
 /* Variation extraction */
  if (!strcmp (ParameterSet->Application, "VariationServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::VariationServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* Variation retrieval */
 /*  if (!strcmp(ParameterSet->Application,"VariationClient")) {
    XM::VariationClient theClient;

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream && ParameterSet->Query && ParameterSet->NoOfMatches)) return 1;
    else {
    theClient.Open(ParameterSet->ListFile, ParameterSet->Bitstream);
    theClient.Start(ParameterSet->Query, ParameterSet->NoOfMatches);
    theClient.Stop();
    theClient.Close();

    return 0;
    }
    } */
#endif

#ifdef USEVE
 /* VideoEditing database extraction */
  if (!strcmp (ParameterSet->Application, "VideoEditingServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::VideoEditingServer theServer;
      printf ("VideoEditingServer\n");
      printf ("BitStream:%s \n", ParameterSet->Bitstream);

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }

 /* VideoEditing query retrieval */
 /*if (!strcmp(ParameterSet->Application,"VideoEditingClient")) {
    XM::VideoEditingClient theClient;

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream &&
    ParameterSet->Query && ParameterSet->NoOfMatches)) return 1;
    else {
    theClient.Open(ParameterSet->ListFile, ParameterSet->Bitstream);
    theClient.Start(ParameterSet->Query, ParameterSet->NoOfMatches);
    theClient.Stop();
    theClient.Close();

    return 0;
    }
    } */
#endif

#ifdef USEVT
 /* VideoText extraction */
  if (!strcmp (ParameterSet->Application, "VideoTextServer")) {
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::VideoTextServer theServer;

      theServer.Open ();
      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);
      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }
#endif
  return -1;
}

//---------------------------------------------------------------------------
int printhelp (TXMApplicationParameterSet * ParameterSet, int type_of_messages)
{
  if (!ParameterSet)
    return 1;
  
  if (type_of_messages == 0) return 0;
  if (type_of_messages > 0) {

    char empty[1] = "";

    if (!ParameterSet->ParameterFileName)
      ParameterSet->ParameterFileName = empty;
    if (!ParameterSet->Application)
      ParameterSet->Application = empty;
    if (!ParameterSet->ListFile)
      ParameterSet->ListFile = empty;
    if (!ParameterSet->Bitstream)
      ParameterSet->Bitstream = empty;
    if (!ParameterSet->Query)
      ParameterSet->Query = empty;
    if (!ParameterSet->LogRedirection)
      ParameterSet->LogRedirection = empty;
    if (!ParameterSet->ResultRedirection)
      ParameterSet->ResultRedirection = empty;

    fprintf (stderr, "Error: Wrong parameter list\n"
	     "call XM with:\n"
	     "XMMain.exe -pParameterfile -aApplication -lListfile -bBitstream "
	     "-qQuery -nNoOfMatches -rLogRedirection -oResultRedirection\n\n"
	     "Parameterfile    = parameter file for application(%s)\n"
	     "Application      = selected XM-application(%s)\n"
	     "Listfile         = file with list of filenames of database"
	     "(%s)\n"
	     "Bitstream        = MPEG-7 bitstream (%s)\n"
	     "Query            = filename of query (%s)\n"
	     "NoOfMatches      = size of list with best matches (%d)\n"
	     "LogRedirection   = file where all the log messeges are going to "
	     "(%s)\n"
	     "ResultRedirection= file where all the results are going to "
	     "(%s)\n",
	     ParameterSet->ParameterFileName, ParameterSet->Application,
	     ParameterSet->ListFile, ParameterSet->Bitstream,
	     ParameterSet->Query, ParameterSet->NoOfMatches,
	     ParameterSet->LogRedirection, ParameterSet->ResultRedirection);
  }
  else {
    fprintf (stderr, "Error : Application \"%s\" unknown\n",
      ParameterSet->Application);
  }

  return 0;
}
