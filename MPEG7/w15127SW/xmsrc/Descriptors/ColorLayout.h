///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// NEC Corp., Akio Yamada, Eiji Kasutani. 
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
// Applicable File Name:  ColorLayout.h
#ifndef __COLOR_LAYOUT_H__
#define __COLOR_LAYOUT_H__

/*
#define DEBUG_FOR_CL_DDL
*/
namespace XM
{

//=============================================================================
class ColorLayoutDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	ColorLayoutDescriptorInterfaceABC();
	virtual ~ColorLayoutDescriptorInterfaceABC() {};

	// start modification for version 3.0
	virtual void addref()=0;
	virtual void release()=0;
	virtual void SetValuesFromParameterFile(void)=0;
	// end modification for version 3.0

	virtual int GetNumberOfYCoeff()=0;
	virtual int GetNumberOfCCoeff()=0;
	virtual int *GetYCoeff()=0;
	virtual int *GetCbCoeff()=0;
	virtual int *GetCrCoeff()=0;
	virtual void SetNumberOfYCoeff(int NumberOfYCoeff)=0;
	virtual void SetNumberOfCCoeff(int NumberOfCCoeff)=0;
	virtual void SetYCoeff(int *YCoeff)=0;
	virtual void SetCbCoeff(int *CbCoeff)=0;
	virtual void SetCrCoeff(int *CrCoeff)=0;
#ifdef DEBUG_FOR_CL_DDL
	virtual void Print(FILE *fp)=0;
	virtual bool IsEqual(int num_y, int num_c, int *y_coeff, int *cb_coeff, int *cr_coeff)=0;
#endif
	static const UUID myID;
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *ParentDescription) = 0;	
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *DescriptorRoot) = 0;
};

    
class ColorLayoutDescriptor;

//=============================================================================
class ColorLayoutDescriptorInterface: public ColorLayoutDescriptorInterfaceABC
{
public:
	ColorLayoutDescriptorInterface(ColorLayoutDescriptor *aColorLayout);
	virtual ~ColorLayoutDescriptorInterface();
	
	// start modification for version 3.0
	virtual void addref();
	virtual void release();
	virtual void  SetValuesFromParameterFile(void);
	// end modification for version 3.0

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);
	virtual int GetNumberOfYCoeff();
	virtual int GetNumberOfCCoeff();
	virtual int *GetYCoeff();
	virtual int *GetCbCoeff();
	virtual int *GetCrCoeff();
	virtual void SetNumberOfYCoeff(int NumberOfYCoeff);
	virtual void SetNumberOfCCoeff(int NumberOfCCoeff);
	virtual void SetYCoeff(int *YCoeff);
	virtual void SetCbCoeff(int *CbCoeff);
	virtual void SetCrCoeff(int *CrCoeff);
#ifdef DEBUG_FOR_CL_DDL
	virtual void Print(FILE *fp);
	virtual bool IsEqual(int num_y, int num_c, int *y_coeff, int *cb_coeff, int *cr_coeff);
#endif
	static const UUID myID;
	static const char *myName;

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *DescriptorRoot);

private:
	ColorLayoutDescriptor *m_ColorLayoutDescriptor;
};

//=============================================================================
class ColorLayoutDescriptor: public Descriptor
{
// start modification for version 3.0
friend class ColorLayoutDescriptorInterface;
// end modification for version 3.0
public:
	ColorLayoutDescriptor();
	// virtual ~ColorLayoutDescriptor();


	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	// start modification for version 3.0
        virtual void addref();
        virtual void release();
	// end modification for version 3.0

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
        virtual unsigned long ImportDDL(GenericDSInterfaceABC *DescriptorRoot);	//sth use Descriptor node for this
	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual ColorLayoutDescriptorInterfaceABC *GetInterface(void);

	// accessor methods
	virtual int GetNumberOfYCoeff();
	virtual int GetNumberOfCCoeff();
	virtual int *GetYCoeff();
	virtual int *GetCbCoeff();
	virtual int *GetCrCoeff();
	virtual void SetNumberOfYCoeff(int NumberOfYCoeff);
	virtual void SetNumberOfCCoeff(int NumberOfCCoeff);
	virtual void SetYCoeff(int *YCoeff);
	virtual void SetCbCoeff(int *CbCoeff);
	virtual void SetCrCoeff(int *CrCoeff);
#ifdef DEBUG_FOR_CL_DDL
	virtual void Print(FILE *fp);
	virtual bool IsEqual(int num_y, int num_c, int *y_coeff, int *cb_coeff, int *cr_coeff);
#endif
private:
	// start modification for version 3.0
        virtual ~ColorLayoutDescriptor();
        unsigned long m_refcount;
	// end modification for version 3.0
	ColorLayoutDescriptorInterface m_Interface;

	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// descriptor data
	int m_NumberOfYCoeff, m_NumberOfCCoeff;
	int *m_y_coeff, *m_cb_coeff, *m_cr_coeff;
};

};

#endif //__COLOR_LAYOUT_H__
