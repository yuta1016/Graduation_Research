///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// MediaIO.h

#ifndef __MEDIAIO_H__
#define __MEDIAIO_H__

#include "Media/Media.h"

#ifdef AFLIB
#include <libtsp.h>         //* typedef for AFILE *
#endif
#include "Media/AudioFrame.h"

/* avetro : define video compression format */
#define MPEG2   0
#define MPEG4   1

/***************************/
/* conflict with XML parser!!
   have to use void pointers uinstead of
   *image  and
   image_info

#ifndef RANDOMIMAGE
#include <magick/magick.h>
#if defined(__cplusplus) || defined(c_plusplus)
#undef class
#endif
#endif


*/
#define MAXMEDIANAMELEN 256

// MAXSIZE* and MINSIZE* can be or-ed
#define MAXSIZEMASK     15
#define MAXSIZE1024x768 1
#define MAXSIZE800x600  2
#define MAXSIZE640x480  3
#define MAXSIZE352x288  4

#define MINSIZEMASK     240
#define MINSIZE352x288  16
#define MINSIZE128x128  32

namespace XM
{

//==========================================================================
  class MediaDBElement
    {
    public:
      MediaDBElement(char *afilename);
      MediaDBElement();
      ~MediaDBElement();
      
      void SetFilename(char *afilename);
      char *GetFilename();

    private:
      
      char filename[MAXMEDIANAMELEN];
    };

/*==========================================================================*/
  class MediaDB
    {
    public:
      MediaDB(char *listfilename);
      MediaDB();
      ~MediaDB();

      /* Load a contenset data base into the memeory*/
      void CreateDatabase(char *listfilename);
      /* Get number of elements in data base*/
      int GetNoOfElements();
      /* Remove data abse from memory*/
      void FreeDatabase();
      /* Go to a entry i of the data base*/
      MediaDBElement *GotoEntry(int apos);  // -1 =last entry; 0= first entry

      /* Allows to connect a MultiMedia object to the database to
	 simplify the code for loading media files from the databas
	 into the memory*/
     int SetMedia(MultiMediaInterfaceABC *aMultiMediaInterface);

/*==========================================================================*/
/*	Audio Interface  
	Encapsulation for AF library from McGill for MPEG-7 XM  and
	Audio I/O on frame basis adapted to XM modularisation

	Author: Bernhard Feiten, Deutsche Telekom Berkom, 15.02.01, 12.03.01

	For writing audio files the following code should be used:
    Audio file format code, evaluated as the sum of a data format code and a file type, as defined in the AF-lib 
	Fformat = Dformat + 256 * Ftype

	For Ftype equal to zero, the file will have the standard AU audio file header. The Ftype flag allows for other file types as described below. Note that not all data formats are allowed for all file types. Dformat: data format 
		FD_MULAW8  = 1,  mu-law 8-bit data
		FD_ALAW8   = 2,  A-law 8-bit data
		FD_UINT8   = 3,  offset-binary 8-bit integer data
		FD_INT8    = 4,  two's-complement 8-bit integer data
		FD_INT16   = 5,  two's-complement 16-bit integer data
		FD_INT24   = 6,  two's-complement 24-bit integer data
		FD_INT32   = 7,  two's-complement 32-bit integer data
		FD_FLOAT32 = 8,  32-bit floating-point data
		FD_FLOAT64 = 9,  64-bit floating-point data
		FD_TEXT    = 10,  text data

	Ftype: file type 
		FTW_DEFAULT   = 0, default audio file type
		FTW_AU        = 1, AU audio file
		FTW_WAVE      = 2, RIFF WAVE file
		FTW_AIFF_C    = 3, AIFF-C audio file
		FTW_NH_EB     = 4, Headerless file (big-endian byte order)
		FTW_NH_EL     = 20, Headerless file (little-endian byte order)
		FTW_NH_NATIVE = 36, Headerless file (native byte order)
		FTW_NH_SWAP   = 52, Headerless file (byte-swapped byte order)
*/
	enum filestatus {closed, reading, writing};

public:
	void InitAudioFileReader(unsigned int hopsize, 
				 unsigned int framelength,
				 unsigned int offs = 0); // may be negativ
	                                                 //to compensate delay

	unsigned int DBOpenAudioFile( );    //return 1 when ok,
	                                    //otherwise 0; opens next
	                                    //file in database

	void OpenNamedAudioFileForReading( char * fname); // Open for reading

