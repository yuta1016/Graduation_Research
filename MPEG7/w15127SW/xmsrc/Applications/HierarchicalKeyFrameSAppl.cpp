////////////////////////////////////////////////////////////////////////
//
// HierarchicalKeyFrameSAppl.cpp
//
// This software module was originally developed by
//
// Jim Errico, Sharp Laboratories of America, Camas, WA
// Peter van Beek, Sharp Laboratories of America, Camas, WA
// 
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938). 
// This software module is an implementation of a part of one or more 
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this 
// software module or modifications thereof for use in hardware or 
// software products claiming conformance to the MPEG-7 standard. 
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents. 
// The original developer of this software module and his/her company, 
// the subsequent editors and their companies, and ISO/IEC have no 
// liability for use of this software module or modifications thereof 
// in an application. No license to this software is granted for non 
// MPEG-7 conforming products. 
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module 
// to a third party and to inhibit third parties from using the software 
// module for non MPEG-7 conforming products. 
//
// Copyright (c) 2000-2015.
//
// This copyright notice must be included in all copies or derivative 
// works of this software module.
//
////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <momusys.h>
#include <vopio.h>
#include <fstream>  
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _VISUALC_
#include <unistd.h>
#else
#include <io.h>
#endif
#include <assert.h>
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif


#include "Applications/ServerApplications.h"
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */

using namespace XM;

const UUID HierarchicalKeyFrameServer::myID = UUID("");
const char *HierarchicalKeyFrameServer::myName = "Hierarchical Key Frame Server";


//----------------------------------------------------------------------------
HierarchicalKeyFrameServer::HierarchicalKeyFrameServer()
{
}

//----------------------------------------------------------------------------
HierarchicalKeyFrameServer::~HierarchicalKeyFrameServer()
{
}

//----------------------------------------------------------------------------
unsigned long HierarchicalKeyFrameServer::Open()
{
        // This server doesn't handle pull
        return 0;
}

