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
// Applicable File Name:  temporal_inter_extraction.cpp

#include "temporal_interpolation_extraction.h"

#include <math.h>
#include <stdlib.h> 

//=============================================================================
CSpline::CSpline()
{
	m_Degree = 2;
	m_LimitRegion = FALSE;
	m_MaxError = 10.0;
	m_MinX = 0.0;
	m_MaxX = 0.0;
	m_MaxLength = MAX_LENGTH;
	m_SplineListSize = 0;

	m_pY = new double[m_MaxLength];
	m_NPoint = 0;
	m_pX = new double[m_MaxLength];
	m_SplineList = new SplineFunction*[m_MaxLength];

	m_TotalPoints = 0;
}

//----------------------------------------------------------------------------
CSpline::~CSpline()
{
	Clear();
	delete []m_pY;
	delete []m_pX;
	delete [] m_SplineList;
}

//----------------------------------------------------------------------------
int CSpline::AddPoint(double X,double Y)
{
	int i;
	int pos;
	double *pCoeff, Error;
	SplineFunction* pSpline;
	int SplitFlag;

	if( m_SplineListSize == 0 ){	// Initialize

		pSpline = new SplineFunction;
		pSpline->Start = pSpline->End = X;
		pSpline->Degree = m_Degree;
		pSpline->pCoeff = new double[m_Degree+1];
		for( i=1; i<=m_Degree; i++ ) (pSpline->pCoeff)[i] = 0.0;
		(pSpline->pCoeff)[0] = Y;
		pSpline->Error = 0.0;
		m_SplineList[m_SplineListSize] = pSpline ; m_SplineListSize++;
		m_pY[m_NPoint] = Y;
		m_pX[m_NPoint] = X;
		m_NPoint ++;
		m_MinX = m_MaxX = X;
		
	} else {	// Update

		m_pY[m_NPoint] = Y;
		m_pX[m_NPoint] = X;
		m_NPoint ++;
		m_MaxX = X;

		pCoeff = new double[m_Degree+1];
		LeastSquareEstimate( pCoeff );
		Error = EstimateError( pCoeff );

		if( m_MaxError < 0.0 || m_NPoint <= m_Degree+1 ){
			SplitFlag = 0;
		} else {
			if( Error > m_MaxError ) SplitFlag = 1;
			else if( m_NPoint >= m_MaxLength ) SplitFlag = 1;
			else if( CheckRegion( pCoeff ) == FALSE ) SplitFlag = 1;
			else SplitFlag = 0;
		}

		if( SplitFlag == FALSE ){	// Update Spline Function

			pos = m_SplineListSize-1;
			pSpline = m_SplineList[pos];
			pSpline->End = X;
			for( i=0; i<=m_Degree; i++ ) (pSpline->pCoeff)[i] = pCoeff[i];
			pSpline->Error = Error;

		} else {	// Split Region
			for( i=0; i<2; i++ ){
				m_pY[i] = m_pY[m_NPoint-2+i];
				m_pX[i] = m_pX[m_NPoint-2+i];
			}
			m_NPoint = 2;

			pSpline = new SplineFunction;
			pSpline->Start = m_pX[0];
			pSpline->End = m_pX[1];
			pSpline->Degree = m_Degree;

			pSpline->pCoeff = new double[m_Degree+1];
			LeastSquareEstimate( pSpline->pCoeff );
			Error = EstimateError( pSpline->pCoeff );
			pSpline->Error = Error;
			m_SplineList[m_SplineListSize] = pSpline;  m_SplineListSize++;

			SplitFlag = 1;

		}
		delete []pCoeff;

		if (m_MaxX <= X) { m_MaxX = X; }
		m_TotalPoints ++;

	}

	return SplitFlag;
}

