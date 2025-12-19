//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MatchList.cpp
//

#include <math.h>
#include <float.h>
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================

using namespace XM;


//=============================================================================
MatchList::MatchList(void):
NoOfMatches(0),
DBEntryNumbers(0),
SubNumbers(0),
DistanceValues(0)
{
}

//----------------------------------------------------------------------------
MatchList::MatchList(int theNoOfMatches):
NoOfMatches(0),
DBEntryNumbers(0),
SubNumbers(0),
DistanceValues(0)
{
  SetNumberOfMatches(theNoOfMatches);
}

//----------------------------------------------------------------------------
MatchList::~MatchList()

{
  if (!NoOfMatches) return;

  if (DBEntryNumbers) delete [] DBEntryNumbers;
  if (SubNumbers) delete [] SubNumbers;
  if (DistanceValues) delete [] DistanceValues;
}

//----------------------------------------------------------------------------
void MatchList::SetNumberOfMatches(int theNoOfMatches)

{
  if (theNoOfMatches<=0) return;
  if (NoOfMatches) {
    if (DBEntryNumbers) delete [] DBEntryNumbers;
    if (SubNumbers) delete [] SubNumbers;
    if (DistanceValues) delete [] DistanceValues;
  }

  NoOfMatches=theNoOfMatches;
  DBEntryNumbers = new int[theNoOfMatches];
  SubNumbers = new int[theNoOfMatches];
  DistanceValues =  new double[theNoOfMatches];

  ResetMatchList();
}

//----------------------------------------------------------------------------
int MatchList::GetNumberOfMatches()

{
  return NoOfMatches;
}

//----------------------------------------------------------------------------
void MatchList::ResetMatchList()

{
  if (NoOfMatches<=0) return;
  for (int i=0; i<NoOfMatches; i++) {
    DBEntryNumbers[i]=-1;
    SubNumbers[i]=-1;
    DistanceValues[i]=DBL_MAX;
  }
}

//----------------------------------------------------------------------------
void MatchList::InsertMatch(int aDBEntryNo, double theDistanceValue)

{
  int i,j;
  if (NoOfMatches<=0) return;

  for (i=0; i<NoOfMatches; i++) {
    if (DistanceValues[i]>theDistanceValue) break;
  }
  if (i>=NoOfMatches) return;
  j=NoOfMatches-1;
  while (j>i) {
    DBEntryNumbers[j]=DBEntryNumbers[j-1];
    DistanceValues[j]=DistanceValues[j-1];
    j--;
  }
  DBEntryNumbers[i]=aDBEntryNo;
  SubNumbers[i]=-1;
  DistanceValues[i]=theDistanceValue;
}

//----------------------------------------------------------------------------
void MatchList::InsertMatch(int aDBEntryNo, double theDistanceValue,
			    int SubNumber)

{
  int i,j;
  if (NoOfMatches<=0) return;

  for (i=0; i<NoOfMatches; i++) {
    if (DistanceValues[i]>theDistanceValue) break;
  }
  if (i>=NoOfMatches) return;
  j=NoOfMatches-1;
  while (j>i) {
    DBEntryNumbers[j]=DBEntryNumbers[j-1];
    SubNumbers[j]=SubNumbers[j-1];
    DistanceValues[j]=DistanceValues[j-1];
    j--;
  }
  DBEntryNumbers[i]=aDBEntryNo;
  SubNumbers[i]=SubNumber;
  DistanceValues[i]=theDistanceValue;
}

//----------------------------------------------------------------------------
unsigned long MatchList::GetDBEntryNo(int index)

{
  return DBEntryNumbers[index];
}

//----------------------------------------------------------------------------
int MatchList::GetSubNumber(int index)

{
  return SubNumbers[index];
}

//----------------------------------------------------------------------------
double MatchList::GetDistanceValue(int index)

{
  return DistanceValues[index];
}

