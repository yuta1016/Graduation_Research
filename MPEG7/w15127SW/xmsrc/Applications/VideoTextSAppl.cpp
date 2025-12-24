///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Gang Wei and Nevenka	Dimitrova	- Philips Research.
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
// Applicable File Name:  VideoTextSAppl.cpp
//
#include <stdio.h>
#include <momusys.h>
#include <vopio.h>

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


//sth please remove spaces from *Name
const UUID VideoTextServer::myID = UUID("");
const char *VideoTextServer::myName = "VideoText DS Server";

//const int	VideoTextServer::STRING_MAXSIZE= 4096;		// maximum size of the strings


//=============================================================================
VideoTextServer::VideoTextServer()
{
}

//----------------------------------------------------------------------------
VideoTextServer::~VideoTextServer()
{
}

//----------------------------------------------------------------------------
unsigned long VideoTextServer::Open()
{
	/* This server doesn't handle pull */
	return 0;
}

//#define _DECODE_VIDEO_ON_THE_FLY
#define _USE_EXISTING_DECODED_FRAME
//----------------------------------------------------------------------------
unsigned long VideoTextServer::Start(char * ListFile, char * sXMLFileName)
{

	//* image media
        MomVop *media; //sth use MultiMedia in future

	//* Image data base 
	MediaDB RamDB;

	//* extraction tool
	VideoTextExtractionTool				*theExtractionTool;
	VideoTextExtractionInterfaceABC		*theExtractionInterface;
	
	//* coding schemes
  	GenericDSCS *theCS;
 	GenericDSCSInterfaceABC *theCSInterface;
	//* get coding scheme interface
	theCS = new GenericDSCS;
	theCSInterface=theCS->GetInterface();

	//****************
	//* get interfaces
	//* create extraction tool
	//* get extraction interface
	//* get extraction interface
	theExtractionTool = new VideoTextExtractionTool;
	theExtractionInterface=theExtractionTool->GetInterface();

	RamDB.CreateDatabase(ListFile);

	//* set predifined descriptor values

	while (true)
	{
		int i=0;              //* frame of sequence
		char ppmimage[25];    //* filename of single image
		char path[MAXMEDIANAMELEN];
//		RamDB.DBDecodeMPEGVideo();
#ifdef _DECODE_VIDEO_ON_THE_FLY //sth should be the normal way
		if (!RamDB.DBDecodeMPEGVideo()) 
			break;
#endif
		//* reset description data
		//* create XML Document for Description
		GenericDS theDocument = GenericDS::CreateDescriptionRoot();
		//* get descriptor interface
		GenericDSInterfaceABC *theDocumentInterfaceABC=
		  theDocument.GetInterface();

		//* connect descriptor to extraction tool
		theExtractionInterface->SetDescriptorInterface(theDocumentInterfaceABC);

		//* connect descriptor to coding scheme
		theCSInterface->SetDescriptionInterface(theDocumentInterfaceABC);

		// connect output file to coding scheme
		theCSInterface->SetEncoderFileName(sXMLFileName);

		//* initialize descriptor for extraction of first frame
		theExtractionInterface->InitExtracting();

		///* get first frame of sequence
		//* malloc media and load input data	    
		sprintf(ppmimage,"m2v/%d.ppm",i);
#ifndef _DECODE_VIDEO_ON_THE_FLY
		//sth are you sure that everybody uses drive f:
		//sth I am sure that it won´t work on Unix
		sprintf(ppmimage,"f:\\m2v\\process/%d.ppm",i);
#endif
		media=RamDB.LoadNamedImage(ppmimage);

		if (!media) 
			break; //* if images in sequence

		//* connect first media to extraction tool
		theExtractionInterface->SetSourceMedia(media);
		
		//* get frame number from image
		long FrameNo;
		FrameNo=RamDB.GetFrameNoFromComment();
		if (FrameNo == -1) 
			FrameNo=i-1;
		theExtractionInterface->SetSourceMediaFrameNumber(FrameNo);

		//* loop frame of vector file
		i=1;
		while (1) 
		{
			//* malloc media and load input data
			sprintf(ppmimage,"m2v/%d.ppm",i);
#ifndef _DECODE_VIDEO_ON_THE_FLY
			sprintf(ppmimage,"f:\\m2v\\process/%d.ppm",i);
#endif
			media=RamDB.LoadNamedImage(ppmimage);

			if (!media) 
				break; //* if no more images in sequence

			//* convert to apropriate color space
			convertRGB_To_YUV(media,media);

			//* connect next media to extraction tool
			theExtractionInterface->SetSourceMedia(media);

			//* get frame number from image
			long FrameNo;
			FrameNo=RamDB.GetFrameNoFromComment();
			if (FrameNo == -1) 
				FrameNo=i-1;
			theExtractionInterface->SetSourceMediaFrameNumber(FrameNo);

			//* extract descriptor form media
			theExtractionInterface->StartExtracting();

			//* free media
			freevop(media);
#ifdef _DECODE_VIDEO_ON_THE_FLY
			unlink(ppmimage);
#endif
			i++;
		}//while (1)

		theExtractionInterface->PostExtracting();

		//* code descriptor data
		theCSInterface->StartEncode();
		freevop(media);
#ifndef _DECODE_VIDEO_ON_THE_FLY
		break;	
#endif
/*		MomVop *VideotTextvop;
		VideotTextvop=theExtractionInterface->GetVideotText();

		if (VideotTextvop) {
			// compute directory name from media filename
			strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
			strncat(path,".dir",MAXMEDIANAMELEN-1-strlen(path));

			// appent keyframe filename to path
			strncat(path,"/VideotText.jpg",MAXMEDIANAMELEN-1-strlen(path));
			RamDB.StoreNamedVop(path,VideotTextvop);
		}//if (VideotTextvop)
*/	
	}//while (true)
	theExtractionInterface->destroy();
	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long VideoTextServer::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long VideoTextServer::Close()
{
	return 0;
}
	
//----------------------------------------------------------------------------
const UUID& VideoTextServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *VideoTextServer::GetName(void)
{ return myName; }


