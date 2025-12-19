///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// K.Masukura (R&D Center Toshiba, Japan)
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
// Applicable File Name:  spatio_temporal_locator_extraction.cpp
//

// ParameterTrajectoryEx
//
//////////////////////////////////////////////////////////////////////
#include "spatio_temporal_locator_extraction.h"

#if defined(__cplusplus) || defined(c_plusplus)
#undef class
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#if defined (WIN32)
        #include "io.h"
#endif

#ifndef FALSE
#define FALSE 0
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

//=============================================================================
CPolygons::CPolygons()
{
	m_Poly.N_Frame = 0;
	m_Poly.VSize = 0;
	gBuf = 0;
}

//----------------------------------------------------------------------------
CPolygons::~CPolygons()
{
}

//----------------------------------------------------------------------------
int CPolygons::MakeFpoly()
{
	int x,y, vec = 0;					
	int xf=0, yf=0;
	int n=0;

	if (SearchEPoint(xf, yf) == 0) {return 0; }
	m_tPoly.VSize = 0;
	
	if ( (yf+1<rawH) && (gBuf[xf+ (yf+1)*rawW ] == 0xff) ) { vec = 1; }
	else if ( (xf+1<rawW) && (gBuf[(xf+1) +yf*rawW] == 0xff) ) { vec = 0; }
	else if ( (yf-1>=0) && (gBuf[xf +(yf-1)*rawW] == 0xff) ) { vec = 3; }
	else if ( (xf-1>=0) && (gBuf[(xf-1) +yf*rawW] == 0xff) ) { vec = 2; }
	else return 0;
	x = xf;  y = yf;
	m_tPoly.x[n] = x;	
	m_tPoly.y[n] = y;	
	n++;

	int i;
	for (i=0; i<1000-1; i++) {


		if (vec == 0) {		
			if ((yf-1>=0) && (gBuf[xf +(yf-1)*rawW] != 0xff)) { yf--; }
			else { vec = 3; }
		} else if (vec == 1) {   
			if ((xf+1<rawW) && (gBuf[(xf+1) +yf*rawW] != 0xff)) { xf++; }
			else { vec = 0; }
		} else if (vec == 2) {  
			if ((yf+1<rawH) && (gBuf[xf+ (yf+1)*rawW ] != 0xff)) { yf++; }
			else { vec = 1; }
		} else if (vec == 3) {  
			if ((xf-1>=0) && (gBuf[(xf-1) +yf*rawW] != 0xff)) { xf--; }
			else { vec = 2; }
		} 
		
		if (vec == 0) {		
			if ((xf+1<rawW) && (gBuf[(xf+1) +yf*rawW] != 0xff)) { vec = 1; xf++;} 
			x = xf;  y = yf;
			m_tPoly.x[n] = x;	
			m_tPoly.y[n] = y;	
			n++;
		}
		else if (vec == 1) {  
			if ((yf+1<rawH) && (gBuf[xf +(yf+1)*rawW] != 0xff)) { vec = 2; yf++; }
			x = xf;  y = yf;
			m_tPoly.x[n] = x;	
			m_tPoly.y[n] = y;	
			n++;
		} 
		else if (vec == 2) {  
			if ((xf-1>=0) && (gBuf[(xf-1) +yf*rawW] != 0xff)) { vec = 3;  xf--;}
			x = xf;  y = yf;
			m_tPoly.x[n] = x;	
			m_tPoly.y[n] = y;	
			n++;
		} 
		else if (vec == 3) {  
			if ((yf-1>=0) && (gBuf[xf +(yf-1)*rawW] != 0xff)) { vec = 0; yf--; }
			x = xf;  y = yf;
			m_tPoly.x[n] = x;	
			m_tPoly.y[n] = y;	
			n++;
		} 
		
		m_tPoly.VSize = n;
		if (m_tPoly.x[0] == x && m_tPoly.y[0] == y) { return n; }
	
	}
	return -1;
}

//----------------------------------------------------------------------------
int CPolygons::SearchEPoint(int &x, int &y)	
{
	char *pSP;
	int l;

	m_lastV = 0;
	//pSP = (char*)memchr(gBuf , 0xff, rawW*rawH );
	//if(pSP == NULL) {return 0;}
	//l = (int)pSP - (int)gBuf;
	
	// 2014/07 fixed the 3 lines above for Linux 64-bit - search for 0xff in gBuf set value of l to the position of the byte or return 0 if not found.
	l = -1;
	for(int i = 0; i < rawW*rawH; i++)
	{
		if((gBuf[i]^0xff) == 0)
		{
			l = i;
			break;
		}
	}
	if(l == -1) { return 0; }
	y = l / rawW -1;
	x = l % rawW;

	return 1;
}

