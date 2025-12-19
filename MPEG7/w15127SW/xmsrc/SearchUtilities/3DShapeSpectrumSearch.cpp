//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Institut National des Telecommunications - ARTEMIS Project Unit, Titus Zaharia
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
// Applicable File Name:  3DShapeSpectrumSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <cstring>

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
	

using namespace XM;


const UUID ShapeSpectrumSearchInterface::myID = UUID("");
const char *ShapeSpectrumSearchInterface::myName = "Shape Spectrum Matching Interface";

const UUID ShapeSpectrumSearchTool::myID = UUID("");
const char *ShapeSpectrumSearchTool::myName = "Shape Spectrum Matching Tool";

const UUID ShapeSpectrumSearchInterfaceABC::myID = UUID();

//=============================================================================
ShapeSpectrumSearchInterfaceABC::ShapeSpectrumSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
ShapeSpectrumSearchInterface::ShapeSpectrumSearchInterface(ShapeSpectrumSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ShapeSpectrumSearchInterface::~ShapeSpectrumSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& ShapeSpectrumSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeSpectrumSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void ShapeSpectrumSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int ShapeSpectrumSearchInterface::
SetRefDescriptorInterface(ShapeSpectrumDescriptorInterfaceABC
		       *RefShapeSpectrumDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefShapeSpectrumDescriptorInterface);
}

//----------------------------------------------------------------------------
ShapeSpectrumDescriptorInterfaceABC* ShapeSpectrumSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int ShapeSpectrumSearchInterface::
SetQueryDescriptorInterface(ShapeSpectrumDescriptorInterfaceABC
		       *QueryShapeSpectrumDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryShapeSpectrumDescriptorInterface);
}

//----------------------------------------------------------------------------
double ShapeSpectrumSearchInterface::GetDistance(int MetricChoice)

{
  return m_SearchTool->GetDistance(MetricChoice);
}

