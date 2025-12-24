///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric ITE-VIL
// W.Price
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ContourShapeSearch.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <float.h>

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     // Naming Convention
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================

using namespace XM;


const UUID ContourShapeSearchInterface::myID = UUID("");
const char *ContourShapeSearchInterface::myName = "Contour-Based Shape Matching Interface";

const UUID ContourShapeSearchTool::myID = UUID("");
const char *ContourShapeSearchTool::myName = "Contour-Based Shape Matching Tool";

const UUID ContourShapeSearchInterfaceABC::myID = UUID();

//=============================================================================
ContourShapeSearchInterfaceABC::ContourShapeSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
ContourShapeSearchInterface::ContourShapeSearchInterface(ContourShapeSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ContourShapeSearchInterface::~ContourShapeSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& ContourShapeSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ContourShapeSearchInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void ContourShapeSearchInterface::destroy(void)
{
  delete m_SearchTool;
}

//----------------------------------------------------------------------------
int ContourShapeSearchInterface::
SetRefDescriptorInterface(ContourShapeDescriptorInterfaceABC
		       *RefContourShapeDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefContourShapeDescriptorInterface);
}

//----------------------------------------------------------------------------
int ContourShapeSearchInterface::
SetQueryDescriptorInterface(ContourShapeDescriptorInterfaceABC
		       *QueryContourShapeDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryContourShapeDescriptorInterface);
}

//----------------------------------------------------------------------------
double ContourShapeSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}



//=============================================================================
ContourShapeSearchTool::ContourShapeSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0),
m_rPeaksX(0), m_rPeaksY(0), m_qPeaksX(0), m_qPeaksY(0)
{
  SetExposedInterface(&m_Interface);

  m_rPeaksX = new float[(CONTOURSHAPE_CSSPEAKMASK)+1];
  m_rPeaksY = new float[(CONTOURSHAPE_CSSPEAKMASK)+1];
  m_qPeaksX = new float[(CONTOURSHAPE_CSSPEAKMASK)+1];
  m_qPeaksY = new float[(CONTOURSHAPE_CSSPEAKMASK)+1];

  m_nodeList = new Node[4*((CONTOURSHAPE_CSSPEAKMASK)+1)*((CONTOURSHAPE_CSSPEAKMASK)+1)];
}

//----------------------------------------------------------------------------
ContourShapeSearchTool::
ContourShapeSearchTool(ContourShapeDescriptorInterfaceABC
                       *aQueryDescriptorInterface)
: m_Interface(this)
, m_RefDescriptorInterface(0)
, m_QueryDescriptorInterface(0)
{
#ifdef VERBOSE
  fprintf(stderr, "Connect descriptor\n");
#endif

  if (aQueryDescriptorInterface)
  {
    ContourShapeDescriptor *descriptor = new ContourShapeDescriptor();
    aQueryDescriptorInterface = descriptor->GetInterface();
  }

  SetQueryDescriptorInterface(aQueryDescriptorInterface);

  SetExposedInterface(&m_Interface);

  m_rPeaksX = new float[(CONTOURSHAPE_CSSPEAKMASK)+1];
  m_rPeaksY = new float[(CONTOURSHAPE_CSSPEAKMASK)+1];
  m_qPeaksX = new float[(CONTOURSHAPE_CSSPEAKMASK)+1];
  m_qPeaksY = new float[(CONTOURSHAPE_CSSPEAKMASK)+1];

  m_nodeList = new Node[4*((CONTOURSHAPE_CSSPEAKMASK)+1)*((CONTOURSHAPE_CSSPEAKMASK)+1)];
}