	void OpenNamedAudioFileForWriting( char * fname,  // Open for writing
					   unsigned int nchan, 
					   double srate,
					   unsigned int Fformat = 2*256+5);
	              // AF-Makros shoudn't be used to avoid the dependency

	void LoadAudioFrame(void);	
	void StoreAudioFrame(void); 
	void CloseAudioFile(void);
	unsigned int AudioFileGetNumOfAudioChannels(void);
	double AudioFileGetSampleRate(void);
	unsigned long int AudioFileGetNumOfSamps(void);


/*  End of audio interface */
/*==========================================================================*/

      /* Motion vektor extraction from an MPEG video stream into the file
	 vertors.ppm
	 The vertors.ppm file contains in a stacked form the vector images
	 from the P-frames of the MPEG-video stream. To load a vector image
	 for frame N use the the function LoadNamedImage("vertors.ppm[N]").
	 The first channel (Y) if the vector image contains the x vector
	 components of the macro blocks, the second channel (U) contains
	 the y-components, and the third channel (V) the coding type of
	 the macro block (0= no vectors for this block, 1= vectors exist)*/
      /* Extract vector from current entry of data base and go to next entry*/
      int DBExtractMPEGMotionVectors();
      /* Extract vectors for named file*/
      int ExtractNamedMPEGMotionVectors(char *MPEGFilename);

      /* Decode MPEG video stream to single ppm files named m2v/%d.ppm.
	 If an extension with the alpha masks for a visual objects exist,
	 then the alpha information is also decoded. The extension stream
	 name is computed by replacing the ".mpg" of the sequence file name
	 with "_a.mpg".
	 To read the images use LoadNamedImage("m2v/N.ppm"), where N is the
	 frame number*/
      /* Decode current entry of data base, put sequence name to media and
	 go to next database entry.*/
      int DBDecodeMPEGVideo();
      int DBDecodeMPEGVideo(int format, int begin, int frames); // PK
#ifdef LISMS_MASTER_H
      int DBOpenMPEGVideo();
      int DBOpenMPEGVideo(int format);
#endif /* LISMS_MASTER_H */
      int DBDecodeMPEG4Video(int numvos, char *sim_dir, char *ifile); // avetro
      /* Decode a named MPEG video sequence and put sequence name to media*/
      int DecodeNamedMPEGVideo(char *MPEGFilename);
      int DecodeNamedMPEGVideo(char *MPEGFilename, int format,
			       int start, int frames); // PK
#ifdef LISMS_MASTER_H
      int OpenNamedMPEGVideo(char *MPEGFilename);
      int OpenNamedMPEGVideo(char *MPEGFilename, int format);
      int ReadMPEGVideoFrames(int start, int frames);
      int CloseMPEGVideo();
#endif /* LISMS_MASTER_H */
      int DecodeNamedMPEG4Video(int numvos, char *sim_dir, char *ifile); // avetro

	  //2009.05.18 Video Signature tools
	  int DBOpenMPEGFile();
	  int OpenNamedMPEGFile(char *MPEGFilename);


      /* Encode images to MPEG video stream*/
      int DBEncodeMPEGVideo(char *mpgfilename,char *param_filename,
			    char *xmlfile);//PK
      int EncodeNamedMPEGVideo(char *mpgfilename,char *param_filename,
			       char *xmlfile);//PK
      char *GenEncoderParFile(char *filename,char * framenames,
			      int no_of_frames,int xsize,int ysize);
      int DBCleanM2Vdir();
      int DBEncodeMPEG4Video(int use_xml, char *xmlfile, char *src_dir, char *sim_dir, 
			     int numvos, int frames, int output_rate); // avetro
      int EncodeNamedMPEG4Video(int use_xml, char *xmlfile, char *src_dir, char *sim_dir, 
				int numvos, int frames, int output_rate); // avetro


      /* Load an image into the memory. This function is applying the Magick
	 library, thus this function can read various image file formats. The
	 loaded image is stored in thein the MomVop structure of attached
	 MultiMedia object. For the moment the return value is also the
	 pointer to the Imgae. If the return value is NULL then no image was
	 loaded. This behaviour is used to detect also when all images of a
	 sequence have been loaded.
	 In case the image contains an alpha or a binary segmentation mask. The
	 shape information is loaded from an extension file or from the
	 trancparency information. The names of extension files are computed
	 by replacing .ext with _a.ext or _a.pgm or _a.pbm.*/
      /* set maximuim or minimum image size for processing*/
      void SetImageScaling(int ScaleFormat);
      /* load teh current image of the database and go to next database entry*/
      MomVop *DBLoadImage();
      /* load a named image file*/
      MomVop *LoadNamedImage(char *ImageFilename);
      /* test if the file with shape extension exists and returns
	 file name if it exists*/
      char *TestAlphaFile(char *Filename, int TestStdExt);
      /* store the last loaded image to a named image file. Used to store
	 image of a sequence for later use or to convert the file format*/ 
      int StoreNamedImage(char *ImageFilename);
      /* Store an image from the internal memeory representation of the
	 image vop with the given name. Used to store processing results*/
      int StoreNamedVop(char *ImageFilename, MomVop *vop);
      
