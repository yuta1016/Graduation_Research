///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// HHI
// K. Mueller
//
// Mitsubishi Electric - Infomation Technology Europe - Visual Information Laboratory
// James Cooper
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
// Copyright (c) 2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MultiViewSearch.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <float.h>
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     // Naming Convention
#include "SearchUtilities/SearchUtilities.h"
#include "Utilities/readparam.h"

//=============================================================================


using namespace XM;


const UUID MultiViewSearchInterface::myID = UUID("");
//sth no space
const char *MultiViewSearchInterface::myName = "MultiView Matching Interface";

const UUID MultiViewSearchTool::myID = UUID("");
const char *MultiViewSearchTool::myName = "MultiView Matching Tool";

const UUID MultiViewSearchInterfaceABC::myID = UUID();

//=============================================================================
MultiViewSearchInterfaceABC::MultiViewSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
MultiViewSearchInterface::MultiViewSearchInterface(MultiViewSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MultiViewSearchInterface::~MultiViewSearchInterface()
{
}

//----------------------------------------------------------------------------
int MultiViewSearchInterface::
SetRefDescriptorInterface(MultiViewDescriptorInterfaceABC
													*RefMultiViewDescriptorInterface)
													
{
  return m_SearchTool->
    SetRefDescriptorInterface(RefMultiViewDescriptorInterface);
}

//----------------------------------------------------------------------------
int MultiViewSearchInterface::
SetQueryDescriptorInterface(MultiViewDescriptorInterfaceABC
														*QueryMultiViewDescriptorInterface)
														
{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryMultiViewDescriptorInterface);
}

//----------------------------------------------------------------------------
SUB_SEARCH_INTERFACE_ABC *MultiViewSearchInterface::Get2DSearchInterface()
{
        return m_SearchTool->Get2DSearchInterface();
}

//----------------------------------------------------------------------------
void MultiViewSearchInterface::Set2DSearchInterface(SUB_SEARCH_INTERFACE_ABC *a2DSearchInterface)
{
  m_SearchTool->Set2DSearchInterface(a2DSearchInterface);
}

//----------------------------------------------------------------------------
double MultiViewSearchInterface::GetDistanceHHI(void)
{
  return m_SearchTool->GetDistanceHHI();
}

double MultiViewSearchInterface::GetDistanceVIL(bool bVisibleViewsOnly)
{
  return m_SearchTool->GetDistanceVIL(bVisibleViewsOnly);
}
//----------------------------------------------------------------------------
double MultiViewSearchInterface::GetDistance2D(bool bVisibleViewsOnly)

{
  return m_SearchTool->GetDistance2D(bVisibleViewsOnly);
}

//----------------------------------------------------------------------------
const UUID& MultiViewSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MultiViewSearchInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void MultiViewSearchInterface::destroy(void)
{ delete m_SearchTool; }


//=============================================================================

MultiViewSearchTool::MultiViewSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0),
m_pShape2DSearchInterface(0)
{
	
	SUB_SEARCH_TOOL *m_2DSearch;

	m_iNumberOfViews = atoi(getpara("NumberOfViews", "2"));
	m_iPattern=~((0x1<<m_iNumberOfViews)-1);

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create sub coding schemes
	m_2DSearch = new SUB_SEARCH_TOOL();

	// connect sub coding schemes with 3D coding scheme
	if (m_2DSearch)
		Set2DSearchInterface(m_2DSearch->GetInterface());


  SetExposedInterface(&m_Interface);

}
//----------------------------------------------------------------------------

MultiViewSearchTool::MultiViewSearchTool(MultiViewDescriptorInterfaceABC *MultiView):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0),
m_pShape2DSearchInterface(0)
{
	
	SUB_SEARCH_TOOL *m_2DSearch;

	m_iNumberOfViews = atoi(getpara("NumberOfViews", "2"));
	m_iPattern=~((0x1<<m_iNumberOfViews)-1);

#ifdef VERBOSE
        fprintf(stderr,"Connect descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!MultiView) {
	  MultiViewDescriptor *descriptor =  new MultiViewDescriptor(0);
	  MultiView = descriptor->GetInterface();
	}

	// connect descritors with search tool
	SetQueryDescriptorInterface(MultiView);

	/* create sub search tool and
	   connect them with descriptor if availabe*/
	if (MultiView->Get2DDescriptorInterface(0)) 
		// create sub search tools
		m_2DSearch = new SUB_SEARCH_TOOL(MultiView->Get2DDescriptorInterface(0));
	else 
		m_2DSearch = new SUB_SEARCH_TOOL();

		// connect sub coding scheme with 3D coding scheme
	if (m_2DSearch)
		Set2DSearchInterface(m_2DSearch->GetInterface());
	

	SetExposedInterface(&m_Interface);


}