//----------------------------------------------------------------------------
int CPolygons::MakeNPoly(int n)			
{
	int x,y,i,j;
	int t2;
	CalcAllTri();
	for (i=0; i<=1000; i++) {
		if (n >= m_tPoly.VSize) {  
			for (j=m_tPoly.VSize; j<n; j++) {
				x = m_tPoly.x[0];	y = m_tPoly.y[0];	 
				t2 = m_tPoly.TriSize[0];
				m_tPoly.x[m_Poly.VSize] = x;
				m_tPoly.y[m_Poly.VSize] = y;
				m_tPoly.TriSize[m_Poly.VSize] = t2;
				m_tPoly.VSize++;
			}
			return j;
		}
		RemoveVertex(MinTriSize());
	}
	return 0;
}

//----------------------------------------------------------------------------
void CPolygons::CalcAllTri()		
{
	int tmp, i;

	for (i=0; i<=m_tPoly.VSize-1; i++) {
		tmp = CalcTriSize(i);
		m_tPoly.TriSize[i] = tmp;
	}
}

//----------------------------------------------------------------------------
int CPolygons::CalcTriSize(int n)
{
	int bx,by ,fx,fy;
	if (n >= m_tPoly.VSize) { return -1; }

	if (n == 0) { 
		bx = m_tPoly.x[m_tPoly.VSize-1];
		by = m_tPoly.y[m_tPoly.VSize-1];
	}
	else { 
		bx = m_tPoly.x[n-1];
		by = m_tPoly.y[n-1];
	}
	if (n == m_tPoly.VSize-1) { 
		fx = m_tPoly.x[0];
		fy = m_tPoly.y[0];
	}
	else {
		fx = m_tPoly.x[n+1];
		fy = m_tPoly.y[n+1];
	} 

	return abs((m_tPoly.x[n] - bx)*(fy - by) - (fx - bx)*(m_tPoly.y[n] - by));
}

//----------------------------------------------------------------------------
int CPolygons::MinTriSize()	
{
	int r = 0, rS, i;

	rS = m_tPoly.TriSize[0];
	for (i=1; i<=m_tPoly.VSize-1; i++) {
		if (m_tPoly.TriSize[i] < rS) {r = i;  rS = m_tPoly.TriSize[i];}
		if (rS == 0) {return r;}
	}
	return r;
}

//----------------------------------------------------------------------------
void CPolygons::RemoveVertex(int n)	
{
	int bn, fn, maxn;
	int vx, vy;

	maxn = m_tPoly.VSize-1;
	if (n == 0) { bn = maxn; }
	else { bn = n-1; }
	if (n == maxn) { fn = 0; }
	else { fn = n+1; } 

	double x1, y1, x2, y2;
	x1 = m_tPoly.x[ n] - m_tPoly.x[bn];    y1 = m_tPoly.y[ n] - m_tPoly.y[bn];
	x2 = m_tPoly.x[fn] - m_tPoly.x[bn];    y2 = m_tPoly.y[fn] - m_tPoly.y[bn];

	if (y2 == 0) {vx = 0;  vy = (int)(y1 / 2);}
	else if (x2 == 0) {vx = (int)(x1 / 2);  vy = 0;}
	else {
		vx = (int)((x1*y2 / 2/x2 - y1/2) / (y2/x2 + x2/y2));
		vy = (int)((y1*x2 / 2/y2 - x1/2) / (y2/x2 + x2/y2));
	}

	m_tPoly.x[fn] += vx; 	m_tPoly.y[fn] += vy; 
	m_tPoly.x[bn] += vx;	m_tPoly.y[bn] += vy;

	int i;
	for (i=n; i<=m_tPoly.VSize-2; i++) {
		m_tPoly.x[i] = m_tPoly.x[i+1] ;
		m_tPoly.y[i] = m_tPoly.y[i+1] ;
		m_tPoly.TriSize[i] = m_tPoly.TriSize[i+1] ;
	}
	m_tPoly.VSize--;

	CalcAllTri();

/*	if (bn == maxn) {bn--;}
	fn--;   if (fn < 0) {fn = 0;}
    maxn--;

	m_tPoly.TriSize[fn] = CalcTriSize(fn);
    m_tPoly.TriSize[bn] = CalcTriSize(bn);    
	if (bn == 0) { m_tPoly.TriSize[maxn] = CalcTriSize(maxn);}
	else { m_tPoly.TriSize[bn-1] = CalcTriSize(bn-1);}
	if (fn == maxn) { m_tPoly.TriSize[0] = CalcTriSize(0);}
	else { m_tPoly.TriSize[fn+1] = CalcTriSize(fn+1);}	*/
}

