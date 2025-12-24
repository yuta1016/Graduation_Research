///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
// Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden)
// 
// Modifications to this base code have been done to enable TimeStamp and
// FastAudio generation by
// Dulce Ponceleon, Jan H. Pieper (Almaden Research Center, IBM, USA) and 
// Gilad Cohen (IBM Research, Haifa ISRAEL)
// 
// dulce@almaden.ibm.com
// jhpieper@almaden.ibm.com
// giladc@il.ibm.com
//
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
// Applicable File Name:  SeqSummaryFastMovingStoryBoardSAppl.cpp
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef _VISUALC_
	#include <unistd.h>
#else
	#include <io.h>
#endif

#include "momusys.h"
#include "vopio.h"


#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"            // Naming Convention 
#include "DescriptionSchemes/DescriptionSchemes.h"     // Naming Convention 
#include "ExtractionUtilities/ExtractionUtilities.h"    // Naming Convention 
#include "CodingSchemes/CodingSchemes.h"          // Naming Convention 
#include "Encoders/Encoders.h"               // Naming Convention 
#include "Media/Media.h"                  // Naming Convention 


#define WRITEKEYFRAMES

using namespace XM;

const UUID SeqSummaryFastMovingStoryBoardServer::myID = UUID("");
const char *SeqSummaryFastMovingStoryBoardServer::myName = "SeqSummaryFastMovingStoryBoardSAppl";

//=============================================================================
SeqSummaryFastMovingStoryBoardServer::SeqSummaryFastMovingStoryBoardServer()
{

}

//----------------------------------------------------------------------------
SeqSummaryFastMovingStoryBoardServer::~SeqSummaryFastMovingStoryBoardServer()
{

}

//----------------------------------------------------------------------------
unsigned long SeqSummaryFastMovingStoryBoardServer::Open()
{
	return 0;
}

//----------------------------------------------------------------------------
/** SeqSummaryFastMovingStoryBoard::Start 
 *
 *  This methods extracts keyframes from a mpeg1 and mpeg2 movie and creates 
 *  timestamps corresponding to speedup version of the audio layer. (The 
 *	algorithm allows also to produce a slower version but for this integration
 *	we chose the fast one).
 *
 *	A FastMovingStoryBoard will be created as a .SMIL file synchronizing the 
 *	keyframes with the fast audio file. This file is playable with
 *	e.g. Realplayer and QuickTime (RealPlayer is a trademark of RealNetworks
 *	and QuickTime is a trademark of Apple Computer, Inc).
 *
 *	The authors could not find an open source demultiplexer from .mpg -> .wav.
 *  Therefore , it is	necessary to provide the audio of the movie as a .wav
 *	file as additional input.
 *	Hopefully this may be fixed in the next version. We encourage the readers
 *	to contact the authors if they can provide source code to generate WAV files
 *	from an MP2 file. 
 *
 *	The ListFile has to have the following content:
 *	<first.mpg>	<first_audio.wav>
 *	<second.mpg> <second_audio.wav>
 *	...
 *
 *	For example:
 *	cuts.mpg cutsaudio.wav
 *	test.mpg test.wav
 *
 *  @param *ListFile  name of the list file
 *  @param *unused    this parameter is not used at the moment - pass NULL
 */
