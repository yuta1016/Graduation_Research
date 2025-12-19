///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jiuhuai Lu
// Tektronix, Inc.
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MediaQualitySAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>
//#include <sys/stat.h>
//#ifndef _VISUALC_
//#include <dirent.h>
//#endif

#include "Applications/ServerApplications.h"
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"
//#include "MediaQualitySAppl.h"

using namespace XM;

const UUID MediaQualityServer::myID = UUID("");
const char *MediaQualityServer::myName = "MediaQualityServer";

//=============================================================================
MediaQualityServer::MediaQualityServer()
{
}

//----------------------------------------------------------------------------
MediaQualityServer::~MediaQualityServer()
{
}

//# define MAXFILESMAXLENGTH 360000
//
//---------------------------------------------------------------------------
//  Scan the named directory for a sorted list of video frames 
//
//
//char *MediaQualityServer::ScanMediaDirectory (char *SeqName)
//{
//    struct dirent **namelist;
//	char *fh = "frame";
//	char fullpath[200];
//	char *filelist;
//	int n;
//
//	filelist = new char [MAXFILESMAXLENGTH];
//	*filelist = '\0';
//
//	fh = strrchr (SeqName, '/'); fh++;
//   n = scandir(SeqName, &namelist, 0, alphasort);
//	if (n<0) perror("scandir");
//	else {
//		while (n--) { 
//			if (strstr (namelist[n]->d_name, fh)) {
//				strcpy (fullpath, SeqName);
//				strcat (fullpath, "/");
//				strcat (fullpath, namelist[n]->d_name);
//				strcat (filelist, fullpath);
//				strcat (filelist, " ");
//			}
//		}
//	}
//	return (filelist);
//}

//----------------------------------------------------------------------------
// This takes two input parameters:
//   ListFile - This gives a text string giving the filename of the file
//              containing a sequence of filenames which in turn contain 
//              the media to encode.
//   Bitstream - This gives the file name of the binary output file.
//
// The contents of each file in ListFile is read in and the DSType
// created using the data. This is then written to the bitstream/DDL file. 


