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
// Applicable File Name:  HarmonicInstrumentTimbreSearch.h
//

#ifndef __HARMONIC_INSTRUMENT_TIMBRE_SEARCH_H__
#define __HARMONIC_INSTRUMENT_TIMBRE_SEARCH_H__

#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
    class HarmonicInstrumentTimbreSearchTool;

//=============================================================================
    class HarmonicInstrumentTimbreSearchInterfaceABC: public I_SearchInterfaceABC
	{
	public:
	    HarmonicInstrumentTimbreSearchInterfaceABC();
	    virtual ~HarmonicInstrumentTimbreSearchInterfaceABC() {};

	    virtual void destroy()=0;

	    virtual int SetRefDescriptorInterface
		(HarmonicInstrumentTimbreDescriptorInterfaceABC
		 *aHarmonicInstrumentTimbreDescriptorInterface) = 0;
	    virtual HarmonicInstrumentTimbreDescriptorInterfaceABC*
		GetQueryDescriptorInterface(void) = 0;
	    virtual int SetQueryDescriptorInterface
		(HarmonicInstrumentTimbreDescriptorInterfaceABC
		 *aHarmonicInstrumentTimbreDescriptorInterface) = 0;

	    virtual double GetDistance(void) = 0;
	
	    /* sub-components do not have get and set functions because
	       there is only one output value*/

	    static const UUID myID;

	    virtual double GetLogAttackTimeWeight(void) = 0;
	    virtual void SetLogAttackTimeWeight(double) = 0;
	    virtual double GetHarmonicSpectralCentroidWeight(void) = 0;
	    virtual void SetHarmonicSpectralCentroidWeight(double) = 0;
	    virtual double GetHarmonicSpectralDeviationWeight(void) = 0;
	    virtual void SetHarmonicSpectralDeviationWeight(double) = 0;
	    virtual double GetHarmonicSpectralSpreadWeight(void) = 0;
	    virtual void SetHarmonicSpectralSpreadWeight(double) = 0;
	    virtual double GetHarmonicSpectralVariationWeight(void) = 0;
	    virtual void SetHarmonicSpectralVariationWeight(double) = 0;


	};

//=============================================================================
    class HarmonicInstrumentTimbreSearchInterface: 
	public HarmonicInstrumentTimbreSearchInterfaceABC
	{
	public:
	    HarmonicInstrumentTimbreSearchInterface(HarmonicInstrumentTimbreSearchTool* aTool);
	    virtual ~HarmonicInstrumentTimbreSearchInterface();
	
	    virtual const UUID& GetInterfaceID(void);
	    virtual const char *GetName(void);

	    virtual void destroy();

	    virtual int SetRefDescriptorInterface
		(HarmonicInstrumentTimbreDescriptorInterfaceABC
		 *aHarmonicInstrumentTimbreDescriptorInterface);
	    virtual HarmonicInstrumentTimbreDescriptorInterfaceABC*
		GetQueryDescriptorInterface(void);
	    virtual int SetQueryDescriptorInterface
		(HarmonicInstrumentTimbreDescriptorInterfaceABC
		 *aHarmonicInstrumentTimbreDescriptorInterface);

	    virtual double GetDistance(void);

	    virtual double GetLogAttackTimeWeight(void);
	    virtual void SetLogAttackTimeWeight(double);
	    virtual double GetHarmonicSpectralCentroidWeight(void);
	    virtual void SetHarmonicSpectralCentroidWeight(double);
	    virtual double GetHarmonicSpectralDeviationWeight(void);
	    virtual void SetHarmonicSpectralDeviationWeight(double);
	    virtual double GetHarmonicSpectralSpreadWeight(void);
	    virtual void SetHarmonicSpectralSpreadWeight(double);
	    virtual double GetHarmonicSpectralVariationWeight(void);
	    virtual void SetHarmonicSpectralVariationWeight(double);


	    static const UUID myID;
	    static const char *myName;

	private:
	    HarmonicInstrumentTimbreSearchTool *m_SearchTool;
	};

//=============================================================================
    class HarmonicInstrumentTimbreSearchTool: public Search
	{
	    friend class HarmonicInstrumentTimbreSearchInterface;
	public:
	    HarmonicInstrumentTimbreSearchTool();
	    HarmonicInstrumentTimbreSearchTool(HarmonicInstrumentTimbreDescriptorInterfaceABC
				       *aQueryDescriptorInterface);

	    virtual const UUID& GetObjectID(void);
	    virtual const char *GetName(void);

	    virtual HarmonicInstrumentTimbreSearchInterfaceABC *GetInterface(void);

	    virtual bool SupportsPush(void);
	    virtual bool SupportsPull(void);

	    virtual int SetRefDescriptorInterface
		(HarmonicInstrumentTimbreDescriptorInterfaceABC
		 *aHarmonicInstrumentTimbreDescriptorInterface);
	    virtual HarmonicInstrumentTimbreDescriptorInterfaceABC*
		GetQueryDescriptorInterface(void);
	    virtual int SetQueryDescriptorInterface
		(HarmonicInstrumentTimbreDescriptorInterfaceABC
		 *aHarmonicInstrumentTimbreDescriptorInterface);

	    virtual double GetDistance(void);

	    static const UUID myID;
	    static const char * myName;
	private:
	    virtual ~HarmonicInstrumentTimbreSearchTool();

	    HarmonicInstrumentTimbreSearchInterface m_Interface;
	    HarmonicInstrumentTimbreDescriptorInterfaceABC *m_RefDescriptorInterface;
	    HarmonicInstrumentTimbreDescriptorInterfaceABC *m_QueryDescriptorInterface;
#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
	    SubHarmonicInstrumentTimbreASearchInterfaceABC *m_SubHarmonicInstrumentTimbreASearch;
	    SubHarmonicInstrumentTimbreBSearchInterfaceABC *m_SubHarmonicInstrumentTimbreBSearch;
#endif /* __HasSubTypes*/

	    // Weighting coefficient
	    double m_LogAttackTimeWeight;
	    double m_HarmonicSpectralCentroidWeight;
	    double m_HarmonicSpectralDeviationWeight;
	    double m_HarmonicSpectralSpreadWeight;
	    double m_HarmonicSpectralVariationWeight;

	    double GetLogAttackTimeWeight(void);
	    void SetLogAttackTimeWeight(double);
	    double GetHarmonicSpectralCentroidWeight(void);
	    void SetHarmonicSpectralCentroidWeight(double);
	    double GetHarmonicSpectralDeviationWeight(void);
	    void SetHarmonicSpectralDeviationWeight(double);
	    double GetHarmonicSpectralSpreadWeight(void);
	    void SetHarmonicSpectralSpreadWeight(double);
	    double GetHarmonicSpectralVariationWeight(void);
	    void SetHarmonicSpectralVariationWeight(double);

	    double GetPartDistanceLogAttackTime(void);
	    double GetPartDistanceHarmonicSpectralCentroid(void);
	    double GetPartDistanceHarmonicSpectralDeviation(void);
	    double GetPartDistanceHarmonicSpectralSpread(void);
  
  
	};
}
#endif //__HARMONIC_INSTRUMENT_TIMBRE_SEARCH_H__
