///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann, Munich University of Technology
// (contributing organizations names)
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
// XM_Main.cpp

#define TIMING_TEST

#ifdef WIN32
#pragma warning(disable : 4786)
#endif // WIN32

//#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>
#include <time.h>

#include "XMManager/ComponentDB.h"
#include "readparam.h"
#include "Applications/ServerApplications.h"
#include "ClientApplications.h"
/*#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"*/
#include "EvaluationTools/EvaluationTools.h"
#include "Utilities/Utilities.h"
#include "Version1.h"
#include "Media/Media.h"
#include "MyExif.h"


//#define BITSTREAMTEST
#ifdef BITSTREAMTEST
#include "Encoders/Encoders.h"
#include "Decoders/Decoders.h"
#endif


typedef unsigned int UINT;
extern LIS::Param *LISParam_default;

bool operator <(const DATEORD& a, const DATEORD& b);
//void normalise(double* vec,int length);
void normalise(double* vec, double mu, double sigma, int length);
double project(double* vec1, double* vec2, int length);
double TMatching(struct tm &t1, struct tm &t2);
void convert_time(char datestring[20], struct tm &outtime);
void labelBoundaries(double** distmat,int DBSIZE,bool* bounds, double thresh);
void saveClust(char * fname, bool* bounds, std::vector<std::string>& imlist);

void stripCR(char *mystring);


