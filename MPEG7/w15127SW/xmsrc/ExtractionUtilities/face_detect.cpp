////////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by 
// Lifang Wu, Mark Pickering, Lansun  Shen
// Kongqiao Wang, Xin Xing, Kebin Jia
// Jun Ren, Yanqing Wang
// end edited by
// Karol Wnukowicz (Visual Atoms) 
// Stavros Paschalakis (Visual Atoms) 
// in the course of development of the MPEG-7 Standard (ISO/IEC 15938). 
// This software module is an implementation of a part 
// of one or more MPEG-7 Standard tools as specified by the MPEG-7 Standard. 
// ISO/IEC gives users of the MPEG-7 Standard free license to this software
// module or modifications thereof for use in hardware or software products
// claiming conformance to the MPEG-7 Standard. 
// Those intending to use this software module in hardware or software products 
// are advised that its use may infringe existing patents. The original 
// developer of this software module and his/her company, the subsequent 
// editors and their companies, and ISO/IEC have no liability for use of this 
// software module or modifications thereof in an implementation.
// Copyright is not released for non MPEG-7 Standard conforming products. 
// <CN1> retains full right to use the code for his/her own purpose, 
// assign or donate the code to a third party and to inhibit third parties 
// from using the code for non MPEG-7 Standard conforming products. 
// This copyright notice must be included in all copies or derivative works. 
// Copyright (c)2001-2015
//
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// face_detect.cpp: implementation of the Cface_detect class.
//////////////////////////////////////////////////////////////////////

#ifdef _VisualC_
#include <stdafx.h>
#else
#include <stdio.h>
#endif
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "face_detect.h"
#include "Utilities/readparam.h"
#include "vopio.h"
#include "inter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
//#define new DEBUG_NEW
#endif

#ifndef WORD
#define WORD  short
#endif

#ifndef DWORD
#define DWORD  long
#endif


#define facedcutnorm(res,in) \
                          intertrans_proc(res,0,in, \
                          Y_CH|U_CH|V_CH,0,Y_CH|U_CH|V_CH, \
			  SCAN_HO,-1,-1, \
			  0,0,0,0, \
                          &faced_cut_norm_coortrans,&faced_inter_cp2)

extern "C" void faced_cut_norm_coortrans(TCoor width,TCoor height,
					TCoor inx,TCoor iny,
					float *outx,float *outy);
extern "C" void faced_inter_cp2(TPixel *res, TPixel *in1,
				TPixel *in2);

FACE_TRANS_PAR facenormpar;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
Cface_detect::Cface_detect ()
{
  inputvop=0;
  facevop=0;
  inImage = NULL;
  inImage0 = NULL;
  R = NULL;
  G = NULL;
  B = NULL;
  image[0] = NULL;
  image[1] = NULL;
  image[3] = NULL;
  image1 = NULL;
  centerPoints[0] = NULL;
  centerPoints[1] = NULL;
  centerPoints[2] = NULL;

  /* allocate new vop for face*/
  facevop=initvop(46,56,/*size*/
		  _444_, /* color format 4:4:4*/
		  UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE/*buffer types*/);
  if (facevop) {
    freeimg(facevop->a_chan);
    facevop->a_chan=0;
  }
  // 2013/03/05 fix: dynamic allocation of array;
  hist2d=new float*[512];
  for(int i=0; i<512;i++)
  {
	  hist2d[i] = new float[512];
  }
}

//------------------------------------------------------------------------
Cface_detect::~Cface_detect ()
{
  if (facevop) freevop(facevop);
  delete_memory (imgheight, imgwidth);
  // 2013/03/05 fix
  for(int i =0; i<512;i++)
  {
	  if(hist2d[i]) delete hist2d[i];
  }
  delete hist2d;
}



////////////////////////////////////
// my code start here
////////////////////////////////////
//------------------------------------------------------------------------
int Cface_detect::call_FaceDetect (char *input_image_name, MomVop * vop)
{
  int face_num;
  char Pathname[256], Filename[256];
  FILE *fp;

  inputvop=vop;
  delete_memory (imgheight, imgwidth);

  //sth read here the face.txt file!
  strcpy (Pathname, input_image_name);
  strcat (Pathname, ".dir");
  fprintf(stderr,"%s ",Pathname);
  strcpy (Filename, Pathname);
  strcat (Filename, "/faces.txt");
  if ((fp = fopen (Filename, "rb")) != NULL) {
    int num=0;

    if ((1 == fscanf(fp,"%d\n",&face_num)) && (face_num > 0)) {


      while(1) {
	if (4 != (fscanf(fp,"%d\t%d\t%d\t%d\n",
			 &c_s_face[num].left_eye_x,
			 &c_s_face[num].left_eye_y,
			 &c_s_face[num].right_eye_x,
			 &c_s_face[num].right_eye_y))) break;
	if (c_s_face[num].left_eye_x || c_s_face[num].left_eye_y ||
	    c_s_face[num].right_eye_x || c_s_face[num].right_eye_y) {
	  num++;
	}
	if (num > 49) break;
      }
    }
    fclose(fp);
    if (num == face_num) return face_num;
  }
  

  imgwidth = vop->width;
  imgheight = vop->height;

  if (Init_Memory (imgheight, imgwidth) == 0)
    return (0);
  //output detect result
  face_num = FaceDetect (vop, c_s_face);

  /* write face list to file*/
  if ((fp = fopen (Filename, "wb")) != NULL) {
    int num=face_num;
    fprintf(fp,"%d\n",face_num);

    while(num) {

      fprintf(fp,"%d\t%d\t%d\t%d\n",
	       c_s_face[face_num-num].left_eye_x,
	       c_s_face[face_num-num].left_eye_y,
	       c_s_face[face_num-num].right_eye_x,
	       c_s_face[face_num-num].right_eye_y);
      num--;
    }
    fclose (fp);
  }
//  delete_memory (imgheight, imgwidth);//sth unfortunate location
  return (face_num);
}

//------------------------------------------------------------------------
int Cface_detect::face_segmentation (int faceno)
{
  float dx, dy;
  double angle;
  int width;
  float eyesdis, emdis;

  angle =
    atan ((c_s_face[faceno].right_eye_y - c_s_face[faceno].left_eye_y)
	  / (float) (c_s_face[faceno].right_eye_x - 
		     c_s_face[faceno].left_eye_x));
  facenormpar.COS = cos (angle);
  facenormpar.SIN = sin (angle);
  //  angle = 180 * angle / PI;

  dx = c_s_face[faceno].right_eye_x - c_s_face[faceno].left_eye_x;
  dy = c_s_face[faceno].right_eye_y - c_s_face[faceno].left_eye_y;
  eyesdis = sqrt (dx * dx + dy * dy);
  emdis = 1.25 * eyesdis;

  width = (int) (2.0 * eyesdis + 0.5);
  //  height = (int) (2.0 * emdis + 0.5);
  facenormpar.scale=((float)width)/(float)facevop->width;


  facenormpar.leftx =
    (int) ((c_s_face[faceno].right_eye_y - c_s_face[faceno].left_eye_y)
	   * 0.5 * emdis / eyesdis + 
	   1.5 * c_s_face[faceno].left_eye_x -
	   0.5 * c_s_face[faceno].right_eye_x + 0.5);
  facenormpar.lefty =
    (int) ((c_s_face[faceno].left_eye_x -
	    c_s_face[faceno].right_eye_x) * 0.5 * emdis / eyesdis +
	   1.5 * c_s_face[faceno].left_eye_y -
	   0.5 * c_s_face[faceno].right_eye_y + 0.5);

  if (facenormpar.leftx < 0 || facenormpar.lefty < 0) {
    return (0);
  }

  /* cut and normalize face from input image*/
  facedcutnorm(facevop,inputvop);


  /* copy y_chan to u_cahn and v_chan if black and white*/
  //memcpy(facevop->u_chan->data->u,facevop->y_chan->data->u,46*56);
  //memcpy(facevop->v_chan->data->u,facevop->y_chan->data->u,46*56);

  return (1);
}

//------------------------------------------------------------------------
MomVop *Cface_detect::GetFaceImage(int faceno)

{
  if (faceno > 50) return 0; //return error zero-vop if out of bounds
  face_segmentation(faceno);
  return(facevop);
}

//------------------------------------------------------------------------
int Cface_detect::GetFaceParameters(Simple_FACE *s_face,int faceno)

{
  if (faceno > 50) return 1; //return error if out of bounds
  s_face=&c_s_face[faceno];
  return 0;
}

//------------------------------------------------------------------------
void faced_cut_norm_coortrans(TCoor width,TCoor height,
			     TCoor inx,TCoor iny,
			     float *outx,float *outy)
{
  *outx=facenormpar.scale* 
    (float)(-iny * facenormpar.SIN + inx * facenormpar.COS) +
	    facenormpar.leftx;
  *outy=facenormpar.scale* 
    (float)(iny * facenormpar.COS + inx * facenormpar.SIN) +
	    facenormpar.lefty;
}

//------------------------------------------------------------------------
void faced_inter_cp2(TPixel *res, TPixel *in1,TPixel *in2)

{
  res->a=in2->a;
  res->y=in2->y;
  res->u=in2->u;
  res->v=in2->v;
}

//------------------------------------------------------------------------
int Cface_detect::FaceDetect (MomVop * vop, Simple_FACE simple_face[50])
{
  int i, j, k;
//  float time1;
  //      long    time0;  //sth
  float color_th;
  int candidatesnum;
//      FILE    *fp;
  int FaceNum;
  int height, width;
  int out_coarse_txt = 0, out_final_txt = 0, out_final_result = 0;

  width = imgwidth;
  height = imgheight;
//      printf("width,height, %d, %d,",width,height);
//      printf("init-memory end");
  if (input_image (vop) == 0)
    return (0);
//      printf("input-image end");
//initialize conditional pdf of face color
  //time0=clock(); //sth
  initialize_cpdf ();
  candidatesnum = 0;
  color_th = (float) 0.0002;
  colorfacesegment (color_th);
  for (int decide = 0; decide < 2; decide++) {
// extract feature points
    feature_extract (decide);
//label objects
    for (k = 1; k <= 3; k++) {
      number[k - 1] =
	objectcluster (image[k - 1], imgheight, imgwidth, centerPoints[k - 1]);
      if (number[k - 1] == -1)
	return (-1);
    }

//coarse detect facecandicates
    for (k = 0; k < 3; k++) {
      coarsedetectface (k);
    }

    for (i = 0; i < imgheight; i++)
      for (j = 0; j < imgwidth; j++)
	image1[i][j] = inImage[i][j];
    candidatesnum = askcandidates (candidatesnum);
  }

//call confirm_face function
//      printf("candidatesNum=%d",candidatesnum);
  FaceNum = call_confirm (candidatesnum);
//      time1=(clock()-time0)/CLOCKS_PER_SEC;

  if (FaceNum > 0) {
    for (i = 0; i < FaceNum; i++) {
      if (Face[i].Isface == 1) {
	simple_face[i].left_eye_x = Face[i].Le.x;
	simple_face[i].left_eye_y = Face[i].Le.y;
	simple_face[i].right_eye_x = Face[i].Re.x;
	simple_face[i].right_eye_y = Face[i].Re.y;
//	  printf("%d %d %d %d",
//	  simple_face[i].left_eye_x,
//	    simple_face[i].left_eye_y,
//	    simple_face[i].right_eye_x,
//	    simple_face[i].right_eye_y);
      }
    }
  }
  return (FaceNum);
}

int Cface_detect::input_image (MomVop * vop)
{
  unsigned char *Ptemp_char;
  int i, j;

  if ((Ptemp_char = new unsigned char[imgwidth * imgheight]) == NULL)
    return (0);
  if (vop->y_chan) {
    Ptemp_char = vop->y_chan->data->u;
    for (i = 0; i < imgheight; i++)
      for (j = 0; j < imgwidth; j++) {
	G[i][j] = *(Ptemp_char + i * imgwidth + j);
      }
  }
  else {
    delete[]Ptemp_char;
    return (0);
  }
  if (!vop->u_chan && !vop->v_chan) {	// grayimage  R=G=B
    for (i = 0; i < imgheight; i++)
      for (j = 0; j < imgwidth; j++) {
	R[i][j] = G[i][j];
	B[i][j] = G[i][j];
      }
    bitperpixel = imgbitcount = 8;
  }
  else {	//color image
    if (vop->u_chan) {
      Ptemp_char = vop->u_chan->data->u;
      for (i = 0; i < imgheight; i++)
	for (j = 0; j < imgwidth; j++) {
	  B[i][j] = *(Ptemp_char + i * imgwidth + j);
	}
    }
    else {
      delete[]Ptemp_char;
      return (0);
    }
    if (vop->v_chan) {
      Ptemp_char = vop->v_chan->data->u;
      for (i = 0; i < imgheight; i++)
	for (j = 0; j < imgwidth; j++) {
	  R[i][j] = *(Ptemp_char + i * imgwidth + j);
	}
    }
    else {
      delete[]Ptemp_char;
      return (0);
    }
    bitperpixel = imgbitcount = 24;
  }
//  printf("bitperpixel=%d ",bitperpixel);
  return (1);
}

void Cface_detect::delete_memory (int height, int width)
{
  int i;

  if (centerPoints[2] != NULL) {
    delete centerPoints[2];
    centerPoints[2] = NULL;
  }
  if (centerPoints[1] != NULL) {
    delete centerPoints[1];
    centerPoints[1] = NULL;
  }
  if (centerPoints[0] != NULL) {
    delete centerPoints[0];
    centerPoints[0] = NULL;
  }
  if (image[2] != NULL) {
    for (i = 0; i < height; i++) {
      delete image[2][i];
      image[2][i] = NULL;
    }
    delete image[2];
    image[2] = NULL;
  }
  if (image[1] != NULL) {
    for (i = 0; i < height; i++) {
      delete image[1][i];
      image[1][i] = NULL;
    }
    delete image[1];
    image[1] = NULL;
  }
  if (image[0] != NULL) {
    for (i = 0; i < height; i++) {
      delete image[0][i];
      image[0][i] = NULL;
    }
    delete image[0];
    image[0] = NULL;
  }
  if (image1 != NULL) {
    for (i = 0; i < height; i++) {
      delete image1[i];
      image1[i] = NULL;
    }
    delete image1;
    image1 = NULL;
  }
  if (inImage0 != NULL) {
    for (i = 0; i < height; i++) {
      delete inImage0[i];
      inImage0[i] = NULL;
    }
    delete inImage0;
    inImage0 = NULL;
  }
  if (inImage != NULL) {
    for (i = 0; i < height; i++) {
      delete inImage[i];
      inImage[i] = NULL;
    }
    delete inImage;
    inImage = NULL;
  }
  if (B != NULL) {
    for (i = 0; i < height; i++) {
      delete B[i];
      B[i] = NULL;
    }
    delete B;
    B = NULL;
  }
  if (G != NULL) {
    for (i = 0; i < height; i++) {
      delete G[i];
      G[i] = NULL;
    }
    delete G;
    G = NULL;
  }
  if (R != NULL) {
    for (i = 0; i < height; i++) {
      free (R[i]);
      R[i] = NULL;
    }
    free (R);
    R = NULL;
  }
  return;
}


void Cface_detect::delete_Memory_UnChar (unsigned char **Ptr, int height,
  int width)
{
  int i;
  if (Ptr != NULL) {
    for (i = 0; i < height; i++) {
      delete Ptr[i];
      Ptr[i] = NULL;
    }
    delete Ptr;
    Ptr = NULL;
  }
  return;
}


unsigned char **Cface_detect::Allocat_Memory_UnChar (int height, int width)
{
  int i;
  unsigned char **Ptr = NULL;

  Ptr = new unsigned char *[height];
  if (!Ptr)
    return (NULL);
  for (i = 0; i < height; i++) {
    Ptr[i] = new unsigned char[width];
    if (!Ptr[i])
      return (NULL);
  }
  return (Ptr);
}

