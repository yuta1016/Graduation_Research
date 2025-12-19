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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Media/Media.h"
#include "ExtractionUtilities/3d_shape_spectrum_extraction.h"



#define SIGN(a, b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

#ifndef PI
#define PI 31415927e-7
#endif

#define PRESQUEZERO 1e-6


int ShapeSpectrumEstimation(MediaThreeD *m_Media3D, int NoOfBins, int NoOfBits, int noMRZ, unsigned long *ulss);

int CalcMeanNormal(ShapeSpectrum_Extractor *Obj, int t, int *ntp, int Notp, T3dPoint &MN);
int CalcCPAproxPar(ShapeSpectrum_Extractor *Obj, int t, int *ntp, int Notp, T3dPoint MeanNormal, float *cp);
int ChangeLocalCoord(T3dPoint *Point, int NoPoints, T3dPoint Orig, T3dPoint N, T3dPoint *Point_out);
int Estim2ndSurf(T3dPoint *Point, int NoPoints, float *Weigths, T3dPoint Orig, float *a);
void EstimCPar(float *a, float *CourbPrinc);

inline void ProduitTensoriel(float *Cv1, float *Cv2, int n, float **M);
inline void  MultMatrixScalar(float **M, float w, int m, int n);
inline void MultVectorScalar(float *v, float w, int n);
inline void IncMatrix(float **M, float **Mdelta, int m, int n);
inline void IncVector(float *b, float *bdelta, int n);
inline void  InitZero(float **M, int m, int n);
inline void  InitZero(float *b, int n);


int vcomparPoint(T3dPoint *x,  T3dPoint *y);
int vicompar(int *x, int *y);

float ComputeArea(float x1,float y1,float z1,float x2,float y2,float z2,float x3,float y3,float z3);

int pinv(double **a, int m, int n, double *w, double **v, double *b, double *sol);
inline double pythag(double a, double b);

double *alocavd(unsigned int n);
double ** alocamd(unsigned int dx,unsigned int dy);
float *alocavf(unsigned int n);
float ** alocamf(unsigned int dx,unsigned int dy);
int *alocavi(unsigned int n);
void freevd(double *v);
void freevi(int *v);
void freevf(float *v);
void dalocd(double ** m,unsigned int dx);
void dalocf(float ** m,unsigned int dx);





//--------------------------------------------------------------------------
int ShapeSpectrumEstimation(MediaThreeD *m_Media3D, int NoOfBins, int NoOfBits, int noMRZ, unsigned long *ulss)
{
  ShapeSpectrum_Extractor *SSE=0, *MRZSSE=0; 
  SSE = new ShapeSpectrum_Extractor(); 
  MRZSSE = new ShapeSpectrum_Extractor();
 
  int i, r; 

  for(r=0;r<noMRZ;r++)
    {
      if(r == 0)
	if(m_Media3D->NoTriangl > 100000)
	  {
	    MRZSSE->ObjAlloc(m_Media3D->NoPoints, m_Media3D->NoTriangl);
	    MRZSSE->ObjInit(m_Media3D);
	    MRZSSE->LoadTrianglesListforEachPoint();
	    MRZSSE->AllocEdges();
	    MRZSSE->LoadEdges();
	    break; 
	  }     
      if(r == 0)
	{
	  SSE->ObjAlloc(m_Media3D->NoPoints, m_Media3D->NoTriangl);
	  SSE->ObjInit(m_Media3D);
	  SSE->LoadTrianglesListforEachPoint();
	  SSE->AllocEdges();
	  SSE->LoadEdges();	 
	}
      if(SSE->NoTriangl <= 100000)
	{
	  if(MRZSSE)
	    delete MRZSSE;
	  MRZSSE = new ShapeSpectrum_Extractor();	  
	  MRZSSE->ObjAlloc(SSE->NoPoints+SSE->NoEdges, 4*SSE->NoTriangl);
	  MRZSSE->AddPointsTriangles(SSE);
	  MRZSSE->LoadTrianglesListforEachPoint();
	  MRZSSE->AdjustPoints_tit();
	  MRZSSE->AllocEdges();
	  MRZSSE->LoadEdges();
	}   
    
	  if(SSE)
	    delete SSE;
      
	  SSE = new ShapeSpectrum_Extractor();
      SSE->ObjDup(MRZSSE); /*SSE=MRZSSE;*/
    }
  

  int trian, Notp, Notp1, k, testc=0; T3dPoint MN;
  int ntp[1500]; int ntp1[1500];
  float AreaLocal;
  float CourbPrincip[2];
  float kamin=1000000.0, kamax=-10000000.0; 
  float ka, ShIndex; 

  MRZSSE->ComputeAreaTriangles();
  MRZSSE->GetTriangleMassCenter();
  MRZSSE->AllocNormals();
  MRZSSE->ComputeNormalTriangles1();

  for(trian =0;trian<MRZSSE->NoTriangl;trian++)
    {
      Notp=0;
      for(k=0;k<3;k++)
	for (i=0;i<MRZSSE->NoAdiacentTriangles[MRZSSE->TObj[trian][k]];i++)
	  if(MRZSSE->ListAdiacentTriangles[MRZSSE->TObj[trian][k]][i] != trian)
	    ntp[Notp++] = MRZSSE->ListAdiacentTriangles[MRZSSE->TObj[trian][k]][i];
      for(k=0;k<Notp;k++)
	ntp1[k] = ntp[k];
      qsort((int *)ntp1, Notp, sizeof(int), (int(*)(const void *x, const void *y))vicompar);
      
     
      if (Notp < 3)
	{
		  //fprintf(stderr, "Notp  0\n"); 
	MRZSSE->CourbMin[trian]=0.0; MRZSSE->CourbMax[trian]=0.0;
	MRZSSE->ShapeIndex[trian]=3.0;
	continue; }

      Notp1 = 1; ntp[0] = ntp1[0]; AreaLocal = MRZSSE->AreasT[trian]; 
      AreaLocal += MRZSSE->AreasT[ntp[0]]; 

      for(k=1;k<Notp;k++)
	if(ntp1[k] != ntp1[k-1])
	  {
	    ntp[Notp1] = ntp1[k];
	    AreaLocal += MRZSSE->AreasT[ntp[Notp1]]; 
	    Notp1++;
	  }
      
      Notp = Notp1;
      
      CalcMeanNormal(MRZSSE, trian, ntp, Notp, MN);  
      testc = CalcCPAproxPar(MRZSSE, trian, ntp, Notp, MN, CourbPrincip); 
      
    

      ka = (float)sqrt(CourbPrincip[0]*CourbPrincip[0]+CourbPrincip[1]*CourbPrincip[1]);
      
      if (testc)
	{
	  double sqAloc = sqrt(AreaLocal);
	  double kAloc = sqAloc * ka;  
	  
	  if (fabs(CourbPrincip[0]-CourbPrincip[1])>PRESQUEZERO) ShIndex=(0.5)-(atan((CourbPrincip[0]+CourbPrincip[1])/(CourbPrincip[0]-CourbPrincip[1])))/PI;
	  else {
	    if (CourbPrincip[0]>0) ShIndex=0;
	    else  if (CourbPrincip[0]<0) ShIndex=1;
	    else {
	      ShIndex=2.0; // surfaces planes
	    }
	  }
	  
	  
	  if(kAloc<0.25)
	    {  
	      ShIndex=2.0;// surfaces planes   
	    } 	  	 	  
	  
	  MRZSSE->ShapeIndex[trian]=fabs(ShIndex); 
	  
	  if (CourbPrincip[0]<CourbPrincip[1]){
	    MRZSSE->CourbMin[trian]=CourbPrincip[0];
	    MRZSSE->CourbMax[trian]=CourbPrincip[1];
	  }
	  else {
	    MRZSSE->CourbMin[trian]=CourbPrincip[1];
	    MRZSSE->CourbMax[trian]=CourbPrincip[0];
	  }
	}
      else
	{MRZSSE->CourbMin[trian]=0.0; MRZSSE->CourbMax[trian]=0.0;
	MRZSSE->ShapeIndex[trian]=3.0;
	}                  
    
    }

  double *ss = alocavd(NoOfBins+2);
  if(!ss) return 0;
  
  for(i=0;i<NoOfBins+2;i++)
    ss[i] = 0.0; 
  MRZSSE->MakeShapeSpectrum(ss, NoOfBins, NoOfBits);

  Quantif(ss, NoOfBins+2, NoOfBits, ulss);
    
  freevd(ss);
 
  if(SSE)
    delete SSE;
  if(MRZSSE)
    delete MRZSSE;
  return 1; 
}