//----------------------------------------------------------------------------
void CPolygons::LMinShift(CPolygon &ipoly)	
{
	double l, lp;
	int i ,li;

	if (ipoly.VSize != m_Poly.VSize) { return ; }
	l = LSubSum(ipoly);	 li=0;

	for (i=0; i<m_Poly.VSize; i++) {
		lp =LSubSum(ipoly);
		if (lp < l) { l = lp;   li = i;}
		shift(1);
	}
	shift(li);
}

//----------------------------------------------------------------------------
double CPolygons::LSubSum(CPolygon &ipoly)		
{
	double sum = 0;
	if (ipoly.VSize != m_Poly.VSize) { return -1; }

	for (int i=0; i<m_Poly.VSize; i++) {
		sum += sqrt(  (m_Poly.X[i]-ipoly.X[i])*(m_Poly.X[i]-ipoly.X[i]) 
					 +(m_Poly.Y[i]-ipoly.Y[i])*(m_Poly.Y[i]-ipoly.Y[i])  ); 
	}
	return sum;
}

//----------------------------------------------------------------------------
void CPolygons::shift(int n)				
{
	int i, j;
	if (m_Poly.VSize == 0) {return;}
	double xtmp, ytmp;

	if (n == 0) return;
	if (n > 0) {  
		for (i=n; i>=1; i--) {
			xtmp = m_Poly.X[0];		ytmp = m_Poly.Y[0];
			for (j=0; j<m_Poly.VSize-1; j++) {
				m_Poly.X[j] = m_Poly.X[j+1];
				m_Poly.Y[j] = m_Poly.Y[j+1];
			}
			m_Poly.X[m_Poly.VSize-1] = xtmp;
			m_Poly.Y[m_Poly.VSize-1] = ytmp;
		}
	} else {		
		for (i=n; i<=-1; i++) {
			xtmp = m_Poly.X[m_Poly.VSize-1];	ytmp = m_Poly.Y[m_Poly.VSize-1];
			for (j=m_Poly.VSize-1; j>0; j--) {
				m_Poly.X[j] = m_Poly.X[j-1];
				m_Poly.Y[j] = m_Poly.Y[j-1];
			}
			m_Poly.X[0] = xtmp;			m_Poly.Y[0] = ytmp;
		}
	}
}


//=============================================================================
CBoundingBox::CBoundingBox()
{
	m_ObjectID = 0;
	m_Num = 0;
}

//----------------------------------------------------------------------------
CBoundingBox::~CBoundingBox()
{
}

