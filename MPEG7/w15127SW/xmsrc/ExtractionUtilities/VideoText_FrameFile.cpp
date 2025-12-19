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
// Applicable File Name:  framefile.cpp
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include "textExt.h"
#ifndef __STDC__
#define __STDC__ 1
#endif
#include "./VideoText_FrameFile.h"
#include "./VideoText_Coordinate.h"
#include "./VideoText_Parameters.h"
//#include "pgm.h"
//#include "Ppm.h"
//#include "vfw.h"
//#include "../misc/inout.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif
#define LINE_THRESHOLD 3
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : const void *arg1, const void *arg2
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
int compareMatrixValue(const void *arg1, const void *arg2 )
{
  /* Compare all of both strings: */
  return  ( *(int*)arg1 - *(int*)arg2);
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : FILE* fPtr
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
FrameFile::FrameFile(FILE* fPtr)
{
/*  int colNr, rowNr;
  
  pixel *pixelrow;
  int format;
  line = NULL;
	int position = 0; //laa 3/1/99

  ppm_readppminit(fPtr, &cols, &rows, &maxval, &format );
  pixelrow = ppm_allocrow(cols);
  //row = pgm_allocrow( cols );
  //laa 3/5/99 change alloc to static arrays.
	fileFramePtr = new unsigned char[cols * rows]; //laa 1/19/99 uint -> uchar

  
	//pgm_writepgminit(fOutputPtr, cols, rows, maxval, 0);
  for (rowNr = 0; rowNr < rows; rowNr++)
  {
    ppm_readppmrow(fPtr, pixelrow, cols, maxval, format);
    for (colNr = 0; colNr < cols; colNr++) 
    {
//      fileFramePtr[rowNr*cols+colNr] = (unsigned int) (
        0.299 *PPM_GETR(pixelrow[colNr]) +
        0.587 *PPM_GETG(pixelrow[colNr]) +
        0.114 *PPM_GETB(pixelrow[colNr])
        );
			//laa 1/18/98
			fileFramePtr[position] = PPM_GETR(pixelrow[colNr]) ; //laa 3/1/99 positions
			position++;
    }
  }
*/
}

FrameFile::FrameFile(short ** imgData, int Height, int Width)
{
	int i, j;
	int k=0;
	fileFramePtr=new unsigned char[Height * Width];
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++)
		{
			fileFramePtr[k]=imgData[i][j];
			k++;
		}
	cols=Width;
	rows=Height;
	line=NULL;

}

FrameFile::FrameFile(BYTE * pnBuf, int Height, int Width)
{
	int i;
	fileFramePtr=new unsigned char[Height * Width];
	for (i=0; i<Height*Width; i++)
			fileFramePtr[i]=pnBuf[i];
	cols=Width;
	rows=Height;
	line=NULL;

}

/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : FILE* fPtr
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
FrameFile::FrameFile(LPVIDEOHDR videoFrame, int width, int height)
{
  line = NULL;
  (this->cols) = width; 
  (this->rows) = height;
  (this->maxval) = 255;
	//laa 3/5/99 dyn -> static
	fileFramePtr = new unsigned char[width * height];//laa 1/19/99 uint -> uchar
  for (int i = 0; i < width*height; i++) {
    //fileFramePtr[i] = (unsigned int) videoFrame->lpData[i];
		fileFramePtr[i] = (unsigned char) videoFrame->lpData[i];//laa 1/19/99 uint -> uchar
  }
}


