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
// purpose, assign or donate the code to a third party and inhibit third parties// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MultiViewSearch.h
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MultiViewSearch_H__
#define __MultiViewSearch_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"

//=============================================================================
//sth is this a case to use a parameter in the parameter file
//const int NumberOfViews = 2; // Number of 2D views, that are used for similarity retrieval

namespace XM
{
	
	// Forward Declarations:
	class MultiViewSearchTool;
	
	//=============================================================================
	class MultiViewSearchInterfaceABC: public I_SearchInterfaceABC
	{
	public:
		MultiViewSearchInterfaceABC();
		virtual ~MultiViewSearchInterfaceABC() {};

		virtual void destroy()=0;
		
		virtual int SetRefDescriptorInterface
			(MultiViewDescriptorInterfaceABC *aMultiViewDescriptorInterface) = 0;
		virtual int SetQueryDescriptorInterface
			(MultiViewDescriptorInterfaceABC *aMultiViewDescriptorInterface) = 0;
		//sth use normal interface and select distance function via
		//parameter file
		virtual double GetDistanceVIL(bool bVisibleViewsOnly) = 0;
		virtual double GetDistanceHHI(void) = 0;
		virtual double GetDistance2D(bool bVisibleViewsOnly) = 0;

		// accessor methods for sub-Search
		//sth use real name instead of SUB_SEARCH_INTERFACE_ABC
		virtual SUB_SEARCH_INTERFACE_ABC* Get2DSearchInterface() = 0;
		virtual void Set2DSearchInterface( SUB_SEARCH_INTERFACE_ABC* a2DSearchInterface ) = 0;

		static const UUID myID;
	};
	
	//=============================================================================
	class MultiViewSearchInterface: 
  public MultiViewSearchInterfaceABC
	{
	public:
		MultiViewSearchInterface(MultiViewSearchTool* aTool);
		virtual ~MultiViewSearchInterface();

		virtual void destroy();
		
		virtual const UUID& GetInterfaceID(void);
		virtual const char *GetName(void);
		
		virtual int SetRefDescriptorInterface
			(MultiViewDescriptorInterfaceABC *aMultiViewDescriptorInterface);
		virtual int SetQueryDescriptorInterface
			(MultiViewDescriptorInterfaceABC *aMultiViewDescriptorInterface);
		virtual double GetDistanceVIL(bool bVisibleViewsOnly);
		virtual double GetDistanceHHI(void);
		virtual double GetDistance2D(bool bVisibleViewsOnly);
		
		
		static const UUID myID;
		static const char *myName;

		// accessor methods for sub-Search
		virtual SUB_SEARCH_INTERFACE_ABC* Get2DSearchInterface();
		virtual void Set2DSearchInterface( SUB_SEARCH_INTERFACE_ABC* a2DSearchInterface );
		
	private:
		MultiViewSearchTool *m_SearchTool;
	};
	
	//=============================================================================
	class MultiViewSearchTool: public Search
	{
	friend class MultiViewSearchInterface;
	public:
		MultiViewSearchTool();
		MultiViewSearchTool(MultiViewDescriptorInterfaceABC *MultiView);
		
		virtual const UUID& GetObjectID(void);
		virtual const char *GetName(void);
		virtual MultiViewSearchInterfaceABC *GetInterface(void);
		
		virtual bool SupportsPush(void);
		virtual bool SupportsPull(void);
		
		virtual int SetRefDescriptorInterface
			(MultiViewDescriptorInterfaceABC *aMultiViewDescriptorInterface);
		virtual int SetQueryDescriptorInterface
			(MultiViewDescriptorInterfaceABC *aMultiViewDescriptorInterface);
		virtual double GetDistanceVIL(bool bVisibleViewsOnly);
		virtual double GetDistanceHHI(void);
		virtual double GetDistance2D(bool bVisibleViewsOnly);
		
		static const UUID myID;
		static const char * myName;

		// accessor methods for sub-Search
		virtual SUB_SEARCH_INTERFACE_ABC* Get2DSearchInterface();
		virtual void Set2DSearchInterface( SUB_SEARCH_INTERFACE_ABC* a2DSearchInterface );

	private:
		virtual ~MultiViewSearchTool();

		MultiViewSearchInterface m_Interface;
		MultiViewDescriptorInterfaceABC *m_RefDescriptorInterface;
		MultiViewDescriptorInterfaceABC *m_QueryDescriptorInterface;

		SUB_SEARCH_INTERFACE_ABC* m_pShape2DSearchInterface;

		double analyseResultMatrix(double** distance, int rank ); 
		int ilog2 (int x);

		int m_iPattern;
		int m_iNumberOfViews;
	};
	
};


#endif //__MultiViewSearch_H__