//sth delimiters missing
float ComputeArea(float x1,float y1,float z1,float x2,float y2,float z2,float x3,float y3,float z3)
{
  double X1, Y1, Z1, X2, Y2, Z2, ad, bd, cd, ar; float arf; 

  X1 = x2-x1; Y1 = y2-y1; Z1 = z2-z1;
  X2 = x3-x1; Y2 = y3-y1; Z2 = z3-z1;

  ad = Y1*Z2 - Z1*Y2;
  bd = Z1*X2 - X1*Z2;
  cd = X1*Y2 - Y1*X2;
  
  ar = ad*ad + bd*bd + cd*cd; 
  
  arf = (float)sqrt(ar); arf /= 2.0; 

  return arf; 
}


ShapeSpectrum_Extractor::ShapeSpectrum_Extractor():
  NoPoints(0),
  NoTriangl(0),
  NoEdges(0),
  PObj(0), 
  TObj(0), 
  TMassCenter(0),
  AreaTot(0),
  AreasT(0), 
  ShapeIndex(0),
  CourbMin(0), 
  CourbMax(0),
  TisVisited(0),
  EObj(0),
  Edges(0),
  NoAdiacentTriangles(0),
  ListAdiacentTriangles(0), 
  TNormals(0)
{
  
  NoPoints = 0; NoTriangl = 0; NoEdges = 0;
  PObj=0; TObj=0; TMassCenter=0; AreasT=0; ShapeIndex=0; CourbMin=0; CourbMax=0;
  TisVisited=0; EObj=0; Edges=0;
  NoAdiacentTriangles=0;ListAdiacentTriangles=0; 
  
}


ShapeSpectrum_Extractor::~ShapeSpectrum_Extractor()
{
  int i;
 
  if(PObj)
    {
      for (i=0;i<NoPoints;i++)
	if(PObj[i])
	  free(PObj[i]);
      if(PObj)
	free(PObj);
    }
 
  if(TObj)
    {
      for (i=0;i<NoTriangl;i++)
	if(TObj[i])
	  free(TObj[i]);
      if(TObj)
	free(TObj);
    }
 
  if(TMassCenter)
    free(TMassCenter);
 
  if(AreasT)
    free(AreasT);
 
  if(TNormals)
    free(TNormals);

  if(ShapeIndex) 
    free(ShapeIndex);

  if(CourbMin)
    free(CourbMin);

  if(CourbMax)
    free(CourbMax);
  if(TisVisited)
    free(TisVisited);
  if(EObj)
    {
      for (i=0;i<3;i++)
	if(EObj[i])
	  free(EObj[i]);
      if(EObj)
	free(EObj);
    }

  if(Edges)
    {
      for (i=0;i<2;i++)
	if(Edges[i])
	  free(Edges[i]);
      if(Edges)
	free(Edges);
    }
  if(NoAdiacentTriangles)
    free(NoAdiacentTriangles);  
  if(ListAdiacentTriangles)
    {
      for (i=0;i<NoPoints;i++)
	if(ListAdiacentTriangles[i])
	  free(ListAdiacentTriangles[i]);
      if(ListAdiacentTriangles)
	free(ListAdiacentTriangles);
    }
}


int ShapeSpectrum_Extractor::ObjAlloc(int nP, int nT)
{
  int i;
  NoPoints=nP;
  NoTriangl=nT;
 
  PObj = 0;
  PObj=(float**)malloc(NoPoints*sizeof(float*));
  if (PObj==NULL) return 0; 
  
  for (i=0;i<NoPoints;i++){
    PObj[i]=0;
    PObj[i]=(float*)malloc(3*sizeof(float));
    if (PObj[i]==NULL) return 0; 
  }

  TObj=0;
  TObj=(int**)malloc(NoTriangl*sizeof(int*));
  if (TObj==NULL) return 0; 

  for (i=0;i<NoTriangl;i++){
    TObj[i]=0;
    TObj[i]=(int*)malloc(3*sizeof(int));
    if (TObj[i]==NULL) return 0; 
  } 

  TisVisited = 0;
  TisVisited = (int*)malloc(NoTriangl*sizeof(int));
  if(TisVisited == NULL)
    return 0;
    
  for (i=0;i<NoTriangl;i++) 
    TisVisited[i] = 0; /*Initialisation */ 

  TMassCenter=0;
  TMassCenter=(T3dPoint*)malloc(NoTriangl*sizeof(T3dPoint));
  if (TMassCenter==NULL) { return 0;}

  NoAdiacentTriangles=0;
  NoAdiacentTriangles=(int*)malloc(NoPoints*sizeof(int));
  if (NoAdiacentTriangles==NULL) { return 0;}

  AreasT=0;
  AreasT=(float*)malloc(NoTriangl*sizeof(float));
  if (AreasT==NULL) {return 0;}  

  CourbMin=0;
  CourbMin=(double*)malloc(NoTriangl*sizeof(double));
  if (CourbMin==NULL) {return 0;}

  CourbMax=0;
  CourbMax=(double*)malloc(NoTriangl*sizeof(double));
  if (CourbMax==NULL) {return 0;}
  
  ShapeIndex = 0; 
  ShapeIndex=(double*)malloc(NoTriangl*sizeof(double));
  if (ShapeIndex==NULL) {return 0;}
  return 1;   
}

int ShapeSpectrum_Extractor::ObjInit(MediaThreeD *media3d)
{
  int i, k;
  if(NoPoints != media3d->NoPoints)
    return 0;
  if(NoTriangl != media3d->NoTriangl)
    return 0;
  for (i=0;i<NoPoints;i++)
    for(k=0;k<3;k++)
      PObj[i][k] = media3d->PObj[i][k]; 
  
  for (i=0;i<NoTriangl;i++)
    for(k=0;k<3;k++)
      TObj[i][k] = media3d->TObj[i][k];

  return 1; 
  
}


