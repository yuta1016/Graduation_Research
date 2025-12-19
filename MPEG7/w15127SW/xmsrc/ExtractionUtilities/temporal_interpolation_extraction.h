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
// Applicable File Name:  temporal_inter_extraction.h

#ifndef __temporal_inter_extraction__
#define __temporal_inter_extraction__

#ifndef BOOL
#define BOOL int
#define FALSE 0
#define TRUE 1
#endif

#define MAX_LENGTH	500

typedef struct SSplineFunction {
	double	Start;
	double	End;
	long	Degree;
	double*	pCoeff;
	double	Error;
} SplineFunction;

typedef struct SSplineStatistics {
	long	Knots;
	double	MaxError;
	double	AveError;
	double	AveLength;
	long	Points;
	long	MaxDegree;
	double	AcceptError;
} SplineStatistics;


#define INTERP_SEQUENTIAL	0
#define INTERP_FIXLENGTH	1
#define INTERP_FIXNKEYP		2
#define INTERP_DIVISION		3

#define BUF_UNIT		1000

typedef struct _SBUFFER {
	int Size;
	double* pTime;
	double** ppData;
} SBUFFER;


//=============================================================================
class CSpline
{
private:
	long m_Degree;
	double m_MaxError;
	long m_MaxLength;
	double* m_pY;
	long m_NPoint;
	double* m_pX;
	SplineFunction** m_SplineList;
	int m_SplineListSize;
	double m_MinX;
	double m_MaxX;
	long m_TotalPoints;
	BOOL m_LimitRegion;
	double m_MinValue;
	double m_MaxValue;
	
public:
	CSpline();
	~CSpline();
	int AddPoint(double X,double Y);
	BOOL AddKnots();
	BOOL GetPoint(double X,double* pY);
	BOOL GetPolynomial(int m,double* pStartX,double* pEncX,double* pCoeff);
	int GetNKnots();
	BOOL LimitRegion(double Minimum,double Maximum);
	BOOL SetParam(long Degree,double MaxError,long MaxLength);
	BOOL GetParam(long* pDegree,double* pMaxError,long* pMaxLength);
	BOOL Clear();
	SplineFunction** GetSplineList()	{ return m_SplineList; }
	BOOL GetInterval(double* pMin,double* pMax);
	BOOL GetStatistics(SplineStatistics* pStat);
	BOOL GetFunctionInfo(int m,double* pStartX,double* pEndX,double* pStartY,double* pEndY,double* pA);
private:
	BOOL LeastSquareEstimate(double* pCoeff);
	double EstimateError(double* pCoeff);
	double CheckRegion(double* pCoeff);

};


//=============================================================================
class CInterp {
public:
	CInterp();
	~CInterp();
	BOOL Initialize(int Mode,int Dimension,int* pDegree,double* pMaxError,int NKeyPoints,int IntervalLength,int MaxLength);
	BOOL AddData(double Time,double* pData);
	BOOL DataEnd();
	int GetNKeyPoints();
	BOOL GetInterpFunc(int m,double* pStartT,double* pEndT,double* pStartX,double* pEndX,double* pCoeff);
	BOOL Reset();
private:
	int m_Mode;
	int m_Dimension;
	int m_NData;
	BOOL m_Initialized;
	BOOL m_InterpCalculated;
	CSpline* m_pSpline;
	SBUFFER m_Buffer;
	int m_TargetNKeyPoints;
	int m_TargetLength;
};

#endif  // __temporal_inter_extraction__
