///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Tisserand Patrice
// IRCAM
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  PercussiveInstrumentTimbreSearch.h
//

#ifndef __PERCUSSIVE_INSTRUMENT_TIMBRE_SEARCH_H__
#define __PERCUSSIVE_INSTRUMENT_TIMBRE_SEARCH_H__

#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
    class PercussiveInstrumentTimbreSearchTool;

//=============================================================================
    class PercussiveInstrumentTimbreSearchInterfaceABC: public I_SearchInterfaceABC
	{
	public:
	    PercussiveInstrumentTimbreSearchInterfaceABC();
	    virtual ~PercussiveInstrumentTimbreSearchInterfaceABC() {};

	    virtual void destroy()=0;

	    virtual int SetRefDescriptorInterface
		(PercussiveInstrumentTimbreDescriptorInterfaceABC
		 *aPercussiveInstrumentTimbreDescriptorInterface) = 0;
	    virtual PercussiveInstrumentTimbreDescriptorInterfaceABC*
		GetQueryDescriptorInterface(void) = 0;
	    virtual int SetQueryDescriptorInterface
		(PercussiveInstrumentTimbreDescriptorInterfaceABC
		 *aPercussiveInstrumentTimbreDescriptorInterface) = 0;

	    virtual double GetDistance(void) = 0;
	
	    /* sub-components do not have get and set functions because
	       there is only one output value*/

	    static const UUID myID;

	    virtual double GetLogAttackTimeWeight(void) = 0;
	    virtual void SetLogAttackTimeWeight(double) = 0;
	    virtual double GetSpectralCentroidWeight(void) = 0;
	    virtual void SetSpectralCentroidWeight(double) = 0;
	    virtual double GetTemporalCentroidWeight(void) = 0;
	    virtual void SetTemporalCentroidWeight(double) = 0;
	
	};

//=============================================================================
    class PercussiveInstrumentTimbreSearchInterface: 
	public PercussiveInstrumentTimbreSearchInterfaceABC
	{
	public:
	    PercussiveInstrumentTimbreSearchInterface(PercussiveInstrumentTimbreSearchTool* aTool);
	    virtual ~PercussiveInstrumentTimbreSearchInterface();
	
	    virtual const UUID& GetInterfaceID(void);
	    virtual const char *GetName(void);

	    virtual void destroy();

	    virtual int SetRefDescriptorInterface
		(PercussiveInstrumentTimbreDescriptorInterfaceABC
		 *aPercussiveInstrumentTimbreDescriptorInterface);
	    virtual PercussiveInstrumentTimbreDescriptorInterfaceABC*
		GetQueryDescriptorInterface(void);
	    virtual int SetQueryDescriptorInterface
		(PercussiveInstrumentTimbreDescriptorInterfaceABC
		 *aPercussiveInstrumentTimbreDescriptorInterface);

	    virtual double GetDistance(void);

	    virtual double GetLogAttackTimeWeight(void);
	    virtual void SetLogAttackTimeWeight(double);
	    virtual double GetSpectralCentroidWeight(void);
	    virtual void SetSpectralCentroidWeight(double);
	    virtual double GetTemporalCentroidWeight(void);
	    virtual void SetTemporalCentroidWeight(double);
	    

	    static const UUID myID;
	    static const char *myName;

	private:
	    PercussiveInstrumentTimbreSearchTool *m_SearchTool;
	};

//=============================================================================
    class PercussiveInstrumentTimbreSearchTool: public Search
	{
	    friend class PercussiveInstrumentTimbreSearchInterface;
	public:
	    PercussiveInstrumentTimbreSearchTool();
	    PercussiveInstrumentTimbreSearchTool(PercussiveInstrumentTimbreDescriptorInterfaceABC
				       *aQueryDescriptorInterface);

	    virtual const UUID& GetObjectID(void);
	    virtual const char *GetName(void);

	    virtual PercussiveInstrumentTimbreSearchInterfaceABC *GetInterface(void);

	    virtual bool SupportsPush(void);
	    virtual bool SupportsPull(void);

	    virtual int SetRefDescriptorInterface
		(PercussiveInstrumentTimbreDescriptorInterfaceABC
		 *aPercussiveInstrumentTimbreDescriptorInterface);
	    virtual PercussiveInstrumentTimbreDescriptorInterfaceABC*
		GetQueryDescriptorInterface(void);
	    virtual int SetQueryDescriptorInterface
		(PercussiveInstrumentTimbreDescriptorInterfaceABC
		 *aPercussiveInstrumentTimbreDescriptorInterface);

	    virtual double GetDistance(void);

	    static const UUID myID;
	    static const char * myName;
	private:
	    virtual ~PercussiveInstrumentTimbreSearchTool();

	    PercussiveInstrumentTimbreSearchInterface m_Interface;
	    PercussiveInstrumentTimbreDescriptorInterfaceABC *m_RefDescriptorInterface;
	    PercussiveInstrumentTimbreDescriptorInterfaceABC *m_QueryDescriptorInterface;

	    // Weighting coefficient
	    double m_LogAttackTimeWeight;
	    double m_SpectralCentroidWeight;
	    double m_TemporalCentroidWeight;


	    double GetLogAttackTimeWeight(void);
	    void SetLogAttackTimeWeight(double);
	    double GetSpectralCentroidWeight(void);
	    void SetSpectralCentroidWeight(double);
	    double GetTemporalCentroidWeight(void);
	    void SetTemporalCentroidWeight(double);

	    double GetPartDistanceLogAttackTimeTemporalCentroid(void);
	    double GetPartDistanceSpectralCentroid(void);
  
	};
}
#endif //__PERCUSSIVE_INSTRUMENT_TIMBRE_SEARCH_H__