int ShapeSpectrum_Extractor::ObjDup(ShapeSpectrum_Extractor *media3d)
{
  int i, k;
  
  NoPoints = media3d->NoPoints;
  NoTriangl = media3d->NoTriangl;
  NoEdges = media3d->NoEdges;
  
  PObj = 0;
  PObj=(float**)malloc(NoPoints*sizeof(float*));
  if (PObj==NULL) return 0; 
  
  for (i=0;i<NoPoints;i++){
    PObj[i]=0;
    PObj[i]=(float*)malloc(3*sizeof(float));
    if (PObj[i]==NULL) return 0; 
  }
  for (i=0;i<NoPoints;i++)
    for(k=0;k<3;k++)
      PObj[i][k] = media3d->PObj[i][k]; 
  
  TObj=0;
  TObj=(int**)malloc(NoTriangl*sizeof(int*));
  if (TObj==NULL) return 0; 

  for (i=0;i<NoTriangl;i++){
    TObj[i]=0;
    TObj[i]=(int*)malloc(3*sizeof(int));
    if (TObj[i]==NULL) return 0; 
  } 
  for (i=0;i<NoTriangl;i++)
    for(k=0;k<3;k++)
      TObj[i][k] = media3d->TObj[i][k];
  TisVisited = 0;
  TisVisited = (int*)malloc(NoTriangl*sizeof(int));
  if(TisVisited == NULL)
    return 0;
  for (i=0;i<NoTriangl;i++)
    TisVisited[i] = media3d->TisVisited[i];  
  TMassCenter=0;
  TMassCenter=(T3dPoint*)malloc(NoTriangl*sizeof(T3dPoint));
  if (TMassCenter==NULL) { return 0;}
  for (i=0;i<NoTriangl;i++)
    TMassCenter[i] = media3d->TMassCenter[i]; 
  NoAdiacentTriangles=0;
  NoAdiacentTriangles=(int*)malloc(NoPoints*sizeof(int));
  if (NoAdiacentTriangles==NULL) { return 0;}
  for (i=0;i<NoPoints;i++)
    NoAdiacentTriangles[i] = media3d->NoAdiacentTriangles[i]; 
  AreasT=0;
  AreasT=(float*)malloc(NoTriangl*sizeof(float));
  if (AreasT==NULL) {return 0;}  
  for (i=0;i<NoTriangl;i++)
    AreasT[i] = media3d->AreasT[i];
 
  CourbMin=0;
  CourbMin=(double*)malloc(NoTriangl*sizeof(double));
  if (CourbMin==NULL) {return 0;}
  for (i=0;i<NoTriangl;i++)
    CourbMin[i] = media3d->CourbMin[i]; 
  CourbMax=0;
  CourbMax=(double*)malloc(NoTriangl*sizeof(double));
  if (CourbMax==NULL) {return 0;}
  for (i=0;i<NoTriangl;i++)
    CourbMax[i] = media3d->CourbMax[i]; 
  ShapeIndex = 0; 
  ShapeIndex=(double*)malloc(NoTriangl*sizeof(double));
  if (ShapeIndex==NULL) {return 0;}
  for (i=0;i<NoTriangl;i++)
    ShapeIndex[i] = media3d->ShapeIndex[i];
  if(media3d->Edges)
    {
      Edges=(int**)malloc(2*sizeof(int*));
      if (Edges==NULL){
	return 0; 
      }
      for (i=0;i<2;i++){
	Edges[i]=(int*)malloc(3*NoTriangl*sizeof(int));
	if (Edges[i]==NULL){
	  return 0;
	}
      }
      for(i=0;i<2;i++)
	for(k=0;k<3*NoTriangl;k++)
	  Edges[i][k] = media3d->Edges[i][k]; 
    }
  if(media3d->EObj)
    {
      EObj=(int**)malloc(3*sizeof(int*));
      if (EObj==NULL){
	return 0;
      }
      for (i=0;i<3;i++){
	EObj[i]=(int*)malloc(NoTriangl*sizeof(int));
	if (EObj[i]==NULL){
	  return 0;
	}
      } 
      for (i=0;i<3;i++)
	for(k=0;k<NoTriangl;k++)
	  EObj[i][k] = media3d->EObj[i][k];
    }
  if(media3d->ListAdiacentTriangles)
    {
      ListAdiacentTriangles=0;
      ListAdiacentTriangles=(int**)malloc(NoPoints*sizeof(int*));
      if (ListAdiacentTriangles==NULL) return 0;
      
      for (i=0;i<NoPoints;i++){
	ListAdiacentTriangles[i]=0;
	ListAdiacentTriangles[i]=(int*)malloc(2*NoAdiacentTriangles[i]*sizeof(int));
	if (ListAdiacentTriangles[i]==NULL) return 0;
      }
      for (i=0;i<NoPoints;i++)
	for(k=0;k<2*NoAdiacentTriangles[i];k++)
	  ListAdiacentTriangles[i][k] = media3d->ListAdiacentTriangles[i][k];	
    }
  if(media3d->TNormals)
    {
      TNormals = 0;
      TNormals=(T3dPoint*)malloc(NoTriangl*sizeof(T3dPoint));
      if (TNormals==NULL) return 0;
      for (i=0;i<NoTriangl;i++)
	TNormals[i] = media3d->TNormals[i];
    }
  return 1;   
}



int ShapeSpectrum_Extractor::AllocEdges(void)
{
  int i;
  
  Edges=(int**)malloc(2*sizeof(int*));
  if (Edges==NULL){
    return 0; 
  }
  for (i=0;i<2;i++){
    Edges[i]=(int*)malloc(3*NoTriangl*sizeof(int));
    if (Edges[i]==NULL){
      return 0;
    }
  }

  EObj=(int**)malloc(3*sizeof(int*));
  if (EObj==NULL){
    return 0;
  }
  for (i=0;i<3;i++){
    EObj[i]=(int*)malloc(NoTriangl*sizeof(int));
    if (EObj[i]==NULL){
      return 0;
    }
  } 
  return 1; 
}

int ShapeSpectrum_Extractor::LoadEdges(void)
{
  int i,j,k=0,l,m;
  int *neigh;
  int *count;
  
  count = 0;
  count=(int*)calloc(NoTriangl,sizeof(int));
  if (count==NULL) {return 0;}
  
  for (i=0;i<NoPoints;i++){
    neigh = 0;
    neigh=(int*)calloc(3*NoAdiacentTriangles[i],sizeof(int));    
    if (neigh==NULL) {return 0;}
    l=0;
    
    for (j=0;j<NoAdiacentTriangles[i];j++){   
      neigh[l]=TObj[ListAdiacentTriangles[i][j]][0]; l++;
      neigh[l]=TObj[ListAdiacentTriangles[i][j]][1]; l++;
      neigh[l]=TObj[ListAdiacentTriangles[i][j]][2]; l++;
    }
    
    for (j=0;j<l;j++){
      for (m=0;m<j;m++)
	if (neigh[m]==neigh[j]) neigh[j]=-1;
    }
    
    for (j=0;j<l;j++){
      if ((neigh[j]!=-1)&&(neigh[j]!=i)){
	if (neigh[j]<i){
	  Edges[0][k]=i;
	  Edges[1][k]=neigh[j];
	  k++;
	}
      }    
    }

    if(neigh)
      free(neigh);   
  }
  
  NoEdges=k;

  for (i=0;i<NoEdges;i++){
    for(j=0;j<NoAdiacentTriangles[Edges[0][i]];j++){
      for(k=0;k<NoAdiacentTriangles[Edges[1][i]];k++){
	if (ListAdiacentTriangles[Edges[0][i]][j]==ListAdiacentTriangles[Edges[1][i]][k]){
	  EObj[count[ListAdiacentTriangles[Edges[0][i]][j]]][ListAdiacentTriangles[Edges[0][i]][j]]=i;
	  count[ListAdiacentTriangles[Edges[0][i]][j]]++;
	}
      }
    }
  }
 
  if(count)
    free(count);
  return 1; 
      
}



void ShapeSpectrum_Extractor::ComputeAreaTriangles(void)
{
  int i;
  AreaTot = 0.0; 
  double AreaTotd = 0.0;

  for (i=0;i<NoTriangl;i++){
    AreasT[i]=ComputeArea(PObj[TObj[i][0]][0],PObj[TObj[i][0]][1],PObj[TObj[i][0]][2],PObj[TObj[i][1]][0],PObj[TObj[i][1]][1],PObj[TObj[i][1]][2],PObj[TObj[i][2]][0],PObj[TObj[i][2]][1],PObj[TObj[i][2]][2]);
    AreaTot += AreasT[i]; 
	AreaTotd += AreasT[i];
  }
}



int ShapeSpectrum_Extractor::AllocNormals(void)
{
  TNormals = 0;
  TNormals=(T3dPoint*)malloc(NoTriangl*sizeof(T3dPoint));
  if (TNormals==NULL) {return 0;}
  return 1; 
}

void ShapeSpectrum_Extractor::ComputeNormalTriangles(void)
{
  int i;
  float x,y,z,norm;
  for (i=0;i<NoTriangl;i++){
    
    x=(PObj[TObj[i][0]][2]-PObj[TObj[i][1]][2])*(PObj[TObj[i][2]][1]-PObj[TObj[i][1]][1])-(PObj[TObj[i][0]][1]-PObj[TObj[i][1]][1])*(PObj[TObj[i][2]][2]-PObj[TObj[i][1]][2]);
    y=(PObj[TObj[i][0]][0]-PObj[TObj[i][1]][0])*(PObj[TObj[i][2]][2]-PObj[TObj[i][1]][2])-(PObj[TObj[i][0]][2]-PObj[TObj[i][1]][2])*(PObj[TObj[i][2]][0]-PObj[TObj[i][1]][0]);
    z=(PObj[TObj[i][0]][1]-PObj[TObj[i][1]][1])*(PObj[TObj[i][2]][0]-PObj[TObj[i][1]][0])-(PObj[TObj[i][0]][0]-PObj[TObj[i][1]][0])*(PObj[TObj[i][2]][1]-PObj[TObj[i][1]][1]);
    norm=sqrt(x*x+y*y+z*z);
    if (norm!=0){
      TNormals[i].x=x/norm;
      TNormals[i].y=y/norm;
      TNormals[i].z=z/norm;
    }
    else
      {
	TNormals[i].x=0;
	TNormals[i].y=0;
	TNormals[i].z=0;
      }
    TNormals[i].x = -TNormals[i].x;  TNormals[i].y = -TNormals[i].y;  TNormals[i].z = -TNormals[i].z; 
  }
}

