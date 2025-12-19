///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich
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
// MediaIO.cpp

#include <math.h>

#ifndef RANDOMIMAGE
//#include <magick/magick.h>
//#include <magick/image.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <magick/api.h>
#if defined(__cplusplus) || defined(c_plusplus)
#undef class
#endif
#endif

#ifdef AFLIB
#include <libtsp.h>         //* typedef for AFILE *
#endif
#include "Media/AudioFrame.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _VISUALC_
#include <unistd.h>
#include <limits.h>
#else
#include <io.h>
#endif

#include "vopio.h"
#include "momusys.h"
#include "intra.h"
#include "Media/Media.h"

#ifndef _VISUALC_
#define CLEANM2V "rm -f m2v/*"
#else  /*DOS*/
#if (LANG == 49) /*deutsch*/
#define CLEANM2V "if exist m2v\\*.* (echo j | del m2v\\*.*)"
#endif
#if (LANG == 33) /*francaise*/
#define CLEANM2V "if exist m2v\\*.* (echo o | del m2v\\*.*)"
#endif
#if (LANG ==34) /*espanol*/
#define CLEANM2V "if exist m2v\\*.* (echo s | del m2v\\*.*)"
#endif
#ifndef CLEANM2V /*default language*/
#define CLEANM2V "if exist m2v\\*.* (echo y | del m2v\\*.*)"
#endif/* select language*/
#endif

#if (MagickLibVersion & 0x0f00)<0x0500
#define _XMUSEMAGICK4_
#else
#if (MagickLibVersion & 0x0ff0)<0x0530
#define _XMUSEMAGICK5_
#else
#define _XMUSEMAGICK53_
#endif
#endif

#ifdef _XMUSEMAGICK4_
#define XMMAGICKPIXELTYPE RunlengthPacket
#define XMMAGICKALPHAFIELD index
#else
#define XMMAGICKPIXELTYPE PixelPacket
#define XMMAGICKALPHAFIELD opacity
#endif

#ifdef _XMUSEMAGICK4_
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif
#endif

//static int __xm_magick_initilazed =0;

using namespace XM;


/*==========================================================================*/
MediaDBElement::MediaDBElement(char *afilename)
{
  strncpy(filename,afilename,MAXMEDIANAMELEN-2);
  filename[MAXMEDIANAMELEN-1]=0;
}

/*--------------------------------------------------------------------------*/
MediaDBElement::MediaDBElement()
{
  filename[0]=0;
}

/*--------------------------------------------------------------------------*/
MediaDBElement::~MediaDBElement()
{
}

/*--------------------------------------------------------------------------*/
void MediaDBElement::SetFilename(char *afilename)

{
  strncpy(filename,afilename,MAXMEDIANAMELEN-1);
}

/*--------------------------------------------------------------------------*/
char *MediaDBElement::GetFilename()
{
  return &filename[0];
}


/*==========================================================================*/
MediaDB::MediaDB(char *listfilename):
m_hopsize(1024),
m_framelength(1024),
m_filestatus(closed),
m_offs(0),
mpegdec_used(0),
listfile(0),
entries(0),
DataBase(0),
pos(0),
incremented(0),
Media(0),
m_ImageScaling(0),
Comment(0),
DecodedImage(0),
DecodedImageInfo(0),
m_AlphaFilename(0)

{
  system(CLEANM2V);

//#ifdef _XMUSEMAGICK53_
//  if (!__xm_magick_initilazed) {
//    InitializeMagick("");
//    __xm_magick_initilazed=1;
//#ifdef DEBUG
//    fprintf(stderr,"Initializing ImageMagick\n");
//#endif
//  };
//#endif

  CreateDatabase(listfilename);
}

/*--------------------------------------------------------------------------*/
MediaDB::MediaDB():
m_hopsize(1024),
m_framelength(1024),
m_filestatus(closed),
m_offs(0),
mpegdec_used(0),
listfile(0),
entries(0),
DataBase(0),
pos(0),
incremented(0),
Media(0),
m_ImageScaling(0),
Comment(0),
DecodedImage(0),
DecodedImageInfo(0),
m_AlphaFilename(0)

{
  system(CLEANM2V);

//#ifdef _XMUSEMAGICK53_
//  if (!__xm_magick_initilazed) {
//    InitializeMagick("");
//    __xm_magick_initilazed=1;
//#ifdef DEBUG
//    fprintf(stderr,"Initializing ImageMagick\n");
//#endif
//  };
//#endif

}

/*--------------------------------------------------------------------------*/
MediaDB::~MediaDB()

{
  if (Comment) delete [] Comment;
  if (DataBase) delete [] DataBase;
  if (DecodedImage) {
#ifndef RANDOMIMAGE
    DestroyImages((Image *)DecodedImage);
#endif
  }
  if (DecodedImageInfo) {
#ifndef RANDOMIMAGE
    DestroyImageInfo((ImageInfo *)DecodedImageInfo);
#endif
  }

  if (m_AlphaFilename) delete [] m_AlphaFilename;

  /* clean directories*/
  unlink("vectorfile.ppm");
  if (mpegdec_used) system(CLEANM2V);
}


/*--------------------------------------------------------------------------*/
void MediaDB::CreateDatabase(char *listfilename)

{
  int listfile;
  const char *delimiter=" \t\n\r";
  char a;
  char isdelimiter=1;

  char thefilename[MAXMEDIANAMELEN];
  int filenamelen;

  listfile=open(listfilename,O_RDONLY);
  if (listfile == -1) {
    fprintf(stderr,"Error: Could not open Listname %s\n",listfilename);
    exit(1);
  }

  entries=0;

  /* extract number of filenames*/
  while (read(listfile,&a,1)) {
    if (strchr(delimiter,a)) {
      isdelimiter=1;
    }
    else {
      if (isdelimiter) {
	entries++;
	isdelimiter=0;
      }
    }
  }

  if (!entries) {
    DataBase=0;
    return;
  }

  /* malloc data base memory*/
  DataBase = new MediaDBElement [entries];

  /* scan filenames*/
  pos=0;
  filenamelen=0;
  isdelimiter=1;
  lseek(listfile,0,SEEK_SET);  // rewind file
  while (read(listfile,&a,1)) {
    if (strchr(delimiter,a)) {
      if (!isdelimiter) {
	thefilename[filenamelen]=0;
	DataBase[pos].SetFilename(thefilename);
	filenamelen=0;
	pos++;
	isdelimiter=1;
      }
    }
    else {
      isdelimiter=0;
      if (filenamelen < (MAXMEDIANAMELEN-2)) {
	thefilename[filenamelen]=a;
	filenamelen++;
      }
    }
  }

  /* if needed write last input file name*/
  if (!isdelimiter) {
    thefilename[filenamelen]=0;
    DataBase[pos].SetFilename(thefilename);
    filenamelen=0;
    pos++;
  }

#ifdef DEBUG
  if (pos != entries) {
    fprintf(stderr,"Error: number of entries mismatches in CreateDatabase\n");
    exit (1);
  }
  fprintf(stderr,"Info: %d files in database\n",entries);
#endif

  /* close list file*/
  close(listfile);

  incremented=0;
  pos=0;
}

/*--------------------------------------------------------------------------*/
int MediaDB::GetNoOfElements()

{
  return entries;
}

/*--------------------------------------------------------------------------*/
void MediaDB::FreeDatabase()
{
  entries=0;
  if (DataBase) {
    if (DataBase) delete [] DataBase;
    DataBase=0;
  }
  incremented=0;
  pos=0;
}

/*--------------------------------------------------------------------------*/
MediaDBElement *MediaDB::GotoEntry(int apos)

{
  // -1 =last entry; 0= first entry
  if (apos>=entries) apos=entries-1;
  if (apos<0) apos=entries-1;

  pos=apos;
  incremented=0;
  return &DataBase[pos];
}

/*--------------------------------------------------------------------------*/
int MediaDB::SetMedia(MultiMediaInterfaceABC *aMultiMediaInterface)
{
  Media=aMultiMediaInterface;
  return 0;
}

/*----------------------------- Audio --------------------------------------*/ 
/*--------------------------------------------------------------------------*/
void MediaDB::InitAudioFileReader(unsigned int hopsize, 
								  unsigned int framelength,
								  unsigned int offs) 		
{
	m_hopsize = hopsize;
	m_framelength = framelength;
	m_offs = offs;
}

/*--------------------------------------------------------------------------*/
void MediaDB::OpenNamedAudioFileForReading(char * fname)
{
#ifdef AFLIB
	long int numofsamps, nchan;
	float srate;
#endif
	if (!Media) throw MediaIOError("No Mediainterface initialized");	
	if (m_filestatus != closed) throw MediaIOError("Filestatus error");
	m_filestatus = closed;
	m_framenumber = 0;

#ifdef AFLIB
	m_AFp = AFopenRead (fname, &numofsamps, &nchan, &srate, NULL);
	if (m_AFp == NULL) throw MediaIOError("Cant open file for reading");
	m_numofsamps = numofsamps/nchan;
	m_nchan = (unsigned int) nchan;
	m_srate = (double) srate;
	m_filestatus = reading;
#endif	
}