//----------------------------------------------------------------------------
BOOL CSpline::GetPoint(double X,double* pY)
{
	int j;
	double Estimate;
	int pos;
	SplineFunction* pSpline;

	if( m_SplineListSize == 0 ) return FALSE;

	if( X < m_MinX || m_MaxX < X ) return FALSE;

	pos = 0;
	while( 1 ){
		if (pos >= m_SplineListSize) { break; }
		pSpline = m_SplineList[pos];  pos++;
		if( pSpline->Start <= X && X <= pSpline->End ){
			Estimate = (pSpline->pCoeff)[pSpline->Degree];
			for( j=pSpline->Degree-1; j>=0; j-- ){
				Estimate *= X;
				Estimate += (pSpline->pCoeff)[j];
			}
			*pY = Estimate;
			return TRUE;
		}
	}

	return FALSE;
}

//----------------------------------------------------------------------------
BOOL CSpline::GetPolynomial(int m,double* pStartX,double* pEndX,double* pCoeff)
{
	int i;
	int pos;
	SplineFunction* pSpline;

	if( m_SplineListSize == 0 ) return FALSE;

	pos = m;
	if( pos ){
		pSpline = m_SplineList[pos];
		*pStartX = pSpline->Start;
		*pEndX = pSpline->End;
		for( i=0; i<pSpline->Degree+1; i++ ) pCoeff[i] = (pSpline->pCoeff)[i];
	} else {
		return FALSE;
	}

	return TRUE;
}

//----------------------------------------------------------------------------
BOOL CSpline::GetFunctionInfo(int m,double* pStartX,double* pEndX,double* pStartY,double* pEndY,double* pA)
{
	int pos;
	SplineFunction* pSpline;

	if( m_SplineListSize == 0 ) return FALSE;

	pos = m;
	if(m < m_SplineListSize){
		pSpline = m_SplineList[pos];
		*pStartX = pSpline->Start;
		*pEndX = pSpline->End;
		GetPoint( *pStartX, pStartY );
		GetPoint( *pEndX, pEndY );
		*pA = (pSpline->pCoeff)[2]*2.0;
	} else {
		return FALSE;
	}

	return TRUE;
}

//----------------------------------------------------------------------------
int CSpline::GetNKnots()
{
	int N;

	N = m_SplineListSize;
	if( N == 0 ) return 0;
	else return N+1;
}

//----------------------------------------------------------------------------
BOOL CSpline::SetParam(long Degree,double MaxError,long MaxLength)
{
	if( m_Degree < 1 || 2 < m_Degree ) return FALSE;
	if( MaxLength < 1 ) return FALSE;

	m_Degree = Degree;
	m_MaxError = MaxError;
	if( MaxLength != m_MaxLength ){
		delete []m_pY;
		m_MaxLength = MaxLength;
		m_pY = new double[m_MaxLength];
	}

	return TRUE;
}

//----------------------------------------------------------------------------
BOOL CSpline::LimitRegion(double Minimum,double Maximum)
{
	if( Maximum < Minimum ) return FALSE;

	m_MinValue = Minimum;
	m_MaxValue = Maximum;
	m_LimitRegion = TRUE;
	return TRUE;
}

//----------------------------------------------------------------------------
BOOL CSpline::GetParam(long* pDegree,double* pMaxError,long* pMaxLength)
{
	*pDegree = m_Degree;
	*pMaxError = m_MaxError;
	*pMaxLength = m_MaxLength;

	return TRUE;
}

//----------------------------------------------------------------------------
BOOL CSpline::Clear()
{
	int pos;
	SplineFunction* pSpline;

	pos = 0;
	while( 1 ){
		if (pos >= m_SplineListSize) { break; }
		pSpline = m_SplineList[pos];  pos++;
		delete [](pSpline->pCoeff);
		delete pSpline;
	}
	m_SplineListSize = 0;

	m_NPoint = 0;
	m_TotalPoints = 0;
	m_LimitRegion = FALSE;

	return TRUE;
}

//----------------------------------------------------------------------------
BOOL CSpline::GetInterval(double* pMin,double* pMax)
{
	*pMin = m_MinX;
	*pMax = m_MaxX;

	if( m_TotalPoints == 0 ) return FALSE;
	else return TRUE;
}