//----------------------------------------------------------------------------
unsigned long HierarchicalKeyFrameServer::Start(char *ListFile, char *DescriptionFile)
{
        MomVop  *media = 0;   // image media
        MediaDB RamDB;          // Media Sequence data base
        int             iMedia, iFrame;
        char    *mediaFileName, buf[MAXMEDIANAMELEN];
        char    frameName[MAXMEDIANAMELEN], frameNameFormat[MAXMEDIANAMELEN];
        ULONG   frameStart, frameEnd;
        bool    bypassHist;

        //-------------------------------------------------------------------------
        // Instantiate concrete implementation objects
        //-------------------------------------------------------------------------

        // Extraction Tools
        HierarchicalKeyFrameExtractionTool *theExtractionTool;
        HierarchicalKeyFrameExtractionInterfaceABC *theExtractionInterface;

        // Generic Coding Scheme
        GenericDSCS *theCS;
        GenericDSCSInterfaceABC *theCSInterface;

        //-------------------------------------------------------------------------
        // Connect Interfaces 
        //-------------------------------------------------------------------------

        // Connect DS to ExtractionTool
        theExtractionTool = new HierarchicalKeyFrameExtractionTool;
        theExtractionInterface = theExtractionTool->GetInterface();

        // Connect DS to coding scheme
	theCS= new GenericDSCS;
        theCSInterface = theCS->GetInterface();


        //-------------------------------------------------------------------------
        // Start Extraction for each database in ListFile
        //-------------------------------------------------------------------------

        // Initialise Database
        RamDB.CreateDatabase(ListFile);


        // Loop through media
        iMedia = 0;
        while ( iMedia < RamDB.GetNoOfElements() )
        {
	        /* +1 simulates loading, requires a dummy last entry in
		   Listfile */
	        RamDB.GotoEntry( iMedia + 1);
		// retrieves "loaded" media file
		mediaFileName = RamDB.GetMediaFilename();
		// rewind
                RamDB.GotoEntry( iMedia );
                // Initialize the extraction, returns true if extraction 
		// is already complete
                bypassHist =
		  theExtractionInterface->InitExtracting(mediaFileName) != 0;

                // Determine frame range from filename
                StripFrameRange( buf, mediaFileName, frameStart, frameEnd );


                // Extract histograms for this media
                if ( ! bypassHist )
                {

                        // Determine media type (by extension)
                        if ( strstr(mediaFileName, ".mpg") ||
                                 strstr(mediaFileName, ".MPG") )
                        {
                                // The media is an MPEG, create frame filename format
                                strcpy( frameNameFormat, "m2v/%ld.ppm" );

                                // Remove old *.ppm files
                                iFrame = frameStart;
                                sprintf(frameName, frameNameFormat, iFrame);
                                while( remove(frameName) == 0 )
                                {
                                        iFrame ++;
                                        sprintf(frameName, frameNameFormat, iFrame);
                                }

                                // Extract MPEG2 frames
								// 3=PPM; skip must be nonnegative
                                if ( !RamDB.DBDecodeMPEGVideo(3, frameStart?frameStart-1:0, frameEnd-frameStart+1) )
                                        break;

                        }
                        else
                        {
								// The source is a directory of .ppm frames (previously decoded to speed things up)
								// Create the frame filename format using the directory path (sprintf, escape the %)
                                sprintf( frameNameFormat, "%s/%%ld.ppm", buf );
                        }

#ifdef SHOW_STATUS
			time_t tStart, tPrev, tNow;
			time(&tStart);
			tPrev = tStart;
			cerr << endl;
			TimeStamp("Extracting histograms");
#endif

                        // Loop through frames
                        iFrame = frameStart;
			bool bInSequence = false;
                        while( iFrame <= frameEnd )
                        {
                                // Load frame
                                sprintf( frameName, frameNameFormat, iFrame );
                                media = RamDB.LoadNamedImage(frameName);

                                // Stop if no more media sequences
                                if (!media) {
				  // Try next frame number until we have entered the sequence because
				  // the mpeg2decode may skip additional frames to synchronize, 
				  if(!bInSequence) {iFrame++;	continue;}
				  else {break;}
				}
				bInSequence = true;


#ifdef XYUVDISP
                                /* display input image*/
                                xyuv_show_simple(0,
						 (unsigned short *)media->
						 y_chan->data->s,
                                                 (unsigned short *)media->
						 u_chan->data->s,
                                                 (unsigned short *)media->
						 v_chan->data->s,
                                                 datasize(media->y_chan)&1,
                                                 media->y_chan->x,
						 media->y_chan->y,
                                                 media->u_chan->x,
						 media->u_chan->y,
                                                 "input");
#endif

                                // Set Data to SegmentExtraction
                                theExtractionInterface->SetSourceMedia(media);

#ifdef SHOW_STATUS
				fprintf(stderr,"%s %6ld of %ld",
					frameName, iFrame - frameStart,
					frameEnd - frameStart + 1);
				time(&tNow);
				long r, d = (long)difftime(tNow,tStart);
				r = (iFrame > frameStart) ? 
				  d * (frameEnd - iFrame) /
				  (iFrame - frameStart) : 0;
				if (tNow > tPrev)
				  {
				    fprintf(stderr,
					    " %ld:%02ld:%02ld %ld:%02ld:%02ld",
					    d/3600, d%3600/60, d%60, r/3600,
					    r%3600/60, r%60);
				    tPrev = tNow;
				  }
#ifdef DEBUG
				cerr << endl;
#else
				fprintf(stderr,"\r");
#endif

#endif //SHOW_STATUS

                                // Start Extraction
                                //cerr << "Extracting " << frameName << endl;
                                theExtractionInterface->StartExtracting();

                                // Free media
                                freevop(media);

                                // next image
                                iFrame ++;
                        }

#ifdef SHOW_STATUS
			cerr << endl << "PostExtraction (Build KeyFrames)"
			     << endl;
#endif

                }

				/* reset description data*/
				/* create XML Document for Description*/
				GenericDS theDocument = GenericDS::CreateDescriptionRoot();
				/* get descriptor interface*/
				GenericDSInterfaceABC *theDocumentInterfaceABC=
				  theDocument.GetInterface();

				/* connect descriptor to extraction tool*/
				theExtractionInterface->
				  SetDescriptorInterface(theDocumentInterfaceABC);

				/* connect descriptor to coding scheme*/
				theCSInterface->
				  SetDescriptionInterface(theDocumentInterfaceABC);

				/* connect output file to coding scheme*/
				theCSInterface->
				  SetEncoderFileName( DescriptionFile );

                // Finish Extraction
                theExtractionInterface->PostExtraction();

                /* code descriptor data*/
				// Output each in separate files??????
                theCSInterface->StartEncode();

                // next media sequence
                iMedia ++;
        }

#ifdef SHOW_STATUS
	TimeStamp("Done");
#endif
	theExtractionInterface->destroy();
	theCSInterface->destroy();
        return 0;
}


//----------------------------------------------------------------------------
unsigned long HierarchicalKeyFrameServer::Stop()
{
        return 0;
}
//----------------------------------------------------------------------------
unsigned long HierarchicalKeyFrameServer::Close()
{
        return 0;
}

	
//----------------------------------------------------------------------------
const UUID& HierarchicalKeyFrameServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HierarchicalKeyFrameServer::GetName(void)
{ return myName; }