/*--------------------------------------------------------------------------*/
unsigned int MediaDB::DBOpenAudioFile()
{
  if (pos<0) pos=0;
  if (pos>=entries) return 0;

  incremented=1;
  OpenNamedAudioFileForReading(DataBase[pos++].GetFilename());
  return 1;
}

/*--------------------------------------------------------------------------*/
void MediaDB::OpenNamedAudioFileForWriting(char * fname,unsigned int nchan,
					   double srate,unsigned int Fformat)		
{
	if (!Media) throw MediaIOError("No Mediainterface initialized");	
	if (m_filestatus != closed) throw MediaIOError("Filestatus error");
	m_filestatus = closed;
	m_numofsamps = 0;
#ifdef AFLIB
	m_AFp = AFopenWrite (fname, Fformat, nchan, srate, NULL);
	if (m_AFp == NULL) throw MediaIOError("Cant open file for reading");

	m_nchan = nchan;
	m_srate = srate;
	m_filestatus = writing;

#endif
}

/*--------------------------------------------------------------------------*/
void MediaDB::LoadAudioFrame( )	
{
	int n=0;
	if (m_filestatus != reading) throw MediaIOError("Filestatus error");

	CAudioFrame af(m_framelength, m_nchan, m_srate);

#ifdef AFLIB
	n = AFreadData (m_AFp, m_offs, af.GetAudioBuf(), (int) (m_framelength * m_nchan)); 
	m_offs += m_hopsize * m_nchan;
#endif

	af.SetAudioFrameLen(n/m_nchan);
	af.SetAudioHopsize(m_hopsize);
	af.SetNumOfAudioChannels(m_nchan);
	af.SetTimeStamp(m_framenumber);
	m_framenumber ++;
	af.SetSampleRate(m_srate);
	if (Media) Media->SetAudioFrame(af);
}

/*--------------------------------------------------------------------------*/
void MediaDB::StoreAudioFrame( ) 
{
#ifdef AFLIB
	int n = 0;
#endif
	int num_of_sample_frames;
	CAudioFrame * af;

	if (m_filestatus != writing) throw MediaIOError("Filestatus error");
	if (!Media) throw MediaIOError("No Media error");
	af = Media->GetAudioFrame();

	if (af->GetNumOfAudioChannels() != m_nchan) throw MediaIOError("Channel number mismatch");
	if (af->GetSampleRate() != m_srate) throw MediaIOError("Samplerate mismatch");
	num_of_sample_frames = af->GetAudioFrameLen();

#ifdef AFLIB
	n = AFwriteData (m_AFp, af->GetAudioBuf(), (int) (num_of_sample_frames * m_nchan));
	m_numofsamps += n/m_nchan;
#endif
}

/*--------------------------------------------------------------------------*/
void MediaDB::CloseAudioFile( void)
{
#ifdef AFLIB
	AFclose(m_AFp);
#endif	
	m_filestatus = closed;
}

/*--------------------------------------------------------------------------*/
unsigned int MediaDB::AudioFileGetNumOfAudioChannels()
{
	if (m_filestatus == closed) throw MediaIOError("Filestatus error");
	return m_nchan;
}

/*--------------------------------------------------------------------------*/
double MediaDB::AudioFileGetSampleRate(void)
{
	if (m_filestatus == closed) throw MediaIOError("Filestatus error");
	return m_srate;
}

/*--------------------------------------------------------------------------*/
unsigned long int MediaDB::AudioFileGetNumOfSamps(void)
{
	if (m_filestatus == closed) throw MediaIOError("Filestatus error");
	return m_numofsamps;
}
/*-end--------------------------- Audio --------------------------------end-*/ 
	

/*--------------------------------------------------------------------------*/
int MediaDB::DBExtractMPEGMotionVectors()

{
  if (pos<0) pos=0;
  if (pos<0) pos=0;
  if (pos>=entries) {
    unlink("vectorfile.ppm");
    return 0;
  }

  incremented=1;
  return ExtractNamedMPEGMotionVectors(DataBase[pos++].GetFilename());
}

/*--------------------------------------------------------------------------*/
int MediaDB::ExtractNamedMPEGMotionVectors(char *MPEGFilename)

{
  char filename[MAXMEDIANAMELEN];
  char *info;
  char command[512];

  strcpy(filename,MPEGFilename);

  /* remove temporal image from previous run*/
  unlink("vectorfile.ppm");

  if (!filename) {
    return 0;
  }
  if (!filename[0]) {
    return 0;
  }

  /* put filename to Media*/
  if (Media) Media->SetVideoFileName(MPEGFilename);

  /* extract frame range info from filename*/
  if (filename[strlen(filename)-1]==']') {
    info=strrchr(filename,'[');
    if (info) {
      *info=0; /* cut filename*/
      info++; /* goto info start*/
      info[strlen(info)-1]=0; /* remove closing bracket*/
    }
  }
  else {
    info=0;
  }

  /* extract vectors from sequence*/
  command[511]=0;
  strcpy(command,"mpeg2decode -o6 -m vectorfile.ppm -b ");
  strncat(command,filename,511-strlen(command));
//   if (info) {
//     /*add frame range to command line*/
//     strncat(command,"-n ",511-strlen(command));
//     strncat(command,info,511-strlen(command));
//     strncat(command," ",511-strlen(command));
//   }
  if (system(command)) {
#ifndef WIN32
    if (system("mpeg2decode | grep MPEG7XM >| /dev/null")) {
      fprintf(stderr,"Error: XM does not call mpeg2dec including "
	      "vector extarction (version comming with XM)\n");
    }
#else
    fprintf(stderr,"Error: Check if the mpeg2dec from MPEG-7 XM is "
	    "called\n");
#endif
    unlink("vectorfile.ppm");
    return 0;
  }

  return 1;
}

/*--------------------------------------------------------------------------*/
int MediaDB::DBDecodeMPEGVideo()

{
  if (pos<0) pos=0;
  if (pos>=entries) {
    if (mpegdec_used) system(CLEANM2V);
    mpegdec_used=0;
    return 0;
  }

  incremented=1;
  return DecodeNamedMPEGVideo(DataBase[pos++].GetFilename(),3, 0, -1);
}

/*--------------------------------------------------------------------------*/
int MediaDB::DBDecodeMPEGVideo(int format, int begin, int nframes)

{
#ifdef DEBUG
  fprintf(stderr,"### DBDDecode: format: %d \n", format);
  
  /*
    printf("pos1:%d entries:%d name-1:%s name:%s name+1:%s\n", 
  	pos, entries, DataBase[pos-1].GetFilename(), 
	DataBase[pos].GetFilename(),DataBase[pos+1].GetFilename()); 
   */
#endif

  if (pos<0) pos=0;
  if (pos>=entries) {
    if (mpegdec_used) system(CLEANM2V);
    mpegdec_used=0;
    return 0;
  }
  incremented=1; // PK Peter Kuhn (Sony) added

  return DecodeNamedMPEGVideo(DataBase[pos++].GetFilename(), format, begin, nframes);
}

/*--------------------------------------------------------------------------*/
int MediaDB::DecodeNamedMPEGVideo(char *MPEGFilename)

{
  return DecodeNamedMPEGVideo(MPEGFilename,3,0,-1);
}

/*--------------------------------------------------------------------------*/
int MediaDB::DecodeNamedMPEGVideo(char *MPEGFilename, int format, int start, 
	int frames)

{
  char filename[MAXMEDIANAMELEN];
  char *info;
  char command[512];
  //char tmpstr[512]; // PK

  strcpy(filename,MPEGFilename);

#ifdef DEBUG
  fprintf(stderr,"dec: filename: %s format:%d start:%d frames:%d\n", 
  	filename, format, start, frames); // PK added
#endif

  /* remove temporal images from previous run*/
  if (mpegdec_used) system(CLEANM2V);
  mpegdec_used=0;

  if (!filename) {
    return 0;
  }
  if (!filename[0]) {
    return 0;
  }

  /* put filename to Media*/
  if (Media) Media->SetVideoFileName(MPEGFilename);

  /* extract frame range info from filename*/
  if (filename[strlen(filename)-1]==']') {
    info=strrchr(filename,'[');
    if (info) {
      *info=0; /* cut filename*/
      info++; /* goto info start*/
      info[strlen(info)-1]=0; /* remove closing bracket*/
    }
  }
  else {
    info=0;
  }
#ifdef DEBUG
  fprintf(stderr,"dec: filename %s \n", filename); // PK added
#endif

  /* extract vectors from sequence*/
  command[511]=0;
  if ((frames == -1)){ 
    sprintf(command,"mpeg2decode -b %s -o%d m2v/", filename,format);
  }
  else {
    sprintf(command,"mpeg2decode -s %d -p %d -b %s -o%d m2v/", 
	    start, frames, filename, format);
  }
  strncat(command,"%d",511-strlen(command));
#ifdef DEBUG
  fprintf(stderr,"### %s #### \n",command); // PK added
#endif

  if (info) {
    /*add frame range to command line*/
//     strncat(command,"-n ",511-strlen(command));
//     strncat(command,info,511-strlen(command));
//     strncat(command," ",511-strlen(command));
  }
  if (system(command)) {
#ifndef WIN32
    if (system("mpeg2decode | grep MPEG7XM >| /dev/null")) {
      fprintf(stderr,"Error: XM does not call mpeg2dec including "
	      "vector extarction (version comming with XM)\n");
    }
#else
    fprintf(stderr,"Error: Check if the mpeg2dec from MPEG-7 XM is "
	    "called\n");
#endif
//     unlink("m2v.ppm");
    return 0;
  }

  /* decode alpha channel if file exists*/
  DecodeAlphaMPEGVideo(filename);

  mpegdec_used=1;
  return 1;
}