//----------------------------------------------------------------------------
int CBoundingBox::Calc(unsigned char* pShape, int width, int height, int frame,
				   int area, SPoint& centroid, SRect& region)
{
	m_pShape = pShape;
	m_Width = width;
	m_Height = height;
	m_Frame = frame;
	m_Area = area;
	m_Centroid = centroid;
	m_Region = region;

	if (m_Area <= AREA_THRESHOLD) {
		if (m_Num > 0) {
			m_Num = 0;
		}
		return FALSE;
	}

	if (1) {
		int       x, y;
		double    xx, yy;
		double    m20, m02, m11;
		double    a, b, c;
		
		m20 = m02 = m11 = 0.0;
		for (y = 0; y < m_Height; y++) {
			for (x = 0; x < m_Width; x++) {
				if (m_pShape[y * m_Width + x] == 0) continue;
				xx = (double)x - m_Centroid.x;
				yy = (double)y - m_Centroid.y;
				m20 += (xx * xx);
				m02 += (yy * yy);
				m11 += (xx * yy);
			}
		}
		
		a = 1.0;
		b = (m20 - m02) / m11;
		c = -1.0;
		
		double det = b * b - 4.0 * a * c;
		
		if (det >= 0.0) {
			double la = (-b + sqrt(det)) / (2.0 * a);
			double lb = m_Centroid.y - la * m_Centroid.x;
			
			double sa = (-b - sqrt(det)) / (2.0 * a);
			double sb = m_Centroid.y - sa * m_Centroid.x;
			
			if (la == 0.0) {
				m_BoundingBox.x[0] = m_Region.left;  m_BoundingBox.y[0] = m_Region.top;
				m_BoundingBox.x[1] = m_Region.right; m_BoundingBox.y[1] = m_Region.top;
				m_BoundingBox.x[2] = m_Region.right; m_BoundingBox.y[2] = m_Region.bottom;
				m_BoundingBox.x[3] = m_Region.left;  m_BoundingBox.y[3] = m_Region.bottom;
			}
			else {
				GetBoundingBox(la, lb, sa, sb);
			}
		}
		else {
			m_BoundingBox.x[0] = m_Region.left;  m_BoundingBox.y[0] = m_Region.top;
			m_BoundingBox.x[1] = m_Region.right; m_BoundingBox.y[1] = m_Region.top;
			m_BoundingBox.x[2] = m_Region.right; m_BoundingBox.y[2] = m_Region.bottom;
			m_BoundingBox.x[3] = m_Region.left;  m_BoundingBox.y[3] = m_Region.bottom;
		}
		
		if (m_Num > 0) {
			Matching(&m_PrevCentroid, &m_PrevBoundingBox, &m_Centroid, &m_BoundingBox);
		}
	}
	else {
		m_BoundingBox.x[0] = (double)(m_Region.left);
		m_BoundingBox.y[0] = (double)(m_Region.top);
		m_BoundingBox.x[1] = (double)(m_Region.right);
		m_BoundingBox.y[1] = (double)(m_Region.top);
		m_BoundingBox.x[2] = (double)(m_Region.right);
		m_BoundingBox.y[2] = (double)(m_Region.bottom);
		m_BoundingBox.x[3] = (double)(m_Region.left);
		m_BoundingBox.y[3] = (double)(m_Region.bottom);
	}

	m_BoundingBox.frame = m_Frame;
	m_Array[m_Num] = m_BoundingBox;

	SParams params;
	params.frame = m_Frame;
	params.area = m_Area;
	m_ParamsArray[m_Num] = params;

	m_Num++;

	m_PrevCentroid = m_Centroid;
	m_PrevBoundingBox = m_BoundingBox;

	return TRUE;
}