//----------------------------------------------------------------------------
BOOL CSpline::GetStatistics( SplineStatistics* pStat )
{
	int pos;
	SplineFunction* pSpline;
	double Knots, MaxError, TotalError, TotalLength;

	pStat->Points = m_TotalPoints;
	pStat->MaxDegree = m_Degree;
	pStat->AcceptError = m_MaxError;

	if( m_SplineListSize == 0 ){
		pStat->Knots = 0;
		pStat->AveLength = pStat->AveError = pStat->MaxError = 0.0;
		return FALSE;
	}

	Knots = 0;
	MaxError = TotalError = TotalLength = 0;

	pos = 0;
	while( 1 ){
		if (pos >= m_SplineListSize) { break; }
		pSpline = m_SplineList[pos];   pos++;
		Knots ++;
		TotalLength += pSpline->End-pSpline->Start;
		TotalError += pSpline->Error;
		if (pSpline->Error >= MaxError) { MaxError = pSpline->Error; }
	}

	pStat->Knots = (long)Knots;
	pStat->AveLength = TotalLength/Knots;
	pStat->AveError = TotalError/Knots;
	pStat->MaxError = MaxError;

	return TRUE;
}

//----------------------------------------------------------------------------
BOOL CSpline::LeastSquareEstimate( double* pCoeff )
{
	int i;
	double a, ta, tb, fa, fb, Sab, Saab, Saabb, Sfab, v;
	
	if( m_NPoint < 2 ) return FALSE;
	
	fa = m_pY[0]; fb = m_pY[m_NPoint-1];
	ta = m_pX[0]; tb = m_pX[m_NPoint-1];
	
	switch( m_Degree ){
	case 1:
		pCoeff[0] = (fa*tb-fb*ta)/(tb-ta);
		pCoeff[1] = (fb-fa)/(tb-ta);
		break;
	case 2:
		if( m_NPoint > 2 ){
			Sab = Saab = Saabb = Sfab = 0.0;
			for( i=1; i<m_NPoint-1; i++ ){
				v = (m_pX[i]-ta)*(m_pX[i]-tb);
				Sab += v;
				Saab += v*(m_pX[i]-ta);
				Saabb += v*v;
				Sfab += v*m_pY[i];
			}			
			a = 2.0*(Sfab-fa*Sab-(fb-fa)/(tb-ta)*Saab)/Saabb;
		} else {
			a = 0.0;
		}
		pCoeff[0] = (fa*tb-fb*ta)/(tb-ta)+ta*tb*a/2.0;
		pCoeff[1] = (fb-fa)/(tb-ta)-(ta+tb)*a/2.0;
		pCoeff[2] = a/2.0;
		break;
	default:
		return FALSE;
	}
	
	
	return TRUE;
}

//----------------------------------------------------------------------------
double CSpline::EstimateError(double* pCoeff)
{
	int i, j;
	double Error, MaxError;
	double Estimate;
	double X;
	
	MaxError = 0.0;
	for( i=0; i<m_NPoint; i++ ){
		
		X = m_pX[i];

		Estimate = pCoeff[m_Degree];
		for( j=m_Degree-1; j>=0; j-- ){
			Estimate *= X;
			Estimate += pCoeff[j];
		}

		Error = sqrt( (Estimate-m_pY[i])*(Estimate-m_pY[i]) );
		if (Error >= MaxError ) { MaxError = Error; }

	}

	return MaxError;
}

//----------------------------------------------------------------------------
double CSpline::CheckRegion(double* pCoeff)
{
	int i, j;
	double Estimate;
	double X;
	
	if( m_LimitRegion == FALSE ) return TRUE;

	for( i=0; i<m_NPoint; i++ ){
		
		X = m_pX[i];

		Estimate = pCoeff[m_Degree];
		for( j=m_Degree-1; j>=0; j-- ){
			Estimate *= X;
			Estimate += pCoeff[j];
		}

		if( Estimate < m_MinValue || Estimate > m_MaxValue ) return FALSE;

	}

	return TRUE;
}