#ifdef LISMS_MASTER_H
/*--------------------------------------------------------------------------*/
int MediaDB::DBOpenMPEGVideo()

{
  if (pos<0) pos=0;
  if (pos>=entries) {
    if (mpegdec_used) system(CLEANM2V);
    mpegdec_used=0;
    return 0;
  }

  incremented=1;
  return OpenNamedMPEGVideo(DataBase[pos++].GetFilename(),3);
}

/*--------------------------------------------------------------------------*/
int MediaDB::DBOpenMPEGVideo(int format)
{
#ifdef DEBUG
  fprintf(stderr,"### DBDOpen: format: %d \n", format);
  
  /*
    printf("pos1:%d entries:%d name-1:%s name:%s name+1:%s\n", 
  	pos, entries, DataBase[pos-1].GetFilename(), 
	DataBase[pos].GetFilename(),DataBase[pos+1].GetFilename()); 
   */
#endif

  if (pos<0) pos=0;
  if (pos>=entries) {
    if (mpegdec_used) system(CLEANM2V);//to do: remove system call
    mpegdec_used=0;
    return 0;
  }
  incremented=1; // PK Peter Kuhn (Sony) added

  return OpenNamedMPEGVideo(DataBase[pos++].GetFilename(), format);
}

/*--------------------------------------------------------------------------*/
int MediaDB::OpenNamedMPEGVideo(char *MPEGFilename)

{
  return OpenNamedMPEGVideo(MPEGFilename,3);
}

/*--------------------------------------------------------------------------*/
int MediaDB::OpenNamedMPEGVideo(char *MPEGFilename, int format)

{
  char filename[MAXMEDIANAMELEN];
  char *info;

  strcpy(filename,MPEGFilename);

  if (format < 0) format=0;
  if (format > 6) format=6;
#ifdef DEBUGPK
  fprintf(stderr,"dec: filename: %s format:%d\n", 
  	filename, format);
#endif

  /* remove temporal images from previous run*/
  if (mpegdec_used) system(CLEANM2V);
  mpegdec_used=0;

  if (!filename) {
    return 0;
  }

  /* put filename to Media*/
  if (Media) Media->SetVideoFileName(MPEGFilename);

  /* extract frame range info from filename*/
  if (filename[strlen(filename)-1]==']') {
    info=strrchr(filename,'[');
    if (info) {
      *info=0; /* cut filename*/
      info++; /* goto info start*/
      info[strlen(info)-1]=0; /* remove closing bracket*/
    }
  }
  else {
    info=0;
  }

  {
    char *argv[7];
    char par0[12]="mpeg2decode";
    char par1[3]="-a";
    char par2[3]="-b";
    char par4[4];
    char par5[7]="m2v/%d";

    sprintf(par4,"-o%d",format);
    
    argv[0]=par0;
    argv[1]=par1;
    argv[2]=par2;
    argv[3]=filename;
    argv[4]=par4;
    argv[5]=par5;
    argv[6]=0;

#ifdef DEBUG2
    fprintf(stderr,"%s %s %s %s %s %s\n",
	    argv[0],argv[1],argv[2],argv[3],argv[4],argv[5]);
#endif

    m_Master.lisms_startslave("mpeg2decode",argv);
  }
  /*      execlp("mpeg2decode", "mpeg2decode",  "-a", "-b", filename, format_param, "m2v/%d", NULL);*/


  //    printf("exec failed\n");
  //    exit(-1);

  mpegdec_used=1;
  return 1;
}

/*--------------------------------------------------------------------------*/
int MediaDB::ReadMPEGVideoFrames(int start, int frames)
{
  char command[32];

  sprintf(command,"f=%d,p=%d", start, frames);

  m_Master.lisms_send(command);
//  m_Master.lisms_send("f=");
//  m_Master.lisms_sendint(start);
//  m_Master.lisms_send(",p=");
//  m_Master.lisms_sendint(frames);
  return m_Master.lisms_waitanswerint(0);
}

/*--------------------------------------------------------------------------*/
int MediaDB::CloseMPEGVideo()
{
  m_Master.lisms_send("f=0,p=0");
  m_Master.lisms_waitanswerint(0);
  m_Master.lisms_close();
  return 0;
}

#endif /* LISMS_MASTER_H */

/*--------------------------------------------------------------------------*/
int MediaDB::DBCleanM2Vdir()

{
  if (pos<0) pos=0;
 
  if (pos>=entries) {
    system(CLEANM2V);
    return 0;
  }

  return 0;
}

/*--------------------------------------------------------------------------*/
int MediaDB::DBDecodeMPEG4Video(int numvos, char *sim_dir, char *ifile)

{
#ifdef DEBUG
  //  printf("### DBDDecode (MPEG4): numvos=%d\n",numvos);//sth to stderr!
#endif
  if (pos<0) pos=0;
  if (pos>=entries) {
    // if (mpeg4dec_used) system(CLEANM4V);
    mpeg4dec_used=0;
    return 0;
  }

  return DecodeNamedMPEG4Video(numvos, sim_dir, ifile);
}

/*--------------------------------------------------------------------------*/
int MediaDB::DecodeNamedMPEG4Video(int numvos, char *sim_dir, char *ifile)

{
  char command[512];
  char tmpstr[512];
  int i;

  /* extract vectors from sequence*/
  command[511]=0;
 
  sprintf(command,"xmpg4dec -n %d -b ", numvos);
  for(i=0; i<numvos; i++) {
    sprintf(tmpstr, "%s ",DataBase[pos++].GetFilename());
    strcat(command, tmpstr);
  }

#ifdef DEBUG
  //  printf("### %s #### \n",command);//sth to stderr!
#endif

  if (system(command)) {
#ifndef WIN32
    if (system("xmpg4dec | grep MPEG7XM >| /dev/null")) {
      fprintf(stderr,"Error: XM does not call xmpg4dec including "
	      "vector extarction (version comming with XM)\n");
    }
#else
    fprintf(stderr,"Error: Check if the xmpg4dec from MPEG-7 XM is "
	    "called\n");
#endif
    return 0;
  }

  mpeg4dec_used=1;
  return 1;
}


/* 2009.05.18 Read video frames - functions implemented for video signature */

int MediaDB::DBOpenMPEGFile()
{
  if (pos>=entries) {
    return 0;
  }

  incremented=1;
  return OpenNamedMPEGFile(DataBase[pos++].GetFilename());
}

int MediaDB::OpenNamedMPEGFile(char *MPEGFilename)
{
	int ret = 0;
	if (!Media) throw MediaIOError("No Media error");
	if(Media->SetVideoFileName(MPEGFilename)==0)
	{
		ret = Media->OpenVideoFile();
	}

	return ret;
}




/*--------------------------------------------------------------------------*/
int MediaDB::DBEncodeMPEGVideo(char *mpgfilename,char *param_filename,
			       char *xmlfilename)
{ 
  return EncodeNamedMPEGVideo(mpgfilename,param_filename,xmlfilename);
}

/*--------------------------------------------------------------------------*/
int MediaDB::EncodeNamedMPEGVideo(char *mpgfilename,char *param_filename,
				  char *xmlfilename)
{  
  //  char *info;
  char command[1024];

  if (!mpgfilename) {
    return 0;
  }

  if (!param_filename) {
    return 0;
  }

  fprintf(stderr,"Encoding MPEG video ... \n");
  fprintf(stderr," mpgfile: %s \n", mpgfilename);
  fprintf(stderr," parfile: %s \n", param_filename);


  /* generate string for encoding command*/
  command[1023]=0;
 
  if ((xmlfilename)!=0){
    sprintf(command,"xmpg2enc %s %s %s",
	    param_filename,mpgfilename,xmlfilename);
    fprintf(stderr," xmlfile: %s \n", xmlfilename);
  }
  else {
    sprintf(command,"xmpg2enc %s %s ",param_filename,mpgfilename);
    //sprintf(command,"mpeg2encode %s %s ",param_filename,mpgfilename);
  }
#ifdef DEBUG
  fprintf(stderr,"### %s #### \n",command); // PK added
#endif

  system(command);

  return 1;
}