//---------------------------------------------------------------------------
TXMApplicationParameterSet *parse_cmdline (int argc, char **argv)
{
 //   XM::ComponentDatabase  theComponentDatabase;
  static TXMApplicationParameterSet ParameterSet;
  ParameterSet.Thresh=3.2;

 //  char *ListFile=0;
 //char *Query=0;
 //char *ParameterFileName=0;
 //char *Bitstream=0;
 //char *Application=0;

 //char *LogRedirection=0;
  char *a = 0;

  ParameterSet.ListFile = 0;
  ParameterSet.Application=0;
  ParameterSet.LogRedirection=0;

#ifdef XYUVDISP
  XYUV_INIT (argc, argv);
#endif
 /* check parameters of program call */
 /* check if help will be printed */
  for (int i = 0; i < argc; i++) {
    if (!strcmp (argv[i], "-h")) {
      return &ParameterSet;
    }
    if (!strcmp (argv[i], "h")) {
      return &ParameterSet;
    }
    if (!strcmp (argv[i], "-?")) {
      return &ParameterSet;
    }
    if (!strcmp (argv[i], "?")) {
      return &ParameterSet;
    }

    /* check stdout output clustering parameter */
    if (strstr (argv[i], "-o") == argv[i]) {
      ParameterSet.ClusterFile = &(argv[i][2]);
      if (*ParameterSet.ClusterFile) {
	fopen (ParameterSet.ClusterFile, "w");

	continue;
      }
     /* redirection name in next parameter */
      ParameterSet.ClusterFile = 0;
      if ((i + 1) == argc) {
	return &ParameterSet;
      }
      if (argv[i + 1][0] == '-') {
	return &ParameterSet;
      }
      i++;
      ParameterSet.ClusterFile = &(argv[i][0]);
      fopen (ParameterSet.ClusterFile, "w");
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
	return &ParameterSet;
      }
      if (argv[i + 1][0] == '-') {
	return &ParameterSet;
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
	return &ParameterSet;
      }
      if (argv[i + 1][0] == '-') {
	return &ParameterSet;
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
	return &ParameterSet;
      }
      if (argv[i + 1][0] == '-') {
	return &ParameterSet;
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
	return &ParameterSet;
      }
      if (argv[i + 1][0] == '-') {
	return &ParameterSet;
      }
      i++;
      ParameterSet.Bitstream = &(argv[i][0]);
    }

//////////////////////////////////////////// now several different parfiles!!
   /* check DC parammeterfile parameter */
    if (strstr (argv[i], "-pDC") == argv[i])
	{
      a = &(argv[i][4]);
      if (!(*a))
	  {
		  /* query name in next parameter */
		  if ((i + 1) == argc)
			  return &ParameterSet;
		  if (argv[i + 1][0] == '-')
			  return &ParameterSet;
		  i++;
		  a = &(argv[i][0]);
	  }
      if (*a)
	  {
		  /* read parameter file */
		  //postpone reading the parfile until just before each ClientApplication opens
		  //readparameters (a);

      }
      if(!ParameterSet.DCParameterFileName)
		  ParameterSet.DCParameterFileName = a;
	}
//****************************
	/* check SC parammeterfile parameter */
    if (strstr (argv[i], "-pSC") == argv[i])
	{
      a = &(argv[i][4]);
      if (!(*a))
	  {
		  /* query name in next parameter */
		  if ((i + 1) == argc)
			  return &ParameterSet;
		  if (argv[i + 1][0] == '-')
			  return &ParameterSet;
		  i++;
		  a = &(argv[i][0]);
	  }
      if (*a)
	  {
		  /* read parameter file */
		  //postpone reading the parfile until just before each ClientApplication opens
		  //readparameters (a);

      }
      if(!ParameterSet.SCParameterFileName)
		  ParameterSet.SCParameterFileName = a;
	}
//****************************
	/* check CS parammeterfile parameter */
    if (strstr (argv[i], "-pCS") == argv[i])
	{
      a = &(argv[i][4]);
      if (!(*a))
	  {
		  /* query name in next parameter */
		  if ((i + 1) == argc)
			  return &ParameterSet;
		  if (argv[i + 1][0] == '-')
			  return &ParameterSet;
		  i++;
		  a = &(argv[i][0]);
	  }
      if (*a)
	  {
		  /* read parameter file */
		  //postpone reading the parfile until just before each ClientApplication opens
		  //readparameters (a);

      }
      if(!ParameterSet.CSParameterFileName)
		  ParameterSet.CSParameterFileName = a;
	}
//****************************
	/* check CL parammeterfile parameter */
    if (strstr (argv[i], "-pCL") == argv[i])
	{
      a = &(argv[i][4]);
      if (!(*a))
	  {
		  /* query name in next parameter */
		  if ((i + 1) == argc)
			  return &ParameterSet;
		  if (argv[i + 1][0] == '-')
			  return &ParameterSet;
		  i++;
		  a = &(argv[i][0]);
	  }
      if (*a)
	  {
		  /* read parameter file */
		  //postpone reading the parfile until just before each ClientApplication opens
		  //readparameters (a);

      }
      if(!ParameterSet.CLParameterFileName)
		  ParameterSet.CLParameterFileName = a;
	}
//****************************
	/* check EH parammeterfile parameter */
    if (strstr (argv[i], "-pEH") == argv[i])
	{
      a = &(argv[i][4]);
      if (!(*a))
	  {
		  /* query name in next parameter */
		  if ((i + 1) == argc)
			  return &ParameterSet;
		  if (argv[i + 1][0] == '-')
			  return &ParameterSet;
		  i++;
		  a = &(argv[i][0]);
	  }
      if (*a)
	  {
		  /* read parameter file */
		  //postpone reading the parfile until just before each ClientApplication opens
		  //readparameters (a);

      }
      if(!ParameterSet.EHParameterFileName)
		  ParameterSet.EHParameterFileName = a;
	}
//****************************
	/* check HT parammeterfile parameter */
    if (strstr (argv[i], "-pHT") == argv[i])
	{
      a = &(argv[i][4]);
      if (!(*a))
	  {
		  /* query name in next parameter */
		  if ((i + 1) == argc)
			  return &ParameterSet;
		  if (argv[i + 1][0] == '-')
			  return &ParameterSet;
		  i++;
		  a = &(argv[i][0]);
	  }
      if (*a)
	  {
		  /* read parameter file */
		  //postpone reading the parfile until just before each ClientApplication opens
		  //readparameters (a);

      }
      if(!ParameterSet.HTParameterFileName)
		  ParameterSet.HTParameterFileName = a;
	}


/////////////////////////////////////////////////////////// end parfiles section
/*  //check query parameter 
    if (strstr (argv[i], "-q") == argv[i]) {
      ParameterSet.Query = &(argv[i][2]);
      if (*ParameterSet.Query)
	continue;
     // query name in next parameter 
      ParameterSet.Query = 0;
      if ((i + 1) == argc) {
	return &ParameterSet;
      }
      if (argv[i + 1][0] == '-') {
	return &ParameterSet;
      }
      i++;
      ParameterSet.Query = &(argv[i][0]);
    }*/

   /* check thresh parameter */
    if (strstr (argv[i], "-t") == argv[i]) {
      a = &(argv[i][2]);
      if (!*a) {
       /* noofmatches name in next parameter */
	a = 0;
	if ((i + 1) == argc) {
	  return &ParameterSet;
	}
	if (argv[i + 1][0] == '-') {
	  return &ParameterSet;
	}
	i++;
	a = &(argv[i][0]);
      }
      ParameterSet.Thresh = atof (a);
    }
  }

 /* if parameters are not set, get them from parameter file */
 /*   if (ParameterSet.ParameterFileName) {
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
  }*/

  return &ParameterSet;
}


