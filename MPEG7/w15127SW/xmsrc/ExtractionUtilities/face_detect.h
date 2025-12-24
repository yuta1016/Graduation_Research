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

////////////////////////////////////////////////////////////////////////////////
// face_detect.h: interface for the Cface_detect class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACE_DETECT_H__7ACA63A1_79C5_11D5_B299_0050BAF03F86__INCLUDED_)
#define AFX_FACE_DETECT_H__7ACA63A1_79C5_11D5_B299_0050BAF03F86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "momusys.h"

//coarse detection
#define POINTS 100
#define ANGLE0 30
#define ANGLE1 20
#ifndef PI
#define PI	      3.14159265358979323846
#endif
#define preH          1.6
#define HW            0.6
#define MEYES         1.2
#define NOSEL         1.0/6.0
#define NOSER         5.0/6.0

//confirm face
#define LHV3 8
#define	LHV2 26
#define	LHV 192
#define	LHR 192
#define	LVV 192
#define	LVR 192

typedef struct
{
  int x;
  int y;
  int count;
  int length;
}
CP;

typedef struct
{
  CP Lb;
  CP Le;
  CP Rb;
  CP Re;
  CP Nose;
  CP Mouth;
  int Isface;
  int eyesdis;
  int EMdis;
  int possible_false;
}
FACE;
typedef struct
{
  int left_eye_x;
  int left_eye_y;
  int right_eye_x;
  int right_eye_y;
}
Simple_FACE;

typedef struct
{
  float scale;
  float SIN;
  float COS;
  int leftx;
  int lefty;
} FACE_TRANS_PAR;

class Cface_detect
{ private:
 public:
  Cface_detect ();
  virtual ~ Cface_detect ();
  Simple_FACE simple_face[50];
  int call_FaceDetect (char *Filename, MomVop * vop);
  MomVop *GetFaceImage(int faceno);
  int GetFaceParameters(Simple_FACE *s_face,int faceno);

//global variable
private:
  Simple_FACE c_s_face[50];
  int imgheight;
  int imgwidth;
  int bitperpixel;
  int imgbitcount;
  // 2013/03/05 fix - dynamic allocation instead of local variable (to prevent stack exception)
  //float hist2d[512][512];
  float **hist2d;
  MomVop *inputvop;
  MomVop *facevop;
  unsigned char **inImage;	//[320][320];//sth := MomImage
  unsigned char **inImage0;	//[320][320];
  unsigned char **R;		//[320][320];
  unsigned char **G;		//[320][320];
  unsigned char **B;		//[320][320];
  unsigned char **image[3];	//[3][320][320];//sth := MomVop
  unsigned char **image1;	//[320][320];
  CP *centerPoints[3];		//[320*320/64];
  FACE candidates[1000];
  FACE Face[50];

private:
   int number[3];
  int facenum[3];
  FACE face[3][1000];

  int FaceDetect (MomVop * vop, Simple_FACE s_face[50]);
  int face_segmentation (int facenum);
  int Init_Memory (int height, int width);
  int input_image (MomVop * vop);
  unsigned char **Allocat_Memory_UnChar (int height, int width);
  void delete_memory (int height, int width);
  void delete_Memory_UnChar (unsigned char **Ptr, int height, int width);

//color segmentation
  void initialize_cpdf ();
  void colorfacesegment (float th);
// coarse detection

  void feature_extract (int decide);
  void masicimage1 (unsigned char **image, int masiclong, int height, int width,
    int startx, int starty, unsigned char **masicimg);
  void showmasicimage (unsigned char **image, int masiclong, int masiclongw,
    int height, int width, int startx, int starty, unsigned char **masicimg);
  void edgedetect1 (unsigned char **masicimg, int height, int width, int TH);
  void masicimage (unsigned char **image, int masiclong, int masiclongw,
    int height, int width, int startx, int starty, unsigned char **masicimg);
  void edgedetect (unsigned char **masicimg, int height, int width, int TH,
    int decide);
  int objectcluster (unsigned char **image, int height, int width,
    CP * centerP);
  int askcandidates (int kk);

// belong to coarsedetectface
  void coarselocate (int num, int z);
  void coarsedetectface (int num);
  int searchpoints (CP Le, CP Re, CP Points[POINTS], int num);
  int scan1 (CP P, CP Points[POINTS], double delta, int h, int num, int direct);
  int scan2 (CP P, CP Points[POINTS], double delta, int h1, int h2, int num);
  void adjust1 (CP P, CP Points[POINTS], int m, int n);
  void adjust2 (CP Points[POINTS], int m, int n);

//      void drawline(int num,CP Le,CP Re);
  FACE deformablematch (FACE tface, CP tempP[POINTS], int count);
  void RotateInFaceArea (FACE tface, FACE face[4], CP tempP[POINTS], int count,
    int num);
  FACE match1 (FACE tface, CP tempP[POINTS]);
  FACE match2 (FACE tface, CP tempP[POINTS]);
  FACE match3 (FACE tface, CP tempP[POINTS]);
  FACE match4 (FACE tface, CP tempP[POINTS]);

  void drawline1 (unsigned char **image, CP Le, CP Re);
//      void drawline2(unsigned char *image,CP Le,CP Re,int gray);

//confirm face
  int call_confirm (int candidatesnum);
  void Initialize_confirm_face_parameter ();
  void confirm_face (unsigned char **orig_img, char **u_img,
    char **v_img, int n, int m, int coarse_N,
    int coarse_eyes[1000][4], int *pfine_N, int fine_eyes[1000][4]);
  void fcorr_filter (unsigned char img[72][54],
    char img_out[72][54], int n, int m, int fp_th, int corr_th);
  void lin_comb (double *hs, double vec[54], double coeff[54]);
  void split_face (char blk[27][24], double vec[54]);
  void fimresize (unsigned char img[72][54], int n, int m, double s);
  void fimrotate (unsigned char img[72][54], int n, int m, double ang);
  void designfilter (double Wn, double b[11]);
  void fver_filter (double h[], unsigned char img[72][54], int l, int n, int m,
    int off);
  void fhor_filter (double h[], unsigned char img[72][54], int l, int n, int m,
    int off);

};

#endif // !defined(AFX_FACE_DETECT_H__7ACA63A1_79C5_11D5_B299_0050BAF03F86__INCLUDED_)