int Cface_detect::Init_Memory (int height, int width)
{
  int i;

  R = new unsigned char *[height];
  if (!R)
    return (0);
  for (i = 0; i < height; i++) {
    R[i] = new unsigned char[width];
    if (!R[i])
      return (0);
  }
  G = new unsigned char *[height];
  if (!G)
    return (0);
  for (i = 0; i < height; i++) {
    G[i] = new unsigned char[width];
    if (!G[i])
      return (0);

  }
  B = new unsigned char *[height];
  if (!B)
    return (0);

  for (i = 0; i < height; i++) {
    B[i] = new unsigned char[width];
    if (!B[i])
      return (0);
  }
  inImage = new unsigned char *[height];
  if (!inImage)
    return (0);

  for (i = 0; i < height; i++) {
    inImage[i] = new unsigned char[width];
    if (!inImage[i])
      return (0);
  }

  inImage0 = new unsigned char *[height];
  if (!inImage0)
    return (0);
  for (i = 0; i < height; i++) {
    inImage0[i] = new unsigned char[width];
    if (!inImage0[i])
      return (0);
  }

  image1 = new unsigned char *[height];
  if (!image1)
    return (0);
  for (i = 0; i < height; i++) {
    image1[i] = new unsigned char[width];
    if (!image1[i])
      return (0);
  }

  image[0] = new unsigned char *[height];
  if (!image[0])
    return (0);

  for (i = 0; i < height; i++) {
    image[0][i] = new unsigned char[width];
    if (!image[0][i])
      return (0);
  }

  image[1] = new unsigned char *[height];
  if (!image[1])
    return (0);
  for (i = 0; i < height; i++) {
    image[1][i] = new unsigned char[width];
    if (!image[1][i])
      return (0);
  }

  image[2] = new unsigned char *[height];
  if (!image[2])
    return (0);
  for (i = 0; i < height; i++) {
    image[2][i] = new unsigned char[width];
    if (!image[2][i])
      return (0);
  }

  centerPoints[0] = new CP[height * width / 64];
  if (!centerPoints[0])
    return (0);
  centerPoints[1] = new CP[height * width / 64];
  if (!centerPoints[1])
    return (0);
  centerPoints[2] = new CP[height * width / 64];
  if (!centerPoints[2])
    return (0);

  return (1);
}



void Cface_detect::initialize_cpdf ()
{

  FILE *fphist;
  int i;

  if ((fphist = fopen (getpara ("FaceHist", "newhist.raw"), "rb")) == NULL)
    perror ("can not open the hist file");
  for (i = 0; i < 512; i++)
    fread (hist2d[i], sizeof (float), 512, fphist);
  fclose (fphist);

  return;
}

void Cface_detect::colorfacesegment (float th)
{
  int i, j;
  int zero_num = 0;
  int fitnum = 0, fitnum0 = 0;
  int cr, cb;
  int temp;
  int m, n, x, y;
  int tempr, tempg, tempb;
  int width, height;
  int masiclen, masicbit;

  unsigned char **b = NULL, **g = NULL, **r = NULL, **trgb = NULL, **trgb1 = NULL;	//[320][320];


/*	unsigned char	b[320][320];
	unsigned char	g[320][320];
	unsigned char	r[320][320];
	unsigned char	trgb[320][320];
	unsigned char	trgb1[320][320];
*/

  if ((r = Allocat_Memory_UnChar (imgheight, imgwidth)) == NULL)
    return;
  if ((g = Allocat_Memory_UnChar (imgheight, imgwidth)) == NULL)
    return;
  if ((b = Allocat_Memory_UnChar (imgheight, imgwidth)) == NULL)
    return;
  if ((trgb = Allocat_Memory_UnChar (imgheight, imgwidth)) == NULL)
    return;
  if ((trgb1 = Allocat_Memory_UnChar (imgheight, imgwidth)) == NULL)
    return;
/*	if(!Allocat_Memory_UnChar(b,imgheight,imgwidth))
	   return;
	if(!Allocat_Memory_UnChar(trgb,imgheight,imgwidth))
	   return;
	if(!Allocat_Memory_UnChar(trgb1,imgheight,imgwidth))
	   return;
*/
  if (bitperpixel == 8) {
    for (i = 0; i < imgheight; i++)
      for (j = 0; j < imgwidth; j++) {
	inImage[i][j] = B[i][j];
	inImage0[i][j] = B[i][j];
      }
    return;
  }
// follow is programmed by xingxin's idea
  masiclen = 8;
  masicbit = 3;

  width = imgwidth / masiclen;
  height = imgheight / masiclen;
  fitnum = width * height;
  for (i = 0; i < imgheight; i += 8) {
    y = i >> masicbit;	//divide by masiclen

    for (j = 0; j < imgwidth; j += masiclen) {
      for (m = 0; m < masiclen; m++)
	for (n = 0; n < masiclen; n++) {
	  if (i + m >= imgheight || j + n >= imgwidth)
	    continue;
	  tempr += R[i + m][j + n];
	  tempg += G[i + m][j + n];
	  tempb += B[i + m][j + n];
	}

      tempr = tempr >> (masicbit * 2);	// divide by 64
      tempg = tempg >> (masicbit * 2);
      tempb = tempb >> (masicbit * 2);

      if (tempr > 255)
	tempr = 255;
      if (tempr < 0)
	tempr = 0;
      if (tempg > 255)
	tempg = 255;
      if (tempg < 0)
	tempg = 0;
      if (tempb > 255)
	tempb = 255;
      if (tempb < 0)
	tempb = 0;

      x = j >> masicbit;

      r[y][x] = (unsigned char) tempr;
      g[y][x] = (unsigned char) tempg;
      b[y][x] = (unsigned char) tempb;
    }
  }


  //convert blocked RGB image into blocked YCrCb image and segment it
//      int cr,cb;
  for (i = 0; i < height; i++)
    for (j = 0; j < width; j++) {
      trgb[i][j] = 0;
      trgb1[i][j] = 0;
    }
  for (i = 0; i < height; i++)
    for (j = 0; j < width; j++) {
      cr = (128 * r[i][j] - 107 * g[i][j] - 21 * b[i][j]) >> 8;

      cb = (-43 * r[i][j] - 85 * g[i][j] + 128 * b[i][j]) >> 8;

      if (hist2d[cb + 255][cr + 255] < th && bitperpixel != 8) {
	r[i][j] = 1;
	g[i][j] = 1;
	b[i][j] = 1;
	trgb[i][j] = 1;
	fitnum--;
	trgb1[i][j] = 1;
      }
    }

  if (fitnum <= 10) {
    for (i = 0; i < imgheight; i++)
      for (j = 0; j < imgwidth; j++) {
	temp = (299 * R[i][j] + 587 * G[i][j] + 114 * B[i][j]) / 1000;
	inImage[i][j] = (unsigned char) temp;
	inImage0[i][j] = (unsigned char) temp;
      }
    return;

  }
//following is programmed by wu's idea
  for (i = 260; i <= 265; i++)
    for (j = 258; j <= 263; j++)
      hist2d[i][j] = 1.0;	// extension the range of color

  for (i = 0; i < imgheight; i++)
    for (j = 0; j < imgwidth; j++) {
      cr = (128 * R[i][j] - 107 * G[i][j] - 21 * B[i][j]) >> 8;

      cb = (-43 * R[i][j] - 85 * G[i][j] + 128 * B[i][j]) >> 8;

      if (hist2d[cb + 255][cr + 255] < th && bitperpixel != 8) {
	trgb[i][j] = 0;
      }
      else {
	trgb[i][j] = 1;
	fitnum0++;
      }
    }

  for (i = 0; i < imgheight; i++)
    for (j = 0; j < imgwidth; j++) {
      if (trgb[i][j] == 1) {
	if (j - 1 < 0 || j + 1 >= imgwidth || i - 1 < 0 || i + 1 >= imgheight)
	  continue;
	if ((trgb[i][j - 1] == 0 && trgb[i][j + 1] == 0) || (trgb[i - 1][j] == 0
	    && trgb[i + 1][j] == 0)) {
	  trgb[i][j] = 0;
	  fitnum0--;
	}
      }
    }
  for (i = 0; i < imgheight; i++)
    for (j = 0; j < imgwidth; j++) {
      int j0, j1, i0, i1;
      j0 = j - 20;
      if (j0 < 0)
	j0 = 0;
      j1 = j + 20;
      if (j1 > imgwidth - 1)
	j1 = imgwidth - 1;
      i0 = i - 20;
      if (i0 < 0)
	i0 = 0;
      i1 = i + 20;
      if (i1 > imgheight - 1)
	i1 = imgheight - 1;
      if (trgb[i][j] == 0) {
	int left_1 = 0, right_1 = 0;
	int top_1 = 0, bottom_1 = 0;

	{
	  int k;
	  for ( k = j0; k < j; k++)
	    if (trgb[i][k] == 1) {
	      left_1 = 1;
	      break;
	    }
	}
	{
	  int k;
	  for (k = j + 1; k <= j1; k++)
	    if (trgb[i][k] == 1) {
	      right_1 = 1;
	      break;
	    }
	}
	{
	  int k;
	  for (k = i0; k < i; k++)
	    if (trgb[k][j] == 1) {
	      top_1 = 1;
	      break;
	    }
	}
	{
	  int k;
	  for (k = i + 1; k <= i1; k++)
	    if (trgb[k][j] == 1) {
	      bottom_1 = 1;
	      break;
	    }
	}
	if ((left_1 == 1 && right_1 == 1) || (top_1 == 1 && bottom_1 == 1)) {
	  trgb[i][j] = 1;
//                              fitnum++;
	}

      }
    }
  for (i = 0; i < imgheight; i++)
    for (j = 0; j < imgwidth; j++) {
      int j0, j1, i0, i1;
      j0 = j - 20;
      if (j0 < 0)
	j0 = 0;
      j1 = j + 20;
      if (j1 > imgwidth - 1)
	j1 = imgwidth - 1;
      i0 = i - 20;
      if (i0 < 0)
	i0 = 0;
      i1 = i + 20;
      if (i1 > imgheight - 1)
	i1 = imgheight - 1;
      if (trgb[i][j] == 0) {
	int left_1 = 0, right_1 = 0;
	int top_1 = 0, bottom_1 = 0;

	{
	  int k;
	  for ( k = j0; k < j; k++)
	    if (trgb[i][k] == 1) {
	      left_1 = 1;
	      break;
	    }
	}
	{
	  int k;
	  for (k = j + 1; k <= j1; k++)
	    if (trgb[i][k] == 1) {
	      right_1 = 1;
	      break;
	    }
	}
	{
	  int k;
	  for (k = i0; k < i; k++)
	    if (trgb[k][j] == 1) {
	      top_1 = 1;
	      break;
	    }
	}
	{
	  int k;
	  for (k = i + 1; k <= i1; k++)
	    if (trgb[k][j] == 1) {
	      bottom_1 = 1;
	      break;
	    }
	}
	if ((left_1 == 1 && right_1 == 1) || (top_1 == 1 && bottom_1 == 1)) {
	  trgb[i][j] = 1;
//                              fitnum++;
	}

      }
    }
  if (fitnum0 <= 600) {
    for (i = 0; i < imgheight; i++)
      for (j = 0; j < imgwidth; j++) {
	temp = (299 * R[i][j] + 587 * G[i][j] + 114 * B[i][j]) / 1000;
	inImage[i][j] = (unsigned char) temp;
	inImage0[i][j] = (unsigned char) temp;
      }
  }
  else {
    for (i = 0; i < imgheight; i++)
      for (j = 0; j < imgwidth; j++) {
//                      if(isgray==1||trgb[i][j]==1||fitnum<600)
	if (trgb[i][j] == 1) {
	  temp = (299 * R[i][j] + 587 * G[i][j] + 114 * B[i][j]) / 1000;
	  inImage[i][j] = (unsigned char) temp;
	  inImage0[i][j] = (unsigned char) temp;
	}
	else {
	  inImage[i][j] = 255;
	  temp = (299 * R[i][j] + 587 * G[i][j] + 114 * B[i][j]) / 1000;
	  inImage0[i][j] = (unsigned char) temp;
	}
      }
  }


  //for display the segmentation result.
/*	for(i=0;i<imgheight;i++)
		for(j=0;j<imgwidth;j++)
		{
			if(inImage[i][j]==255)
			{
				R[i][j]=255;
				G[i][j]=255;
				B[i][j]=255;
			}
		}
*/
  delete_Memory_UnChar (trgb1, imgheight, imgwidth);
  delete_Memory_UnChar (trgb, imgheight, imgwidth);
  delete_Memory_UnChar (b, imgheight, imgwidth);
  delete_Memory_UnChar (g, imgheight, imgwidth);
  delete_Memory_UnChar (r, imgheight, imgwidth);
  return;
}

void Cface_detect::feature_extract (int decide)
{
  int starty, startx, masiclong;
  int i, j, k;
  unsigned char **masicimg, **masicimg1;	//[320/2][320/2];

  if ((masicimg =
      Allocat_Memory_UnChar ((imgheight + 1) / 2, (imgwidth + 1) / 2)) == NULL)
    return;
  if ((masicimg1 =
      Allocat_Memory_UnChar ((imgheight + 1) / 2, (imgwidth + 1) / 2)) == NULL)
    return;

  for (k = 0; k < 3; k++)
    for (i = 0; i < imgheight; i++)
      for (j = 0; j < imgwidth; j++)
	image[k][i][j] = 255;

//                                              printf("call in feature extraction %s\n",sbmpname);

  if (decide == 0) {

    for (masiclong = 6; masiclong >= 2; masiclong--) {
      int mh, mw, m2, hm, wm;
      mh = imgheight / masiclong;
      mw = imgwidth / masiclong;
      m2 = 2 * masiclong;
      hm = imgheight - m2;
      wm = imgwidth - m2;
      int hmasiclong = masiclong / 2;
      for (starty = 0; starty < masiclong - 1; starty += hmasiclong) {
	for (startx = 0; startx < masiclong - 1; startx += hmasiclong) {
	  masicimage1 (inImage0, masiclong, imgheight, imgwidth, startx, starty,
	    masicimg);
	  for (k = 1; k <= 3; k++) {
	    for (i = 0; i < mh; i++)
	      for (j = 0; j < mw; j++)
		masicimg1[i][j] = masicimg[i][j];

	    edgedetect (masicimg, mh, mw, k * 20, decide);

	    showmasicimage (image1, masiclong, masiclong, imgheight, imgwidth,
	      startx, starty, masicimg);
	    for (i = m2; i < hm; i++)
	      for (j = m2; j < wm; j++)
		if (image1[i][j] == 0 && inImage[i][j] < 255)
		  image[k - 1][i][j] = 0;

	    //产生漏检...........

	    for (i = 0; i < mh; i++)
	      for (j = 0; j < mw; j++)
		masicimg[i][j] = masicimg1[i][j];
	  }
	}
      }
    }
  }
  else {
    for (masiclong = 6; masiclong >= 2; masiclong--) {
      int mh, mw, m2, hm, wm;
      int masiclongw, m2w;
      masiclongw = 2;
      mh = imgheight / masiclong;
      mw = imgwidth / masiclongw;
      m2 = 2 * masiclong;
      m2w = 2 * masiclongw;
      hm = imgheight - m2;
      wm = imgwidth - m2w;
      int hmasiclong = masiclong / 2;
      int hmasiclongw = masiclongw / 2;
      for (starty = 0; starty < masiclong - 1; starty += hmasiclong) {
	for (startx = 0; startx < masiclongw - 1; startx += hmasiclongw) {
	  masicimage (inImage0, masiclong, masiclongw, imgheight, imgwidth,
	    startx, starty, masicimg);
	  for (k = 1; k <= 3; k++) {
	    for (i = 0; i < mh; i++)
	      for (j = 0; j < mw; j++)
		masicimg1[i][j] = masicimg[i][j];

	    edgedetect1 (masicimg, mh, mw, k * 15);
	    showmasicimage (image1, masiclong, masiclongw, imgheight, imgwidth,
	      startx, starty, masicimg);


	    //产生漏检...........
	    for (i = m2; i < hm; i++)
	      for (j = m2w; j < wm; j++)
		if (image1[i][j] == 0 && inImage[i][j] < 255)
		  image[k - 1][i][j] = 0;

	    //产生漏检...........

	    for (i = 0; i < mh; i++)
	      for (j = 0; j < mw; j++)
		masicimg[i][j] = masicimg1[i][j];
	  }
	}
      }
    }
  }
  delete_Memory_UnChar (masicimg1, (imgheight + 1) / 2, (imgwidth + 1) / 2);
  delete_Memory_UnChar (masicimg, (imgheight + 1) / 2, (imgwidth + 1) / 2);
  return;
}

//this function is programmed by Wu
void Cface_detect::masicimage1 (unsigned char **image,
  int masiclong, int height, int width, int startx, int starty,
  unsigned char **masicimg)
{
  int i, j, k, l, x, y;
  int sum;
  float divide;
  int skip;

  divide = (float) (masiclong * masiclong);
  for (i = starty; i < height; i += masiclong) {
    y = i / masiclong;
    for (j = startx; j < width; j += masiclong) {
      x = j / masiclong;
      sum = 0;
      skip = 0;
      for (k = 0; k < masiclong; k++) {
	for (l = 0; l < masiclong; l++) {
	  if (i + k >= height || j + l >= width)
	    continue;
	  if (image[i + k][j + l] == 255) {
	    skip = 1;
	    break;
	  }
	  else
	    sum += image[i + k][j + l];
	}
	if (skip == 1) {
	  sum = 255;
	  break;
	}
      }

      if (skip == 0)
	sum = (int) (sum / divide + 0.5);
      masicimg[y][x] = (unsigned char) sum;
    }
  }
  return;
}

