///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Institut National des Telecommunications - ARTEMIS Project Unit, Titus Zaharia
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
// Applicable File Name:  3DShapeSpectrum.h

namespace XM
{
 
//=============================================================================
class ShapeSpectrumDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	ShapeSpectrumDescriptorInterfaceABC();
	virtual ~ShapeSpectrumDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;

	virtual int SetNoOfBins(int NoOfBins) = 0;
	virtual int GetNoOfBins() = 0;
	virtual int SetNoOfBits(int NoOfBits) = 0;
	virtual int GetNoOfBits() = 0;
	virtual void  SetValuesFromParameterFile(void) = 0;
	
	virtual unsigned long SetShapeSpectrumSize(unsigned long newSize) = 0;
	virtual unsigned long GetShapeSpectrumSize(void) = 0;
	virtual unsigned long *GetShapeSpectrum(void) = 0;
	virtual int SetElement(unsigned long index, unsigned long value) = 0;
	virtual int GetElement(unsigned long index) = 0;
	virtual int SetPlanarElement(unsigned long value) = 0;
	virtual int GetPlanarElement(void) = 0;
	virtual int SetSingularElement(unsigned long value) = 0;
	virtual int GetSingularElement(void) = 0;

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;

	
	static const UUID myID;

};

class ShapeSpectrumDescriptor;

//=============================================================================
class ShapeSpectrumDescriptorInterface: public ShapeSpectrumDescriptorInterfaceABC
{
public:
	ShapeSpectrumDescriptorInterface(ShapeSpectrumDescriptor *aHisto);
	virtual ~ShapeSpectrumDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual int SetNoOfBins(int NoOfBins);
	virtual int GetNoOfBins();
	virtual int SetNoOfBits(int NoOfBits);
	virtual int GetNoOfBits();	
	virtual void  SetValuesFromParameterFile(void);

	virtual unsigned long SetShapeSpectrumSize(unsigned long newSize);
	virtual unsigned long GetShapeSpectrumSize(void);
	virtual unsigned long *GetShapeSpectrum(void);
	virtual int SetElement(unsigned long index, unsigned long value);
	virtual int GetElement(unsigned long index);
	virtual int SetPlanarElement(unsigned long value);
	virtual int GetPlanarElement(void);
	virtual int SetSingularElement(unsigned long value);
	virtual int GetSingularElement(void);

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);
	static const UUID myID;
	static const char *myName;


private:
	ShapeSpectrumDescriptor *m_ShapeSpectrumDescriptor;
};

//=============================================================================
class ShapeSpectrumDescriptor: public Descriptor
{
friend class ShapeSpectrumDescriptorInterface;
public:
	ShapeSpectrumDescriptor();

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();
	
	//	virtual long ShapeSpectrumDescriptor::GetShapeSpectrumContents(void);
	//	virtual void ShapeSpectrumDescriptor::SetShapeSpectrumContents(const long val);

	virtual int SetNoOfBins(int NoOfBins);
	virtual int GetNoOfBins();
	virtual int SetNoOfBits(int NoOfBits);
	virtual int GetNoOfBits();

	virtual unsigned long SetShapeSpectrumSize(unsigned long newSize);
	virtual unsigned long GetShapeSpectrumSize(void);
	virtual unsigned long * GetShapeSpectrum(void);
	virtual int SetElement(unsigned long index, unsigned long value);
	virtual int GetElement(unsigned long index);
	virtual int SetPlanarElement(unsigned long value);
	virtual int GetPlanarElement(void);
	virtual int SetSingularElement(unsigned long value);
	virtual int GetSingularElement(void);

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual ShapeSpectrumDescriptorInterfaceABC *GetInterface(void);

	
private:
	virtual ~ShapeSpectrumDescriptor();
	unsigned long m_refcount;

	ShapeSpectrumDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	unsigned long m_ShapeSpectrumSize;
	unsigned long m_ShapeSpectrumNoOfBits;
	unsigned long *m_ShapeSpectrum;
	unsigned long m_PlanarSurfaces;
	unsigned long m_SingularSurfaces;

};

};

