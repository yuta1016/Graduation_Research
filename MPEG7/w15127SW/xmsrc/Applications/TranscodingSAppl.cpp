///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Peter Kuhn, Teruhiko Suzuki (Sony Corporation Japan)
//
// and modified by
//
// Anthony Vetro (Mitsubishi)
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
// Applicable File Name:  TranscodingSAppl.cpp
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
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif


#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */

#define STRLEN 1024

// #define WRITETRANSFRAMES

using namespace XM;

const UUID TranscodingServer::myID = UUID("");
const char *TranscodingServer::myName = "TranscodingSAppl";

//=============================================================================
TranscodingServer::TranscodingServer()
{
}

//----------------------------------------------------------------------------
TranscodingServer::~TranscodingServer()
{
}

//----------------------------------------------------------------------------
unsigned long TranscodingServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long TranscodingServer::Start(char *ListFile, char *unused)
{
        int is_difficulty, is_ddl, format, tch_start, tch_frames, i, tch_first;
        char outpath[STRLEN], ddlfile[STRLEN], mpgfile[STRLEN];
        int is_motion_intensity, is_shape;
        int compression_format, numvos, one_seg=0, k;
        char *intensity_filename;
	
	is_ddl=1;
        tch_first=1;
	/* image media*/
        MomVop *media, *prevmedia; //sth see MultiMedia class, should be modified in long term
	long FrameNo;

	/* for writing trans frames*/
	unsigned long is_trans_frame;

	/* Image data base */
	MediaDB RamDB;

	/* extraction tool*/
	TranscodingExtractionTool *theExtractionTool;
	TranscodingExtractionInterfaceABC
	  *theExtractionInterface;
	
	/* coding schemes*/
  	GenericDSCS *theCS;
 	GenericDSCSInterfaceABC *theCSInterface;

	/*****************/
	/* get interfaces*/
	/* create extraction tool*/
	/* get extraction interface*/
	theExtractionTool = new 
	  TranscodingExtractionTool;
	theExtractionInterface=theExtractionTool->GetInterface();

	/* get coding scheme interface*/
	theCS = new GenericDSCS;
	theCSInterface=theCS->GetInterface();

	RamDB.CreateDatabase(ListFile);

	/* set predifined descriptor values*/

	while (1) {

	  char ppmimage[25];    /* filename of single image*/
	  char path[MAXMEDIANAMELEN];

#ifdef WRITETRANSFRAMES
	  int scene=0;
#endif

          format = 3; // ppm
	  strcpy(path, theExtractionInterface->GetSimDir());
	  tch_start = theExtractionInterface->GetStartFrame();
	  tch_frames = theExtractionInterface->GetFrames();
	  compression_format = theExtractionInterface->GetCompressionFormat();
	  numvos = theExtractionInterface->GetNumVO();
	  intensity_filename = theExtractionInterface->GetMotionIntensityfilename();
	  fprintf(stderr,"path: %s tch_start: %d, tch_frames:%d \n", path, tch_start, tch_frames);
	 	  
	  if (compression_format == MPEG2) {
	  if (!RamDB.DBDecodeMPEGVideo(format, tch_start, tch_frames)){
		fprintf(stderr,"no (more) ppm decoding\n");
		// break;
		exit(1);
	  }	
	  } else if(compression_format == MPEG4) {
	    if (!RamDB.DBDecodeMPEG4Video(numvos, path, intensity_filename)){
		fprintf(stderr,"no (more) frames to decode\n");
		// break;
		exit(1);
	    }	
	  }

	  // avetro : var to indicate if seg boundaries are computed
	  // could also be set according to hints being used
	  if (compression_format == MPEG2) {
	    one_seg=0;
	  } 
	  else if(compression_format == MPEG4) {
	    one_seg=1;
	  }

	  /*********************************/
          /* create directory for sceneframes*/
          /* compute directory name from media filename*/
	  // strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
	  // strncat(path,".dir",MAXMEDIANAMELEN-1-strlen(path));

	  /* create directory*/
#ifndef _VISUALC_
	  mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
#else
	  fprintf(stderr,"Warning : mkdir not supported by VC++, Please "
		  "check if directory\n");
#endif

	  {
	    //sth new style possible like in DSDummyType (also long term)
	    /* reset description data*/
	    /* create XML Document for Description*/
	    GenericDS theDocument = GenericDS::CreateDescriptionRoot();
	    /* get descriptor interface*/
	    GenericDSInterfaceABC *theDocumentInterfaceABC=
	      theDocument.GetInterface();

	    /* connect descriptor to extraction tool*/
	    theExtractionInterface->
	      SetDSInterface(theDocumentInterfaceABC);

	    /* connect descriptor to coding scheme*/
	    theCSInterface->
	      SetDescriptionInterface(theDocumentInterfaceABC);

	    /* connect output file to coding scheme*/
	    strcpy(outpath,path);
	    strncat(path,"/TransDS.ddl",MAXMEDIANAMELEN-1-strlen(path));
	    strcpy(ddlfile,path);
	    fprintf(stderr,"ddlfile: %s\n",ddlfile); //sth done print to stdout
	    theCSInterface->SetEncoderFileName(ddlfile);
	 	    
	    /* allow file IO for prelimenary results*/
	    theExtractionInterface->
	      SetSourceMediaFilename(RamDB.GetMediaFilename());
// 	    theExtractionInterface->SetSourceMediaFilename("test.mpg");

	    /* reset descrition data*/
 	    // theDescriptionInterfaceABC->InitDescriptor();

	    /* loop frame of vector file*/
	    
	    i=tch_start;
	    while (1) {

	      /* malloc media and load input data*/

	      fprintf(stderr,"i: %d tch_start: %d, tch_frames:%d \n", i, tch_start, tch_frames);
	      
              if(compression_format == MPEG2) {
	      sprintf(ppmimage,"m2v/%d.ppm",i);
	      media=RamDB.LoadNamedImage(ppmimage);
              }
	      
	      if (media){
      	          if(tch_first==1){
	              prevmedia = initvop(media->width,media->height,_444_,
			  UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE);
                  }
		  
		  prevmedia->u_chan->data->u = media->u_chan->data->u;
		  prevmedia->v_chan->data->u = media->v_chan->data->u;
		  prevmedia->y_chan->data->u = media->y_chan->data->u;
						   			  	 		 
	          /* convert to apropriate color space*/
	          //convertRGB_To_YUV(media,media);

	          /* connect media to extraction tool*/		  
	          theExtractionInterface->SetSourceMedia(media, prevmedia);

	          /* initialize values for the extraction before first frame*/
	          if (tch_first==1) theExtractionInterface->InitExtracting();

	          /* get frame number from image*/
	          {	      
		    FrameNo=RamDB.GetFrameNoFromComment();
		    if (FrameNo == -1) FrameNo=i;
	 	    theExtractionInterface->SetSourceMediaFrameNumber(FrameNo);
	          }

	          /* extract descriptor form media*/
	          is_trans_frame=theExtractionInterface->StartExtracting();

	          /* free media*/
	          freevop(media); //sth with MultiMedia not needed anymore

	          /* delete image*/
	      	  unlink(ppmimage); //sth unlike not needed anymore
	      	  tch_first=0;
	      } else if (i > (tch_start+tch_frames+1))
	      {
	      	break;
	      }
	      else if(!media && one_seg) { 

		  // avetro: add case when there is no media
		  // but still want to set up a single seg boundary

		  theExtractionInterface->InitExtracting();
	          is_trans_frame=theExtractionInterface->StartExtracting();
		  i=tch_frames;
		  tch_first=0;
		  break;
	      }
	      i++;
	    }
	    theExtractionInterface->CloseSession(i); //sth I guess that should be PostExtracting
		    	    	    

	    //sth this section could become client application, please
	    //sth leave it here until I showed up with summarization client
	    
            if(compression_format == MPEG2) {
	    /* decode into Y U V in m2v directory */
            RamDB.CreateDatabase(ListFile); //sth not needed if unlike is not done, leave it for moving to client
	    if (!RamDB.DBDecodeMPEGVideo(0, tch_start, tch_frames), 0){
		fprintf(stderr,"YUV decoding failed\n");
		exit(1);
	    }
            }

            theExtractionInterface->GetMediaCodingParams();

	    is_difficulty = theExtractionInterface->GetUseDifficulty();		        	
	    
	    fprintf(stderr,"realpath: %s\n",outpath);
															    								    		     			    	    
            if(is_difficulty==1 && compression_format==MPEG2){
		sprintf(mpgfile,"%s/%s.mpg",outpath,
			theExtractionInterface->GetDifficultyParamfilename());
			
	        /* reencode MPEG with q=1 to extract stat.out.q1.bits */
		        RamDB.DBEncodeMPEGVideo(
				mpgfile,
				theExtractionInterface->GetDifficultyParamfilename(),
				0);																						
                /* calculate difficulty hint */
		theExtractionInterface->GetCalculateDifficulty();
									
	    }
	    else if(is_difficulty==1 && compression_format==MPEG4){

		RamDB.DBEncodeMPEG4Video(
	    		0,
	    		ddlfile,
	    		theExtractionInterface->GetSimDir(), /* src_dir */
	    		theExtractionInterface->GetSimDir(), /* sim_dir */
			theExtractionInterface->GetNumVO(),			
			theExtractionInterface->GetFrames(),
			0);
								
                /* calculate difficulty hint */
		theExtractionInterface->GetCalculateObjectDifficulty();
	    }

	    is_motion_intensity = theExtractionInterface->GetUseMotionIntensity();
	    if(is_motion_intensity==1 && compression_format==MPEG4)
		theExtractionInterface->GetCalculateMotionIntensity();

	    is_shape = theExtractionInterface->GetUseShape();
	    if(is_shape==1 && compression_format==MPEG4)
		theExtractionInterface->GetCalculateShape();


	    is_ddl = theExtractionInterface->GetUseDDL();
	    
	    printf("is_ddl:%d \n", is_ddl);
	    
            fprintf(stderr,"start postextracting ...\n");
	    for(k=0; k<numvos; k++)
	    theExtractionInterface->PostExtracting(tch_start, k);
	   				   
	    /* code descriptor data*/
	    theCSInterface->StartEncode();

            if(compression_format == MPEG2) {
	    /* reencode MPEG video using motion and difficulty hints */
	    //* PK debug
	    sprintf(mpgfile,"%s/%s.reenc.mpg",outpath,
			theExtractionInterface->GetReencodingParamfilename());			
	    		RamDB.DBEncodeMPEGVideo(
	    			mpgfile,
	    			theExtractionInterface->GetReencodingParamfilename(), 
				ddlfile);										
  			RamDB.DBCleanM2Vdir(); 
	    		
            } else if(compression_format == MPEG4) {
                /* re-encode MPEG4 video using motion, difficulty and shape hints */
                RamDB.DBEncodeMPEG4Video(
	    		1,
	    		ddlfile,
	    		theExtractionInterface->GetSimDir(), /* src_dir */
	    		theExtractionInterface->GetSimDir(), /* sim_dir */
			theExtractionInterface->GetNumVO(),			
			theExtractionInterface->GetFrames(),
			theExtractionInterface->GetBitRate());
            }
	  }
	}
		
        if(compression_format == MPEG2) {
	if(prevmedia)freevop(prevmedia);
	if(media)freevop(media);
        }
	
	theExtractionInterface->destroy();
	theCSInterface->destroy();
									
	return 0;
}


//----------------------------------------------------------------------------
unsigned long TranscodingServer::Stop()
{
	return 0;
}
//----------------------------------------------------------------------------
unsigned long TranscodingServer::Close()
{
	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& TranscodingServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TranscodingServer::GetName(void)
{ return myName; }