//----------------------------------------------------------------------------
MultiViewSearchTool::~MultiViewSearchTool()
{
  // destroy sub search tool scheme
	if (m_pShape2DSearchInterface) m_pShape2DSearchInterface->destroy();

	// release descriptor
	if (m_RefDescriptorInterface)
	  m_RefDescriptorInterface->release();
	if (m_QueryDescriptorInterface)
	  m_QueryDescriptorInterface->release();

}

//----------------------------------------------------------------------------
const UUID& MultiViewSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MultiViewSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool MultiViewSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool MultiViewSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int MultiViewSearchTool::SetRefDescriptorInterface(MultiViewDescriptorInterfaceABC
													*aRefMultiViewDescriptorInterface)
{
	// check if new value is different from old value
	if (m_RefDescriptorInterface == aRefMultiViewDescriptorInterface) 
		return 0;
	
	// release old interface
	if (m_RefDescriptorInterface) 
		m_RefDescriptorInterface->release();
	
	// add new interface
	m_RefDescriptorInterface = aRefMultiViewDescriptorInterface;
	if (m_RefDescriptorInterface)
	{
		m_RefDescriptorInterface->addref();
		
		// conect sub descriptors with sub search tool
		if ((m_RefDescriptorInterface->Get2DDescriptorInterface(0)) && m_pShape2DSearchInterface)
			m_pShape2DSearchInterface->SetRefDescriptorInterface(m_RefDescriptorInterface->
					Get2DDescriptorInterface(0));
	}
	else 
	{
	  // if no descriptor, release also descriptor interfaces from sub coding schemes
		m_pShape2DSearchInterface->SetRefDescriptorInterface(0);

		return -1;
	}
	
	
	return 0;
}

//----------------------------------------------------------------------------
int MultiViewSearchTool::SetQueryDescriptorInterface(MultiViewDescriptorInterfaceABC
													*aQueryMultiViewDescriptorInterface)
{
	// check if new value is different from old value
	if (m_QueryDescriptorInterface == aQueryMultiViewDescriptorInterface) 
		return 0;
	
	// release old interface
	if (m_QueryDescriptorInterface) 
		m_QueryDescriptorInterface->release();
	
	// add new interface
	m_QueryDescriptorInterface = aQueryMultiViewDescriptorInterface;
	if (m_QueryDescriptorInterface)
	{
		m_QueryDescriptorInterface->addref();
		
		// conect sub descriptors with sub search tool
		if ((m_QueryDescriptorInterface->Get2DDescriptorInterface(0)) && m_pShape2DSearchInterface)
			m_pShape2DSearchInterface->SetQueryDescriptorInterface(m_QueryDescriptorInterface->
			  Get2DDescriptorInterface(0));
	}
	else 
	{
	  // if no descriptor, release also descriptor interfaces from sub search tool
		for( int i = 0; i < TotNumberOfViews; i++ )
			m_pShape2DSearchInterface->SetQueryDescriptorInterface(0);

		return -1;
	}
	
	
	return 0;
}

//----------------------------------------------------------------------------
SUB_SEARCH_INTERFACE_ABC *MultiViewSearchTool::Get2DSearchInterface()
{
  return m_pShape2DSearchInterface;
}

//----------------------------------------------------------------------------
void MultiViewSearchTool::
Set2DSearchInterface(SUB_SEARCH_INTERFACE_ABC  *a2DSearchInterface)
{
  m_pShape2DSearchInterface = a2DSearchInterface;
}

//----------------------------------------------------------------------------
double MultiViewSearchTool::GetDistance2D(bool bVisibleViewsOnly)
{
	
  // Merge 2D distance measure to 3D measure
	double dist;

	double totDistance = DBL_MAX;

	SUB_DESCRIPTOR_INTERFACE_ABC *theDescriptorInterface;
	//theDescriptorInterface = GetShapeSearch()->GetInterface();
	//m_pShapeSearchTool->SetQueryDescriptorInterface( theDescriptorInterface );

	for( int i = 0; i < m_RefDescriptorInterface->GetNoOfViews(); i++ ) 
	{
		if(!bVisibleViewsOnly || (bVisibleViewsOnly && m_RefDescriptorInterface->GetVisibleViewFlag(i)))
		{
			// get Interfaces to Ref- and Query Descriptor and link them to 2D Measure Tool
			theDescriptorInterface = m_RefDescriptorInterface->Get2DDescriptorInterface(i);
			m_pShape2DSearchInterface->SetRefDescriptorInterface( theDescriptorInterface );

			dist = m_pShape2DSearchInterface->GetDistance();
			if(dist < totDistance) totDistance = dist;
		}

	} // End for( int i = 0; i < NumberOfViews, i++ )
	
	return totDistance;//DBL_MAX;
}

//-----------------------------------------------------------------

