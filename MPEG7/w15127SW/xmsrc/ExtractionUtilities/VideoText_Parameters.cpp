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
// Applicable File Name:  parameters.cpp
//
//////////////////////////////////////////////////////////////////////

#include "./VideoText_Parameters.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Parameters::Parameters(int width)
{
  switch (width)
  {
  case 160:
    (this->maxHeightThreshold)  = 40;
	  (this->minHeightThreshold)  = 4;
	  (this->maxWidthThreshold)   = 30;
	  (this->minWidthThreshold)   = 3;
	  (this->maxSurfaceThreshold) = 200;
	  (this->minSurfaceThreshold) = 10;
    break;
  case 320:
    (this->maxHeightThreshold)  = 70;
	  (this->minHeightThreshold)  = 5;
	  (this->maxWidthThreshold)   = 40;
	  (this->minWidthThreshold)   = 2;
	  (this->maxSurfaceThreshold) = 300;
	  (this->minSurfaceThreshold) = 15;
    break;
  default:
  // take 320 settings
    (this->maxHeightThreshold)  = 70;
	  (this->minHeightThreshold)  = 5;
	  (this->maxWidthThreshold)   = 40;
	  (this->minWidthThreshold)   = 2;
	  (this->maxSurfaceThreshold) = 300;
	  (this->minSurfaceThreshold) = 15;
    break;
  }
}

Parameters::~Parameters()
{
}

int Parameters::getMaxHeightThreshold()
{
  return (this->maxHeightThreshold);
}

int Parameters::getMinHeightThreshold()
{
  return (this->minHeightThreshold);
}

int Parameters::getMaxWidthThreshold()
{
  return (this->maxWidthThreshold);
}

int Parameters::getMinWidthThreshold()
{
  return (this->minWidthThreshold);
}

int Parameters::getMaxSurfaceThreshold()
{
  return (this->maxSurfaceThreshold);
}

int Parameters::getMinSurfaceThreshold()
{
  return (this->minSurfaceThreshold);
}