//---------------------------------------------------------------------------
int select_application (TXMApplicationParameterSet * ParameterSet)
{
  if (!ParameterSet)
    return 1;

  //if (!ParameterSet->Application)
  //  return 1;

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
  if (!ParameterSet->Application)
  {
	ParameterSet->Application=new char[8];
	strcpy(ParameterSet->Application,"Default");
  }
  if (!strcmp (ParameterSet->Application, "PrintApplications")) {
#ifdef USEDT
    printf ("DummyTypeServer\n");
    printf ("DummyTypeClient\n");
#endif
#ifdef USEDTDS
    printf ("DSDummyTypeServer\n");
    printf ("DSDummyTypeClient\n");
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

    return 0;
  }

  //speed things up - put the servers (run individually) at the front


  #ifdef USECL
 /* color layout database extraction */
  if (!strcmp (ParameterSet->Application, "ColorLayoutServer"))
  {
	  resetpara();
	  readparameters(ParameterSet->CLParameterFileName);
	  if(!ParameterSet->Bitstream)
		  ParameterSet->Bitstream= getpara ("Bitstream", (char *) NULL);


    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::ColorLayoutServer theServer;

      theServer.Open ();

#ifdef TIMING_TEST
  time_t start_time;
  time(&start_time);
#endif //TIMING_TEST

      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);

#ifdef TIMING_TEST
  cout<<"CL extraction: "<<time(NULL)-start_time<<endl;
#endif //TIMING_TEST

      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }
  #endif //USECL
  #ifdef USECT
 /* color structure database extraction */
  if (!strcmp (ParameterSet->Application, "ColorStructureServer"))
  {
	  readparameters(ParameterSet->CSParameterFileName);
	  if(!ParameterSet->Bitstream)
		  ParameterSet->Bitstream= getpara ("Bitstream", (char *) NULL);
	  if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
		  return 1;
	  else
	  {
		  XM::ColorStructureServer theServer;
		  theServer.Open ();

#ifdef TIMING_TEST
  time_t start_time;
  time(&start_time);
#endif //TIMING_TEST

      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);

#ifdef TIMING_TEST
  cout<<"CS extraction: "<<time(NULL)-start_time<<endl;
#endif //TIMING_TEST

		  theServer.Stop ();
		  theServer.Close ();
		  return 0;
	  }
  }
  #endif //USECT
  #ifdef USEDC
 /* dominant color database extraction */
  if (!strcmp (ParameterSet->Application, "DominantColorServer")) {
	  readparameters(ParameterSet->DCParameterFileName);
	  if(!ParameterSet->Bitstream)
		  ParameterSet->Bitstream= getpara ("Bitstream", (char *) NULL);
	  if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::DominantColorServer theServer;

      theServer.Open ();

#ifdef TIMING_TEST
  time_t start_time;
  time(&start_time);
#endif //TIMING_TEST

      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);

#ifdef TIMING_TEST
  cout<<"DC extraction: "<<time(NULL)-start_time<<endl;
#endif //TIMING_TEST

      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }
  #endif //USEDC
  #ifdef USESCC
 /* scalable color database extraction */
  if (!strcmp (ParameterSet->Application, "ScalableColorServer")) {
	  readparameters(ParameterSet->SCParameterFileName);
	  if(!ParameterSet->Bitstream)
		  ParameterSet->Bitstream= getpara ("Bitstream", (char *) NULL);    
	  if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::ScalableColorServer theServer;

      theServer.Open ();

#ifdef TIMING_TEST
  time_t start_time;
  time(&start_time);
#endif //TIMING_TEST

      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);

#ifdef TIMING_TEST
  cout<<"SC extraction: "<<time(NULL)-start_time<<endl;
#endif //TIMING_TEST

      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }
  #endif //USESCC
  #ifdef USEHT
 /* homogeneous texture database extraction */
  if (!strcmp (ParameterSet->Application, "HomogeneousTextureServer")) {
	  readparameters(ParameterSet->HTParameterFileName);
	  if(!ParameterSet->Bitstream)
		  ParameterSet->Bitstream= getpara ("Bitstream", (char *) NULL);
	  if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::HomogeneousTextureServer theServer;

      theServer.Open ();

#ifdef TIMING_TEST
  time_t start_time;
  time(&start_time);
#endif //TIMING_TEST

      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);

#ifdef TIMING_TEST
  cout<<"HT extraction: "<<time(NULL)-start_time<<endl;
#endif //TIMING_TEST

      theServer.Stop ();
      theServer.Close ();
      return 0;
    }
	
  }
  #endif //USEHT
  #ifdef USEEH
 /* edge histogram database extraction */
  if (!strcmp (ParameterSet->Application, "EdgeHistogramServer")){
	readparameters(ParameterSet->EHParameterFileName);
	if(!ParameterSet->Bitstream)
		  ParameterSet->Bitstream= getpara ("Bitstream", (char *) NULL);
    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else {
      XM::EdgeHistogramServer theServer;

      theServer.Open ();

#ifdef TIMING_TEST
  time_t start_time;
  time(&start_time);
#endif //TIMING_TEST

      theServer.Start (ParameterSet->ListFile, ParameterSet->Bitstream);

#ifdef TIMING_TEST
  cout<<"EH extraction: "<<time(NULL)-start_time<<endl;
#endif //TIMING_TEST

      theServer.Stop ();
      theServer.Close ();

      return 0;
    }
  }
  #endif //USEEH