//this function is programmed by Wu
void Cface_detect::masicimage (unsigned char **image,
  int masiclong, int masiclongw, int height, int width, int startx, int starty,
  unsigned char **masicimg)
{
  int i, j, k, l, x, y;
  int sum;
  float divide;
  int skip;

  divide = (float) masiclong *masiclongw;
  for (i = starty; i < height; i += masiclong) {
    y = i / masiclong;
    for (j = startx; j < width; j += masiclongw) {
      x = j / masiclongw;
      sum = 0;
      skip = 0;
      for (k = 0; k < masiclong; k++) {
	for (l = 0; l < masiclongw; l++) {
	  if (i + k >= height || j + l >= width)
	    continue;
	  if (image[i + k][j + l] == 255) {
	    skip = 1;
	    break;
	  }
	  else
	    sum += image[i + k][j + l];
	}
	if (skip == 1) {
	  sum = 255;
	  break;
	}
      }

      if (skip == 0)
	sum = (int) (sum / divide + 0.5);
      masicimg[y][x] = (unsigned char) sum;
    }
  }
  return;
}



// this function is programmed by Wu, its structure is almost similar to edgedetect_b by wang
void Cface_detect::edgedetect (unsigned char **masicimg, int height,
  int width, int TH, int decide)
{
  int i, j;
  int **tempimage = NULL;

  tempimage = new int *[height];
  if (!tempimage)
    return;
  for (i = 0; i < height; i++) {
    tempimage[i] = new int[width];
    if (!tempimage[i])
      return;
  }



  for (i = 1; i < height - 1; i++)
    for (j = 0; j < width; j++)
      if (masicimg[i][j] != 255 && masicimg[i - 1][j] != 255
	&& masicimg[i + 1][j] != 255) {
	tempimage[i][j] =
	  (int) (-masicimg[i][j] - masicimg[i][j] + masicimg[i - 1][j]
	  + masicimg[i + 1][j]);
      }

  for (i = 1; i < height - 1; i++)
    for (j = 1; j < width - 1; j++) {
      if (tempimage[i][j - 1] > TH && tempimage[i][j] > TH
	&& tempimage[i][j + 1] > TH && masicimg[i][j] != 255)
	masicimg[i][j] = 0;
      else
	masicimg[i][j] = 255;
    }

  for (i = 1; i < height - 1; i++)
    for (j = 1; j < width - 1; j++) {
      if (masicimg[i][j] == 255 && masicimg[i][j - 1] == 0
	&& masicimg[i][j + 1] == 0)
	masicimg[i][j] = 0;
    }

  for (i = 0; i < height; i++) {
    delete tempimage[i];
    tempimage[i] = NULL;
  }
  delete tempimage;
  tempimage = NULL;
  return;
}



//this function is modified from edgedetect1_b by Wu
// this function is programmed by Wu, its structure & idea is very different 
//from edgedetect_b by wang and edgedetect by wu
void Cface_detect::edgedetect1 (unsigned char **masicimg, int height, int width,
  int TH)
{
  int i, j;
  int **tempimage = NULL, **tempimage1 = NULL;	//[320/2][320/2];


  tempimage = new int *[height];
  if (!tempimage)
    return;
  for (i = 0; i < height; i++) {
    tempimage[i] = new int[width];
    if (!tempimage[i])
      return;
  }
  tempimage1 = new int *[height];
  if (!tempimage1)
    return;
  for (i = 0; i < height; i++) {
    tempimage1[i] = new int[width];
    if (!tempimage1[i])
      return;
  }


  for (i = 1; i < height - 1; i++)
    for (j = 0; j < width; j++)
      if (masicimg[i][j] != 255 && masicimg[i - 1][j] != 255
	&& masicimg[i + 1][j] != 255) {
	tempimage[i][j] =
	  (int) (-masicimg[i][j] - masicimg[i][j] + masicimg[i - 1][j]
	  + masicimg[i + 1][j]);
	tempimage1[i][j] =
	  (int) (-masicimg[i][j] - masicimg[i][j] + masicimg[i][j - 1] +
	  masicimg[i][j + 1]);
      }

  for (i = 1; i < height - 1; i++)
    for (j = 1; j < width - 1; j++) {
      if (tempimage[i][j - 1] > TH && tempimage[i][j] > TH
	&& tempimage[i][j + 1] > TH && tempimage1[i][j] >= 0
	&& masicimg[i][j] != 255)
	masicimg[i][j] = 0;
      else
	masicimg[i][j] = 255;
    }
  for (i = 1; i < height - 1; i++)
    for (j = 1; j < width - 1; j++) {
      if (masicimg[i][j] == 255 && masicimg[i][j - 1] == 0
	&& masicimg[i][j + 1] == 0)
	masicimg[i][j] = 0;
    }
  for (i = 0; i < height; i++) {
    delete tempimage1[i];
    tempimage1[i] = NULL;
  }
  delete tempimage1;
  tempimage1 = NULL;
  for (i = 0; i < height; i++) {
    delete tempimage[i];
    tempimage[i] = NULL;
  }
  delete tempimage;
  tempimage = NULL;
  return;
}




//this function is programmed by wu
void Cface_detect::showmasicimage (unsigned char **image, int masiclong,
  int masiclongw, int height, int width, int startx, int starty,
  unsigned char **masicimg)
{
  int i, j, k, l, m, n;

  for (i = 0; i < height; i++)
    for (j = 0; j < width; j++)
      image[i][j] = 0;

  for (i = starty; i < height; i += masiclong)
    for (j = startx; j < width; j += masiclongw) {
      m = i / masiclong;
      n = j / masiclongw;
      for (k = 0; k < masiclong; k++)
	for (l = 0; l < masiclongw; l++) {
	  if (i + k >= imgheight || j + l >= imgwidth)
	    continue;
	  image[i + k][j + l] = masicimg[m][n];
	}
    }

  return;
}

//label subject areas
int Cface_detect::objectcluster (unsigned char **image, int height, int width,
  CP * centerP)
{
  int i, j, k, l;
  unsigned char **label = NULL;	//[320][320];
  int number, num[8];
  int Nochange;

  if ((label = Allocat_Memory_UnChar (height, width)) == NULL)
    return (-1);
  //Label every subject area!
  for (i = 0; i < height; i++)
    for (j = 0; j < width; j++)
      label[i][j] = 0;

  k = 0;
  for (i = 1; i < height - 1; i++)
    for (j = 1; j < width - 1; j++) {
      if (image[i][j] == 0) {
	if (label[i - 1][j - 1] == 0 && label[i - 1][j] == 0 &&
	  label[i - 1][j + 1] == 0 && label[i][j - 1] == 0) {
	  k++;
	  label[i][j] = k;
	}
	else {
	  num[0] = label[i - 1][j - 1];
	  num[1] = label[i - 1][j];
	  num[2] = label[i - 1][j + 1];
	  num[3] = label[i][j - 1];
	  number = 100000;
	  for (l = 0; l <= 3; l++)
	    if (num[l] != 0 && num[l] < number)
	      number = num[l];
	  label[i][j] = number;
	}
      }
    }

  //scan to cluster!
  do {
    Nochange = 1;

    for (i = height - 2; i >= 1; i--)
      for (j = 1; j < width - 1; j++)
	if (label[i][j] != 0) {
	  num[0] = label[i - 1][j - 1];
	  num[1] = label[i - 1][j];
	  num[2] = label[i - 1][j + 1];
	  num[3] = label[i][j - 1];
	  num[4] = label[i][j + 1];
	  num[5] = label[i + 1][j - 1];
	  num[6] = label[i + 1][j];
	  num[7] = label[i + 1][j + 1];

	  number = 100000;
	  for (l = 0; l <= 7; l++)
	    if (num[l] != 0 && num[l] < k)
	      number = num[l];
	  if (label[i][j] > number) {
	    label[i][j] = number;
	    if (Nochange == 1)
	      Nochange = 0;
	  }
	}

    for (i = height - 2; i >= 1; i--)
      for (j = width - 2; j >= 1; j--)
	if (label[i][j] != 0) {
	  num[0] = label[i - 1][j - 1];
	  num[1] = label[i - 1][j];
	  num[2] = label[i - 1][j + 1];
	  num[3] = label[i][j - 1];
	  num[4] = label[i][j + 1];
	  num[5] = label[i + 1][j - 1];
	  num[6] = label[i + 1][j];
	  num[7] = label[i + 1][j + 1];

	  number = 100000;
	  for (l = 0; l <= 7; l++)
	    if (num[l] != 0 && num[l] < k)
	      number = num[l];
	  if (label[i][j] > number) {
	    label[i][j] = number;
	    if (Nochange == 1)
	      Nochange = 0;
	  }
	}

    for (i = 1; i < height - 1; i++)
      for (j = 1; j < width - 1; j++)
	if (label[i][j] != 0) {
	  num[0] = label[i - 1][j - 1];
	  num[1] = label[i - 1][j];
	  num[2] = label[i - 1][j + 1];
	  num[3] = label[i][j - 1];
	  num[4] = label[i][j + 1];
	  num[5] = label[i + 1][j - 1];
	  num[6] = label[i + 1][j];
	  num[7] = label[i + 1][j + 1];

	  number = 100000;
	  for (l = 0; l <= 7; l++)
	    if (num[l] != 0 && num[l] < number)
	      number = num[l];
	  if (label[i][j] > number) {
	    label[i][j] = number;
	    if (Nochange == 1)
	      Nochange = 0;
	  }
	}

    for (i = 1; i < height - 1; i++)
      for (j = width - 2; j >= 1; j--)
	if (label[i][j] != 0) {
	  num[0] = label[i - 1][j - 1];
	  num[1] = label[i - 1][j];
	  num[2] = label[i - 1][j + 1];
	  num[3] = label[i][j - 1];
	  num[4] = label[i][j + 1];
	  num[5] = label[i + 1][j - 1];
	  num[6] = label[i + 1][j];
	  num[7] = label[i + 1][j + 1];

	  number = 100000;
	  for (l = 0; l <= 7; l++)
	    if (num[l] != 0 && num[l] < number)
	      number = num[l];
	  if (label[i][j] > number) {
	    label[i][j] = number;
	    if (Nochange == 1)
	      Nochange = 0;
	  }
	}

  } while (Nochange == 0);

  int *count = NULL;		//[320*320/64];
  int *x, *y;			//[320*320/64],y[320*320/64];

  count = new int[height * width / 64 + 1];
  x = new int[height * width / 64 + 1];
  y = new int[height * width / 64 + 1];
  for (i = 0; i <= k; i++) {
    count[i] = 0;
    x[i] = 0;	//right
    y[i] = width;	//left
  }

  for (i = 1; i < height - 1; i++)
    for (j = 1; j < width - 1; j++)
      if (image[i][j] == 0)
	count[label[i][j]]++;

  //discard bigger or smaller areas
  int areath1 = 350;
//      int areath2=3;
  for (i = 1; i < height - 1; i++)
    for (j = 1; j < width - 1; j++) {
      if (image[i][j] == 0 && count[label[i][j]] > areath1)
	image[i][j] = 255;
//                      if(image[i][j]==0 && count[label[i][j]]<areath2)
//                              image[i][j]=255;
    }

  for (i = 1; i <= k; i++)
    if (count[i] > areath1 /*|| count[i]<areath2 */ )
      count[i] = 0;

  //discard longer areas
  for (i = 1; i < height - 1; i++)
    for (j = 1; j < width - 1; j++)
      if (image[i][j] == 0) {
	if (j > x[label[i][j]])
	  x[label[i][j]] = j;
	if (j < y[label[i][j]])
	  y[label[i][j]] = j;
      }

  int longth = 40;
  for (i = 1; i <= k; i++)
    if (count[i] != 0 && (x[i] - y[i] + 1) > longth)
      count[i] = 0;

  j = 0;
  for (i = 1; i <= k; i++)
    if (count[i] != 0) {
      centerP[j].length = x[i] - y[i] + 1;
      j++;
    }

  for (i = 1; i < height - 1; i++)
    for (j = 1; j < width - 1; j++)
      if (image[i][j] == 0 && count[label[i][j]] == 0)
	image[i][j] = 255;

  for (i = 0; i <= k; i++) {
    x[i] = 0;
    y[i] = 0;
  }

  for (i = 1; i < height - 1; i++)
    for (j = 1; j < width - 1; j++)
      if (image[i][j] == 0) {
	x[label[i][j]] += j;
	y[label[i][j]] += i;
      }

  for (i = 1; i <= k; i++)
    if (count[i] != 0) {
      x[i] = (int) (x[i] / ((float) count[i]) + 0.5);
      y[i] = (int) (y[i] / ((float) count[i]) + 0.5);
      image[y[i]][x[i]] = 100;
    }

  j = 0;
  for (i = 1; i <= k; i++)
    if (count[i] != 0) {
      centerP[j].x = x[i];
      centerP[j].y = y[i];
      centerP[j].count = count[i];
      j++;
    }
  delete y;
  y = NULL;
  delete x;
  x = NULL;
  delete count;
  count = NULL;
  delete_Memory_UnChar (label, height, width);
  return (j);
}


int Cface_detect::askcandidates (int kk)
{
  int i, j, k;


  k = kk;
  for (i = 0; i < 3; i++)
    for (j = 0; j < facenum[i]; j++)
      if (face[i][j].Isface == 1) {
	candidates[k] = face[i][j];
	k++;
      }

  return k;
}