/*--------------------------------------------------------------------------*/
char *MediaDB::GenEncoderParFile(char *filename,char *framenames,
				 int no_of_frames,int xsize, int ysize)

{
  FILE *tmpparfile;

  if (!(no_of_frames && xsize && ysize)) return 0;

  if (!filename) {
    filename=tmpnam(0);
    if (!filename) return 0;
    if (!filename[0]) return 0;
  }

  tmpparfile=fopen(filename,"w+");

  if (!tmpparfile) return 0;

  fprintf(tmpparfile,"\n%s\n",framenames);
  fprintf(tmpparfile,"-\n-\n-\n-\n2\n%d\n0\n",no_of_frames);
  fprintf(tmpparfile,"00:00:00:00\n12\n3\n1\n");
  fprintf(tmpparfile,"0\n"); //sth put this to the end
  fprintf(tmpparfile,"0\n%d\n%d\n",xsize,ysize);
  fprintf(tmpparfile,"8\n3\n1152000.0\n20\n0\n1\n4\n8\n");
  fprintf(tmpparfile,"1\n3\n1\n5\n5\n5\n%d\n%d\n",xsize,ysize);
  fprintf(tmpparfile,"0\n0\n1 1 1\n0 0 0\n0 0 0\n0 0 0\n0 0 0\n0\n");
  fprintf(tmpparfile,"1\n0\n0\n0\n0\n0\n0\n0\n");
  fprintf(tmpparfile,"0\n0\n2 2 11 11\n");
  fprintf(tmpparfile,"1 1 3 3\n1 1 7 7\n");
  fprintf(tmpparfile,"1 1 7 7\n1 1 3 3\n");

  fclose(tmpparfile);

  return filename;
}

/*--------------------------------------------------------------------------*/
int MediaDB::DBEncodeMPEG4Video(int use_xml, char *xmlfile, 
	char *src_dir, char *sim_dir, int numvos, int frames,
	int output_rate)

{ 
  return EncodeNamedMPEG4Video(use_xml, xmlfile, 
				src_dir, sim_dir, 
			       	numvos, frames, 
				output_rate);	
}

/*--------------------------------------------------------------------------*/
int MediaDB::EncodeNamedMPEG4Video(int use_xml, char *xmlfile, 
	char *src_dir, char *sim_dir, int numvos, int frames,
	int output_rate)
{
  char command[1024];
  char tmpstr[1024];
  int i;
 
  printf("Encoding MPEG4 video ... \n");
  printf(" use_xml: %d \n", use_xml);
  printf(" xmlfile: %s \n", xmlfile);
  printf(" src_dir: %s \n", src_dir);
  printf(" sim_dir: %s \n", sim_dir);

  /* extract vectors from sequence*/
  command[511]=0;

  sprintf(command,"xmpg4enc -n %d -e %d ", numvos, frames-1);

  if (use_xml==1) {
	/* re-encode using extracted transcoding hints */
	sprintf(tmpstr,"-c 2 -r %d -x %s -b ", output_rate, xmlfile);
	strcat(command, tmpstr);
	for(i=0; i<numvos; i++) {
	  sprintf(tmpstr,"enc_%d.bits.reenc ",i);
	  strcat(command, tmpstr);
	}
  } else {
	/* just to extract difficulty hint, no bitstream output */
	sprintf(tmpstr,"-c 0 -i 1 -p 1 -b ");
	strcat(command, tmpstr);
	for(i=0; i<numvos; i++) {
	  sprintf(tmpstr,"enc_%d.bits.q1 ",i);
	  strcat(command, tmpstr);
	}
  }

#ifdef DEBUG
  //  printf("### %s #### \n",command); //sth to stderr
#endif
  
  if (system(command)) {
#ifndef WIN32
    if (system("xmpg4enc | grep MPEG7XM >| /dev/null")) {
      fprintf(stderr,"Error: XM does not call xmpg4enc ");
    }
#else
    fprintf(stderr,"Error: Check if the xmpg4enc from MPEG-7 XM is "
	    "called\n");
#endif
    return 0;
  }

  return 1;
}

/*--------------------------------------------------------------------------*/
void MediaDB::SetImageScaling(int ScalingFormat)

{
  m_ImageScaling=ScalingFormat;
}

/*--------------------------------------------------------------------------*/
MomVop *MediaDB::DBLoadImage()

{
  if (pos<0) pos=0;
  if (pos>=entries) return 0;

  incremented=1;
  return LoadNamedImage(DataBase[pos++].GetFilename());
}

/*--------------------------------------------------------------------------*/
MomVop *MediaDB::LoadNamedImage(char *imagefilename)

