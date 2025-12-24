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
// Applicable File Name:  area.h
//
//////////////////////////////////////////////////////////////////////
#if !defined(AREA_H)
#define AREA_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef _VISUALC_
#include <windows.h>
#endif
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include "VideoText_Coordinate.h"

using namespace std;

typedef vector<Coordinate*> Coord_type;

class Area  
{
public:
	int size();
	int getWidth();
	int getHeight();
	int getBottom();
	int getTop();
  int getLeft();
  int getRight();
	int getMinGray();
	int getMaxGray();
  Coordinate* getCoordinateAt(int index);
	Coordinate* getFirstCoordinate();
  Coordinate* getNextCoordinate();
  void addCoordinate(int x , int y, int gray=0);
	Area(int x, int y, int gray = 0);
	virtual ~Area();
  void sortCoordinates(int width);
  bool operator==(Area a);
  bool operator<(Area a);
  bool operator() (Area a);
  void dump(FILE* fPtr);
	int ACCEPT; //to whether or not this area is accepted. laa 7/21/99
private:
	int minX;
	int minY;
	int maxX;
	int maxY;
	int minGray;//laa 7/18/99
	int maxGray;//laa 7/18/99
  Coordinate *coordinate;
  vector<Coordinate*> coordinateList;
  int index;
};

#endif // !defined(AREA_H)
