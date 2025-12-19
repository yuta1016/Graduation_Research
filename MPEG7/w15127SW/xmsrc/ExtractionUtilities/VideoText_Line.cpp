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
// Applicable File Name:  line.cpp
//
//////////////////////////////////////////////////////////////////////

#include "./VideoText_Line.h"
//#include "./pgm.h"
//#include "./Ppm.h"

#include "./VideoText_Parameters.h"
typedef unsigned short gray;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

class CompareChar
{
public:
  bool operator() (Area* a1, Area* a2)
  {   
    return ((a1->getLeft()) < 
            (a2->getLeft()));
  }
};

Line::Line(Area* area)
{
	ACCEPT = 1;
  charList.push_back(area);
  (this->maxX) = area->getRight();
  (this->maxY) = area->getBottom();
  (this->minX) = area->getLeft();
  (this->minY) = area->getTop();
}

void Line::addArea(Area* area)
{
	ACCEPT = 1;
  charList.push_back(area);
  if ((this->maxX) < area->getRight())
    (this->maxX) = area->getRight();
  if ((this->maxY) < area->getBottom())
    (this->maxY) = area->getBottom();
  if ((this->minX) > area->getLeft())
    (this->minX) = area->getLeft();
  if ((this->minY) > area->getTop())
    (this->minY) = area->getTop();
}

//laa 1/18/98 add a line to an existing line
void Line::addLine(Line* line)
{
	for (int i = 0; i < line->charList.size(); i++){
		
		charList.push_back(line->getAreaAt(i));
	}
  if ((this->maxX) < line->getRight())
    (this->maxX) = line->getRight();
  if ((this->maxY) < line->getBottom())
    (this->maxY) = line->getBottom();
  if ((this->minX) > line->getLeft())
    (this->minX) = line->getLeft();
  if ((this->minY) > line->getTop())
    (this->minY) = line->getTop();
}

Area* Line::getAreaAt(int index)
{
  return (this->charList[index]);
}

int Line::getWidth()
{
  return ((this->maxX) - (this->minX) +1);
}

int Line::getHeight()
{
  return ((this->maxY) - (this->minY) +1);
}

int Line::size()
{
  return (charList.size());
}

int Line::getLeft()
{
  return (this->minX);
}

int Line::getRight()
{
  return (this->maxX);
}

int Line::getTop()
{
  return (this->minY);
}

int Line::getBottom()
{
  return (this->maxY);
}

int Line::getCentreY()
{
//  return (this->minY+((maxY-minY)/2));//Dont think this is right laa 11/11/98
		return (((this->maxY + this->minY)/2));
}

int Line::getCentreX()
{
  //return (this->minX+((maxX-minX/2))); //Dont think this is right laa 11/11/98
	return (((this->maxX + this->minX)/2));
}

Line::~Line()
{
  charList.clear();
}


//this is temporary for my cs4721 purposes laa 3/2/99

void Line::writeImageFile(FILE* fImagePtr, unsigned char *OrigImage, //uint -> uchar
													int Imgrows,int Imgcols) //laa 12/18/98
{ 
}

int Line::getNrChar()
{
  return (this->charList).size();
}

void Line::swap(int CC1, int CC2, int *lineTable, int imageWidth, int CurrentCount)
{
	Area* area1;
	Area* area2;
	Coordinate* coordinate;
	
	area1 = this->getAreaAt(CC1 + CurrentCount - 1);	
	area2 = this->getAreaAt(CC2 + CurrentCount - 1);	
	if (area2->ACCEPT){	
		coordinate = area2->getCoordinateAt(0);
		area1->addCoordinate(coordinate->getX(), coordinate->getY(), area2->getMaxGray());
		lineTable[coordinate->getY()*imageWidth + coordinate->getX()] = CC1;
		for (int k = 1; k < area2->size(); k++){
			coordinate = area2->getCoordinateAt(k);
			area1->addCoordinate( coordinate->getX(), coordinate->getY(),area2->getMinGray());
			lineTable[coordinate->getY()*imageWidth + coordinate->getX()] = CC1;
//			delete coordinate;
		}
		area2->ACCEPT = 0;
	}
}