{
  unsigned int xsize,ysize;
  MomVop *vop;

  /* free old media before replacing with new meda*/
  if (Media) {
    MomVop *oldvop=Media->GetImage();
    if (oldvop) {
      Media->FreeImage();
    }
  }

#ifdef RANDOMIMAGE
  xsize=8;
  ysize=8;
  vop=initvop(xsize,ysize,_444_,UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE);
	    
  { /*load random values in example to file*/
    int pos;
    unsigned char *pixelval;
    for(pos=0,pixelval=(unsigned char *)vop->a_chan->f;
	pos< (64); *(pixelval++)=(unsigned char)rand()%64,pos++);
    for(pos=0,pixelval=(unsigned char *)vop->y_chan->f;
	pos< (64); *(pixelval++)=(unsigned char)rand()%64,pos++);
    for(pos=0,pixelval=(unsigned char *)vop->u_chan->f;
	pos< (64); *(pixelval++)=(unsigned char)rand()%64,pos++);
    for(pos=0,pixelval=(unsigned char *)vop->v_chan->f;
	pos< (64); *(pixelval++)=(unsigned char)rand()%64,pos++);
  }
#else //RANDOMIMAGE

  MomImageType buf_type;
  TChan bitfilter;
  MomImageData r,g,b,a;
  int sr,sg,sb,sa;        // size of pixel for the buffers
  XMMAGICKPIXELTYPE *pixpacket;
  unsigned long pixels;
  unsigned long totalpix=0;
  int minalpha=255,
    maxalpha=0;
  Image *l_ResizedImage;
#ifdef _XMUSEMAGICK4_
  unsigned long packet;
#else
  ExceptionInfo errorinfo;
#endif

#ifdef _XMUSEMAGICK4_
#else
  GetExceptionInfo(&errorinfo);
#endif

  /* allocate image and image info buffer*/
  if (DecodedImage) {
    DestroyImages((Image *)DecodedImage);
    DecodedImage=0;
  }
  if (!DecodedImageInfo) DecodedImageInfo=(void *)CloneImageInfo(0);

  if (!imagefilename) return 0;
  if (!imagefilename[0]) return 0;

  /* put filename to Media*/
  if (Media) Media->SetImageFileName(imagefilename);

  /* test if input image is readable*/
  {
    /* remove additional info about file in ending []*/
    char *a=0;
    if (imagefilename[strlen(imagefilename)-1]==']') {
      a=strrchr(imagefilename,'[');
      if (a) *a=0;
    }

    /* test open file*/
    int inputfile=open(imagefilename,O_RDONLY);
    if (a) *a='[';
    if (inputfile==-1) return 0;
    close(inputfile);
  }

  //#ifdef DEBUG
  fprintf(stderr,"Loading : %s",imagefilename);
  fflush(stderr);
  //#endif

  /* set parameters for loading*/
  //#ifdef _XMUSEMAGICK4_
  GetImageInfo((ImageInfo *)DecodedImageInfo);
  //#endif
  strcpy(((ImageInfo *)DecodedImageInfo)->filename,imagefilename);
//  (ImageInfo *)(DecodedImageInfo)->colorspace=YCbCrColorspace;
//  ((ImageInfo *)DecodedImageInfo)->colorspace=RGBColorspace;
//  ((ImageInfo *)DecodedImageInfo)->depth=16;
//  ((ImageInfo *)DecodedImageInfo)->depth=16;

  /* load image form file*/
#ifdef _XMUSEMAGICK4_
  DecodedImage=(void *)ReadImage((ImageInfo *)DecodedImageInfo);
#else
  DecodedImage=(void *)ReadImage((ImageInfo *)DecodedImageInfo,&errorinfo);
#endif
  if (DecodedImage == NULL) {
#ifdef _XMUSEMAGICK53_
    fprintf(stderr,"(%s: %s)\n",errorinfo.reason,errorinfo.description);
#else
#ifdef _XMUSEMAGICK5_
    fprintf(stderr,"(%s)\n",errorinfo.message);
#else
    fprintf(stderr," could not load %s\n",imagefilename);
#endif
#endif
    return 0;
  }

  l_ResizedImage=0;
#ifndef _XMUSEMAGICK4_
  {
    int doresize=0;

    if (m_ImageScaling) {
      unsigned long columns,rows;
      double scale1,scale2;
      columns=((Image *)DecodedImage)->columns;
      rows=((Image *)DecodedImage)->rows;

      switch (m_ImageScaling | MAXSIZEMASK) {
      case MAXSIZE1024x768:
	scale1=1024.0/(double)columns;
	scale2=768.0/(double)rows;
	if (scale1 > scale2) scale1=scale2;
	if (scale1 < 1.0) {
	  doresize=1;
	  columns=(unsigned long)(columns*scale1+0.5);
	  rows=(unsigned long)(rows*scale1+0.5);
	}
	break;
      case MAXSIZE800x600:
	scale1=800.0/(double)columns;
	scale2=600.0/(double)rows;
	if (scale1 > scale2) scale1=scale2;
	if (scale1 < 1.0) {
	  doresize=1;
	  columns=(unsigned long)(columns*scale1+0.5);
	  rows=(unsigned long)(rows*scale1+0.5);
	}
	break;
      case MAXSIZE640x480:
	scale1=640.0/(double)columns;
	scale2=480.0/(double)rows;
	if (scale1 > scale2) scale1=scale2;
	if (scale1 < 1.0) {
	  doresize=1;
	  columns=(unsigned long)(columns*scale1+0.5);
	  rows=(unsigned long)(rows*scale1+0.5);
	}
	break;
      case MAXSIZE352x288:
	scale1=352.0/(double)columns;
	scale2=288.0/(double)rows;
	if (scale1 > scale2) scale1=scale2;
	if (scale1 < 1.0) {
	  doresize=1;
	  columns=(unsigned long)(columns*scale1+0.5);
	  rows=(unsigned long)(rows*scale1+0.5);
	}
	break;
      }

      switch (m_ImageScaling | MINSIZEMASK) {
      case MINSIZE352x288:
	scale1=352.0/(double)columns;
	scale2=288.0/(double)rows;
	if (scale1 > scale2) scale1=scale2;
	if (scale1 > 1.0) {
	  doresize=1;
	  columns=(unsigned long)(columns*scale1+0.5);
	  rows=(unsigned long)(rows*scale1+0.5);
	}
	break;
      case MINSIZE128x128:
	scale1=128.0/(double)columns;
	scale2=128.0/(double)rows;
	if (scale1 > scale2) scale1=scale2;
	if (scale1 > 1.0) {
	  doresize=1;
	  columns=(unsigned long)(columns*scale1+0.5);
	  rows=(unsigned long)(rows*scale1+0.5);
	}
	break;
      }

      if (doresize) {
#ifdef DEBUG
	fprintf(stderr,"scaled to %dx%d pixels\n",columns,rows);
#endif
	l_ResizedImage=ZoomImage((Image *)DecodedImage,columns,rows,&errorinfo);
      }
    }
  }
#endif

  if (!l_ResizedImage) {
    l_ResizedImage =(Image *)DecodedImage;
  }

  /* allocate MomVop buffers*/
  xsize=l_ResizedImage->columns;
  ysize=l_ResizedImage->rows;
  totalpix=xsize*ysize;

  if (l_ResizedImage->depth > 8) {
    buf_type=SHORT_TYPE;
    bitfilter=65535;
  }
  else {
    buf_type=UCHAR_TYPE;
    bitfilter=255;
  }
  if (l_ResizedImage->colorspace == GRAYColorspace) {
    vop=initvop(xsize,ysize,_400_,buf_type,buf_type,buf_type,buf_type);
  }
  else {
    vop=initvop(xsize,ysize,_444_,buf_type,buf_type,buf_type,buf_type);
  }
  if (!vop) {
    fprintf(stderr,"\n");
    return 0;
  }

    // KW added 2014
  vop->scene = l_ResizedImage->scene;

  /* copy data from DecodedImage to Vop*/
  if (vop->y_chan) g.u=vop->y_chan->data->u;
  else g.u=0;
  sg=datasize(vop->y_chan);
  if (vop->u_chan) b.u=vop->u_chan->data->u;
  else b.u=0;
  sb=datasize(vop->u_chan);
  if (vop->v_chan) r.u=vop->v_chan->data->u;
  else r.u=0;
  sr=datasize(vop->v_chan);
  if (vop->a_chan) a.u=vop->a_chan->data->u;
  else a.u=0;
  sa=datasize(vop->a_chan);

  /* range of alpha values*/
  if (l_ResizedImage->matte) {
    minalpha=255;
    maxalpha=0;
  }
  else {
    minalpha=255;
    maxalpha=255;
  }

#ifdef _XMUSEMAGICK4_
  pixpacket=l_ResizedImage->pixels;
  for (packet=0; packet < l_ResizedImage->packets; packet++) {
    for (pixels=0; pixels <= pixpacket->length; pixels++) {
#else
  {
    unsigned long packets;
    packets = l_ResizedImage->rows * l_ResizedImage->columns;
    pixpacket=GetImagePixels(l_ResizedImage,0,0,
			     l_ResizedImage->columns,
			     l_ResizedImage->rows);
    for (pixels=0; pixels < packets; pixels++,pixpacket++) {
#endif

      if (g.u) {
	setdata(g,buf_type,((TChan)(pixpacket->green))&bitfilter);
	g.u+=sg;
      }
      if (b.u) {
	setdata(b,buf_type,((TChan)(pixpacket->blue))&bitfilter);
	b.u+=sb;
      }
      if (r.u) {
	setdata(r,buf_type,((TChan)(pixpacket->red))&bitfilter);
	r.u+=sr;
      }
      if (a.u) {
// 	if (!((Image *)(DecodedImage)->matte) ||
// 	    (pixpacket->green == DecodedImage->matte_color.green) &&
// 	    (pixpacket->blue == DecodedImage->matte_color.blue) &&
// 	    (pixpacket->red == DecodedImage->matte_color.red)) {
// 	  *a.u=0;
	if (l_ResizedImage->matte) {
	  setdata(a,buf_type,((TChan)pixpacket->XMMAGICKALPHAFIELD)&bitfilter);
	  if (minalpha>pixpacket->XMMAGICKALPHAFIELD) {
	    minalpha=pixpacket->XMMAGICKALPHAFIELD;
	  }
	  if (maxalpha<pixpacket->XMMAGICKALPHAFIELD) {
	    maxalpha=pixpacket->XMMAGICKALPHAFIELD;
	  }
	}
	else {
	  setdata(a,buf_type,(TChan)255);
	}
	a.u+=sa;
      }

      totalpix--;
      if (!totalpix) break; /* to treate stacked images*/
    }
#ifdef _XMUSEMAGICK4_
    if (!totalpix) break;   /* to treate stacked images*/
    pixpacket++;
#endif
  }

  /* destroy resized image if it was allocated*/
  if (l_ResizedImage!=DecodedImage) {
    DestroyImages((Image *)l_ResizedImage);
  }


  if (Media) {
    Media->SetMinAlpha(minalpha);
    Media->SetMaxAlpha(maxalpha);
  }

  /* load alpha channel if file exists*/
  if (LoadAlphaImage(imagefilename,vop->a_chan) &&
      !((Image *)(DecodedImage))->matte) {
    /* remove alpha chanel if there is no segment mask*/
    freeimg(vop->a_chan);
    vop->a_chan=0;
  }
  fprintf(stderr,"\n");

#ifdef XYUVDISP
  /* display alpha mask of input image*/
  if (vop->a_chan) {
    xyuv_show_simple(1,(unsigned short *)vop->a_chan->data->s,
		     (unsigned short *)0,(unsigned short *)0,
		     datasize(vop->a_chan)&1,
		     vop->a_chan->x,vop->a_chan->y,
		     0,0,"input-alpha");
  }
#endif

  if (Comment) delete [] Comment;
  Comment=0;
#ifdef _XMUSEMAGICK4_
  if (((Image *)DecodedImage)->comments) {
    char *comment=((Image *)DecodedImage)->comments;
    int commentlength=strlen(comment);
#else
  if (GetImageAttribute(((Image *)DecodedImage),"Comment")) {
    char *comment=(GetImageAttribute(((Image *)DecodedImage),"Comment"))->
      value;
    int commentlength=strlen(comment);
#endif
    if (commentlength) {
      Comment = new char [commentlength+1];
      Comment[commentlength]=0;
      strncpy(Comment,comment,commentlength);
    }
  }

  /* put comment to Media*/
  if (Media) {
    Media->SetImageComment(Comment);
    Media->SetImageTime(GetFrameNoFromComment());
  }

#endif //  RANDOMIMAGE
  if (Media) Media->SetImage(vop);

#ifdef _DL_FLOAT_TEST
  float ftesta=1.0;
  float ftestb=2.0;
  ftesta=ftesta+ftestb;
  if (ftesta != 3.0) {
    fprintf(stderr,"Warning: dynamic link problem with float operations"
	    " (MediaIO)\n");
    ftesta=1.0;
    ftestb=ftesta;
    ftestb=2.0;
    ftesta=ftesta+ftestb;
    if ((ftesta - 2.9) <= 0.2) {
      fprintf(stderr,"         Hope Initalization was OK!\n");
    }
  }
#endif

  return vop;

}

/*--------------------------------------------------------------------------*/
int MediaDB::StoreNamedImage(char *imagefilename)

{
  if (DecodedImage) {
#ifndef RANDOMIMAGE
    strcpy(((Image *)DecodedImage)->filename,imagefilename);
#ifdef DEBUG
    fprintf(stderr,"Storing : %s\n",imagefilename);
#endif
    if (WriteImage((ImageInfo *)DecodedImageInfo,(Image *)DecodedImage)) 
      return 0;
    return 1;
#else //RANDOMIMAGE
    return 0;
#endif //RANDOMIMAGE
  }
  return 1;
}

/*--------------------------------------------------------------------------*/
int MediaDB::StoreNamedVop(char *imagefilename, MomVop *vop)

{
#ifndef RANDOMIMAGE
  MomImageType a_type,y_type,u_type,v_type;
  MomImageData r,g,b,a;
  int sr,sg,sb,sa;        // size of pixel for the buffers
  Image *OutImage;
  ImageInfo OutImage_Info;
  XMMAGICKPIXELTYPE *pixpacket;
  unsigned long packets,packet;
  unsigned int *matte;
  int returnvalue;

  /*
    Allocate image structure.
  */
  GetImageInfo(&OutImage_Info);
  OutImage=AllocateImage(&OutImage_Info);
  if (OutImage == (Image *) NULL)
    MagickError(ResourceLimitError,"Unable to display image",
      "Memory allocation failed");
  /*
    Initialize image attributes.
  */
  OutImage->columns=vop->width;
  OutImage->rows=vop->height;
  packets=
#ifdef _XMUSEMAGICK4_
    OutImage->packets=
#endif
    OutImage->columns*OutImage->rows;
#ifdef _XMUSEMAGICK4_
  pixpacket=OutImage->pixels=
    (XMMAGICKPIXELTYPE *)malloc(packets*sizeof(XMMAGICKPIXELTYPE));
#else
  pixpacket=SetImagePixels(OutImage,0,0,vop->width,vop->height);
#endif

  if (pixpacket == (XMMAGICKPIXELTYPE *) NULL)
    MagickError(ResourceLimitError,"Unable to store image",
		"Memory allocation failed");

  /* set pointers to Vop data*/
  if (vop->y_chan) {
    g.u=vop->y_chan->data->u;
    sg=datasize(vop->y_chan);
    y_type=vop->y_chan->type;
  }
  else {
    g.u=0;
    sg=0;
    y_type=UCHAR_TYPE; //to avoid compiler warnings
  }
  sg=datasize(vop->y_chan);

  if (vop->u_chan) {
    b.u=vop->u_chan->data->u;
    sb=datasize(vop->u_chan);
    u_type=vop->u_chan->type;
  }
  else {
    b.u=g.u;
    sb=sg;
    u_type=y_type;
  }
  if (vop->v_chan) {
    r.u=vop->v_chan->data->u;
    sr=datasize(vop->v_chan);
    v_type=vop->v_chan->type;
  }
  else {
    r.u=g.u;
    sr=sg;
    v_type=y_type;
  }

  matte=(unsigned int *)&(OutImage->matte);
  if (vop->a_chan) {
    a.u=vop->a_chan->data->u;
    a_type=vop->a_chan->type;
    *matte=1;
  }
  else {
    a.u=0;
    *matte=0;
    a_type=UCHAR_TYPE; //to avoid compiler warnings
  }
  sa=datasize(vop->a_chan);

  /* copy data from DecodedImage to Vop*/
  for (packet=0; packet < packets; packet++, pixpacket++) {


#ifdef _XMUSEMAGICK4_
    pixpacket->length=0;
#endif
    if (g.u) {
      pixpacket->green=getdata(g,y_type);
      pixpacket->blue=getdata(b,u_type);
      pixpacket->red=getdata(r,v_type);
      g.u+=sg;
      b.u+=sb;
      r.u+=sr;
      if (a.u) {
	pixpacket->XMMAGICKALPHAFIELD=getdata(a,a_type);
// 	if (*a.u) {
//	  pixpacket->index=1;
// 	}
// 	else {
// 	  pixpacket->index=0;
// 	}
	a.u+=sa;
      }
    }
  }

#ifdef _XMUSEMAGICK4_
#else
  if (!SyncImagePixels(OutImage))
    MagickError(ResourceLimitError,"Unable to store image",
		"Syncing Cache failed");
#endif

  /* write image to file*/
  if (OutImage) {
    int writeerror=0;

    strcpy(OutImage->filename,imagefilename);
#ifdef DEBUG
    fprintf(stderr,"Storing : %s",imagefilename);
#endif
    if ((writeerror=WriteImage(&OutImage_Info,OutImage))) {
      returnvalue=0;
#ifdef DEBUG
      fprintf(stderr," OK\n");
#endif
    }
    else {
      returnvalue=1;
#ifdef DEBUG
      fprintf(stderr," failed (Err %d)\n",writeerror);
#endif
    }
  }
  else {
    returnvalue=1;
  }

  /* destroy Magick Image*/
  DestroyImages(OutImage);

  return returnvalue;
#else //RANDOMIMAGE
  return 0;
#endif //RANDOMIMAGE

}

/*--------------------------------------------------------------------------*/
int MediaDB::DeleteImage()

{
  if (DecodedImage) {
#ifndef RANDOMIMAGE
    DestroyImages((Image *)DecodedImage);
#endif //RANDOMIMAGE
    DecodedImage=0;
  }
  return 0;
}

/*--------------------------------------------------------------------------*/
MediaThreeD *MediaDB::DBLoad3DMesh()

{
  if (pos<0) pos=0;
  if (pos>=entries) return 0;
  char *name = DataBase[pos++].GetFilename();
   
  return LoadNamed3DMesh(name);		
}

/*--------------------------------------------------------------------------*/
MediaThreeD *MediaDB::LoadNamed3DMesh(char *meshfilename)

{
  //sth  unsigned int NoObj, NoV, NoT;
  MediaThreeD *mesh=0;
  mesh = Load3DMesh(meshfilename);
  return mesh;
}

/*--------------------------------------------------------------------------*/
int MediaDB::DBLoadKeyPointList()

{
  if (pos<0) pos=0;
  if (pos>=entries) return 0;

  incremented=1;
  return LoadNamedKeyPointList(DataBase[pos++].GetFilename());
}

/*--------------------------------------------------------------------------*/
int MediaDB::LoadNamedKeyPointList(char *KeyPointListFilename)
{
  int first=0, last=INT_MAX;
  char filename[MAXMEDIANAMELEN];
  char *info;
  int KeyPointFile;
  TnDKeyPoint keypoint;
  int prevtime=0x80008080;
  char line[512];
  int linelen;
  int keypoints=0;
  int maxdimensions=0;

  if (!Media) {
    return 0;
  }
  strcpy(filename,KeyPointListFilename);

  if (!filename) {
    return 0;
  }
  if (!filename[0]) {
    return 0;
  }

  /* put filename to Media*/
  Media->SetKeyPointFileName(KeyPointListFilename);

  /* clean existing key point list*/
  Media->FreeKeyPointLists();

  /* extract frame range info from filename*/
  if (filename[strlen(filename)-1]==']') {
    info=strrchr(filename,'[');
    if (info) {
      *info=0; /* cut filename*/
      info++; /* goto info start*/
    }
    sscanf(info,"%d-%d]",&first,&last);
  }

  /* open pointer file*/
  KeyPointFile=open(filename,O_RDONLY);

  /* read all lines*/
  while (KeyPointFile!=-1) {

    /* for counting dimensions*/
    int whitespace =1;

    keypoint.dim=0;

    /* read line*/
    linelen=0;
    line[511]=0;
    while (linelen<511) {
      if (!read(KeyPointFile,&line[linelen],1)) {
	line[linelen]=0;
	close(KeyPointFile);
	KeyPointFile=-1;
	break;
      }
      if (!strchr("\t ",line[linelen])) {
	if (whitespace) {
	  keypoint.dim++;
	  whitespace=0;
	}
      }
      else {
	  whitespace=1;
      }
      if (line[linelen]=='\n') {
	line[linelen]=0;
	break;
      }
      linelen++;
    }

    /* if line not empty added key point to list for timepoint*/
    if (keypoint.dim) {

      char *token;
      int i;

      /* read first colums = time*/
      //sscanf(line,"%10d ", &(keypoint.time));
      token=strtok(line,"\t ");
      token=strtok(NULL,"\t ");
      if (token==NULL) { break; }
      keypoint.time=atoi(token);

      /* check if time is out of range*/
      if ((keypoint.time<first) || (keypoint.time>last)) continue;

      /* decrement dimension and write value to key point*/
      keypoint.dim--;

      if (keypoint.dim>maxdimensions) {
	if (maxdimensions) delete [] keypoint.p;
	keypoint.p=new float [keypoint.dim];
	if (!keypoint.p) {
	  keypoint.dim=0;
	  maxdimensions=0;
	  continue;
	}
      }

      /* read values for the dimensions*/
      i=0;
      while (i <keypoint.dim) {

#ifdef DEBUG
	if (!token) fprintf(stderr,"INTERNAL ERROR: in "
			      "LoadNamedKeyPointList\n");
#endif
	token=strtok(NULL,"\t ");
	if (token==NULL) { break; }
	keypoint.p[i]=atof(token);
	//token=strtok(token,"\t ");
	//scanf(line,"%f", &(keypoint.p[i]));
	i++;
      }
    }
    else {
      keypoint.dim=0;
      keypoint.p=0;
    }

    /* add new list for time point */
    if (keypoint.time != prevtime) {
      Media->AddKeyPointsList(keypoint.time);
      prevtime=keypoint.time;
    }
    /* add key point*/
    Media->AddKeyPoint(&keypoint);
    keypoints++;
  }

  /* delete dimension value memory*/
  if (maxdimensions) delete [] keypoint.p;

  return keypoints;
}

//sth please remove this when moved to LoadNamedKeyPointList
 /*--------------------------------------------------------------------------*/
int MediaDB::DBLoadKeyPointListSylvie()

{
  if (pos<0) pos=0;
  if (pos>=entries) return 0;

  return LoadNamedKeyPointListSylvie(DataBase[pos++].GetFilename());
}

/*--------------------------------------------------------------------------*/
int MediaDB::LoadNamedKeyPointListSylvie(char *KeyPointListFilename)
{
  int first=0, last=INT_MAX;
  char filename[MAXMEDIANAMELEN];
  char *info;
  int KeyPointFile;
  T4DKeyPoint *keypoint;
  char line[512];
  int linelen;
  int keypoints=0;

  if (!Media) {
    return 0;
  }
  strcpy(filename,KeyPointListFilename);

  if (!filename) {
    return 0;
  }
  if (!filename[0]) {
    return 0;
  }

  //sth cout << "Filename: " << filename << "\n";

  /* put filename to Media*/
  Media->SetKeyPointFileName(KeyPointListFilename);

  /* extract frame range info from filename*/
  if (filename[strlen(filename)-1]==']') {
    info=strrchr(filename,'[');
    if (info) {
      *info=0; /* cut filename*/
      info++; /* goto info start*/
    }
    sscanf(info,"%d-%d]",&first,&last);
  }

  /* open pointer file*/
  KeyPointFile=open(filename,O_RDONLY);

  /* calculate keypoints number */
  while (KeyPointFile!=-1) {

    /* read line*/
    linelen=0;
    line[511]=0;
    while (linelen<511) {
      if (!read(KeyPointFile,&line[linelen],1)) {
	close(KeyPointFile);
	KeyPointFile=-1;
	break;
      }
      if (line[linelen]=='\n') {
	line[linelen]=0;
	break;
      }
      linelen++; //added by SJ, August 22: BUG FIX
    }

    /* if line not empty added key point to list for timepoint*/
    if (linelen) {
      keypoints++;
    }
  }

  // 2013/03/04 fix - move this line after keypoints are set
  /* put number of keypoints to Media*/
  //Media->SetKeyPointNumber(keypoints);

  /* check if keypoint list must be create newly*/
  keypoint= Media->GetKeyPointPointer();
  if (keypoints != Media->GetKeyPointNumber()) {

    /* delete old keypoints if they exist*/
    if (keypoint) delete [] keypoint;
    
    /* create new key point list*/
    keypoint = new T4DKeyPoint[keypoints];
    if (!keypoint) return(-1);
  }

  T4DKeyPoint *current_keypoint;
  current_keypoint = keypoint;

  /* read values */
  KeyPointFile=open(filename,O_RDONLY);
  while (KeyPointFile!=-1) {

    /* read line*/
    linelen=0;
    line[511]=0;
    while (linelen<511) {
      if (!read(KeyPointFile,&line[linelen],1)) {
	close(KeyPointFile);
	KeyPointFile=-1;
	break;
      }
      if (line[linelen]=='\n') {
	line[linelen]=0;
	break;
      }
      linelen++; //added by SJ, August 22: BUG FIX
    }

    /* if line not empty added key point to list for timepoint*/
    if (linelen) {

      sscanf(line,"%10d %10f %10f %10f", &(current_keypoint->time),
	     &(current_keypoint->x_position),&(current_keypoint->y_position),
	     &(current_keypoint->z_position));

      // removed by SJ, Aug 22: our file names are not named this way => first/last not known
      // if ((keypoint->time<first) || (keypoint->time>last)) continue;

      current_keypoint++;
    }
  }

  /* put number of keypoints to Media*/
  Media->SetKeyPointNumber(keypoints); // fix - moved from line 1988
  /* put pointer on keypoints to Media*/
  Media->SetKeyPointPointer(keypoint);
  
  return keypoints;
}

/*--------------------------------------------------------------------------*/
// unsigned long MediaDB::DBLoadPointerList()

// {
//   if (pos<0) pos=0;
//   if (pos>=entries) return 0;

//   incremented=1;
//   return LoadNamedPointerList(DataBase[pos++].GetFilename());
// }

// /*--------------------------------------------------------------------------*/
// unsigned long MediaDB::LoadNamedPointerList(char *PointerFilename)

// {
//   char a;
//   unsigned long lines=0;
//   unsigned long first, last;
//   char filename[MAXMEDIANAMELEN];
//   char *info;
//   int PointerFile;


//   strcpy(filename,PointerFilename);

//   if (!filename) {
//     return 0;
//   }

//   /* extract frame range info from filename*/
//   if (filename[strlen(filename)-1]==']') {
//     info=strrchr(filename,'[');
//     if (info) {
//       *info=0; /* cut filename*/
//       info++; /* goto info start*/
//     }
//     sscanf(info,"%lu-%lu]",&first,&last);
//     return last-first+1;
//   }

//   /* else count line in the file*/

//   /* open pointer file*/
//   PointerFile=open(filename,O_RDONLY);
//   if (PointerFile==-1) return 0;

//   /* goto frame number*/
//   while (read(PointerFile,&a,1)) {
//     if (a=='\n') lines++;
//   }
//   if (a!='\n') { /* does not end with '\n'*/
//     lines++;
//   }

//   close(PointerFile);
//   if (!lines) lines=1;
//   return lines;
// }

// /*--------------------------------------------------------------------------*/
// T4DKeyPoint *MediaDB::DBLoadPointer(int ImageNumber)

// {
//   int currentpos;

//   if (incremented) {
//     currentpos=pos-1;
//   }
//   else {
//     currentpos=pos;
//   }

//   if (currentpos<0) {
//     fprintf(stderr,"Warning: No pointer list selected\n"
// 	    "Warning: loading first database entry\n");
//     currentpos=0;
//     pos=1;
//   }
//   if (currentpos>=entries) return 0;

//   return LoadNamedPointer(DataBase[currentpos].GetFilename(),ImageNumber);
// }

// /*--------------------------------------------------------------------------*/
// T4DKeyPoint *MediaDB::LoadNamedPointer(char *PointerFilename,
// 					   int ImageNumber)

// {
//   char filename[MAXMEDIANAMELEN];
//   char *info;
//   int PointerFile;
//   char line[512];
//   int linelen;
//   T4DKeyPoint *keypoint;

//   strcpy(filename,PointerFilename);

//   if (!filename) {
//     return 0;
//   }

//   /* extract frame range info from filename*/
//   if (filename[strlen(filename)-1]==']') {
//     info=strrchr(filename,'[');
//     if (info) {
//       *info=0; /* cut filename*/
//       info++; /* goto info start*/
//       info[strlen(info)-1]=0; /* remove closing bracket*/
//     }
//   }
//   else {
//     info=0;
//   }
//   ImageNumber+=atoi(info)-1;

//   /* open pointer file*/
//   PointerFile=open(filename,O_RDONLY);
//   if (PointerFile==-1) return 0;

//   /* goto frame number*/
//   while (ImageNumber) {
//     if (!read(PointerFile,line,1)) {
//       close(PointerFile);
//       return 0;
//     }
//     if (line[0]=='\n') ImageNumber++;
//   }

//   /* extract pointer from file*/
//   linelen=0;
//   while (linelen<511) {
//     if (!read(PointerFile,&line[linelen],1)) {
//       /* check if file ends with '\n'*/
//       if (!linelen) {
// 	close(PointerFile);
// 	return 0;
//       }
//       break;
//     }
//     linelen++;
//   }
//   line[linelen]=0;
//   close(PointerFile);

//   /* extarct values from file*/
//   keypoint = new T4DKeyPoint;
  
//   sscanf(line,"%10d %10f %10f %10f\n", &(keypoint->time),
// 	 &(keypoint->x_position),&(keypoint->y_position),
// 	 &(keypoint->z_position));


//   return keypoint;
// }

/*--------------------------------------------------------------------------*/
char *MediaDB::GetMediaFilename()

{
  int currentpos;

  if (incremented) {
    currentpos=pos-1;
  }
  else {
    currentpos=pos;
  }

  if (currentpos<0) {
    fprintf(stderr,"Warning: No pointer list selected\n"
	    "Warning: loading first database entry\n");
    currentpos=0;
    pos=1;
  }
  if (currentpos>=entries) return 0;

  return DataBase[currentpos].GetFilename();
}

/*--------------------------------------------------------------------------*/
char *MediaDB::GetImageComment()

{
  return Comment;
}

/*--------------------------------------------------------------------------*/
unsigned long MediaDB::GetFrameNoFromComment()

{
  char *a;

  if (!Comment) return (unsigned long) -1;

  a=strstr(Comment,"FrameNo:");
  if (!a) return (unsigned long) -1;
  a+=8;    // add size of keyword "FrameNo:"
  return (unsigned long) atol(a);
}

/*--------------------------------------------------------------------------*/
unsigned long MediaDB::GetCodingTypeFromComment()

{
  char *a;

  if (!Comment) return (unsigned long) -1;

  a=strstr(Comment,"CodingType:");
  if (!a) return (unsigned long) -1;
  a+=11;    // add size of keyword "CodingType:"
  return (unsigned long) atol(a);
}

/*--------------------------------------------------------------------------*/
int MediaDB::DecodeAlphaMPEGVideo(char *MPEGFilename)

{
  char *filename;
  char *info;
  char command[512];

  filename=TestAlphaFile(MPEGFilename,0);

  if (!filename) {
//     unlink("m2v.ppm");
    return 0;
  }
  if (!filename[0]) {
    return 0;
  }

  /* extract frame range info from filename*/
  if (filename[strlen(filename)-1]==']') {
    info=strrchr(filename,'[');
    if (info) {
      *info=0; /* cut filename*/
      info++; /* goto info start*/
      info[strlen(info)-1]=0; /* remove closing bracket*/
    }
  }
  else {
    info=0;
  }

  /* extract vectors from sequence*/
  command[511]=0;
  strcpy(command,"mpeg2decode -o3 m2v/%d_a -b ");
  strncat(command,filename,511-strlen(command));
  if (info) {
    /*add frame range to command line*/
//     strncat(command,"-n ",511-strlen(command));
//     strncat(command,info,511-strlen(command));
//     strncat(command," ",511-strlen(command));
  }
  if (system(command)) {
#ifndef WIN32
    if (system("mpeg2decode | grep MPEG7XM >| /dev/null")) {
      fprintf(stderr,"Error: XM does not call mpeg2dec including "
	      "vector extarction (version comming with XM)\n");
    }
#else
    fprintf(stderr,"Error: Check if the mpeg2dec from MPEG-7 XM is "
	    "called\n");
#endif
    return 0;
  }

  mpegdec_used=1;
  return 1;
}

/*--------------------------------------------------------------------------*/
int MediaDB::LoadAlphaImage(char *ImageFilename,MomImage *alpha)
{
  int minalpha=255,
    maxalpha=0;
  
#ifndef RANDOMIMAGE
  Image *AlphaImage;
  ImageInfo AlphaImageInfo;
  XMMAGICKPIXELTYPE *pixpacket;
  MomImageType buf_type=alpha->type;
  TChan bitfilter;
  MomImageData a;
  int sa;        // size of pixel for the buffers
  char *filename;
  unsigned long pixels;
  unsigned long totalpix=0;
#ifdef _XMUSEMAGICK4_
  unsigned long packet;
#else
  ExceptionInfo errorinfo;
#endif

  filename=TestAlphaFile(ImageFilename,1);
  if (!filename) return 1;
  if (!filename[0]) return 1;
  if (!alpha) return 2;

  /* allocate image and image info buffer*/
  GetImageInfo(&AlphaImageInfo);

#ifdef _XMUSEMAGICK4_
#else
  GetExceptionInfo(&errorinfo);
#endif

  //#ifdef DEBUG
  fprintf(stderr," \t%s",filename);
  //#endif

  /* set parameters for loading*/
  strcpy(AlphaImageInfo.filename,filename);
  AlphaImageInfo.colorspace=RGBColorspace;
//   AlphaImageInfo->depth=16;

  /* load image form file*/
#ifdef _XMUSEMAGICK4_
  AlphaImage=ReadImage(&AlphaImageInfo);
#else
  AlphaImage=ReadImage(&AlphaImageInfo,&errorinfo);
#endif
  if (AlphaImage == NULL) {
    return 1;
  }

  if ((AlphaImage)->depth > 8) {
    bitfilter=65535;
  }
  else {
    bitfilter=255;
  }

  totalpix=alpha->x*alpha->y;

  /* copy data from AlphaImage to Vop*/
  if (alpha) a.u=alpha->data->u;
  else a.u=0;
  sa=datasize(alpha);
  minalpha=255;
  maxalpha=0;

  // if AlphaImage should be scaled
  if ((AlphaImage->columns != alpha->x) ||
      (AlphaImage->rows != alpha->y)) {
    Image *tmpImg;
#ifdef _XMUSEMAGICK4_
    tmpImg=ZoomImage(AlphaImage,alpha->x,alpha->y);
#else
    tmpImg=ZoomImage(AlphaImage,alpha->x,alpha->y,&errorinfo);
#endif
    DestroyImage(AlphaImage);
    AlphaImage=tmpImg;
    if (!AlphaImage) return 1;
  }


  if (a.u) {
#ifdef _XMUSEMAGICK4_
    pixpacket=AlphaImage->pixels;
    for (packet=0; packet < AlphaImage->packets; packet++) {
      for (pixels=0; pixels <= pixpacket->length; pixels++) {
#else
    {
      unsigned long packets;
      packets= AlphaImage->rows*AlphaImage->columns;
      pixpacket=GetImagePixels(AlphaImage,0,0,
			       AlphaImage->columns,
			       AlphaImage->rows);
      for (pixels=0; pixels < packets; pixels++, pixpacket++) {
#endif


	setdata(a,buf_type,((TChan)pixpacket->green)&bitfilter);
	if (minalpha>pixpacket->green) {
	  minalpha=pixpacket->green;
	}
	if (maxalpha<pixpacket->green) {
	  maxalpha=pixpacket->green;
	}
	a.u+=sa;

	totalpix--;
	if (!totalpix) break; /* to treate stacked images*/
      }
#ifdef _XMUSEMAGICK4_
      if (!totalpix) break;   /* to treate stacked images*/
      pixpacket++;
#endif
    }
  }
  DestroyImages(AlphaImage);

#endif //  RANDOMIMAGE
  if (Media) {
    Media->SetMinAlpha(minalpha);
    Media->SetMaxAlpha(maxalpha);
  }

  return 0;
}

/*--------------------------------------------------------------------------*/
char *MediaDB::TestAlphaFile(char *Filename, int TestStdExt)

{
  static char default_ext[3][4]={"ppm","pgm","pbm"};
  char *namestart,*ext,*info;
  int length;
  int i;
  int inputfile;

  if (!Filename || !Filename[0]) return NULL;

  /* allocate alpha filename*/
  if (m_AlphaFilename) {
    delete [] m_AlphaFilename;
  }
  m_AlphaFilename= new char [strlen(Filename)+6];
  if (!m_AlphaFilename) return NULL;

  /* get beginning of filename*/
  namestart=strrchr(Filename,'\\');
  if (!namestart) {
    namestart=Filename;
  }
  else {
    namestart++;
  }
  length=namestart-Filename;

  /* copy path to m_AlphaFilename*/
  strncpy(m_AlphaFilename,Filename,length);
  m_AlphaFilename[length]=0;

  /*get info*/
  if (Filename[strlen(Filename)-1] == ']') {
    info=strrchr(namestart,'[');
    if (info) *info=0;
  }
  else {
    info=0;
  }

  /* get dot of filename extension*/
  ext=strrchr(namestart,'.');
  if (!ext) {
    ext=namestart+strlen(namestart);
  }
  else {
    *ext=0;
  }

  /* append filename to path*/
  strcat(m_AlphaFilename,namestart);

  /* append _a to path*/
  strcat(m_AlphaFilename,"_a");
  length=strlen(m_AlphaFilename);

  /* append extension*/
  if (ext) {
    *ext='.';
    strcat(m_AlphaFilename,ext);
  }      
  if (info) *info='[';

  /* test m_AlphaFilename*/
  inputfile=open(m_AlphaFilename,O_RDONLY);
  if (inputfile!=-1) {
    close(inputfile);
    /* append info*/
    if (info) strcat(m_AlphaFilename,info);
    return m_AlphaFilename;
  }

  /* test default extensions*/
  if (!TestStdExt) return NULL;

  for (i=0; i<3; i++) {
    strcpy(&m_AlphaFilename[length],".");
    strcat(&m_AlphaFilename[length],default_ext[i]);

    /* test m_AlphaFilename*/
    inputfile=open(m_AlphaFilename,O_RDONLY);
    if (inputfile!=-1) {
      close(inputfile);
      /* append info*/
      if (info) strcat(m_AlphaFilename,info);
      return m_AlphaFilename;
    }
  }
  return NULL;
}
