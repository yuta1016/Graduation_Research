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
// Applicable File Name:  MatchList.h
//


//=============================================================================

#ifndef _MATCHLIST_H_
#define _MATCHLIST_H_


namespace XM
{

//=============================================================================
  class MatchList
    {
    public:
      MatchList(void);
      MatchList(int NoOfMatches);
      ~MatchList(void);

      void ResetMatchList();
      int  GetNumberOfMatches(void);
      void SetNumberOfMatches(int NoOfMatches);
      void InsertMatch(int DBEntryNo, double DistanceValue);
      void InsertMatch(int DBEntryNo, double DistanceValue, int SubNumber);
      unsigned long GetDBEntryNo(int index);
      int GetSubNumber(int index);
      double GetDistanceValue(int index);

    protected:
      int NoOfMatches;
      int *DBEntryNumbers;
      int *SubNumbers;
      double *DistanceValues;
    };

}; //namespace

#endif // _MATCHLIST_H_