//----------------------------------------------------------------------------
void CBoundingBox::GetBoundingBox(double la, double lb, double sa, double sb)
{
	int       x, y;
	double    lb1, lb2, sb1, sb2;
	double    min, max;

	if (la < 0.0) {
		// No.1
		min = lb;
		for (y = m_Region.top; y <= m_Region.bottom ; y++) {
			for (x = m_Region.left; x <= m_Region.right; x++) {
				if (m_pShape[y * m_Width + x] == 0) continue;
				lb1 = (double)y - la * (double)x;
				if (lb1 < min) min = lb1;
			}
		}
		lb1 = min;
		
		// No.2
		max = lb;
		for (y = m_Region.top; y <= m_Region.bottom ; y++) {
			for (x = m_Region.left; x <= m_Region.right; x++) {
				if (m_pShape[y * m_Width + x] == 0) continue;
				lb2 = (double)y - la * (double)x;
				if (lb2 > max) max = lb2;
			}
		}
		lb2 = max;

		// No.3
		min = sb;
		for (y = m_Region.top; y <= m_Region.bottom ; y++) {
			for (x = m_Region.left; x <= m_Region.right; x++) {
				if (m_pShape[y * m_Width + x] == 0) continue;
				sb1 = (double)y - sa * (double)x;
				if (sb1 < min) min = sb1;
			}
		}
		sb1 = min;
		
		// No.4
		max = sb;
		for (y = m_Region.top; y <= m_Region.bottom ; y++) {
			for (x = m_Region.left; x <= m_Region.right; x++) {
				if (m_pShape[y * m_Width + x] == 0) continue;
				sb2 = (double)y - sa * (double)x;
				if (sb2 > max) max = sb2;
			}
		}
		sb2 = max;

		CalcSimultaneousEquations(la, lb1, sa, sb2, &(m_BoundingBox.x[0]), &(m_BoundingBox.y[0]));
		CalcSimultaneousEquations(la, lb1, sa, sb1, &(m_BoundingBox.x[1]), &(m_BoundingBox.y[1]));
		CalcSimultaneousEquations(la, lb2, sa, sb1, &(m_BoundingBox.x[2]), &(m_BoundingBox.y[2]));
		CalcSimultaneousEquations(la, lb2, sa, sb2, &(m_BoundingBox.x[3]), &(m_BoundingBox.y[3]));
	}
	else {
		// No.1
		min = lb;
		for (y = m_Region.top; y <= m_Region.bottom ; y++) {
			for (x = m_Region.left; x <= m_Region.right; x++) {
				if (m_pShape[y * m_Width + x] == 0) continue;
				lb1 = (double)y - la * (double)x;
				if (lb1 < min) min = lb1;
			}
		}
		lb1 = min;
		
		// No.2
		max = lb;
		for (y = m_Region.top; y <= m_Region.bottom ; y++) {
			for (x = m_Region.left; x <= m_Region.right; x++) {
				if (m_pShape[y * m_Width + x] == 0) continue;
				lb2 = (double)y - la * (double)x;
				if (lb2 > max) max = lb2;
			}
		}
		lb2 = max;

		// No.3
		min = sb;
		for (y = m_Region.top; y <= m_Region.bottom ; y++) {
			for (x = m_Region.left; x <= m_Region.right; x++) {
				if (m_pShape[y * m_Width + x] == 0) continue;
				sb1 = (double)y - sa * (double)x;
				if (sb1 < min) min = sb1;
			}
		}
		sb1 = min;
		
		// No.4
		max = sb;
		for (y = m_Region.top; y <= m_Region.bottom ; y++) {
			for (x = m_Region.left; x <= m_Region.right; x++) {
				if (m_pShape[y * m_Width + x] == 0) continue;
				sb2 = (double)y - sa * (double)x;
				if (sb2 > max) max = sb2;
			}
		}
		sb2 = max;

		CalcSimultaneousEquations(la, lb1, sa, sb1, &(m_BoundingBox.x[0]), &(m_BoundingBox.y[0]));
		CalcSimultaneousEquations(la, lb1, sa, sb2, &(m_BoundingBox.x[1]), &(m_BoundingBox.y[1]));
		CalcSimultaneousEquations(la, lb2, sa, sb2, &(m_BoundingBox.x[2]), &(m_BoundingBox.y[2]));
		CalcSimultaneousEquations(la, lb2, sa, sb1, &(m_BoundingBox.x[3]), &(m_BoundingBox.y[3]));
	}
}

//----------------------------------------------------------------------------
void CBoundingBox::CalcSimultaneousEquations(double a1, double b1, double a2, double b2,
										double* x, double* y)
{
	*x = (b2 - b1) / (a1 - a2);
	*y = a1 * (*x) + b1;
}