//new constructor to be used for CDWIN laa 4/16/99
FrameFile::FrameFile(int width, int height)
{
  line = NULL;
  (this->cols) = width; 
  (this->rows) = height;
  (this->maxval) = 255;
	//laa 3/5/99 dyn -> static

	 fileFramePtr = new unsigned char[width * height];//laa 1/19/99 uint -> uchar
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : FILE* fPtr
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
void FrameFile::setImage(LPVIDEOHDR videoFrame, int width, int height)
{
  line = NULL;
  (this->cols) = width; 
  (this->rows) = height;
  (this->maxval) = 255;
  for (int i = 0; i < width*height; i++) {
    //fileFramePtr[i] = (unsigned int) videoFrame->lpData[i];
		fileFramePtr[i] = (unsigned char) videoFrame->lpData[i];//laa 1/19/99 uint -> uchar
  }
}
/*************************************************************************
 * Procedure   : NA
 * Purpose     : NA
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
FrameFile::~FrameFile()
{
//  delete [] fileFramePtr;
}
/*************************************************************************
 * Procedure   : NA
 * Purpose     : NA
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
void FrameFile::cleanFrame(int nrLines)
{
	delete [] fileFramePtr;
  Line *line;
  Area *area;
  Coordinate *coordinate;
  
  for (int lineIndex = 0; lineIndex < nrLines; lineIndex++) {
    line = linesList[lineIndex];
		if (line->ACCEPT){
	    for (int areaIndex = 0; areaIndex < line->size(); areaIndex++) {
		    area = line->getAreaAt(areaIndex);
			  for (int coordinateIndex = 0; coordinateIndex < area->size(); coordinateIndex++) {
				  coordinate = area->getCoordinateAt(coordinateIndex);
					delete coordinate;
				}
				delete area;
			}
			delete line;
		}
  }

// if(nrLines>0){
  areaList.clear();
  currentLine.clear();
  lineList.clear();
  charList.clear();
  linesList.clear();
//  }

}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : FILE* fPtr
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
void FrameFile::write(FILE* fPtr)
{
/*  int colNr, rowNr;
  gray *row;
	int position = 0; //laa 3/1/99
//  row = pgm_allocrow( cols );
//  pgm_writepgminit(fPtr, cols, rows, maxval, 0);
  for (rowNr = 0; rowNr < rows; rowNr++)
  {
    for (colNr = 0; colNr < cols; colNr++) 
    {
      //row[colNr] = fileFramePtr[rowNr*cols+colNr];
			row[colNr] = fileFramePtr[position];
			position++; //laa 3/1/99
    }
//    pgm_writepgmrow(fPtr, row, cols, maxval, 0 );
  }
*/
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
/*void FrameFile::sharpen()
{
  int sum, rowNr, colNr;
  int sharpenMatrix[9] = {-1, -1, -1, -1, 12, -1, -1, -1, -1};
//  unsigned int *sharpenFrame;
	unsigned char *sharpenFrame; //laa 1/19/99 uint -> uchar
  //sharpenFrame = new unsigned int[cols * rows]; 
	sharpenFrame = new unsigned char[cols * rows]; //laa 1/19/99 uint -> uchar
	int position = 0; //laa 3/1/99

//testing
	int divRow = rows/3, rowPos;
	int divCol = cols/3, colPos;
	int nedgeCount[7] = {0};
	int max, min;

  for (rowNr = 1; rowNr < rows+2; rowNr++)
  {
		position = rowNr * cols;
		rowPos = (rowNr - 3)/divRow;
    for (colNr = 1; colNr < cols+2; colNr++) 
    {
			position++; //laa 3/1/99
			if (rowNr < rows-1){
     
			sum = - fileFramePtr[position - cols - 1] -
        fileFramePtr[position - cols]    -
        fileFramePtr[position - cols + 1]  -
        fileFramePtr[position - 1]     +
        fileFramePtr[position]          * 12 -
        fileFramePtr[position + 1]        -
        fileFramePtr[position + cols -1]    -
        fileFramePtr[position + cols]     -
        fileFramePtr[position + cols + 1];


      sum /= 4;
//      sharpenFrame[rowNr*cols+colNr] = __max(__min( sum, 255 ), 0);
			min =  (((sum) > (255)) ? (255) : (sum));
			sharpenFrame[position] = (((min) > (0)) ? (min) : (0));
			//sharpenFrame[position] = __max(__min( sum, 255 ), 0);     
			}

			//testing
			if (rowNr > 3){
				int pos = position - 2 * cols;
				sum = - sharpenFrame[pos - cols - 1] -
					sharpenFrame[pos - cols]    -
	        sharpenFrame[pos - cols + 1]  -
		      sharpenFrame[pos - 1]     +
			    sharpenFrame[pos]          * 8 -
				  sharpenFrame[pos + 1]        -
					sharpenFrame[pos + cols -1]    -
	        sharpenFrame[pos + cols]     -
		      sharpenFrame[pos + cols + 1];
	
		    //if (sum > 200)
				if ((sum) > 200) { //laa 1/18/98
				  sum = 255;
					colPos = 2 + colNr/divCol;	 
				  nedgeCount[rowPos]++;
				  nedgeCount[colPos]++;
				  nedgeCount[6]++; //keep the count of number of pixels marked as edges...
				}
	      else
		      sum = 0;
			  
				
				sharpenFrame[position] = sum;//laa 3/1/99
			}
    }
  }
//  delete [] fileFramePtr;

  delete [] sharpenFrame;
	//fileFramePtr = sharpenFrame;
}*/

void FrameFile::sharpen()
{
  int sum, rowNr, colNr;
  int position = 0; //laa: Need to reduce the number of calculations
  int sharpenMatrix[9] = {-1, -1, -1, -1, 12, -1, -1, -1, -1};
  unsigned char *sharpenFrame;
  sharpenFrame = new unsigned char[cols * rows];
 //laa 3/5/99 dyn -> static
//	 unsigned char sharpenFrame[IMGROW*IMGCOL];
	int min;
  for (rowNr = 1; rowNr < rows-1; rowNr++){
	  position = rowNr * cols + 1;	
    for (colNr = 1; colNr < cols-1; colNr++) {		
			sum= (-1) *(fileFramePtr[position - cols - 1]) -
        (fileFramePtr[position - cols]      ) - 
        (fileFramePtr[position - cols + 1]    ) - 
        (fileFramePtr[position - 1]        ) +
        (12) * (fileFramePtr[position]          ) -
        (fileFramePtr[position + 1]        ) - 
        (fileFramePtr[position + cols - 1]    ) - 
        (fileFramePtr[position + cols]      ) - 
        (fileFramePtr[position + cols + 1]    ); //laa 11/11/98
      sum /= 4;
    //  sharpenFrame[rowNr*cols+colNr] = __max(__min( sum, 255 ), 0);
			min =  (((sum) > (255)) ? (255) : (sum));
			sharpenFrame[position] = (((min) > (0)) ? (min) : (0));
	 // sharpenFrame[position] = __max(__min( sum, 255 ), 0); //laa
	  position++;
    }
  }
//	memcpy(fileFramePtr, sharpenFrame, rows*cols); //laa dyn->static
	delete [] fileFramePtr;
  fileFramePtr = sharpenFrame; 
}

/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
void FrameFile::smooth()
{
  unsigned int smoothArray[9];
  int rowNr, colNr;

	unsigned char *smoothFrame;//laa 1/19/99 uint -> uchar
	 smoothFrame = new unsigned char[cols * rows];//laa 1/19/99 uint -> uchar
//	unsigned char smoothFrame[IMGROW*IMGCOL]; //dyn -> static laa 3/5/99

	int position = 0;
  for (rowNr = 1; rowNr < rows-1; rowNr++)
  {
		position = rowNr * cols + 1;
    for (colNr = 1; colNr < cols-1; colNr++) 
    {
      /*smoothArray[0] = fileFramePtr[(rowNr-1)*cols+colNr-1];
      smoothArray[1] = fileFramePtr[(rowNr-1)*cols+colNr];
      smoothArray[2] = fileFramePtr[(rowNr-1)*cols+colNr+1];
      smoothArray[3] = fileFramePtr[rowNr*cols+colNr-1];
      smoothArray[4] = fileFramePtr[rowNr*cols+colNr];
      smoothArray[5] = fileFramePtr[rowNr*cols+colNr+1];
      smoothArray[6] = fileFramePtr[(rowNr+1)*cols+colNr-1];
      smoothArray[7] = fileFramePtr[(rowNr+1)*cols+colNr];
      smoothArray[8] = fileFramePtr[(rowNr+1)*cols+colNr+1];*/
			smoothArray[0] = fileFramePtr[position - cols - 1];
      smoothArray[1] = fileFramePtr[position - cols];
      smoothArray[2] = fileFramePtr[position - cols + 1];
      smoothArray[3] = fileFramePtr[position - 1];
      smoothArray[4] = fileFramePtr[position];
      smoothArray[5] = fileFramePtr[position + 1];
      smoothArray[6] = fileFramePtr[position + cols - 1];
      smoothArray[7] = fileFramePtr[position + cols];
      smoothArray[8] = fileFramePtr[position + cols + 1];
			position++;
      qsort(smoothArray, 9, sizeof(smoothArray[0]), 
         compareMatrixValue);
      smoothFrame[position] = smoothArray[4];
    }
  }
  delete [] fileFramePtr;
//		memcpy(fileFramePtr, smoothFrame, rows*cols); 

/*	int length = cols*rows; //laa 3/1/99
  for (int i = 0; i < length; i++){
	  fileFramePtr[i] = smoothFrame[i]; 
  }*/
  fileFramePtr = smoothFrame; 
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
void FrameFile::edgeDetect(int nedgeCount[7], int thresh) {
  int sum, rowNr, colNr;
  int edgeMatrix[9] = {-1, -1, -1, -1, 8, -1, -1, -1, -1 };

	unsigned char *edgeDetectedFrame; //laa 1/19/99 uint -> uchar
  edgeDetectedFrame = new unsigned char[cols * rows];  //laa 1/19/99 uint -> uchar
	//unsigned char edgeDetectedFrame[IMGROW*IMGCOL]; //laa 3/5/99 dyn -> static
  //for (int i = 0; i < (cols * rows); i++) 
 //   edgeDetectedFrame[i] = 0;
	int position = 0;
  
	
  for (rowNr = 0; rowNr < rows; rowNr++)
  {
    edgeDetectedFrame[(rowNr)*cols] = 0;
    edgeDetectedFrame[(rowNr)*cols+cols-1] = 0;
	 
  }

	for (colNr = 0; colNr < cols; colNr++)
  {
    edgeDetectedFrame[(rows-1)*cols+colNr] = 0;
    edgeDetectedFrame[colNr] = 0;
  }
 
	int divRow = rows/3, rowPos;
	int divCol = cols/3, colPos;

  for (rowNr = 1; rowNr < rows-1; rowNr++)
  {
	//	rowPos = rowNr/divRow;
		position = rowNr * cols + 1;
    for (colNr = 1; colNr < cols-1; colNr++) 
    {
/*      sum = (fileFramePtr[(rowNr-1)*cols+colNr-1])*edgeMatrix[0] +
        (fileFramePtr[(rowNr-1)*cols+colNr]      )*edgeMatrix[1] +
        (fileFramePtr[(rowNr-1)*cols+colNr+1]    )*edgeMatrix[2] +
        (fileFramePtr[rowNr*cols+colNr-1]        )*edgeMatrix[3] +
        (fileFramePtr[rowNr*cols+colNr]          )*edgeMatrix[4] +
        (fileFramePtr[rowNr*cols+colNr+1]        )*edgeMatrix[5] +
        (fileFramePtr[(rowNr+1)*cols+colNr-1]    )*edgeMatrix[6] +
        (fileFramePtr[(rowNr+1)*cols+colNr]      )*edgeMatrix[7] +
        (fileFramePtr[(rowNr+1)*cols+colNr+1]    )*edgeMatrix[8];  */
				sum = - fileFramePtr[position - cols - 1] -
        fileFramePtr[position - cols]    -
        fileFramePtr[position - cols + 1]  -
        fileFramePtr[position - 1]     +
        fileFramePtr[position]          * 8 -
        fileFramePtr[position + 1]        -
        fileFramePtr[position + cols -1]    -
        fileFramePtr[position + cols]     -
        fileFramePtr[position + cols + 1];

				if (sum > thresh){
			//if ((sum) > 200) { //laa 1/18/98
        sum = 255;
	//			colPos = 2 + colNr/divCol;	 
	//		  nedgeCount[rowPos]++;
	//		  nedgeCount[colPos]++;
    //	  nedgeCount[6]++; //keep the count of number of pixels marked as edges...
			}
      else
        sum = 0;

			
			edgeDetectedFrame[position] = sum;//laa 3/1/99
			position++;

			//try to erode?? laa 3/1/99
			/*if ((rowNr > 3) && (colNr > 3)){
				int min = 300; int pos;
				for (int row_low = rowNr - 1; row_low <= rowNr; row_low++){
					pos = row_low * cols + colNr - 1;
					for (int col_low = colNr - 1; col_low <= colNr; col_low++){
						if (edgeDetectedFrame[pos] < min){
							min = edgeDetectedFrame[pos];
						}
						pos++;
					}
				}
				if ((min == 255) || (min == 0)){
					edgeDetectedFrame[position] = min;
				}
			}//if*/
			

    }
  }
  delete [] fileFramePtr;
	
	
//	memcpy(fileFramePtr, edgeDetectedFrame, rows*cols); 
	/*int length = cols*rows;
		for (int i = 0; i < length; i++){
	  fileFramePtr[i] = edgeDetectedFrame[i]; 
  }*/
  fileFramePtr = edgeDetectedFrame;
//	delete [] edgeDetectedFrame;
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
//unsigned int *FrameFile::getImage() {
unsigned char *FrameFile::getImage() { //laa 1/19/99 uint->uchar
  return (this->fileFramePtr);
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
int FrameFile::getRowSize() {
  return (this->rows);
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
int FrameFile::getColSize() {
  return (this->cols);
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
class CompareArea
{
public:
  bool operator() (Area* a1, Area* a2)
  {
    return  ( 
      ((a1->getBottom()*(this->width))+a1->getLeft()) < 
      ((a2->getBottom()*(this->width))+a2->getLeft()) 
      );
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
class CompareLine
{
public:
  bool operator() (Line *line1, Line *line2)
  {
    return  ( (line1->size()) > (line2->size()) );
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
int FrameFile::findAreas(FILE *fPtr)
{
  int checkedElements;
  int row, col, x, y;
  Area *area = NULL, *prevArea = NULL;
  Coordinate *coordinate;
  int areaIndex;
  //Area_type::const_iterator  previousIndex = NULL;
  int lineTop = 0, index = 0;
	int position = 0;
	
  Parameters parameters(cols);

  for (row = 1; row < (rows-1); ++row)
  {
		int pos = 0;
		position = row * cols;
    for (col = 1; col < (cols-1); ++col)
    {
			position++;
      if (fileFramePtr[position] ==  255)
      {
        area = new Area(col, row);
#if WRITE_LOG
        fprintf(stdout, "\n(%d,%d) ", col, row);
#endif
        fileFramePtr[position] = 1;
        checkedElements = 0;
        coordinate = area->getFirstCoordinate();
        int index = 0;
        while (index < area->size())
        {
          x = (*coordinate).getX();
          y = (*coordinate).getY();
          pos = (y*cols)+x;
          //if ( (fileFramePtr[(y*cols)+x+1] ==  255) && 
		  if ( (fileFramePtr[pos+1] ==  255) && //laa 3/1/99 
		     (x < (cols - 2))
            )
          {
            (*area).addCoordinate(x+1, y);
#if WRITE_LOG
            //fflush(stdout);
            fprintf(stdout, "(%d,%d) ", x+1, y);
#endif
            //fileFramePtr[y *cols+x+1] = 1;
						fileFramePtr[pos+1] = 1;//laa 3/1/99 
          }
          //if ( (fileFramePtr[(y*cols)+x-1] ==  255) &&
					if ( (fileFramePtr[pos-1] ==  255) &&
            (x > 1)
            )
          {
            (*area).addCoordinate(x-1, y);
#if WRITE_LOG
            fprintf(stdout, "(%d,%d) ", x-1, y);
#endif
            //fileFramePtr[((y)*cols)+x-1] = 1;
						fileFramePtr[pos-1] = 1;
          }
          //if ((fileFramePtr[((y+1)*cols)+x] == 255) &&
					if ((fileFramePtr[pos + cols] == 255) &&
            (y < (rows - 2))
            )
          {
            (*area).addCoordinate(x, y+1);
#if WRITE_LOG
            fprintf(stdout, "(%d,%d) ", x, y+1);
#endif
            //fileFramePtr[((y+1)*cols)+x] = 1;
						fileFramePtr[pos + cols] = 1;
          }
          //if ((fileFramePtr[((y-1)*cols)+x] ==  255) &&
					if ((fileFramePtr[pos - cols] ==  255) &&
            (y > 1)
            )
          {
            (*area).addCoordinate(x, y-1);
#if WRITE_LOG
            fprintf(stdout, "(%d,%d) ", x, y-1);
#endif
            //fileFramePtr[((y-1)*cols)+x] = 1;
						fileFramePtr[pos - cols] = 1;
          }
					
					
					
					
					
					//check for 8-connectivity laa 3/1/99
				// pixel to the topright from the current pixel
          if ((fileFramePtr[pos - cols + 1] ==  255) &&
            (x < (cols - 2)) && (y > 1)// safeguard go only until the rightmost column minus two
            )
          {
            (*area).addCoordinate(x+1, y-1);
#if WRITE_LOG
//            fflush(stdout);
            fprintf(stdout, "(%d,%d) ", x+1, y-1);
#endif
            fileFramePtr[pos-cols +1] = 1;
          }
          // pixel to the top-left from the current pixel
          if ((fileFramePtr[pos - cols -1] == 255) &&
            (x > 1) && (y > 1) // safeguard start from second column
            )
          {
            (*area).addCoordinate(x-1, y-1);
#if WRITE_LOG
            fprintf(stdout, "(%d,%d) ", x-1, y-1);
#endif
            fileFramePtr[pos - cols -1] = 1;
          }
          // pixel underneath-right the current pixel
          if ((fileFramePtr[pos+cols+1] == 255) &&
            (y < (rows - 2)) && (x < (cols - 2)) // safeguard go only until the lowest row minus two 
            )
          {
            (*area).addCoordinate(x+1, y+1);
#if WRITE_LOG
            fprintf(stdout, "(%d,%d) ", x+1, y+1);
#endif
            fileFramePtr[pos+cols+1] = 1;
          }
          // pixel bottom-left the current pixel
          if ((fileFramePtr[pos+cols-1] ==  255) &&
            (x > 1) && (y < (rows - 2))  // safeguard go only until the lowest row minus two 
            )
          {
            (*area).addCoordinate(x-1, y+1);
#if WRITE_LOG
            fprintf(stdout, "(%d,%d) ", x-1, y+1);
#endif
            fileFramePtr[pos+cols-1] = 1;
          }
		  //End of laa 8-connected components 11/11/98

          index++;
          if (index < area->size())
            coordinate = area->getNextCoordinate();
        }
        //dump(fPtr);
        //area->dump(fPtr);
        if (
          (area->size()      > parameters.getMaxSurfaceThreshold()) || 
          (area->size()      < parameters.getMinSurfaceThreshold()) ||
          (area->getWidth()  > parameters.getMaxWidthThreshold())   ||
          (area->getWidth()  < parameters.getMinWidthThreshold())   ||
          (area->getHeight() > parameters.getMaxHeightThreshold())  ||
          (area->getHeight() < parameters.getMinHeightThreshold())  
          )
        {
          for (int i=0; i < area->size(); i++)
          {
            coordinate = area->getCoordinateAt(i);
            fileFramePtr[coordinate->getY()*cols+coordinate->getX()] = 0;
            delete coordinate;
          }
          delete area;
          area = NULL;
        } 
        else
        {
//          fprintf(fPtr, "%d) %d-%d\n", areaList.size(), (*area).getLeft(), (*area).getBottom());
          //(*area).sortCoordinates((this->cols));
          areaList.push_back(area);
        }
      }    
    }
  }

  // clean upo the areas that do not have neighbours 
  CompareArea compare;
  compare.setWidth(cols);
  sort(areaList.begin(), areaList.end(), compare);
//  fprintf(fPtr, "Size %d\n", areaList.size());

  //dump(fPtr);
  for (areaIndex = 0; areaIndex < areaList.size(); areaIndex++)
  {
    area = areaList[areaIndex];
    //fprintf(fPtr, "%d) %d-%d\n", areaIndex, area->getLeft(), area->getBottom());
    //area->sortCoordinates(cols));
    if (prevArea != NULL)
    {
      if (
				(area->getBottom() ) <= (prevArea->getBottom() + LINE_THRESHOLD) &&
				(	
					((abs)(area->getLeft() - prevArea->getRight()) < area->getWidth() * 5) ||
					((abs)(area->getRight() - prevArea->getLeft()) < area->getWidth() * 5)
				)
				//&&((abs)(area->getLeft() - prevArea->getRight()) < area->getWidth() * 2)
				)
      {
        if (line == NULL)
        {
          line = new Line(prevArea);
        }
        else
        {
          line->addArea(prevArea);
        }
      }    
      else
      {
        if (line != NULL)
        {
          if ((line->size()) > 0)
          {
            line->addArea(prevArea);
            linesList.push_back(line);
         // fprintf(fPtr, "Char found on line %d\n", line->size());
            line = NULL;
          }
          else
          {
            for (int i=0; i < line->size(); i++)
            {
              area = line->getAreaAt(i);
              for (int j=0; j < area->size(); j++)
              {
                coordinate = area->getCoordinateAt(j);
                fileFramePtr[coordinate->getY()*cols+coordinate->getX()] = 0;
                delete coordinate;
              }
              delete area;
            }
            delete line;
          }
        }
      }
    }
    prevArea = area;
  }
  if (line != NULL)
  {
    if ((line->size()) > 0)
    {
      line->addArea(prevArea);
      linesList.push_back(line);
   // fprintf(fPtr, "Char found on line %d\n", line->size()); 


      line = NULL;
    }
    else
    {
      for (int i=0; i < line->size(); i++)
      {
        area = line->getAreaAt(i);
        for (int j=0; j < area->size(); j++)
        {
          coordinate = area->getCoordinateAt(j);
          fileFramePtr[coordinate->getY()*cols+coordinate->getX()] = 0;
          delete coordinate;
        }
        delete area;
      }
      delete line;
    }
  }
//laa 1/18/99
	if (linesList.size() > 1){
		for (int count1 = 0; count1 < linesList.size()-1; count1++){	
			if (linesList[count1]->ACCEPT){
				for (int count2 = count1+1; count2 < linesList.size(); count2++){
					if 
				 ( 
					(linesList[count2]->ACCEPT) &&
						
					(

					 (
					  ((abs)(linesList[count1]->getBottom() - linesList[count2]->getBottom()) < LINE_THRESHOLD+1) &&
					  ((abs)(linesList[count1]->getHeight() - linesList[count2]->getHeight()) < 10) &&
						(	
							((abs)(linesList[count1]->getLeft() - linesList[count2]->getRight()) < 40) ||
							((abs)(linesList[count1]->getRight() - linesList[count2]->getLeft()) < 40)
						)

					 ) ||
					 (
					  ( 
					   (linesList[count2]->getCentreX() < linesList[count1]->getRight()) &&
					   (linesList[count2]->getCentreX() > linesList[count1]->getLeft()) &&
					   (linesList[count2]->getCentreY() > linesList[count1]->getTop()) &&
					   (linesList[count2]->getCentreY() < linesList[count1]->getBottom()) 
					  ) ||
						( 
					   (linesList[count1]->getCentreX() < linesList[count2]->getRight()) &&
					   (linesList[count1]->getCentreX() > linesList[count2]->getLeft()) &&
					   (linesList[count1]->getCentreY() > linesList[count2]->getTop()) &&
					   (linesList[count1]->getCentreY() < linesList[count2]->getBottom()) 
					  ) 

						 
					 ) ||
					  
					 (( 
					  (
						 (
						   (linesList[count1]->getBottom() > linesList[count2]->getTop()) && 
					     (linesList[count1]->getTop() < linesList[count2]->getTop())
						 ) ||
						 (
						   (linesList[count2]->getBottom() > linesList[count1]->getTop()) && 
					     (linesList[count2]->getTop() < linesList[count1]->getTop())
						 ) ||
						 (
					    ( (abs)(linesList[count2]->getBottom() - linesList[count1]->getTop()) < 5) ||
					    ( (abs)(linesList[count2]->getTop() - linesList[count1]->getBottom()) < 5)
						 ) 
						) 
						&&
						( 
						 (
						  (linesList[count2]->getLeft() > linesList[count1]->getLeft()) || 
							( (abs)(linesList[count2]->getLeft() - linesList[count1]->getLeft()) < 5) 
						 ) &&
						 (
						  (linesList[count2]->getRight() < linesList[count1]->getRight())|| 
							( (abs)(linesList[count2]->getRight() - linesList[count1]->getRight()) < 5) 
						 ) 
						)
					 )) 
					
					)
				 )
				
				{
					linesList[count1]->addLine(linesList[count2]);
					linesList[count2]->ACCEPT = 0;
					

				}//if condition
			}//for count2
		}//if accept
		

	}//for count1
	}
/*	int position = 0;
	for (int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			if (fileFramePtr[position] > 0){
				fileFramePtr[position] = 255;

			}
		}
	}*/
	CompareLine compareLine;
  compareLine.setWidth(linesList.size());
  sort(linesList.begin(), linesList.end(), compareLine);
  return linesList.size();
}

int FrameFile::findAreas()
{
  int checkedElements;
  int row, col, x, y;
  Area *area = NULL, *prevArea = NULL;
  Coordinate *coordinate;
  int areaIndex;
  //Area_type::const_iterator  previousIndex = NULL;
  int lineTop = 0, index = 0;
	int position = 0;
	
  Parameters parameters(cols);


  for (row = 1; row < (rows-1); ++row)
  {
		int pos = 0;
		position = row * cols;
    for (col = 1; col < (cols-1); ++col)
    {
			position++;
      if (fileFramePtr[position] ==  255)
      {
        area = new Area(col, row);
        fileFramePtr[position] = 1;
        checkedElements = 0;
        coordinate = area->getFirstCoordinate();
        int index = 0;
        while (index < area->size())
        {
          x = (*coordinate).getX();
          y = (*coordinate).getY();
          pos = (y*cols)+x;
          //if ( (fileFramePtr[(y*cols)+x+1] ==  255) && 
		  if ( (fileFramePtr[pos+1] ==  255) && //laa 3/1/99 
		     (x < (cols - 2))
            )
          {
            (*area).addCoordinate(x+1, y);
						fileFramePtr[pos+1] = 1;//laa 3/1/99 
          }
					if ( (fileFramePtr[pos-1] ==  255) &&
            (x > 1)
            )
          {
            (*area).addCoordinate(x-1, y);
						fileFramePtr[pos-1] = 1;
          }
					if ((fileFramePtr[pos + cols] == 255) &&
            (y < (rows - 2))
            )
          {
            (*area).addCoordinate(x, y+1);
						fileFramePtr[pos + cols] = 1;
          }
					if ((fileFramePtr[pos - cols] ==  255) &&
            (y > 1)
            )
          {
            (*area).addCoordinate(x, y-1);
						fileFramePtr[pos - cols] = 1;
          }
					
					
          if ((fileFramePtr[pos - cols + 1] ==  255) &&
            (x < (cols - 2)) && (y > 1)// safeguard go only until the rightmost column minus two
            )
          {
            (*area).addCoordinate(x+1, y-1);
            fileFramePtr[pos-cols +1] = 1;
          }
          // pixel to the top-left from the current pixel
          if ((fileFramePtr[pos - cols -1] == 255) &&
            (x > 1) && (y > 1) // safeguard start from second column
            )
          {
            (*area).addCoordinate(x-1, y-1);
            fileFramePtr[pos - cols -1] = 1;
          }
          // pixel underneath-right the current pixel
          if ((fileFramePtr[pos+cols+1] == 255) &&
            (y < (rows - 2)) && (x < (cols - 2)) // safeguard go only until the lowest row minus two 
            )
          {
            (*area).addCoordinate(x+1, y+1);
            fileFramePtr[pos+cols+1] = 1;
          }
          // pixel bottom-left the current pixel
          if ((fileFramePtr[pos+cols-1] ==  255) &&
            (x > 1) && (y < (rows - 2))  // safeguard go only until the lowest row minus two 
            )
          {
            (*area).addCoordinate(x-1, y+1);
            fileFramePtr[pos+cols-1] = 1;
          }
		  //End of laa 8-connected components 11/11/98

          index++;
          if (index < area->size())
            coordinate = area->getNextCoordinate();
        }
        //dump(fPtr);
        //area->dump(fPtr);
        if (
          (area->size()      > parameters.getMaxSurfaceThreshold()) || 
          (area->size()      < parameters.getMinSurfaceThreshold()) ||
          (area->getWidth()  > parameters.getMaxWidthThreshold())   ||
          (area->getWidth()  < parameters.getMinWidthThreshold())   ||
          (area->getHeight() > parameters.getMaxHeightThreshold())  ||
          (area->getHeight() < parameters.getMinHeightThreshold())  
          )
        {
          for (int i=0; i < area->size(); i++)
          {
            coordinate = area->getCoordinateAt(i);
            fileFramePtr[coordinate->getY()*cols+coordinate->getX()] = 0;
            delete coordinate;
          }
          delete area;
          area = NULL;
        } 
        else
        {
//          fprintf(fPtr, "%d) %d-%d\n", areaList.size(), (*area).getLeft(), (*area).getBottom());
          //(*area).sortCoordinates((this->cols));
          areaList.push_back(area);
        }
      }    
    }
  }

  // clean upo the areas that do not have neighbours 
  CompareArea compare;
  compare.setWidth(cols);
  sort(areaList.begin(), areaList.end(), compare);
//  fprintf(fPtr, "Size %d\n", areaList.size());

  //dump(fPtr);
  for (areaIndex = 0; areaIndex < areaList.size(); areaIndex++)
  {
    area = areaList[areaIndex];
    //fprintf(fPtr, "%d) %d-%d\n", areaIndex, area->getLeft(), area->getBottom());
    //area->sortCoordinates(cols));
    if (prevArea != NULL)
    {
      if (
				(area->getBottom() ) <= (prevArea->getBottom() + LINE_THRESHOLD) &&
				(	
					((abs)(area->getLeft() - prevArea->getRight()) < area->getWidth() * 5) ||
					((abs)(area->getRight() - prevArea->getLeft()) < area->getWidth() * 5)
				)
				//&&((abs)(area->getLeft() - prevArea->getRight()) < area->getWidth() * 2)
				)
      {
        if (line == NULL)
        {
          line = new Line(prevArea);
        }
        else
        {
          line->addArea(prevArea);
        }
      }    
      else
      {
        if (line != NULL)
        {
          if ((line->size()) > 0)
          {
            line->addArea(prevArea);
            linesList.push_back(line);
         // fprintf(fPtr, "Char found on line %d\n", line->size());
            line = NULL;
          }
          else
          {
            for (int i=0; i < line->size(); i++)
            {
              area = line->getAreaAt(i);
              for (int j=0; j < area->size(); j++)
              {
                coordinate = area->getCoordinateAt(j);
                fileFramePtr[coordinate->getY()*cols+coordinate->getX()] = 0;
                delete coordinate;
              }
              delete area;
            }
            delete line;
          }
        }
      }
    }
    prevArea = area;
  }
  if (line != NULL)
  {
    if ((line->size()) > 0)
    {
      line->addArea(prevArea);
      linesList.push_back(line);
   // fprintf(fPtr, "Char found on line %d\n", line->size()); 


      line = NULL;
    }
    else
    {
      for (int i=0; i < line->size(); i++)
      {
        area = line->getAreaAt(i);
        for (int j=0; j < area->size(); j++)
        {
          coordinate = area->getCoordinateAt(j);
          fileFramePtr[coordinate->getY()*cols+coordinate->getX()] = 0;
          delete coordinate;
        }
        delete area;
      }
      delete line;
    }
  }
//laa 1/18/99
	if (linesList.size() > 1){
		for (int count1 = 0; count1 < linesList.size()-1; count1++){	
			if (linesList[count1]->ACCEPT){
				for (int count2 = count1+1; count2 < linesList.size(); count2++){
					if 
				 ( 
					(linesList[count2]->ACCEPT) &&
						
					(

					 (
					  ((abs)(linesList[count1]->getBottom() - linesList[count2]->getBottom()) < LINE_THRESHOLD+1) &&
					  ((abs)(linesList[count1]->getHeight() - linesList[count2]->getHeight()) < 10) &&
						(	
							((abs)(linesList[count1]->getLeft() - linesList[count2]->getRight()) < 40) ||
							((abs)(linesList[count1]->getRight() - linesList[count2]->getLeft()) < 40)
						)

					 ) ||
					 (
					  ( 
					   (linesList[count2]->getCentreX() < linesList[count1]->getRight()) &&
					   (linesList[count2]->getCentreX() > linesList[count1]->getLeft()) &&
					   (linesList[count2]->getCentreY() > linesList[count1]->getTop()) &&
					   (linesList[count2]->getCentreY() < linesList[count1]->getBottom()) 
					  ) ||
						( 
					   (linesList[count1]->getCentreX() < linesList[count2]->getRight()) &&
					   (linesList[count1]->getCentreX() > linesList[count2]->getLeft()) &&
					   (linesList[count1]->getCentreY() > linesList[count2]->getTop()) &&
					   (linesList[count1]->getCentreY() < linesList[count2]->getBottom()) 
					  ) 

						 
					 ) ||
					  
					 (( 
					  (
						 (
						   (linesList[count1]->getBottom() > linesList[count2]->getTop()) && 
					     (linesList[count1]->getTop() < linesList[count2]->getTop())
						 ) ||
						 (
						   (linesList[count2]->getBottom() > linesList[count1]->getTop()) && 
					     (linesList[count2]->getTop() < linesList[count1]->getTop())
						 ) ||
						 (
					    ( (abs)(linesList[count2]->getBottom() - linesList[count1]->getTop()) < 5) ||
					    ( (abs)(linesList[count2]->getTop() - linesList[count1]->getBottom()) < 5)
						 ) 
						) 
						&&
						( 
						 (
						  (linesList[count2]->getLeft() > linesList[count1]->getLeft()) || 
							( (abs)(linesList[count2]->getLeft() - linesList[count1]->getLeft()) < 5) 
						 ) &&
						 (
						  (linesList[count2]->getRight() < linesList[count1]->getRight())|| 
							( (abs)(linesList[count2]->getRight() - linesList[count1]->getRight()) < 5) 
						 ) 
						)
					 )) 
					
					)
				 )
				
				{
					linesList[count1]->addLine(linesList[count2]);
					linesList[count2]->ACCEPT = 0;
					

				}//if condition
			}//for count2
		}//if accept
		

	}//for count1
	}
/*	int position = 0;
	for (int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			if (fileFramePtr[position] > 0){
				fileFramePtr[position] = 255;

			}
		}
	}*/
	CompareLine compareLine;
  compareLine.setWidth(linesList.size());
  sort(linesList.begin(), linesList.end(), compareLine);
  return linesList.size();
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
void FrameFile::dump(FILE* fPtr) {
  int areaIndex;
  for (areaIndex = 0; areaIndex < areaList.size(); areaIndex++)
  {
    fprintf(fPtr, "%d) Size %d Bottom: %d Left %d\n", areaIndex, 
      areaList[areaIndex]->size(), 
      areaList[areaIndex]->getBottom(), areaList[areaIndex]->getLeft());
  }
}
/*************************************************************************
 * Procedure   : 
 * Purpose     : 
 * Input       : NA
 * Returns     : NA
 * Assumptions : NA
 *************************************************************************/
Line* FrameFile::getLineAt(int index) {
  return (linesList[index]);
}