//----------------------------------------------------------------------------
BOOL CSpline::AddKnots()
{
	int pos;
	int i;
	SplineFunction* pSpline;
	double *pCoeff, Error;
	
	if( m_NPoint <= m_Degree+1 ) return TRUE;
	
	m_NPoint --;
	
	pCoeff = new double[m_Degree+1];
	LeastSquareEstimate( pCoeff );
	Error = EstimateError( pCoeff );
	
	pos = m_SplineListSize-1;
	pSpline = m_SplineList[pos];
	pSpline->End = m_pX[m_NPoint-1];
	for( i=0; i<=m_Degree; i++ ) (pSpline->pCoeff)[i] = pCoeff[i];
	pSpline->Error = Error;
	delete []pCoeff;
	
	// new function
	for( i=0; i<2; i++ ){
		m_pY[i] = m_pY[m_NPoint-1+i];
		m_pX[i] = m_pX[m_NPoint-1+i];
	}
	m_NPoint = 2;
	
	pSpline = new SplineFunction;
	pSpline->Start = m_pX[0];
	pSpline->End = m_pX[1];
	pSpline->Degree = m_Degree;
	
	pSpline->pCoeff = new double[m_Degree+1];
	LeastSquareEstimate( pSpline->pCoeff );
	Error = EstimateError( pSpline->pCoeff );
	pSpline->Error = Error;
	m_SplineList[m_SplineListSize] = pSpline;  m_SplineListSize++;
	
	return TRUE;
}


//=============================================================================
CInterp::CInterp()
{
	m_Mode = INTERP_SEQUENTIAL;
	m_Dimension = 1;
	m_Initialized = m_InterpCalculated = FALSE;
	m_pSpline = NULL;
	m_NData = 0;
	m_Buffer.pTime = NULL;
	m_Buffer.ppData = NULL;
	m_Buffer.Size = 0;
}

//----------------------------------------------------------------------------
CInterp::~CInterp()
{
}

//----------------------------------------------------------------------------
BOOL CInterp::Initialize(int Mode,int Dimension,int* pDegree,double* pMaxError,int NKeyPoints,int IntervalLength,int MaxLength)
{
	int i;

	if( m_Initialized ) return FALSE;

	if( Mode != INTERP_SEQUENTIAL &&
		Mode != INTERP_FIXLENGTH &&
		Mode != INTERP_FIXNKEYP &&
		Mode != INTERP_DIVISION ){
		return FALSE;
	}
	m_Mode = Mode;

	if( Dimension <= 0 ) return FALSE;
	m_Dimension = Dimension;

	m_pSpline = new CSpline[m_Dimension];
	if( m_pSpline == NULL ) return FALSE;
	for( i=0; i<m_Dimension; i++ ) m_pSpline[i].Clear();

	if( m_Mode == INTERP_SEQUENTIAL ){

		for( i=0; i<m_Dimension; i++ ) m_pSpline[i].SetParam( pDegree[i], pMaxError[i], MaxLength );

	} else if( m_Mode == INTERP_FIXLENGTH ){

		m_TargetLength = IntervalLength;

		for( i=0; i<m_Dimension; i++ ) m_pSpline[i].SetParam( pDegree[i], -1.0, MaxLength );

	} else if( m_Mode == INTERP_FIXNKEYP || m_Mode == INTERP_DIVISION ){

		m_TargetNKeyPoints = NKeyPoints;

		m_Buffer.pTime = (double*)malloc(sizeof(double)*BUF_UNIT);
		m_Buffer.ppData = (double**)malloc(sizeof(double*)*m_Dimension);
		for( i=0; i<m_Dimension; i++ ) m_Buffer.ppData[i] = (double*)malloc(sizeof(double)*BUF_UNIT);
		m_Buffer.Size = BUF_UNIT;

		for( i=0; i<m_Dimension; i++ ) m_pSpline[i].SetParam( pDegree[i], -1.0, MaxLength );

	}

	m_Initialized = TRUE;

	return TRUE;
}