double MultiViewSearchTool::GetDistanceVIL(bool bVisibleViewsOnly)
{	
	double dists[8] = {DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX};

	double totDistance=0;
	int i, j;

	for( i = 0; i < m_QueryDescriptorInterface->GetNoOfViews(); i++ )
	{
		for( j = 0; j < m_RefDescriptorInterface->GetNoOfViews(); j++ )
		{
			// if visible view only are required make sure view is visible!
			if(!bVisibleViewsOnly || (bVisibleViewsOnly && m_RefDescriptorInterface->GetVisibleViewFlag(j)))
			{
				// get Interfaces to Ref- and Query Descriptor and link them to 2D Measure Tool
			
				SUB_DESCRIPTOR_INTERFACE_ABC *theDescriptorInterface;
				theDescriptorInterface = m_QueryDescriptorInterface->Get2DDescriptorInterface(i);
				m_pShape2DSearchInterface->SetQueryDescriptorInterface( theDescriptorInterface );
		
				theDescriptorInterface = m_RefDescriptorInterface->Get2DDescriptorInterface(j);
				m_pShape2DSearchInterface->SetRefDescriptorInterface( theDescriptorInterface );

				double cost = m_pShape2DSearchInterface->GetDistance();

				// best cost for this query view
				if(dists[i] > cost) dists[i] = cost;			
			}
		}
	}

	// Calculate the cost of the match(average).
	for( i = 0; i < m_QueryDescriptorInterface->GetNoOfViews(); i++)
		totDistance += dists[i];

	totDistance /= m_QueryDescriptorInterface->GetNoOfViews();

	return totDistance;
}

//----------------------------------------------------------------------------
double MultiViewSearchTool::GetDistanceHHI(void)
{
	
  // Merge 2D distance measure to 3D measure
	double **dist;
	int i;
	dist = new double*[m_iNumberOfViews];
	for( i = 0; i < m_iNumberOfViews; i++)
		dist[i] = new double[m_iNumberOfViews];
	//double dist[NumberOfViews][NumberOfViews];// = {{ 1,2,3,4},{5,7,9,11}, {12,15,18,21}, {22,26,30,34}};

//	double accumDistance;
	double totDistance = DBL_MAX;

	for( i = 0; i < m_iNumberOfViews; i++ ) {
		for( int j = 0; j < m_iNumberOfViews; j++ ) {
			// get Interfaces to Ref- and Query Descriptor and link them to 2D Measure Tool
			
			SUB_DESCRIPTOR_INTERFACE_ABC *theDescriptorInterface;
			theDescriptorInterface = m_QueryDescriptorInterface->Get2DDescriptorInterface(i);
			m_pShape2DSearchInterface->SetQueryDescriptorInterface( theDescriptorInterface );
	
			theDescriptorInterface = m_RefDescriptorInterface->Get2DDescriptorInterface(j);
			m_pShape2DSearchInterface->SetRefDescriptorInterface( theDescriptorInterface );

			dist[i][j] = m_pShape2DSearchInterface->GetDistance();
		}
	} // End for( i = 0; i < NumberOfViews, i++ )
	
	totDistance = analyseResultMatrix(dist, m_iNumberOfViews );

	for( i = 0; i < m_iNumberOfViews; i++)
		delete [] dist[i];
	delete [] dist;

	return totDistance;//DBL_MAX;
}

//----------------------------------------------------------------------------
double MultiViewSearchTool::analyseResultMatrix(double **dist, int rank ) 
{
	
	if( rank < 2 ) {
		int index_i = ilog2(~m_iPattern); // analyse pattern for colomn position
		return dist[m_iNumberOfViews-1][index_i];
	}

	double totalMin = DBL_MAX;
	for( int i = 0; i < m_iNumberOfViews; i++ )
	{
		if((0x1<<i)&m_iPattern) continue; // go through the matrix' columns and skip the marked coloms
		m_iPattern |= (0x1<<i); // mark collumn for next recursive function call
		//pos_i = i+1;
		// add distance value of current loop index i to iterative result from sub-Matrix
		double minPart1 = analyseResultMatrix(dist, rank-1 );
		double minPart2 = dist[m_iNumberOfViews-rank][i];

		m_iPattern &= (~(0x1<<i));// unmark collumn after recursive function call

		if(minPart1 == DBL_MAX) continue;
		if(minPart2 == DBL_MAX) continue;
		double tempMin = minPart1 + minPart2;
		if( tempMin < totalMin ) totalMin = tempMin;
	}

	return totalMin;
}
//sth unknow delimiter
///////////////////////
int MultiViewSearchTool::ilog2 (int x) {
   int count = 0;
   while (x > 1)  {
      x >>= 1;
      count++;
   }
   return count;
}

//----------------------------------------------------------------------------
MultiViewSearchInterfaceABC *MultiViewSearchTool::GetInterface(void)
{ return &m_Interface; }