void ShapeSpectrum_Extractor::ComputeNormalTriangles1(void)
{
  int i;
 
  double xd, yd, zd, normd, d1x, d1y, d1z, d2x, d2y, d2z; 
  
  for (i=0;i<NoTriangl;i++){
    d1x = (double)(PObj[TObj[i][0]][0]);
    d1x -= (double)(PObj[TObj[i][1]][0]);

    d1y = (double)(PObj[TObj[i][0]][1]); 
    d1y -= (double)(PObj[TObj[i][1]][1]);

    d1z = (double)(PObj[TObj[i][0]][2]); 
    d1z -= (double)(PObj[TObj[i][1]][2]);

    d2x = (double)(PObj[TObj[i][0]][0]); 
    d2x -= (double)(PObj[TObj[i][2]][0]);
    d2y = (double)(PObj[TObj[i][0]][1]);
    d2y -= (double)(PObj[TObj[i][2]][1]);
    d2z = (double)(PObj[TObj[i][0]][2]); 
    d2z -= (double)(PObj[TObj[i][2]][2]);

    xd = d1y*d2z - d1z*d2y;
    yd = d1z*d2x - d1x*d2z; 
    zd = d1x*d2y - d1y*d2x; 

    
    normd = sqrt(xd*xd+yd*yd+zd*zd);

    if (normd != 0.0){
      TNormals[i].x = (float)(xd/normd);
      TNormals[i].y = (float)(yd/normd);
      TNormals[i].z = (float)(zd/normd);
    }
    else
      {
	TNormals[i].x=0.0;
	TNormals[i].y=0.0;
	TNormals[i].z=0.0;
      }
    TNormals[i].x = -TNormals[i].x;  TNormals[i].y = -TNormals[i].y;  TNormals[i].z = -TNormals[i].z; 
  }
}

void ShapeSpectrum_Extractor::GetTriangleMassCenter(void)
{
  for (int i=0;i<NoTriangl;i++){
    TMassCenter[i].x=(PObj[TObj[i][0]][0] + PObj[TObj[i][1]][0] + PObj[TObj[i][2]][0])/3.0;
    TMassCenter[i].y=(PObj[TObj[i][0]][1] + PObj[TObj[i][1]][1] + PObj[TObj[i][2]][1])/3.0;
    TMassCenter[i].z=(PObj[TObj[i][0]][2] + PObj[TObj[i][1]][2] + PObj[TObj[i][2]][2])/3.0;
    }    
}


int ShapeSpectrum_Extractor::LoadTrianglesListforEachPoint(void)
{

  int i;
  int *cont;
  for (i=0;i<NoPoints;i++)
    NoAdiacentTriangles[i]=0;
  for (i=0;i<NoTriangl;i++){
    NoAdiacentTriangles[TObj[i][0]]++;
    NoAdiacentTriangles[TObj[i][1]]++;
    NoAdiacentTriangles[TObj[i][2]]++;
  }
  
  cont = 0;
  cont=(int*)malloc(5*NoPoints*sizeof(int));
  if (cont==NULL) {return 0;}
  for (i=0;i<NoPoints;i++)
    cont[i]=0;

  ListAdiacentTriangles=0;
  ListAdiacentTriangles=(int**)malloc(NoPoints*sizeof(int*));
  if (ListAdiacentTriangles==NULL) return 0;

  for (i=0;i<NoPoints;i++){
    ListAdiacentTriangles[i]=0;
    ListAdiacentTriangles[i]=(int*)malloc(2*NoAdiacentTriangles[i]*sizeof(int));
    if (ListAdiacentTriangles[i]==NULL) return 0;
  }
  
  for (i=0;i<NoTriangl;i++){
    ListAdiacentTriangles[TObj[i][0]][cont[TObj[i][0]]]=i;
    cont[TObj[i][0]]++;
    ListAdiacentTriangles[TObj[i][1]][cont[TObj[i][1]]]=i;
    cont[TObj[i][1]]++;
    ListAdiacentTriangles[TObj[i][2]][cont[TObj[i][2]]]=i;
    cont[TObj[i][2]]++;
  }
  
  if(cont)
    free(cont);
  return 1;  
}


void ShapeSpectrum_Extractor::AddPointsTriangles(ShapeSpectrum_Extractor *AOb)
{
  int i,j,PCur,TCur,v0,v1,v2,vi0,vi1,vi2,temp,ord=0;
  for (i=0;i<AOb->NoPoints;i++){
    PObj[i][0] = AOb->PObj[i][0];
    PObj[i][1] = AOb->PObj[i][1];
    PObj[i][2] = AOb->PObj[i][2];
  }
  PCur=i;

  for (i=0;i<AOb->NoEdges;i++){
    PObj[PCur][0] = 0.5*(AOb->PObj[AOb->Edges[0][i]][0] + AOb->PObj[AOb->Edges[1][i]][0]);
    PObj[PCur][1] = 0.5*(AOb->PObj[AOb->Edges[0][i]][1] + AOb->PObj[AOb->Edges[1][i]][1]);
    PObj[PCur][2] = 0.5*(AOb->PObj[AOb->Edges[0][i]][2] + AOb->PObj[AOb->Edges[1][i]][2]); PCur++;
  }
  TCur=0;
  for (i=0;i<AOb->NoTriangl;i++){
    vi0=AOb->TObj[i][0];
    vi1=AOb->TObj[i][1];
    vi2=AOb->TObj[i][2];
    
    if (vi0<vi1) {
      temp=vi0;vi0=vi1;vi1=temp;
    }
    if (vi0<vi2) {
      temp=vi0;vi0=vi2;vi2=temp;
    }
    if (vi1<vi2) {
      temp=vi1;vi1=vi2;vi2=temp;
    }   

    
    j=AOb->EObj[0][i];
    if ((AOb->Edges[0][j]==vi0)&&(AOb->Edges[1][j]==vi1))
      v2=j+AOb->NoPoints;
    if ((AOb->Edges[0][j]==vi0)&&(AOb->Edges[1][j]==vi2))
      v1=j+AOb->NoPoints;
    if ((AOb->Edges[0][j]==vi1)&&(AOb->Edges[1][j]==vi2))
      v0=j+AOb->NoPoints;          
    j=AOb->EObj[1][i];
    if ((AOb->Edges[0][j]==vi0)&&(AOb->Edges[1][j]==vi1))
      v2=j+AOb->NoPoints;
    if ((AOb->Edges[0][j]==vi0)&&(AOb->Edges[1][j]==vi2))
      v1=j+AOb->NoPoints;
    if ((AOb->Edges[0][j]==vi1)&&(AOb->Edges[1][j]==vi2))
      v0=j+AOb->NoPoints;          
    j=AOb->EObj[2][i];
    if ((AOb->Edges[0][j]==vi0)&&(AOb->Edges[1][j]==vi1))
      v2=j+AOb->NoPoints;
    if ((AOb->Edges[0][j]==vi0)&&(AOb->Edges[1][j]==vi2))
      v1=j+AOb->NoPoints;
    if ((AOb->Edges[0][j]==vi1)&&(AOb->Edges[1][j]==vi2))
      v0=j+AOb->NoPoints;          

    
    if (vi0==AOb->TObj[i][0]){
      if (vi1==AOb->TObj[i][1]) ord=0;
      else ord=1;
    }
    if (vi0==AOb->TObj[i][1]){
      if (vi1==AOb->TObj[i][2]) ord=0;
      else ord=1;
    }      
    if (vi0==AOb->TObj[i][2]){
      if (vi1==AOb->TObj[i][0]) ord=0;
      else ord=1;
    }      
    
    if (ord){
      TObj[TCur][0] = vi0;
      TObj[TCur][1] = v1;
      TObj[TCur][2] = v2;
      TCur++;
      TObj[TCur][0] = vi1;
      TObj[TCur][1] = v2;
      TObj[TCur][2] = v0;
      TCur++;
      TObj[TCur][0] = vi2;
      TObj[TCur][1] = v0;
      TObj[TCur][2] = v1;
      TCur++;
      TObj[TCur][0] = v1;
      TObj[TCur][1] = v0;
      TObj[TCur][2] = v2;
      TCur++;
      }
    else {
      TObj[TCur][0] = vi0;
      TObj[TCur][1] = v2;
      TObj[TCur][2] = v1;
      TCur++;
      TObj[TCur][0] = vi1;
      TObj[TCur][1] = v0;
      TObj[TCur][2] = v2;
      TCur++;
      TObj[TCur][0] = vi2;
      TObj[TCur][1] = v1;
      TObj[TCur][2] = v0;
      TCur++;
      TObj[TCur][0] = v1;
      TObj[TCur][1] = v2;
      TObj[TCur][2] = v0;
      TCur++;
      }    
  }
}