void Cface_detect::coarselocate (int num, int z)
{
  int i, j, k, l, m, n, p, q, x, y;
  CP numP, Lb, Rb, Le, Re, tempP[POINTS], tempP1[POINTS];
  int eyesnum;
  FACE tempface[15];
  float RbRe = 0.5;

  for (i = 0; i < 15; i++) {
    tempface[i].Isface = 0;
    tempface[i].Lb.x = -1;
    tempface[i].Le.x = -1;
    tempface[i].Lb.y = -1;
    tempface[i].Le.y = -1;
    tempface[i].Rb.x = -1;
    tempface[i].Re.x = -1;
    tempface[i].Rb.y = -1;
    tempface[i].Re.y = -1;
  }

  eyesnum = 0;

  numP = centerPoints[num][z];

  Lb = numP;

  l = scan1 (Lb, tempP, PI * ANGLE0 / 180, 35, num, 0);	//if the height of the face is less than
  //35,it is discarded.

  if (l == -1)
    return;

  if (l == 0) {
    Lb.x = -1;
    Lb.y = -1;
    Lb.count = 0;
    Le = numP;	//no left eyebrow!
  }
  else if (l == 1)
    Le = tempP[0];
  else {
    i = 35;
    j = 0;
    for (k = 0; k < l; k++)
      if ((tempP[k].y - Lb.y) < i) {
	i = tempP[k].y - Lb.y;
	j = k;
      }
    Le = tempP[j];
  }


  /////////////////////////////////////////////////////////////////

  if (l >= 1)	//there are left eye and left eyebrow!
  {
    j =
      (int) (sqrt ((double)(Lb.x - Le.x) * (Lb.x - Le.x) + (Lb.y - Le.y) * (Lb.y -
	Le.y)) + 0.5);

    i = scan1 (Lb, tempP, PI * ANGLE1 / 180, (int) (1.2 * j + 0.5), num, 1);
    if (i == -1)
      return;

    if (i <= 1) {
      k = scan1 (Le, tempP, PI * ANGLE1 / 180, (int) (1.2 * j + 0.5), num, 1);
      if (k == -1)
	return;

      if (k <= 1) {
	int hh = 70;
	if (hh > 5 * j)
	  hh = 5 * j;
	if (hh > imgwidth - Le.x)
	  hh = imgwidth - Le.x;
	m =
	  scan2 (Le, tempP, PI * ANGLE1 / 180, (int) (1.2 * j + 0.5), hh, num);
	if (m == -1)
	  return;

	if (m == 1) {
	  tempface[eyesnum].Lb = Lb;
	  tempface[eyesnum].Le = Le;
	  tempface[eyesnum].Re = tempP[0];
	  tempface[eyesnum].Isface = 1;
	  Rb = tempP[0];
	  eyesnum++;

	  float nn =
	    (float) sqrt ((float)(Le.x - Rb.x) * (Le.x - Rb.x) + (Le.y -
     Rb.y) * (Le.y - Rb.y));
	  int mm = (int) (RbRe * nn + 0.5);
	  n = scan1 (Rb, tempP1, PI * ANGLE0 / 180, mm, num, 0);	///?????
	  if (n == -1)
	    return;

	  if (n >= 1) {
	    p = mm;	//(int)(1.5*j+0.5);
	    q = 0;
	    for (x = 0; x < n; x++)
	      if ((tempP1[x].y - Rb.y) < p) {
		p = tempP1[x].y - Rb.y;
		q = x;
	      }
	    Re = tempP1[q];

	    tempface[eyesnum].Lb = Lb;
	    tempface[eyesnum].Le = Le;
	    tempface[eyesnum].Rb = Rb;
	    tempface[eyesnum].Re = Re;
	    tempface[eyesnum].Isface = 1;
	    eyesnum++;
	  }	//if(n>=1)
	}	//if(m==1)

	else if (m == 2) {

	  for (y = 0; y < m; y++) {
	    tempface[eyesnum].Lb = Lb;
	    tempface[eyesnum].Le = Le;
	    tempface[eyesnum].Re = tempP[y];
	    tempface[eyesnum].Isface = 1;
	    Rb = tempP[y];
	    eyesnum++;

	    float nn =
	      (float) sqrt ((float)(Le.x - Rb.x) * (Le.x - Rb.x) + (Le.y -
       Rb.y) * (Le.y - Rb.y));
	    int mm = (int) (RbRe * nn + 0.5);
	    n = scan1 (Rb, tempP1, PI * ANGLE0 / 180, mm, num, 0);
	    if (n == -1)
	      return;

	    if (n >= 1) {
	      p = mm;	//(int)(1.5*j+0.5);
	      q = 0;
	      for (x = 0; x < n; x++)
		if ((tempP1[x].y - Rb.y) < p) {
		  p = tempP1[x].y - Rb.y;
		  q = x;
		}
	      Re = tempP1[q];

	      tempface[eyesnum].Lb = Lb;
	      tempface[eyesnum].Le = Le;
	      tempface[eyesnum].Rb = Rb;
	      tempface[eyesnum].Re = Re;
	      tempface[eyesnum].Isface = 1;
	      eyesnum++;
	    }	//if(n>=1)
	  }	//for(y)
	}	//else if(m==2)

	else if (m >= 3) {
	  adjust1 (Le, tempP, m, 3);

	  for (y = 0; y < 3; y++) {
	    tempface[eyesnum].Lb = Lb;
	    tempface[eyesnum].Le = Le;
	    tempface[eyesnum].Re = tempP[y];
	    tempface[eyesnum].Isface = 1;
	    Rb = tempP[y];
	    eyesnum++;

	    float nn =
	      (float) sqrt ((float)(Le.x - Rb.x) * (Le.x - Rb.x) + (Le.y -
       Rb.y) * (Le.y - Rb.y));
	    int mm = (int) (RbRe * nn + 0.5);
	    n = scan1 (Rb, tempP1, PI * ANGLE0 / 180, mm, num, 0);
	    if (n == -1)
	      return;

	    if (n >= 1) {
	      p = mm;	//(int)(1.5*j+0.5);
	      q = 0;
	      for (x = 0; x < n; x++)
		if ((tempP1[x].y - Rb.y) < p) {
		  p = tempP1[x].y - Rb.y;
		  q = x;
		}
	      Re = tempP1[q];

	      tempface[eyesnum].Lb = Lb;
	      tempface[eyesnum].Le = Le;
	      tempface[eyesnum].Rb = Rb;
	      tempface[eyesnum].Re = Re;
	      tempface[eyesnum].Isface = 1;
	      eyesnum++;
	    }	//if(n>=1)
	  }	//for(y)
	}	//else(m>=3)
      }	//if(k<=1)
    }	//if(i<=1)

    Le = Lb;	//if there is no left eyebrow!

    int hh = 70;
    if ((imgwidth - Le.x) < 70)
      hh = imgwidth - Le.x;

    m = scan1 (Le, tempP, PI * ANGLE1 / 180, hh, num, 1);	//provided that distance between eyes
    if (m == -1)
      return;	// is less 40 points!

    if (m == 1) {
      tempface[eyesnum].Le = Le;
      tempface[eyesnum].Re = tempP[0];
      tempface[eyesnum].Isface = 1;
      Rb = tempP[0];
      eyesnum++;

      float nn =
	(float) sqrt ((float)(Le.x - Rb.x) * (Le.x - Rb.x) + (Le.y - Rb.y) * (Le.y -
	  Rb.y));
      int mm = (int) (RbRe * nn + 0.5);
      n = scan1 (Rb, tempP1, PI * ANGLE0 / 180, mm, num, 0);	//??
      if (n == -1)
	return;

      if (n >= 1) {
	p = mm;	//15;
	q = 0;
	for (x = 0; x < n; x++)
	  if ((tempP1[x].y - Rb.y) < p) {
	    p = tempP1[x].y - Rb.y;
	    q = x;
	  }
	Re = tempP1[q];

	tempface[eyesnum].Le = Le;
	tempface[eyesnum].Rb = Rb;
	tempface[eyesnum].Re = Re;
	tempface[eyesnum].Isface = 1;
	eyesnum++;
      }	//if(n>=1)
    }	//if(m==1)

    else if (m == 2) {

      for (y = 0; y < 2; y++) {
	tempface[eyesnum].Le = Le;
	tempface[eyesnum].Re = tempP[y];
	tempface[eyesnum].Isface = 1;
	Rb = tempP[y];
	eyesnum++;

	float nn =
	  (float) sqrt ((float)(Le.x - Rb.x) * (Le.x - Rb.x) + (Le.y - Rb.y) * (Le.y -
	    Rb.y));
	int mm = (int) (RbRe * nn + 0.5);
	n = scan1 (Rb, tempP1, PI * ANGLE0 / 180, mm, num, 0);	//??
	if (n == -1)
	  return;

	if (n >= 1) {
	  p = mm;	//15;
	  q = 0;
	  for (x = 0; x < n; x++)
	    if ((tempP1[x].y - Rb.y) < p) {
	      p = tempP1[x].y - Rb.y;
	      q = x;
	    }
	  Re = tempP1[q];

	  tempface[eyesnum].Le = Le;
	  tempface[eyesnum].Rb = Rb;
	  tempface[eyesnum].Re = Re;
	  tempface[eyesnum].Isface = 1;
	  eyesnum++;
	}	//if(n>=1)
      }	//for(y)
    }	//else if(m==2)

    else if (m >= 3) {
      adjust1 (Le, tempP, m, 3);

      for (y = 0; y < 3; y++) {
	tempface[eyesnum].Le = Le;
	tempface[eyesnum].Re = tempP[y];
	tempface[eyesnum].Isface = 1;
	Rb = tempP[y];
	eyesnum++;

	float nn =
	  (float) sqrt ((float)(Le.x - Rb.x) * (Le.x - Rb.x) + (Le.y - Rb.y) * (Le.y -
	    Rb.y));
	int mm = (int) (RbRe * nn + 0.5);
	n = scan1 (Rb, tempP1, PI * ANGLE0 / 180, mm, num, 0);	//??
	if (n == -1)
	  return;

	if (n >= 1) {
	  p = mm;	//15;
	  q = 0;
	  for (x = 0; x < n; x++)
	    if ((tempP1[x].y - Rb.y) < p) {
	      p = tempP1[x].y - Rb.y;
	      q = x;
	    }
	  Re = tempP1[q];

	  tempface[eyesnum].Le = Le;
	  tempface[eyesnum].Rb = Rb;
	  tempface[eyesnum].Re = Re;
	  tempface[eyesnum].Isface = 1;
	  eyesnum++;
	}	//if(n>=1)
      }	//for(y)
    }	//else(m>=3)
  }	//if(l>=1)

  ////////////////////////////////////////////////

  else	//(l==0)//there is no left eyebrow
  {


    int hhh = 70;
    if ((imgwidth - Le.x) < 70)
      hhh = imgwidth - Le.x;

    m = scan1 (Le, tempP, PI * ANGLE1 / 180, hhh, num, 1);	//provided that distance between eyes
    // is less 60 points!

    if (m == -1)
      return;

    if (m == 1) {
      tempface[eyesnum].Le = Le;
      tempface[eyesnum].Re = tempP[0];
      tempface[eyesnum].Isface = 1;
      Rb = tempP[0];
      eyesnum++;

      float nn =
	(float) sqrt ((float)(Le.x - Rb.x) * (Le.x - Rb.x) + (Le.y - Rb.y) * (Le.y -
	  Rb.y));
      int mm = (int) (RbRe * nn + 0.5);
      n = scan1 (Rb, tempP1, PI * ANGLE0 / 180, mm, num, 0);

      if (n == -1)
	return;

      if (n >= 1) {
	p = mm;	//35;
	q = 0;
	for (x = 0; x < n; x++)
	  if ((tempP1[x].y - Rb.y) < p) {
	    p = tempP1[x].y - Rb.y;
	    q = x;
	  }
	Re = tempP1[q];

	tempface[eyesnum].Le = Le;
	tempface[eyesnum].Rb = Rb;
	tempface[eyesnum].Re = Re;
	tempface[eyesnum].Isface = 1;
	eyesnum++;
      }	//if(n>=1)
    }	//if(m==1)

    else if (m == 2) {

      for (y = 0; y < 2; y++) {
	tempface[eyesnum].Le = Le;
	tempface[eyesnum].Re = tempP[y];
	tempface[eyesnum].Isface = 1;
	Rb = tempP[y];
	eyesnum++;

	float nn =
	  (float) sqrt ((float)(Le.x - Rb.x) * (Le.x - Rb.x) + (Le.y - Rb.y) * (Le.y -
	    Rb.y));
	int mm = (int) (RbRe * nn + 0.5);
	n = scan1 (Rb, tempP1, PI * ANGLE0 / 180, mm, num, 0);
	if (n == -1)
	  return;

	if (n >= 1) {
	  p = mm;	//35;
	  q = 0;
	  for (x = 0; x < n; x++)
	    if ((tempP1[x].y - Rb.y) < p) {
	      p = tempP1[x].y - Rb.y;
	      q = x;
	    }
	  Re = tempP1[q];

	  tempface[eyesnum].Le = Le;
	  tempface[eyesnum].Rb = Rb;
	  tempface[eyesnum].Re = Re;
	  tempface[eyesnum].Isface = 1;
	  eyesnum++;
	}	//if(n>=1)
      }	//for(y)
    }	//else if(m==2)

    else if (m >= 3) {
      adjust1 (Le, tempP, m, 3);

      for (y = 0; y < 3; y++) {
	tempface[eyesnum].Le = Le;
	tempface[eyesnum].Re = tempP[y];
	tempface[eyesnum].Isface = 1;
	Rb = tempP[y];
	eyesnum++;

	float nn =
	  (float) sqrt ((float)(Le.x - Rb.x) * (Le.x - Rb.x) + (Le.y - Rb.y) * (Le.y -
	    Rb.y));
	int mm = (int) (RbRe * nn + 0.5);
	n = scan1 (Rb, tempP1, PI * ANGLE0 / 180, mm, num, 0);
	if (n == -1)
	  return;

	if (n >= 1) {
	  p = mm;	//35;
	  q = 0;
	  for (x = 0; x < n; x++)
	    if ((tempP1[x].y - Rb.y) < p) {
	      p = tempP1[x].y - Rb.y;
	      q = x;
	    }
	  Re = tempP1[q];

	  tempface[eyesnum].Le = Le;
	  tempface[eyesnum].Rb = Rb;
	  tempface[eyesnum].Re = Re;
	  tempface[eyesnum].Isface = 1;
	  eyesnum++;
	}	//if(n>=1)
      }	//for(y)
    }	//else(m>=3)
  }	//else(l==0)

  ///////////////////////////////////////////////////////////////

  if (eyesnum > 0)	//1998,12,27
  {
    for (i = 0; i < eyesnum; i++)
      if (tempface[i].Isface == 1
	&& (tempface[i].Le.count >= 10 * tempface[i].Re.count
	  || tempface[i].Re.count >= 10 * tempface[i].Le.count))
	tempface[i].Isface = 0;
  }

   /**/ if (eyesnum >= 2) {
    for (i = 0; i < eyesnum - 1; i++) {

      for (j = i + 1; j < eyesnum; j++)
	if (tempface[i].Isface == 1 && tempface[j].Isface == 1 &&
	  tempface[i].Le.x == tempface[j].Le.x
	  && tempface[i].Le.y == tempface[j].Le.y
	  && tempface[i].Re.x == tempface[j].Re.x
	  && tempface[i].Re.y == tempface[j].Re.y) {
	  tempface[j].Isface = 0;
	  if (tempface[i].Lb.x == -1 && tempface[j].Lb.x > 0)
	    tempface[i].Lb = tempface[j].Lb;

	  if (tempface[i].Rb.x == -1 && tempface[j].Rb.x > 0)
	    tempface[i].Rb = tempface[j].Rb;
	}
    }
  }

  for (i = 0; i < eyesnum; i++) {
    j = (int) sqrt ((float)(tempface[i].Le.x - tempface[i].Re.x) *
      (tempface[i].Le.x - tempface[i].Re.x) +
      (tempface[i].Le.y - tempface[i].Re.y) *
      (tempface[i].Le.y - tempface[i].Re.y));
    //      if(tempface[i].Le.length>tempface[i].Re.length)  k=tempface[i].Le.length;
    //      else  k=tempface[i].Re.length;
    //      if(k>0.7*j)
    //              tempface[i].Isface=0;

    if (tempface[i].Isface == 1)
      tempface[i].eyesdis = j;
  }

  for (k = 0; k < eyesnum; k++)
    if (tempface[k].Isface == 1) {
      face[num][facenum[num]] = tempface[k];	//face[num][facenum[num]].Isface=1;
      //      drawline(num,face[num][facenum[num]].Le,face[num][facenum[num]].Re);

      facenum[num]++;
    }



  return;

}	//end

/****************************************************************
*																*
*	rotate face area to search possible nose,mouth and beard;	*
*	origin is Le;												*
*	rotated image is stored in RatateImage[200][100];			*
*	return width of possible face.								*
*																*
****************************************************************/

void Cface_detect::RotateInFaceArea (FACE tface, FACE face[4], CP tempP[POINTS],
  int count, int num)
{
  double COS, SIN, x, y, xt, yt;
  int i, height, width;

  tface.Nose.count = 0;

  SIN = (double) (tface.Re.y - tface.Le.y) / (double) tface.eyesdis;
  COS = (double) (tface.Re.x - tface.Le.x) / (double) tface.eyesdis;

  height = (int) (preH * tface.eyesdis);
  width = tface.eyesdis;

  for (i = 0; i < count; i++) {
    x = tempP[i].x - tface.Le.x;	//take Le as origin
    y = tempP[i].y - tface.Le.y;

    yt = y * COS - x * SIN;	//rotated position
    xt = y * SIN + x * COS;

    tempP[i].x = (int) (xt + 0.5);
    tempP[i].y = (int) (yt + 0.5);

    //      RotateImage[(int)(yt+0.5)][(int)(xt+0.5)]=0;
  }

  if (count > 4)
    adjust2 (tempP, count, 4);
  else
    adjust2 (tempP, count, 4);
//       adjust2wu(tempP,count,count,width);


  for (i = 0; i < count && i < 4; i++) {
    face[i] = deformablematch (tface, tempP, i + 1);


    if (face[i].Isface == 1) {
      yt = face[i].Mouth.y * COS + face[i].Mouth.x * SIN;	//rotated position
      xt = -face[i].Mouth.y * SIN + face[i].Mouth.x * COS;

      face[i].Mouth.x = (int) (xt + 0.5) + face[i].Le.x;
      face[i].Mouth.y = (int) (yt + 0.5) + face[i].Le.y;

      if (face[i].Nose.count != 0) {
	yt = face[i].Nose.y * COS + face[i].Nose.x * SIN;	//rotated position
	xt = -face[i].Nose.y * SIN + face[i].Nose.x * COS;

	face[i].Nose.x = (int) (xt + 0.5) + face[i].Le.x;
	face[i].Nose.y = (int) (yt + 0.5) + face[i].Le.y;
      }
    }

    tface.Isface = 1;
  }

  return;
}


FACE Cface_detect::deformablematch (FACE tface, CP tempP[POINTS], int count)
{


  if (count == 1)
    tface = match1 (tface, tempP);
  else if (count == 2)
    tface = match2 (tface, tempP);
  else if (count == 3)
    tface = match3 (tface, tempP);
  else
    tface = match4 (tface, tempP);


  return tface;
}



///////////////////////////////////////////////////////////////

FACE Cface_detect::match1 (FACE tface, CP tempP[POINTS])
{
  int h, w;

  h = tempP[0].y;
  tface.EMdis = h;
  w = tface.eyesdis;
  if (h < (int) (HW * w + 0.5) || tempP[0].x > (int) (0.67 * w + 0.5) ||
    tempP[0].x < (int) (0.33 * w + 0.5) ||
    tempP[0].length > (int) (MEYES * w + 0.5)) tface.Isface = 0;
  else
    tface.Mouth = tempP[0];	//should be rotate later!

  return tface;
}