//set up data structures for boundary detection
  	int n,i,descIndex,ordIndex,numOfElements,DBSIZE,numCols=(6*3 + 1);
	double **distmat,**distptr=0;
	distmat = new double*[numCols];
	XM::MediaDB originalDB;
	if (!ParameterSet->ListFile) return 2;
	originalDB.CreateDatabase(ParameterSet->ListFile);

	FILE *imFile;
	std::vector<DATEORD> dateOrdVec;
	std::vector<std::string> inlist;
	std::vector<std::string> outlist;
	struct tm time1,time2;
	int n1,n2,n3,n4,n5,n6;
	//load time information and sort originalDB into correct sequence
	DBSIZE=originalDB.GetNoOfElements();
	for(i=0; i<DBSIZE; i++)
    {
        
		//get the exif info
		inlist.push_back(originalDB.GotoEntry(i)->GetFilename());
		imFile = fopen(inlist.back().c_str(),"rb");
		if(!imFile)
		{
			cerr<<"Could not open image file "<<inlist.back().c_str()<<endl;
			return 0;
		}
		CMyExif imExif;
		DATEORD tempDate;
		imExif.DecodeExif(imFile);
		fclose(imFile);
		

		//if(imExif.m_exifinfo->IsExif)
		if(strlen(imExif.m_exifinfo->DateTime)>10) //can have date correctly set but IsExif==0!!
		{
			for(n=0; n<20; n++)
				tempDate.date[n] = imExif.m_exifinfo->DateTime[n];
			//fix leading zeros!!!
			sscanf(tempDate.date,"%d:%d:%d %d:%d:%d",&n1,&n2,&n3,&n4,&n5,&n6);
			sprintf(tempDate.date,"%.4d:%.2d:%.2d %.2d:%.2d:%.2d",n1,n2,n3,n4,n5,n6);
			//just testing!
			//if(tempDate.date[16]!=':')
			//	cout<<" "<<tempDate.date<<endl;
		}
		else
		{
			for(n=0; n<20; n++)
				tempDate.date[n] = 0; // this should make it 'less than' anything
			//display a warning/error dialog
			std::string errString = "Error: missing EXIF data for ";
			errString += originalDB.GotoEntry(i)->GetFilename();
			errString += "\n";
			cerr<<errString.c_str();
		}
		
		tempDate.index = i;
		dateOrdVec.push_back(tempDate);
	}

	//sort by EXIF date: "stable" version preserves ordering of equivalent ones
	std::stable_sort( dateOrdVec.begin(), dateOrdVec.end());

	// impose this order on database
	for (n=0; n<DBSIZE; n++)
		outlist.push_back(inlist[dateOrdVec[n].index]);

	distmat[18] = new double[DBSIZE-1];
	//calc time differences
	FILE *timeDistFile; //debug parts, make sure the distances pop out ok)
	timeDistFile = fopen("T_1.txt","w");

//debug: this tests the changeover in Spring 2005 (Europe)
//NB current setup only tested for Win32 / MSVC++ 6.0 compiler
//assumed Linux mktime function is the same implementation!!
/*	time_t dbtemp1, dbtemp2;
	convert_time("2005:03:26 23:00:00",time1);
	dbtemp1 =mktime(&time1);
	convert_time("2005:03:27 23:00:00",time2);
	dbtemp2 =mktime(&time2);
*/
	convert_time(dateOrdVec[0].date,time1);
	for(i=1; i<DBSIZE; i++)
	{
		convert_time(dateOrdVec[i].date,time2);
		distmat[18][i-1]=TMatching(time1,time2);
		fprintf(timeDistFile,"%f\n", distmat[18][i-1]);
		time1=time2;
	}

	fclose(timeDistFile);

#ifdef USECL
 /* color layout query retrieval */
//  if (!strcmp (ParameterSet->Application, "ColorLayoutClient"))
  {
	resetpara();
	if(readparameters(ParameterSet->CLParameterFileName)>0)
		return 0;
	ParameterSet->Bitstream= getpara ("Bitstream", (char *) NULL);

    XM::ColorLayoutClient theClient(originalDB);

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 2;
    else {
      theClient.Open (ParameterSet->Bitstream);
      //theClient.Start (ParameterSet->Query, ParameterSet->NoOfMatches);
	  numOfElements=theClient.Start (dateOrdVec, &distptr);
      theClient.Stop ();
      theClient.Close ();

      //return 0;
    }
	if(numOfElements!=DBSIZE)
	{
		DBSIZE = (numOfElements<DBSIZE) ? numOfElements : DBSIZE;
		cout<<"Mismatch in size of CL database.  Using minimum size: "<<DBSIZE<<endl;

	}

	distmat[2*3+0] = distptr[0];
	distmat[2*3+1] = distptr[1];
	distmat[2*3+2] = distptr[2];
	//free just the memory we have just copied (array index structure not the underlying data)
	delete [] distptr;
  }
#endif