int ShapeSpectrum_Extractor::AdjustPoints_tit(void)
{
 
  T3dPoint Point; 
  int nOne,nTri,i, point;
  float beta,w;
  float x,y,z;
  int Index, NeighNo;
  int *AdiacentPoints, *AdiacentTriangles; 
 
  float **PObj1=0;
  PObj1 = (float **)malloc(NoPoints*sizeof(float *));
  if(!PObj1)
    return 0; 
  for (point=0;point<NoPoints;point++)
    {
      PObj1[point] = 0;
      PObj1[point] = (float *)malloc(3*sizeof(float));
      if(!PObj1[point])
	return 0;
      for(i=0;i<3;i++)
	PObj1[point][i] = PObj[point][i];
    }
  
  for (point=0;point<NoPoints;point++){   
    Point.x=PObj[point][0];
    Point.y=PObj[point][1];
    Point.z=PObj[point][2];	
    Index=point;
    NeighNo=GetNeighNo1(Index);
  
    if (NeighNo>0){
      
      T3dPoint *Neigh,*NeighOne; Neigh = 0; NeighOne = 0;
      Neigh = 0;
      Neigh=0;
      Neigh=(T3dPoint*)malloc(NeighNo*sizeof(T3dPoint));     
      if(Neigh == 0)
	{fprintf(stderr, "Alloc error Neigh\n"); return 0;}
      NeighOne=0;
      NeighOne=(T3dPoint*)malloc(NeighNo*sizeof(T3dPoint));
      if(NeighOne == 0)
	{fprintf(stderr, "Alloc error NeighOne\n"); return 0;}
      AdiacentTriangles=0;
      AdiacentTriangles=(int*)malloc((int)(NeighNo/2)*sizeof(int));
      if (AdiacentTriangles==NULL) {printf("\n alloc error");return 0;}
      AdiacentPoints=0;
      AdiacentPoints=(int*)malloc(NeighNo*sizeof(int));
      if (AdiacentPoints==NULL) {printf("\n alloc error");return 0;}

      if (GetNeigh1(Index,NeighNo,Neigh,AdiacentTriangles,AdiacentPoints,NeighOne,&nOne, &nTri)){

	beta=(1+12*(cos(2*PI/nOne))+4*(cos(2*PI/nOne))*(cos(2*PI/nOne)))/32.0;
	w=nOne*beta/(1-beta);

	x=0.0;y=0.0;z=0.0;
	
	for (i=0;i<nOne;i++){
	    x+=NeighOne[i].x;
	    y+=NeighOne[i].y;
	    z+=NeighOne[i].z;
	}

	
	x+=w*Point.x;
	y+=w*Point.y;
	z+=w*Point.z;

	if((nOne+w)==0)
	  {return 0;}

	x/=(nOne+w);
	y/=(nOne+w);
	z/=(nOne+w);

	PObj1[point][0]=x;
	PObj1[point][1]=y;
	PObj1[point][2]=z;
      }
     
      if(Neigh != 0)
	free(Neigh);
      if(NeighOne!=0)
	free(NeighOne);
      if(AdiacentTriangles != 0)
	free(AdiacentTriangles);
      if(AdiacentPoints != 0)
	free(AdiacentPoints);
    }
   
  }

  for (point=0;point<NoPoints;point++)
    for(i=0;i<3;i++)
      PObj[point][i] = PObj1[point][i];
  if(PObj1)
    {
      for (int point=0;point<NoPoints;point++)
	if(PObj1[point])
	  free(PObj1[point]); 
      free(PObj1);
    }
  return 1;
}


int ShapeSpectrum_Extractor::GetNeighNo1(int index)
{  
  int i;
  int neighNo=0;  

  for (i=0;i<NoAdiacentTriangles[index];i++){
    neighNo=neighNo+2;
  }
  return neighNo;  
}


int ShapeSpectrum_Extractor::GetNeigh1(int index, int neighno,T3dPoint *neighs,int *AdTri, int *AdPoi,T3dPoint *neighsOne, int *nOne, int *nTri)
{
  int i,j,k,find=0;
  int *neighInd;  

  neighInd=0;
  neighInd=(int *)malloc(neighno*sizeof(int));
  if(neighInd==NULL)
    {fprintf(stderr, "not enough memory\n"); return -1;}
  j=0;
  k=0;
  for (i=0;i<NoAdiacentTriangles[index];i++){
    if (TObj[ListAdiacentTriangles[index][i]][0] == index) {
      
      neighInd[j]=TObj[ListAdiacentTriangles[index][i]][1];  j++;
      neighInd[j]=TObj[ListAdiacentTriangles[index][i]][2]; j++;
      AdTri[k]=ListAdiacentTriangles[index][i];k++;
    }
    else if (TObj[ListAdiacentTriangles[index][i]][1] == index) {
     
      neighInd[j]=TObj[ListAdiacentTriangles[index][i]][0];  j++; 
      neighInd[j]=TObj[ListAdiacentTriangles[index][i]][2]; j++;
      AdTri[k]=ListAdiacentTriangles[index][i];k++;
    }
    if (TObj[ListAdiacentTriangles[index][i]][2] == index) {
     
      neighInd[j]=TObj[ListAdiacentTriangles[index][i]][0];  j++; 
      neighInd[j]=TObj[ListAdiacentTriangles[index][i]][1]; j++;
      AdTri[k]=ListAdiacentTriangles[index][i];k++;
    }
  }
  *nTri=k;
  
  k=0;
  for (i=0;i<neighno;i++){
    find=0;
    for (j=0;j<i;j++){
      if (neighInd[j]==neighInd[i]){ find=1;
      break;}
    }
    if (find==0) {
      AdPoi[k]=neighInd[i];
      k++;
    }
  }
 
  *nOne=k;

  for (i=0;i<neighno;i++){
    find=0;
    for (j=0;j<neighno;j++)
      if ((neighInd[i]==neighInd[j])&&(j!=i)){
	find=1;j=neighno;
      }
    if (find==0) 
      {
	if(neighInd)
	  free(neighInd);
	return 0;
      }
  }
  for (i=0;i<neighno;i++){
    neighs[i].x=PObj[neighInd[i]][0];
    neighs[i].y=PObj[neighInd[i]][1];
    neighs[i].z=PObj[neighInd[i]][2];
  }
  for (i=0;i<k;i++){
    neighsOne[i].x=PObj[AdPoi[i]][0];
    neighsOne[i].y=PObj[AdPoi[i]][1];
    neighsOne[i].z=PObj[AdPoi[i]][2];
  }
  
  if(neighInd)
    free(neighInd);
  return 1;
}



int ShapeSpectrum_Extractor::MakeShapeSpectrum(double *ss, int NoBins, int NoBits)
{
  int i; unsigned char iS;  

  for (i=0;i<NoBins+2;i++)
    ss[i]=0.0;
  for (i=0;i<NoTriangl;i++)
    {
      if (ShapeIndex[i]==2.0)
	{iS = (unsigned char)(NoBins); // planar surfaces
	  }
      else if (ShapeIndex[i]==3.0) 
	{iS = (unsigned char)(NoBins+1); // singular surfaces
	}
      else if(ShapeIndex[i]>1.0)
	{return 0;}
      else if (ShapeIndex[i]<0.0)
	{return 0;}
      else if (ShapeIndex[i]!=1.0)
	iS = (unsigned char)(ShapeIndex[i]*(NoBins));
      else if (ShapeIndex[i]==1.0)
	iS = (unsigned char)(NoBins-1);
      ss[iS]+=AreasT[i]/AreaTot;
    }
  return 1; 
}