//----------------------------------------------------------------------------
void CBoundingBox::Matching(SPoint* pPrevCentroid, SBox* pPrevBoundingBox,
					   SPoint* pCurrCentroid, SBox* pCurrBoundingBox)
{
	double tx = pCurrCentroid->x - pPrevCentroid->x;
	double ty = pCurrCentroid->y - pPrevCentroid->y;

	SBox tCurrBoundingBox;
	for (int i = 0; i < 4; i++) {
		tCurrBoundingBox.x[i] = pCurrBoundingBox->x[i] - tx;
		tCurrBoundingBox.y[i] = pCurrBoundingBox->y[i] - ty;
	}

	double d[4];
	double dx, dy;
	int    min_index;
	double min;

	dx = tCurrBoundingBox.x[0] - pPrevBoundingBox->x[0];
	dy = tCurrBoundingBox.y[0] - pPrevBoundingBox->y[0];
	d[0] = sqrt(dx * dx + dy * dy);
	dx = tCurrBoundingBox.x[1] - pPrevBoundingBox->x[1];
	dy = tCurrBoundingBox.y[1] - pPrevBoundingBox->y[1];
	d[0] += sqrt(dx * dx + dy * dy);
	dx = tCurrBoundingBox.x[2] - pPrevBoundingBox->x[2];
	dy = tCurrBoundingBox.y[2] - pPrevBoundingBox->y[2];
	d[0] += sqrt(dx * dx + dy * dy);
	dx = tCurrBoundingBox.x[3] - pPrevBoundingBox->x[3];
	dy = tCurrBoundingBox.y[3] - pPrevBoundingBox->y[3];
	d[0] += sqrt(dx * dx + dy * dy);
	min = d[0]; min_index = 0;

	dx = tCurrBoundingBox.x[0] - pPrevBoundingBox->x[1];
	dy = tCurrBoundingBox.y[0] - pPrevBoundingBox->y[1];
	d[1] = sqrt(dx * dx + dy * dy);
	dx = tCurrBoundingBox.x[1] - pPrevBoundingBox->x[2];
	dy = tCurrBoundingBox.y[1] - pPrevBoundingBox->y[2];
	d[1] += sqrt(dx * dx + dy * dy);
	dx = tCurrBoundingBox.x[2] - pPrevBoundingBox->x[3];
	dy = tCurrBoundingBox.y[2] - pPrevBoundingBox->y[3];
	d[1] += sqrt(dx * dx + dy * dy);
	dx = tCurrBoundingBox.x[3] - pPrevBoundingBox->x[0];
	dy = tCurrBoundingBox.y[3] - pPrevBoundingBox->y[0];
	d[1] += sqrt(dx * dx + dy * dy);
	if (d[1] < min) { min = d[1]; min_index = 1; }

	dx = tCurrBoundingBox.x[0] - pPrevBoundingBox->x[2];
	dy = tCurrBoundingBox.y[0] - pPrevBoundingBox->y[2];
	d[2] = sqrt(dx * dx + dy * dy);
	dx = tCurrBoundingBox.x[1] - pPrevBoundingBox->x[3];
	dy = tCurrBoundingBox.y[1] - pPrevBoundingBox->y[3];
	d[2] += sqrt(dx * dx + dy * dy);
	dx = tCurrBoundingBox.x[2] - pPrevBoundingBox->x[0];
	dy = tCurrBoundingBox.y[2] - pPrevBoundingBox->y[0];
	d[2] += sqrt(dx * dx + dy * dy);
	dx = tCurrBoundingBox.x[3] - pPrevBoundingBox->x[1];
	dy = tCurrBoundingBox.y[3] - pPrevBoundingBox->y[1];
	d[2] += sqrt(dx * dx + dy * dy);
	if (d[2] < min) { min = d[2]; min_index = 2; }

	dx = tCurrBoundingBox.x[0] - pPrevBoundingBox->x[3];
	dy = tCurrBoundingBox.y[0] - pPrevBoundingBox->y[3];
	d[3] = sqrt(dx * dx + dy * dy);
	dx = tCurrBoundingBox.x[1] - pPrevBoundingBox->x[0];
	dy = tCurrBoundingBox.y[1] - pPrevBoundingBox->y[0];
	d[3] += sqrt(dx * dx + dy * dy);
	dx = tCurrBoundingBox.x[2] - pPrevBoundingBox->x[1];
	dy = tCurrBoundingBox.y[2] - pPrevBoundingBox->y[1];
	d[3] += sqrt(dx * dx + dy * dy);
	dx = tCurrBoundingBox.x[3] - pPrevBoundingBox->x[2];
	dy = tCurrBoundingBox.y[3] - pPrevBoundingBox->y[2];
	d[3] += sqrt(dx * dx + dy * dy);
	if (d[3] < min) { min = d[3]; min_index = 3; }

	double tmp;

	switch (min_index) {
	case 0:
		break;
	case 1:
		tmp = pCurrBoundingBox->x[3];
		pCurrBoundingBox->x[3] = pCurrBoundingBox->x[2];
		pCurrBoundingBox->x[2] = pCurrBoundingBox->x[1];
		pCurrBoundingBox->x[1] = pCurrBoundingBox->x[0];
		pCurrBoundingBox->x[0] = tmp;
		tmp = pCurrBoundingBox->y[3];
		pCurrBoundingBox->y[3] = pCurrBoundingBox->y[2];
		pCurrBoundingBox->y[2] = pCurrBoundingBox->y[1];
		pCurrBoundingBox->y[1] = pCurrBoundingBox->y[0];
		pCurrBoundingBox->y[0] = tmp;
		break;
	case 2:
		tmp = pCurrBoundingBox->x[0];
		pCurrBoundingBox->x[0] = pCurrBoundingBox->x[2];
		pCurrBoundingBox->x[2] = tmp;
		tmp = pCurrBoundingBox->x[1];
		pCurrBoundingBox->x[1] = pCurrBoundingBox->x[3];
		pCurrBoundingBox->x[3] = tmp;
		tmp = pCurrBoundingBox->y[0];
		pCurrBoundingBox->y[0] = pCurrBoundingBox->y[2];
		pCurrBoundingBox->y[2] = tmp;
		tmp = pCurrBoundingBox->y[1];
		pCurrBoundingBox->y[1] = pCurrBoundingBox->y[3];
		pCurrBoundingBox->y[3] = tmp;
		break;
	case 3:
		tmp = pCurrBoundingBox->x[0];
		pCurrBoundingBox->x[0] = pCurrBoundingBox->x[1];
		pCurrBoundingBox->x[1] = pCurrBoundingBox->x[2];
		pCurrBoundingBox->x[2] = pCurrBoundingBox->x[3];
		pCurrBoundingBox->x[3] = tmp;
		tmp = pCurrBoundingBox->y[0];
		pCurrBoundingBox->y[0] = pCurrBoundingBox->y[1];
		pCurrBoundingBox->y[1] = pCurrBoundingBox->y[2];
		pCurrBoundingBox->y[2] = pCurrBoundingBox->y[3];
		pCurrBoundingBox->y[3] = tmp;
		break;
	}
}