#ifdef USECT
 /* color structure query retrieval */
  //if (!strcmp (ParameterSet->Application, "ColorStructureClient"))
  {
	resetpara();
	if(readparameters(ParameterSet->CSParameterFileName)>0)
		return 0;
	ParameterSet->Bitstream= getpara ("Bitstream", (char *) NULL);

    XM::ColorStructureClient theClient(originalDB);

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 2;
    else 
	{
      theClient.Open (ParameterSet->Bitstream);
	  numOfElements=theClient.Start (dateOrdVec, &distptr);
	  theClient.Stop ();
	  theClient.Close ();

      //return 0;
    }
	if(numOfElements!=DBSIZE)
	{
		DBSIZE = (numOfElements<DBSIZE) ? numOfElements : DBSIZE;
		cout<<"Mismatch in size of CS database.  Using minimum size: "<<DBSIZE<<endl;

	}
	distmat[3*3+0] = distptr[0];
	distmat[3*3+1] = distptr[1];
	distmat[3*3+2] = distptr[2];
	delete [] distptr;
  }
#endif


#ifdef USEDC
 /* dominant color query retrieval */
//  if (!strcmp (ParameterSet->Application, "DominantColorClient"))
  {
	resetpara();
	if(readparameters(ParameterSet->DCParameterFileName)>0)
		return 0;
	ParameterSet->Bitstream= getpara ("Bitstream", (char *) NULL);

    XM::DominantColorClient theClient(originalDB);

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 2;
    else {
      theClient.Open (ParameterSet->Bitstream);
      numOfElements=theClient.Start (dateOrdVec, &distptr);
      theClient.Stop ();
      theClient.Close ();

      //return 0;
    }
	if(numOfElements!=DBSIZE)
	{
		DBSIZE = (numOfElements<DBSIZE) ? numOfElements : DBSIZE;
		cout<<"Mismatch in size of DC database.  Using minimum size: "<<DBSIZE<<endl;

	}
	distmat[0] = distptr[0];
	distmat[1] = distptr[1];
	distmat[2] = distptr[2];
	delete [] distptr;
  }
#endif



#ifdef USESCC
 /* scalable color query retrieval */
  //if (!strcmp (ParameterSet->Application, "ScalableColorClient"))
  {
	resetpara();
	if(readparameters(ParameterSet->SCParameterFileName)>0)
		return 0;
	ParameterSet->Bitstream= getpara ("Bitstream", (char *) NULL);

    XM::ScalableColorClient theClient(originalDB);

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 2;
    else {
      theClient.Open (ParameterSet->Bitstream);
      numOfElements=theClient.Start (dateOrdVec, &distptr);
      theClient.Stop ();
      theClient.Close ();

      //return 0;
    }
	if(numOfElements!=DBSIZE)
	{
		DBSIZE = (numOfElements<DBSIZE) ? numOfElements : DBSIZE;
		cout<<"Mismatch in size of SC database.  Using minimum size: "<<DBSIZE<<endl;

	}
	distmat[3+0] = distptr[0];
	distmat[3+1] = distptr[1];
	distmat[3+2] = distptr[2];
	delete [] distptr;
  }
#endif



#ifdef USEHT
 /* homogeneous texture query retrieval */
//  if (!strcmp (ParameterSet->Application, "HomogeneousTextureClient"))
	{
	  resetpara();
	  if(readparameters(ParameterSet->HTParameterFileName)>0)
		  return 0;
	  ParameterSet->Bitstream= getpara ("Bitstream", (char *) NULL);

      XM::HomogeneousTextureClient theClient(originalDB);

      if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
        return 1;
      else {
      theClient.Open (ParameterSet->Bitstream);
      numOfElements=theClient.Start(dateOrdVec, &distptr);
      theClient.Stop ();
      theClient.Close ();

      //return 0;
    }
	if(numOfElements!=DBSIZE)
	{
		DBSIZE = (numOfElements<DBSIZE) ? numOfElements : DBSIZE;
		cout<<"Mismatch in size of HT database.  Using minimum size: "<<DBSIZE<<endl;

	}
	distmat[4*3+0] = distptr[0];
	distmat[4*3+1] = distptr[1];
	distmat[4*3+2] = distptr[2];
	delete [] distptr;
	}
#endif

#ifdef USEEH
 /* edge histogram query retrieval */
  //if (!strcmp (ParameterSet->Application, "EdgeHistogramClient"))
  {
	resetpara();
	if(readparameters(ParameterSet->EHParameterFileName)>0)
		return 0;
	ParameterSet->Bitstream= getpara ("Bitstream", (char *) NULL);

    XM::EdgeHistogramClient theClient(originalDB);

    if (!(ParameterSet->ListFile && ParameterSet->Bitstream))
      return 1;
    else
	{
      theClient.Open (ParameterSet->Bitstream);
      numOfElements=theClient.Start (dateOrdVec, &distptr);
      theClient.Stop ();
      theClient.Close ();

      //return 0;
    }
	if(numOfElements!=DBSIZE)
	{
		DBSIZE = (numOfElements<DBSIZE) ? numOfElements : DBSIZE;
		cout<<"Mismatch in size of EH database.  Using minimum size: "<<DBSIZE<<endl;

	}
	distmat[5*3+0] = distptr[0];
	distmat[5*3+1] = distptr[1];
	distmat[5*3+2] = distptr[2];
	delete [] distptr;
  }