int CalcMeanNormal(ShapeSpectrum_Extractor *Obj, int t, int *ntp, int Notp, T3dPoint &MN)
{
  float xn = 0.0, yn = 0.0, zn = 0.0;
  int adt; 

  xn+=Obj->TNormals[t].x*Obj->AreasT[t];
  yn+=Obj->TNormals[t].y*Obj->AreasT[t];
  zn+=Obj->TNormals[t].z*Obj->AreasT[t];
  
  for (adt=0;adt<Notp;adt++){
    xn+=Obj->TNormals[ntp[adt]].x*Obj->AreasT[ntp[adt]];
    yn+=Obj->TNormals[ntp[adt]].y*Obj->AreasT[ntp[adt]];
    zn+=Obj->TNormals[ntp[adt]].z*Obj->AreasT[ntp[adt]];
  }
  float norm=sqrt(xn*xn+yn*yn+zn*zn);
  
  if (norm!=0)
    {MN.x=xn/norm; MN.y=yn/norm; MN.z=zn/norm;}
  else 
    {MN.x=0.0; MN.y=0.0; MN.z=0.0;}
  return 1; 
}




int CalcCPAproxPar(ShapeSpectrum_Extractor *Obj, int t, int *ntp, int Notp, T3dPoint MeanNormal, float *cp)
{
  int adt, i, j;
  T3dPoint Coord;
  T3dPoint Point[500], Point1[500];
  
  Coord = Obj->TMassCenter[t];
  
  float *ar = alocavf(500); 
  if(!ar) return 0; 

  float areaEst = 0.0; 
  float wijtot = 0.0; int NoPoints = 0, NoPoints1 = 0; 
  for (adt=0;adt<Notp;adt++)
    {
      if( t != ntp[adt] )
	{
	  Point[NoPoints].x = Obj->TMassCenter[ntp[adt]].x;
	  Point[NoPoints].y = Obj->TMassCenter[ntp[adt]].y; 
	  Point[NoPoints].z = Obj->TMassCenter[ntp[adt]].z;

	  ar[NoPoints] = Obj->AreasT[ntp[adt]];	 
	  NoPoints++;
	  
	}
    }
  
  Point[NoPoints].x = Obj->TMassCenter[t].x; 
  Point[NoPoints].y = Obj->TMassCenter[t].y;  
  Point[NoPoints].z = Obj->TMassCenter[t].z;  
  ar[NoPoints] = Obj->AreasT[t]; 
  NoPoints++; 
 
  for(i=0;i<NoPoints;i++)
    {Point1[i].x = Point[i].x; Point1[i].y = Point[i].y; Point1[i].z = Point[i].z;}

  qsort(Point, NoPoints, sizeof(T3dPoint),  (int(*)(const void *x, const void *y))vcomparPoint);
  

  int *indexp, *indexpfin; 
  indexp = alocavi(NoPoints); 
  if(!indexp) return 0; 
  indexpfin = alocavi(NoPoints);
  if(!indexpfin) return 0;

  T3dPoint P1[1], P2[1];
 
  for(i=0;i<NoPoints;i++)
    {
      j = 0;
      P1[0].x = Point1[i].x; P1[0].y = Point1[i].y; P1[0].z = Point1[i].z;     
      P2[0].x = Point[0].x; P2[0].y = Point[0].y; P2[0].z = Point[0].z;      
      
      while( (vcomparPoint(P2, P1) < 0) )
	{	  
	  j++;
	  P2[0].x = Point[j].x; P2[0].y = Point[j].y; P2[0].z = Point[j].z; 
	}
      indexp[j] = i;
    }

  int NoPoints_avant = NoPoints; 

  NoPoints1 = 1;
  Point1[0] = Point[0]; indexpfin[0] = indexp[0];
  areaEst = 0.0;
 
  for(i=1;i<NoPoints;i++)
    {
      if ( (Point[i].x != Point[i-1].x) || (Point[i].y != Point[i-1].y) || (Point[i].z != Point[i-1].z) )
	{
	  Point1[NoPoints1] = Point[i];
	  indexpfin[NoPoints1] = indexp[i];
	  areaEst += ar[indexp[i]]; 
	  NoPoints1++;
	}
    }
 
  freevi(indexp); indexp = 0;
  NoPoints = NoPoints1; 
  
  if(NoPoints < 6 )
	{
	  if(indexp) freevi(indexp); 
	  if(ar) freevf(ar); 
	  if(indexpfin) freevi(indexpfin);
	  return 0;}

  for(i=0;i<NoPoints;i++)
    Point[i] = Point1[i];
  

  /* test surface estimation */ 
  float psurf[6], ckc[2];
  float *Weigths = 0; 
  Weigths = alocavf(NoPoints);
  if(!Weigths) return 0; 
 
  for(i=0;i<NoPoints;i++)
    {
      //      Weigths[i] = 1.0;
      if(areaEst != 0.0)
	Weigths[i] = ar[indexpfin[i]] / areaEst; 
      else 
	{
	  if(ar) freevf(ar); 
	  if(indexpfin) freevi(indexpfin); 
	  if(Weigths) freevf(Weigths);
	  return 0;
	}
    }

  freevf(ar); freevi(indexpfin);  ar = 0; indexpfin = 0;
  
  T3dPoint Point_loc[500];
  T3dPoint Origzero; 
  Origzero.x = 0.0; Origzero.y = 0.0; Origzero.z = 0.0; 
  int testc;
 
  testc = ChangeLocalCoord(Point, NoPoints, Obj->TMassCenter[t], MeanNormal, Point_loc);
  if (testc == 0)
    {cp[0] = 0.0; cp[1] = 0.0; 
	if(Weigths) freevf(Weigths); 
	return 0;}

  testc = Estim2ndSurf(Point_loc, NoPoints, Weigths, Origzero, psurf);
  if (testc == 0)
    {cp[0] = 0.0; cp[1] = 0.0;
	if(Weigths) freevf(Weigths); 
    return 0;}

  EstimCPar(psurf, ckc);
  cp[0] = ckc[1]; cp[1] = ckc[0];

  freevf(Weigths); 
   
  return 1; 
}



int ChangeLocalCoord(T3dPoint *Point, int NoPoints, T3dPoint Orig, T3dPoint N, T3dPoint *Point_out)
{
 
  T3dPoint T1, T2, P; 
  float norm; 
  int i;

  if ( (fabs(N.x) <= fabs(N.y)) && (fabs(N.x) <= fabs(N.z)) )
    {
      norm = (float)sqrt((double)(N.y*N.y + N.z*N.z));
      if(norm < 1.0e-6)
	{return 0;}
      T1.x = 0.0; T1.y = -N.z/norm; T1.z = N.y/norm;
    }
  else if ( (fabs(N.y) <= fabs(N.x)) && (fabs(N.y) <= fabs(N.z)) )
    {
      norm = (float)sqrt((double)(N.x*N.x + N.z*N.z));
      if(norm < 1.0e-6)
	{return 0;}
      T1.x = -N.z/norm; T1.y = 0.0; T1.z = N.x/norm;
    }
  else if ( (fabs(N.z) <= fabs(N.x)) && (fabs(N.z) <= fabs(N.y)) )
   {
      norm = (float)sqrt((double)(N.y*N.y + N.x*N.x));
      if(norm < 1.0e-6)
	{return 0;}
      T1.x =  -N.y/norm; T1.y = N.x/norm; T1.z = 0.0;
   }
  else 
    {return 0;}
   

  T2.x = N.y * T1.z - N.z * T1.y; 
  T2.y = N.z * T1.x - N.x * T1.z; 
  T2.z = N.x * T1.y - N.y * T1.x; 

  float normZ = T2.x * T2.x + T2.y * T2.y + T2.z * T2.z; 
 
  for(i=0;i<NoPoints;i++)
    {
      P.x = Point[i].x - Orig.x; 
      P.y = Point[i].y - Orig.y; 
      P.z = Point[i].z - Orig.z; 
      Point_out[i].x = T1.x*P.x + T1.y*P.y + T1.z*P.z; 
      Point_out[i].y = T2.x*P.x + T2.y*P.y + T2.z*P.z;
      Point_out[i].z = N.x*P.x + N.y*P.y + N.z*P.z;
    }
  return 1; 
}




