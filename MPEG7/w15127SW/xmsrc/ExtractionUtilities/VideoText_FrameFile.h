///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Gang Wei,  Nevenka	Dimitrova,	and Lalitha Agnihotri - Philips Research.
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
// Applicable File Name:  framefile.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(FRAMEFILE_H)
#define FRAMEFILE_H

#define IMGROW 288
#define IMGCOL 342

//#include "pgm.h"
//#include "Ppm.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef _VISUALC_
#include <windows.h>
#include <vfw.h>
#endif
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <memory.h>
#include "VideoText_Line.h"
//#include "../misc/inout.h"

using namespace std;

typedef vector<Area *> Area_type;
typedef vector<Line *> Line_type;

#ifndef BYTE
#define BYTE char
#endif

class FrameFile  
{
public:
	FrameFile(FILE*);
	FrameFile(int width, int height);
	FrameFile(short ** imgData, int Height, int Width);
	FrameFile(BYTE * pnBuf, int Height, int Width);
  FrameFile(LPVIDEOHDR videoFrame, int cols, int rows);
	virtual ~FrameFile();
  void write(FILE*);
  void sharpen();
  void smooth();  
//  unsigned int *getImage();
	unsigned char *getImage();
  int getRowSize();
  int getColSize();
//  void edgeDetect();
	void edgeDetect(int nedgeCount[7], int thresh = 200); //for edge filtering laa 1/20/98
  Line* getLineAt(int);
  int findAreas(FILE*);
  int findAreas();
  void dump(FILE* fPtr);
  void setImage(LPVIDEOHDR videoFrame, int cols, int rows);
  void cleanFrame(int nrLines);
//	unsigned int *fileFramePtr; //laa 1/18/99
	unsigned char *fileFramePtr; //laa 1/19/99 change uint to uchar
		
	  int cols, rows;
//		unsigned char fileFramePtr[IMGROW*IMGCOL]; //laa 1/19/99 change dynamic to static
 //laa 3/5/99 hardcode
private:
  //unsigned int *fileFramePtr;
  //int cols, rows;
//  pixval maxval; 
	BYTE maxval;
  Area_type areaList;
  Area_type currentLine;
  Area_type lineList;
  Area_type charList;
  Line *line;
  Line_type linesList;
};

#endif // !defined(FRAMEFILE_H)
