///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// University of California Santa Barbara, Kapil Chhabra, Yining Deng. 
// Mitsubishi Electric ITE-VIL, Leszek Cieplinski. 
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  DominantColor.h
#ifndef __DOMINANTCOLOR_H__
#define __DOMINANTCOLOR_H__


namespace XM
{

class ColorQuantizerDescriptorInterface;
class ColorSpaceDescriptorInterface;

typedef struct {
  int weight;
  int centroid[3];
  int variance[3];
} dc_t;

//=============================================================================
class DominantColorDescriptorInterfaceABC: public I_InterfaceABC
{
public:

  DominantColorDescriptorInterfaceABC();
  virtual ~DominantColorDescriptorInterfaceABC() {}

  virtual void addref()=0;
  virtual void release()=0;

  virtual void SetValuesFromParameterFile(void) = 0;

  virtual ColorQuantizerDescriptorInterfaceABC
    *GetColorQuantizerDescriptorInterface(void) = 0;
  virtual unsigned long SetColorQuantizerDescriptorInterface(
		ColorQuantizerDescriptorInterfaceABC *) = 0;
  virtual ColorSpaceDescriptorInterfaceABC
    *GetColorSpaceDescriptorInterface(void) = 0;
  virtual unsigned long SetColorSpaceDescriptorInterface(
		ColorSpaceDescriptorInterfaceABC *) = 0;

  virtual int GetDominantColorsNumber()=0;
  virtual bool GetColorSpacePresent()=0;
  virtual bool GetColorQuantizationPresent()=0;
  virtual bool GetVariancePresent()=0;
  virtual int GetSpatialCoherency()=0;
  virtual dc_t *GetDominantColors()=0;
  virtual void SetDominantColorsNumber(int dc_number)=0;
  virtual void SetColorSpacePresent(bool colspcpres)=0;
  virtual void SetColorQuantizationPresent(bool colquantpres)=0;
  virtual void SetVariancePresent(bool varpres)=0;
  virtual void SetSpatialCoherency(int sc)=0;
  virtual void SetDominantColors(int *percents, int **colors, int **variances)=0;
  virtual void Print()=0;
  virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
  static const UUID myID;
};

    
class DominantColorDescriptor;

//=============================================================================
class DominantColorDescriptorInterface: public DominantColorDescriptorInterfaceABC
{

public:

  DominantColorDescriptorInterface(DominantColorDescriptor *aDomCol);
  virtual ~DominantColorDescriptorInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  virtual void SetValuesFromParameterFile(void);

  virtual ColorQuantizerDescriptorInterfaceABC
    *GetColorQuantizerDescriptorInterface(void);
  virtual unsigned long SetColorQuantizerDescriptorInterface(
		ColorQuantizerDescriptorInterfaceABC *);	
  virtual ColorSpaceDescriptorInterfaceABC
    *GetColorSpaceDescriptorInterface(void);
  virtual unsigned long SetColorSpaceDescriptorInterface(
		ColorSpaceDescriptorInterfaceABC *);	

  virtual int GetDominantColorsNumber();
  virtual bool GetColorSpacePresent();
  virtual bool GetColorQuantizationPresent();
  virtual bool GetVariancePresent();
  virtual int GetSpatialCoherency();
  virtual dc_t *GetDominantColors();
  virtual void SetDominantColorsNumber(int dc_number);
  virtual void SetColorSpacePresent(bool colspcpres);
  virtual void SetColorQuantizationPresent(bool colquantpres);
  virtual void SetVariancePresent(bool varpres);
  virtual void SetSpatialCoherency(int sc);
  virtual void SetDominantColors(int *percents, int **colors,
                                 int **variances);
  virtual void Print();
  virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

  static const UUID myID;
  static const char *myName;

private:

  DominantColorDescriptor *m_DominantColorDescriptor;	

};

//=============================================================================
class DominantColorDescriptor: public Descriptor
{
  friend class DominantColorDescriptorInterface;

public:

  DominantColorDescriptor();
  DominantColorDescriptor(
		ColorSpaceDescriptorInterfaceABC *colorspace,
		ColorQuantizerDescriptorInterfaceABC *quantizer);

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

  virtual DominantColorDescriptorInterfaceABC *GetInterface(void);

  // accessor methods

  virtual ColorQuantizerDescriptorInterfaceABC
    *GetColorQuantizerDescriptorInterface(void);
  virtual unsigned long SetColorQuantizerDescriptorInterface(
		ColorQuantizerDescriptorInterfaceABC *);	
  virtual ColorSpaceDescriptorInterfaceABC
    *GetColorSpaceDescriptorInterface(void);
  virtual unsigned long SetColorSpaceDescriptorInterface(
		ColorSpaceDescriptorInterfaceABC *);	

  virtual int GetDominantColorsNumber();
  virtual bool GetColorSpacePresent() {return m_ColorSpacePresent;};
  virtual bool GetColorQuantizationPresent() {return m_ColorQuantizationPresent;};
  virtual bool GetVariancePresent() {return m_VariancePresent;};
  virtual int GetSpatialCoherency();
  virtual dc_t *GetDominantColors();

  virtual void SetDominantColorsNumber(int dc_number);
  virtual void SetColorSpacePresent(bool colspcpres)
    {m_ColorSpacePresent=colspcpres;};
  virtual void SetColorQuantizationPresent(bool colquantpres)
    {m_ColorQuantizationPresent=colquantpres;};
  virtual void SetVariancePresent(bool varpres) {m_VariancePresent=varpres;};
  virtual void SetSpatialCoherency(int sc);
  virtual void SetDominantColors(int *percents, int **colors,
                                 int **variances);
  virtual void Print();
  virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

  // LC: maximum size of DC descriptor
  //static const int DCNMAX=8;
  enum{DCNMAX=8}; //RJOC
  enum{BINNUMDEF=32}; //RJOC

private:

  virtual ~DominantColorDescriptor();

  unsigned long m_refcount;

  DominantColorDescriptorInterface m_Interface;

  const bool m_isProprietary;
  static const char *valName;
  static const UUID valID;

  // descriptor data
  int m_DominantColorsNumber;
  bool m_ColorSpacePresent;
  bool m_ColorQuantizationPresent;
  bool m_VariancePresent;
  int m_SpatialCoherency;
  dc_t m_dc[DCNMAX]; // let us keep the array size at maximum to simplify
  ColorSpaceDescriptorInterfaceABC *m_ColorSpaceInterface;
  ColorQuantizerDescriptorInterfaceABC *m_QuantizerInterface;

};


};

#endif //__DOMINANTCOLOR_H__
