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
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// XM_Main.cpp


//-p ../EdgeHistogram.xml.par -a EdgeHistogramClient -l ..\dbtest.lst -b ..\EHtest.mp7 -q ..\ayamada_59Jeju_Mar02_0001.jpg
//-p ../DominantColor.bin.par -a DominantColorClient -l ..\dbtest.lst -b ..\DCtest.mp7 -q ..\ayamada_59Jeju_Mar02_0001.jpg
//-p ../DominantColor.bin.par -a DominantColorServer -l ..\dbtest.lst -b ..\DCtest.mp7
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RANDOMIMAGE
#include <time.h>
#include <sys/types.h>
#include <magick/api.h>
#if defined(__cplusplus) || defined(c_plusplus)
#undef class
#endif
#if (MagickLibVersion & 0x0f00)==0x0400
#define _XMUSEMAGICK4_
#endif
#if (MagickLibVersion & 0x0ff0)==0x0530
#define _XMUSEMAGICK53_
#endif
#endif

#ifdef USEXML
#include <util/PlatformUtils.hpp>// Initiliase XML.
#include <dom/DOM.hpp>
#endif

/*#include "XMManager.h"
#include "XMManagerDlg.h"
#include "DlgProxy.h"*/

#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif

#include "Version1.h"
//#include "Version2.h"

//#define BITSTREAMTEST
#ifdef BITSTREAMTEST
#include "Encoders/Encoders.h"
#include "Decoders/Decoders.h"
#endif

int main (int argc, char **argv)

{
#ifdef _DL_FLOAT_TEST
  float ftesta=1.0;
  float ftestb=2.0;
  ftesta=ftesta+ftestb;
  if (ftesta != 3.0) {
    fprintf(stderr,"Warning: dynamic link problem with float operations"
	    " (XMMain)\n");
    ftesta=1.0;
    ftestb=ftesta;
    ftestb=2.0;
    ftesta=ftesta+ftestb;
    if ((ftesta - 2.9) <= 0.2) {
      fprintf(stderr,"         Hope Initalization was OK!\n");
    }
  }
#endif


  TXMApplicationParameterSet *ParameterSet;

  //   XM::ComponentDatabase  theComponentDatabase;
  int application_notfound=0;
  int status=0;

#ifdef XYUVDISP
  XYUV_INIT(argc,argv);
#endif

#ifdef BITSTREAMTEST
  {
    XM::EncoderFileIO encStreamBuffer(0x1000);
    XM::DecoderFileIO decStreamBuffer(0x1000);
    unsigned char i1=0xf0,i2=0x65;
    unsigned char o1=0,o2=0;
    long b1=4, b2=7;
    double di=5.5,dout;

    encStreamBuffer.StartBitStream("test.mp7");
//    encStreamBuffer.PutBits((char *)&i1, b1);
//    encStreamBuffer.PutBits((char *)&i2, b2);
    {
      int i;
      for (i=0; i<35; i++) {
	encStreamBuffer.PutBitsLong(i,5);
      }
    }
    encStreamBuffer.CloseBitStream();

    decStreamBuffer.OpenBitStream("test.mp7");
//    decStreamBuffer.GetBits((char *)&o1,b1);
//    decStreamBuffer.TakeBits(b1);
//    decStreamBuffer.GetBits((char *)&o2, b2);
//    decStreamBuffer.TakeBits(b2);
    {
      int i;
      for (i=0; i<35; i++) {
	decStreamBuffer.GetBitsLong(&b2,5);
	decStreamBuffer.TakeBits(5);
	if (b2 != (i&0x1f)) {
	  fprintf(stderr,"b2 = %d ,should be %d\n",b2,i);
	}
      }
    }
    decStreamBuffer.CloseBitStream();

//    if (((i1 & (0xff << (8-b1))) != o1) ||
//      ((i2 & (0xff << (8-b2))) != o2)) {
//      fprintf(stderr,"internal error with bitencoding/decoding\n");
//      fprintf(stderr,"out1 = %d ,should be %d\n",o1,
//        (i1 & (0xff << (8-b1))));
//      fprintf(stderr,"out2 = %d ,should be %d\n",o2,
//        (i2 & (0xff << (8-b2))));
//    }
  }
#endif
#ifdef TESTMAGICK
  ExceptionInfo
    exception;

  Image
    *image;

  ImageInfo
    *image_info;

  /*
    Initialize the image info structure and read an image.
  */
  fprintf(stderr,"loading testset/join.ppm\n");
  InitializeMagick(*argv);
  GetExceptionInfo(&exception);
  image_info=CloneImageInfo((ImageInfo *) NULL);
  (void) strcpy(image_info->filename,"testset/join.ppm");
  image=ReadImage(image_info,&exception);
  if (image == (Image *) NULL)
    MagickError(exception.severity,exception.reason,exception.description);
  fprintf(stderr,"done\n");
#endif

  ParameterSet=parse_cmdline (argc, argv);
  fprintf (stderr, "MPEG-7 XM Version ");
  fprintf (stderr, M7XMVERSION "\n");

  /* Initialize the XML4C system*/
#ifdef USEXML
  try {
    XMLPlatformUtils::Initialize();
  } catch (const XMLException& toCatch) {
    fprintf(stderr,"ERROR(GenericDS): Error during Xerces-c "
	    "Initialization: ");
    fprintf(stderr,(char *)toCatch.getMessage());
    exit(-1);
  }
  fprintf(stderr,"Initializing Xerces Parser\n");
#endif

#ifdef _XMUSEMAGICK53_
#ifndef RANDOMIMAGE
  InitializeMagick(*argv);
  fprintf(stderr,"Initializing ImageMagick\n");
#endif
#endif
	

  if(ParameterSet)
	application_notfound=select_application (ParameterSet);
  
  status=printhelp(ParameterSet,application_notfound);

  if (status== 1) {
    fprintf(stderr,"internal Error: CMDLineParameterSet not found\n");
  }

  if (application_notfound) return 0x80000000;
  return 0;
}