int Estim2ndSurf(T3dPoint *Point, int NoPoints, float *Weigths, T3dPoint Orig, float *a)
{
  T3dPoint *P = NULL;
  P = (T3dPoint *)malloc(NoPoints * sizeof(T3dPoint)); 
  if(P == NULL)
    {fprintf(stderr, "not enough memory P\n"); return 0;}
  int i, j; 
  float b[6], bc[6], Cv[6];
  float **MC = alocamf(6, 6); 
  if(!MC) return 0; 
  float **MCc = alocamf(6, 6);
  if(!MCc) return 0; 

  for(i=0;i<NoPoints;i++)
    {
      P[i].x = Point[i].x - Orig.x;
      P[i].y = Point[i].y - Orig.y;
      P[i].z = Point[i].z - Orig.z;
    }

  InitZero(MC, 6, 6);
  InitZero(b, 6); 
  
  for(i=0;i<NoPoints;i++)
    {
      Cv[0] = P[i].x * P[i].x;
      Cv[1] = P[i].y * P[i].y; 
      Cv[2] = P[i].x * P[i].y;
      Cv[3] = P[i].x;
      Cv[4] = P[i].y; 
      Cv[5] = 1.0;
      for(j=0;j<6;j++)
	bc[j] =  Cv[j] *  P[i].z;
      
      ProduitTensoriel(Cv, Cv, 6, MCc);
      MultMatrixScalar(MCc, Weigths[i], 6, 6); 
      IncMatrix(MC, MCc, 6, 6);
      MultVectorScalar(bc, Weigths[i], 6); 
      IncVector(b, bc, 6);
    }
  
  /*pinv*/
  double **v0 = alocamd(6, 6); 
  if(!v0) return 0; 
  double **MC0 = alocamd(6, 6);
  if(!MC0) return 0; 
  double **MC0d = alocamd(6, 6); 
  if(!MC0d) return 0; 
  double **v0d = alocamd(6, 6); 
  if(!v0d) return 0; 
  double *w0 = alocavd(6); 
  if(!w0) return 0; 
  double *b0 = alocavd(6); 
  if(!b0) return 0; 
  double *x0 = alocavd(6);
  if(!x0) return 0; 
  double *w0d = alocavd(6);
  if(!w0d) return 0; 

  for(i=0;i<6;i++)
    b0[i] = (double)b[i];  
  
  for(i=0;i<6;i++)    
    for(j=0;j<6;j++)      
      {MC0[i][j] = (double)MC[i][j]; MC0d[i][j] = (double)MC[i][j];}


  int testpinv = pinv(MC0d, 6, 6, w0d, v0d, b0, x0);
  if(testpinv == 0)
	{
		freevd(w0d); freevd(x0); freevd(b0); freevd(w0);
		dalocd(v0d, 6); dalocd(MC0d, 6); 
		dalocd(MC0, 6); dalocd(v0, 6); 
		dalocf(MC, 6); dalocf(MCc, 6);
		if(P) free(P);
		return 0;
	}
  
  for(i=0;i<6;i++)
    a[i] = x0[i];
  
  freevd(w0d); freevd(x0); freevd(b0); freevd(w0);
  dalocd(v0d, 6); dalocd(MC0d, 6); 
  dalocd(MC0, 6); dalocd(v0, 6); 
   /*FIN pinv*/

  dalocf(MC, 6); dalocf(MCc, 6);
  if(P)
    free(P);
  
  return 1; 
}



void EstimCPar(float *a, float *CourbPrinc)
{
  float NA, NA2, NA3; 
  float EFG[2][2], efg[2][2], aa[2][2]; 
  float det, b, c, l1, l2; 

  EFG[0][0] = 1.0 + a[3]*a[3];
  EFG[0][1] = a[3]*a[4];
  EFG[1][0] = EFG[0][1]; 
  EFG[1][1] = 1.0 + a[4]*a[4];

  NA = sqrt( (double)(1.0 + a[3]*a[3] + a[4]*a[4]) );
  NA2 = NA*NA; NA3 = NA2*NA; 

  efg[0][0] = 2.0*a[0];
  efg[0][1] = a[2];
  efg[1][0] = efg[0][1];
  efg[1][1] = 2.0*a[1];  
  

  aa[0][0] = -(efg[0][1] * EFG[0][1] - efg[0][0] * EFG[1][1])/NA3;
  aa[0][1] = -(efg[0][0] * EFG[0][1] - efg[0][1] * EFG[0][0])/NA3;
  aa[1][0] = -(efg[1][1] * EFG[0][1] - efg[0][1] * EFG[1][1])/NA3;
  aa[1][1] = -(efg[0][1] * EFG[0][1] - efg[1][1] * EFG[0][0])/NA3;
  
  b =  (aa[0][0] + aa[1][1]);
  c = (aa[0][0]*aa[1][1] - aa[0][1]*aa[1][0]); 
  det = b*b - 4.0*c;
  
  if(det<0.0)
    if(fabs(det) > 1.0e-4)
	{return ;}
  
  l1 = b + sqrt((double)(det)); 
  l2 = b - sqrt((double)(det));

  CourbPrinc[0] = l2; CourbPrinc[1] = l1;
  
}




inline void ProduitTensoriel(float *Cv1, float *Cv2, int n, float **M)
{
  int i, j;
  for(i=0;i<n;i++)
    for(j=0;j<n;j++)
      M[i][j] = Cv1[i]*Cv2[j];

}

inline void  MultMatrixScalar(float **M, float w, int m, int n)
{ 
  int i, j;
  for(i=0;i<m;i++)
    for(j=0;j<n;j++)
      M[i][j] *= w;
}

inline void MultVectorScalar(float *v, float w, int n)
{
  int i; 
  for(i=0;i<n;i++)
    v[i] *= w; 
}

inline void IncMatrix(float **M, float **Mdelta, int m, int n)
{
  int i, j;
  for(i=0;i<m;i++)
    for(j=0;j<n;j++)
      M[i][j] += Mdelta[i][j];
}


inline void IncVector(float *b, float *bdelta, int n)
{
  int i; 
  for(i=0;i<n;i++)
    b[i] += bdelta[i]; 
}

inline void  InitZero(float **M, int m, int n)
{
  int i, j;
  for(i=0;i<m;i++)
    for(j=0;j<n;j++)
      M[i][j] = 0.0; 
}

inline void  InitZero(float *b, int n)
{
  int i; 
  for(i=0;i<n;i++)
    b[i] =  0.0; 
}
  


int vcomparPoint(T3dPoint *x,  T3dPoint *y)
{
  T3dPoint p1, p2; 
  p1 = (*x); p2 = (*y);

  if(p1.x < p2.x) return -1; 
  if(p1.x > p2.x) return 1; 
  
  if(p1.y < p2.y) return -1; 
  if(p1.y > p2.y) return 1; 
  
  if(p1.z < p2.z) return -1; 
  if(p1.z > p2.z) return 1; 
  
  return 0;
}

int vicompar(int *x, int *y)
{
  int xi, yi;
  
  xi = (*x); yi = (*y);

  if(xi<yi) return -1;
  else if (xi>yi) return 1;
  return 0;
}



int Quantif(double *d, int NoBinsdb, int nbits, unsigned long *ulss)
{
  int i, index; 
  double delta = 1.0; double nbf = (double)nbits;  double indexd; 

  nbf = pow(2.0, nbf);

  delta = 1.0/nbf;
  for(i=0;i<NoBinsdb; i++)
    {
      index = (int)(d[i]/delta);
      ulss[i] = (unsigned long)index;
      indexd = (double)index;
      d[i] = (indexd + 0.5)*delta;
    }
  return 1; 
}