FACE Cface_detect::match2 (FACE tface, CP tempP[POINTS])
{
  int h, w;

  h = tempP[1].y;
  tface.EMdis = h;
  w = tface.eyesdis;
  if (h > (int) (HW * w + 0.5) && tempP[1].x > (int) (0.25 * w + 0.5) &&
    tempP[1].x < (int) (0.75 * w + 0.5)
    && tempP[1].length < (int) (MEYES * w + 0.5)) {
    tface.Mouth = tempP[1];
    if (tempP[0].y < (int) (0.5 * h + 0.5))
      return tface;
    else if (tempP[0].y < (int) (0.875 * h + 0.5) &&
      tempP[0].x > (int) (NOSEL * w + 0.5) &&
      tempP[0].x < (int) (NOSER * w + 0.5) &&
      (tempP[0].count <= tempP[1].count || tempP[0].length <= tempP[1].length)) {
      tface.Nose = tempP[0];
      return tface;
    }
    else
      return match1 (tface, tempP);
  }
  else
    return match1 (tface, tempP);
}

FACE Cface_detect::match3 (FACE tface, CP tempP[POINTS])
{
  int h, w;

  h = tempP[2].y;
  tface.EMdis = h;
  w = tface.eyesdis;
  if (h > (int) (HW * w + 0.5) && tempP[2].x > (int) (0.25 * w + 0.5) &&
    tempP[2].x < (int) (0.75 * w + 0.5)
    && tempP[2].length < (int) (MEYES * w + 0.5)) {
    tface.Mouth = tempP[2];
    if (tempP[1].y < (int) (0.5 * h + 0.5))
      return match2 (tface, tempP);
    else if (tempP[0].y < (int) (0.5 * h + 0.5)) {
      if (tempP[1].y < (int) (0.875 * h + 0.5) &&
	tempP[1].x > (int) (NOSEL * w + 0.5) &&
	tempP[1].x < (int) (NOSER * w + 0.5) &&
	(tempP[1].count <= tempP[2].count ||
	  tempP[1].length <= tempP[2].length)) {
	tface.Nose = tempP[1];
	return tface;
      }
      else
	return match2 (tface, tempP);
    }
    else {

      if (tempP[0].y < (int) (0.875 * h + 0.5) &&
	tempP[0].x > (int) (NOSEL * w + 0.5) &&
	tempP[0].x < (int) (NOSER * w + 0.5) &&
	tempP[1].y < (int) (0.875 * h + 0.5) &&
	tempP[1].x > (int) (NOSEL * w + 0.5) &&
	tempP[1].x < (int) (NOSER * w + 0.5) &&
	(tempP[0].count <= tempP[2].count ||
	  tempP[0].length <= tempP[2].length) &&
	(tempP[1].count <= tempP[2].count ||
	  tempP[1].length <= tempP[2].length)) {
	if (tempP[0].y > tempP[1].y)
	  tface.Nose = tempP[0];
	else
	  tface.Nose = tempP[1];

	return tface;
      }
      else
	return match2 (tface, tempP);
    }
  }
  else
    return match2 (tface, tempP);

}


FACE Cface_detect::match4 (FACE tface, CP tempP[POINTS])
{
  int h, w;

  h = tempP[3].y;
  tface.EMdis = h;
  w = tface.eyesdis;

  if (h > (int) (HW * w + 0.5) && tempP[0].y < (int) (0.5 * h + 0.5)
    && tempP[1].y >= (int) (0.5 * h + 0.5)
    && tempP[1].y < (int) (0.875 * h + 0.5)
    && tempP[1].x > (int) (NOSEL * w + 0.5)
    && tempP[1].x < (int) (NOSER * w + 0.5)
    && tempP[2].y >= (int) (0.5 * h + 0.5)
    && tempP[2].y < (int) (0.875 * h + 0.5)
    && tempP[2].x > (int) (NOSEL * w + 0.5)
    && tempP[2].x < (int) (NOSER * w + 0.5)
    && tempP[3].x > (int) (0.25 * w + 0.5)
    && tempP[3].x < (int) (0.75 * w + 0.5) && (tempP[0].count <= tempP[3].count
      || tempP[0].length <= tempP[3].length)
    && (tempP[1].count <= tempP[3].count || tempP[1].length <= tempP[3].length)
    && (tempP[2].count <= tempP[3].count || tempP[2].length <= tempP[3].length)
    && tempP[3].length < (int) (MEYES * w + 0.5)) {
    tface.Mouth = tempP[3];
    if (tempP[1].y < tempP[2].y)
      tface.Nose = tempP[2];
    else
      tface.Nose = tempP[1];

    return tface;
  }
  else
    return match3 (tface, tempP);
}

void Cface_detect::coarsedetectface (int num)
{
  int i, j, k, l, facek, m, n;
  FACE tface[1000], ttface[4];
  CP tempP[POINTS];

  facenum[num] = 0;
  for (j = 0; j < number[num]; j++)
    coarselocate (num, j);

  if (facenum[num] >= 2) {
    for (i = 0; i < facenum[num] - 1; i++)
      for (j = i + 1; j < facenum[num]; j++)
	if (face[num][i].Isface == 1 && face[num][j].Isface == 1 &&
	  face[num][i].Le.x == face[num][j].Le.x
	  && face[num][i].Le.y == face[num][j].Le.y
	  && face[num][i].Re.x == face[num][j].Re.x
	  && face[num][i].Re.y == face[num][j].Re.y) {
	  face[num][j].Isface = 0;
	  if (face[num][i].Lb.x == -1 && face[num][j].Lb.x > 0)
	    face[num][i].Lb = face[num][j].Lb;

	  if (face[num][i].Rb.x == -1 && face[num][j].Rb.x > 0)
	    face[num][i].Rb = face[num][j].Rb;
	}
  }

  facek = 0;

  for (i = 0; i < facenum[num]; i++) {
    if (face[num][i].Isface == 1) {
      j = searchpoints (face[num][i].Le, face[num][i].Re, tempP, num);

      if (j > 0) {
	RotateInFaceArea (face[num][i], ttface, tempP, j, num);

	for (m = 0; m < j - 1 && m < 3; m++)
	  for (n = m + 1; n < j && n < 4; n++)
	    if (ttface[m].Isface == 1 && ttface[n].Isface == 1 &&
	      ttface[m].Mouth.x == ttface[n].Mouth.x &&
	      ttface[m].Mouth.y == ttface[n].Mouth.y)
	      ttface[n].Isface = 0;

	for (k = 0; k < j && k < 4; k++)
	  if (ttface[k].Isface == 1) {
	    tface[facek] = ttface[k];
	    facek++;
	  }
      }
    }

  }

  facenum[num] = facek;
  for (i = 0; i < facenum[num]; i++)
    face[num][i] = tface[i];



  if (num > 0) {
    for (i = 0; i < facenum[num]; i++)
      for (j = 0; j < facenum[0]; j++)
	if (abs (face[num][i].Le.x - face[0][j].Le.x) < 4 &&
	  abs (face[num][i].Le.y - face[0][j].Le.y) < 4 &&
	  abs (face[num][i].Re.x - face[0][j].Re.x) < 4 &&
	  abs (face[num][i].Re.y - face[0][j].Re.y) < 4 &&
	  abs (face[num][i].Mouth.x - face[0][j].Mouth.x) < 4 &&
	  abs (face[num][i].Mouth.y - face[0][j].Mouth.y) < 4)
	  face[num][i].Isface = 0;
  }

  int tempcount;		//1998,12,28
  for (i = 0; i < facenum[num]; i++) {
    if (face[num][i].Le.count > face[num][i].Re.count)
      tempcount = face[num][i].Re.count;
    else
      tempcount = face[num][i].Le.count;
// for a face: length of mouth *5 > length of eye
//              if(5*face[num][i].Mouth.count<tempcount) 
//                      face[num][i].Isface=0;
  }
  if (facenum[num] >= 2) {
    for (i = 0; i < facenum[num] - 1; i++)
      for (j = i + 1; j < facenum[num]; j++) {
	if (face[num][i].Isface == 1 && face[num][j].Isface == 1 &&
	  face[num][i].Le.x == face[num][j].Lb.x &&
	  face[num][i].Le.y == face[num][j].Lb.y &&
//             face[num][i].Re.x==face[num][j].Rb.x              && 
//                 face[num][i].Re.y==face[num][j].Rb.y              &&
	  ((abs (face[num][i].Mouth.x - face[num][j].Mouth.x) < 3 &&
	      abs (face[num][i].Mouth.y - face[num][j].Mouth.y) < 3) ||
	    (abs (face[num][i].Mouth.x - face[num][j].Nose.x) < 3 &&
	      abs (face[num][i].Mouth.y - face[num][j].Nose.y) < 3))) {
	  if (face[num][j].Rb.x == -1) {
	    l =
	      scan1 (face[num][i].Re, tempP, ANGLE0,
	      (int) (0.5 * face[num][j].eyesdis + 0.5), num, 0);
	    if (l > 1)
	      adjust2 (tempP, l, 1);

	    if (l > 0 && tempP[0].x == face[num][j].Re.x
	      && tempP[0].y == face[num][j].Re.y) {
	      //      face[num][i].Isface=0;
	      face[num][j].Rb = face[num][i].Re;
	    }
	  }
	}

	if (face[num][i].Isface == 1 && face[num][j].Isface == 1 &&
	  face[num][i].Lb.x == face[num][j].Le.x &&
	  face[num][i].Lb.y == face[num][j].Le.y &&
//             face[num][i].Rb.x==face[num][j].Re.x              && 
//                 face[num][i].Rb.y==face[num][j].Re.y              &&
	  ((abs (face[num][i].Mouth.x - face[num][j].Mouth.x) < 3 &&
	      abs (face[num][i].Mouth.y - face[num][j].Mouth.y) < 3) ||
	    (abs (face[num][i].Nose.x - face[num][j].Mouth.x) < 3 &&
	      abs (face[num][i].Nose.y - face[num][j].Mouth.y) < 3))) {
	  if (face[num][i].Rb.x == -1) {
	    l =
	      scan1 (face[num][j].Re, tempP, ANGLE0,
	      (int) (0.5 * face[num][i].eyesdis + 0.5), num, 0);
	    if (l > 1)
	      adjust2 (tempP, l, 1);

	    if (l > 0 && tempP[0].x == face[num][i].Re.x
	      && tempP[0].y == face[num][i].Re.y) {
	      //      face[num][j].Isface=0;
	      face[num][i].Rb = face[num][j].Re;
	    }
	  }

	}

	if (face[num][i].Isface == 1 && face[num][j].Isface == 1 &&
	  face[num][i].Le.x == face[num][j].Lb.x &&
	  face[num][i].Le.y == face[num][j].Lb.y &&
	  face[num][i].Re.x == face[num][j].Rb.x &&
	  face[num][i].Re.y == face[num][j].Rb.y &&
	  (abs (face[num][i].Mouth.x - face[num][j].Mouth.x) < 3 &&
	    abs (face[num][i].Mouth.y - face[num][j].Mouth.y) < 3)) {

	  for (m = 0; m < facenum[num] && m != i && m != j; m++)
	    if (face[num][m].Isface == 1 &&
	      abs (face[num][m].Mouth.x - face[num][i].Mouth.x) < 3 &&
	      abs (face[num][m].Mouth.y - face[num][i].Mouth.y) < 3 &&
	      (face[num][m].Le.x == face[num][j].Le.x &&
		face[num][m].Le.y == face[num][j].Le.y &&
		face[num][m].Re.x == face[num][i].Re.x &&
		face[num][m].Re.y == face[num][i].Re.y) ||
	      (face[num][m].Le.x == face[num][i].Le.x &&
		face[num][m].Le.y == face[num][i].Le.y &&
		face[num][m].Re.x == face[num][j].Re.x &&
		face[num][m].Re.y == face[num][j].Re.y))
	      face[num][m].Isface = 0;

	  face[num][i].Isface = 0;
	}

	if (face[num][i].Isface == 1 && face[num][j].Isface == 1 &&
	  face[num][j].Le.x == face[num][i].Lb.x &&
	  face[num][j].Le.y == face[num][i].Lb.y &&
	  face[num][j].Re.x == face[num][i].Rb.x &&
	  face[num][j].Re.y == face[num][i].Rb.y &&
	  (abs (face[num][i].Mouth.x - face[num][j].Mouth.x) < 3 &&
	    abs (face[num][i].Mouth.y - face[num][j].Mouth.y) < 3)) {

	  for (m = 0; m < facenum[num] && m != i && m != j; m++)
	    if (face[num][m].Isface == 1 &&
	      abs (face[num][m].Mouth.x - face[num][i].Mouth.x) < 3 &&
	      abs (face[num][m].Mouth.y - face[num][i].Mouth.y) < 3 &&
	      (face[num][m].Le.x == face[num][j].Le.x &&
		face[num][m].Le.y == face[num][j].Le.y &&
		face[num][m].Re.x == face[num][i].Re.x &&
		face[num][m].Re.y == face[num][i].Re.y) ||
	      (face[num][m].Le.x == face[num][i].Le.x &&
		face[num][m].Le.y == face[num][i].Le.y &&
		face[num][m].Re.x == face[num][j].Re.x &&
		face[num][m].Re.y == face[num][j].Re.y))
	      face[num][m].Isface = 0;

	  face[num][j].Isface = 0;
	}

	if (face[num][i].Isface == 1 && face[num][j].Isface == 1 &&
	  face[num][i].Le.x == face[num][j].Lb.x &&
	  face[num][i].Le.y == face[num][j].Lb.y &&
	  face[num][i].Re.x == face[num][j].Rb.x &&
	  face[num][i].Re.y == face[num][j].Rb.y &&
	  (abs (face[num][i].Mouth.x - face[num][j].Nose.x) < 3 &&
	    abs (face[num][i].Mouth.y - face[num][j].Nose.y) < 3)) {

	  for (m = 0; m < facenum[num] && m != i && m != j; m++)
	    if (face[num][m].Isface == 1 &&
	      abs (face[num][m].Mouth.x - face[num][i].Mouth.x) < 3 &&
	      abs (face[num][m].Mouth.y - face[num][i].Mouth.y) < 3 &&
	      (face[num][m].Le.x == face[num][i].Le.x &&
		face[num][m].Le.y == face[num][i].Le.y &&
		face[num][m].Re.x == face[num][j].Re.x &&
		face[num][m].Re.y == face[num][j].Re.y) ||
	      (face[num][m].Le.x == face[num][j].Le.x &&
		face[num][m].Le.y == face[num][j].Le.y &&
		face[num][m].Re.x == face[num][i].Re.x &&
		face[num][m].Re.y == face[num][i].Re.y))
	      face[num][m].Isface = 0;

	  face[num][i].Isface = 0;
	}

	if (face[num][i].Isface == 1 && face[num][j].Isface == 1 &&
	  face[num][j].Le.x == face[num][i].Lb.x &&
	  face[num][j].Le.y == face[num][i].Lb.y &&
	  face[num][j].Re.x == face[num][i].Rb.x &&
	  face[num][j].Re.y == face[num][i].Rb.y &&
	  (abs (face[num][i].Nose.x - face[num][j].Mouth.x) < 3 &&
	    abs (face[num][i].Nose.y - face[num][j].Mouth.y) < 3)) {

	  for (m = 0; m < facenum[num] && m != i && m != j; m++)
	    if (face[num][m].Isface == 1 &&
	      abs (face[num][m].Mouth.x - face[num][j].Mouth.x) < 3 &&
	      abs (face[num][m].Mouth.y - face[num][j].Mouth.y) < 3 &&
	      (face[num][m].Le.x == face[num][j].Le.x &&
		face[num][m].Le.y == face[num][j].Le.y &&
		face[num][m].Re.x == face[num][i].Re.x &&
		face[num][m].Re.y == face[num][i].Re.y) ||
	      (face[num][m].Le.x == face[num][i].Le.x &&
		face[num][m].Le.y == face[num][i].Le.y &&
		face[num][m].Re.x == face[num][j].Re.x &&
		face[num][m].Re.y == face[num][j].Re.y))
	      face[num][m].Isface = 0;

	  face[num][j].Isface = 0;
	}

	if (face[num][i].Isface == 1 && face[num][j].Isface == 1 &&
	  face[num][i].Re.x == face[num][j].Re.x &&
	  face[num][i].Re.y == face[num][j].Re.y &&
	  face[num][i].Le.x == face[num][j].Le.x &&
	  face[num][i].Le.y == face[num][j].Le.y &&
	  (abs (face[num][i].Mouth.x - face[num][j].Nose.x) < 3 &&
	    abs (face[num][i].Mouth.y - face[num][j].Nose.y) < 3) &&
	  face[num][i].Nose.count > 0)
	  face[num][j].Isface = 0;

	if (face[num][i].Isface == 1 && face[num][j].Isface == 1 &&
	  face[num][i].Re.x == face[num][j].Re.x &&
	  face[num][i].Re.y == face[num][j].Re.y &&
	  face[num][i].Le.x == face[num][j].Le.x &&
	  face[num][i].Le.y == face[num][j].Le.y &&
	  (abs (face[num][i].Nose.x - face[num][j].Mouth.x) < 3 &&
	    abs (face[num][i].Nose.y - face[num][j].Mouth.y) < 3) &&
	  face[num][j].Nose.count > 0)
	  face[num][i].Isface = 0;

	if (face[num][i].Isface == 1 && face[num][j].Isface == 1 &&
	  face[num][i].Re.x == face[num][j].Re.x &&
	  face[num][i].Re.y == face[num][j].Re.y &&
	  face[num][i].Le.x == face[num][j].Lb.x &&
	  face[num][i].Le.y == face[num][j].Lb.y &&
	  (abs (face[num][i].Mouth.x - face[num][j].Nose.x) < 3 &&
	    abs (face[num][i].Mouth.y - face[num][j].Nose.y) < 3) &&
	  face[num][i].Nose.count == 0)
	  face[num][i].Isface = 0;

	if (face[num][i].Isface == 1 && face[num][j].Isface == 1 &&
	  face[num][i].Re.x == face[num][j].Re.x &&
	  face[num][i].Re.y == face[num][j].Re.y &&
	  face[num][i].Lb.x == face[num][j].Le.x &&
	  face[num][i].Lb.y == face[num][j].Le.y &&
	  (abs (face[num][i].Nose.x - face[num][j].Mouth.x) < 3 &&
	    abs (face[num][i].Nose.y - face[num][j].Mouth.y) < 3) &&
	  face[num][j].Nose.count == 0)
	  face[num][j].Isface = 0;

	if (face[num][i].Isface == 1 && face[num][j].Isface == 1 &&
	  face[num][i].Rb.x == face[num][j].Re.x &&
	  face[num][i].Rb.y == face[num][j].Re.y &&
	  face[num][i].Le.x == face[num][j].Le.x &&
	  face[num][i].Le.y == face[num][j].Le.y &&
	  (abs (face[num][j].Mouth.x - face[num][i].Nose.x) < 3 &&
	    abs (face[num][j].Mouth.y - face[num][i].Nose.y) < 3) &&
	  face[num][j].Nose.count == 0)
	  face[num][j].Isface = 0;

	if (face[num][i].Isface == 1 && face[num][j].Isface == 1 &&
	  face[num][i].Re.x == face[num][j].Rb.x &&
	  face[num][i].Re.y == face[num][j].Rb.y &&
	  face[num][i].Le.x == face[num][j].Le.x &&
	  face[num][i].Le.y == face[num][j].Le.y &&
	  (abs (face[num][j].Nose.x - face[num][i].Mouth.x) < 3 &&
	    abs (face[num][j].Nose.y - face[num][i].Mouth.y) < 3) &&
	  face[num][i].Nose.count == 0)
	  face[num][i].Isface = 0;



      }
  }
  return;
}