      /* Load 3D Mesh from file. The file name is specified in the content
	 set data base. This media type is used for the 3D shape description.
	 To get further information about the 3D mesh format, please
	 contact Titus Zaharia*/
      MediaThreeD *DBLoad3DMesh();
      /* load a the 3D mesh from a named files*/
      MediaThreeD *LoadNamed3DMesh(char *meshfilename);

      /* Load a key point list into memory. The needs to have the following
	 format in ASCII
	 time x-coordinate, y-coordinate, z-coordinate ..
	 Each key point is stored in the TnDKeyPoint structur
	 (see MultiMedia.h).
	 The key points of one time point are stored in one linked list using
	 the Utilities/LinkedList library. The linked lists for the
	 timepoints are stored also in a linked list. The resulting 2D
	 linked List is stored in the attached MultiMedia object*/
      /* Load the current key point file of the database and go to next
	 database entry*/
      int DBLoadKeyPointList();//sth properly removed soon
      /* Load a named key point file*/
      int LoadNamedKeyPointList(char *KeyPointFilename);


      //added by Sylvie Jeannin, August 22: above functions not understood / adequate
      //sth please rename the function in appllication
#define DBLoadKeyPointListSylvie DBLoad4DKeyPointList
#define LoadNamedKeyPointListSylvie LoadNamed4DKeyPointList
      int LoadNamed4DKeyPointList(char *KeyPointFilename);
      int DBLoad4DKeyPointList();


      /* old will be removed!!!!*/
      /* old version of the keypoint list, only counts the number of
	 keypoints (lines). Decoding (file IO) has to be done in extration
	 which is not perimtted*/
/*       unsigned long DBLoadPointerList(); */
/*       T4DKeyPoint *DBLoadPointer(int ImageNumber); */
/*       unsigned long LoadNamedPointerList(char *PointerFilename); */
/*       T4DKeyPoint *LoadNamedPointer(char *PointerFilename,int ImageNumber); */

      /* Get the filename of the last loaded file from the content set
	 database. If GotoEntry was used then the file name of the specified
	 entry is returned*/
      char *GetMediaFilename();
      /* Get the comment of the last loaded image (only if included
	 in file format*/
      char *GetImageComment();
      /* Get the number in the image comment string after the
	 string "FrameNo:"*/
      unsigned long GetFrameNoFromComment();
      /* get the MPEG coding type from image comment string after the
	 string "CodingType:"
	 -1=not in comment
	 1=I_frame
	 2=P-frame
	 3=B-frame
      */
      unsigned long GetCodingTypeFromComment();

    private:

      //audio
#ifdef AFLIB
      AFILE	* m_AFp; 
#endif
      double	m_srate;
      unsigned int	m_nchan;
      unsigned int	m_hopsize;
      unsigned int	m_framelength;    // number of samples
      //-- bufsize = m_framesize * m_chan * sizeof(float) 

      unsigned long int	m_framenumber;
      unsigned long int	m_numofsamps;
      filestatus		m_filestatus;
      unsigned long int m_offs;

      //video
      int mpegdec_used;
      int mpeg4dec_used;
      int listfile;
      int entries;
      MediaDBElement *DataBase;
      int pos;
      bool incremented;
      MultiMediaInterfaceABC *Media;
      int m_ImageScaling;
      char *Comment;
      void *DecodedImage;
      void *DecodedImageInfo;
      char *m_AlphaFilename;
#ifdef LISMS_MASTER_H
      lisms::master m_Master;
#endif //LISMS_MASTER_H
      int DeleteImage();
      int DecodeAlphaMPEGVideo(char *MPEGFilename);
      int LoadAlphaImage(char *ImageFilename,MomImage *alpha);

    };
};

class MediaIOError 
{
private:
	MediaIOError( ) { }
public:
	MediaIOError( const char * msg ) { m_msg = msg;}
	const char * m_msg;
};



#endif //__MEDIAIO_H__
