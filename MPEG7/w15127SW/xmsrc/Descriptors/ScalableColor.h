///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Intel Corporation
// TU-Munich, S.Herrmann
// Heinrich-Hertz-Institute (HHI), Bela Makai, Karsten Mueller
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
// Applicable File Name:  ScalColorHist.h
#ifndef __SCAL_ColorHIST__
#define __SCAL_ColorHIST__


namespace XM
{

	const static int scalableColorQuantValues[256][3] = 
	{
		{217,9,255},{-71,9,255},{-27,8,127},{-54,9,255},{-8,7,63},{-14,7,63},{-22,7,63},{-29,8,127},
		{-6,6,31},{-13,7,63},{-11,6,31},{-22,7,63},{-9,7,63},{-14,7,63},{-19,7,63},{-22,7,63},
		{0,4,7},{-1,5,15},{0,3,3},{-2,6,31},{1,5,15},{-5,6,31},{0,5,15},{0,7,63},
		{2,5,15},{-2,6,31},{-2,5,15},{0,7,63},{3,5,15},{-5,6,31},{-1,6,31},{4,7,63},
		{0,3,3},{0,3,3},{0,3,3},{-1,5,15},{0,3,3},{0,3,3},{-1,5,15},{-2,5,15},
		{-1,5,15},{-1,4,7},{-1,5,15},{-3,5,15},{-1,5,15},{-2,5,15},{-4,5,15},{-5,5,15},
		{-1,5,15},{0,3,3},{-2,5,15},{-2,5,15},{-2,5,15},{-3,5,15},{-3,5,15},{0,5,15},
		{0,5,15},{0,5,15},{0,5,15},{2,5,15},{-1,5,15},{0,5,15},{3,6,31},{3,5,15},
		{0,2,1},{0,2,1},{0,3,3},{0,4,7},{0,2,1},{0,2,1},{0,3,3},{-1,4,7},
		{-1,4,7},{-1,4,7},{-2,5,15},{-1,5,15},{-2,5,15},{-2,5,15},{-2,5,15},{-1,5,15},
		{0,3,3},{0,2,1},{0,3,3},{-1,4,7},{0,2,1},{0,3,3},{-1,4,7},{-1,5,15},
		{-2,5,15},{-1,4,7},{-2,5,15},{-1,5,15},{-3,5,15},{-3,5,15},{-2,5,15},{0,5,15},
		{0,3,3},{0,3,3},{0,3,3},{-1,4,7},{0,3,3},{0,3,3},{-2,5,15},{-2,5,15},
		{-2,5,15},{-2,4,7},{-2,5,15},{-1,5,15},{-3,5,15},{-3,5,15},{-1,5,15},{0,5,15},
		{1,4,7},{0,3,3},{0,4,7},{-1,4,7},{0,3,3},{0,4,7},{-1,4,7},{0,4,7},
		{-1,4,7},{-1,3,3},{-1,4,7},{0,4,7},{-1,5,15},{0,5,15},{1,5,15},{-1,5,15},
		{0,2,1},{0,2,1},{0,3,3},{0,3,3},{0,2,1},{0,2,1},{0,3,3},{0,3,3},
		{0,2,1},{0,2,1},{0,3,3},{0,4,7},{0,2,1},{0,2,1},{0,3,3},{0,3,3},
		{0,3,3},{0,2,1},{0,3,3},{1,4,7},{0,2,1},{0,3,3},{-1,4,7},{1,4,7},
		{0,3,3},{0,3,3},{0,3,3},{0,4,7},{0,3,3},{0,3,3},{-1,4,7},{0,4,7},
		{0,3,3},{0,2,1},{0,3,3},{0,3,3},{0,2,1},{0,2,1},{0,3,3},{0,3,3},
		{0,3,3},{0,2,1},{0,3,3},{1,4,7},{0,2,1},{0,3,3},{0,4,7},{1,4,7},
		{0,3,3},{0,2,1},{0,3,3},{1,5,15},{0,3,3},{0,3,3},{-1,5,15},{2,5,15},
		{0,3,3},{0,3,3},{0,3,3},{0,4,7},{0,3,3},{0,3,3},{-1,4,7},{1,5,15},
		{0,3,3},{0,2,1},{0,3,3},{0,3,3},{0,2,1},{0,3,3},{0,4,7},{0,4,7},
		{0,3,3},{0,2,1},{0,3,3},{1,4,7},{0,3,3},{0,3,3},{-1,5,15},{1,5,15},
		{0,3,3},{0,2,1},{-1,3,3},{1,5,15},{0,3,3},{-1,4,7},{-1,5,15},{2,5,15},
		{0,3,3},{0,3,3},{0,3,3},{0,4,7},{0,3,3},{-1,3,3},{0,4,7},{1,4,7},
		{1,3,3},{0,2,1},{-1,3,3},{0,3,3},{0,3,3},{0,3,3},{0,3,3},{1,4,7},
		{0,3,3},{0,2,1},{-1,3,3},{0,4,7},{0,3,3},{0,3,3},{0,4,7},{1,4,7},
		{0,3,3},{0,2,1},{0,3,3},{0,4,7},{0,3,3},{-1,3,3},{0,4,7},{1,4,7},
		{0,3,3},{0,3,3},{0,3,3},{0,3,3},{0,3,3},{-1,3,3},{0,3,3},{-1,4,7}
	};

//=============================================================================
class ScalableColorDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	ScalableColorDescriptorInterfaceABC();
	virtual ~ScalableColorDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;