unsigned long MediaQualityServer::Start(char *ListFile, char *DDLFile)
{
  	char seqref_filename[MAXMEDIANAMELEN];
	char seqdecoded_filename[MAXMEDIANAMELEN];

  // Check that the file names are ok
  assert(ListFile  != NULL);
  assert(DDLFile != NULL);

  // -------------------- Initialisation of data ----------------------------
  
  // Media data base
  MediaDB RamDB;
  MediaDB *RamDB_REF=NULL; //sth only one input list
  MediaDB *RamDB_COMP=NULL;

  // Media
  //MultiMedia *theMedia;
  //MultiMediaInterfaceABC *theMediaInterface;
  
  // This sets up the dummytype description interface
  GenericDS theDescription = GenericDS::CreateDescriptionRoot();
  GenericDSInterfaceABC* theDescriptionInterface =
    theDescription.GetInterface();

  // This creates the extraction tool and extraction tool interface. Set
  // to null initially for safety.
  MediaQualityExtractionTool*         theExtractionTool               = NULL;
  MediaQualityExtractionInterfaceABC* theExtractionInterface          = NULL;
	
  // This sets up the coding scheme for this D/DType and the interface
  GenericDSCS*                     theCS                           = NULL;
  GenericDSCSInterfaceABC*         theCSInterface                  = NULL;

  // Ok, make new instatiations of these datatypes
  try{
    // Pass it null so that it creates a new description
    theExtractionTool = new MediaQualityExtractionTool(theDescriptionInterface);

    // Obtain the extraction interface from this extraction tool
    theExtractionInterface=theExtractionTool->GetInterface();

    // Now create the coding scheme
    theCS = new GenericDSCS(theDescriptionInterface);

    // Obtain the interface
    theCSInterface=theCS->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"MediaQualityServer::Start: Allocation error\n");
    return(1);
  }

  // All of the pointers should now be non-null so check them
  assert(theDescriptionInterface         != NULL);
  assert(theExtractionTool               != NULL);
  assert(theExtractionInterface          != NULL);
  assert(theCS                           != NULL);
  assert(theCSInterface                  != NULL);

  // To break sequence video file into smaller files containing single
  //  video frame  

  {
	//char *refFlist, *decFlist, *totFlist;

    sscanf (ListFile, "%s%s", seqdecoded_filename, seqref_filename);
    // sth work arround the structure

        // printf ("%s\n", seqdecoded_filename);//sth don´t print to stdout
        // printf ("%s\n", seqref_filename);//sth
		//decFlist = ScanMediaDirectory (seqdecoded_filename); 
	RamDB_REF = new MediaDB (seqdecoded_filename);
		//RamDB_COMP->CreateDatabase(decFlist);
	//RamDB_COMP->CreateDatabase(seqdecoded_filename);

		//refFlist = ScanMediaDirectory (seqref_filename);
	RamDB_COMP = new MediaDB (seqref_filename);
		//RamDB_REF->CreateDatabase(refFlist);
	//RamDB_REF->CreateDatabase(seqref_filename);
	
	//totFlist = (char *) malloc 
	//	((strlen(refFlist)+strlen(decFlist)+1)*sizeof(char));

	//strcat (totFlist, decFlist);
	//strcpy (totFlist, refFlist); 

	//nfiles = nfiles1 > nfiles2? nfiles2: nfiles1;
  	//RamDBCreateDatabase (totFlist);
  }

  // Creat RamDB by RamDB.CreateDatabase 
  {
  	int NDBaseEntries=RamDB_COMP->GetNoOfElements();	

	theExtractionInterface->InitExtracting(RamDB_COMP, RamDB_REF);

  	for(int ientry=0;ientry<NDBaseEntries;ientry++) {
		RamDB_REF->GotoEntry(ientry);
		MomVop *refimage = RamDB_REF->DBLoadImage();
		// sth compute 2nd filename from first
		RamDB_COMP->GotoEntry(ientry);
		MomVop *decimage = RamDB_COMP->DBLoadImage();

		theExtractionInterface->SetSourceMedia(decimage, refimage);

		// Extraction 
		if (theExtractionInterface->StartExtracting()==(unsigned long)-1) {
			fprintf (stderr, "Error returing from Ext::StartExtracting (%d)\n",
				ientry);
		}
 	} 

	// Integrate the values from extractions
	if (theExtractionInterface->PostExtracting()==(unsigned long)-1) {
		fprintf (stderr, "Error returing from Ext::PostExtracting\n");
	}
  }
	{
		char path[MAXMEDIANAMELEN], *pt;
		// Create directory for DDL files or for additional media results
		// sth use original directory structure
		// Compute directory name from media filename
		// use this only if required
		strcpy (path, seqdecoded_filename);
		pt = strrchr (path, '/'); 
		if (pt) *(pt+1) = '\0';
		else path[0]='\0';

		//strncat(path,"_XML",MAXMEDIANAMELEN-1-strlen(path));
		//path[MAXMEDIANAMELEN-1]=0;

#ifndef _VISUALC_
		//mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
#else
		//_mkdir(path);
#endif
		// Tell the dummytype about the output stream
		// strncat(path,"/",MAXMEDIANAMELEN-1-strlen(path));
		
		strncat(path,DDLFile,MAXMEDIANAMELEN-1-strlen(path));

		theCSInterface->SetEncoderFileName(path);

		// Write the D/DSType to binary
		/*int mesg = */theCSInterface->StartEncode();
	}

	// Tidy up all the memory allocations

	theExtractionInterface->destroy();
	theCSInterface->destroy();

	RamDB.FreeDatabase();
	// All done
	return(0);
}
  

//----------------------------------------------------------------------------
unsigned long MediaQualityServer::Stop()
{
  return(0);
}


//----------------------------------------------------------------------------
unsigned long MediaQualityServer::Open()
{
  return(0);
}


//----------------------------------------------------------------------------
unsigned long MediaQualityServer::Close()
{
  return(0);
}

	
//----------------------------------------------------------------------------
const UUID& MediaQualityServer::GetObjectID(void)
{ 
  return(myID);
}

//----------------------------------------------------------------------------
const char *MediaQualityServer::GetName(void)
{ 
  return(myName); 
}