unsigned long SeqSummaryFastMovingStoryBoardServer::Start(char *ListFile, char *unused)
{
	//image media
        MomVop *media; //sth switch to MultiMedia in future
	long FrameNo;
	int loopcnt = 0;

	//for writing key frames
	unsigned long is_key_frame;

	//Image data base 
	MediaDB RamDB;

	//extraction tool
	SeqSummaryFastMovingStoryBoardExtractionTool *theExtractionTool;
	SeqSummaryFastMovingStoryBoardExtractionInterfaceABC *theExtractionInterface;
	
	//coding schemes
  GenericDSCS *theCS;
 	GenericDSCSInterfaceABC *theCSInterface;

	//***************
	//* get interfaces
	//* create extraction tool
	//* get extraction interface
	theExtractionTool = new SeqSummaryFastMovingStoryBoardExtractionTool;
	theExtractionInterface=theExtractionTool->GetInterface();

	//get coding scheme interface
	theCS = new GenericDSCS;
	theCSInterface=theCS->GetInterface();

	//load filenames into database
	//note: only the .mpg files can be processed by the database
	//      the .wav files are just stored to query their filename
	RamDB.CreateDatabase(ListFile);

	//set predifined descriptor values

	while (1)
	{

	  int i;											//frame of sequence
	  char ppmimage[25];					//filename of single image
	  char path[MAXMEDIANAMELEN];

		if (loopcnt*2 >= RamDB.GetNoOfElements())
		  //sth and have one input for video and audio use multimedia
		{
			//no more .mpg in the database
			break;
		}

		//skip over .wav file
		RamDB.GotoEntry(loopcnt*2);

	  //decode .mpg
		if (!RamDB.DBDecodeMPEGVideo()) break;

		loopcnt++;
		
	  //*******************************
    // create directory for keyframes
    // compute directory name from media filename
	  strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
	  strncat(path,".dir",MAXMEDIANAMELEN-1-strlen(path));

	  // create directory
#ifndef _VISUALC_
	  mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
#else
	  //sth i guess _mkdir works with VC++
	  //fprintf(stderr,"Warning : mkdir not supported by VC++, Please check if directory\n");
		//little hack, but works at the moment
		char cmdstr[MAXMEDIANAMELEN+10];
		sprintf(cmdstr, "md %s", path);
		for (int ij=0; cmdstr[ij] != NULL; ij++)
		{
			if (cmdstr[ij] == '/') cmdstr[ij] = '\\';
		}
		system(cmdstr);
#endif

	  {
	    //reset description data
	    //create XML Document for Description
	    GenericDS theDocument = GenericDS::CreateDescriptionRoot();

	    //get descriptor interface
	    GenericDSInterfaceABC *theDocumentInterfaceABC=theDocument.GetInterface();

	    //connect descriptor to extraction tool
	    theExtractionInterface->SetDescriptorInterface(theDocumentInterfaceABC);

	    //connect descriptor to coding scheme
	    theCSInterface->SetDescriptionInterface(theDocumentInterfaceABC);

	    //connect output file to coding scheme
	    //sth const filename can be replaced by "unused" paranmeter
	    strncat(path,"/SeqSummaryFastMovingStoryBoardDS.ddl",MAXMEDIANAMELEN-1-strlen(path));
	    theCSInterface->SetEncoderFileName(path);

	    //allow file IO for prelimenary results
	    theExtractionInterface->SetSourceMediaFilename(RamDB.GetMediaFilename());

	    //set input wav file
	    theExtractionInterface->SetSourceWavFilename(RamDB.GotoEntry((loopcnt*2)-1)->GetFilename());


	    //*************************
			//loop frame of vector file
			//*************************
	    i=0;
	    while (1)
			{
	      //malloc media and load input data
	      sprintf(ppmimage,"m2v/%d.ppm",i);
	      media=RamDB.LoadNamedImage(ppmimage);

	      if (!media) break; // if no more images in sequence

	      //convert to apropriate color space
	      convertRGB_To_YUV(media,media);

	      //connect media to extraction tool
	      theExtractionInterface->SetSourceMedia(media);

	      //initialize values for the extraction before first frame
	      if (!i) theExtractionInterface->InitExtracting();
				{
		      //get frame number from image
					FrameNo=RamDB.GetFrameNoFromComment();
					if (FrameNo == -1) 
					{
						FrameNo=i;
					}
					theExtractionInterface->SetSourceMediaFrameNumber(FrameNo);
	      }

	      //extract descriptor form media
	      is_key_frame=theExtractionInterface->StartExtracting();

	      //free media
	      freevop(media);

	      
#ifdef WRITEKEYFRAMES 
				// write key frame to disk
				if (is_key_frame == 2)  //only write keyframes that are part of the DS
				{
					char keyfilename[16];

					//compute directory name from media filename
					strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
					strncat(path,".dir",MAXMEDIANAMELEN-1-strlen(path));

					//appent keyframe filename to path
					strncat(path,"/",MAXMEDIANAMELEN-1-strlen(path));
					sprintf(keyfilename,"key%05ld.jpg",FrameNo);
					strncat(path,keyfilename,MAXMEDIANAMELEN-1-strlen(path));

					//write keyframe
					RamDB.StoreNamedImage(path);
			  }
#endif //WRITEKEYFRAMES

	      //delete image
	      unlink(ppmimage);

	      i++;
	    }
			//*************************************
			//After all frames have been extracted:
			//close files
			//write Desciption Scheme
			//*************************************

			//postprocressing of the Extraction Tool
	    theExtractionInterface->PostExtraction();

	    //write descriptor data (pretty print xml document object model)
	    theCSInterface->StartEncode();
	  }
	}

	//that's it...
	theExtractionInterface->destroy();
	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryFastMovingStoryBoardServer::Stop()
{
	return 0;
}
//----------------------------------------------------------------------------
unsigned long SeqSummaryFastMovingStoryBoardServer::Close()
{
	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& SeqSummaryFastMovingStoryBoardServer::GetObjectID(void)
{ 
	return myID;
}

//----------------------------------------------------------------------------
const char *SeqSummaryFastMovingStoryBoardServer::GetName(void)
{ 
	return myName;
}