//----------------------------------------------------------------------------
BOOL CInterp::AddData(double Time,double* pData)
{
	int i, Split;

	if( !m_Initialized ) return FALSE;

	if( m_Mode == INTERP_SEQUENTIAL ){

		Split = 0;
		for( i=0; i<m_Dimension; i++ ) Split += m_pSpline[i].AddPoint( Time, pData[i] );
		if( Split ){
			for( i=0; i<m_Dimension; i++ ) m_pSpline[i].AddKnots();
		}
		m_NData ++;

		if( m_NData > 1 ) m_InterpCalculated = TRUE;

	} else if( m_Mode == INTERP_FIXLENGTH ){

		for( i=0; i<m_Dimension; i++ ) m_pSpline[i].AddPoint( Time, pData[i] );
		if( (m_NData+m_TargetLength-1)%m_TargetLength == 0 && m_NData > m_TargetLength ){
			for( i=0; i<m_Dimension; i++ ) m_pSpline[i].AddKnots();
		}
		m_NData ++;
	
		if( m_NData > 1 ) m_InterpCalculated = TRUE;

	} else if( m_Mode == INTERP_FIXNKEYP || m_Mode == INTERP_DIVISION ){

		if( m_NData >= m_Buffer.Size ){
			m_Buffer.Size += BUF_UNIT;
			m_Buffer.pTime = (double*)realloc( m_Buffer.pTime, sizeof(double)*m_Buffer.Size );
			for( i=0; i<m_Dimension; i++ ){
				m_Buffer.ppData[i] = (double*)realloc( m_Buffer.ppData[i], sizeof(double)*m_Buffer.Size );
			}
		}

		m_Buffer.pTime[m_NData] = Time;
		for( i=0; i<m_Dimension; i++ ) m_Buffer.ppData[i][m_NData] = pData[i];

		m_NData ++;

	}

	return TRUE;
}

//----------------------------------------------------------------------------
BOOL CInterp::DataEnd()
{
	double AverageLength, Interval;
	int i, j;

	if( !m_Initialized ) return FALSE;

	if( m_Mode == INTERP_SEQUENTIAL ){

	} else if( m_Mode == INTERP_FIXLENGTH ){

	} else if( m_Mode == INTERP_FIXNKEYP ){

		AverageLength = (double)(m_NData-1)/(double)m_TargetNKeyPoints;

		for( j=0; j<m_Dimension; j++ ){
			m_pSpline[j].AddPoint( m_Buffer.pTime[0], m_Buffer.ppData[j][0] );
		}
		Interval = 1.0;
		for( i=1; i<m_NData; i++ ){
			for( j=0; j<m_Dimension; j++ ){
				m_pSpline[j].AddPoint( m_Buffer.pTime[i], m_Buffer.ppData[j][i] );
			}
			if( (double)i > AverageLength*Interval ){
				for( j=0; j<m_Dimension; j++ ){
					m_pSpline[j].AddKnots();
				}
				Interval += 1.0;
			}
		}

		m_InterpCalculated = TRUE;

	} else if( m_Mode == INTERP_DIVISION ){


	}

	return TRUE;
}

//----------------------------------------------------------------------------
int CInterp::GetNKeyPoints()
{
	if( !m_Initialized ) return 0;

	return m_pSpline[0].GetNKnots();
}

//----------------------------------------------------------------------------
BOOL CInterp::GetInterpFunc(int m,double* pStartT,double* pEndT,double* pStartX,double* pEndX,double* pCoeff)
// Only second order coefficients are output whatever the value of Degree is.
{
	int i;

	if( !m_InterpCalculated ) return FALSE;

	for( i=0; i<m_Dimension; i++ ){
		m_pSpline[i].GetFunctionInfo( m, pStartT, pEndT, pStartX+i, pEndX+i, pCoeff+i );
		pCoeff[i] /= 2.0; //***************** new syntax (Geneva) *********************
	}
	
	return TRUE;
}

//----------------------------------------------------------------------------
BOOL CInterp::Reset()
{
	int i;

	if( m_pSpline != NULL ){
		for( i=0; i<m_Dimension; i++ ) m_pSpline[i].Clear();
		delete []m_pSpline;
	}
	if( m_Buffer.Size ){
		free( m_Buffer.pTime );
		for( i=0; i<m_Dimension; i++ ) free(m_Buffer.ppData[i]);
		free(m_Buffer.ppData);
	}

	m_Mode = INTERP_SEQUENTIAL;
	m_Dimension = 1;
	m_Initialized = m_InterpCalculated = FALSE;
	m_pSpline = NULL;
	m_NData = 0;
	m_Buffer.pTime = NULL;
	m_Buffer.ppData = NULL;
	m_Buffer.Size = 0;

	return TRUE;
}
