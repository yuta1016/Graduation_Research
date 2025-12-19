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
// Applicable File Name:  HarmonicInstrumentTimbre.h

#ifndef __HARMONIC_INSTRUMENT_TIMBRE_H__
#define __HARMONIC_INSTRUMENT_TIMBRE_H__

#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

namespace XM
{

//=============================================================================
class HarmonicInstrumentTimbreDescriptorInterfaceABC: public I_InterfaceABC
    {
    public:
	HarmonicInstrumentTimbreDescriptorInterfaceABC();
	virtual ~HarmonicInstrumentTimbreDescriptorInterfaceABC() {};
	
	virtual void addref()=0;
	virtual void release()=0;
	
	static const UUID myID;
	
	
	virtual double GetHarmonicSpectralCentroid(void) = 0;
	virtual void SetHarmonicSpectralCentroid(double) = 0;
	virtual double GetHarmonicSpectralDeviation(void) = 0;
	virtual void SetHarmonicSpectralDeviation(double) = 0;
	virtual double GetHarmonicSpectralSpread(void) = 0;
	virtual void SetHarmonicSpectralSpread(double) = 0;
	virtual double GetHarmonicSpectralVariation(void) = 0;
	virtual void SetHarmonicSpectralVariation(double) = 0;
	virtual double GetLogAttackTime(void) = 0;
	virtual void SetLogAttackTime(double) = 0;

	virtual unsigned long
	    ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
    };

class HarmonicInstrumentTimbreDescriptor;

//=============================================================================
class HarmonicInstrumentTimbreDescriptorInterface: public HarmonicInstrumentTimbreDescriptorInterfaceABC
    {
    public:
	HarmonicInstrumentTimbreDescriptorInterface(HarmonicInstrumentTimbreDescriptor
					    *aHarmonicInstrumentTimbreDescriptor);
	virtual ~HarmonicInstrumentTimbreDescriptorInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);
	
	virtual void addref();
	virtual void release();
	
	
	virtual double GetHarmonicSpectralCentroid(void);
	virtual void SetHarmonicSpectralCentroid(double);
	virtual double GetHarmonicSpectralDeviation(void);
	virtual void SetHarmonicSpectralDeviation(double);
	virtual double GetHarmonicSpectralSpread(void);
	virtual void SetHarmonicSpectralSpread(double);
	virtual double GetHarmonicSpectralVariation(void);
	virtual void SetHarmonicSpectralVariation(double);
	virtual double GetLogAttackTime(void);
	virtual void SetLogAttackTime(double);

	virtual unsigned long
	    ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);
	
	static const UUID myID;
	static const char *myName;
    private:
	HarmonicInstrumentTimbreDescriptor *m_Descriptor;
    };
 
//=============================================================================
class HarmonicInstrumentTimbreDescriptor: public Descriptor
    {
	friend class HarmonicInstrumentTimbreDescriptorInterface;
    public:
	HarmonicInstrumentTimbreDescriptor();
	
	
	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);
	
	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	
	virtual void addref();
	virtual void release();

	
	virtual unsigned long
	    ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);
	
	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual HarmonicInstrumentTimbreDescriptorInterfaceABC *GetInterface(void);
	
	// Accessor methods
	virtual double GetHarmonicSpectralCentroid(void);
	virtual void SetHarmonicSpectralCentroid(double);
	virtual double GetHarmonicSpectralDeviation(void);
	virtual void SetHarmonicSpectralDeviation(double);
	virtual double GetHarmonicSpectralSpread(void);
	virtual void SetHarmonicSpectralSpread(double);
	virtual double GetHarmonicSpectralVariation(void);
	virtual void SetHarmonicSpectralVariation(double);
	virtual double GetLogAttackTime(void);
	virtual void SetLogAttackTime(double);

    private:
	virtual ~HarmonicInstrumentTimbreDescriptor();
	
	unsigned long m_refcount;
	
	HarmonicInstrumentTimbreDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;
	

	// actual values:
	double m_HarmonicSpectralCentroid; 
	double m_HarmonicSpectralDeviation;
	double m_HarmonicSpectralSpread;
	double m_HarmonicSpectralVariation;
	double m_LogAttackTime;
	
	
    };
}
#endif //__HARMONIC_INSTRUMENT_TIMBRE_H__

