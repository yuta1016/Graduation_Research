////////////////////////////////////////////////////////////////////////////////
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
// Applicable File Name:  line.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(LINE_H)
#define LINE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef _VISUALC_
#include <windows.h>
#endif
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <string>
#include <memory.h>
#include "VideoText_Area.h"

using namespace std;

class Line  
{
public:
	int ACCEPT;
	int getNrChar();
	Line(Area* area);
	void addLine(Line* line);
  virtual ~Line();
	void addArea(Area* area);
	int size();
  Area*  getAreaAt(int index);
 // void writeImageFile(FILE* fImagePtr); 
 // void writeImageFile(FILE* fImagePtr, unsigned int *OrigImage, int Imgrows,int Imgcols); //laa 1/18/99
	void writeImageFile(FILE* fImagePtr, unsigned char *OrigImage, int Imgrows,int Imgcols); 
	void swap(int CC1, int CC2, int *lineTable, int imageWidth, int CurrentCount); //laa 7/20/99
	//laa 1/19/99 uint -> uchar
	int getLeft();
  int getRight();
  int getTop();
  int getBottom();
  int getHeight();
  int getWidth();
  int getCentreX();
  int getCentreY();
private:
  vector<Area *> charList;
  int minX;
  int maxX;
  int minY;
  int maxY;
};

#endif // !defined(LINE_H)