//----------------------------------------------------------------------------
ContourShapeSearchTool::~ContourShapeSearchTool()
{
  if (m_rPeaksX) delete[] m_rPeaksX;
  if (m_rPeaksY) delete[] m_rPeaksY;
  if (m_qPeaksX) delete[] m_qPeaksX;
  if (m_qPeaksY) delete[] m_qPeaksY;

  if (m_nodeList) delete[] m_nodeList;

  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& ContourShapeSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ContourShapeSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool ContourShapeSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool ContourShapeSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int ContourShapeSearchTool::
SetRefDescriptorInterface(ContourShapeDescriptorInterfaceABC
		       *aRefContourShapeDescriptorInterface)

{
  if (m_RefDescriptorInterface ==
      aRefContourShapeDescriptorInterface) return 0;

  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();

  m_RefDescriptorInterface = aRefContourShapeDescriptorInterface;
  if (m_RefDescriptorInterface)
  {
    m_RefDescriptorInterface->addref();
  }
  else
  {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
int  ContourShapeSearchTool::
SetQueryDescriptorInterface(ContourShapeDescriptorInterfaceABC
		       *aQueryContourShapeDescriptorInterface)

{
  if (m_QueryDescriptorInterface ==
      aQueryContourShapeDescriptorInterface) return 0;

  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();

  m_QueryDescriptorInterface = aQueryContourShapeDescriptorInterface;
  if (m_QueryDescriptorInterface)
  {
    m_QueryDescriptorInterface->addref();
  }
  else
  {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
static inline float range(float x)
{
  while (x < 0.0)  x += 1.0;
  while (x >= 1.0) x -= 1.0;
  return x;
}

//----------------------------------------------------------------------------
double ContourShapeSearchTool::GetDistance(void)

{
  if (!m_RefDescriptorInterface) return DBL_MAX;
  if (!m_QueryDescriptorInterface) return DBL_MAX;

  if (strcmp(m_RefDescriptorInterface->GetName(),
             "Contour-Based Shape Description Interface") != 0)
  {
    return DBL_MAX;
  }

  if (strcmp(m_QueryDescriptorInterface->GetName(),
             "Contour-Based Shape Description Interface") != 0)
  {
    return DBL_MAX;
  }

  unsigned long lRefE, lRefC, lQueryE, lQueryC;
  m_RefDescriptorInterface->GetGlobalCurvature(lRefC, lRefE);
  m_QueryDescriptorInterface->GetGlobalCurvature(lQueryC, lQueryE);

  float fRefE = 0.5 + lRefE, fRefC = 0.5 + lRefC;
  float fQueryE = 0.5 + lQueryE, fQueryC = 0.5 + lQueryC;
  float fDenomE = (fRefE > fQueryE) ? fRefE : fQueryE;
  float fDenomC = (fRefC > fQueryC) ? fRefC : fQueryC;
  fDenomE += CONTOURSHAPE_EMIN * (CONTOURSHAPE_EMASK+1) / (CONTOURSHAPE_EMAX - CONTOURSHAPE_EMIN);
  fDenomC += CONTOURSHAPE_CMIN * (CONTOURSHAPE_CMASK+1) / (CONTOURSHAPE_CMAX - CONTOURSHAPE_CMIN);
  if ((fabs(fRefE - fQueryE) >= CONTOURSHAPE_ETHR * fDenomE) ||
      (fabs(fRefC - fQueryC) >= CONTOURSHAPE_CTHR * fDenomC))
    return DBL_MAX;
  float tCost = (CONTOURSHAPE_ECOST*fabs(fRefE - fQueryE)/fDenomE) +
                (CONTOURSHAPE_CCOST*fabs(fRefC - fQueryC)/fDenomC);

  int nRefPeaks   = m_RefDescriptorInterface->GetNoOfPeaks();
  int nQueryPeaks = m_QueryDescriptorInterface->GetNoOfPeaks();
  for (int nr = 0; nr < nRefPeaks; nr++)
  {
    unsigned short irx, iry;
    m_RefDescriptorInterface->GetPeak(nr, irx, iry);
    m_rPeaksX[nr] = (irx * CONTOURSHAPE_XMAX / (float)CONTOURSHAPE_XMASK);
    if (nr == 0)
      m_rPeaksY[nr] = (iry * CONTOURSHAPE_YMAX / (float)CONTOURSHAPE_YMASK);
    else
      m_rPeaksY[nr] = (iry * m_rPeaksY[nr-1] / (float)CONTOURSHAPE_YnMASK);
  }

  for (int nq = 0; nq < nQueryPeaks; nq++)
  {
    unsigned short iqx, iqy;
    m_QueryDescriptorInterface->GetPeak(nq, iqx, iqy);
    m_qPeaksX[nq] = (iqx * CONTOURSHAPE_XMAX / (float)CONTOURSHAPE_XMASK);
    if (nq == 0)
      m_qPeaksY[nq] = (iqy * CONTOURSHAPE_YMAX / (float)CONTOURSHAPE_YMASK);
    else
      m_qPeaksY[nq] = (iqy * m_qPeaksY[nq-1] / (float)CONTOURSHAPE_YnMASK);
  }

  int nNodes = 0;
  for (int i0 = 0; (i0 < nRefPeaks) && (i0 < CONTOURSHAPE_NMATCHPEAKS); i0++)
  {
    float iRefX = m_rPeaksX[i0];
    float iRefY = m_rPeaksY[i0];

    for (int j0 = 0; j0 < nQueryPeaks; j0++)
    {
      float iQueryX = m_qPeaksX[j0];
      float iQueryY = m_qPeaksY[j0];
      float denom = (iRefY > iQueryY) ? iRefY : iQueryY;
      if ((fabs(iRefY - iQueryY) / denom) < 0.7)
      {
        m_nodeList[nNodes].cost   = 0.0;
        m_nodeList[nNodes].nr     = nRefPeaks;
        m_nodeList[nNodes].nq     = nQueryPeaks;

        m_nodeList[nNodes+1].cost = 0.0;
        m_nodeList[nNodes+1].nr   = nRefPeaks;
        m_nodeList[nNodes+1].nq   = nQueryPeaks;

        for (int pr = 0; pr < nRefPeaks; pr++)
        {
          float frx = range(m_rPeaksX[pr] - iRefX);
          float fry = m_rPeaksY[pr];

          m_nodeList[nNodes].rPeaks[pr].x = frx;
          m_nodeList[nNodes].rPeaks[pr].y = fry;

          m_nodeList[nNodes+1].rPeaks[pr].x = frx;
          m_nodeList[nNodes+1].rPeaks[pr].y = fry;
        }

        for (int pq = 0; pq < nQueryPeaks; pq++)
        {
          float fqx = range(m_qPeaksX[pq] - iQueryX);
          float fqy = m_qPeaksY[pq];

          m_nodeList[nNodes].qPeaks[pq].x = fqx;
          m_nodeList[nNodes].qPeaks[pq].y = fqy;

          m_nodeList[nNodes+1].qPeaks[pq].x = range(-1.0 * fqx);
          m_nodeList[nNodes+1].qPeaks[pq].y = fqy;
        }

        nNodes += 2;
      }
    }
  }

  if (nRefPeaks == 0)
  {
    m_nodeList[nNodes].cost   = 0.0;
    m_nodeList[nNodes].nr     = nRefPeaks;
    m_nodeList[nNodes].nq     = nQueryPeaks;

    for (int pq = 0; pq < nQueryPeaks; pq++)
    {
      float fqx = m_qPeaksX[pq];
      float fqy = m_qPeaksY[pq];

      m_nodeList[nNodes].qPeaks[pq].x = fqx;
      m_nodeList[nNodes].qPeaks[pq].y = fqy;
    }

    nNodes++;
  }

  for (int i1 = 0; (i1 < nQueryPeaks) && (i1 < CONTOURSHAPE_NMATCHPEAKS); i1++)
  {
    float iQueryX = m_qPeaksX[i1];
    float iQueryY = m_qPeaksY[i1];

    for (int j1 = 0; j1 < nRefPeaks; j1++)
    {
      float iRefX = m_rPeaksX[j1];
      float iRefY = m_rPeaksY[j1];
      float denom = (iQueryY > iRefY) ? iQueryY : iRefY;
      if ((fabs(iQueryY - iRefY) / denom) < 0.7)
      {
        m_nodeList[nNodes].cost   = 0.0;
        m_nodeList[nNodes].nr     = nQueryPeaks;
        m_nodeList[nNodes].nq     = nRefPeaks;

        m_nodeList[nNodes+1].cost = 0.0;
        m_nodeList[nNodes+1].nr   = nQueryPeaks;
        m_nodeList[nNodes+1].nq   = nRefPeaks;

        for (int pq = 0; pq < nQueryPeaks; pq++)
        {
          float fqx = range(m_qPeaksX[pq] - iQueryX);
          float fqy = m_qPeaksY[pq];

          m_nodeList[nNodes].rPeaks[pq].x = fqx;
          m_nodeList[nNodes].rPeaks[pq].y = fqy;

          m_nodeList[nNodes+1].rPeaks[pq].x = range(-1.0 * fqx);
          m_nodeList[nNodes+1].rPeaks[pq].y = fqy;
        }

        for (int pr = 0; pr < nRefPeaks; pr++)
        {
          float frx = range(m_rPeaksX[pr] - iRefX);
          float fry = m_rPeaksY[pr];

          m_nodeList[nNodes].qPeaks[pr].x = frx;
          m_nodeList[nNodes].qPeaks[pr].y = fry;

          m_nodeList[nNodes+1].qPeaks[pr].x = frx;
          m_nodeList[nNodes+1].qPeaks[pr].y = fry;
        }

        nNodes += 2;
      }
    }
  }

  if (nQueryPeaks == 0)
  {
    m_nodeList[nNodes].cost   = 0.0;
    m_nodeList[nNodes].nr     = nQueryPeaks;
    m_nodeList[nNodes].nq     = nRefPeaks;

    for (int pr = 0; pr < nRefPeaks; pr++)
    {
      float frx = m_rPeaksX[pr];
      float fry = m_rPeaksY[pr];

      m_nodeList[nNodes].qPeaks[pr].x = frx;
      m_nodeList[nNodes].qPeaks[pr].y = fry;
    }

    nNodes++;
  }

  if (nNodes == 0)
    return DBL_MAX;

  int index = 0;
  while ((m_nodeList[index].nr > 0) || (m_nodeList[index].nq > 0))
  {
    int ir = -1, iq = -1;

    if ((m_nodeList[index].nr > 0) && (m_nodeList[index].nq > 0))
    {
      ir = 0;
      for (int mr = 1; mr < m_nodeList[index].nr; mr++)
      {
        if (m_nodeList[index].rPeaks[ir].y < m_nodeList[index].rPeaks[mr].y)
          ir = mr;
      }
      iq = 0;
      float xd = fabs(m_nodeList[index].rPeaks[ir].x - m_nodeList[index].qPeaks[iq].x);
      if (xd > 0.5) xd = 1.0 - xd;
      float yd = fabs(m_nodeList[index].rPeaks[ir].y - m_nodeList[index].qPeaks[iq].y);
      float sqd = xd*xd + yd*yd;
      for (int mq = 1; mq < m_nodeList[index].nq; mq++)
      {
        xd = fabs(m_nodeList[index].rPeaks[ir].x - m_nodeList[index].qPeaks[mq].x);
        if (xd > 0.5) xd = 1.0 - xd;
        yd = fabs(m_nodeList[index].rPeaks[ir].y - m_nodeList[index].qPeaks[mq].y);
        float d = xd*xd + yd*yd;
        if (d < sqd)
        {
          sqd = d;
          iq = mq;
        }
      }

      float dx = fabs(m_nodeList[index].rPeaks[ir].x - m_nodeList[index].qPeaks[iq].x);
      if (dx > 0.5) dx = 1.0 - dx;

      if (dx < 0.1)
      {
        float dy = fabs(m_nodeList[index].rPeaks[ir].y - m_nodeList[index].qPeaks[iq].y);
        m_nodeList[index].cost += sqrt(dx*dx + dy*dy);
        if (ir < --m_nodeList[index].nr)
          memmove(&m_nodeList[index].rPeaks[ir], &m_nodeList[index].rPeaks[ir+1], (m_nodeList[index].nr-ir)*sizeof(m_nodeList[index].rPeaks[0]));
        if (iq < --m_nodeList[index].nq)
          memmove(&m_nodeList[index].qPeaks[iq], &m_nodeList[index].qPeaks[iq+1], (m_nodeList[index].nq-iq)*sizeof(m_nodeList[index].qPeaks[0]));
      }
      else
      {
        m_nodeList[index].cost += m_nodeList[index].rPeaks[ir].y;
        if (ir < --m_nodeList[index].nr)
          memmove(&m_nodeList[index].rPeaks[ir], &m_nodeList[index].rPeaks[ir+1], (m_nodeList[index].nr-ir)*sizeof(m_nodeList[index].rPeaks[0]));
      }
    }
    else if (m_nodeList[index].nr > 0)
    {
      m_nodeList[index].cost += m_nodeList[index].rPeaks[0].y;
      if (--m_nodeList[index].nr > 0)
        memmove(&m_nodeList[index].rPeaks[0], &m_nodeList[index].rPeaks[1], (m_nodeList[index].nr)*sizeof(m_nodeList[index].rPeaks[0]));
    }
    else // if (m_nodeList[index].nq > 0)
    {
      m_nodeList[index].cost += m_nodeList[index].qPeaks[0].y;
      if (--m_nodeList[index].nq > 0)
        memmove(&m_nodeList[index].qPeaks[0], &m_nodeList[index].qPeaks[1], (m_nodeList[index].nq)*sizeof(m_nodeList[index].qPeaks[0]));
    }

    index = 0;
    double minCost = m_nodeList[index].cost;
    for (int c0 = 1; c0 < nNodes; c0++)
    {
      if (m_nodeList[c0].cost < minCost)
      {
        index = c0;
        minCost = m_nodeList[c0].cost;
      }
    }
  }

  double cost = m_nodeList[index].cost + tCost;

  return cost;
}

//----------------------------------------------------------------------------
ContourShapeSearchInterfaceABC *ContourShapeSearchTool::GetInterface(void)
{ return &m_Interface; }

