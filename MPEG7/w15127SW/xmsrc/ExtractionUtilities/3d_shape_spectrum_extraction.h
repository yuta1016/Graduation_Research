///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Institut National des Telecommunications - ARTEMIS Project Unit, 
// Titus Zaharia, Marius Preda
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
// Applicable File Name:  3d_shape_spectrum_extraction.h
//

#ifndef _3D_SHAPE_SPECTRUM_EXTRACTOR_H_
#define _3D_SHAPE_SPECTRUM_EXTRACTOR_H_

#ifndef M_PI
#define	M_PI		(3.14159265358979323846)
#endif

#include <Media/Media.h>

typedef struct{
  float x,y,z;
}T3dPoint;


class ShapeSpectrum_Extractor
{
 public:
  int NoPoints;
  int NoTriangl;
  int NoEdges;
  float **PObj;
  int **TObj;
  
  int *TisVisited;
  int **EObj;  
  int **Edges; 

  float AreaTot;
  float *AreasT;
  T3dPoint *TMassCenter;
  int *IsBorderPoint; 
  T3dPoint *TNormals;
  
  int *NoAdiacentTriangles; 
  int **ListAdiacentTriangles; 

  double *CourbMin;
  double *CourbMax;
  double *ShapeIndex; 


  /*methods*/
  ShapeSpectrum_Extractor();
  ~ShapeSpectrum_Extractor();
  int ObjAlloc(int nP, int nT);
  int ObjInit(MediaThreeD *media3d);
  int ObjDup(ShapeSpectrum_Extractor *media3d); 
  
  int AllocEdges(void);
  int LoadEdges(void); 
  
  void ComputeAreaTriangles(void);
  int AllocNormals(void);
  void ComputeNormalTriangles(void);
  void ComputeNormalTriangles1(void);
  void GetTriangleMassCenter(void); 
  int LoadTrianglesListforEachPoint(void);
  
  void AddPointsTriangles(ShapeSpectrum_Extractor *AOb);
  int AdjustPoints(void);
  int AdjustPoints_tit(void);
  int GetNeighNo1(int index);
  int GetNeigh1(int index, int neighno,T3dPoint *neighs,int *AdTri, int *AdPoi,T3dPoint *neighsOne, int *nOne, int *nTri);
  int MakeShapeSpectrum(double *ss, int NoBins, int NoBits);

};

int ShapeSpectrumEstimation(MediaThreeD *m_Media3D, int NoOfBins, int NoOfBits, int noMRZ, unsigned long *ulss); 
int Quantif(double *d, int NoBinsdb, int nbits, unsigned long *ulss);

#endif
