//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Nam Kyu Kim, Dong-Gye Sim, Hae-Kwang Kim - Hyundai
// Ajay Divakaran - Mitsubishi
// Xinding Sun, B. S. Manjunath -UCSB
// Sharp corporation, Yasuaki Tokumo, Shuichi Watanabe.
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
// Applicable File Name:  MotionActivity.h

#ifndef __MotionActivity__
#define __MotionActivity__

#ifndef NUM_Nsr_Nmr_Nlr
#define	NUM_Nsr_Nmr_Nlr			3	// Number of Spatial Parameter Number
#define NUM_N0_N1_N2_N3_N4		5	// Number of Temporal Parameter Number
#endif

#ifndef MAX_DIVISION_BLOCKS // for SLMA
#define MAX_DIVISION_BLOCKS	(16*16)
#endif

namespace XM
{

//=============================================================================
class MotionActivityDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	MotionActivityDescriptorInterfaceABC();
	virtual ~MotionActivityDescriptorInterfaceABC() {}
	virtual void addref()=0;
	virtual void release()=0;


	virtual int GetStartFrameNum(void) = 0;
	virtual unsigned long SetStartFrameNum(int) = 0;
	virtual int GetEndFrameNum(void) = 0;
	virtual unsigned long SetEndFrameNum(int) = 0;
	virtual int GetIntensity(void) = 0;
	virtual unsigned long SetIntensity(int) = 0; 
	virtual bool GetDirectionFlag(void) = 0;
	virtual unsigned long SetDirectionFlag(bool) = 0;
	virtual bool GetSpatialDistributionFlag(void) = 0;
	virtual unsigned long SetSpatialDistributionFlag(bool) = 0;
	virtual bool GetSpatialLocalizedDistributionFlag(void) = 0; // for SLMA
	virtual unsigned long SetSpatialLocalizedDistributionFlag(bool) = 0; // for SLMA
	virtual bool GetTemporalDistributionFlag(void) = 0;
	virtual unsigned long SetTemporalDistributionFlag(bool) = 0;
	virtual int GetDominantDirection(void) = 0;
	virtual unsigned long SetDominantDirection(int) = 0;
	virtual int GetSpatialParameters(unsigned long index) = 0;
	virtual unsigned long SetSpatialParameters( unsigned long index, int value ) = 0;
	//2013/03/04/*
	virtual int GetSpaLocNumber(void) = 0; // for SLMA
	virtual unsigned long SetSpaLocNumber(int) = 0; // for SLMA
	// */
	virtual int GetSpatialLocalizedParameter(unsigned long index) = 0; // for SLMA
	virtual unsigned long SetSpatialLocalizedParameter(unsigned long index, int value) = 0; // for SLMA
	virtual int GetTemporalParameters(unsigned long index) = 0;
	virtual unsigned long SetTemporalParameters( unsigned long index, int value ) = 0;

	static const UUID myID;
	virtual unsigned long
	  ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
	virtual unsigned long
	  ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
};

class MotionActivityDescriptor;

//=============================================================================
class MotionActivityDescriptorInterface: public MotionActivityDescriptorInterfaceABC
{
public:
	MotionActivityDescriptorInterface(MotionActivityDescriptor *aTraj);
	virtual ~MotionActivityDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual int GetStartFrameNum(void);
	virtual unsigned long SetStartFrameNum(int);
	virtual int GetEndFrameNum(void);
	virtual unsigned long SetEndFrameNum(int);
	virtual int GetIntensity(void);
	virtual unsigned long SetIntensity(int);
	virtual bool GetDirectionFlag(void);
	virtual unsigned long SetDirectionFlag(bool);
	virtual bool GetSpatialDistributionFlag(void);
	virtual unsigned long SetSpatialDistributionFlag(bool);
	virtual bool GetSpatialLocalizedDistributionFlag(void); // for SLMA
	virtual unsigned long SetSpatialLocalizedDistributionFlag(bool); // for SLMA
	virtual bool GetTemporalDistributionFlag(void);
	virtual unsigned long SetTemporalDistributionFlag(bool);
	virtual int GetDominantDirection(void);
	virtual unsigned long SetDominantDirection(int);
	virtual int GetSpatialParameters(unsigned long index);
	virtual unsigned long SetSpatialParameters( unsigned long index, int value );
	///*
	virtual int GetSpaLocNumber(void); // for SLMA
	virtual unsigned long SetSpaLocNumber(int); // for SLMA
	//*/
	virtual int GetSpatialLocalizedParameter(unsigned long index); // for SLMA
	virtual unsigned long SetSpatialLocalizedParameter(unsigned long index, int value); // for SLMA
	virtual int GetTemporalParameters(unsigned long index);
	virtual unsigned long SetTemporalParameters( unsigned long index, int value );
	virtual unsigned long
	  ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);
	static const UUID myID;
	static const char *myName;