#endif

//suggested coefficients derived from CE data
  double globalmean[19] = {11125.269, 13196.142, 14010.391, 466.910, 540.533, 572.207, 35.611, 41.502, 44.165, 9.461, 10.938, 11.629, 3.294, 3.775, 4.035, 10.171, 11.441, 12.073, 4.325};
  double globalstddev[19] = {8546.776, 8913.440, 9140.741, 221.610, 215.327, 211.885, 20.446, 20.718, 21.026, 5.131, 5.009, 5.068, 2.744, 2.937, 2.933, 4.213, 4.065, 3.916, 2.504};
// normalise everything !!!
  for(descIndex=0; descIndex<6; descIndex++)
	  for(ordIndex=0; ordIndex<3; ordIndex++)
		  //normalise(distmat[descIndex*3+ordIndex],DBSIZE-1-ordIndex);
		  normalise(distmat[descIndex*3+ordIndex], globalmean[descIndex*3+ordIndex], globalstddev[descIndex*3+ordIndex], DBSIZE-1-ordIndex);

  normalise(distmat[18],globalmean[18], globalstddev[18], DBSIZE-1);

  //do the detection!!
  bool *bounds=new bool[DBSIZE-1];


  labelBoundaries(distmat,DBSIZE,bounds,ParameterSet->Thresh);
  saveClust(ParameterSet->ClusterFile,bounds, outlist);





  //free the distmat !!!! (the actual data allocated by clients)
  for(i=0; i<7; i++)
	  delete [] distmat[i*3];

  //other tidying ...
  originalDB.FreeDatabase();

  return 0;
}

//---------------------------------------------------------------------------
int printhelp (TXMApplicationParameterSet * ParameterSet, int type_of_messages)
{
  if (!ParameterSet)
    return 1;
  
  char empty[1] = "";
  
  if (type_of_messages == 0) return 0;
  if (type_of_messages == 1) {

    if (!ParameterSet->Application)
      ParameterSet->Application = empty;
    if (!ParameterSet->ListFile)
      ParameterSet->ListFile = empty;
    if (!ParameterSet->Bitstream)
      ParameterSet->Bitstream = empty;
    if (!ParameterSet->LogRedirection)
      ParameterSet->LogRedirection = empty;
    if (!ParameterSet->ClusterFile)
      ParameterSet->ClusterFile = empty;

    fprintf (stderr, "Error: Wrong parameter list\n"
	     "call XM with:\n"
	     "XMMain.exe -p[DC|CS|SC|CL|EH|HT]Parameterfile -aApplication -lListfile -bBitstream "
	     "-rLogRedirection -oClusterFile\n\n"
	     "Parameterfile    = parameter file for particular server application\n"
	     "Application      = selected XM-application (%s)\n"
	     "Listfile         = file with list of filenames of database"
	     "(%s)\n"
	     "Bitstream        = MPEG-7 bitstream (%s)\n"
	     "LogRedirection   = file where all the log messages are going to "
	     "(%s)\n",
		 ParameterSet->Application,
	     ParameterSet->ListFile, ParameterSet->Bitstream,
	     ParameterSet->LogRedirection);
  }
  else if(type_of_messages > 1) //clustering/client error
  {

    if (!ParameterSet->ListFile)
      ParameterSet->ListFile = empty;
    if (!ParameterSet->LogRedirection)
      ParameterSet->LogRedirection = empty;
    if (!ParameterSet->ClusterFile)
      ParameterSet->ClusterFile = empty;

	      fprintf (stderr, "Error: Wrong parameter list\n"
	     "call XM with:\n"
	     "XMMain.exe -p[DC|CS|SC|CL|EH|HT]ParameterfileXYZ -lListfile "
	     "-tThresh -rLogRedirection -oClusterFile\n\n"
	     "ParameterfileXYZ = parameter files for various descriptors\n"
	     "Listfile         = file with list of filenames of database"
	     "(%s)\n"
	     "Thresh           = cluster boundary threshold (%f)\n"
	     "LogRedirection   = file where all the log messages are going to "
	     "(%s)\n"
	     "ClusterFile      = file where all the results are going to "
	     "(%s)\n",
	     ParameterSet->ListFile,
		 ParameterSet->Thresh,
	     ParameterSet->LogRedirection, ParameterSet->ClusterFile);
  }
  else {
    fprintf (stderr, "Error : Application \"%s\" unknown\n",
      ParameterSet->Application);
  }

  return 0;
}