int Cface_detect::searchpoints (CP Le, CP Re, CP Points[POINTS], int num)
{
  int k, m, n;
  int x1, x2, x3, x4, y1, y2, y3, y4, Length;
  float k1, k2, k3, k4;
  double SIN, COS;
  int pointnum;
  int Lx, Rx, Uy, Dy;
  float delta;

  x1 = Le.x;
  y1 = Le.y;
  x2 = Re.x;
  y2 = Re.y;

  SIN = sin (fabs (atan ((y2 - y1) / ((float) (x2 - x1)))));
  COS = cos (fabs (atan ((y2 - y1) / ((float) (x2 - x1)))));
  delta = (180 * fabs (atan ((y2 - y1) / ((float) (x2 - x1))) / PI));

  Length = (int) (sqrt ((double)(x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)) + 0.5);

  if (delta > 2.0 && y2 > y1) {
    x3 = x1 - (int) (preH * Length * SIN + 0.5);
    y3 = y1 + (int) (preH * Length * COS + 0.5);

    x4 = x2 - (int) (preH * Length * SIN + 0.5);
    y4 = y2 + (int) (preH * Length * COS + 0.5);

    k1 = (y2 - y4) / ((float) (x2 - x4));
    k2 = k1;	//(y1-y3)/((float)(x1-x3));
    k3 = -1 / k1;	//(y2-y1)/((float)(x2-x1));
    k4 = k3;	//(y4-y3)/((float)(x4-x3));

    pointnum = 0;
    for (k = 0; k < number[num]; k++) {
      m = x1 + (int) ((centerPoints[num][k].y - y1) / k2 + 0.5);
      n = x3 + (int) ((centerPoints[num][k].y - y3) / k4 + 0.5);

      if (m > n)
	Lx = m;
      else
	Lx = n;

      m = x2 + (int) ((centerPoints[num][k].y - y2) / k1 + 0.5);
      n = x1 + (int) ((centerPoints[num][k].y - y1) / k3 + 0.5);

      if (m > n)
	Rx = n;
      else
	Rx = m;

      m = y1 + (int) (k2 * (centerPoints[num][k].x - x1) + 0.5);
      n = y1 + (int) (k3 * (centerPoints[num][k].x - x1) + 0.5);

      if (m > n)
	Uy = m;
      else
	Uy = n;

      m = y2 + (int) (k1 * (centerPoints[num][k].x - x2) + 0.5);
      n = y3 + (int) (k4 * (centerPoints[num][k].x - x3) + 0.5);

      if (m > n)
	Dy = n;
      else
	Dy = m;

      if (centerPoints[num][k].x > Lx && centerPoints[num][k].x < Rx &&
	centerPoints[num][k].y > Uy && centerPoints[num][k].y < Dy) {
	Points[pointnum] = centerPoints[num][k];
	pointnum++;
      }
    }

    return (pointnum);
  }
  else if (delta > 2.0 && y2 < y1) {
    x3 = x1 + (int) (preH * Length * SIN + 0.5);
    y3 = y1 + (int) (preH * Length * COS + 0.5);

    x4 = x2 + (int) (preH * Length * SIN + 0.5);
    y4 = y2 + (int) (preH * Length * COS + 0.5);

    k1 = (y2 - y4) / ((float) (x2 - x4));
    k2 = k1;	//(y1-y3)/((float)(x1-x3));
    k3 = -1 / k1;	//(y2-y1)/((float)(x2-x1));
    k4 = k3;	//(y4-y3)/((float)(x4-x3));

    pointnum = 0;
    for (k = 0; k < number[num]; k++) {
      m = x1 + (int) ((centerPoints[num][k].y - y1) / k2 + 0.5);
      n = x1 + (int) ((centerPoints[num][k].y - y1) / k3 + 0.5);

      if (m > n)
	Lx = m;
      else
	Lx = n;

      m = x2 + (int) ((centerPoints[num][k].y - y2) / k1 + 0.5);
      n = x3 + (int) ((centerPoints[num][k].y - y3) / k4 + 0.5);

      if (m > n)
	Rx = n;
      else
	Rx = m;

      m = y2 + (int) (k1 * (centerPoints[num][k].x - x2) + 0.5);
      n = y1 + (int) (k3 * (centerPoints[num][k].x - x1) + 0.5);

      if (m > n)
	Uy = m;
      else
	Uy = n;

      m = y1 + (int) (k2 * (centerPoints[num][k].x - x1) + 0.5);
      n = y3 + (int) (k4 * (centerPoints[num][k].x - x3) + 0.5);

      if (m > n)
	Dy = n;
      else
	Dy = m;

      if (centerPoints[num][k].x > Lx && centerPoints[num][k].x < Rx &&
	centerPoints[num][k].y > Uy && centerPoints[num][k].y < Dy) {
	Points[pointnum] = centerPoints[num][k];
	pointnum++;
      }
    }

    return (pointnum);
  }

  else {

    if (y1 > y2)
      m = y1;
    else
      m = y2;

    pointnum = 0;
    for (k = 0; k < number[num]; k++)
      if (centerPoints[num][k].x > x1 && centerPoints[num][k].x < x2 &&
	centerPoints[num][k].y > m
	&& centerPoints[num][k].y < m + preH * Length) {
	Points[pointnum] = centerPoints[num][k];
	pointnum++;
      }

    return (pointnum);
  }

}	//end

int Cface_detect::scan1 (CP P, CP Points[POINTS], double delta, int h, int num,
  int direct)
{
  int i, k;


  if (direct == 0) {
    if ((P.y + h) > imgheight)
      return -1;
    else {

      k = 0;
      for (i = 0; i < number[num]; i++)
	if ((centerPoints[num][i].y - P.y) < h
	  && (centerPoints[num][i].y - P.y) > 0
	  && fabs ((double)centerPoints[num][i].x - P.x) <
	  (centerPoints[num][i].y - P.y) * tan (delta)) {
	  Points[k] = centerPoints[num][i];
	  k++;
	}

      return k;
    }
  }

  else if (direct == 1) {
    if ((P.x + h) > imgwidth)
      return -1;
    else {

      k = 0;
      for (i = 0; i < number[num]; i++) {
//              if(centerPoints[num][i].x==140 && centerPoints[num][i].y==73)
//                      printf("!");

	if ((centerPoints[num][i].x - P.x) < h
	  && (centerPoints[num][i].x - P.x) > 0
	  && abs (centerPoints[num][i].y - P.y) <
	  (int) ((centerPoints[num][i].x - P.x) * tan (delta) + 0.5)) {
	  Points[k] = centerPoints[num][i];
	  k++;
	}
      }
      return k;
    }
  }
  else
    return -1;


}

int Cface_detect::scan2 (CP P, CP Points[POINTS], double delta, int h1, int h2,
  int num)
{
  int i, k;


  if ((P.x + h2) > imgwidth)
    return -1;
  else {

    k = 0;
    for (i = 0; i < number[num]; i++)
      if ((centerPoints[num][i].x - P.x) < h2
	&& (centerPoints[num][i].x - P.x) >= h1
	&& abs (centerPoints[num][i].y - P.y) <
	(int) ((centerPoints[num][i].x - P.x) * tan (delta))) {
	Points[k] = centerPoints[num][i];
	k++;
      }

    return k;
  }

}

void Cface_detect::adjust1 (CP P, CP Points[POINTS], int m, int n)
{
  int i, j;
  CP P1;

  for (j = 1; j <= n; j++)
    for (i = m - 1; i >= j; i--)
      if (Points[i].x < Points[i - 1].x) {
	P1 = Points[i];
	Points[i] = Points[i - 1];
	Points[i - 1] = P1;
      }


  return;
}

void Cface_detect::adjust2 (CP Points[POINTS], int m, int n)
{
  int i, j;
  CP P1;

  for (j = 1; j <= n; j++)
    for (i = m - 1; i >= j; i--)
      if (Points[i].y < Points[i - 1].y) {
	P1 = Points[i];
	Points[i] = Points[i - 1];
	Points[i - 1] = P1;
      }


  return;
}

void Cface_detect::drawline1 (unsigned char **image, CP Le, CP Re)
{
  int i, j, k, l;
  int L, R, U, D;
  CP t;

  if (Le.x > Re.x) {
    t = Le;
    Le = Re;
    Re = t;
  }

  if (Le.y > Re.y) {
    U = Re.y;
    D = Le.y;
  }
  else if (Le.y < Re.y) {
    U = Le.y;
    D = Re.y;
  }
  else {
    U = Le.y;
    D = U;
  }

  L = Le.x;
  R = Re.x;

  if (U == D) {
    for (i = L; i <= R; i++)
      image[U][i] = 255;
  }
  else {

    for (i = U + 1; i < D; i++) {
      k =
	(int) ((Re.x - Le.x) * (i - Le.y) / (float) (Re.y - Le.y) + Le.x + 0.5);
      image[i][k] = 255;
    }
    for (i = L + 1; i < R; i++) {
      k =
	(int) (((Le.y - Re.y) * (i - Le.x)) / ((float) (Le.x - Re.x)) + Le.y +
	0.5);
      image[k][i] = 255;
    }

  }
  return;
}

//////////////////////////////////////////////////
// initialize const array for confirm face start
///////////////////////////////////////////////////
double H[5] = { 0.1281, -0.0115, -0.2333, -0.0115, 0.1281 };
unsigned char temp_img[640][640];

const int HVI3[LHV3] = { 11, 11, 11, 11, 11, 11, 11, 11 };

const int HVJ3[LHV3] = { 1, 2, 3, 4, 19, 20, 21, 22 };

const int HVIM3[LHV3] = { 17, 17, 18, 20, 17, 18, 20, 17 };

const int HVJM3[LHV3] = { 0, 1, 1, 1, 22, 22, 22, 23 };

const int HVI2[LHV2] =
  { 11, 10, 11, 10, 11, 10, 11, 10, 11, 10, 11, 10, 11, 11, 10, 10, 11, 10,
  11, 10, 11, 10, 11, 10, 11, 11
};
const int HVJ2[LHV2] =
  { 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 7, 16, 17, 18, 18, 19, 19, 20, 20, 21,
    21,
  22, 22, 23
};

const int HVIM2[LHV2] =
  { 15, 16, 16, 17, 19, 20, 17, 18, 19, 17, 18, 19, 16, 16, 17, 18,
  19, 17, 18, 19, 16, 17, 19, 20, 15, 16
};

const int HVJM2[LHV2] =
  { 0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 5, 18, 20, 20, 20, 21, 21, 21, 22,
  22, 22, 22, 23, 23
};


const int HVI1[LHV] =
  { 6, 7, 8, 11, 12, 6, 7, 8, 10, 11, 12, 6, 7, 8, 10, 11, 12, 6, 7, 8, 10, 11,
    12,
  6, 7, 8, 10, 11, 12, 6, 7, 8, 9, 10, 11, 12, 7, 8, 9, 10, 11, 12, 13, 27, 28,
  29, 7, 8, 9, 10, 11, 12, 27, 28, 29, 30, 8, 9, 10, 11, 12, 22, 23, 27, 28, 29,
  30, 8, 9, 10, 22, 23, 24, 27, 28, 29, 30, 31, 22, 23, 24, 25, 27, 28, 29, 30,
  31, 22, 23, 24, 25, 27, 28, 29, 30, 31, 22, 23, 24, 25, 27, 28, 29, 30, 31,
  22, 23, 24, 25, 27, 28, 29, 30, 31, 8, 9, 10, 22, 23, 24, 27, 28, 29, 30, 31,
  8, 9, 10, 11, 12, 22, 23, 27, 28, 29, 30, 7, 8, 9, 10, 11, 12, 27, 28, 29, 30,
  7, 8, 9, 10, 11, 12, 13, 27, 28, 29, 6, 7, 8, 9, 10, 11, 12, 6, 7, 8, 10, 11,
    12,
  6, 7, 8, 10, 11, 12, 6, 7, 8, 10, 11, 12, 6, 7, 8, 10, 11, 12, 6, 7, 8, 11, 12
};

const int HVJ1[LHV] =
  { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4,
    4, 4, 4, 4,
  5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10,
    10, 10,
  10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12,
  12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14,
  14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18,
  18, 18, 18, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21,
  21, 21, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23
};

const int HVIM1[LHV] =
  { 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 15, 16,
  17, 18, 19, 20, 21, 22, 23, 26, 27, 28, 29, 15, 16, 17, 18, 19, 20, 21, 22,
  23, 27, 28, 15, 16, 17, 18, 19, 20, 21, 15, 16, 17, 18, 19, 20, 15, 16, 17,
  18, 19, 15, 16, 17, 18, 14, 15, 16, 17, 18, 5, 14, 15, 16, 17, 18, 5, 14, 15,
  16, 17, 18, 5, 6, 13, 14, 15, 16, 17, 18, 5, 6, 12, 13, 14, 15, 16, 17, 18, 5,
  6, 12, 13, 14, 15, 16, 17, 18, 5, 6, 13, 14, 15, 16, 17, 18, 5, 14, 15, 16,
    17,
  18, 5, 14, 15, 16, 17, 18, 14, 15, 16, 17, 18, 15, 16, 17, 18, 15, 16, 17, 18,
  19, 15, 16, 17, 18, 19, 20, 15, 16, 17, 18, 19, 20, 21, 15, 16, 17, 18, 19,
    20,
  21, 22, 23, 27, 28, 15, 16, 17, 18, 19, 20, 21, 22, 23, 26, 27, 28, 29, 14,
    15,
  16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29
};

const int HVJM1[LHV] =
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1,
  1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5,
    5, 5, 5,
  5, 6, 6, 6, 6, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 10, 10, 10,
    10, 10,
  10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12,
    12,
  12, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15,
    15,
  15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19,
    19,
  19, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
    22,
  22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23,
    23,
  23, 23, 23, 23, 23, 23, 23, 23
};

