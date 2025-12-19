///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// K.Masukura (R&D Center Toshiba, Japan)
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
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  spatio_temporal_locator_extraction.cpp
//

// spatio_temporal_locator_extraction.cpp
//
//////////////////////////////////////////////////////////////////////

#ifndef __spatiotemporal_extraction__
#define __spatiotemporal_extraction__

#include "Descriptors/SpatioTemporalLocator.h" 

#define MAXSPRIT 30
#define MAXNVerticesTmp 1000


#define AFFINE_MODEL	1
#include "ExtractionUtilities/temporal_interpolation_extraction.h"

#define AREA_THRESHOLD     50
#define MAXNumData 1000 


typedef struct {
	int       frame;
	double    x;
	double    y;
} SPoint;


typedef struct {
	int       frame;
	double    x[4];
	double    y[4];
} SBox;


typedef struct {
	int       frame;
	int       area;
} SParams;

typedef struct {
   long left;
   long top;
   long right;
   long bottom;
} 	SRect;

//=============================================================================
class CPolygon {
public:
	int N_Frame;					// Max Frame

	int VSize;					// Size of Vertex
	int Frame;					// Refernce No.
	double X[MAXNVertices+1];		// List of X coordinate
	double Y[MAXNVertices+1];		// List of Y coordinate
};

//=============================================================================
class CPolygonTmp {
public:
	int VSize;					// Size of Vertex
	int x[MAXNVerticesTmp];
	int y[MAXNVerticesTmp];
	int TriSize[MAXNVerticesTmp];

};


//=============================================================================
class CPolygons {
protected:
	int NowSprit, NowFrame;


public:
	CPolygon m_Poly;

	CPolygons();
	~CPolygons();

	int rawW, rawH, MaxSprit, VerN; 
	int sf[MAXSPRIT], ef[MAXSPRIT], FP[MAXSPRIT];
	unsigned char *gBuf;

public:
	int MakeFpoly();
	int SearchEPoint(int &x, int &y);	
	int MakeNPoly(int n);				
	void CalcAllTri();					
	int CalcTriSize(int n);							
	int MinTriSize();					
	void RemoveVertex(int n);		
	void LMinShift(CPolygon &ipoly);
	double LSubSum(CPolygon &ipoly);
	void shift(int n);

	CPolygonTmp m_tPoly;
	int m_lastV;	


};


//=============================================================================
class CBoundingBox  
{
private:
	unsigned char*               m_pShape;
	int                          m_Width;
	int                          m_Height;
	int                          m_Frame;

	int                          m_ObjectID;  
	int                          m_Area;       
	SPoint                       m_Centroid;  
	SPoint                       m_PrevCentroid;
	SRect                        m_Region;    
	SBox                         m_BoundingBox; 
	SBox                         m_PrevBoundingBox;
	int           m_Num;        
	SBox          m_Array[MAXNumData];     
	SParams       m_ParamsArray[MAXNumData];   

	void GetBoundingBox(double la, double lb, double sa, double sb);
	void CalcSimultaneousEquations(double a1, double b1, double a2, double b2,
								   double* x, double* y);
	void Matching(SPoint* pPrevCentroid, SBox* pPrevBoundingBox,
		          SPoint* pCurrCentroid, SBox* pCurrBoundingBox);
	double GetArea(SBox* pBox);

public:
	CBoundingBox();
	virtual ~CBoundingBox();

	int Calc(unsigned char* pShape, int width, int height, int frame,
		       int area, SPoint& centroid, SRect& region);

	inline SBox GetBoundingBox() { return m_BoundingBox; }
};

//=============================================================================
class CCentroid  
{
private:
	unsigned char*             m_pShape;
	int                        m_Width;
	int                        m_Height;
	int                        m_Frame;

	int                        m_ObjectID; 
	int                        m_Area;    
	SPoint                     m_Centroid; 
	SRect			           m_Region;   
	int				 m_Num;    
	SPoint			 m_Array[MAXNumData];   
	SRect			 m_RegionArray[MAXNumData];


public:
	CCentroid();
	virtual ~CCentroid();

	int Calc(unsigned char* pShape, int width, int height, int frame);

	inline int GetArea() { return m_Area; }
	inline SPoint GetCentroid() { return m_Centroid; }
	inline SRect GetRegion() { return m_Region; }
};

//=============================================================================
class CExtractMP
{
public:
	CExtractMP();
	~CExtractMP();
	int  Initialize(unsigned char* pShape,int Width,int Height,int Model, int frame);
	int  GetInitRegion(double* pRegionX,double* pRegionY);
	int  ExtractMotionParameters(double* pParam, unsigned char*pShape, int frame);
	int  CalcAffine(int vcnt, double *bx0,double *by0, double *bx1, double *by1,double *affine);

	int                        m_Width;
	int                        m_Height;
	int                        m_Frame;
	int                        m_Model;
	unsigned char*             m_pShape;
	CBoundingBox               m_BoundingBox;
	CCentroid                  m_Centroid;
	double                    m_InitRegionX[4];
	double                    m_InitRegionY[4];
};


#endif // __spatiotemporal_extraction__