void normalise(double* vec, double mu, double sigma, int length)
{
	int i;

	//subtract mean and get s.d.
	for(i=0; i<length; i++)
	{
		vec[i]-=mu;
		vec[i]/=sigma;
	}
	

}

double project(double* vec1, double* vec2, int length)
{
	double val=0;
	for(int i=0; i<length; i++)
	{
		val+=(vec1[i]*vec2[i]);
		//cout<<vec1[i]<<" ";
	}
	//cout<<endl;
	return val;
}

//for windows, need to always check if there's an extra spanner, sorry, CR character. ///////////////////
void stripCR(char *mystring)
{
	for(int i=0; i<strlen(mystring); i++)
		if (mystring[i]==13)
			mystring[i]='\0';
}

// Save the cluster configuration info to file //////////////////////////////////////////////////
void saveClust(char* fname, bool* bounds, std::vector<std::string>& imlist)
{
	
	std::string tempstr;
	if(fname)
	{
		/////////////////////////////////output to file
		ofstream ccdFile;
		//file open
		ccdFile.open(fname,ios::out|ios::binary);
		if (ccdFile.fail())
		{
			cerr<<"Failed to open "<<fname<<endl;
			exit(1);
		}
		
		int loc;
		for(int i=0; i<imlist.size(); i++)
		{
			//chop off the path - print filename only:
			loc=imlist[i].find_last_of("//\\");
			tempstr=imlist[i].substr(loc+1);
			//print to file followed by boundary, if appropriate
			ccdFile<<tempstr.c_str()<<endl;
			if (i<imlist.size()-1)
				if(bounds[i])
					ccdFile<<"#boundary"<<endl;
		}
		ccdFile.close();
	}
	else
	{
		///////////////////////////////////output to stdout
		std::string tempstr;
		int loc;
		for(int i=0; i<imlist.size(); i++)
		{
			//chop off the path - print filename only:
			loc=imlist[i].find_last_of("//\\");
			tempstr=imlist[i].substr(loc+1);
			//print to file followed by boundary, if appropriate
			cout<<tempstr.c_str()<<endl;
			if (i<imlist.size()-1)
				if(bounds[i])
					cout<<"#boundary"<<endl;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
bool operator <(const DATEORD& a, const DATEORD& b)
{
	bool ans=true;
	for ( unsigned int i=0; i<20; i++ )
	{
		//compare each char - equal is ok (includes . places)
		if(a.date[i]>b.date[i])  //no checking to make sure date is valid !!
		{
			ans=false;
			return ans;
		}
		else if (a.date[i]<b.date[i])
		{
			ans=true;
			return ans;
		}
		//else equal => carry on checking
	}
	return ans;
}

double TMatching(struct tm &t1, struct tm &t2)
{

	double ts,dist = 0.0;
	ts = difftime(mktime(&t2),mktime(&t1));
	if(ts!= 0)
		dist = log((0.00001 + ts/(60.0*60*24)))-log(0.00001);

	return dist;
}

void convert_time(char datestring[20], struct tm &outtime)
{
	int n1,n2,n3,n4,n5,n6;
	sscanf(datestring,"%d:%d:%d %d:%d:%d",&n1,&n2,&n3,&n4,&n5,&n6);
	outtime.tm_year=n1-1900;
	outtime.tm_mon=n2-1;
	outtime.tm_mday=n3;
	outtime.tm_hour=n4;
	outtime.tm_min=n5;
	outtime.tm_sec=n6;
	//Flag Daylight Saving off
	//we will assume the camera is never adjusted for DST
	//therefore image timestamps are always the same
	outtime.tm_isdst=0;
	//NB, mktime will correct the tm structure to reflect the right locale
	//but the output value in seconds is universal, so difference calculation is reliable
}

void labelBoundaries(double** distmat,int DBSIZE,bool* bounds, double thresh)
{
	int descIndex,j,A=12;
	double feats[77];
	double measure;
	//double discrimVec[77]={0.031465, 0.116695, 0.005151, 0.213670, 0.193581, 0.006212, 0.033837, -0.132235, 0.224712, -0.258615, -0.005964, -0.011410, 0.011964, -0.056998, 0.068802, -0.072162, 0.032298, -0.012397, -0.022014, 0.041715, 0.095388, -0.017759, -0.001083, 0.068792, 0.571100, -0.081147, -0.065341, 0.064486, -0.157878, 0.409529, 0.101840, 0.089256, -0.153045, -0.139014, -0.186650, 0.010912, 0.431626, -0.209402, 0.118642, -0.347412, -0.081058, 0.356640, 0.264178, -0.246831, -0.281693, 0.225525, 0.263879, 0.100120, 0.414128, -0.203103, 0.008984, 0.019075, 0.078652, -0.192338, -0.092828, -0.049419, -0.001251, 0.290865, 0.108195, -0.025733, 0.260717, -0.317638, -0.154705, -0.338826, -0.034116, 0.033093, 0.353289, 0.035592, 0.126082, -0.064547, 0.253016, 0.267234, 2.996441, -0.314819, -0.306814, 0.032479, -0.120907};
	double discrimVec[77]={0.058272, 0.197563, -0.042537, 0.271843, 0.008482, -0.024884, 0.171798, 0.095796, 0.278498, -0.195524, 0.032404, -0.119948, 0.259809, -0.007701, 0.111678, -0.183539, -0.025873, -0.010745, -0.078830, -0.261807, 0.007185, 0.120285, 0.180834, 0.019613, 0.454560, -0.098585, 0.054315, -0.064018, -0.053874, 0.466156, -0.008623, -0.051991, -0.364750, -0.076681, -0.232668, 0.047693, 0.266095, -0.327915, 0.059406, -0.115284, -0.141851, 0.382768, 0.218967, -0.065169, -0.187191, -0.056664, 0.266485, 0.184106, -0.071825, -0.137026, -0.008907, 0.010236, -0.072533, 0.056673, 0.265327, -0.049612, -0.061132, 0.014758, 0.016739, 0.028752, 0.488964, -0.310819, -0.164232, -0.353360, 0.095097, -0.235120, 0.218648, 0.115732, 0.143895, 0.117823, 0.202895, 0.143563, 2.895239, -0.374811, -0.291065, -0.003533, -0.078601};
	//start boundary detection
	//int j;
	for(j=0; j<DBSIZE-1; j++)
	{
		//populate feature vector
		for(descIndex=0; descIndex<6; descIndex++)
		{
			feats[descIndex*A]  =distmat[descIndex*3][j];

			//one missing at the start
			if(j>0)
			{
				feats[descIndex*A+3]=distmat[descIndex*3][j-1];
				feats[descIndex*A+5]=distmat[descIndex*3+1][j-1];
				// ... AND at the end
				if(j<DBSIZE-2)
					feats[descIndex*A+9]=distmat[descIndex*3+2][j-1];
				else
					feats[descIndex*A+9]=0;				
			}
			else
			{
				feats[descIndex*A+3]=0;
				feats[descIndex*A+5]=0;
				feats[descIndex*A+9]=0;
			}

			//one missing off the end
			if(j<DBSIZE-2)
			{
				feats[descIndex*A+1]=distmat[descIndex*3][j+1];
				feats[descIndex*A+6]=distmat[descIndex*3+1][j];
			}
			else
			{
				feats[descIndex*A+1]=0;
				feats[descIndex*A+6]=0;
			}

			
			//two missing off the start
			if(j>1)
			{
				feats[descIndex*A+4]=distmat[descIndex*3][j-2];
				feats[descIndex*A+7]=distmat[descIndex*3+1][j-2];
				feats[descIndex*A+10]=distmat[descIndex*3+2][j-2];
			}
			else
			{
				feats[descIndex*A+4]=0;
				feats[descIndex*A+7]=0;
				feats[descIndex*A+10]=0;
			}


			//two missing off the end
			if(j<DBSIZE-3)
			{
				feats[descIndex*A+2]=distmat[descIndex*3][j+2];
				feats[descIndex*A+8]=distmat[descIndex*3+1][j+1];
				feats[descIndex*A+11]=distmat[descIndex*3+2][j];
			}
			else
			{
				feats[descIndex*A+2]=0;
				feats[descIndex*A+8]=0;
				feats[descIndex*A+11]=0;
			}	
			
		}

		feats[72]=distmat[18][j];
		if(j>0)			feats[73]=distmat[18][j-1]; else feats[73]=0;
		if(j<DBSIZE-2)	feats[74]=distmat[18][j+1]; else feats[74]=0;
		if(j<DBSIZE-3)	feats[75]=distmat[18][j+2]; else feats[75]=0;
		if(j>1)			feats[76]=distmat[18][j-2]; else feats[76]=0;

		measure = project(feats,discrimVec,77);

		if (measure>=thresh) 
		{
			bounds[j] = true;
		}
		else bounds[j] = false;
	}

	//write the output to file
	//write_ccd(bounds)

	//load and compare GT
	//int hits=0, misses=0, falsePos=0, trueReject=0;
	//vector<string> imnames;
	//loadClust(argv[8],GTbounds, imnames, DBSIZE);
	
	//saveClust("dbGT.txt",GTbounds, imnames);
/*	
	for(j=0; j<DBSIZE-1; j++)
	{
			 if( GTbounds[j] &&  bounds[j] ) hits++;
		else if( GTbounds[j] && !bounds[j] ) misses++;
		else if( !GTbounds[j] && bounds[j] ) falsePos++;
		else								 trueReject++;
	}

	cout<<"Precision = "<<double(hits)/(hits+falsePos)<<" Recall = "<<double(hits)/(hits+misses)<<endl;
*/

//	delete [] bounds;
	return;

}