//----------------------------------------------------------------------------
double CBoundingBox::GetArea(SBox* pBox)
{
	double dx, dy;
	double a, b;
	
	dx = pBox->x[0] - pBox->x[1];
	dy = pBox->y[0] - pBox->y[1];
	a = sqrt(dx * dx + dy * dy);
	
	dx = pBox->x[1] - pBox->x[2];
	dy = pBox->y[1] - pBox->y[2];
	b = sqrt(dx * dx + dy * dy);
	
	return a * b;
}

//=============================================================================
CCentroid::CCentroid()
{
	m_ObjectID = 0;
	m_Num = 0;
}

//----------------------------------------------------------------------------
CCentroid::~CCentroid()
{
}

//----------------------------------------------------------------------------
int CCentroid::Calc(unsigned char* pShape, int width, int height, int frame)
{
	m_pShape = pShape;
	m_Width = width;
	m_Height = height;
	m_Frame = frame;

	m_Region.left = m_Width;
	m_Region.top = m_Height;
	m_Region.right = -1;
	m_Region.bottom = -1;

	int    x, y;

	m_Area = 0;
	m_Centroid.frame = m_Frame;
	m_Centroid.x = m_Centroid.y = 0.0;
	for (y = 0; y < m_Height; y++) {
		for (x = 0; x < m_Width; x++) {
			if (pShape[y * m_Width + x] == 0) continue;
			m_Centroid.x += (double)x;
			m_Centroid.y += (double)y;
			if (m_Region.left > x)   m_Region.left = x;
			if (m_Region.top > y)    m_Region.top = y;
			if (m_Region.right < x)  m_Region.right = x;
			if (m_Region.bottom < y) m_Region.bottom = y;
			m_Area++;
		}
	}

	if (m_Area > AREA_THRESHOLD) {
		m_Centroid.x /= (double)m_Area;
		m_Centroid.y /= (double)m_Area;
		m_Array[m_Num] = m_Centroid;
		m_RegionArray[m_Num] = m_Region;
		m_Num++;
	}
	
	if (m_Area <= AREA_THRESHOLD) {
		if (m_Num > 0) {
			m_Num = 0;
		}
		return FALSE;
	}
	else return TRUE;
}


//=============================================================================
CExtractMP::CExtractMP()
{
}

//----------------------------------------------------------------------------
CExtractMP::~CExtractMP()
{
}

//----------------------------------------------------------------------------
int CExtractMP::Initialize(unsigned char* pShape,int Width,int Height,int Model, int frame)
{
	int ret;
	SPoint spoint;
	SRect srect;

	switch( Model ){
	case AFFINE_MODEL:
		break;
	default:
		return FALSE;
	}

	m_Width = Width;
	m_Height = Height;
	m_Model = Model;
	m_pShape = pShape;
	m_Frame = frame;

	if (m_pShape == NULL) return FALSE;

	// extract bounding box
	ret = m_Centroid.Calc(m_pShape, m_Width, m_Height, m_Frame );
	if( ret == FALSE ) return FALSE;
	spoint = m_Centroid.GetCentroid();
	srect = m_Centroid.GetRegion();
	ret = m_BoundingBox.Calc(m_pShape, m_Width, m_Height, m_Frame,
		              m_Centroid.GetArea(), spoint, srect);
	if( ret == FALSE ) return FALSE;

	for( int i=0; i<4; i++ ){
		m_InitRegionX[i] = m_BoundingBox.GetBoundingBox().x[i];
		m_InitRegionY[i] = m_BoundingBox.GetBoundingBox().y[i];
	}

	return TRUE;
}