private:
	MotionActivityDescriptor *m_MotionActivityDescriptor;
};

//=============================================================================
class MotionActivityDescriptor: public Descriptor
{
friend class MotionActivityDescriptorInterface;
public:
	MotionActivityDescriptor();

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual MotionActivityDescriptorInterfaceABC *GetInterface(void);

	virtual int GetStartFrameNum(void);
	virtual unsigned long SetStartFrameNum(int);
	virtual int GetEndFrameNum(void);
	virtual unsigned long SetEndFrameNum(int);
	virtual int GetIntensity(void);
	virtual unsigned long SetIntensity(int);
	virtual bool GetDirectionFlag(void);
	virtual unsigned long SetDirectionFlag(bool);
	virtual bool GetSpatialDistributionFlag(void);
	virtual unsigned long SetSpatialDistributionFlag(bool);
	virtual bool GetSpatialLocalizedDistributionFlag(void); // for SLMA
	virtual unsigned long SetSpatialLocalizedDistributionFlag(bool); // for SLMA
	virtual bool GetTemporalDistributionFlag(void);
	virtual unsigned long SetTemporalDistributionFlag(bool);
	virtual int GetDominantDirection(void);
	virtual unsigned long SetDominantDirection(int);
	virtual int GetSpatialParameters(unsigned long index);
	virtual unsigned long SetSpatialParameters( unsigned long index, int value );
	
	virtual int GetSpaLocNumber(void); // for SLMA
	virtual unsigned long SetSpaLocNumber(int); // for SLMA
	
	virtual int GetSpatialLocalizedParameter(unsigned long index); // for SLMA
	virtual unsigned long SetSpatialLocalizedParameter(unsigned long index, int value); // for SLMA
	virtual int GetTemporalParameters(unsigned long index);
	virtual unsigned long SetTemporalParameters( unsigned long index, int value );
	virtual unsigned long
	  ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);
private:

	virtual ~MotionActivityDescriptor();

	unsigned long m_refcount;

	MotionActivityDescriptorInterface m_Interface;

	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// shot start end information:
	int		m_StartFrameNum;
	int		m_EndFrameNum;
	//below here, MotionActivity Desctiotor Syntax
	int		m_Intensity;
	bool	m_DirectionFlag;
	bool	m_SpatialDistributionFlag;
	bool	m_SpatialLocalizedDistributionFlag; // for SLMA
	bool	m_TemporalDistributionFlag;
	int	m_DominantDirection;
	int     m_SpatialParameters[NUM_Nsr_Nmr_Nlr];  // NUM_Nsr_Nmr_Nlr=3 (Nsr, Nmr, Nlr)
	
	int	m_SpaLocNumber; // for SLMA
	
	int	m_SpatialLocalizedParameter[MAX_DIVISION_BLOCKS]; // for SLMA
	int	m_TemporalParameters[NUM_N0_N1_N2_N3_N4]; // NUM_N0_N1_N2_N3_N4=5 (N0,N1,N2,N3,N4) 
};

};

#endif //__MotionActivity__