const int HRI1[LHR] =
  { 5, 9, 10, 14, 15, 16, 5, 8, 9, 13, 14, 15, 16, 17, 5, 8, 9, 13, 14, 15, 16,
    17,
  5, 8, 9, 13, 14, 15, 16, 17, 5, 6, 9, 13, 14, 15, 16, 17, 18, 5, 6, 9, 14, 15,
  16, 17, 18, 5, 6, 14, 15, 16, 17, 18, 24, 25, 26, 31, 5, 6, 14, 15, 16, 17,
    24,
  25, 26, 27, 31, 5, 6, 14, 15, 16, 24, 25, 26, 27, 5, 6, 20, 21, 24, 25, 26,
    27,
  20, 21, 25, 26, 27, 20, 21, 25, 26, 27, 20, 21, 25, 26, 27, 20, 21, 25, 26,
    27,
  5, 6, 20, 21, 24, 25, 26, 27, 5, 6, 14, 15, 16, 24, 25, 26, 27, 5, 6, 14, 15,
    16,
  17, 24, 25, 26, 27, 31, 5, 6, 14, 15, 16, 17, 18, 24, 25, 26, 31, 5, 6, 9, 14,
  15, 16, 17, 18, 5, 6, 9, 13, 14, 15, 16, 17, 18, 5, 8, 9, 13, 14, 15, 16, 17,
    5,
  8, 9, 13, 14, 15, 16, 17, 5, 8, 9, 13, 14, 15, 16, 17, 5, 9, 10, 14, 15, 16
};


const int HRJ1[LHR] =
  { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3,
    3, 3, 3, 3,
  3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9,
    9, 9, 10,
  10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13,
  14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
  17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20,
  20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22,
  22, 22, 22, 22, 23, 23, 23, 23, 23, 23
};

const int HRIM1[LHR] =
  { 11, 12, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 11, 12, 20, 21,
  22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 11, 21, 22, 23, 24, 25, 26, 27, 28,
  31, 11, 12, 21, 22, 23, 24, 25, 26, 27, 11, 12, 21, 22, 23, 24, 11, 21, 22,
  23, 11, 20, 21, 22, 8, 11, 12, 21, 8, 9, 10, 11, 12, 8, 9, 10, 11, 12, 8, 9,
  10, 11, 12, 13, 14, 15, 16, 17, 7, 8, 9, 10, 11, 13, 14, 15, 16, 17, 7, 8, 9,
  10, 11, 13, 14, 15, 16, 17, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 8, 9, 10,
  11, 12, 8, 9, 10, 11, 12, 8, 11, 12, 21, 11, 20, 21, 22, 11, 21, 22, 23, 11,
  12, 21, 22, 23, 24, 11, 12, 21, 22, 23, 24, 25, 26, 27, 11, 21, 22, 23, 24,
  25, 26, 27, 28, 31, 11, 12, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
  11, 12, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
};

const int HRJM1[LHR] =
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5,
    5, 5, 5,
  6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10,
    10,
  10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12,
  12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14,
  15, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19,
  19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21,
  21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23,
  23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23
};

const int VVI1[LVV] =
  { 11, 12, 6, 10, 11, 13, 6, 7, 10, 11, 12, 13, 14, 6, 7, 8, 9, 10, 11, 12,
  13, 14, 15, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 7, 8, 9, 10, 12, 13, 14, 21,
    28,
  7, 8, 9, 10, 11, 12, 13, 18, 19, 20, 21, 22, 27, 28, 29, 30, 8, 9, 10, 11, 12,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 27, 28, 29, 30, 10, 11, 13, 14, 15,
    16,
  17, 18, 19, 20, 21, 22, 23, 24, 28, 29, 30, 23, 24, 23, 24, 10, 11, 13, 14,
    15,
  16, 17, 18, 19, 20, 21, 22, 23, 24, 28, 29, 30, 8, 9, 10, 11, 12, 15, 16, 17,
  18, 19, 20, 21, 22, 23, 24, 27, 28, 29, 30, 7, 8, 9, 10, 11, 12, 13, 18, 19,
  20, 21, 22, 27, 28, 29, 30, 7, 8, 9, 10, 12, 13, 14, 21, 28, 6, 7, 8, 9, 10,
    11,
  12, 13, 14, 15, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 6, 7, 10, 11, 12, 13, 14,
    6,
  10, 11, 13, 11, 12
};

const int VVJ1[LVV] =
  { 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5,
    5, 5,
  5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7,
  7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9,
    9, 9, 9,
  9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14,
  14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18,
  18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20,
  20, 20, 20, 21, 21, 21, 21, 22, 22
};

const int VVIM1[LVV] =
  { 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 15, 16, 17, 18, 19, 20, 21, 22,
  23, 24, 25, 26, 27, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
  17, 18, 19, 20, 21, 22, 23, 24, 26, 27, 28, 29, 30, 18, 19, 20, 21, 22, 28,
  29, 30, 31, 17, 18, 19, 24, 30, 31, 16, 17, 24, 5, 5, 6, 5, 6, 7, 5, 6, 7, 8,
    9,
  10, 11, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 5, 6, 7, 8, 9, 10,
  11, 12, 13, 14, 15, 16, 17, 18, 5, 6, 7, 8, 9, 10, 11, 5, 6, 7, 5, 6, 5, 16,
    17,
  24, 17, 18, 19, 24, 30, 31, 18, 19, 20, 21, 22, 28, 29, 30, 31, 17, 18, 19,
  20, 21, 22, 23, 24, 26, 27, 28, 29, 30, 16, 17, 18, 19, 20, 21, 22, 23, 24,
  25, 26, 27, 28, 29, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 14,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24
};

const int VVJM1[LVV] =
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
    2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4,
    4, 4,
  4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 7, 8, 8, 9, 9, 9, 10, 10, 10, 10, 10,
    10,
  10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12,
  12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 14, 14,
  14, 15, 15, 16, 17, 17, 17, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19,
  19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21,
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22,
  22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23
};

const int VRI1[LVR] =
  { 14, 15, 16, 17, 18, 19, 20, 21, 22, 8, 9, 15, 16, 17, 18, 19, 20, 21, 22,
  23, 24, 25, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 18, 19, 20,
  21, 22, 23, 26, 27, 28, 29, 30, 31, 18, 19, 20, 21, 22, 23, 27, 28, 29, 30,
  31, 18, 19, 20, 23, 24, 29, 30, 31, 23, 24, 6, 6, 7, 8, 6, 7, 8, 9, 10, 11,
    12,
  13, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 6, 7, 8, 9,
    10,
  11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 6, 7, 8, 9, 10, 11, 12, 13, 6, 7,
  8, 6, 23, 24, 18, 19, 20, 23, 24, 29, 30, 31, 18, 19, 20, 21, 22, 23, 27, 28,
  29, 30, 31, 18, 19, 20, 21, 22, 23, 26, 27, 28, 29, 30, 31, 18, 19, 20, 21,
  22, 23, 24, 25, 26, 27, 28, 29, 30, 8, 9, 15, 16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 14, 15, 16, 17, 18, 19, 20, 21, 22
};

const int VRJ1[LVR] =
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
    2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4,
    4, 4,
  4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 8, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10,
  10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12,
  12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13,
  13, 13, 13, 14, 14, 14, 15, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19,
  19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
  20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22,
  22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23
};

const int VRIM1[LVR] =
  { 12, 11, 12, 6, 10, 14, 5, 6, 7, 10, 11, 12, 13, 14, 15, 16, 5, 6, 7, 8, 9,
    10,
  11, 12, 13, 14, 15, 16, 6, 7, 8, 9, 10, 11, 13, 14, 15, 6, 7, 8, 9, 10, 14, 7,
  8, 9, 10, 12, 17, 18, 19, 20, 21, 27, 28, 29, 30, 9, 10, 11, 15, 16, 17, 18,
  19, 20, 21, 22, 23, 27, 28, 29, 30, 31, 14, 15, 16, 17, 18, 19, 20, 21, 22,
  23, 24, 25, 26, 27, 28, 29, 30, 31, 24, 25, 31, 31, 31, 24, 25, 31, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 9, 10, 11,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 27, 28, 29, 30, 31, 7, 8, 9, 10, 12, 17,
  18, 19, 20, 21, 27, 28, 29, 30, 6, 7, 8, 9, 10, 14, 6, 7, 8, 9, 10, 11, 13,
  14, 15, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 5, 6, 7, 10, 11, 12, 13,
  14, 15, 16, 6, 10, 14, 11, 12, 12
};

const int VRJM1[LVR] =
  { 0, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7,
  7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9,
  9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 11, 12, 13, 13, 13, 14, 14, 14, 14, 14,
  14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18,
  18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20,
  20, 20, 20, 20, 20, 21, 21, 21, 22, 22, 23
};

double hv_coeff[54] = { 11, 15, 6, 6, 15, 11,
  -3, 1, -8, -8, 1, -3,
  -30, 0, -12, -12, 0, -30,
  -2, 21, -28, -28, 21, -2,
  -5, 1, -17, -17, 1, -5,
  -6, -11, 17, 17, -11, -6,
  -25, -4, 8, 8, -4, -25,
  -1, 6, 19, 19, 6, -1,
  -20, -6, 19, 19, -6, -20
};

double hr_coeff[54] = { 18, 7, -18, -18, 7, 18,
  8, 16, -16, -16, 16, 8,
  34, -7, -4, -4, -7, 34,
  17, 16, -16, -16, 16, 17,
  5, 3, -3, -3, 3, 5,
  -9, 4, 26, 26, 4, -9,
  -19, 6, 3, 3, 6, -19,
  3, 13, -2, -2, 13, 3,
  -13, 8, 8, 8, 8, -13
};

double vv_coeff[54] = { 6, -11, 3, 3, -11, 6,
  -4, 4, 7, 7, 4, -4,
  -5, 7, 21, 21, 7, -5,
  1, 0, 20, 20, 0, 1,
  5, -5, 4, 4, -5, 5,
  -6, 9, -8, -8, 9, -6,
  17, -2, -7, -7, -2, 17,
  10, -1, -14, -14, -1, 10,
  17, -3, -19, -19, -3, 17
};

double vr_coeff[54] = { 7, -1, 21, 21, -1, 7,
  7, 1, 11, 11, 1, 7,
  -27, 13, -1, -1, 13, -27,
  -6, -16, 22, 22, -16, -6,
  -13, 1, 8, 8, 1, -13,
  10, 13, -9, -9, 13, 10,
  9, -2, -6, -6, -2, 9,
  3, -1, -10, -10, -1, 3,
  9, -1, -9, -9, -1, 9
};
//////////////////////////////////////////////////
// initialize const array for confirm face end
///////////////////////////////////////////////////
int Cface_detect::call_confirm (int candidatesnum)
{
  int i, j;
  char **u_img = NULL, **v_img = NULL;	//[320][320];
  int coarse_eyes[1000][4], fine_eyes[1000][4];
  int finenum;
  u_img = new char *[imgheight];
  if (!u_img)
    return (0);
  for (i = 0; i < imgheight; i++) {
    u_img[i] = new char[imgwidth];
    if (!u_img[i])
      return (0);
  }
  v_img = new char *[imgheight];
  if (!v_img)
    return (0);
  for (i = 0; i < imgheight; i++) {
    v_img[i] = new char[imgwidth];
    if (!v_img[i])
      return (0);
  }

  for (i = 0; i < imgheight; i++) {
    for (j = 0; j < imgwidth; j++) {
      u_img[i][j] =
	(char) (-0.1688 * (double) R[i][j] - 0.3312 * (double) G[i][j] +
	0.5 * (double) B[i][j]);
      v_img[i][j] =
	(char) (0.5 * (double) R[i][j] - 0.4186 * (double) G[i][j] -
	0.0814 * (double) B[i][j]);
    }
  }
  j = 0;
  for (i = 0; i < candidatesnum; i++) {
    if (candidates[i].eyesdis >= 12 && candidates[i].eyesdis < 90) {
      coarse_eyes[j][0] = candidates[i].Le.x;
      coarse_eyes[j][1] = candidates[i].Le.y;
      coarse_eyes[j][2] = candidates[i].Re.x;
      coarse_eyes[j][3] = candidates[i].Re.y;
      j++;
    }
  }
  confirm_face (inImage0, u_img, v_img, imgheight, imgwidth, candidatesnum,
    coarse_eyes, &finenum, fine_eyes);
  for (i = 0; i < finenum; i++) {
    Face[i].Le.x = fine_eyes[i][0];
    Face[i].Le.y = fine_eyes[i][1];
    Face[i].Re.x = fine_eyes[i][2];
    Face[i].Re.y = fine_eyes[i][3];
    Face[i].Isface = 1;
  }
  for (i = 0; i < imgheight; i++) {
    delete v_img[i];
    v_img[i] = NULL;
  }
  delete v_img;
  v_img = NULL;
  for (i = 0; i < imgheight; i++) {
    delete u_img[i];
    u_img[i] = NULL;
  }
  delete u_img;
  u_img = NULL;

  return finenum;
}

void Cface_detect::fhor_filter (double h[], unsigned char img[72][54], int l,
  int n, int m, int off)
{

  int i, j, k, hl;
  double sum;

  hl = (l - 1) / 2;

  for (i = 0; i < 320; ++i) {
    for (j = 0; j < 342; ++j) {
      temp_img[i][j] = 0;
    }
  }


  for (i = 0; i < n; ++i) {
    for (j = 0; j < m; ++j) {
      temp_img[i][j + 11] = img[i][j];
      img[i][j] = 0;
    }
  }


  for (i = 0; i < n; ++i) {
    for (j = 11 - hl; j < m + 11 - hl; ++j) {
      sum = 0;
      for (k = 0; k < l; ++k) {
	sum += temp_img[i][j + k] * h[k];
      }
      img[i][j - 11 + hl] += (unsigned char) (sum + off + .5);
    }
  }
  return;
}

void Cface_detect::fver_filter (double h[], unsigned char img[72][54], int l,
  int n, int m, int off)
{

  int i, j, k, hl;
  double sum;

  hl = (l - 1) / 2;

  for (i = 0; i < 320; ++i) {
    for (j = 0; j < 342; ++j) {
      temp_img[i][j] = 0;
    }
  }

  for (i = 0; i < n; ++i) {
    for (j = 0; j < m; ++j) {
      temp_img[i + 11][j] = img[i][j];
      img[i][j] = 0;
    }
  }


  for (i = 11 - hl; i < n + 11 - hl; ++i) {
    for (j = 0; j < m; ++j) {
      sum = 0;
      for (k = 0; k < l; ++k) {
	sum += temp_img[i + k][j] * h[k];
      }

      img[i - 11 + hl][j] += (unsigned char) (sum + off + .5);
    }
  }
  return;
}

void Cface_detect::designfilter (double Wn, double b[11])
{
  int i;
  double wind[11], gain;



  for (i = 0; i < 11; ++i) {
    wind[i] = .54 - .46 * cos (2 * PI * i / 10);
  }


  b[5] = Wn * wind[5];


  for (i = 1; i <= 5; ++i) {
    b[5 + i] = (sin (i * PI * Wn) / (i * PI)) * wind[5 + i];
    b[5 - i] = b[5 + i];
  }

  gain = 0;
  for (i = 0; i < 11; ++i) {
    gain += b[i];
  }

  gain = fabs (gain);


  for (i = 0; i < 11; ++i) {
    b[i] = b[i] / gain;
  }
  return;
}



void Cface_detect::fimrotate (unsigned char img[72][54], int n, int m,
  double ang)
{
  int i, j, x0, x1, y0, y1, top, left;
  double th, c, s, cx, cy, a, b, x, y, fx0, fx1;

  top = 321 - (n / 2);
  left = 321 - (m / 2);

  for (i = 0; i < 640; ++i) {
    for (j = 0; j < 640; ++j) {
      temp_img[i][j] = 0;
    }
  }

  for (i = 0; i < n; ++i) {
    for (j = 0; j < m; ++j) {
      temp_img[top + i][left + j] = img[i][j];
      img[i][j] = 0;
    }
  }


  th = -ang * PI / 180;

  c = cos (th);
  s = sin (th);

  cx = -c * 320.5 + s * 320.5 + 320.5;
  cy = -s * 320.5 - c * 320.5 + 320.5;

  for (i = top; i < top + n; ++i) {
    for (j = left; j < left + m; ++j) {

      a = c * j - s * i + cx;
      b = s * j + c * i + cy;

      x0 = (int) floor (a);
      x1 = x0 + 1;
      x = a - x0;

      y0 = (int) floor (b);
      y1 = y0 + 1;
      y = b - y0;

      fx0 =
	(unsigned char) (temp_img[y0][x0] + x * (temp_img[y0][x1] -
      temp_img[y0][x0]));

      fx1 =
	(unsigned char) (temp_img[y1][x0] + x * (temp_img[y1][x1] -
      temp_img[y1][x0]));

      img[i - top][j - left] = (unsigned char) (fx0 + y * (fx1 - fx0));


    }
  }
  return;
}

