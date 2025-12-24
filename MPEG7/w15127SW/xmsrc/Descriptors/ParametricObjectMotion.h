///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S. Herrmann
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// (contributing organizations names)
//
// Updated:
// Institut National des Telecommunications, ARTEMIS Project Unit,
//      Marius PREDA, Titus ZAHARIA (18/01/2000)
//
// Updated 12/13/00:
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - update according to LaBaule CD-text (coordinates, duration)
//
// Updated 06/14/01:
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - XML-IO
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
// Applicable File Name:  ParametricObjectMotion.h
#ifndef __ParametricObjectMotion__
#define __ParametricObjectMotion__


namespace XM
{

//=============================================================================
class ParametricObjectMotionDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	ParametricObjectMotionDescriptorInterfaceABC();
	virtual ~ParametricObjectMotionDescriptorInterfaceABC() {}

	virtual void addref() = 0;
	virtual void release() = 0;

	virtual void  SetValuesFromParameterFile(void) = 0;

	virtual unsigned long GetCoordFlag(void) = 0;	// Aljoscha 12/13/00
	virtual unsigned long SetCoordFlag(unsigned long value) = 0;
				// Aljoscha 12/13/00

	virtual unsigned long 		GetCoordRef(void) = 0;											// Aljoscha 12/13/00
  virtual unsigned long 	SetCoordRef(unsigned long value) = 0;					// Aljoscha 12/13/00

	virtual unsigned long 		GetSpatialReference(void) = 0;							// Aljoscha 12/13/00
  virtual unsigned long 	SetSpatialReference(unsigned long value) = 0;	// Aljoscha 12/13/00

	virtual int 		GetModelCode(void) = 0;
  virtual unsigned long 	SetModelCode(int value) = 0;

	virtual int 		GetDuration(void) = 0;																// Aljoscha 12/13/00
  virtual unsigned long 	SetDuration(int value) = 0;										// Aljoscha 12/13/00

  virtual double 		*GetMotionParameters() = 0;
  virtual unsigned long	GetMotionParameterValue(int index,
							double& value) = 0;
  virtual unsigned long 	SetMotionParameterValue(int index,
							double value) = 0;

  virtual double 		GetXOrigin(void) = 0;
  virtual unsigned long 	SetXOrigin(double value) = 0;

  virtual double 		GetYOrigin(void) = 0;
  virtual unsigned long 	SetYOrigin(double value) = 0;

  static const UUID myID;
  virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
};

class ParametricObjectMotionDescriptor;

//=============================================================================
class ParametricObjectMotionDescriptorInterface: public ParametricObjectMotionDescriptorInterfaceABC
{
public:
	ParametricObjectMotionDescriptorInterface(
			      ParametricObjectMotionDescriptor
			      *aParametricObjectMotion);
	virtual ~ParametricObjectMotionDescriptorInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual void  SetValuesFromParameterFile(void);

	virtual unsigned long 		GetCoordFlag(void);											// Aljoscha 12/13/00
  virtual unsigned long 	SetCoordFlag(unsigned long value);				// Aljoscha 12/13/00

	virtual unsigned long 		GetCoordRef(void);											// Aljoscha 12/13/00
  virtual unsigned long 	SetCoordRef(unsigned long value);					// Aljoscha 12/13/00

	virtual unsigned long 		GetSpatialReference(void);							// Aljoscha 12/13/00
  virtual unsigned long 	SetSpatialReference(unsigned long value);	// Aljoscha 12/13/00

	virtual int 		GetModelCode(void);
  virtual unsigned long 	SetModelCode(int value);

	virtual int 		GetDuration(void);																// Aljoscha 12/13/00
  virtual unsigned long 	SetDuration(int value);										// Aljoscha 12/13/00

  virtual double 		*GetMotionParameters();
  virtual unsigned long	GetMotionParameterValue(int index,
							double& value);
  virtual unsigned long 	SetMotionParameterValue(int index,
							double value);

  virtual double 		GetXOrigin(void);
  virtual unsigned long 	SetXOrigin(double value);

  virtual double 		GetYOrigin(void);
  virtual unsigned long 	SetYOrigin(double value);

	static const UUID myID;
	static const char *myName;


	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

private:
	ParametricObjectMotionDescriptor *m_ParametricObjectMotionDescriptor;	
};

//=============================================================================
class ParametricObjectMotionDescriptor: public Descriptor
{
friend class  ParametricObjectMotionDescriptorInterface;
public:
	ParametricObjectMotionDescriptor();


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

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual ParametricObjectMotionDescriptorInterfaceABC 
	  *GetInterface(void);

	// accessor methods
	virtual unsigned long 		GetCoordFlag(void);											// Aljoscha 12/13/00
  virtual unsigned long 	SetCoordFlag(unsigned long value);				// Aljoscha 12/13/00

	virtual unsigned long 		GetCoordRef(void);											// Aljoscha 12/13/00
  virtual unsigned long 	SetCoordRef(unsigned long value);					// Aljoscha 12/13/00

	virtual unsigned long 		GetSpatialReference(void);							// Aljoscha 12/13/00
  virtual unsigned long 	SetSpatialReference(unsigned long value);	// Aljoscha 12/13/00

	virtual int 		GetModelCode(void);
  virtual unsigned long 	SetModelCode(int value);

	virtual int 		GetDuration(void);																// Aljoscha 12/13/00
  virtual unsigned long 	SetDuration(int value);										// Aljoscha 12/13/00

  virtual double 		*GetMotionParameters();
  virtual unsigned long	GetMotionParameterValue(int index,
							double& value);
  virtual unsigned long 	SetMotionParameterValue(int index,
							double value);

  virtual double 		GetXOrigin(void);
  virtual unsigned long 	SetXOrigin(double value);

  virtual double 		GetYOrigin(void);
  virtual unsigned long 	SetYOrigin(double value);

private:
	virtual ~ParametricObjectMotionDescriptor();
	unsigned long m_refcount;

	ParametricObjectMotionDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// descriptor data
	unsigned long m_CoordFlag;												// Aljoscha 12/13/00
	unsigned long m_CoordRef;													// Aljoscha 12/13/00
	unsigned long m_SpatialReference;									// Aljoscha 12/13/00
  int m_ModelCode;
	int m_Duration;																		// Aljoscha 12/13/00
  double *m_MotionParameters;
  double m_XOrigin;
  double m_YOrigin;
  int m_NoOfMotionParameters;
};

};

#endif //__ParametricObjectMotion__