//----------------------------------------------------------------------------
int CExtractMP::GetInitRegion(double* pRegionX,double* pRegionY)
{
	int i;
	
	for( i=0; i<4; i++ ){
		pRegionX[i] = m_InitRegionX[i];
		pRegionY[i] = m_InitRegionY[i];
	}

	return TRUE;
}

//----------------------------------------------------------------------------
int CExtractMP::ExtractMotionParameters(double* pParam, unsigned char*pShape, int frame)
{
	int ret;
	SPoint spoint;
	SRect srect;

	// read shape data
	m_Frame = frame;
	m_pShape = pShape;
	if (m_pShape == NULL) return FALSE;

	// extract bounding box
	ret = m_Centroid.Calc(m_pShape, m_Width, m_Height, m_Frame );
	if( ret == FALSE ) return FALSE;
	spoint = m_Centroid.GetCentroid();
	srect = m_Centroid.GetRegion();
	ret = m_BoundingBox.Calc(m_pShape, m_Width, m_Height, m_Frame,
		              m_Centroid.GetArea(), spoint, srect);
	if( ret == FALSE ) return FALSE;

	// estimate motion parameters
	ret = CalcAffine(4, m_InitRegionX, m_InitRegionY, m_BoundingBox.GetBoundingBox().x, 
		m_BoundingBox.GetBoundingBox().y, pParam );

	m_Frame++;

	return ret;
}

//----------------------------------------------------------------------------
int CExtractMP::CalcAffine(int vcnt, double *bx0,double *by0, double *bx1, double *by1,double *affine)
{
	double Sxx, Sxy, Syy, Sx, Sy, Sxxd, Syxd, Sxyd, Syyd, Sxd, Syd, S1;
	double den;
	double *px, *py, *qx, *qy;
	int i;
	
	Sxx = Sxy = Syy = Sx = Sy = Sxxd = Syxd = Sxyd = Syyd = Sxd = Syd = 0.0;
	S1 = (double)vcnt;
	px = bx0; py = by0; qx = bx1; qy = by1;
	for( i=0; i<vcnt; i++ ){
		Sxx += px[i]*px[i];
		Sxy += px[i]*py[i];
		Syy += py[i]*py[i];
		Sx += px[i];
		Sy += py[i];
		Sxxd += px[i]*qx[i];
		Syxd += py[i]*qx[i];
		Sxyd += px[i]*qy[i];
		Syyd += py[i]*qy[i];
		Sxd += qx[i];
		Syd += qy[i];
	}

	den = (S1*Syy-Sy*Sy)*(S1*Sxx-Sx*Sx)-(S1*Sxy-Sx*Sy)*(S1*Sxy-Sx*Sy);
	if( den == 0.0 ){
		affine[0] = 1.0;
		affine[3] = 0.0;
	} else {
		affine[0] = ((S1*Syy-Sy*Sy)*(S1*Sxxd-Sx*Sxd)-(S1*Sxy-Sx*Sy)*(S1*Syxd-Sy*Sxd))/den;
		affine[3] = ((S1*Syy-Sy*Sy)*(S1*Sxyd-Sx*Syd)-(S1*Sxy-Sx*Sy)*(S1*Syyd-Sy*Syd))/den;
	}
	den = S1*Sxy-Sx*Sy;
	if( den == 0.0 ){
		affine[1] = affine[3];
		affine[4] = affine[0];
	} else {
		affine[1] = ((S1*Sxxd-Sx*Sxd)-(S1*Sxx-Sx*Sx)*affine[0])/den;
		affine[4] = ((S1*Sxyd-Sx*Syd)-(S1*Sxx-Sx*Sx)*affine[3])/den;
	}
	affine[2] = (Sxd-Sx*affine[0]-Sy*affine[1])/S1;
	affine[5] = (Syd-Sx*affine[3]-Sy*affine[4])/S1;

	return TRUE;
}