void Cface_detect::fimresize (unsigned char img[72][54], int n, int m, double s)
{
  int i, j, x0, x1, y0, y1, bot, right;
  double a, b, x, y, fx0, fx1;

  bot = (int) (n * s);
  right = (int) (m * s);

  for (i = 0; i < n; ++i) {
    for (j = 0; j < m; ++j) {
      temp_img[i][j] = img[i][j];
      img[i][j] = 0;
    }
  }


  for (i = 0; i < bot; ++i) {
    for (j = 0; j < right; ++j) {

      a = j / s;
      b = i / s;

      x0 = (int) floor (a);
      x1 = x0 + 1;
      x = a - x0;

      y0 = (int) floor (b);
      y1 = y0 + 1;
      y = b - y0;

      fx0 =
	(unsigned char) (temp_img[y0][x0] + x * (temp_img[y0][x1] -
      temp_img[y0][x0]));

      fx1 =
	(unsigned char) (temp_img[y1][x0] + x * (temp_img[y1][x1] -
      temp_img[y1][x0]));

      img[i][j] = (unsigned char) (fx0 + y * (fx1 - fx0));


    }
  }
  return;
}


void Cface_detect::split_face (char blk[27][24], double vec[54])
{
  int i, j, k, u, v;

  k = 0;

  for (i = 0; i < 27; i += 3) {
    for (j = 0; j < 24; j += 4) {
      vec[k] = 0;
      for (u = 0; u < 3; ++u) {
	for (v = 0; v < 4; ++v) {
	  vec[k] += blk[i + u][j + v];
	}
      }
      ++k;
    }
  }
  return;
}

void Cface_detect::lin_comb (double *hs, double vec[54], double coeff[54])
{
  int i;

  *hs = 0;
  for (i = 0; i < 54; ++i) {
    *hs += vec[i] * coeff[i];
  }
  return;
}


void Cface_detect::fcorr_filter (unsigned char img[72][54],
  char img_out[72][54], int n, int m, int fp_th, int corr_th)
{

  int i, j, k, l;
  int shv2, shvm2, shv1, shvm1, shr1, shrm1, svv1, svvm1, svr1, svrm1;
  int corr;
  double hv_vec[54], hr_vec[54], vv_vec[54], vr_vec[54], hvs, hrs, vvs, vrs;
  unsigned char loc_img[72][54];
  char hor_val_img[72][54];
  char hor_rid_img[72][54];
  char ver_val_img[72][54];
  char ver_rid_img[72][54];
  char hor_line_img[72][54];
  char hv_blk[27][24], hr_blk[27][24], vv_blk[27][24], vr_blk[27][24];



  for (i = 0; i < n; ++i) {
    for (j = 0; j < m; ++j) {
      loc_img[i][j] = img[i][j];
      img_out[i][j] = 0;
      hor_line_img[i][j] = 0;
    }
  }


  fver_filter (H, loc_img, 5, n, m, 128);

  for (i = 0; i < n; ++i) {
    for (j = 0; j < m; ++j) {


      if (loc_img[i][j] > 130) {
	hor_val_img[i][j] = 1;
      }
      else {
	hor_val_img[i][j] = -1;
      }

      if (loc_img[i][j] < 127) {
	hor_rid_img[i][j] = 1;
      }
      else {
	hor_rid_img[i][j] = -1;
      }
    }
  }

  for (i = 0; i < n; ++i) {
    for (j = 0; j < m; ++j) {
      loc_img[i][j] = img[i][j];
    }
  }

  fhor_filter (H, loc_img, 5, n, m, 128);

  for (i = 0; i < n; ++i) {
    for (j = 0; j < m; ++j) {


      if (loc_img[i][j] >= 129) {
	ver_val_img[i][j] = 1;
      }
      else {
	ver_val_img[i][j] = -1;
      }

      if (loc_img[i][j] <= 127) {
	ver_rid_img[i][j] = 1;
      }
      else {
	ver_rid_img[i][j] = -1;
      }

    }
  }

  for (i = 0; i < n - 32; ++i) {
    for (j = 0; j < m - 24; ++j) {

      shv2 = 0;
      shvm2 = 0;
      for (k = 0; k < LHV2; ++k) {
	shv2 += hor_val_img[i + HVI2[k]][j + HVJ2[k]];
      }
      for (k = 0; k < LHV2; ++k) {
	shvm2 += hor_val_img[i + HVIM2[k]][j + HVJM2[k]];
      }

      shv1 = 0;
      shvm1 = 0;

      if (shv2 - shvm2 > 20) {

	for (k = 0; k < LHV; ++k) {
	  shv1 += hor_val_img[i + HVI1[k]][j + HVJ1[k]];
	}
	for (k = 0; k < LHV; ++k) {
	  shvm1 += hor_val_img[i + HVIM1[k]][j + HVJM1[k]];
	}
      }

      shr1 = 0;
      shrm1 = 0;
      svv1 = 0;
      svvm1 = 0;
      svr1 = 0;
      svrm1 = 0;

      if (shv1 - shvm1 > 50) {
	for (k = 0; k < LHR; ++k) {
	  shr1 += hor_rid_img[i + HRI1[k]][j + HRJ1[k]];
	}
	for (k = 0; k < LHR; ++k) {
	  shrm1 += hor_rid_img[i + HRIM1[k]][j + HRJM1[k]];
	}

	for (k = 0; k < LVV; ++k) {
	  svv1 += ver_val_img[i + VVI1[k]][j + VVJ1[k]];
	}
	for (k = 0; k < LVV; ++k) {
	  svvm1 += ver_val_img[i + VVIM1[k]][j + VVJM1[k]];
	}

	for (k = 0; k < LVR; ++k) {
	  svr1 += ver_rid_img[i + VRI1[k]][j + VRJ1[k]];
	}
	for (k = 0; k < LVR; ++k) {
	  svrm1 += ver_rid_img[i + VRIM1[k]][j + VRJM1[k]];
	}
      }
      corr =
	(shv1 - shvm1) * (shr1 - shrm1) * (svv1 - svvm1) * (svr1 -
	svrm1) / 20000000;


      if (corr > corr_th &&
	(shr1 - shrm1) > 0 && (svv1 - svvm1) > 0 && (svr1 - svrm1) > 0) {

	for (k = 0; k < 27; ++k) {
	  for (l = 0; l < 24; ++l) {
	    hv_blk[k][l] = hor_val_img[i + 5 + k][j + l];
	    hr_blk[k][l] = hor_rid_img[i + 5 + k][j + l];
	    vv_blk[k][l] = ver_val_img[i + 5 + k][j + l];
	    vr_blk[k][l] = ver_rid_img[i + 5 + k][j + l];
	  }
	}

	split_face (hv_blk, hv_vec);
	split_face (hr_blk, hr_vec);
	split_face (vv_blk, vv_vec);
	split_face (vr_blk, vr_vec);

	hvs = 0;
	hrs = 0;
	vvs = 0;
	vrs = 0;

	lin_comb (&hvs, hv_vec, hv_coeff);
	lin_comb (&hrs, hr_vec, hr_coeff);
	lin_comb (&vvs, vv_vec, vv_coeff);
	lin_comb (&vrs, vr_vec, vr_coeff);

	if (hvs + hrs + vvs + vrs < fp_th) {
	  img_out[i + 16][j + 12] = 0;
	}
	else {
	  img_out[i + 16][j + 12] = corr;
	}
      }
    }
  }
  return;
}

void Cface_detect::confirm_face (unsigned char **orig_img,
  char **u_img,
  char **v_img,
  int n, int m,
  int coarse_N, int coarse_eyes[1000][4], int *pfine_N, int fine_eyes[1000][4])
{

  int frn, frm, i, j, k, mean_u, mean_v, fine_N;
  int wd, ht, fp_th, corr_th;
  int faces[1000][4];
  int face_cand[1000][6];
  int fnum_loc_faces;
  int floc_faces[1000][3];
  unsigned char cand_img[72][54];
  unsigned char fres_img[72][54];
  unsigned char fimg[72][54];
  char fcorr_img[72][54];
  double h[11], scale, fs, fsp, rot, uv_angle;
  double th, c, s, x_dis, y_dis, li, lj;

  int fi, fj;
  int num_face_cand, num_coarse, lx, ly, rx, ry, x0, x1, y0, y1;
  double eye_dis, eye_angle, cfx, cfy, fx, fy, xx, yy, fx0, fx1;

  num_face_cand = 0;

  corr_th = 2;
  fp_th = 0;


  for (num_coarse = 0; num_coarse < coarse_N; num_coarse++) {

    rx = coarse_eyes[num_coarse][0];
    ry = coarse_eyes[num_coarse][1];
    lx = coarse_eyes[num_coarse][2];
    ly = coarse_eyes[num_coarse][3];


    /* extract the region of the image containing the candidate face */

    eye_dis = sqrt (pow ((double)rx - lx, 2) + pow ((double)ry - ly, 2));

    scale = 16 / eye_dis;

    eye_angle = atan2 ((double)ly - ry, (double)lx - rx);


    cfx =
      rx + (7.5 / 16.0) * eye_dis * cos (-eye_angle) +
      0.25 * eye_dis * sin (-eye_angle);

    cfy =
      ry + (7.5 / 16.0) * eye_dis * sin (eye_angle) +
      0.25 * eye_dis * cos (eye_angle);

    for (fi = -24; fi < 24; ++fi) {
      for (fj = -18; fj < 18; ++fj) {
	fx =
	  cfx + (fj / 16.0) * eye_dis * cos (-eye_angle) +
	  (fi / 16.0) * eye_dis * sin (-eye_angle);

	fy =
	  cfy + (fj / 16.0) * eye_dis * sin (eye_angle) +
	  (fi / 16.0) * eye_dis * cos (eye_angle);

	x0 = (int) floor (fx);
	x1 = x0 + 1;
	xx = fx - x0;

	y0 = (int) floor (fy);
	y1 = y0 + 1;
	yy = fy - y0;

	if (y0 >= 0 && y1 < n && x0 >= 0 && x1 < m) {

	  fx0 =
	    (unsigned char) (orig_img[y0][x0] + xx * (orig_img[y0][x1] -
	  orig_img[y0][x0]));

	  fx1 =
	    (unsigned char) (orig_img[y1][x0] + xx * (orig_img[y1][x1] -
	  orig_img[y1][x0]));

	  cand_img[fi + 24][fj + 18] = (unsigned char) (fx0 + yy * (fx1 - fx0));

	}
	else {
	  cand_img[fi + 24][fj + 18] = 0;
	}
      }

    }


    /* resize the candidate face region to 1.33 1.0 and .977  */

    for (fsp = -0.0212; fsp <= 0.0212; fsp += 0.0212) {
      fs = pow (10.0, fsp);

      /* copy the candidate face region to a new array */

      for (fi = 0; fi < 48; ++fi) {
	for (fj = 0; fj < 36; ++fj) {
	  fres_img[fi][fj] = cand_img[fi][fj];
	}

      }

      /* filter and resize the face region */

      designfilter (fs, h);

      fhor_filter (h, fres_img, 11, 48, 36, 0);

      fver_filter (h, fres_img, 11, 48, 36, 0);

      fimresize (fres_img, 48, 36, fs);

      frn = (int) (48 * fs);
      frm = (int) (36 * fs);


      for (fi = 0; fi < frn; ++fi) {
	for (fj = 0; fj < frm; ++fj) {
	  fimg[fi][fj] = fres_img[fi][fj];
	}
      }


      /* find the face correlation */

      fcorr_filter (fimg, fcorr_img, frn, frm, fp_th, corr_th);

      fnum_loc_faces = 0;

      /* find the candidate face positions */

      for (fi = 16; fi < frn - 16; ++fi) {
	for (fj = 12; fj < frm - 12; ++fj) {

	  if (fcorr_img[fi][fj] > corr_th) {
	    if (fcorr_img[fi][fj] >= fcorr_img[fi - 1][fj - 1]
	      && fcorr_img[fi][fj] >= fcorr_img[fi - 1][fj]
	      && fcorr_img[fi][fj] >= fcorr_img[fi - 1][fj + 1]
	      && fcorr_img[fi][fj] >= fcorr_img[fi][fj - 1]
	      && fcorr_img[fi][fj] >= fcorr_img[fi][fj + 1]
	      && fcorr_img[fi][fj] >= fcorr_img[fi + 1][fj - 1]
	      && fcorr_img[fi][fj] >= fcorr_img[fi + 1][fj]
	      && fcorr_img[fi][fj] >= fcorr_img[fi + 1][fj + 1]) {

	      floc_faces[fnum_loc_faces][0] = fi;
	      floc_faces[fnum_loc_faces][1] = fj;
	      floc_faces[fnum_loc_faces][2] = (int) fcorr_img[fi][fj];

	      ++(fnum_loc_faces);

	    }
	  }
	}
      }

      /* store the location of the fine candidate faces */

      for (k = 0; k < fnum_loc_faces; ++k) {

	face_cand[num_face_cand][0] =
	  (int) ((floc_faces[k][0] / fs) + cfy - 24);
	face_cand[num_face_cand][1] =
	  (int) ((floc_faces[k][1] / fs) + cfx - 18);
	face_cand[num_face_cand][2] = floc_faces[k][2];
	face_cand[num_face_cand][3] = (int) (fs * scale * 100);
	face_cand[num_face_cand][4] = (int) (eye_angle * 100);
	face_cand[num_face_cand][5] = 1;


	++(num_face_cand);

      }
    }
  }


  /* eliminate multiple candidates at the same location but at different scales */

  for (i = 0; i < num_face_cand; ++i) {
    if (face_cand[i][5] == 1) {
      for (j = 0; j < num_face_cand; ++j) {
	if (face_cand[j][5] == 0)
	  continue;

	x_dis = fabs ((double)face_cand[i][1] - face_cand[j][1]);
	y_dis = fabs ((double)face_cand[i][0] - face_cand[j][0]);

	scale = ((double) face_cand[i][3]) / 100;

	if (i != j && face_cand[j][2] <= face_cand[i][2] &&
	  x_dis < (18.0 / scale) && y_dis < (24.0 / scale)) {
	  face_cand[j][5] = 0;
	}
      }
    }
  }

  /* eliminate non flesh-colored candidates */

  fine_N = 0;
  for (i = 0; i < num_face_cand; ++i) {
    if (face_cand[i][5] == 1) {

      scale = (double) (face_cand[i][3]) / 100;
      rot = (double) (face_cand[i][4]) / 100;

      mean_u = 0;
      mean_v = 0;

      wd = (int) (12 / scale);
      ht = (int) (16 / scale);
      for (fi = -ht; fi < ht; ++fi) {
	for (fj = -wd; fj < wd; ++fj) {
	if (face_cand[i][0] + fi >= 0 && face_cand[i][0] + fi < n &&
	    face_cand[i][1] + fj >= 0 && face_cand[i][1] + fj < m) {
	    mean_u += u_img[face_cand[i][0] + fi][face_cand[i][1] + fj];
	    mean_v += v_img[face_cand[i][0] + fi][face_cand[i][1] + fj];
	  }
	}
      }

      mean_u = mean_u;
      mean_v = mean_v;

      uv_angle = atan2 ((double)mean_v, (double)mean_u) * 180 / PI;


      if (((uv_angle < 180) && (uv_angle > 90)) || ((mean_u == 0)
	  && (mean_v == 0)) || (face_cand[i][2] > 24)) {
	faces[fine_N][0] = face_cand[i][0];
	faces[fine_N][1] = face_cand[i][1];
	faces[fine_N][2] = face_cand[i][3];
	faces[fine_N][3] = face_cand[i][4];

	++fine_N;
      }
    }
  }

  /* find the eye positions for each detected face */

  for (k = 0; k < fine_N; ++k) {
    scale = (double) (faces[k][2]) / 100;
    rot = (double) (faces[k][3]) / 100;

    th = -rot * PI / 180;

    c = cos (th);
    s = sin (th);

    lj = -7.5 / scale;
    li = -4 / scale;

    fine_eyes[k][0] = faces[k][1] + (int) (c * lj - s * li);
    fine_eyes[k][1] = faces[k][0] + (int) (s * lj + c * li);

    lj = 8.5 / scale;

    fine_eyes[k][2] = faces[k][1] + (int) (c * lj - s * li);
    fine_eyes[k][3] = faces[k][0] + (int) (s * lj + c * li);

  }

  *pfine_N = fine_N;
  return;
}


/////////////////////////////////////
// my code end here
////////////////////////////////////