	virtual void  SetValuesFromParameterFile(void) = 0;


	virtual unsigned long GetNumberOfCoefficients(void) = 0;
	virtual unsigned long SetNumberOfCoefficients(unsigned long) = 0;	
	virtual unsigned long *GetScalableHistogram(void)=0;
	virtual unsigned long GetCoefficient(unsigned long index) = 0;
	virtual unsigned long SetCoefficient(unsigned long index, int value) = 0;
	virtual unsigned long SetCoeffValue(unsigned long index, int value) = 0;
	virtual unsigned long GetCoeffSign(unsigned long index) = 0;
	virtual unsigned long SetCoeffSign(unsigned long index, int value) = 0;
	virtual unsigned long GetNumberOfBitplanesDiscarded(void) = 0;
	virtual unsigned long SetNumberOfBitplanesDiscarded(unsigned long) = 0;
		
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
	virtual void  SetGoFGoP(bool set) = 0;

	static const UUID myID;

};

class ScalableColorDescriptor;

//=============================================================================
class ScalableColorDescriptorInterface: public ScalableColorDescriptorInterfaceABC
{
public:
	ScalableColorDescriptorInterface(ScalableColorDescriptor *aHisto);
	virtual ~ScalableColorDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual void  SetValuesFromParameterFile(void);


	virtual unsigned long *GetScalableHistogram(void);
	virtual unsigned long GetNumberOfCoefficients(void);
	virtual unsigned long SetNumberOfCoefficients(unsigned long);	
	virtual unsigned long GetCoefficient(unsigned long index);
	virtual unsigned long SetCoefficient(unsigned long index, int value);
	virtual unsigned long SetCoeffValue(unsigned long index, int value);
	virtual unsigned long GetCoeffSign(unsigned long index);
	virtual unsigned long SetCoeffSign(unsigned long index, int value);
	virtual unsigned long GetNumberOfBitplanesDiscarded(void);
	virtual unsigned long SetNumberOfBitplanesDiscarded(unsigned long);

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);
	virtual void SetGoFGoP(bool set);

	static const UUID myID;
	static const char *myName;


private:
	ScalableColorDescriptor *m_ScalableColorDescriptor;
};

//=============================================================================
class ScalableColorDescriptor: public Descriptor
{
friend class ScalableColorDescriptorInterface;
public:
	ScalableColorDescriptor();
	//	ScalableColorDescriptor(const ScalableColorDescriptor&);

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);
	virtual void SetGoFGoP(bool set);
	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual ScalableColorDescriptorInterfaceABC *GetInterface(void);

	// Accesssor methods

	virtual unsigned long *GetScalableHistogram(void);
	virtual unsigned long GetNumberOfCoefficients(void);
	virtual unsigned long SetNumberOfCoefficients(unsigned long);	
	virtual unsigned long GetCoefficient(unsigned long index);
	virtual unsigned long SetCoefficient(unsigned long index, int value);
	virtual unsigned long SetCoeffValue(unsigned long index, int value);
	virtual unsigned long GetCoeffSign(unsigned long index);
	virtual unsigned long SetCoeffSign(unsigned long index, int value);
	virtual unsigned long GetNumberOfBitplanesDiscarded();
	virtual unsigned long SetNumberOfBitplanesDiscarded(unsigned long);



private:
	virtual ~ScalableColorDescriptor();
	
	unsigned long m_refcount;

	ScalableColorDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// actual values:

	unsigned long *m_ScalableHistogram;  /* An array of m_HistoSize
				Coefficients representing
				the frequency of each quantized
				value in the  source media */
	unsigned long *m_Sign; // An array of signs of m_HistoSize Coefficients
	unsigned long m_NumberOfCoefficients;   /* The size of the ScalableHistogram */
	unsigned long m_NumberOfBitplanesDiscarded; /* A normalization factor to
				  be applied to the entries of
				  the ScalableHistogram before use */
	bool m_isGoFGop;	// Indicates if the descriptor is part of GoFGoP descriptor.
};

};

#endif //__SCAL_ColorHIST__