int pinv(double **a, int m, int n, double *w, double **v, double *b, double *sol)
{  
  int check, i, it, j, jj, k, l, lim;
  double norm_a, c, f, g, h, som, ech, x, y, z;
  double *rvec = 0;
 
  rvec = (double *)malloc(n*sizeof(double)); 
  if(!rvec) return 0; 
  g = ech = norm_a = 0.0;
  for (i=0;i<n;i++) {
    l = i+1; rvec[i] = ech*g;
    g = som = ech = 0.0;
    if (i < m) {
      for (k=i;k<m;k++) ech += fabs(a[k][i]);
      if (ech) {
	for (k=i;k<m;k++) 
	  {a[k][i] /= ech; som += a[k][i]*a[k][i];}
	f = a[i][i];
	g = -SIGN(sqrt(som),f);
	h = f*g-som;
	a[i][i] = f-g;
	for (j=l;j<n;j++) {
	  som = 0.0;
	  for (k=i;k<m;k++) 
	    som += a[k][i]*a[k][j];
	  f = som/h;
	  for (k=i;k<m;k++) 
	    a[k][j] += f*a[k][i];
	}
	for (k=i;k<m;k++) a[k][i] *= ech;
      }
    }
    w[i] = ech *g;
    g = som = ech = 0.0;
    if (i < m && i != (n-1)) {
      for (k=l;k<n;k++) ech += fabs(a[i][k]);
      if (ech) {
	for (k=l;k<n;k++) {
	  a[i][k] /= ech;
	  som += a[i][k]*a[i][k];
	}
	f = a[i][l];
	g = -SIGN(sqrt(som),f);
	h = f*g-som;
	a[i][l] = f-g;
	for (k=l;k<n;k++) rvec[k] = a[i][k]/h;
	for (j=l;j<m;j++) {
	  som = 0.0;
	  for (k=l;k<n;k++) 
	    som += a[j][k]*a[i][k];
	  for (k=l;k<n;k++) 
	    a[j][k] += som*rvec[k];
	}
	for (k=l;k<n;k++) a[i][k] *= ech;
      }
    }
    norm_a = MAX(norm_a,(fabs(w[i])+fabs(rvec[i])));
  }
  for (i=(n-1);i>=0;i--) {
    if (i < (n-1)) {
      if (g) {
	for (j=l;j<n;j++)
	  v[j][i] = (a[i][j]/a[i][l])/g;
	for (j=l;j<n;j++) {
	  som = 0.0;
	  for (k=l;k<n;k++) 
	    som += a[i][k]*v[k][j];
	  for (k=l;k<n;k++) 
	    v[k][j] += som*v[k][i];
	}
      }
      
      for (j=l;j<n;j++) v[i][j] = v[j][i] = 0.0;
    }
    v[i][i] = 1.0; g = rvec[i]; l = i;
  }

  for(i=(MIN(m,n)-1);i>=0;i--) {
    l = i+1;
    g = w[i];
    for (j=l;j<n;j++) a[i][j] = 0.0;
    if (g) {
      g = 1.0/g;
      for (j=l;j<n;j++) {
	som = 0.0;
	for(k=l;k<m;k++) som += a[k][i]*a[k][j];
	f = (som/a[i][i])*g;
	for(k=i;k<m;k++) a[k][j] += f*a[k][i];
      }
      for(j=i;j<m;j++) a[j][i] *= g;
    } else for (j=i;j<m;j++) a[j][i]=0.0;
    ++a[i][i];
  }
  for(k=(n-1);k>=0;k--) {
    for(it=0;it<30;it++) {
      check = 1;
      for (l=k;l>=0;l--) {
	lim = l-1;
	if ((double)(fabs(rvec[l])+norm_a) == norm_a) {
	  check = 0;
	  break;
	}
	if ((double)(fabs(w[lim])+norm_a) == norm_a) break;
      }
      if (check) {
	c = 0.0; som = 1.0;
	for (i=l;i<=k;i++) {
	  f = som*rvec[i];
	  rvec[i] = c*rvec[i];
	  if ((double)(fabs(f)+norm_a) == norm_a) break;
	  g = w[i]; h = pythag(f,g);
	  w[i] = h; h = 1.0/h; c = g*h; som = -f*h;
	  for (j=0;j<m;j++) {
	    y = a[j][lim]; z = a[j][i];
	    a[j][lim] = y*c + z*som;
	    a[j][i] = z*c - y*som;
	  }
	}
      }
      z = w[k];
      if (l == k) {
	if (z < 0.0) {
	  w[k] = -z;
	  for (j=0;j<n;j++) 
	    v[j][k] = -v[j][k];
	}
	break;
      }
      if (it == 29) 			 
		{if(rvec) free(rvec); rvec = 0; return 0;}
	
      x = w[l]; lim = k-1; y = w[lim]; g = rvec[lim]; h = rvec[k];
      f = ((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
      g = pythag(f,1.0);
      f = ((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
      c = som = 1.0;
      for (j=l;j<=lim;j++) {
	i = j+1; g = rvec[i]; y = w[i]; h = som*g; g = c*g;
	z = pythag(f,h); rvec[j] = z;
	c = f/z; som = h/z; f = x*c + g*som; g = g*c - x*som; 
	h = y*som; y *= c; 
	for (jj=0;jj<n;jj++) { 
	  x = v[jj][j]; z = v[jj][i]; 
	  v[jj][j] = x*c + z*som; 
	  v[jj][i] = z*c - x*som; 
	} 
	z = pythag(f,h); 
	w[j] = z; 
	if(z) 
	  {z = 1.0/z; c = f*z; som = h*z;} 
	f = c*g+som*y; x = c*y-som*g; 
	for (jj=0;jj<m;jj++) { 
	  y = a[jj][j]; z = a[jj][i]; 
	  a[jj][j] = y*c + z*som; 
	  a[jj][i] = z*c - y*som; 
	} 
      } 
      rvec[l] = 0.0; rvec[k] = f; w[k] = x; 
    } 
  }

  for (j=0;j<n;j++) 
    { 
      som = 0.0; 
      if(w[j]) { 
	for(i=0;i<m;i++) 
	  som += a[i][j]*b[i]; 
	som /= w[j];
      } 
      rvec[j] = som; 
    }
 
  for(j=0;j<n;j++) 
    { 
      som = 0.0; 
      for(i=0;i<n;i++) 
	som += v[j][i]*rvec[i]; 
      sol[j]=som; 
    }   

  if(rvec)
    free(rvec);
 
  return 1; 
}


inline double pythag(double a, double b) 
{ 
	double aa,ab; 
	aa = fabs(a); ab = fabs(b); 
	if (aa > ab) return aa*sqrt(1.0 + (ab/aa)*(ab/aa)); 
	else return (ab == 0.0 ? 0.0 : ab*sqrt(1.0 + (aa/ab)*(aa/ab))); 
}


double *alocavd(unsigned int n)
{
  double *b = 0;
  b=(double *)malloc((size_t)(n*sizeof(double)));
  if (b==NULL)
    {
      printf("not enough memory");
      return 0;
    }
  
  return b; 
}


double ** alocamd(unsigned int dx, unsigned int dy)
{
  double **b=0;unsigned int i;
  
  b=(double **)malloc((size_t)(dx*sizeof(double *)));
  if (b==NULL)
    {
      printf("not enough memory");
      return 0;
    }
  for(i=0;i<dx;i++)
    {
      b[i] = 0;
      b[i]=(double *)malloc((size_t)(dy*sizeof(double)));
      if (b[i]==NULL)
	{
	  printf("not enough memory");
	  return 0;
	}
    }
  return b;
}

float *alocavf(unsigned int n)
{
  float *b = 0;
  b=(float *)malloc((size_t)(n*sizeof(float)));
  if (b==NULL)
    {
      printf("not enough memory");
      return 0;
    }
  return b; 
}



float ** alocamf(unsigned int dx,unsigned int dy)

{
 float **b=0;unsigned int i;
 b=(float **)malloc((size_t)(dx*sizeof(float *)));
 if (b==NULL)
   {
     printf("not enough memory");
     return 0; 
   }
 
 for(i=0;i<dx;i++)
   {
     b[i]=0;
     b[i]=(float *)malloc((size_t)(dy*sizeof(float)));
     if (b[i]==NULL)
       {
	 printf("not enough memory");
	 return 0; 
       }
   }
 return b;
}


int *alocavi(unsigned int n)
{
  int *b = 0;
  b=(int *)malloc((size_t)(n*sizeof(int)));
  if (b==NULL)
    {
      printf("not enough memory");
      return 0; 
    }
  return b; 
}


void freevd(double *v)
{
  if(v)
    free((double *) v);
}

void freevi(int *v)
{
  if(v)
    free((int *) v);
}

void freevf(float *v)
{
  if(v)
    free((float *) v);
}

void dalocd(double ** m,unsigned int dx)
{
 unsigned int i;
 if(m)
   {
     for(i=0;i<dx;i++)
       if(m[i])
	 free((double *)m[i]);
     free((double **)m);
   }
}


void dalocf(float ** m,unsigned int dx)
{
 unsigned int i;
 if(m)
   {
     for(i=0;i<dx;i++)
       if(m[i])
	 free((float *)m[i]);
     free((float **)m);
   }
}
