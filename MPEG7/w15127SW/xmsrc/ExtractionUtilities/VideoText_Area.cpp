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
// Applicable File Name:  Area.cpp
//
#include "./VideoText_Area.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/

Area::Area(int x, int y, int gray) //laa 7/18/99
{
	ACCEPT = 1; //initially all areas are accepted! laa 7/21/99
  coordinate = new Coordinate(x, y);
  coordinateList.push_back(coordinate);
  (this->minX) = x;
  (this->minY) = y;
  (this->maxX) = x;
  (this->maxY) = y;
	(this->minGray) = gray;
	(this->maxGray) = gray;
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
Area::~Area()
{
//	delete coordinate;//added by Gang
  coordinateList.clear();
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
void Area::addCoordinate(int x, int y, int gray)
{
  coordinateList.push_back(new Coordinate(x, y));
  if (x < (this->minX))
    (this->minX) = x;
  if (y < (this->minY))
    (this->minY) = y;
  if (x > (this->maxX))
    (this->maxX) = x;
  if (y > (this->maxY))
    (this->maxY) = y;
	if (gray < (this->minGray))//laa 7/18/99
    (this->minGray) = gray;
	if (gray > (this->maxGray))//laa 7/18/99
    (this->maxGray) = gray;	
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
int Area::getWidth()
{
  return ((this->maxX) - (this->minX) + 1);
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
int Area::getBottom()
{
  return (maxY);
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
int Area::getTop()
{
  return (minY);
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
int Area::getHeight()
{
  return ((this->maxY) - (this->minY) + 1);
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
int Area::getLeft()
{
  return (this->minX);
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
int Area::getRight()
{
  return (this->maxX);
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
int Area::size()
{
  return (coordinateList.size());
}

int Area::getMinGray()
{
  return (this->minGray);
}

int Area::getMaxGray()
{
  return (this->maxGray);
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
Coordinate* Area::getCoordinateAt(int index)
{
  return (Coordinate*) coordinateList[index];
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
Coordinate* Area::getFirstCoordinate()
{
  index = 0;
  return (Coordinate*) coordinateList[index];
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
Coordinate* Area::getNextCoordinate()
{
  index++;
  return (Coordinate*) coordinateList[index];
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
int comp(Coordinate* c1, Coordinate* c2)
{
  if ((c1 != NULL) && (c2 != NULL))
    return (c1->getX()) - (c2->getX()) ;
  return 0;
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
class CompareObj
{
public:
  bool operator() (Coordinate* cx1, Coordinate* cx2)
  {
    return  ( (cx1->getY()*(this->width)+cx1->getX()) < 
      (cx2->getY()*(this->width)+cx2->getX()) ) ;
  }
  void setWidth(int w) {
    (this->width) = w;
  }
private:
  int width;
};
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
void Area::sortCoordinates(int w)
{
  CompareObj compare;
  compare.setWidth(w);
  sort(coordinateList.begin(), coordinateList.end(), compare);
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
bool Area::operator==(Area a)
{
  return ((a.getTop()*(this->getWidth())+(a.getLeft()) == 
    (this->getTop()*(this->getWidth())+(this->getLeft())
    )));
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
bool Area::operator<(Area a)
{
  return ((a.getTop()*(this->getWidth())+(a.getLeft()) < 
    (this->getTop()*(this->getWidth())+(this->getLeft())
    )));
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
bool Area::operator() (Area a)
{
  return  ((a.getTop()*(this->getWidth())+(a.getLeft()) < 
    (this->getTop()*(this->getWidth())+(this->getLeft())
    )));
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
void Area::dump(FILE* fPtr) {
    fprintf(fPtr, "--) Size %d Top/Bottom: %d/%d Left/right: %d/%d\n", 
      this->size(),  this->getTop(), this->getBottom(), 
      this->getLeft(), this->getRight());
}