//=============================================================================
ShapeSpectrumSearchTool::ShapeSpectrumSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
ShapeSpectrumSearchTool::
ShapeSpectrumSearchTool(ShapeSpectrumDescriptorInterfaceABC
			 *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	/* create descriptor if it does not exist*/
	if (!aQueryDescriptorInterface) {
	  ShapeSpectrumDescriptor *descriptor =
	    new ShapeSpectrumDescriptor();
	  aQueryDescriptorInterface=descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetQueryDescriptorInterface(aQueryDescriptorInterface);

	/* create sub coding schemes and
	   connect them with descriptor if availabe*/
	/* no sub matching tools (see how ColorHistCS if needed)*/

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ShapeSpectrumSearchTool::~ShapeSpectrumSearchTool()
{
	/* release descriptor*/
	if (m_RefDescriptorInterface)
	  m_RefDescriptorInterface->release();
	if (m_QueryDescriptorInterface)
	  m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& ShapeSpectrumSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeSpectrumSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool ShapeSpectrumSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool ShapeSpectrumSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int ShapeSpectrumSearchTool::
SetRefDescriptorInterface(ShapeSpectrumDescriptorInterfaceABC
		       *aRefDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_RefDescriptorInterface == 
	    aRefDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_RefDescriptorInterface) 
	  m_RefDescriptorInterface->release();

	/* add new interface*/
	m_RefDescriptorInterface = 
	  aRefDescriptorInterface;
	if (m_RefDescriptorInterface) {
	  m_RefDescriptorInterface->addref();

	  /* connect sub descriptors with sub search tools*/
	  /* not available*/
	}
	else {
	  /* if no descriptor, release also descriptor interfaces from
	     sub search tools*/
	  /* not available*/
	  return -1;
	}
	
	
	return 0;
}

//----------------------------------------------------------------------------
ShapeSpectrumDescriptorInterfaceABC* ShapeSpectrumSearchTool::
GetQueryDescriptorInterface(void)

{
        return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  ShapeSpectrumSearchTool::
SetQueryDescriptorInterface(ShapeSpectrumDescriptorInterfaceABC
		       *aQueryDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_QueryDescriptorInterface == 
	    aQueryDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_QueryDescriptorInterface) 
	  m_QueryDescriptorInterface->release();

	/* add new interface*/
	m_QueryDescriptorInterface = 
	  aQueryDescriptorInterface;
	if (m_QueryDescriptorInterface) {
	  m_QueryDescriptorInterface->addref();

	  /* conect sub descriptors with sub coding schemes*/
	  /* not available*/
	}
	else {
	  /* if no descriptor, release also descriptor interfaces from
	     sub extraction tools schemes*/
	  /* not available*/
	  return -1;
	}
	
	
	return 0;
}

//----------------------------------------------------------------------------
double ShapeSpectrumSearchTool::GetDistance(int MetricChoice)

{  
        double /*distance,*/sqrerr,preverr;

        /* check for descriptors*/
        if (!m_RefDescriptorInterface) return DBL_MAX;
        if (!m_QueryDescriptorInterface) return DBL_MAX;
	
	if (strcmp(m_RefDescriptorInterface->GetName(),
		   "Shape Spectrum Description Interface") != 0) {
	  return DBL_MAX;
	}
	
	if (strcmp(m_QueryDescriptorInterface->GetName(),
		   "Shape Spectrum Description Interface") != 0) {
	  return DBL_MAX;
	}

	if (m_RefDescriptorInterface->
	    GetShapeSpectrumSize() !=
	    m_QueryDescriptorInterface->
	    GetShapeSpectrumSize())
	  return DBL_MAX;

	/* perform matching */
	sqrerr=0;
	preverr=0;

	unsigned int i;
	unsigned long *dreful=0, *dqueryul=0; 
	dreful = (unsigned long *)malloc((2*m_RefDescriptorInterface->GetShapeSpectrumSize())*sizeof(unsigned long)); //sth please use new and check for delete
	if(!dreful) return -1.0;
	dqueryul = (unsigned long *)malloc((2*m_QueryDescriptorInterface->GetShapeSpectrumSize())*sizeof(unsigned long));
	if(!dqueryul) return -1.0; 
	
	for (i=0; i< m_QueryDescriptorInterface->
	       GetShapeSpectrumSize(); i++) 
	  {
	    dreful[i] = (unsigned long)m_RefDescriptorInterface->GetElement(i);
	    dqueryul[i] = (unsigned long)m_QueryDescriptorInterface->GetElement(i);
	  }
	
	dreful[i] = (unsigned long)m_RefDescriptorInterface->GetPlanarElement();
	dqueryul[i] = (unsigned long)m_QueryDescriptorInterface->GetPlanarElement(); 
	i++;
	dreful[i] = (unsigned long)m_RefDescriptorInterface->GetSingularElement();
	dqueryul[i] = (unsigned long)m_QueryDescriptorInterface->GetSingularElement();
	Dist3DSSD(dreful, dqueryul, m_QueryDescriptorInterface->GetShapeSpectrumSize(), m_QueryDescriptorInterface->GetNoOfBits(), MetricChoice, sqrerr);
	
	if(dreful) free(dreful);
	if(dqueryul) free(dqueryul);

	return sqrerr;
}

//----------------------------------------------------------------------------
ShapeSpectrumSearchInterfaceABC *ShapeSpectrumSearchTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
int Dist3DSSD(unsigned long *dreful, unsigned long *dqueryul, int n, int NoBits, int MetricChoice, double &err)
{
  int i; double Nivd;
  double *dref=0, *dq=0; 
  dref = (double *)malloc((n+2)*sizeof(double));
  if(!dref) return -1;   
  dq = (double *)malloc((n+2)*sizeof(double));
  if(!dq) return -1;

  Nivd = pow(2.0, (double)n);

  for(i=0;i<(n+2);i++)
    {
      dref[i] = (double)dreful[i];
      dq[i] = (double)dqueryul[i];
      
      dref[i] = (dref[i] + 0.5)/Nivd;
      dq[i] = (dq[i] + 0.5)/Nivd;
    }

  err = 0;
  switch(MetricChoice) 
    {
    case 0: err = dist_L1(dref, dq, n); break; 
    case 1: err = dist_L2(dref, dq, n); break;
    case 2: err = dist_pond(dref, dq, n); break;
      
    case 3: dist_L1(dref, dq, n+1); break; 
    case 4: dist_L2(dref, dq, n+1); break;
    case 5: dist_pond(dref, dq, n+1); break;
      
    case 6: dist_L1(dref, dq, n+2); break; 
    case 7: dist_L2(dref, dq, n+2); break;
    case 8: dist_pond(dref, dq, n+2); break;    
    }
  
  if (dref)
    free(dref); 
  if (dq)
    free(dq); 
  return 1; 
}




double dist_L1(double *dref, double *dq, int n)
{
  double dist=0.0; 
  int i; 
  double som1=0.0, som2=0.0;

  for(i=0;i<n;i++)
    {som1 += dref[i]; som2 += dq[i];}
  
   if(som1!=1)
    for(i=0;i<n;i++)
      {
	if(som1!=0.0)
	  dref[i] /= som1; 
      }
   if(som2!=1)
     for(i=0;i<n;i++)
       if(som2!=0.0)
	 dq[i] /= som2;

   for(i=0;i<n;i++)
    {
      dist += fabs(dref[i] - dq[i]);
    } 
   
   return dist; 
}

//sth delimiters
double dist_L2(double *dref, double *dq, int n)
{
  double dist=0.0; 
  int i; 
  double som1=0.0, som2=0.0;

  for(i=0;i<n;i++)
    {som1 += dref[i]; som2 += dq[i];}
  
   if(som1!=1)
    for(i=0;i<n;i++)
      {
	if(som1!=0.0)
	  dref[i] /= som1; 
      }
   if(som2!=1)
     for(i=0;i<n;i++)
       if(som2!=0.0)
	 dq[i] /= som2;

   for(i=0;i<n;i++)
    {
      dist += (dref[i] - dq[i]) * (dref[i] - dq[i]);
    } 
   
   return dist; 
}


double dist_pond(double *dref, double *dq, int n)
{
  double dist=0.0; 
  int i; 
  double som1=0.0, som2=0.0;

  for(i=0;i<n;i++)
    {som1 += dref[i]; som2 += dq[i];}
  
   if(som1!=1)
    for(i=0;i<n;i++)
      {
	if(som1!=0.0)
	  dref[i] /= som1; 
      }
   if(som2!=1)
     for(i=0;i<n;i++)
       if(som2!=0.0)
	 dq[i] /= som2;  


   for(i=0;i<n;i++)
     {      
       dist += ((dref[i] + dq[i]) / 2.0) / ( 1.0 + fabs(dref[i] - dq[i]) );
     }
   
  if(dist<0.5)
    dist = 0;
  else 
    dist = 2.0 * (dist-0.5);
  
  return 1.0-dist;

}
