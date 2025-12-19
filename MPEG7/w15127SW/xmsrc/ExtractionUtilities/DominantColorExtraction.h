///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// University of California Santa Barbara, Kapil Chhabra, Yining Deng
// Mitsubishi Electric ITE-VIL, Leszek Cieplinski, LG-Elite.
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
// Applicable File Name:  DominantColorExtraction.h
//
#ifndef __DOMINANTCOLOREXTRACTION_H__
#define __DOMINANTCOLOREXTRACTION_H__
#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                                      /* Naming Convention */
#include <momusys.h>
#include <intra.h>
#include "imgutil.h"
#include "Media/Media.h"

//=============================================================================

namespace XM
{

// unquantized DC structure
class DCF {
public:
  friend class DominantColorExtractionTool;
  DCF() {
    size = 0;
  };
  DCF(int mysize) {
    size = mysize;
    //myAlloc();
  };
  DCF(const DCF &dcf) {
    size = dcf.size;
    //myAlloc();
    myCopy(dcf);
  };
  ~DCF() {
    //myFree();
  };
  inline int getSize() const {return size;};
  inline int resize(int mysize) {size = mysize; return size;}; // no error checking!
  inline void incrSize() {size++;}; // no error checking!
  inline int getSC() const {return sc;};
  inline void setSC(int mysc) {sc = mysc;};
  inline float getWeight(int index) const {return weights[index];};
  inline const float* getCentroid(int index) const {return centroids[index];};
  inline const float* getVariance(int index) const {return variances[index];};
  void reset();
  void agglom(double distthr);
  void mergeEmpty(int jam, int jbm);
  void verifyCentroids();
  inline void scaleWeights(int factor) {
    for(int i=0; i<size; i++)
      weights[i] /= factor;
  };
  void addCluster(const int where, const float weight,
                  const float c1, const float c2, const float c3,
                  const float v1, const float v2, const float v3);
  float distance(const DCF &other) const;
  friend bool operator< (const DCF &o1, const DCF &o2);
  friend bool operator> (const DCF &o1, const DCF &o2);
  friend bool operator== (const DCF &o1, const DCF &o2);
  friend bool operator!= (const DCF &o1, const DCF &o2);
private:
  int size;
  int sc;
  float weights[DominantColorDescriptor::DCNMAX];
  float centroids[DominantColorDescriptor::DCNMAX][3];
  float variances[DominantColorDescriptor::DCNMAX][3];
  void myCopy(const DCF &dcf);
};

// Forward Declarations:
class DominantColorDescriptor;
class DominantColorExtractionTool;

//=============================================================================
class DominantColorExtractionInterfaceABC: public I_InterfaceABC
{
public:
  DominantColorExtractionInterfaceABC();
  virtual ~DominantColorExtractionInterfaceABC() {};

  virtual void destroy()=0;

  virtual int SetSourceMedia(MultiMediaInterfaceABC *media) = 0;
  virtual DominantColorDescriptorInterfaceABC* GetDescriptorInterface()=0;
  virtual int SetDescriptorInterface(DominantColorDescriptorInterfaceABC
				     *aDominantColorDescriptorInterface)=0;
  virtual unsigned long InitExtracting()=0; 
  virtual unsigned long StartExtracting()=0; 
  virtual unsigned long PostExtracting()=0; 
  virtual const DCF * const GetDCF()=0;
  virtual unsigned long DescFromDCF(const DCF *pdcf)=0;
                                                                     
  static const UUID myID;
};

//=============================================================================
class DominantColorExtractionInterface:
    public DominantColorExtractionInterfaceABC
{
public:

  DominantColorExtractionInterface(DominantColorExtractionTool* aTool);
  virtual ~DominantColorExtractionInterface();

  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy();

  virtual int SetSourceMedia(MultiMediaInterfaceABC *media);
  virtual DominantColorDescriptorInterfaceABC* GetDescriptorInterface();
  virtual int SetDescriptorInterface(DominantColorDescriptorInterfaceABC
				     *aDominantColorDescriptorInterface);
  virtual unsigned long InitExtracting();
  virtual unsigned long StartExtracting();
  virtual unsigned long PostExtracting();
  virtual const DCF * const GetDCF();
  virtual unsigned long DescFromDCF(const DCF *pdcf);

  static const UUID myID;
  static const char *myName;

private:

  DominantColorExtractionTool *m_ExtractionTool;

};

//=============================================================================
class DominantColorExtractionTool: public DescriptorExtractor
{
  friend class DominantColorExtractionInterface;

public:

  DominantColorExtractionTool();
  DominantColorExtractionTool(DominantColorDescriptorInterfaceABC *dc);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual bool SupportsPush(void);
  virtual bool SupportsPull(void);

  virtual int SetSourceMedia(MultiMediaInterfaceABC *media);
  virtual DominantColorDescriptorInterfaceABC* GetDescriptorInterface();
  virtual int SetDescriptorInterface(DominantColorDescriptorInterfaceABC
                                     *aDominantColorDescriptorInterface);
  virtual unsigned long InitExtracting();
  virtual unsigned long StartExtracting();
  virtual unsigned long PostExtracting();
  virtual const DCF *const GetDCF();
  virtual unsigned long DescFromDCF(const DCF *pdcf);

  virtual DominantColorExtractionInterfaceABC *GetInterface(void);

  // access is allowed only by class factories for this
  // object.  This avoids having to duplicate the
  // ID definition in multiple locations.  In the future, we may
  // have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
  // IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
  static const UUID myID;
  static const char * myName;

private:

  void RGB2LUV(MomVop *rgbvop);
  void Extract(unsigned char *quantImageAlpha);
  double Cluster(unsigned char *quantImageAlpha);
  int Centroids(unsigned char *quantImageAlpha);
  double Dist(unsigned char *quantImageAlpha);
  void Vars(unsigned char *quantImageAlpha);
  void Split(double splfct, unsigned char *quantImageAlpha);
  
  int QuantizeSC(double sc);
  int GetSpatialCoherency(int dim, unsigned char *quantImageAlpha);
  double GetCoherencyWithColorAllow(int dim, bool *IVisit,
				    float l, float u, float v, float Allow,
				    int NeighborRange,
				    unsigned int *OUTPUT_Corres_Pixel_Count);
  void ConvertCSCQ(bool cspres, bool cqpres, dc_t *qdesc);
  void LUV2RGB(int *RGB, const float *LUV, int size);

  virtual ~DominantColorExtractionTool();

  DominantColorExtractionInterface m_Interface;
  DominantColorDescriptorInterfaceABC *m_DescriptorInterface;
  MultiMediaInterfaceABC *m_Media;

 // enum{LUVX0=0.9820, LUVY0=1, LUVZ0=1.1830};
  static const double LUVX0;
  static const double LUVY0;
  static const double LUVZ0;
  static const double LUVU20;
  static const double LUVV20;
  

  int m_nummax;
  int m_imwidth, m_imheight;
  int m_imsize; // image size
  int m_oldsize; // old image size

  float *m_image; // image date in LUV colour space
  int *m_closest; // closest centroids array
  DCF     m_dcf;

};


};


#endif //__DOMINANTCOLOREXTRACTION_H__
