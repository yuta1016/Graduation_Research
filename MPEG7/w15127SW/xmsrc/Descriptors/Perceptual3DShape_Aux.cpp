///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// In Kyu Park, *Duck Hoon Kim, **Il Dong Yun, Hui Zhang
// Samsung Advanced Institute of Technology, *Seoul National University, **Hankuk University of Foreign Studies
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  Perceptual3DShape_Aux.cpp
//

#include <stdio.h>
#include "math.h"

#include <fstream>
//#include "../lib/cv_P3DS/cv.h"
#include <cv.h>
#include "float.h"

#include "Descriptors/Perceptual3DShape.h"
using namespace XM;

using namespace std;

#include "Media/Media.h"

extern "C"
{
#ifdef _WIN32
#include "lib/cv_P3DS_2010/qhull_a.h"
#else
#include <qhull/qhull_a.h>
#endif // _WIN32
}

typedef int BOOL;
#define TRUE 1
#define FALSE 0
#include "Perceptual3DShape_Aux.h"

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

////////////////////////////////////////////////////////////////////////////////
//from P3DS.cpp
/////////////////////////////////////////////////////////////////////////////
// CNode

CNode::CNode() {
	idx = -1;

	vol = 0;
	x = y = x = 0;
	for ( int i = 0 ; i < 9 ; i++ ) mat[i] = 0;
	var_x = var_y = var_z = 0;
	convex = 0;
}


/*
CNode::CNode(const CNode& node) {
	idx = node.idx;

	vol = node.vol;
	x = node.x; y = node.y; z = node.z;
	for ( int i = 0 ; i < 9 ; i++ ) mat[i] = node.mat[i];
	var_x = node.var_x; var_y = node.var_y; var_z = node.var_z;
	convex = node.convex;	

	for ( i = 0 ; i < node.pNode.size() ; i++ ) {
		pNode.Add(node.pNode[i]);
	}
}
const CNode& CNode::operator =(const CNode& node) {	
	idx = node.idx;

	vol = node.vol;
	x = node.x; y = node.y; z = node.z;
	for ( int i = 0 ; i < 9 ; i++ ) mat[i] = node.mat[i];
	var_x = node.var_x; var_y = node.var_y; var_z = node.var_z;
	convex = node.convex;	

	for ( i = 0 ; i < node.pNode.size() ; i++ ) {
		pNode.Add(node.pNode[i]);
	}
	return *this;
}

bool CNode::operator ==(const CNode& node) const {
	return true;
}
*/

/////////////////////////////////////////////////////////////////////////////
// P3DS

P3DS::P3DS() {
	m_fx = m_fy = m_fz = 0;
	m_fv = 0; m_nBody = 0;
}

P3DS::~P3DS() {
	Clear();	
}

void P3DS::Clear()
{
	for ( int i = pNodeArray.size() - 1 ; i >= 0 ; i-- ) {
		CNode * pChild = pNodeArray[i];
    //delete [] pChild;pNodeArray.RemoveAt(i);
    delete pChild;pNodeArray.erase(pNodeArray.begin()+i);
	}
	m_fx = m_fy = m_fz = 0;
	m_fv = 0; m_nBody = 0;
}

P3DS::P3DS(const P3DS& p3ds) {
	m_fx = p3ds.m_fx; m_fy = p3ds.m_fy; m_fz = p3ds.m_fz;
	m_fv = p3ds.m_fv; m_nBody = p3ds.m_nBody;

	for ( int i = 0 ; i < p3ds.pNodeArray.size() ; i++ ) {
		CNode * pChild = new CNode;
		*pChild = *p3ds.pNodeArray[i];
		//pNodeArray.Add(pChild);
		pNodeArray.push_back(pChild);
	}
}

const P3DS& P3DS::operator =(const P3DS& p3ds) {
	m_fx = p3ds.m_fx; m_fy = p3ds.m_fy; m_fz = p3ds.m_fz;
	m_fv = p3ds.m_fv; m_nBody = p3ds.m_nBody;

	for ( int i = 0 ; i < p3ds.pNodeArray.size() ; i++ ) {
		CNode * pChild = new CNode;
		*pChild = *p3ds.pNodeArray[i];
		//pNodeArray.Add(pChild);
    pNodeArray.push_back(pChild);
	}

	return *this;
}

bool P3DS::operator ==(const P3DS& p3ds) const {
	return true;
}

void P3DS::InitCGraph()
{
	// unary feature
	CUnary::nFeature = 4;
	CUnary::nStart = 0;
	CUnary::pMin[0] = 0; CUnary::pMin[1] = 1; CUnary::pMin[2] = 1; CUnary::pMin[3] = 1;	
	CUnary::pWeight[0] = 0.5; CUnary::pWeight[1] = 0.3; CUnary::pWeight[2] = 0.1; CUnary::pWeight[3] = 0.1;
	
	// binary feature	
	CBinary::nFeature = 3;
	CBinary::pMin[0] = 0; CBinary::pMin[1] = 1; CBinary::pMin[2] = 1;	
	CBinary::pWeight[0] = 0.5; CBinary::pWeight[1] = 0.3; CBinary::pWeight[2] = 0.2;
}

BOOL P3DS::Load(LPCTSTR lpszPathName)
{
	int i, j;
	int nNode; int nIndex;
	int tmp;

	fstream gph;
	//gph.open(lpszPathName, ios::in|ios::nocreate);
	gph.open(lpszPathName, ios::in);
	if ( !gph.is_open() ) {gph.clear(); return FALSE;}

	gph >> m_fx >> m_fy >> m_fz;
	gph >> m_fv >> m_nBody;	
	gph >> nNode;

	for ( i = 0 ; i < nNode ; i++ ) {
		CNode * pN = new CNode;
		//pNodeArray.Add(pN);
    pNodeArray.push_back(pN);
	}

	for ( i = 0 ; i < nNode ; i++ ) {
		CNode * pN = pNodeArray[i];
		gph >> pN->idx;
		gph >> pN->vol;
		gph >> pN->x >> pN->y >> pN->z;
		for ( j = 0 ; j < 9 ; j++ ) gph >> pN->mat[j];		
		gph >> pN->var_x >> pN->var_y >> pN->var_z;
		gph >> pN->convex;		
		gph >> tmp >> tmp;

		gph >> nIndex;
		for ( j = 0 ; j < nIndex ; j++ ) {
			gph >> tmp; 
      //pN->pNode.Add(pNodeArray[tmp-1]);			
      pN->pNode.push_back(pNodeArray[tmp-1]);
		}
	}
	gph.close();

	return TRUE;
}

void P3DS::Save(LPCTSTR lpszPathName)
{
	int i, j;
	fstream gph;
	gph.open(lpszPathName, ios::out);

	gph << m_fx << " " << m_fy << " " << m_fz << "\n";
	gph << m_fv << " " << m_nBody << "\n";	
	gph << pNodeArray.size() << "\n\n";
	for ( i = 0 ; i < pNodeArray.size() ; i++ ) {
		CNode * pN = pNodeArray[i];
		gph << pN->idx << "\n";
		gph << pN->vol << "\n";
		gph << pN->x << " " << pN->y << " " << pN->z << "\n";
		for ( j = 0 ; j < 9 ; j++ ) gph << pN->mat[j] << " ";
		gph << "\n";
		gph << pN->var_x << " " << pN->var_y << " " << pN->var_z << "\n";
		gph << pN->convex << "\n";
		gph << 0 << " " << 0 << "\n";

		gph << pN->pNode.size() << "\n"; //
		for ( j = 0 ; j < pN->pNode.size() ; j++ ) {
			gph << pN->pNode[j]->idx << " ";	
		}
		gph << "\n\n";
	}
	gph.close();
}

//////////////////////////////////////////////////////////////////////////////////
//new-added by ZhangHui, for quantization.

//this value is [0, 1], convert it to byte (0~255).
inline unsigned long Quantize(float val, int nParaBits)
{if(val>=1) return nParaBits-1;return (unsigned long)floor(val*nParaBits);}
//this value is [-1, 1], convert it to byte (0~255).
inline unsigned long Quantize2(float val, int nParaBits)
{if(val>=1) return nParaBits-1;return (unsigned long)floor((val+1)*nParaBits/2);}

void P3DS::Save_quantize(LPCTSTR lpszPathName, 
  int nBits)///<i: bit number used for quantization.
{
	int i, j;
	fstream gph;
	gph.open(lpszPathName, ios::out);

	int nValueBits = (int)(0.5+pow((double)2, nBits));

	gph << m_fx << " " << m_fy << " " << m_fz << "\n";
	gph << m_fv << " " << m_nBody << "\n";	
	gph << pNodeArray.size() << "\n\n";
	for ( i = 0 ; i < pNodeArray.size() ; i++ ) {
		CNode * pN = pNodeArray[i];
		gph << pN->idx << "\n";
		gph << Quantize(pN->vol, nValueBits) << "\n";
		gph << Quantize(pN->x, nValueBits) << " " << Quantize(pN->y, nValueBits) << " " << Quantize(pN->z, nValueBits) << "\n";
		gph << Quantize(pN->var_x, nValueBits) << " " << Quantize(pN->var_y, nValueBits) << " " << Quantize(pN->var_z, nValueBits) << "\n";
		gph << Quantize(pN->convex, nValueBits) << "\n";

		gph << Quantize2(pN->mat[0], nValueBits) << " ";
		gph << Quantize2(pN->mat[3], nValueBits) << " ";
		gph << Quantize2(pN->mat[6], nValueBits) << " ";
		gph << Quantize2(pN->mat[1], nValueBits) << " ";
		gph << Quantize2(pN->mat[4], nValueBits) << " ";
		gph << Quantize2(pN->mat[7], nValueBits) << " ";
		gph << "\n";

		gph << 0 << " " << 0 << "\n";

		gph << pN->pNode.size() << "\n"; //
		for ( j = 0 ; j < pN->pNode.size() ; j++ ) {
			gph << pN->pNode[j]->idx << " ";	
		}
		gph << "\n\n";
	}
	gph.close();
}

void P3DS::AttributeCreate(CGraph *pGraph)
{
	int i, j;
	int nNode = pNodeArray.size();
	for ( i = 0 ; i < nNode ; i++ ) {
		CNode * pNode = pNodeArray[i];
		CInteger * pInteger = new CInteger;
		//pGraph->pIndexArray.Add(pInteger);
    pGraph->pIndexArray.push_back(pInteger);
		for ( j = 0 ; j < pNode->pNode.size() ; j++ ) {
			//pInteger->Add(pNode->pNode[j]->idx-1);
      pInteger->push_back(pNode->pNode[j]->idx-1);
		}
	}
	AttributeUnary(pGraph);
	AttributeBinary(pGraph);
}

void P3DS::AttributeUnary(CGraph *pGraph)
{
	int i;
	int nNode = pNodeArray.size();
	pGraph->pUnary = new CUnary [nNode];
	
	for ( i = 0 ; i < nNode ; i++ ) {
		CNode *pN = pNodeArray[i];
		pGraph->pUnary[i].f[0] = pN->vol; // f[0]: volume
		pGraph->pUnary[i].f[1] = pN->convex; // f[1]: convexity
		pGraph->pUnary[i].f[2] = 1-(pN->var_z*pN->var_z)/(pN->var_x*pN->var_x); // f[2]: e1
		if ( pGraph->pUnary[i].f[2] < 0 ) pGraph->pUnary[i].f[2] = 0;
		else pGraph->pUnary[i].f[2] = sqrt(pGraph->pUnary[i].f[2]);
		pGraph->pUnary[i].f[3] = 1-(pN->var_z*pN->var_z)/(pN->var_y*pN->var_y); // f[3]: e2
		if ( pGraph->pUnary[i].f[3] < 0 ) pGraph->pUnary[i].f[3] = 0;
		else pGraph->pUnary[i].f[3] = sqrt(pGraph->pUnary[i].f[3]);
	}
}

void P3DS::AttributeBinary(CGraph *pGraph)
{
	int i, j, k;
	int nNode = pNodeArray.size();
	pGraph->pBinary = new CBinary * [nNode];
	for ( i = 0 ; i < nNode ; i++ ) pGraph->pBinary[i] = new CBinary [nNode];

	int nIndex; double tmp;	
	for ( i = 0 ; i < nNode ; i++ ) {
		CNode *pNi = pNodeArray[i];	

		for ( j = i+1 ; j < nNode ; j++ ) {
			CNode *pNj = pNodeArray[j];

			nIndex = -1;
			
			////////////////////////////////////////////////
			// distance between centers of two nodes i, j
			////////////////////////////////////////////////
			nIndex++; // nIndex-st feature
			tmp = sqrt(pow(pNi->x-pNj->x,2)+pow(pNi->y-pNj->y,2)+pow(pNi->z-pNj->z,2));
			pGraph->pBinary[i][j].f[nIndex] = tmp;

			/////////////////////////////
			// angle between 1st & 1st
			/////////////////////////////
			nIndex++; // nIndex-st feature
			tmp = pNi->mat[0]*pNj->mat[0]+pNi->mat[3]*pNj->mat[3]+pNi->mat[6]*pNj->mat[6];
			tmp /= sqrt(pow(pNi->mat[0],2)+pow(pNi->mat[3],2)+pow(pNi->mat[6],2));
			tmp /= sqrt(pow(pNj->mat[0],2)+pow(pNj->mat[3],2)+pow(pNj->mat[6],2));
			tmp = fabs(tmp);
			pGraph->pBinary[i][j].f[nIndex] = tmp;

			/////////////////////////////
			// angle between 2nd & 2nd
			/////////////////////////////
			nIndex++; // nIndex-st feature
			tmp = pNi->mat[1]*pNj->mat[1]+pNi->mat[4]*pNj->mat[4]+pNi->mat[7]*pNj->mat[7];
			tmp /= sqrt(pow(pNi->mat[1],2)+pow(pNi->mat[4],2)+pow(pNi->mat[7],2));
			tmp /= sqrt(pow(pNj->mat[1],2)+pow(pNj->mat[4],2)+pow(pNj->mat[7],2));
			tmp = fabs(tmp);
			pGraph->pBinary[i][j].f[nIndex] = tmp;

			for ( k = 0 ; k < BINARY_FEATURE ; k++ ) {
				pGraph->pBinary[j][i].f[k] = pGraph->pBinary[i][j].f[k];
			}			
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//from GraphDef.cpp
/////////////////////////////////////////////////////////////////////////////
static float DistFunc1(const float *w, const float *u, void *param)
{
	int i, j;
	info_dst_d1 *mat = (info_dst_d1 *) param;

	for ( i = 0 ; i < mat->rows ; i++ ) {
		if ( w == &mat->x[i] ) break;
	}	

	for ( j = 0 ; j < mat->cols ; j++ ) {
		if ( u == &mat->y[j] ) break;
	}	
    
    return mat->mat[(--i)*_NODE_MAX+(--j)];
}

float CGraph::wEarth[_NODE_MAX]; float CGraph::uEarth[_NODE_MAX];
float CGraph::InnerDist[_NODE_MAX*_NODE_MAX];
float CGraph::OuterDist[_NODE_MAX*_NODE_MAX];

CGraph::CGraph() {
	pUnary = NULL; pBinary = NULL;
	diff = 0;
}

CGraph::~CGraph() {
	Clear();	
}

void CGraph::Clear()
{
	AttributeFree();

	int i;
	int nNode = pIndexArray.size();
	for ( i = nNode - 1 ; i >= 0 ; i-- ) {
    pIndexArray[i]->clear();
    //zhanghui noted: it seems this one is correct.
    //delete [] pIndexArray[i];
		delete pIndexArray[i];
	}
	pIndexArray.clear();
}

/*
CGraph::CGraph(const CGraph& graph) {
	pIndexArray.Copy(graph.pIndexArray);

	int i, j;
	int nNode = pIndexArray.size();	
	pUnary = new CUnary [nNode];
	pBinary = new CBinary * [nNode];
	for ( i = 0 ; i < nNode ; i++ ) {
		pUnary[i] = graph.pUnary[i];
		pBinary[i] = new CBinary [nNode];
		for ( j = 0 ; j < nNode ; j++ ) {
			pBinary[i][j] = graph.pBinary[i][j];
		}
	}
	diff = graph.diff;
}

const CGraph& CGraph::operator =(const CGraph& graph) {	
	pIndexArray.Copy(graph.pIndexArray);

	int i, j;
	int nNode = pIndexArray.size();
	pUnary = new CUnary [nNode];
	pBinary = new CBinary * [nNode];
	for ( i = 0 ; i < nNode ; i++ ) {
		pUnary[i] = graph.pUnary[i];
		pBinary[i] = new CBinary [nNode];
		for ( j = 0 ; j < nNode ; j++ ) {
			pBinary[i][j] = graph.pBinary[i][j];
		}
	}
	diff = graph.diff;
	return *this;
}

bool CGraph::operator ==(const CGraph& graph) const {
	return true;
}
*/

void CGraph::AttributeFree()
{
	if ( pUnary ) delete [] pUnary;

	int nNode = pIndexArray.size();
	if ( pBinary ) {		
		for ( int i = 0 ; i < nNode ; i++ )
			delete [] pBinary[i];
		delete [] pBinary;
	}
}

double CGraph::MatchFastEMDApart(CGraph *pModel, float *cost, float *flow, 
  double penalty, double weight)
{
	CGraph *pQuery = this;

	int nQuery = pQuery->pIndexArray.size();
	if ( nQuery == 1 ) return pModel->MatchSingleGraph(pQuery);

	int nModel = pModel->pIndexArray.size();
	if ( nModel == 1 ) return pQuery->MatchSingleGraph(pModel);	

	MatchFastEMDApartInnerNextImagine(pModel, penalty);
	
	int i, j;
	int nMax = max(nQuery, nModel);
	for ( i = 0 ; i < nQuery ; i++ ) wEarth[i] = (float) 1/nMax;
	for ( i = 0 ; i < nModel ; i++ ) uEarth[i] = (float) 1/nMax;

	bool bDelete = false;
	if ( !cost && !flow ) {
		cost = new float [nQuery*nModel];
		flow = new float [nQuery*nModel];
		bDelete = true;
	}

	double value = 0;
	for ( i = 0 ; i < nQuery ; i++ ) {
		CUnary & uQ = pQuery->pUnary[i];
		for ( j = 0 ; j < nModel ; j++ ) {
			CUnary & uM = pModel->pUnary[j];
			uQ.DiffNorm2(uM,value);

			cost[i*nModel+j] = (float) (weight*OuterDist[i*_NODE_MAX+j]) + (float) ((1-weight)*value);
			flow[i*nModel+j] = 0;
		}
	}
	
	CvMat mW = cvMat( nQuery, 1, CV_32F, wEarth );
	CvMat mU = cvMat( nModel, 1, CV_32F, uEarth );
	CvMat mCost = cvMat( nQuery, nModel, CV_32F, cost );
	CvMat mFlow = cvMat( nQuery, nModel, CV_32F, flow );

	double similarity = cvCalcEMD2(&mW, &mU, CV_DIST_USER, NULL, &mCost, &mFlow, 0, NULL);
	CostFastEMDApart(similarity,pModel,flow,penalty,weight);

	if ( bDelete ) {
		delete [] cost; delete [] flow; 
	}

	return similarity;
}

void CGraph::CostFastEMDApart(double & similarity, CGraph * pModel, float * flow, double & penalty, double & weight)
{	
	CGraph * pQuery = this;
	int nQuery = pQuery->pIndexArray.size();
	int nModel = pModel->pIndexArray.size();
		
	int i, j;
	double value = 0;
	if ( nQuery != nModel ) {
		if ( nQuery > nModel ) {
			for ( i = 0 ; i < nQuery ; i++ ) {
				bool bTest = false;
				for ( j = 0 ; j < nModel ; j++ ) {
					if ( flow[i*nModel+j] > FLT_EPSILON ) {	bTest = true; break; }
				}
				if ( !bTest ) {
					double diff = 0;
					CInteger *pQueryNode = pQuery->pIndexArray[i];
					int nQueryNode = pQueryNode->size();
					
					CUnary & uQ = pQuery->pUnary[i];	
					uQ.DiffNorm2Min(value); diff += (1-weight)*value;

					value = penalty*(nQueryNode+1)/nQuery; diff += weight*value;					

					similarity += (float) (diff/nQuery);					
				}
			}
		} else {
			for ( j = 0 ; j < nModel ; j++ ) {
				bool bTest = false;
				for ( i = 0 ; i < nQuery ; i++ ) {
					if ( flow[i*nModel+j] > FLT_EPSILON ) { bTest = true; break; }
				}
				if ( !bTest ) {
					double diff = 0;
					CInteger *pModelNode = pModel->pIndexArray[j];
					int nModelNode = pModelNode->size();

					CUnary & uM = pModel->pUnary[j];					
					uM.DiffNorm2Min(value); diff += (1-weight)*value;
					
					value = penalty*(nModelNode+1)/nModel; diff += weight*value;					

					similarity += (float) (diff/nModel);					
				}
			}
		}
	}
}

void CGraph::MatchFastEMDApartInnerNextImagine(CGraph *pModel, double penalty)
{
	CGraph *pQuery = this;

	int i, j, x, y;
	
	int nQuery = pQuery->pIndexArray.size();
	int nModel = pModel->pIndexArray.size();	

	int nMax = max(nQuery, nModel);

	double value = 0;

	float w_earth = 0; float u_earth = 0;
	for ( i = 0 ; i < nQuery ; i++ ) {
		CInteger *pQueryNode = pQuery->pIndexArray[i];
		int nQueryNode = pQueryNode->size();

		////////////////////////////////////////
		for ( x = 0 ; x < nQueryNode ; x++ ) wEarth[x] = (float) 1/nMax;
		wEarth[nQueryNode] = (float) 1/nMax;

		for ( j = 0 ; j < nModel ; j++ ) {			
			CInteger *pModelNode = pModel->pIndexArray[j];
			int nModelNode = pModelNode->size();

			////////////////////////////////////////			
			for ( y = 0 ; y < nModelNode ; y++ ) uEarth[y] = (float) 1/nMax;			
			uEarth[nModelNode] = (float) 1/nMax;
			
			w_earth = 0; u_earth = 0;
			for ( x = 0 ; x < nQueryNode + 1 ; x++ ) w_earth += wEarth[x];
			for ( y = 0 ; y < nModelNode + 1 ; y++ ) u_earth += uEarth[y];

			wEarth[nQueryNode+1] = 1 - w_earth;
			uEarth[nModelNode+1] = 1 - u_earth;

			if ( wEarth[nQueryNode+1] < 0 ) wEarth[nQueryNode+1] = 0;
			if ( uEarth[nModelNode+1] < 0 ) uEarth[nModelNode+1] = 0;

			////////////////////////////////////////						
			for ( x = 0 ; x < nQueryNode ; x++ ) {				
				CBinary & bQ = pQuery->pBinary[i][(*pQueryNode)[x]];
				for ( y = 0 ; y < nModelNode ; y++ ) {					
					CBinary & bM = pModel->pBinary[j][(*pModelNode)[y]];
					bQ.DiffNorm2(bM,value);
					InnerDist[x*_NODE_MAX+y] = (float) value;
				}
			}

			////////////////////////////////////////
			for ( x = 0 ; x < nQueryNode ; x++ ) {				
				CBinary & bQ = pQuery->pBinary[i][(*pQueryNode)[x]];
				bQ.DiffNorm2Zero(value);
				InnerDist[x*_NODE_MAX+nModelNode] = (float) value;
			}

			for ( y = 0 ; y < nModelNode ; y++ ) {				
				CBinary & bM = pModel->pBinary[j][(*pModelNode)[y]];
				bM.DiffNorm2Zero(value);
				InnerDist[nQueryNode*_NODE_MAX+y] = (float) value;
			}

			////////////////////////////////////////
			for ( x = 0 ; x < nQueryNode+1 ; x++ ) InnerDist[x*_NODE_MAX+(nModelNode+1)] = (float) penalty;
			for ( y = 0 ; y < nModelNode+1 ; y++ ) InnerDist[(nQueryNode+1)*_NODE_MAX+y] = (float) penalty;			
			////////////////////////////////////////

			InnerDist[nQueryNode*_NODE_MAX+nModelNode] = 0;
			InnerDist[(nQueryNode+1)*_NODE_MAX+(nModelNode+1)] = 0;
			////////////////////////////////////////
			mat_d1.x = wEarth; mat_d1.y = uEarth;
			mat_d1.rows = nQueryNode+2; mat_d1.cols = nModelNode+2;
			mat_d1.mat = InnerDist;

#if 1
			// 2014/11: replaced the depracated function cvCalcEMD with the new version cvCalcEMD2
			CvMat wEarthMat = cvMat( nQueryNode+2, 1, CV_32FC1, (void*)wEarth );
			CvMat uEarthMat = cvMat( nModelNode+2, 1, CV_32FC1, (void*)uEarth );

			OuterDist[i*_NODE_MAX+j] =
				cvCalcEMD2( &wEarthMat, &uEarthMat, CV_DIST_USER, DistFunc1, 0, 0, 0, &mat_d1);
#else
			// the old depraciated function
			OuterDist[i*_NODE_MAX+j] =
				cvCalcEMD(wEarth, nQueryNode+2, uEarth, nModelNode+2, 0, CV_DIST_USER, DistFunc1, 0, &mat_d1);
#endif // 
		}
	}
}

double CGraph::MatchSingleGraph(CGraph *pGraph)
{
	int i;	
	double value, dist = 0;	
	int nNode = pIndexArray.size();
	for ( i = 0 ; i < nNode ; i++ ) {		
		pGraph->pUnary[0].DiffNorm2(pUnary[i],value); // pGraph has one node
		dist += pUnary[i].f[0] * value;
	}

	return dist;
}

////////////////////////////////////////////////////////////////////////////////
//from Node.cpp
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CUnary

int    CUnary::nFeature;
int    CUnary::nStart;
double CUnary::pMin[UNARY_FEATURE];
double CUnary::pWeight[UNARY_FEATURE];

CUnary::CUnary() {	
	for ( int i = 0 ; i < UNARY_FEATURE ; i++ ) f[i] = 0;
}

CUnary::CUnary(const CUnary& unary) {	
	for ( int i = 0 ; i < UNARY_FEATURE ; i++ ) f[i] = unary.f[i];	
}

const CUnary& CUnary::operator =(const CUnary& unary) {		
	for ( int i = 0 ; i < UNARY_FEATURE ; i++ ) f[i] = unary.f[i];
	return *this;
}

bool CUnary::operator ==(const CUnary& unary) const {	
	return true;
}

void CUnary::DiffNorm2(CUnary &unary, double &diff)
{
	diff = 0; if ( !nFeature ) return;

	int i; double total = 0;
	for ( i = nStart ; i < nFeature ; i++ ) {
		diff += pWeight[i]*pow(f[i]-unary.f[i],2);
		total += pWeight[i];
	}
	diff /= total;
	diff = sqrt(diff);
}

void CUnary::DiffNorm2Zero(double &diff)
{
	diff = 0; if ( !nFeature ) return;

	int i; double total = 0;
	for ( i = nStart ; i < nFeature ; i++ ) {
		diff += pWeight[i]*pow(f[i],2);
		total += pWeight[i];
	}
	diff /= total;
	diff = sqrt(diff);
}

void CUnary::DiffNorm2Min(double &diff)
{
	diff = 0; if ( !nFeature ) return;

	int i; double total = 0;
	for ( i = nStart ; i < nFeature ; i++ ) {
		diff += pWeight[i]*pow(f[i]-pMin[i],2);
		total += pWeight[i];
	}
	diff /= total;
	diff = sqrt(diff);
}

/////////////////////////////////////////////////////////////////////////////
// CBinary

int    CBinary::nFeature;
double CBinary::pMin[BINARY_FEATURE];
double CBinary::pWeight[BINARY_FEATURE];

CBinary::CBinary() {	
	for ( int i = 0 ; i < BINARY_FEATURE ; i++ ) f[i] = 0;
}

CBinary::CBinary(const CBinary& binary) {
	for ( int i = 0 ; i < BINARY_FEATURE ; i++ ) f[i] = binary.f[i];
}

const CBinary& CBinary::operator =(const CBinary& binary) {
	for ( int i = 0 ; i < BINARY_FEATURE ; i++ ) f[i] = binary.f[i];
	return *this;
}

bool CBinary::operator ==(const CBinary& binary) const {	
	return true;
}

void CBinary::DiffNorm2(CBinary &binary, double &diff)
{
	diff = 0; if ( !nFeature ) return;

	for ( int i = 0 ; i < nFeature ; i++ ) {
		diff += pWeight[i]*pow(f[i]-binary.f[i],2);
	}
	diff = sqrt(diff);
}

void CBinary::DiffNorm2Zero(double &diff)
{
	diff = 0; if ( !nFeature ) return;

	for ( int i = 0 ; i < nFeature ; i++ ) {
		diff += pWeight[i]*pow(f[i],2);
	}
	diff = sqrt(diff);
}

void CBinary::DiffNorm2Min(double &diff)
{
	diff = 0; if ( !nFeature ) return;
	
	for ( int i = 0 ; i < nFeature ; i++ ) {
		diff += pWeight[i]*pow(f[i]-pMin[i],2);
	}
	diff = sqrt(diff);
}



////////////////////////////////////////////////////////////////////////////////////
//auxiliary functions for data quantization, and neighbourhood relation generation.

//this value is [0, 1], convert from byte (0~255).
inline float InvQuantize(unsigned long val, int nParaBits)
{return ((float)val+0.5F)/nParaBits;}
//this value is [-1, 1], convert from byte (0~255).
inline float InvQuantize2(unsigned long val, int nParaBits)
{return ((float)val*2+1)/nParaBits-1;}

//check whether there is a neighbouring relationship between i and j (i<j).
//pAdj is from IsConnected of P3DS descriptor.
BOOL CheckNeighbouring(long* pAdj, 
	int i, int j, 
	int nNodes)///<i: the total nodes number.
{
	assert(i<j);//this should be obeyed.
	int nStart = 0;
	for(int k=0;k<i;k++) nStart += nNodes-1-k;
	return (BOOL)(pAdj[nStart+j-i-1]);
}

inline float SqrLen(float a, float b, float c)
{
	return a*a+b*b+c*c;
}
////////////////////////////////////////////////////////////////////////////////
//new added member for CGraph.
//Convert standard data to members of CGraph.
//Based on the conversion in ::MakeGraph().
CGraph::CGraph(Perceptual3DShapeDescriptorInterfaceABC * pInterface)
{
	int i,j;
	//bits per attributes.
	int BitsPerAttributes = pInterface->GetPerceptual3DBitsPerAttribute();
	int nValueBits = (int)(0.5+pow((double)2, BitsPerAttributes));

	int nNodes = pInterface->GetPerceptual3DShapeNNodes();
  pUnary = new CUnary [nNodes];
  pBinary = new CBinary * [nNodes];
	for ( i = 0 ; i < nNodes ; i++ ) pBinary[i] = new CBinary [nNodes];

	for ( i = 0 ; i < nNodes ; i++ ) {CInteger * pNode = new CInteger;pIndexArray.push_back(pNode);}

//temporary data for mat.
  float* mat_all = (float*)malloc(sizeof(float*)*nNodes+9*nNodes*sizeof(float));
  float** mat = (float**)(mat_all+9*nNodes);
  for ( i = 0 ; i < nNodes ; i++ ) mat[i] = mat_all + 9*i;
	
	P3DS_COMPONENTS* pComps = pInterface->GetPerceptual3DShapeNodesAll();
	long* pAdj = pInterface->GetPerceptual3DShapeIsConnected();
  float volume_total = 0;
	for ( i = 0 ; i < nNodes ; i++ )
	{
		CInteger * pNode = pIndexArray[i];
		//pNode->idx = i+1;//starting from 1.


		pUnary[i].f[0] = InvQuantize(pComps[i].Volume, nValueBits); // f[0]: volume
		pUnary[i].f[1] = InvQuantize(pComps[i].Convexity, nValueBits); // f[1]: convexity

    volume_total += pUnary[i].f[0];

    float var_x, var_y, var_z;
		var_x = InvQuantize(pComps[i].Max_1, nValueBits);
		var_y = InvQuantize(pComps[i].Max_2, nValueBits);
		var_z = InvQuantize(pComps[i].Max_3, nValueBits);

		pUnary[i].f[2] = 1-(var_z*var_z)/(var_x*var_x); // f[2]: e1
		if ( pUnary[i].f[2] < 0 ) pUnary[i].f[2] = 0;
		else pUnary[i].f[2] = sqrt(pUnary[i].f[2]);
		pUnary[i].f[3] = 1-(var_z*var_z)/(var_y*var_y); // f[3]: e2
		if ( pUnary[i].f[3] < 0 ) pUnary[i].f[3] = 0;
		else pUnary[i].f[3] = sqrt(pUnary[i].f[3]);

		
		//the conversion from Transform_* to mat[9].
		mat[i][0] = InvQuantize2(pComps[i].PCA_Axis_1X, nValueBits);
		mat[i][3] = InvQuantize2(pComps[i].PCA_Axis_1Y, nValueBits);
		mat[i][6] = InvQuantize2(pComps[i].PCA_Axis_1Z, nValueBits);
		mat[i][1] = InvQuantize2(pComps[i].PCA_Axis_2X, nValueBits);
		mat[i][4] = InvQuantize2(pComps[i].PCA_Axis_2Y, nValueBits);
		mat[i][7] = InvQuantize2(pComps[i].PCA_Axis_2Z, nValueBits);
		//do the normalization.
		float len = sqrt(SqrLen(mat[i][0], mat[i][3], mat[i][6]));
		mat[i][0] /= len;mat[i][3] /= len;mat[i][6] /= len;
		len = sqrt(SqrLen(mat[i][1], mat[i][4], mat[i][7]));
		mat[i][1] /= len;mat[i][4] /= len;mat[i][7] /= len;

		//generate neighbouring nodes.
		for(j=0;j<i;j++)
		  if(CheckNeighbouring(pAdj, j, i, nNodes))
			pNode->push_back(j);
		for(j=i+1;j<nNodes;j++)
		  if(CheckNeighbouring(pAdj, i, j, nNodes))
			pNode->push_back(j);
	}

  //zhanghui 2005-11-10: also normalize the volume.
	for ( i = 0 ; i < nNodes ; i++ )
		pUnary[i].f[0] /= volume_total;

//similar with P3DS::AttributeBinary().
//zhanghui noted:
//strange: here so many binary relationship are created, no matter whether they are neighboring nodes...
  float tmp;
	for ( i = 0 ; i < nNodes ; i++ ) {
		for ( j = i+1 ; j < nNodes ; j++ ) {
			int nIndex = -1;
			
			////////////////////////////////////////////////
			// distance between centers of two nodes i, j
			////////////////////////////////////////////////
			nIndex++; // nIndex-st feature
      float xi, yi, zi, xj, yj, zj;
      xi = InvQuantize(pComps[i].Center_X, nValueBits);
      yi = InvQuantize(pComps[i].Center_Y, nValueBits);
      zi = InvQuantize(pComps[i].Center_Z, nValueBits);
      xj = InvQuantize(pComps[j].Center_X, nValueBits);
      yj = InvQuantize(pComps[j].Center_Y, nValueBits);
      zj = InvQuantize(pComps[j].Center_Z, nValueBits);
			tmp = sqrt(pow(xi-xj,2)+pow(yi-yj,2)+pow(zi-zj,2));
			pBinary[i][j].f[nIndex] = tmp;

			/////////////////////////////
			// angle between 1st & 1st
			/////////////////////////////
			nIndex++; // nIndex-st feature
			tmp = mat[i][0]*mat[j][0]+mat[i][3]*mat[j][3]+mat[i][6]*mat[j][6];
//zhanghui noted: no need to do this since it is already normalized.
			//tmp /= sqrt(pow(mat[i][0],2)+pow(mat[i][3],2)+pow(mat[i][6],2));
			//tmp /= sqrt(pow(mat[j][0],2)+pow(mat[j][3],2)+pow(mat[j][6],2));
			tmp = fabs(tmp);
			pBinary[i][j].f[nIndex] = tmp;

			/////////////////////////////
			// angle between 2nd & 2nd
			/////////////////////////////
			nIndex++; // nIndex-st feature
			tmp = mat[i][1]*mat[j][1]+mat[i][4]*mat[j][4]+mat[i][7]*mat[j][7];
			//tmp /= sqrt(pow(mat[i][1],2)+pow(mat[i][4],2)+pow(mat[i][7],2));
			//tmp /= sqrt(pow(mat[j][1],2)+pow(mat[j][4],2)+pow(mat[j][7],2));
			tmp = fabs(tmp);
			pBinary[i][j].f[nIndex] = tmp;

			for (int k = 0 ; k < BINARY_FEATURE ; k++ ) {
				pBinary[j][i].f[k] = pBinary[i][j].f[k];
			}			
		}
	}

  free(mat_all);

//	Save("d:\\ant_1.gph");
}


//////////////////////////////////////////////////////////////////////////////////
//new-added by ZhangHui, for quantization.
void P3DS::Quant(int nBits)
{
  int i;unsigned long tmp;
  int nNodes = pNodeArray.size();
	int nValueBits = (int)(0.5+pow((double)2, nBits));
	//now for every component.
	for(i=0;i<nNodes;i++)//for each component.
	{
		tmp = Quantize(pNodeArray[i]->vol, nValueBits);pNodeArray[i]->vol = InvQuantize(tmp, nValueBits);
		tmp = Quantize(pNodeArray[i]->x, nValueBits);pNodeArray[i]->x = InvQuantize(tmp, nValueBits);
		tmp = Quantize(pNodeArray[i]->y, nValueBits);pNodeArray[i]->y = InvQuantize(tmp, nValueBits);
		tmp = Quantize(pNodeArray[i]->z, nValueBits);pNodeArray[i]->z = InvQuantize(tmp, nValueBits);
		tmp = Quantize(pNodeArray[i]->var_x, nValueBits);pNodeArray[i]->var_x = InvQuantize(tmp, nValueBits);
		tmp = Quantize(pNodeArray[i]->var_y, nValueBits);pNodeArray[i]->var_y = InvQuantize(tmp, nValueBits);
		tmp = Quantize(pNodeArray[i]->var_z, nValueBits);pNodeArray[i]->var_z = InvQuantize(tmp, nValueBits);
		tmp = Quantize(pNodeArray[i]->convex, nValueBits);pNodeArray[i]->convex = InvQuantize(tmp, nValueBits);
		//the conversion from mat[9] into Transform_*.
		tmp = Quantize2(pNodeArray[i]->mat[0], nValueBits);pNodeArray[i]->mat[0] = InvQuantize2(tmp, nValueBits);
		tmp = Quantize2(pNodeArray[i]->mat[3], nValueBits);pNodeArray[i]->mat[3] = InvQuantize2(tmp, nValueBits);
		tmp = Quantize2(pNodeArray[i]->mat[6], nValueBits);pNodeArray[i]->mat[6] = InvQuantize2(tmp, nValueBits);
		tmp = Quantize2(pNodeArray[i]->mat[1], nValueBits);pNodeArray[i]->mat[1] = InvQuantize2(tmp, nValueBits);
		tmp = Quantize2(pNodeArray[i]->mat[4], nValueBits);pNodeArray[i]->mat[4] = InvQuantize2(tmp, nValueBits);
		tmp = Quantize2(pNodeArray[i]->mat[7], nValueBits);pNodeArray[i]->mat[7] = InvQuantize2(tmp, nValueBits);

		//do the normalization.
		float len = (float)sqrt(SqrLen(pNodeArray[i]->mat[0], pNodeArray[i]->mat[3], pNodeArray[i]->mat[6]));
		pNodeArray[i]->mat[0] /= len;pNodeArray[i]->mat[3] /= len;pNodeArray[i]->mat[6] /= len;
		len = (float)sqrt(SqrLen(pNodeArray[i]->mat[1], pNodeArray[i]->mat[4], pNodeArray[i]->mat[7]));
		pNodeArray[i]->mat[1] /= len;pNodeArray[i]->mat[4] /= len;pNodeArray[i]->mat[7] /= len;
	}
}


////////////////////////////////////////////////////////////////////////////////
//from Dot.cpp
/////////////////////////////////////////////////////////////////////////////

double CDot::Distance() {
	return sqrt(pow((double)x,2) + pow((double)y,2));
}

CVoxel::CVoxel()
{
	m_pData = NULL;

	m_nXSize = 0; m_nYSize = 0; m_nZSize = 0;

	root = 0; link = 0; leaf = 0;
	
	convexity = 0;
}

CVoxel::CVoxel(int XSize, int YSize, int ZSize)
{
	m_nXSize = XSize;
	m_nYSize = YSize;
	m_nZSize = ZSize;
	
	root = 0;
	link = 0;
	leaf = 0;
	
	convexity = 0;	
}

CVoxel::CVoxel(CVoxel &Source)
{	
	m_nXSize = Source.GetXSize();
	m_nYSize = Source.GetYSize();
	m_nZSize = Source.GetZSize();

	root = Source.root;
	link = Source.link;
	leaf = Source.leaf;
	
	convexity = Source.convexity;

    Allocate();

	int i, j, k;
	for ( i = 0 ; i < m_nXSize ; i++ ) {
		for ( j = 0 ; j < m_nYSize ; j++ ) {
			for ( k = 0 ; k < m_nZSize ; k++ ) {
				m_pData[i][j][k] = Source.m_pData[i][j][k];
			}
		}
	}
}

CVoxel::~CVoxel()
{
	Free();
}
	
void CVoxel::Free()
{
	if ( m_pData ) {
		int i, j;
		for ( i = 0 ; i < m_nXSize ; i++ ) {
			for ( j = 0 ; j < m_nYSize ; j++ ) {
				delete [] m_pData[i][j];
			}
			delete [] m_pData[i];
		} delete [] m_pData; m_pData = NULL;
	}
	m_nXSize = m_nYSize = m_nZSize = 0;
}

void CVoxel::Allocate()
{
	int i, j, k;
	m_pData = new int ** [m_nXSize];
	for ( i = 0 ; i < m_nXSize ; i++ ) {
		m_pData[i] = new int * [m_nYSize];
		for ( j = 0 ; j < m_nYSize ; j++ ) {
			m_pData[i][j] = new int [m_nZSize];
			for ( k = 0 ; k < m_nZSize ; k++ ) {
				m_pData[i][j][k] = 0;
			}
		}
	}
}

void CVoxel::Allocate(int XSize, int YSize, int ZSize)
{
	m_nXSize = XSize;
	m_nYSize = YSize;
	m_nZSize = ZSize;

	Allocate();	
}

int CVoxel::GetXSize() const
{
	return m_nXSize;
}

int CVoxel::GetYSize() const
{
	return m_nYSize;
}

int CVoxel::GetZSize() const
{
	return m_nZSize;
}

int CVoxel::Erode(CVoxel & SE)
{	
	int i, j, k;
	int x, y, z;
	int x_o, y_o, z_o;
	int x_d, y_d, z_d;

	x_o = SE.GetXSize()/2;
    y_o = SE.GetYSize()/2;
	z_o = SE.GetZSize()/2;

	int XSize = SE.GetXSize();
	int YSize = SE.GetYSize();
	int ZSize = SE.GetZSize();

	int count = 0;	

	CVoxel origin(*this);

	for ( i = 0 ; i < m_nXSize ; i++ ) {	
		for ( j = 0 ; j < m_nYSize ; j++ ) {
			for ( k = 0 ; k < m_nZSize ; k++ ) {
				if ( origin.m_pData[i][j][k] ) {
					bool test = false;
					x_d = i - x_o; y_d = j - y_o; z_d = k - z_o;
					for ( x = 0 ; x < XSize ; x++ ) {
						for ( y = 0 ; y < YSize ; y++ ) {
							for ( z = 0 ; z < ZSize ; z++ ) {
								if ( SE.m_pData[x][y][z] > 0 ) {
									if ( !(x_d + x < 0 || x_d + x >= m_nXSize)
										&& !(y_d + y < 0 || y_d + y >= m_nYSize)
										&& !(z_d + z < 0 || z_d + z >= m_nZSize) ) {
										if ( !origin.m_pData[x_d+x][y_d+y][z_d+z] ) {
											test = true; break;
										}
									} else {
										test = true; break;
									}
								}
								if ( test ) break;
							}
						}
						if ( test ) break;
					}
					if ( test ) {
						(*this).m_pData[i][j][k] = 0;
						count++;
					}
				}
			}
		}
	}

	return count;
}

int CVoxel::FastDilate(CVoxel & SE)
{
	int i, j, k;
	int x, y, z;
	int x_o, y_o, z_o;
	int x_d, y_d, z_d;

    x_o = SE.GetXSize()/2;
    y_o = SE.GetYSize()/2;
	z_o = SE.GetZSize()/2;

	int XSize = SE.GetXSize();
	int YSize = SE.GetYSize();
	int ZSize = SE.GetZSize();

	CVoxel bound;
	bound.Allocate(m_nXSize, m_nYSize, m_nZSize);

	int count = 0;
	for ( i = 0 ; i < m_nXSize ; i++ ) {
		for ( j = 0 ; j < m_nYSize ; j++ ) {
			for ( k = 0 ; k < m_nZSize ; k++ ) {
				if ( m_pData[i][j][k] ) {
					x = -1; y = 0; z = 0;
					if ( !(i + x < 0 || i + x >= m_nXSize)
						&& !(j + y < 0 || j + y >= m_nYSize)
						&& !(k + z < 0 || k + z >= m_nZSize)
						&& !m_pData[i+x][j+y][k+z] ) {
						bound.m_pData[i][j][k] = 1;
						count++;
					} else {
					x = 1; y = 0; z = 0;
					if ( !(i + x < 0 || i + x >= m_nXSize)
						&& !(j + y < 0 || j + y >= m_nYSize)
						&& !(k + z < 0 || k + z >= m_nZSize)
						&& !m_pData[i+x][j+y][k+z] ) {
						bound.m_pData[i][j][k] = 1;
						count++;
					} else {
					x = 0; y = -1; z = 0;
					if ( !(i + x < 0 || i + x >= m_nXSize)
						&& !(j + y < 0 || j + y >= m_nYSize)
						&& !(k + z < 0 || k + z >= m_nZSize)
						&& !m_pData[i+x][j+y][k+z] ) {
						bound.m_pData[i][j][k] = 1;
						count++;
					} else {
					x = 0; y = 1; z = 0;
					if ( !(i + x < 0 || i + x >= m_nXSize)
						&& !(j + y < 0 || j + y >= m_nYSize)
						&& !(k + z < 0 || k + z >= m_nZSize)
						&& !m_pData[i+x][j+y][k+z] ) {
						bound.m_pData[i][j][k] = 1;
						count++;
					} else {
					x = 0; y = 0; z = -1;
					if ( !(i + x < 0 || i + x >= m_nXSize)
						&& !(j + y < 0 || j + y >= m_nYSize)
						&& !(k + z < 0 || k + z >= m_nZSize)
						&& !m_pData[i+x][j+y][k+z] ) {
						bound.m_pData[i][j][k] = 1;
						count++;
					} else {
					x = 0; y = 0; z = 1;
					if ( !(i + x < 0 || i + x >= m_nXSize)
						&& !(j + y < 0 || j + y >= m_nYSize)
						&& !(k + z < 0 || k + z >= m_nZSize)
						&& !m_pData[i+x][j+y][k+z] ) {
						bound.m_pData[i][j][k] = 1;
						count++;
					}}}}}}
				}
			}
		}
	}
	if ( !count ) {
		for ( i = 0 ; i < m_nXSize ; i++ ) {
			for ( j = 0 ; j < m_nYSize ; j++ ) {
				for ( k = 0 ; k < m_nZSize ; k++ ) {
					bound.m_pData[i][j][k] = m_pData[i][j][k];
				}
			}
		}
	}
	
	for ( i = 0 ; i < m_nXSize ; i++ ) {
		for ( j = 0 ; j < m_nYSize ; j++ ) {
			for ( k = 0 ; k < m_nZSize ; k++ ) {
				if ( bound.m_pData[i][j][k] ) {					
					x_d = i - x_o; y_d = j - y_o; z_d = k - z_o;
					for ( x = 0 ; x < XSize ; x++ ) {
						for ( y = 0 ; y < YSize ; y++ ) {
							for ( z = 0 ; z < ZSize ; z++ ) {
								if ( !(x_d + x < 0 || x_d + x >= m_nXSize)
									&& !(y_d + y < 0 || y_d + y >= m_nYSize)
									&& !(z_d + z < 0 || z_d + z >= m_nZSize)
									&& SE.m_pData[x][y][z] ) {
									m_pData[x_d+x][y_d+y][z_d+z] = 1;
								}
							}
						}
					}
				}
			}
		}
	}

	return GetNumber();
}

int CVoxel::FastErode(CVoxel & SE)
{	
	int i, j, k;
	int x, y, z;
	int x_o, y_o, z_o;
	int x_d, y_d, z_d;

    x_o = SE.GetXSize()/2;
    y_o = SE.GetYSize()/2;
	z_o = SE.GetZSize()/2;

	int XSize = SE.GetXSize();
	int YSize = SE.GetYSize();
	int ZSize = SE.GetZSize();

	CVoxel bound;
	bound.Allocate(m_nXSize, m_nYSize, m_nZSize);

	int count = 0;
	for ( i = 0 ; i < m_nXSize ; i++ ) {
		for ( j = 0 ; j < m_nYSize ; j++ ) {
			for ( k = 0 ; k < m_nZSize ; k++ ) {
				if ( !m_pData[i][j][k] ) {					
					x = -1; y = 0; z = 0;
					if ( !(i + x < 0 || i + x >= m_nXSize)
						&& !(j + y < 0 || j + y >= m_nYSize)
						&& !(k + z < 0 || k + z >= m_nZSize)
						&& m_pData[i+x][j+y][k+z] ) {
						bound.m_pData[i][j][k] = 1;
						count++;
					} else {
					x = 1; y = 0; z = 0;
					if ( !(i + x < 0 || i + x >= m_nXSize)
						&& !(j + y < 0 || j + y >= m_nYSize)
						&& !(k + z < 0 || k + z >= m_nZSize)
						&& m_pData[i+x][j+y][k+z] ) {
						bound.m_pData[i][j][k] = 1;
						count++;
					} else {
					x = 0; y = -1; z = 0;
					if ( !(i + x < 0 || i + x >= m_nXSize)
						&& !(j + y < 0 || j + y >= m_nYSize)
						&& !(k + z < 0 || k + z >= m_nZSize)
						&& m_pData[i+x][j+y][k+z] ) {
						bound.m_pData[i][j][k] = 1;
						count++;
					} else {
					x = 0; y = 1; z = 0;
					if ( !(i + x < 0 || i + x >= m_nXSize)
						&& !(j + y < 0 || j + y >= m_nYSize)
						&& !(k + z < 0 || k + z >= m_nZSize)
						&& m_pData[i+x][j+y][k+z] ) {
						bound.m_pData[i][j][k] = 1;
						count++;
					} else {
					x = 0; y = 0; z = -1;
					if ( !(i + x < 0 || i + x >= m_nXSize)
						&& !(j + y < 0 || j + y >= m_nYSize)
						&& !(k + z < 0 || k + z >= m_nZSize)
						&& m_pData[i+x][j+y][k+z] ) {
						bound.m_pData[i][j][k] = 1;
						count++;
					} else {
					x = 0; y = 0; z = 1;
					if ( !(i + x < 0 || i + x >= m_nXSize)
						&& !(j + y < 0 || j + y >= m_nYSize)
						&& !(k + z < 0 || k + z >= m_nZSize)
						&& m_pData[i+x][j+y][k+z] ) {
						bound.m_pData[i][j][k] = 1;
						count++;
					}}}}}}
				}
			}
		}
	}
	if ( !count ) {
		for ( i = 0 ; i < m_nXSize ; i++ ) {
			for ( j = 0 ; j < m_nYSize ; j++ ) {
				for ( k = 0 ; k < m_nZSize ; k++ ) {
					bound.m_pData[i][j][k] = m_pData[i][j][k];
				}
			}
		}
	}
	
	for ( i = 0 ; i < m_nXSize ; i++ ) {
		for ( j = 0 ; j < m_nYSize ; j++ ) {
			for ( k = 0 ; k < m_nZSize ; k++ ) {
				if ( bound.m_pData[i][j][k] ) {
					x_d = i - x_o; y_d = j - y_o; z_d = k - z_o;
					for ( x = 0 ; x < XSize ; x++ ) {
						for ( y = 0 ; y < YSize ; y++ ) {
							for ( z = 0 ; z < ZSize ; z++ ) {
								if ( !(x_d + x < 0 || x_d + x >= m_nXSize)
									&& !(y_d + y < 0 || y_d + y >= m_nYSize)
									&& !(z_d + z < 0 || z_d + z >= m_nZSize)
									&& SE.m_pData[x][y][z] ) {
									m_pData[x_d+x][y_d+y][z_d+z] = 0;
								}
							}
						}
					}
				}
			}
		}
	}

	return GetNumber();
}

int CVoxel::GetNumber()
{
	int i, j, k;
	int count = 0;
	for ( i = 0 ; i < m_nXSize ; i++ ) {
		for ( j = 0 ; j < m_nYSize ; j++ ) {
			for ( k = 0 ; k < m_nZSize ; k++ ) {
				if ( (*this).m_pData[i][j][k] ) count++;
			}
		}
	}

	return count;
}

int CVoxel::Boundary()
{
	int i, j, k;

	CVoxel Origin, SE;
	Origin.Allocate(m_nXSize, m_nYSize, m_nZSize);
	SE.Allocate(3,3,3);

	///// 6-adjacency /////	
	SE.m_pData[1][1][1] = 1;
	SE.m_pData[1][1][0] = 1;
	SE.m_pData[1][1][2] = 1;
	SE.m_pData[1][0][1] = 1;
	SE.m_pData[1][2][1] = 1;
	SE.m_pData[0][1][1] = 1;
	SE.m_pData[2][1][1] = 1;

	int count = 0;

	for ( i = 0 ; i < m_nXSize ; i++ ) {
		for ( j = 0 ; j < m_nYSize ; j++ ) {
			for ( k = 0 ; k < m_nZSize ; k++ ) {
				Origin.m_pData[i][j][k] = m_pData[i][j][k];
			}
		}
	}

    Erode(SE);

    for ( i = 0 ; i < m_nXSize ; i++ ) {
		for ( j = 0 ; j < m_nYSize ; j++ ) {
			for ( k = 0 ; k < m_nZSize ; k++ ) {
				if ( Origin.m_pData[i][j][k] && !(*this).m_pData[i][j][k] ) {
					(*this).m_pData[i][j][k] = 1;
					count++;
				}
				else {
					(*this).m_pData[i][j][k] = 0;
				}
			}
		}
	}

	return count;
}

int CVoxel::Component()
{
	int i, j, k;
	int x, y, z;
	int a, b, c;
	int num = 0;
	int XSize = m_nXSize;
	int YSize = m_nYSize;
	int ZSize = m_nZSize;
	
	CVoxel cc;
    cc.Allocate(XSize, YSize, ZSize);

    for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				if ( (*this).m_pData[i][j][k] && !cc.m_pData[i][j][k] ) {
					num++;                
                
					CDotArray pArray;
					CDot * pDot = new CDot;
					pDot->x = i; pDot->y = j; pDot->z = k;
					//pArray.Add(pDot);
          pArray.push_back(pDot);
					cc.m_pData[i][j][k] = num;

					while ( pArray.size() ) {					
						x = pArray[0]->x; y = pArray[0]->y; z = pArray[0]->z;
                    
						a = -1; b = 0; c = 0;
						if ( !(x+a < 0 || x+a >= XSize) 
							&& !(y+b < 0 || y+b >= YSize)
							&& !(z+c < 0 || z+c >= ZSize)
							&& !cc.m_pData[x+a][y+b][z+c]
							&& (*this).m_pData[x+a][y+b][z+c]) {
							pDot = new CDot;
							pDot->x = x+a; pDot->y = y+b; pDot->z = z+c;
							//pArray.Add(pDot);
              pArray.push_back(pDot);
							cc.m_pData[x+a][y+b][z+c] = num;
						}
						a = 1; b = 0; c = 0;
						if ( !(x+a < 0 || x+a >= XSize) 
							&& !(y+b < 0 || y+b >= YSize)
							&& !(z+c < 0 || z+c >= ZSize)
							&& !cc.m_pData[x+a][y+b][z+c]
							&& (*this).m_pData[x+a][y+b][z+c]) {
							pDot = new CDot;
							pDot->x = x+a; pDot->y = y+b; pDot->z = z+c;
							pArray.push_back(pDot);//pArray.Add(pDot);
							cc.m_pData[x+a][y+b][z+c] = num;
						}
						a = 0; b = -1; c = 0;
						if ( !(x+a < 0 || x+a >= XSize) 
							&& !(y+b < 0 || y+b >= YSize)
							&& !(z+c < 0 || z+c >= ZSize)
							&& !cc.m_pData[x+a][y+b][z+c]
							&& (*this).m_pData[x+a][y+b][z+c]) {
							pDot = new CDot;
							pDot->x = x+a; pDot->y = y+b; pDot->z = z+c;
							pArray.push_back(pDot);//pArray.Add(pDot);
							cc.m_pData[x+a][y+b][z+c] = num;
						}
						a = 0; b = 1; c = 0;
						if ( !(x+a < 0 || x+a >= XSize) 
							&& !(y+b < 0 || y+b >= YSize)
							&& !(z+c < 0 || z+c >= ZSize)
							&& !cc.m_pData[x+a][y+b][z+c]
							&& (*this).m_pData[x+a][y+b][z+c]) {
							pDot = new CDot;
							pDot->x = x+a; pDot->y = y+b; pDot->z = z+c;
							pArray.push_back(pDot);//pArray.Add(pDot);
							cc.m_pData[x+a][y+b][z+c] = num;
						}
						a = 0; b = 0; c = -1;
						if ( !(x+a < 0 || x+a >= XSize) 
							&& !(y+b < 0 || y+b >= YSize)
							&& !(z+c < 0 || z+c >= ZSize)
							&& !cc.m_pData[x+a][y+b][z+c]
							&& (*this).m_pData[x+a][y+b][z+c]) {
							pDot = new CDot;
							pDot->x = x+a; pDot->y = y+b; pDot->z = z+c;
							pArray.push_back(pDot);//pArray.Add(pDot);
							cc.m_pData[x+a][y+b][z+c] = num;
						}
						a = 0; b = 0; c = 1;
						if ( !(x+a < 0 || x+a >= XSize) 
							&& !(y+b < 0 || y+b >= YSize)
							&& !(z+c < 0 || z+c >= ZSize)
							&& !cc.m_pData[x+a][y+b][z+c]
							&& (*this).m_pData[x+a][y+b][z+c]) {
							pDot = new CDot;
							pDot->x = x+a; pDot->y = y+b; pDot->z = z+c;
							pArray.push_back(pDot);//pArray.Add(pDot);
							cc.m_pData[x+a][y+b][z+c] = num;
						}

						pDot = pArray[0];
						//pArray.RemoveAt(0);delete [] pDot;
            pArray.erase(pArray.begin());delete pDot;
					}
				}
			}
		}
	}

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				(*this).m_pData[i][j][k] = cc.m_pData[i][j][k];
			}
		}
	}

	return num;
}

int CVoxel::Hull()
{
	int i, j, k;

	int XSize = GetXSize();
	int YSize = GetYSize();
	int ZSize = GetZSize();

	CVoxel bound;
	bound.Allocate(XSize, YSize, ZSize);
	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				bound.m_pData[i][j][k] = m_pData[i][j][k];
			}
		}
	}
	bound.Boundary();

	int dim = 3;				// dimension of points
	int numpoints = bound.GetNumber();		// number of points
	if ( numpoints < 3 ) return numpoints;	
	
	boolT ismalloc= False;    // True if qhull should free points in qh_freeqhull() or reallocation
	char flags[250];          // option flags for qhull, see qh_opt.htm
	FILE *outfile= NULL;    // output from qh_produce_output() use NULL to skip qh_produce_output()	
	FILE *errfile= stderr;    // error messages from qhull code
	int exitcode;             // 0 if no error from qhull
	facetT *facet;	    /* set by FORALLfacets */
	int curlong, totlong;	  // memory remaining after qh_memfreeshort

    // Run 1: convex hull

	int nNum = 0;
	coordT *points = (coordT*)malloc((numpoints)*(dim)*sizeof(coordT));
	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				if ( bound.m_pData[i][j][k] ) {
					points[dim*nNum]   = i;
					points[dim*nNum+1] = j;
					points[dim*nNum+2] = k;
					nNum++;
				}
			}
		}
	}
	
	sprintf (flags, "qhull QJ FA");	
	exitcode= qh_new_qhull (dim, numpoints, points, ismalloc, flags, outfile, errfile);	

	CVoxel hull;
	hull.Allocate(XSize, YSize, ZSize);

	int nHull = 0;
	if (!exitcode) {                  // if no error
		int index[3];
		CVertex A, B, C, max, min;		
		min.x = 0.5; min.y = 0.5; min.z = 0.5;
		max.x = XSize+0.5; max.y = YSize+0.5; max.z = ZSize+0.5;
		//min.x = 0; min.y = 0; min.z = 0;
		//max.x = XSize-1; max.y = YSize-1; max.z = ZSize-1;

		FORALLfacets {
			vertexT *vertex, **vertexp;
			setT *vertices;

			vertices= qh_facet3vertex (facet);

			int count = 0;
			FOREACHvertex_(vertices) {
				index[count++] = qh_pointid(vertex->point);
			}
			qh_settempfree(&vertices);

			A.x = points[dim*index[0]]+0.5; A.y = points[dim*index[0]+1]+0.5; A.z = points[dim*index[0]+2]+0.5;
			B.x = points[dim*index[1]]+0.5; B.y = points[dim*index[1]+1]+0.5; B.z = points[dim*index[1]+2]+0.5;
			C.x = points[dim*index[2]]+0.5; C.y = points[dim*index[2]+1]+0.5; C.z = points[dim*index[2]+2]+0.5;

			hull.GetVoxel(A, B, C, max, min);
		}
		hull.Filling();		

		nHull = hull.GetNumber();
	} else nHull = GetNumber();

	qh_freeqhull(!qh_ALL);                   // free long memory
	qh_memfreeshort (&curlong, &totlong);    // free short memory and memory allocator

//zhanghui changed:
	//delete[] points;
  free(points);

	return nHull;
}

int CVoxel::Hull(CDotArray & pArray)
{
	int i, j, k;

	int XSize = GetXSize();
	int YSize = GetYSize();
	int ZSize = GetZSize();

	CVoxel bound;
	bound.Allocate(XSize, YSize, ZSize);
	for ( i = 0 ; i < pArray.size() ; i++ ) {
		bound.m_pData[pArray[i]->x][pArray[i]->y][pArray[i]->z] = 1;
	}	
	bound.Boundary();

	int dim = 3;				// dimension of points
	int numpoints = bound.GetNumber();		// number of points
	if ( numpoints < 3 ) return numpoints;
	
	boolT ismalloc= False;    // True if qhull should free points in qh_freeqhull() or reallocation
	char flags[250];          // option flags for qhull, see qh_opt.htm
	FILE *outfile= NULL;    // output from qh_produce_output() use NULL to skip qh_produce_output()	
	FILE *errfile= stderr;    // error messages from qhull code
	int exitcode;             // 0 if no error from qhull
	facetT *facet;	    // set by FORALLfacets
	int curlong, totlong;	  // memory remaining after qh_memfreeshort

	// Run 1: convex hull

	int nNum = 0;
	coordT *points = (coordT*)malloc((numpoints)*(dim)*sizeof(coordT));
	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				if ( bound.m_pData[i][j][k] ) {
					points[dim*nNum]   = i;
					points[dim*nNum+1] = j;
					points[dim*nNum+2] = k;
					nNum++;
				}
			}
		}
	}
		
	sprintf (flags, "qhull QJ FA");	
	exitcode= qh_new_qhull (dim, numpoints, points, ismalloc, flags, outfile, errfile);	

	CVoxel hull;
	hull.Allocate(m_nXSize, m_nYSize, m_nZSize);

	int nHull = 0;
	if (!exitcode) {                  // if no error
		int index[3];
		CVertex A, B, C, max, min;		
		min.x = 0.5; min.y = 0.5; min.z = 0.5;
		max.x = m_nXSize+0.5; max.y = m_nYSize+0.5; max.z = m_nZSize+0.5;

		FORALLfacets {
			vertexT *vertex, **vertexp;
			setT *vertices;

			vertices= qh_facet3vertex (facet);

			int count = 0;
			FOREACHvertex_(vertices) {
				index[count++] = qh_pointid(vertex->point);
			}
			qh_settempfree(&vertices);

			A.x = points[dim*index[0]]+0.5; A.y = points[dim*index[0]+1]+0.5; A.z = points[dim*index[0]+2]+0.5;
			B.x = points[dim*index[1]]+0.5; B.y = points[dim*index[1]+1]+0.5; B.z = points[dim*index[1]+2]+0.5;
			C.x = points[dim*index[2]]+0.5; C.y = points[dim*index[2]+1]+0.5; C.z = points[dim*index[2]+2]+0.5;

			hull.GetVoxel(A, B, C, max, min);
		}
		hull.Filling();		

		nHull = hull.GetNumber();
	} else nHull = pArray.size();

	qh_freeqhull(!qh_ALL);                   // free long memory
	qh_memfreeshort (&curlong, &totlong);    // free short memory and memory allocator

//zhanghui changed:
	//delete[] points;
  free(points);

	return nHull;
}

int CVoxel::MakeHull()
{
	int i, j, k;

	int XSize = GetXSize();
	int YSize = GetYSize();
	int ZSize = GetZSize();

	CVoxel bound;
	bound.Allocate(XSize, YSize, ZSize);
	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				bound.m_pData[i][j][k] = m_pData[i][j][k];
			}
		}
	}
	bound.Boundary();

	int dim = 3;				// dimension of points
	int numpoints = bound.GetNumber();		// number of points
	if ( numpoints < 3 ) return numpoints;	
	
	boolT ismalloc= False;    // True if qhull should free points in qh_freeqhull() or reallocation
	char flags[250];          // option flags for qhull, see qh_opt.htm
	FILE *outfile= NULL;    // output from qh_produce_output() use NULL to skip qh_produce_output()	
	FILE *errfile= stderr;    // error messages from qhull code
	int exitcode;             // 0 if no error from qhull
	facetT *facet;	    /* set by FORALLfacets */
	int curlong, totlong;	  // memory remaining after qh_memfreeshort

    // Run 1: convex hull

	int nNum = 0;
	coordT *points = (coordT*)malloc((numpoints)*(dim)*sizeof(coordT));
	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				if ( bound.m_pData[i][j][k] ) {
					points[dim*nNum]   = i;
					points[dim*nNum+1] = j;
					points[dim*nNum+2] = k;
					nNum++;
				}
			}
		}
	}
		
	sprintf (flags, "qhull QJ FA");	
	exitcode= qh_new_qhull (dim, numpoints, points, ismalloc, flags, outfile, errfile);
	
	CVoxel hull;
	hull.Allocate(XSize, YSize, ZSize);

	if (!exitcode) {                  // if no error
		int index[3];
		CVertex A, B, C, max, min;
		min.x = 0.5; min.y = 0.5; min.z = 0.5;
		max.x = XSize+0.5; max.y = YSize+0.5; max.z = ZSize+0.5;
		//min.x = 0; min.y = 0; min.z = 0;
		//max.x = XSize-1; max.y = YSize-1; max.z = ZSize-1;

		FORALLfacets {
			vertexT *vertex, **vertexp;
			setT *vertices;

			vertices= qh_facet3vertex (facet);

			int count = 0;
			FOREACHvertex_(vertices) {
				index[count++] = qh_pointid(vertex->point);			
			}
			qh_settempfree(&vertices);

			A.x = points[dim*index[0]]+0.5; A.y = points[dim*index[0]+1]+0.5; A.z = points[dim*index[0]+2]+0.5;
			B.x = points[dim*index[1]]+0.5; B.y = points[dim*index[1]+1]+0.5; B.z = points[dim*index[1]+2]+0.5;
			C.x = points[dim*index[2]]+0.5; C.y = points[dim*index[2]+1]+0.5; C.z = points[dim*index[2]+2]+0.5;

			hull.GetVoxel(A, B, C, max, min);
		}
		hull.Filling();		

		for ( i = 0 ; i < XSize ; i++ ) {
			for ( j = 0 ; j < YSize ; j++ ) {
				for ( k = 0 ; k < ZSize ; k++ ) {
					m_pData[i][j][k] = hull.m_pData[i][j][k];
				}
			}
		}
	}

	qh_freeqhull(!qh_ALL);                   // free long memory
	qh_memfreeshort (&curlong, &totlong);    // free short memory and memory allocator

//zhanghui changed:
	//delete[] points;
  free(points);

	return (*this).GetNumber();
}

void CVoxel::Filling()
{
    ////내부를 채우는 알고리듬 
	int i,j,k;
    int x,y,z;
	int count = 0;

	int XSize = m_nXSize;
	int YSize = m_nYSize;
	int ZSize = m_nZSize;

	CVoxel Voxel;
	Voxel.Allocate(XSize,YSize,ZSize);
    
    /// shape에서 모두 1로 채우자
    for ( i = 0; i < XSize ; i++ ) {
		for ( j = 0; j < YSize ; j++ ) {
			for ( k = 0; k < ZSize ; k++ ) {
				Voxel.m_pData[i][j][k] = 1;
			}
		}
	}

    /// 최초 스캔을 하면서 
    /// ( shape = 1 ) 에 대해서 
    /// 1) 그 복셀이 0 이면서  
    /// 2) 6-connected중 하나라도 외부공간(shape == 0)이 존재할때 
    /// shape을 1->0 으로 바꾼다.

    for (i=0; i<XSize; i++){
        for ( j=0; j<YSize; j++){
            for ( k=0; k<ZSize ; k++){
                /// shape이 1이고, in_v가 0인 경우에 대해서 
                if ( Voxel.m_pData[i][j][k] && !m_pData[i][j][k] ){
                    /// 6-connected에 대해서 비교
					bool test = false;
					x = -1; y = 0; z = 0;
					if ( !(i+x < 0 || i+x > XSize-1) 
						&& !(j+y < 0 || j+y > YSize-1) 
						&& !(k+z < 0 || k+z > ZSize-1) ) {
						if ( !Voxel.m_pData[i+x][j+y][k+z] )
							test = true;						
					} else
						test = true;
					x = 1; y = 0; z = 0;
					if ( !(i+x < 0 || i+x > XSize-1) 
						&& !(j+y < 0 || j+y > YSize-1) 
						&& !(k+z < 0 || k+z > ZSize-1) ) {
						if ( !Voxel.m_pData[i+x][j+y][k+z] )
							test = true;						
					} else
						test = true;
					x = 0; y = -1; z = 0;
					if ( !(i+x < 0 || i+x > XSize-1) 
						&& !(j+y < 0 || j+y > YSize-1) 
						&& !(k+z < 0 || k+z > ZSize-1) ) {
						if ( !Voxel.m_pData[i+x][j+y][k+z] )
							test = true;						
					} else
						test = true;
					x = 0; y = 1; z = 0;
					if ( !(i+x < 0 || i+x > XSize-1) 
						&& !(j+y < 0 || j+y > YSize-1) 
						&& !(k+z < 0 || k+z > ZSize-1) ) {
						if ( !Voxel.m_pData[i+x][j+y][k+z] )
							test = true;						
					} else
						test = true;
					x = 0; y = 0; z = -1;
					if ( !(i+x < 0 || i+x > XSize-1) 
						&& !(j+y < 0 || j+y > YSize-1) 
						&& !(k+z < 0 || k+z > ZSize-1) ) {
						if ( !Voxel.m_pData[i+x][j+y][k+z] )
							test = true;						
					} else
						test = true;
					x = 0; y = 0; z = 1;
					if ( !(i+x < 0 || i+x > XSize-1) 
						&& !(j+y < 0 || j+y > YSize-1) 
						&& !(k+z < 0 || k+z > ZSize-1) ) {
						if ( !Voxel.m_pData[i+x][j+y][k+z] )
							test = true;						
					} else
						test = true;
					if ( test ) {
						Voxel.m_pData[i][j][k] = 0;
					}
                }
            }
        }
    }

    //iteration
    int itr = 0;
    bool conti = true;
    while (conti){
        itr++;
        conti = false;
        for (i=0; i<XSize; i++){
			for ( j=0; j<YSize; j++){
				for ( k=0; k<ZSize ; k++){
					/// shape이 1이고, in_v가 0인 경우에 대해서 
					if ( Voxel.m_pData[i][j][k] && !m_pData[i][j][k] ){
						/// 6-connected에 대해서 비교
						bool test = false;
						x = -1; y = 0; z = 0;
						if ( !(i+x < 0 || i+x > XSize-1) 
							&& !(j+y < 0 || j+y > YSize-1) 
							&& !(k+z < 0 || k+z > ZSize-1) ) {
							if ( !Voxel.m_pData[i+x][j+y][k+z] )
								test = true;						
						} else
							test = true;
						x = 1; y = 0; z = 0;
						if ( !(i+x < 0 || i+x > XSize-1) 
							&& !(j+y < 0 || j+y > YSize-1) 
							&& !(k+z < 0 || k+z > ZSize-1) ) {
							if ( !Voxel.m_pData[i+x][j+y][k+z] )
								test = true;						
						} else
							test = true;
						x = 0; y = -1; z = 0;
						if ( !(i+x < 0 || i+x > XSize-1) 
							&& !(j+y < 0 || j+y > YSize-1) 
							&& !(k+z < 0 || k+z > ZSize-1) ) {
							if ( !Voxel.m_pData[i+x][j+y][k+z] )
								test = true;						
						} else
							test = true;
						x = 0; y = 1; z = 0;
						if ( !(i+x < 0 || i+x > XSize-1) 
							&& !(j+y < 0 || j+y > YSize-1) 
							&& !(k+z < 0 || k+z > ZSize-1) ) {
							if ( !Voxel.m_pData[i+x][j+y][k+z] )
								test = true;						
						} else
							test = true;
						x = 0; y = 0; z = -1;
						if ( !(i+x < 0 || i+x > XSize-1) 
							&& !(j+y < 0 || j+y > YSize-1) 
							&& !(k+z < 0 || k+z > ZSize-1) ) {
							if ( !Voxel.m_pData[i+x][j+y][k+z] )
								test = true;						
						} else
							test = true;
						x = 0; y = 0; z = 1;
						if ( !(i+x < 0 || i+x > XSize-1) 
							&& !(j+y < 0 || j+y > YSize-1) 
							&& !(k+z < 0 || k+z > ZSize-1) ) {
							if ( !Voxel.m_pData[i+x][j+y][k+z] )
								test = true;						
						} else
							test = true;
						if ( test ) {
							Voxel.m_pData[i][j][k] = 0;
							conti = true;
						}
					}
				}
			}
		}
    }

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				m_pData[i][j][k] = Voxel.m_pData[i][j][k];
			}
		}
	}
}

int CVoxel::Simply()
{
	int i, j ,k;	
	int XSize = GetXSize();
	int YSize = GetYSize();
	int ZSize = GetZSize();

	CVoxel bench;
	bench.Allocate(XSize, YSize, ZSize);

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				if ( m_pData[i][j][k] ) {
					bench.m_pData[i][j][k] = 1;
				}
			}
		}
	}
	
	int comp = bench.Component();
	int * num_of_comp = new int [comp];
	for ( i = 0 ; i < comp ; i++ ) num_of_comp[i] = 0;	

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				if ( bench.m_pData[i][j][k] ) {
					num_of_comp[bench.m_pData[i][j][k]-1]++;
				}
			}
		}
	}

	int index = 0;
	for ( i = 1 ; i < comp ; i++ ) {
		if ( num_of_comp[i] > num_of_comp[index] )
			index = i;
	}

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				if ( bench.m_pData[i][j][k] && bench.m_pData[i][j][k] != index+1 ) {
					m_pData[i][j][k] = 0;
				}
			}
		}
	}

	delete num_of_comp;

	return comp;
}

void CVoxel::GetVoxel(CVertex &A, CVertex &B, CVertex &C, CVertex &max, CVertex &min)
{
	CVertex Left, Mid, Right;
	int x_max, y_max, z_max;
	int x_min, y_min, z_min;

	int XSize = m_nXSize;
	int YSize = m_nYSize;
	int ZSize = m_nZSize;

	x_max = (int) floor(XSize*(max(max(A.x,B.x),C.x) - min.x)/(max.x - min.x));
	y_max = (int) floor(YSize*(max(max(A.y,B.y),C.y) - min.y)/(max.y - min.y));
	z_max = (int) floor(ZSize*(max(max(A.z,B.z),C.z) - min.z)/(max.z - min.z));

	x_min = (int) floor(XSize*(min(min(A.x,B.x),C.x) - min.x)/(max.x - min.x));
	y_min = (int) floor(YSize*(min(min(A.y,B.y),C.y) - min.y)/(max.y - min.y));
	z_min = (int) floor(ZSize*(min(min(A.z,B.z),C.z) - min.z)/(max.z - min.z));

	if ( x_max == XSize ) x_max--;	
	if ( y_max == YSize ) y_max--;	
	if ( z_max == ZSize ) z_max--;

	if ( x_min == x_max && y_min == y_max && z_min == z_max ) {
		m_pData[x_min][y_min][z_min] = 1;
		return;
	}
	
	//why three times?
	GetVoxelLine(A,B,max,min);
	GetVoxelLine(B,C,max,min);
	GetVoxelLine(C,A,max,min);	

	GetVoxelLine(B,A,C,max,min);
	GetVoxelLine(A,B,C,max,min);
	GetVoxelLine(A,C,B,max,min);
}

void CVoxel::GetVoxelLine(CVertex &Left, CVertex &Mid, CVertex &Right, CVertex &max, CVertex &min)
{
	int x_length, y_length, z_length;
	int length;
	double dx, dy, dz;

	int XSize = m_nXSize;
	int YSize = m_nYSize;
	int ZSize = m_nZSize;
	
	x_length = (int) floor(XSize*Right.x/(max.x - min.x));
	x_length -= (int) floor(XSize*Left.x/(max.x - min.x));
	y_length = (int) floor(YSize*Right.y/(max.y - min.y));
	y_length -= (int) floor(YSize*Left.y/(max.y - min.y));
	z_length = (int) floor(ZSize*Right.z/(max.z - min.z));
	z_length -= (int) floor(ZSize*Left.z/(max.z - min.z));
	if ( x_length < 0 )	x_length = -x_length;
	if ( y_length < 0 )	y_length = -y_length;
	if ( z_length < 0 ) z_length = -z_length;
	length = max(max(x_length,y_length),z_length);
	length++;

	dx = (Right.x - Left.x)/length;
	dy = (Right.y - Left.y)/length;
	dz = (Right.z - Left.z)/length;
	CVertex Prog = Left;

	////////////////////////////////////
	// DDA ( 선분 Voxel로 채우기 )
	////////////////////////////////////
	
	for ( int i = 0 ; i < length ; i++ ) {
		GetVoxelLine(Mid,Prog,max,min);
		Prog.x = Prog.x + dx;
		Prog.y = Prog.y + dy;
		Prog.z = Prog.z + dz;
	}
}

void CVoxel::GetVoxelLine(CVertex &start, CVertex &end, CVertex &max, CVertex &min)
{
	float x, y, z;
	int x_length, y_length, z_length;
	int length;
	double dx, dy, dz;
	int a, b, c;

	int XSize = m_nXSize;
	int YSize = m_nYSize;
	int ZSize = m_nZSize;

	x_length = (int) floor(XSize*end.x/(max.x - min.x));	
	x_length -= (int) floor(XSize*start.x/(max.x - min.x));
	y_length = (int) floor(YSize*end.y/(max.y - min.y));
	y_length -= (int) floor(YSize*start.y/(max.y - min.y));
	z_length = (int) floor(ZSize*end.z/(max.z - min.z));
	z_length -= (int) floor(ZSize*start.z/(max.z - min.z));
	if ( x_length < 0 )	x_length = -x_length;
	if ( y_length < 0 )	y_length = -y_length;
	if ( z_length < 0 ) z_length = -z_length;
	length = max(max(x_length,y_length),z_length);
	length++;

	dx = (end.x - start.x)/length;
	dy = (end.y - start.y)/length;
	dz = (end.z - start.z)/length;

	x = start.x;
	y = start.y;
	z = start.z;
	
	for ( int i = 0 ; i < length ; i++ ) {				
		a = (int) floor(XSize*(x - min.x)/(max.x - min.x));
		b = (int) floor(YSize*(y - min.y)/(max.y - min.y));
		c = (int) floor(ZSize*(z - min.z)/(max.z - min.z));
		if ( a == XSize ) a--; if ( a < 0 ) a = 0;
		if ( b == YSize ) b--; if ( b < 0 ) b = 0;
		if ( c == ZSize ) c--; if ( c < 0 ) c = 0;

		m_pData[a][b][c] = 1;
		x = x + dx;
		y = y + dy;
		z = z + dz;
	}
}

int CVoxel::MakeVoxel(CVertexArray &pVertex, CMeshArray &pMesh, CVertex &max, CVertex &min)
{
	/////////////////////////////
	// 3D Morphology
	/////////////////////////////	

	// Mesh Filling
	int i;
	int indexA, indexB, indexC;
	CVertex A, B, C;
	
	for ( i = 0 ; i < pMesh.size() ; i++ ) {
		indexA = pMesh[i]->a; indexB = pMesh[i]->b; indexC = pMesh[i]->c;
		
		A.x = pVertex[indexA]->x; A.y = pVertex[indexA]->y;	A.z = pVertex[indexA]->z;
		B.x = pVertex[indexB]->x; B.y = pVertex[indexB]->y;	B.z = pVertex[indexB]->z;
		C.x = pVertex[indexC]->x; C.y = pVertex[indexC]->y;	C.z = pVertex[indexC]->z;
				
		GetVoxel(A, B, C, max, min);		
	}
		
    // Region Filling
	Filling();
	
	return Simply();
}

BOOL CVoxel::ComputeVoxelSize(CVertex & width, int size)
{
	double max = max(width.x,max(width.y,width.z));
	double min = min(width.x,min(width.y,width.z));

	if ( max < DBL_MIN ) {
		m_nXSize = m_nYSize = m_nZSize = 1;
		return FALSE;
	}

	if ( min < DBL_MIN ) {
		if ( width.x < DBL_MIN ) {
			m_nXSize = 1;
			if ( width.y < DBL_MIN ) {
				m_nYSize = 1; m_nZSize = size;
			} else {
				if ( width.z < DBL_MIN ) {
					m_nYSize = size; m_nZSize = 1;
				} else {
					if ( width.y < width.z ) {
						m_nYSize = size;
						m_nZSize = (int) floor(size*(width.z/width.y) + 0.5);
					} else {						
						m_nYSize = (int) floor(size*(width.y/width.z) + 0.5);
						m_nZSize = size;
					}
				}
			}
			return TRUE;
		}
		if ( width.y < DBL_MIN ) {
			m_nYSize = 1;
			if ( width.z < DBL_MIN ) {
				m_nZSize = 1; m_nXSize = size;
			} else {
				if ( width.x < DBL_MIN ) {
					m_nZSize = size; m_nXSize = 1;
				} else {
					if ( width.z < width.x ) {
						m_nZSize = size;
						m_nXSize = (int) floor(size*(width.x/width.z) + 0.5);
					} else {						
						m_nZSize = (int) floor(size*(width.z/width.x) + 0.5);
						m_nXSize = size;
					}
				}
			}
			return TRUE;
		}
		if ( width.z < DBL_MIN ) {
			m_nZSize = 1;
			if ( width.x < DBL_MIN ) {
				m_nXSize = 1; m_nYSize = size;
			} else {
				if ( width.y < DBL_MIN ) {
					m_nXSize = size; m_nYSize = 1;
				} else {
					if ( width.x < width.y ) {
						m_nXSize = size;
						m_nYSize = (int) floor(size*(width.y/width.x) + 0.5);
					} else {						
						m_nXSize = (int) floor(size*(width.x/width.y) + 0.5);
						m_nYSize = size;
					}
				}
			}
			return TRUE;
		}
	}

	if ( width.x < width.y ) {
		if ( width.x < width.z ) {
			m_nXSize = size;			
			m_nYSize = (int) floor(size*(width.y/width.x) + 0.5);
			m_nZSize = (int) floor(size*(width.z/width.x) + 0.5);
		}
		else {			
			m_nXSize = (int) floor(size*(width.x/width.z) + 0.5);
			m_nYSize = (int) floor(size*(width.y/width.z) + 0.5);
			m_nZSize = size;
		}
	}
	else {
		if ( width.y < width.z ) {			
			m_nXSize = (int) floor(size*(width.x/width.y) + 0.5);
			m_nYSize = size;
			m_nZSize = (int) floor(size*(width.z/width.y) + 0.5);
		}
		else {			
			m_nXSize = (int) floor(size*(width.x/width.z) + 0.5);
			m_nYSize = (int) floor(size*(width.y/width.z) + 0.5);
			m_nZSize = size;
		}
	}

	return TRUE;
}

BOOL CVoxel::GetVoxelSizeDynamic(CVertexArray &pVertex, CMeshArray &pMesh, CVertex &max, CVertex &min, CVertex &width, int nVoxel)
{	
	int size = 0;	

	double ratio = 0;
	if ( !(width.x > width.y) && !(width.x > width.z) ) {
		if ( width.x < DBL_MIN ) {
			ratio = width.y/width.z; if ( ratio < 1 ) ratio = 1/ratio;
		} else ratio = (width.y/width.x)*(width.z/width.x);
	} else {
		if ( !(width.y > width.z) && !(width.y > width.x) ) {
			if ( width.y < DBL_MIN ) {
				ratio = width.z/width.x; if ( ratio < 1 ) ratio = 1/ratio;
			} else ratio = (width.z/width.y)*(width.x/width.y);
		} else {
			if ( width.z < DBL_MIN ) {
				ratio = width.x/width.y; if ( ratio < 1 ) ratio = 1/ratio;
			} else ratio = (width.x/width.z)*(width.y/width.z);
		}
	}
	size = (int) floor(pow((double) nVoxel/ratio,(double) 1/3)+0.5);

	int p_num = 0, p_size = 0;;
	for (;;) {
		if ( !ComputeVoxelSize(width, size) ) break;		
		Allocate(m_nXSize,m_nYSize,m_nZSize);
		MakeVoxel(pVertex, pMesh, max, min);
		int num_of_voxel = GetNumber();

		if ( num_of_voxel < p_num ) {			
			ComputeVoxelSize(width, p_size);			
			break;
		}

		if ( num_of_voxel < nVoxel ) {
			p_num = num_of_voxel; p_size = size;			
			double dUp = (double) m_nXSize*m_nYSize*m_nZSize*nVoxel/num_of_voxel;
			int nUp = (int) floor(pow(dUp/ratio,(double) 1/3)+0.5);
			if ( size < nUp ) size = nUp;
			else size++;			
			Free();
		} else {			
			break;
		}
	}
    
	return TRUE;
}

BOOL CVoxel::GetVoxelSizeStatic(CVertex &width, int nVoxel)
{
	if ( max(width.x,max(width.y,width.z)) < DBL_MIN ) {m_nXSize = m_nYSize = m_nZSize = 1;}
	else {		
		m_nXSize = (int) ceil(pow(nVoxel*(width.x*width.x)/(width.y*width.z),(double) 1/3));
		m_nYSize = (int) ceil(pow(nVoxel*(width.y*width.y)/(width.z*width.x),(double) 1/3));
		m_nZSize = (int) ceil(pow(nVoxel*(width.z*width.z)/(width.x*width.y),(double) 1/3));
	}

    Allocate(m_nXSize,m_nYSize,m_nZSize);
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//from Part.cpp
/////////////////////////////////////////////////////////////////////////////
CPart::CPart(const CPart& part) {
	index = part.index;

	num = part.num;
	mean = part.mean;
	for ( int i = 0 ; i < 9 ; i++ )
		mat[i] = part.mat[i];
	length = part.length;
	
	form = part.form;		
	modify = part.modify;
}

const CPart& CPart::operator =(const CPart& part) {
	index = part.index;

	num = part.num;
	mean = part.mean;
	for ( int i = 0 ; i < 9 ; i++ )
		mat[i] = part.mat[i];
	length = part.length;
	
	form = part.form;		
	modify = part.modify;
	return *this;
}

bool CPart::operator ==(const CPart& part) const {
	return true;
}

////////////////////////////////////////////////////////////////////////////////
//from Split.cpp
/////////////////////////////////////////////////////////////////////////////
#define TH_RESIDUE 2
#define TH_MINIMUM 7

/////////////////////////////////////////////////////////////////////////////
// CSplit

/////////////////////////////////////////////////////////////////////////////
// CSplit construction/destruction

CVoxel CSplit::SE[RANK];

CSplit::CSplit()
{
    m_nDivX = 0; m_nDivY = 0; m_nDivZ = 0;
	m_nDepth = 0;
}

CSplit::~CSplit()
{	
	Clear();
}

/////////////////////////////////////////////////////////////////////////////
// CSplit commands

/*
BOOL CSplit::LoadBinary(LPCTSTR lpszPathName)
{
	int i, j, k;
	char c;
	CString tmp;
	
	fstream bin;
	bin.open(lpszPathName, ios::in|ios::binary|ios::nocreate);
	if ( !bin.is_open() ) {bin.close(); return FALSE;}
	
	m_Voxel.Free();

	bin.read(tmp.GetBuffer(4),4); m_nDivX = atoi(tmp);
	bin.read(tmp.GetBuffer(4),4); m_nDivY = atoi(tmp);
	bin.read(tmp.GetBuffer(4),4); m_nDivZ = atoi(tmp);

	m_Voxel.Allocate(m_nDivX,m_nDivY,m_nDivZ);

	for ( i = 0 ; i < m_nDivX ; i++ ) {
		for ( j = 0 ; j < m_nDivY ; j++ ) {
			for ( k = 0 ; k < m_nDivZ ; k++ ) {
				bin.read(&c,1);
				if ( c == '1' ) m_Voxel.m_pData[i][j][k] = 1;
			}
		}
	}
	bin.close();

 	Initialize();
	return TRUE;
}

BOOL CSplit::LoadGray(LPCTSTR lpszPathName)
{
	int i, j, k;
	CString tmp;
	
	fstream dec;
	dec.open(lpszPathName, ios::in|ios::binary|ios::nocreate);
	if ( !dec.is_open() ) {dec.clear(); return FALSE;}
	
	m_Voxel.Free();
	m_Part.Free();

	dec.read(tmp.GetBuffer(4),4); m_nDivX = atoi(tmp);
	dec.read(tmp.GetBuffer(4),4); m_nDivY = atoi(tmp);
	dec.read(tmp.GetBuffer(4),4); m_nDivZ = atoi(tmp);
	dec.read(tmp.GetBuffer(4),4); m_Part.root = atoi(tmp);
	dec.read(tmp.GetBuffer(4),4); m_Part.leaf = atoi(tmp);
	
	m_Voxel.Allocate(m_nDivX,m_nDivY,m_nDivZ);	
	m_Part.Allocate(m_nDivX,m_nDivY,m_nDivZ);

	tmp.Format("%d",m_Part.root+m_Part.leaf);
	int s = tmp.GetLength();
	for ( i = 0 ; i < m_nDivX ; i++ ) {
		for ( j = 0 ; j < m_nDivY ; j++ ) {
			for ( k = 0 ; k < m_nDivZ ; k++ ) {				
				dec.read(tmp.GetBuffer(s),s);
				int index = atoi(tmp);
				if ( index ) m_Voxel.m_pData[i][j][k] = 1;
				else m_Voxel.m_pData[i][j][k] = 0;
				m_Part.m_pData[i][j][k] = index;
			}
		}
	}
	dec.close();
	return TRUE;
}

void CSplit::SaveBinary(LPCTSTR lpszPathName)
{	
	int i, j, k;
	CString tmp;

	fstream bin;
	bin.open(lpszPathName, ios::out|ios::binary);
	
	tmp.Format("%d",m_nDivX); bin.write(tmp,4);
	tmp.Format("%d",m_nDivY); bin.write(tmp,4);
	tmp.Format("%d",m_nDivZ); bin.write(tmp,4);
	
	for ( i = 0 ; i < m_nDivX ; i++ ) {
		for ( j = 0 ; j < m_nDivY ; j++ ) {
			for ( k = 0 ; k < m_nDivZ ; k++ ) {
				if ( m_Voxel.m_pData[i][j][k] ) bin.put('1');
				else bin.put('0');
			}
		}
	}
	bin.close();
}
void CSplit::SaveGray(LPCTSTR lpszPathName)
{
	int i, j, k;
	CString tmp;

	fstream dec;
	dec.open(lpszPathName, ios::out|ios::binary);	
	
	tmp.Format("%d",m_nDivX); dec.write(tmp,4);
	tmp.Format("%d",m_nDivY); dec.write(tmp,4);
	tmp.Format("%d",m_nDivZ); dec.write(tmp,4);
	tmp.Format("%d",m_Part.root); dec.write(tmp,4);
	tmp.Format("%d",m_Part.leaf); dec.write(tmp,4);

	tmp.Format("%d",m_Part.root+m_Part.leaf);
	int s = tmp.GetLength();	
	for ( i = 0 ; i < m_nDivX ; i++ ) {
		for ( j = 0 ; j < m_nDivY ; j++ ) {
			for ( k = 0 ; k < m_nDivZ ; k++ ) {
				int index = m_Part.m_pData[i][j][k];
				tmp.Format("%d",index); dec.write(tmp,s);
			}
		}
	}
	dec.close();
}
*/

void CSplit::Initialize()
{
	int i, j, k;
    int XSize = m_Voxel.GetXSize();
    int YSize = m_Voxel.GetYSize();
	int ZSize = m_Voxel.GetZSize();

	m_Part.Allocate(XSize, YSize, ZSize);
    
	// make part
	CPart * pPart = new CPart;
	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				if ( m_Voxel.m_pData[i][j][k] ) {
					CDot * pDot = new CDot;
					pDot->x = i; pDot->y = j; pDot->z = k;
					pPart->pDot.push_back(pDot);//pPart->pDot.Add(pDot);
				}
			}
		}
	}
	m_pArray[0].push_back(pPart);//m_pArray[0].Add(pPart);

	//MakeSE(); // delete after changing into static function
}

void CSplit::MakeGraph()
{
	int i, j, k;
	int x, y, z;
	int XSize = m_Part.GetXSize();
	int YSize = m_Part.GetYSize();
	int ZSize = m_Part.GetZSize();

	int num_of_body = m_Part.root;
	int num_of_branch = m_Part.leaf;
	int num_of_part = num_of_body + num_of_branch;

	int ** relation = new int * [num_of_part];
	for ( i = 0 ; i < num_of_part ; i++ ) {
		relation[i] = new int [num_of_part];
		for ( j = 0 ; j < num_of_part ; j++ ) {
			relation[i][j] = 0;
		}
	}

	for ( i = 0 ; i < num_of_part ; i++ ) {
		CPart *pPart = new CPart;
		pPart->index = i+1;
		m_pPart.push_back(pPart);//m_pPart.Add(pPart);
	}

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				int voxel = m_Part.m_pData[i][j][k];
				if ( voxel ) {
					CDot *pDot = new CDot;
					pDot->x = i; pDot->y = j; pDot->z = k;
					m_pPart[voxel-1]->pDot.push_back(pDot);//m_pPart[voxel-1]->pDot.Add(pDot);

					m_pPart[voxel-1]->mean.x += i;
					m_pPart[voxel-1]->mean.y += j;
					m_pPart[voxel-1]->mean.z += k;
					m_pPart[voxel-1]->num++;

					x = -1; y = 0; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize) 
						&& !(k+z < 0 || k+z >= ZSize)
						&& m_Part.m_pData[i+x][j+y][k+z] > 0 ) {
						int index = m_Part.m_pData[i+x][j+y][k+z];
						relation[voxel-1][index-1] = 1;
					}
					x = 1; y = 0; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize) 
						&& !(k+z < 0 || k+z >= ZSize)
						&& m_Part.m_pData[i+x][j+y][k+z] > 0 ) {
						int index = m_Part.m_pData[i+x][j+y][k+z];
						relation[voxel-1][index-1] = 1;
					}
					x = 0; y = -1; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize) 
						&& !(k+z < 0 || k+z >= ZSize)
						&& m_Part.m_pData[i+x][j+y][k+z] > 0 ) {
						int index = m_Part.m_pData[i+x][j+y][k+z];
						relation[voxel-1][index-1] = 1;
					}
					x = 0; y = 1; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize) 
						&& !(k+z < 0 || k+z >= ZSize)
						&& m_Part.m_pData[i+x][j+y][k+z] > 0 ) {
						int index = m_Part.m_pData[i+x][j+y][k+z];
						relation[voxel-1][index-1] = 1;
					}
					x = 0; y = 0; z = -1;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize) 
						&& !(k+z < 0 || k+z >= ZSize)
						&& m_Part.m_pData[i+x][j+y][k+z] > 0 ) {
						int index = m_Part.m_pData[i+x][j+y][k+z];
						relation[voxel-1][index-1] = 1;
					}
					x = 0; y = 0; z = 1;
					if ( !(i+x < 0 || i+x >= XSize)
						&& !(j+y < 0 || j+y >= YSize) 
						&& !(k+z < 0 || k+z >= ZSize)
						&& m_Part.m_pData[i+x][j+y][k+z] > 0 ) {
						int index = m_Part.m_pData[i+x][j+y][k+z];
						relation[voxel-1][index-1] = 1;
					}
				}
			}
		}
	}
	
	for ( i = 0 ; i < num_of_part ; i++ ) {
		CPart *pPart = m_pPart[i];
		ModelAssay(pPart);
	}

	for ( i = 0 ; i < num_of_part ; i++ ) {
		for ( j = 0 ; j < num_of_part ; j++ ) {
			if ( i != j && relation[i][j] ) {
				m_pPart[i]->pPart.push_back(m_pPart[j]);//m_pPart[i]->pPart.Add(m_pPart[j]);
			}
		}
	}

	for ( i = 0 ; i < num_of_part ; i++ )
		delete [] relation[i];
	delete [] relation;
}

bool CSplit::Decompose()
{
	int i, j;
	int x, y, z;
	int XSize = m_Voxel.GetXSize();
	int YSize = m_Voxel.GetYSize();
	int ZSize = m_Voxel.GetZSize();

	m_nDepth++; // for increasing the depth of the part array
	int num_of_level = m_pArray[m_nDepth-1].size();
	int num_of_part = 0;

	CVoxel bench;
	bench.Allocate(XSize, YSize, ZSize);

//zhanghui changed: here no need to use "new".  
//	CPartArray * pArrayRoot = new CPartArray;	
//	CPartArray * pArrayLeaf = new CPartArray;
	CPartArray pArrayRoot;	
	CPartArray pArrayLeaf;
	
	CPart * pPartPrev, * pPartNext;
	CDot * pDot;
	for ( i = 0 ; i < num_of_level ; i++ ) {
		pPartPrev = m_pArray[m_nDepth-1][i];
		for ( j = 0 ; j < pPartPrev->pDot.size() ; j++ ) {
			pDot = pPartPrev->pDot[j];
			x = pDot->x; y = pDot->y; z = pDot->z;
			bench.m_pData[x][y][z] = 1;
		}

		bool query = true;
		if ( pPartPrev->modify ) {
			switch ( pPartPrev->form ) {
				case -1: // for the first time
					query = Selection(bench);
					pPartPrev->form = 0;
					break;
				case 0: // root
					query = SplitBody(bench);
					break;
				case 1: // leaf
					query = SplitBranch(bench);
					break;			
			}
		} else {
			query = false;
		}
				
		if ( !query ) {
			pPartNext = new CPart;
			pPartNext->modify = false;
			for ( x = 0 ; x < XSize ; x++ ) {
				for ( y = 0 ; y < YSize ; y++ ) {
					for ( z = 0 ; z < ZSize ; z++ ) {
						if ( bench.m_pData[x][y][z] ) {
							pDot = new CDot;
							pDot->x = x; pDot->y = y; pDot->z = z;
							pPartNext->pDot.push_back(pDot);//pPartNext->pDot.Add(pDot);
						}
						bench.m_pData[x][y][z] = 0;
					}
				}
			}
			
			if ( pPartPrev->form == 0 ) {
				pPartNext->form = 0;
				pArrayRoot.push_back(pPartNext);//pArrayRoot->Add(pPartNext);
			} else {			
				pPartNext->form = 1;
				pArrayLeaf.push_back(pPartNext);
			}			
		} else {
			num_of_part = bench.root + bench.leaf;
			for ( j = 0 ; j < num_of_part ; j++ ) {
				pPartNext = new CPart;
				for ( x = 0 ; x < XSize ; x++ ) {
					for ( y = 0 ; y < YSize ; y++ ) {
						for ( z = 0 ; z < ZSize ; z++ ) {							
							if ( bench.m_pData[x][y][z] == j+1 ) {
								pDot = new CDot;
								pDot->x = x; pDot->y = y; pDot->z = z;
								pPartNext->pDot.push_back(pDot);
								bench.m_pData[x][y][z] = 0;
							}							
						}
					}
				}
			
				if ( j < bench.root ) {
					pPartNext->form = 0;
					pArrayRoot.push_back(pPartNext);//pArrayRoot->Add(pPartNext);
				} else {
					pPartNext->form = 1;
					pArrayLeaf.push_back(pPartNext);//pArrayLeaf->Add(pPartNext);
				}
			}
		}
	}
	bool stop = false;
	for ( i = 0 ; i < pArrayRoot.size() ; i++ ) {
		//m_pArray[m_nDepth].push_back(pArrayRoot->GetAt(i));
    m_pArray[m_nDepth].push_back(pArrayRoot[i]);
		//if ( pArrayRoot->GetAt(i)->modify ) stop = true;
    if ( pArrayRoot[i]->modify ) stop = true;
	}	
	for ( i = 0 ; i < pArrayLeaf.size() ; i++ ) {
		//m_pArray[m_nDepth].push_back(pArrayLeaf->GetAt(i));
    m_pArray[m_nDepth].push_back(pArrayLeaf[i]);
		//if ( pArrayLeaf->GetAt(i)->modify ) stop = true;
    if ( pArrayLeaf[i]->modify ) stop = true;
	}

//zhanghui commented.
//	delete [] pArrayRoot;
//	delete [] pArrayLeaf;
  pArrayRoot.clear();
  pArrayLeaf.clear();

	return stop;
}

bool CSplit::Selection(CVoxel &source)
{
	int i, j, k;
	int a;
	int XSize = source.GetXSize();
	int YSize = source.GetYSize();
	int ZSize = source.GetZSize();
	double maximum = 0;
	
	CVoxel dump[RANK];

	CVoxel bench;
	bench.Allocate(XSize, YSize, ZSize);

	double initial_convexity = 0, final_convexity = 0;
	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				bench.m_pData[i][j][k] = 0;			
				if ( source.m_pData[i][j][k] ) {
					bench.m_pData[i][j][k] = 1;			
				}
			}
		}
	}
	initial_convexity = pow((double)bench.GetNumber(),2)/bench.Hull();

	int index = 0, size = 0;	
	for (;;) {
		dump[size].Allocate(XSize, YSize, ZSize);	
		for ( i = 0 ; i < XSize ; i++ ) {
			for ( j = 0 ; j < YSize ; j++ ) {
				for ( k = 0 ; k < ZSize ; k++ ) {
					if ( source.m_pData[i][j][k] )
						dump[size].m_pData[i][j][k] = 1;
				}
			}
		}	

		if ( !Partition(dump[size],size) ) break;
		Flat(dump[size]);
	
		int part = dump[size].root + dump[size].leaf;
		double convexity = 0;
		
		for ( a = 0 ; a < part ; a++ ) {			
			for ( i = 0 ; i < XSize ; i++ ) {
				for ( j = 0 ; j < YSize ; j++ ) {
					for ( k = 0 ; k < ZSize ; k++ ) {
						bench.m_pData[i][j][k] = 0;
						if ( dump[size].m_pData[i][j][k] == a+1 ) {
							bench.m_pData[i][j][k] = 1;
						}
					}
				}
			}
			convexity += pow((double)bench.GetNumber(),2)/bench.Hull();
		}
		if ( convexity >= maximum ) {
			maximum = convexity;
			index = size;
		}
		dump[size].convexity = convexity;
	
		size++; if ( size >= RANK ) break;		
	}

	final_convexity = dump[index].convexity;
	if ( initial_convexity > final_convexity ) {
		return false;
	}

	if ( dump[index].root + dump[index].leaf == 1 )
		return false;

	source.root = dump[index].root;
	source.link = 0;
	source.leaf = dump[index].leaf;
	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				source.m_pData[i][j][k] = dump[index].m_pData[i][j][k];
			}
		}
	}

	return true;
}

bool CSplit::SplitBody(CVoxel &source)
{
	if ( VerifyPart(source) ) {
		if ( Selection(source) ) {
			return TrimPart(source);
		}
	}
	return false;
}

bool CSplit::SplitBranch(CVoxel &source)
{
	if ( VerifyPart(source) ) {
		if ( Selection(source) ) {
			if ( VerifyBranch(source) ) {				
				return TrimPart(source);
			}
		}
	}
	return false;
}

bool CSplit::Partition(CVoxel &source, int size)
{
	int i, j, k;

	int XSize = source.GetXSize();
	int YSize = source.GetYSize();
	int ZSize = source.GetZSize();

	CVoxel body, branch;
	body.Allocate(XSize, YSize, ZSize);
	branch.Allocate(XSize, YSize, ZSize);

	CVoxel bench;
	bench.Allocate(XSize, YSize, ZSize);

	CVoxel erode, opening;
	erode.Allocate(XSize, YSize, ZSize);
	opening.Allocate(XSize, YSize, ZSize);
	
	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				erode.m_pData[i][j][k] = source.m_pData[i][j][k];
			}
		}
	}
	erode.FastErode(SE[size]);
	if ( !erode.GetNumber() ) {			
		source.root = 1;
		source.leaf = 0;
		return false;
	}

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				opening.m_pData[i][j][k] = erode.m_pData[i][j][k];
			}
		}
	}
	opening.FastDilate(SE[size]);

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				body.m_pData[i][j][k] = opening.m_pData[i][j][k];
				if ( !opening.m_pData[i][j][k] && source.m_pData[i][j][k] ) branch.m_pData[i][j][k] = 1;
				else branch.m_pData[i][j][k] = 0;
			}
		}
	}

	EarlyProcess(body, branch, 2);
	if ( branch.GetNumber() )
		Residual(body,branch,source);
	else {
		source.root = 1;
		source.leaf = 0;
	}

	return true;
}

void CSplit::Flat(CVoxel &source)
{
	int i, j, k;
	int a, b;
	int x, y, z;
	int XSize = source.GetXSize();
	int YSize = source.GetYSize();
	int ZSize = source.GetZSize();
	
	CVoxel bench, buffer;
	bench.Allocate(XSize, YSize, ZSize);
	buffer.Allocate(XSize, YSize, ZSize);
	
	int num_of_body = source.root;
	int num_of_branch = source.leaf;
	int num_of_part = num_of_body + num_of_branch;

	for ( a = num_of_body ; a < num_of_part ; a++ ) {
		for ( i = 0 ; i < XSize ; i++ ) {
			for ( j = 0 ; j < YSize ; j++ ) {
				for ( k = 0 ; k < ZSize ; k++ ) {
					bench.m_pData[i][j][k] = 0;
					if ( source.m_pData[i][j][k] == a+1 ) {
						bool test = false;
						for ( x = -1 ; x <= 1 ; x++ ) {
							for ( y = -1 ; y <= 1 ; y++ ) {
								for ( z = -1 ; z <= 1 ; z++ ) {						
									if ( !(i+x < 0 || i+x >= XSize)
										&& !(j+y < 0 || j+y >= YSize)
										&& !(k+z < 0 || k+z >= ZSize)
										&& source.m_pData[i+x][j+y][k+z] && source.m_pData[i+x][j+y][k+z] != a+1 ) {
										test = true;
									}
									if ( test ) break;
								}
								if ( test ) break;
							}
							if ( test ) break;
						}
						if ( test )	bench.m_pData[i][j][k] = 1;
					}
				}
			}
		}
		int region = bench.Component();
		
		for ( b = 0 ; b < region ; b++ ) {
			bool test = false;
			int index = -1;
			
			for ( i = 0 ; i < XSize ; i++ ) {
				for ( j = 0 ; j < YSize ; j++ ) {
					for ( k = 0 ; k < ZSize ; k++ ) {
						if ( buffer.m_pData[i][j][k] && source.m_pData[i][j][k] != a+1 ) {
							if ( index == -1 )
								index = source.m_pData[i][j][k];
							else {
								if ( index != source.m_pData[i][j][k] )
									test = true;
							}
						}
					}						
					if ( test ) break;
				}
				if ( test ) break;
			}
			if ( !test && index != 0 ) {
				for ( i = 0 ; i < XSize ; i++ ) {
					for ( j = 0 ; j < YSize ; j++ ) {
						for ( k = 0 ; k < ZSize ; k++ ) {
							if ( buffer.m_pData[i][j][k] && source.m_pData[i][j][k] != a+1 ) {
								source.m_pData[i][j][k] = a+1;
							}
						}
					}
				}
			}
		}
	}
}

bool CSplit::VerifyPart(CVoxel &part)
{
	int i, j, k;
	int XSize = part.GetXSize();
	int YSize = part.GetYSize();
	int ZSize = part.GetZSize();

	CVoxel bench;
	bench.Allocate(XSize, YSize, ZSize);
	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				bench.m_pData[i][j][k] = 0;
				if ( part.m_pData[i][j][k] ) {				
					bench.m_pData[i][j][k] = 1;
				}
			}
		}
	}	
	bench.MakeHull();

	if ( part.GetNumber() == bench.GetNumber() )
		return false;
	return true;
}

bool CSplit::TrimPart(CVoxel &source)
{
	int i, j, k;
	int x, y, z;
	int XSize = source.GetXSize();
	int YSize = source.GetYSize();	
	int ZSize = source.GetZSize();	

	int num_of_body = source.root;
	int num_of_branch = source.leaf;
	int num_of_part = num_of_body + num_of_branch;

	CVoxel bench;
	bench.Allocate(XSize, YSize, ZSize);

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				if ( !source.m_pData[i][j][k] && m_Voxel.m_pData[i][j][k] )
					bench.m_pData[i][j][k] = 1;
			}
		}
	}
	int ** inner = new int * [num_of_part];
	for ( i = 0 ; i < num_of_part ; i++ ) {
		inner[i] = new int [num_of_part];
		for ( j = 0 ; j < num_of_part ; j++ ) {
			inner[i][j] = 0;
		}
	}

	int num_of_outlier = bench.Component();
	int ** outer = new int * [num_of_part];
	for ( i = 0 ; i < num_of_part ; i++ ) {
		outer[i] = new int [num_of_outlier];
		for ( j = 0 ; j < num_of_outlier ; j++ ) {
			outer[i][j] = 0;
		}
	}

	int * in_bound = new int [num_of_part];
	int * out_bound = new int [num_of_part];
	for ( i = 0 ; i < num_of_part ; i++ ) {
		in_bound[i] = out_bound[i] = 0;
	}

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				if ( source.m_pData[i][j][k] ) {
					bool in = false, out = false;
					int pixel = source.m_pData[i][j][k];
					x = -1; y = 0; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( source.m_pData[i+x][j+y][k+z] && source.m_pData[i+x][j+y][k+z] != pixel )
							inner[pixel-1][source.m_pData[i+x][j+y][k+z]-1] = 1;
						if ( bench.m_pData[i+x][j+y][k+z] )
							outer[pixel-1][bench.m_pData[i+x][j+y][k+z]-1] = 1;
						if ( m_Voxel.m_pData[i+x][j+y][k+z]
							&& source.m_pData[i+x][j+y][k+z] != pixel ) in = true;
						if ( !m_Voxel.m_pData[i+x][j+y][k+z] ) out = true;
					}
					x = 1; y = 0; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( source.m_pData[i+x][j+y][k+z] && source.m_pData[i+x][j+y][k+z] != pixel )
							inner[pixel-1][source.m_pData[i+x][j+y][k+z]-1] = 1;
						if ( bench.m_pData[i+x][j+y][k+z] )
							outer[pixel-1][bench.m_pData[i+x][j+y][k+z]-1] = 1;
						if ( m_Voxel.m_pData[i+x][j+y][k+z]
							&& source.m_pData[i+x][j+y][k+z] != pixel ) in = true;
						if ( !m_Voxel.m_pData[i+x][j+y][k+z] ) out = true;
					}
					x = 0; y = -1; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( source.m_pData[i+x][j+y][k+z] && source.m_pData[i+x][j+y][k+z] != pixel )
							inner[pixel-1][source.m_pData[i+x][j+y][k+z]-1] = 1;
						if ( bench.m_pData[i+x][j+y][k+z] )
							outer[pixel-1][bench.m_pData[i+x][j+y][k+z]-1] = 1;
						if ( m_Voxel.m_pData[i+x][j+y][k+z]
							&& source.m_pData[i+x][j+y][k+z] != pixel ) in = true;
						if ( !m_Voxel.m_pData[i+x][j+y][k+z] ) out = true;
					}
					x = 0; y = 1; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( source.m_pData[i+x][j+y][k+z] && source.m_pData[i+x][j+y][k+z] != pixel )
							inner[pixel-1][source.m_pData[i+x][j+y][k+z]-1] = 1;
						if ( bench.m_pData[i+x][j+y][k+z] )
							outer[pixel-1][bench.m_pData[i+x][j+y][k+z]-1] = 1;
						if ( m_Voxel.m_pData[i+x][j+y][k+z]
							&& source.m_pData[i+x][j+y][k+z] != pixel ) in = true;
						if ( !m_Voxel.m_pData[i+x][j+y][k+z] ) out = true;
					}
					x = 0; y = 0; z = -1;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( source.m_pData[i+x][j+y][k+z] && source.m_pData[i+x][j+y][k+z] != pixel )
							inner[pixel-1][source.m_pData[i+x][j+y][k+z]-1] = 1;
						if ( bench.m_pData[i+x][j+y][k+z] )
							outer[pixel-1][bench.m_pData[i+x][j+y][k+z]-1] = 1;
						if ( m_Voxel.m_pData[i+x][j+y][k+z]
							&& source.m_pData[i+x][j+y][k+z] != pixel ) in = true;
						if ( !m_Voxel.m_pData[i+x][j+y][k+z] ) out = true;
					}
					x = 0; y = 0; z = 1;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( source.m_pData[i+x][j+y][k+z] && source.m_pData[i+x][j+y][k+z] != pixel )
							inner[pixel-1][source.m_pData[i+x][j+y][k+z]-1] = 1;
						if ( bench.m_pData[i+x][j+y][k+z] )
							outer[pixel-1][bench.m_pData[i+x][j+y][k+z]-1] = 1;
						if ( m_Voxel.m_pData[i+x][j+y][k+z]
							&& source.m_pData[i+x][j+y][k+z] != pixel ) in = true;
						if ( !m_Voxel.m_pData[i+x][j+y][k+z] ) out = true;
					}
					if ( in ) in_bound[source.m_pData[i][j][k]-1]++;
					if ( !in & out ) out_bound[source.m_pData[i][j][k]-1]++;
				}
			}
		}
	}

	int * check_branch = new int [num_of_branch];
	for ( i = 0 ; i < num_of_branch ; i++ )
		check_branch[i] = 0;

	for ( i = num_of_body ; i < num_of_part ; i++ ) {
		bool test = false;
		for ( x = 0 ; x < num_of_part ; x++ ) {
			for ( y = 0 ; y < num_of_outlier ; y++ ) {				
				if ( inner[i][x] && outer[i][y] ) {
					if ( outer[x][y] ) {
						check_branch[i-num_of_body] = -1;
						test = true;
						break;
					}
				}
			}
			if ( test ) break;
		}		
		if ( !test ) {
			for ( x = 0 ; x < num_of_part ; x++ ) {
				for ( y = x+1 ; y < num_of_part ; y++ ) {
					if ( inner[i][x] && inner[i][y] ) {
						if ( inner[x][y] ) {
							check_branch[i-num_of_body] = -1;
							test = true;
							break;
						}
					}
				}
				if ( test ) break;
			}			
		}
	}

	for ( i = 0 ; i < num_of_branch ; i++ ) {
		if ( (double) out_bound[num_of_body+i]/in_bound[num_of_body+i] > 1 )
			check_branch[i] = 0; // the threshold for the siginificant part, which might be trimmed
	}	

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				bench.m_pData[i][j][k] = 0;
				if ( source.m_pData[i][j][k] ) {
					if ( source.m_pData[i][j][k] > num_of_body ) {
						if ( check_branch[source.m_pData[i][j][k]-num_of_body-1] < 0 )
							bench.m_pData[i][j][k] = 1;
					} else {
						bench.m_pData[i][j][k] = 1;
					}
				}
			}
		}
	}	
	
	int num_body = bench.Component();
	int num_branch = 0;

	for ( i = 0 ; i < num_of_branch ; i++ ){
		if ( check_branch[i] == 0 )
			check_branch[i] = ++num_branch;
	}

	for ( i = 0 ; i < num_of_part ; i++ ) {
		delete [] inner[i];
		delete [] outer[i];
	}
	delete [] inner;
	delete [] outer;		
	delete [] in_bound;
	delete [] out_bound;

	if ( num_body + num_branch == 1 ) {
		delete [] check_branch;
		return false;
	}

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				int index = source.m_pData[i][j][k];
				if ( index > num_of_body ) {
					if ( check_branch[index-num_of_body-1] > 0 )
						source.m_pData[i][j][k] = num_body + check_branch[index-num_of_body-1];
				}
				if ( bench.m_pData[i][j][k] )
					source.m_pData[i][j][k] = bench.m_pData[i][j][k];
			}
		}
	}
	source.root = num_body;
	source.leaf = num_branch;

	delete [] check_branch;
	return true;
}

bool CSplit::VerifyBranch(CVoxel &source)
{
	int i, j, k;
	int x, y, z;
	int XSize = source.GetXSize();
	int YSize = source.GetYSize();
	int ZSize = source.GetZSize();
	
	int num_of_body = source.root;
	int num_of_branch = source.leaf;
	int num_of_part = num_of_body + num_of_branch;

	int * row = new int [num_of_part];
	int ** relation = new int * [num_of_part];	
	for ( i = 0 ; i < num_of_part ; i++ ) {
		row[i] = 0;
		relation[i] = new int [num_of_part];
		for ( j = 0 ; j < num_of_part ; j++ ) {
			relation[i][j] = 0;
		}
	}

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				if ( source.m_pData[i][j][k] ) {
					int pixel = source.m_pData[i][j][k];
					x = -1; y = 0; z = 0;
					if ( !(i+x < 0 || i+x >= XSize)
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( source.m_pData[i+x][j+y][k+z] ) {
							if ( source.m_pData[i+x][j+y][k+z] != pixel ) {
								relation[pixel-1][source.m_pData[i+x][j+y][k+z]-1] = 1;
								relation[source.m_pData[i+x][j+y][k+z]-1][pixel-1] = 1;
							}
						} else {
							if ( m_Voxel.m_pData[i+x][j+y][k+z] ) {
								row[pixel-1] = 1;
							}
						}
					}
					x = 1; y = 0; z = 0;
					if ( !(i+x < 0 || i+x >= XSize)
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( source.m_pData[i+x][j+y][k+z] ) {
							if ( source.m_pData[i+x][j+y][k+z] != pixel ) {
								relation[pixel-1][source.m_pData[i+x][j+y][k+z]-1] = 1;
								relation[source.m_pData[i+x][j+y][k+z]-1][pixel-1] = 1;
							}
						} else {
							if ( m_Voxel.m_pData[i+x][j+y][k+z] ) {
								row[pixel-1] = 1;
							}
						}
					}
					x = 0; y = -1; z = 0;
					if ( !(i+x < 0 || i+x >= XSize)
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( source.m_pData[i+x][j+y][k+z] ) {
							if ( source.m_pData[i+x][j+y][k+z] != pixel ) {
								relation[pixel-1][source.m_pData[i+x][j+y][k+z]-1] = 1;
								relation[source.m_pData[i+x][j+y][k+z]-1][pixel-1] = 1;
							}
						} else {
							if ( m_Voxel.m_pData[i+x][j+y][k+z] ) {
								row[pixel-1] = 1;
							}
						}
					}
					x = 0; y = 1; z = 0;
					if ( !(i+x < 0 || i+x >= XSize)
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( source.m_pData[i+x][j+y][k+z] ) {
							if ( source.m_pData[i+x][j+y][k+z] != pixel ) {
								relation[pixel-1][source.m_pData[i+x][j+y][k+z]-1] = 1;
								relation[source.m_pData[i+x][j+y][k+z]-1][pixel-1] = 1;
							}
						} else {
							if ( m_Voxel.m_pData[i+x][j+y][k+z] ) {
								row[pixel-1] = 1;
							}
						}
					}
					x = 0; y = 0; z = -1;
					if ( !(i+x < 0 || i+x >= XSize)
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( source.m_pData[i+x][j+y][k+z] ) {
							if ( source.m_pData[i+x][j+y][k+z] != pixel ) {
								relation[pixel-1][source.m_pData[i+x][j+y][k+z]-1] = 1;
								relation[source.m_pData[i+x][j+y][k+z]-1][pixel-1] = 1;
							}
						} else {
							if ( m_Voxel.m_pData[i+x][j+y][k+z] ) {
								row[pixel-1] = 1;
							}
						}
					}
					x = 0; y = 0; z = 1;
					if ( !(i+x < 0 || i+x >= XSize)
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( source.m_pData[i+x][j+y][k+z] ) {
							if ( source.m_pData[i+x][j+y][k+z] != pixel ) {
								relation[pixel-1][source.m_pData[i+x][j+y][k+z]-1] = 1;
								relation[source.m_pData[i+x][j+y][k+z]-1][pixel-1] = 1;
							}
						} else {
							if ( m_Voxel.m_pData[i+x][j+y][k+z] ) {
								row[pixel-1] = 1;
							}
						}
					}
				}
			}
		}
	}
	
	for ( i = 0 ; i < num_of_part ; i++ ) {		
		for ( j = 0 ; j < num_of_part ; j++ ) {
			if ( relation[i][j] )
				row[i]++;
		}
	}

	int num_leaf = 0;
	for ( i = 0 ; i < num_of_part ; i++ ) {	
		if( row[i] == 1 )
			num_leaf++;
	}

	for ( i = 0 ; i < num_of_part ; i++ )
		delete [] relation[i];
	delete [] relation;

	delete [] row;

	if ( num_leaf < 2 ) return false;
	return true;
}

void CSplit::Residual(CVoxel &body, CVoxel &branch, CVoxel &source)
{
	int i, j, k;
	int x, y, z;
	int XSize = m_Voxel.GetXSize();
	int YSize = m_Voxel.GetYSize();
	int ZSize = m_Voxel.GetZSize();

	int num_of_body = body.Component();
	int num_of_branch = branch.Component();
	
	int ** neighbor = new int * [num_of_branch];
	int * outlier = new int [num_of_branch];
	int * merge = new int [num_of_branch];
	int * num = new int [num_of_branch];	
	
	for ( i = 0 ; i < num_of_branch ; i++ ) {
		neighbor[i] = new int [num_of_body];
		for ( j = 0 ; j < num_of_body ; j++ ) {
			neighbor[i][j] = 0;
		}
		outlier[i] = 0; merge[i] = 0; num[i] = 0;
	}

	int * check = new int [num_of_body];
	for ( i = 0 ; i < num_of_body ; i++ ) {
		check[i] = 0;
	}

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				if ( branch.m_pData[i][j][k] ) {
					bool test = false;
					x = -1; y = 0; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( body.m_pData[i+x][j+y][k+z] )
							check[body.m_pData[i+x][j+y][k+z]-1] = 1;
						else
							if ( m_Voxel.m_pData[i+x][j+y][k+z] && !branch.m_pData[i+x][j+y][k+z] ) test = true;
					}
					x = 1; y = 0; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( body.m_pData[i+x][j+y][k+z] )
							check[body.m_pData[i+x][j+y][k+z]-1] = 1;
						else
							if ( m_Voxel.m_pData[i+x][j+y][k+z] && !branch.m_pData[i+x][j+y][k+z] ) test = true;
					}
					x = 0; y = -1; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( body.m_pData[i+x][j+y][k+z] )
							check[body.m_pData[i+x][j+y][k+z]-1] = 1;
						else
							if ( m_Voxel.m_pData[i+x][j+y][k+z] && !branch.m_pData[i+x][j+y][k+z] ) test = true;
					}
					x = 0; y = 1; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( body.m_pData[i+x][j+y][k+z] )
							check[body.m_pData[i+x][j+y][k+z]-1] = 1;
						else
							if ( m_Voxel.m_pData[i+x][j+y][k+z] && !branch.m_pData[i+x][j+y][k+z] ) test = true;
					}
					x = 0; y = 0; z = -1;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( body.m_pData[i+x][j+y][k+z] )
							check[body.m_pData[i+x][j+y][k+z]-1] = 1;
						else
							if ( m_Voxel.m_pData[i+x][j+y][k+z] && !branch.m_pData[i+x][j+y][k+z] ) test = true;
					}
					x = 0; y = 0; z = 1;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize)
						&& !(k+z < 0 || k+z >= ZSize) ) {
						if ( body.m_pData[i+x][j+y][k+z] )
							check[body.m_pData[i+x][j+y][k+z]-1] = 1;
						else
							if ( m_Voxel.m_pData[i+x][j+y][k+z] && !branch.m_pData[i+x][j+y][k+z] ) test = true;
					}
					if ( test )
						outlier[branch.m_pData[i][j][k]-1]++;
				
					for ( x = 0 ; x < num_of_body ; x++ ) {
						if ( check[x] ) neighbor[branch.m_pData[i][j][k]-1][x]++;
						check[x] = 0;
					}
					num[branch.m_pData[i][j][k]-1]++;
				}
			}
		}
	}

	int valid_branch = 0;
	for ( i = 0 ; i < num_of_branch ; i++ ) {			
		int index = -1, max = 0;		
		int proxy = 0;			
		for ( j = 0 ; j < num_of_body ; j++ ) {
			if ( neighbor[i][j] ) {
				if ( max < neighbor[i][j] ) {
					index = j; max = neighbor[i][j];
				}
				proxy += neighbor[i][j];
			}
		}
		proxy += outlier[i];
		
		if ( (double) num[i]/proxy > TH_RESIDUE )
			merge[i] = ++valid_branch; // the threshold for the residual part
		else
			merge[i] = -index-1;

	}		

	for ( i = 0 ; i < num_of_branch ; i++ )
		delete [] neighbor[i];
	delete [] neighbor;
	delete [] check;
	delete [] num;
	delete [] outlier;

	source.root = num_of_body;
	source.link = 0;
	source.leaf = valid_branch;	

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				source.m_pData[i][j][k] = body.m_pData[i][j][k];
				int missing = branch.m_pData[i][j][k];
				if ( missing ) {
					if ( merge[branch.m_pData[i][j][k]-1] > 0 )
						source.m_pData[i][j][k] = num_of_body + merge[missing-1];
					else
						source.m_pData[i][j][k] = -merge[missing-1];
				}
			}
		}
	}
	delete [] merge;
}

void CSplit::Clear()
{
	int i, j, k;

	if ( m_nDepth != 0 ) {
		for ( i = m_nDepth-1 ; i >= 0 ; i-- ) {
			for ( j = m_pArray[i].size()-1 ; j >= 0 ; j-- ) {
				CPart *pPart = m_pArray[i][j];
				for ( k = pPart->pDot.size()-1 ; k >= 0 ; k-- ) {
					CDot *pDot = pPart->pDot[k];
					//pPart->pDot.RemoveAt(k);delete [] pDot;
          delete pDot;pPart->pDot.erase(pPart->pDot.begin()+k);
				}
				//m_pArray[i].RemoveAt(j);delete [] pPart;
        m_pArray[i].erase(m_pArray[i].begin()+j);delete pPart;
			}
		}
	} else {
		for ( j = m_pArray[0].size()-1 ; j >= 0 ; j-- ) {
			CPart *pPart = m_pArray[0][j];
			for ( k = pPart->pDot.size()-1 ; k >= 0 ; k-- ) {
				CDot *pDot = pPart->pDot[k];
				//pPart->pDot.RemoveAt(k);delete [] pDot;
        pPart->pDot.erase(pPart->pDot.begin()+k);delete pDot;
			}
			//m_pArray[0].RemoveAt(j);delete [] pPart;
      m_pArray[0].erase(m_pArray[0].begin()+j);delete pPart;
		}
	}

	for ( i = m_pPart.size()-1 ; i >= 0 ; i-- ) {
		CPart *pPart = m_pPart[i];
		for ( j = pPart->pDot.size()-1 ; j >= 0 ; j-- ) {
			CDot *pDot = pPart->pDot[j];
			//pPart->pDot.RemoveAt(j);delete [] pDot;
      pPart->pDot.erase(pPart->pDot.begin()+j);delete pDot;
		}
		//m_pPart.RemoveAt(i);delete [] pPart;
    m_pPart.erase(m_pPart.begin()+i);delete pPart;
	}

	m_nDivX = m_nDivY = m_nDivZ = 0;
	m_Voxel.Free(); m_Part.Free();
}

void CSplit::EarlyProcess(CVoxel &body, CVoxel &branch, int iteration)
{
	int i, j, k;
	int x, y, z;
	int XSize = body.GetXSize();
	int YSize = body.GetYSize();
	int ZSize = body.GetZSize();

	int num = body.GetNumber() + branch.GetNumber();
	int count = 0;

	CVoxel root;
	root.Allocate(XSize, YSize, ZSize);
	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				root.m_pData[i][j][k] = body.m_pData[i][j][k];
			}
		}
	}

	CVoxel bench;
	bench.Allocate(XSize, YSize, ZSize);

	while ( ++count <= iteration ) {
		for ( i = 0 ; i < XSize ; i++ ) {
			for ( j = 0 ; j < YSize ; j++ ) {
				for ( k = 0 ; k < ZSize ; k++ ) {
					if ( body.m_pData[i][j][k] ) {
						for ( x = -1 ; x <= 1 ; x++ ) {
							for ( y = -1 ; y <= 1 ; y++ ) {
								for ( z = -1 ; z <= 1 ; z++ ) {
									if ( !(i+x < 0 || i+x >= XSize)
										&& !(j+y < 0 || j+y >= YSize)
										&& !(k+z < 0 || k+z >= ZSize)
										&& branch.m_pData[i+x][j+y][k+z] ) {
										bench.m_pData[i+x][j+y][k+z] = 1;
									}
								}
							}
						}
					}
				}
			}
		}

		for ( i = 0 ; i < XSize ; i++ ) {
			for ( j = 0 ; j < YSize ; j++ ) {
				for ( k = 0 ; k < ZSize ; k++ ) {
					if ( bench.m_pData[i][j][k] ) {						
						body.m_pData[i][j][k] = 1;
						branch.m_pData[i][j][k] = 0;
						bench.m_pData[i][j][k] = 0;
					}
				}
			}
		}
	}

	count = 0;
	while ( ++count <= iteration ) {
		for ( i = 0 ; i < XSize ; i++ ) {
			for ( j = 0 ; j < YSize ; j++ ) {
				for ( k = 0 ; k < ZSize ; k++ ) {
					if ( branch.m_pData[i][j][k] ) {
						for ( x = -1 ; x <= 1 ; x++ ) {
							for ( y = -1 ; y <= 1 ; y++ ) {
								for ( z = -1 ; z <= 1 ; z++ ) {
									if ( !(i+x < 0 || i+x >= XSize)
										&& !(j+y < 0 || j+y >= YSize)
										&& !(k+z < 0 || k+z >= ZSize)
										&& body.m_pData[i+x][j+y][k+z] ) {
										bench.m_pData[i+x][j+y][k+z] = 1;
									}
								}
							}
						}
					}
				}
			}
		}

		for ( i = 0 ; i < XSize ; i++ ) {
			for ( j = 0 ; j < YSize ; j++ ) {
				for ( k = 0 ; k < ZSize ; k++ ) {
					if ( bench.m_pData[i][j][k] ) {
						body.m_pData[i][j][k] = 0;
						branch.m_pData[i][j][k] = 1;
						bench.m_pData[i][j][k] = 0;
					}
				}
			}
		}
	}

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				bench.m_pData[i][j][k] = body.m_pData[i][j][k];
			}
		}
	}
	count = bench.Component();
	int * overlap = new int [count];
	for ( i = 0 ; i < count ; i++ ) overlap[i] = 0;

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				if ( bench.m_pData[i][j][k] && root.m_pData[i][j][k] ) {
					overlap[bench.m_pData[i][j][k]-1] = 1;
				}
			}
		}
	}

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				if ( bench.m_pData[i][j][k] ) {
					if ( !overlap[bench.m_pData[i][j][k]-1] ) {
						body.m_pData[i][j][k] = 0;
						branch.m_pData[i][j][k] = 1;
					}
				}
			}
		}
	}

	delete [] overlap;
}

void CSplit::MergePart()
{
	int a;
	int i, j, k;	
	int x, y;

	int XSize = m_Part.GetXSize();
	int YSize = m_Part.GetYSize();	
	int ZSize = m_Part.GetZSize();
	int nObject = m_Part.GetNumber();
	int nPart = m_pPart.size();
	
	CVoxel bench;
	bench.Allocate(XSize, YSize, ZSize);

	//////////////////////////////////////////////////////////////////	
	int nCount = 0;
	info_mrg * info = new info_mrg [nPart*(nPart-1)/2];	
	for ( x = 0 ; x < m_pPart.size() ; x++ ) {
		for ( y = 0 ; y < m_pPart[x]->pPart.size() ; y++ ) {
			if ( m_pPart[x]->index < m_pPart[x]->pPart[y]->index ) {
				info[nCount].p0 = m_pPart[x]; info[nCount].p1 = m_pPart[x]->pPart[y];
				for ( i = 0 ; i < XSize ; i++ ) {
					for ( j = 0 ; j < YSize ; j++ ) {
						for ( k = 0 ; k < ZSize ; k++ ) bench.m_pData[i][j][k] = 0;
					}
				}

				int n0 = 0; int n1 = 0;
				for ( i = 0 ; i < info[nCount].p0->pDot.size() ; i++ ) {
					CDot * pDot = info[nCount].p0->pDot[i];
					bench.m_pData[pDot->x][pDot->y][pDot->z] = 1;
					n0++;
				}
				for ( i = 0 ; i < info[nCount].p1->pDot.size() ; i++ ) {
					CDot * pDot = info[nCount].p1->pDot[i];
					bench.m_pData[pDot->x][pDot->y][pDot->z] = 1;
					n1++;
				}
							
				info[nCount].msr  = info[nCount].p0->convexity + info[nCount].p1->convexity;
				info[nCount].msr -= pow((double)(n0+n1),2)/bench.Hull();

				nCount++;
			}
		}
	}

	qsort( info, nCount, sizeof(info_mrg), compare_mrg );

	for ( a = 0 ; a < nCount ; a++ ) {
		double msr_g = info[a].msr/nObject;
		double msr_l = info[a].msr/(info[a].p0->pDot.size()+info[a].p1->pDot.size());
				
		if ( !(msr_g > 0.005) && !(msr_l > 0.03) ) {
			CPart * pTake = NULL; CPart * pGive = NULL;
			if ( info[a].p0->form != info[a].p1->form ) {
				if ( info[a].p0->form == 0 ) {
					pTake = info[a].p0; pGive = info[a].p1;
				} else {
					pTake = info[a].p1; pGive = info[a].p0;
				}
			} else {
				if ( info[a].p0->pDot.size() > info[a].p1->pDot.size() ) {
					pTake = info[a].p0; pGive = info[a].p1;
				} else {
					pTake = info[a].p1; pGive = info[a].p0;
				}
			}

			for ( i = 0 ; i < m_pPart.size() ; i++ ) {
				if ( m_pPart[i] == pGive ) {
					m_pPart.erase(m_pPart.begin()+i);//m_pPart.RemoveAt(i);
					for ( j = i ; j < m_pPart.size(); j++ ) {
						m_pPart[j]->index--;
					}					
					break;				
				}
			}
			for ( i = 0 ; i < pGive->pDot.size() ; i++ ) {
				pTake->pDot.push_back(pGive->pDot[i]);
			}
			pTake->convexity = pow((double)pTake->pDot.size(),2)/m_Part.Hull(pTake->pDot);
			for ( i = nCount-1 ; i >= 0 ; i-- ) {
				if ( info[i].p0 == pGive || info[i].p1 == pGive ) {
					for ( j = i ; j < nCount-1 ; j++ ) {
						info[j].msr = info[j+1].msr; info[j].p0 = info[j+1].p0; info[j].p1 = info[j+1].p1;
					}
					nCount--;
				}
			}			
			for ( x = 0 ; x < pGive->pPart.size() ; x++ ) {				
				for ( i = 0 ; i < pGive->pPart[x]->pPart.size() ; i++ ) {
					if ( pGive == pGive->pPart[x]->pPart[i] ) {
						pGive->pPart[x]->pPart.erase(pGive->pPart[x]->pPart.begin()+i);//pGive->pPart[x]->pPart.RemoveAt(i); 
            break;
					}
				}
				if ( pTake != pGive->pPart[x] ) {
					bool bTest = false;
					for ( i = 0 ; i < pGive->pPart[x]->pPart.size() ; i++ ) {
						if ( pTake == pGive->pPart[x]->pPart[i] ) {
							bTest = true; break;
						}
					}
					if ( !bTest ) {
						pTake->pPart.push_back(pGive->pPart[x]);
						pGive->pPart[x]->pPart.push_back(pTake);

						info[nCount].p0 = pTake; info[nCount].p1 = pGive->pPart[x];						

						nCount++;
					}
				}
			}
			for ( x = 0 ; x < nCount ; x++ ) {
				if ( info[x].p0 == pTake || info[x].p1 == pTake ) {
					for ( i = 0 ; i < XSize ; i++ ) {
						for ( j = 0 ; j < YSize ; j++ ) {
							for ( k = 0 ; k < ZSize ; k++ ) bench.m_pData[i][j][k] = 0;
						}
					}

					int n0 = 0; int n1 = 0;
					for ( i = 0 ; i < info[x].p0->pDot.size() ; i++ ) {
						CDot * pDot = info[x].p0->pDot[i];
						bench.m_pData[pDot->x][pDot->y][pDot->z] = 1;
						n0++;
					}
					for ( i = 0 ; i < info[x].p1->pDot.size() ; i++ ) {
						CDot * pDot = info[x].p1->pDot[i];
						bench.m_pData[pDot->x][pDot->y][pDot->z] = 1;
						n1++;
					}

					info[x].msr  = info[x].p0->convexity + info[x].p1->convexity;
					info[x].msr -= pow((double)(n0+n1),2)/bench.Hull();
				}
			}

			qsort( info, nCount, sizeof(info_mrg), compare_mrg );
      //zhanghui noted: it seems this one is correct.
			delete pGive; //delete [] pGive; 
      a = -1; // refreshment;
		}
	}

	delete [] info;
}

void CSplit::MakeSE()
{
	int i, j, k, num;

	for ( num = 1 ; num <= RANK ; num++ ) {		
		SE[num-1].Allocate(2*num+1,2*num+1,2*num+1);
		for ( i = 0 ; i < 2*num+1 ; i++ ) {
			for ( j = 0 ; j < 2*num+1 ; j++ ) {
				for ( k = 0 ; k < 2*num+1 ; k++ ) {
					double radius = sqrt(pow((double)(i-num),2)+pow((double)(j-num),2)+pow((double)(k-num),2));
					if ( radius <= num ) {
						SE[num-1].m_pData[i][j][k] = 1;					
					}
				}
			}
		}
	}
}

void CSplit::PartSplit()
{	
	while ( Decompose() );		
}

void CSplit::PartMerge()
{
	int i, j, k;
	int x, y, z;
	
	int nPart = m_pArray[m_nDepth].size();	
	for ( i = 0 ; i < nPart ; i++ ) {
		CPart * pPart = m_pArray[m_nDepth][i]; m_pPart.push_back(pPart);
		pPart->index = i+1;
		for ( j = 0 ; j < pPart->pDot.size() ; j++ ) {
			CDot * pDot = pPart->pDot[j];
			x = pDot->x; y = pDot->y; z = pDot->z;
			m_Part.m_pData[x][y][z] = i+1;
		}
		pPart->convexity = pow((double)pPart->pDot.size(),2)/m_Part.Hull(pPart->pDot);
	}
	m_pArray[m_nDepth].clear();

	int ** relation = new int * [nPart];
	for ( i = 0 ; i < nPart ; i++ ) {
		relation[i] = new int [nPart];
		for ( j = 0 ; j < nPart ; j++ )	relation[i][j] = 0;
	}

	int XSize = m_Part.GetXSize();
	int YSize = m_Part.GetYSize();
	int ZSize = m_Part.GetZSize();

	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				int voxel = m_Part.m_pData[i][j][k];
				if ( voxel ) {
					x = -1; y = 0; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize) 
						&& !(k+z < 0 || k+z >= ZSize)
						&& m_Part.m_pData[i+x][j+y][k+z] > 0 ) {
						int index = m_Part.m_pData[i+x][j+y][k+z];
						relation[voxel-1][index-1] = 1;
					}
					x = 1; y = 0; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize) 
						&& !(k+z < 0 || k+z >= ZSize)
						&& m_Part.m_pData[i+x][j+y][k+z] > 0 ) {
						int index = m_Part.m_pData[i+x][j+y][k+z];
						relation[voxel-1][index-1] = 1;
					}
					x = 0; y = -1; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize) 
						&& !(k+z < 0 || k+z >= ZSize)
						&& m_Part.m_pData[i+x][j+y][k+z] > 0 ) {
						int index = m_Part.m_pData[i+x][j+y][k+z];
						relation[voxel-1][index-1] = 1;
					}
					x = 0; y = 1; z = 0;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize) 
						&& !(k+z < 0 || k+z >= ZSize)
						&& m_Part.m_pData[i+x][j+y][k+z] > 0 ) {
						int index = m_Part.m_pData[i+x][j+y][k+z];
						relation[voxel-1][index-1] = 1;
					}
					x = 0; y = 0; z = -1;
					if ( !(i+x < 0 || i+x >= XSize) 
						&& !(j+y < 0 || j+y >= YSize) 
						&& !(k+z < 0 || k+z >= ZSize)
						&& m_Part.m_pData[i+x][j+y][k+z] > 0 ) {
						int index = m_Part.m_pData[i+x][j+y][k+z];
						relation[voxel-1][index-1] = 1;
					}
					x = 0; y = 0; z = 1;
					if ( !(i+x < 0 || i+x >= XSize)
						&& !(j+y < 0 || j+y >= YSize) 
						&& !(k+z < 0 || k+z >= ZSize)
						&& m_Part.m_pData[i+x][j+y][k+z] > 0 ) {
						int index = m_Part.m_pData[i+x][j+y][k+z];
						relation[voxel-1][index-1] = 1;
					}
				}
			}
		}
	}
	
	for ( i = 0 ; i < nPart ; i++ ) {		
		for ( j = 0 ; j < nPart ; j++ ) {
			if ( i != j && relation[i][j] ) {
				m_pPart[i]->pPart.push_back(m_pPart[j]);
			}
		}
	}

	for ( i = 0 ; i < nPart ; i++ ) delete [] relation[i];
	delete [] relation;

	MergePart();

	nPart = m_pPart.size();
	int nRoot = 0; int nLeaf = 0;	
	for ( i = 0 ; i < nPart ; i++ ) {
		CPart * pPart = m_pPart[i];		
		for ( j = 0 ; j < pPart->pDot.size() ; j++ ) {
			CDot * pDot = pPart->pDot[j];
			x = pDot->x; y = pDot->y; z = pDot->z;
			m_Part.m_pData[x][y][z] = i+1;

			pPart->mean.x += x;
			pPart->mean.y += y;
			pPart->mean.z += z;
			pPart->num++;
		}
		if ( pPart->form == 0 ) nRoot++; // root
		if ( pPart->form == 1 ) nLeaf++; // leaf
		ModelAssay(pPart);
	}
	m_Part.root = nRoot;
	m_Part.link = 0;
	m_Part.leaf = nLeaf;
}

void CSplit::ModelAssay(CPart *pPart)
{
	int i;

	int size = pPart->num;
	int XSize = m_Part.GetXSize();
	int YSize = m_Part.GetYSize();
	int ZSize = m_Part.GetZSize();

	CVoxel part; part.Allocate(XSize, YSize, ZSize);

	CDot max, min;
	max.x = pPart->pDot[0]->x; max.y = pPart->pDot[0]->y; max.z = pPart->pDot[0]->z;
	min.x = pPart->pDot[0]->x; min.y = pPart->pDot[0]->y; min.z = pPart->pDot[0]->z;
	for ( i = 0 ; i < size ; i++ ) {
		CDot * pDot = pPart->pDot[i]; 
		part.m_pData[pDot->x][pDot->y][pDot->z] = 1;
		if ( pDot->x > max.x ) max.x = pDot->x; if ( pDot->x < min.x ) min.x = pDot->x;
		if ( pDot->y > max.y ) max.y = pDot->y; if ( pDot->y < min.y ) min.y = pDot->y;
		if ( pDot->z > max.z ) max.z = pDot->z; if ( pDot->z < min.z ) min.z = pDot->z;
	}
	pPart->convexity = (double) size/part.Hull();
	
	float a[3*3]; float v[3*3]; float d[3];
	for ( i = 0 ; i < 3*3 ; i++ ) { a[i] = 0; v[i] = 0; }
	for ( i = 0 ; i < 3 ; i++ ) d[i] = 0;

	float x, y, z;
	float c_x, c_y, c_z;
	c_x = (float) pPart->mean.x/size; c_y = (float) pPart->mean.y/size; c_z = (float) pPart->mean.z/size;

	for ( i = 0 ; i < size ; i++ ) {
		x = (float) pPart->pDot[i]->x - c_x;
		y = (float) pPart->pDot[i]->y - c_y;
		z = (float) pPart->pDot[i]->z - c_z;
		
		a[0] += x*x; a[1] += x*y; a[2] += x*z;
		a[4] += y*y; a[5] += y*z; a[8] += z*z;
	}
	a[3] = a[1]; a[6] = a[2]; a[7] = a[5];

	for ( i = 0 ; i < 3*3 ; i++ ) a[i] /= size;

	CvMat A = cvMat( 3, 3, CV_32F, a ); // symmetric matrix A
	CvMat Evects = cvMat( 3, 3, CV_32F, v ); // eigenvectors
	CvMat Evals = cvMat( 3, 1, CV_32F, d ); // eigenvalues
	cvEigenVV( &A, &Evects, &Evals, 1e-10 );

	pPart->mat[0] = v[0]; pPart->mat[1] = v[3]; pPart->mat[2] = v[6];
	pPart->mat[3] = v[1]; pPart->mat[4] = v[4]; pPart->mat[5] = v[7];
	pPart->mat[6] = v[2]; pPart->mat[7] = v[5]; pPart->mat[8] = v[8];

	float x_max = 0, y_max = 0, z_max = 0;	
	for ( i = 0 ; i < size ; i++ ) {
		CDot * pDot = pPart->pDot[i];
		x = (float) (v[0]*(pDot->x-c_x) + v[1]*(pDot->y-c_y) + v[2]*(pDot->z-c_z));
		y = (float) (v[3]*(pDot->x-c_x) + v[4]*(pDot->y-c_y) + v[5]*(pDot->z-c_z));
		z = (float) (v[6]*(pDot->x-c_x) + v[7]*(pDot->y-c_y) + v[8]*(pDot->z-c_z));
		x = (float) (fabs(x)+0.5); y = (float) (fabs(y)+0.5); z = (float) (fabs(z)+0.5);

		if ( x > x_max ) x_max = x; if ( y > y_max ) y_max = y; if ( z > z_max ) z_max = z;
	}	
	pPart->length.x = (float) x_max; pPart->length.y = (float) y_max; pPart->length.z = (float) z_max;
}


////////////////////////////////////////////////////////////////////////////////
//from stdafx.cpp
/////////////////////////////////////////////////////////////////////////////
int compare_mrg( const void *arg1, const void *arg2 )
{
	info_mrg * pE1 = (info_mrg *) arg1; info_mrg * pE2 = (info_mrg *) arg2;

	if ( pE1->msr > pE2->msr ) return 1;
	return -1;
}

////////////////////////////////////////////////////////////////////////////////
//from Model.cpp
/////////////////////////////////////////////////////////////////////////////
CModel::CModel()
{
	m_bOpen = FALSE;
}

CModel::~CModel() {
	Clear();
}

void CModel::Clear()
{
	m_nPoint = 0;
	m_nPolygon = 0;

	int i;
	for ( i = m_pVertex.size()-1 ; i >= 0 ; i-- ) {
		CVertex * pVertex = m_pVertex[i];
		pVertex->pFace.clear();//pVertex->pFace.RemoveAll();
		delete pVertex;
		//m_pVertex.RemoveAt(i);		
    m_pVertex.erase(m_pVertex.begin()+i);
	}

	for ( i = m_pMesh.size()-1 ; i >= 0 ; i-- ) {
		CMesh * pMesh = m_pMesh[i];
		//m_pMesh.RemoveAt(i);
    m_pMesh.erase(m_pMesh.begin()+i);
		delete pMesh;
	}
}

BOOL CModel::Open(MediaThreeD* media3d)//(LPCTSTR lpszPathName)
{
//	fstream in;
//	in.open(lpszPathName, ios::in|ios::nocreate);
//	if ( !in.is_open() ) { in.close(); return FALSE; }
//	in.close();

	Clear();

//	CString ext; ext.Format("%s",lpszPathName);	ext = ext.Right(3);
//	if ( ext == "wrl" )	m_bOpen = ReadWrl(lpszPathName);
  m_bOpen = ReadWrl(media3d);

	if ( m_bOpen ) {
		Translate();
		Align(); Adjust(); // Align by PCA and Adjust using std

		m_Max[0] = -INF; m_Max[1] = -INF; m_Max[2] = -INF;
		m_Min[0] = INF;	m_Min[1] = INF;	m_Min[2] = INF;

		int i;
		for ( i = 0 ; i < m_pVertex.size() ; i++ ) {
			m_pVertex[i]->nIndex = i;

			if ( m_Max[0] < m_pVertex[i]->x ) m_Max[0] = m_pVertex[i]->x;
			if ( m_Max[1] < m_pVertex[i]->y ) m_Max[1] = m_pVertex[i]->y;
			if ( m_Max[2] < m_pVertex[i]->z ) m_Max[2] = m_pVertex[i]->z;

			if ( m_Min[0] > m_pVertex[i]->x ) m_Min[0] = m_pVertex[i]->x;
			if ( m_Min[1] > m_pVertex[i]->y ) m_Min[1] = m_pVertex[i]->y;
			if ( m_Min[2] > m_pVertex[i]->z ) m_Min[2] = m_pVertex[i]->z;
		}

		double width_x = m_Max[0]-m_Min[0];
		double width_y = m_Max[1]-m_Min[1];
		double width_z = m_Max[2]-m_Min[2];
		m_dDiagonal = (float) sqrt(pow(width_x,2) + pow(width_y,2) + pow(width_z,2));

		MakeNormal();

		return TRUE;
	}

	return FALSE;
}

BOOL CModel::Save(LPCTSTR lpszPathName)
{
	int i;	

	FILE *output = fopen(lpszPathName, "w");

    fprintf(output,"#VRML V2.0 utf8 (Converted to ASCII)\n");
	fprintf(output,"Shape {\n");
	fprintf(output,"	geometry IndexedFaceSet {\n");
	fprintf(output,"		coord Coordinate {\n");
	fprintf(output,"			point [\n");
	for ( i = 0 ; i < m_nPoint ; i++ )
		fprintf(output,"%f %f %f,\n",m_pVertex[i]->x,m_pVertex[i]->y,m_pVertex[i]->z);
	fprintf(output," ]\n");
	fprintf(output," }\n");
	fprintf(output,"                         coordIndex [\n");
	for ( i = 0 ; i < m_nPolygon ; i++ )
		fprintf(output,"%d, %d, %d, -1,\n",m_pMesh[i]->a,m_pMesh[i]->b,m_pMesh[i]->c);
	fprintf(output," ]\n");
	fprintf(output," }\n");
	fprintf(output," }\n"); 

	fclose(output);
	    
    return TRUE;
}


BOOL CModel::ReadWrl(MediaThreeD *media3d)//(LPCSTR lpszPathName)
{
	int i;

	m_nPoint = 0; m_nPolygon = 0;

	m_nPoint = media3d->NoPoints;
	m_nPolygon = media3d->NoTriangl;

 	for ( i = 0; i < m_nPoint; i++ ) {
        CVertex * pVertex = new CVertex;
        pVertex->x = media3d->PObj[i][0];
		pVertex->y = media3d->PObj[i][1];
		pVertex->z = media3d->PObj[i][2];        
        m_pVertex.push_back(pVertex);
 	}
	
	for ( i = 0; i < m_nPolygon; i++ ) {
        CMesh * pMesh = new CMesh;
		pMesh->a = media3d->TObj[i][0];
		pMesh->b = media3d->TObj[i][1];
		pMesh->c = media3d->TObj[i][2];
		
        m_pMesh.push_back(pMesh);

		m_pVertex[pMesh->a]->pFace.push_back(pMesh);
		m_pVertex[pMesh->b]->pFace.push_back(pMesh);
		m_pVertex[pMesh->c]->pFace.push_back(pMesh);
 	}
	
	return TRUE;
}

void CModel::Align()
{
	int i;
	
	float a[3*3], v[3*3], d[3];	
	for ( i = 0 ; i < 3*3 ; i++ ) {	a[i] = 0; v[i] = 0; }
	for ( i = 0 ; i < 3 ; i++ ) d[i] = 0;

	int point = m_pVertex.size();
	int polygon = m_pMesh.size();

	float sum = 0;
	float x, y, z;
	for ( i = 0 ; i < polygon ; i++ ) {
		x = (float) (m_pVertex[m_pMesh[i]->a]->x + m_pVertex[m_pMesh[i]->b]->x + m_pVertex[m_pMesh[i]->c]->x)/3;
		y = (float) (m_pVertex[m_pMesh[i]->a]->y + m_pVertex[m_pMesh[i]->b]->y + m_pVertex[m_pMesh[i]->c]->y)/3;
		z = (float) (m_pVertex[m_pMesh[i]->a]->z + m_pVertex[m_pMesh[i]->b]->z + m_pVertex[m_pMesh[i]->c]->z)/3;
		
		a[0] += (float) m_pMesh[i]->area*x*x; a[1] += (float) m_pMesh[i]->area*x*y; a[2] += (float) m_pMesh[i]->area*x*z;
		a[4] += (float) m_pMesh[i]->area*y*y; a[5] += (float) m_pMesh[i]->area*y*z; a[8] += (float) m_pMesh[i]->area*z*z;
		sum += (float) m_pMesh[i]->area;
	}
	a[3] = a[1]; a[6] = a[2]; a[7] = a[5];

	for ( i = 0 ; i < 3*3 ; i++ ) a[i] /= sum;

	CvMat A = cvMat( 3, 3, CV_32F, a ); // the symmetric matrix A
	CvMat Evects = cvMat( 3, 3, CV_32F, v ); // eigenvectors
	CvMat Evals = cvMat( 3, 1, CV_32F, d ); // eigenvalues
	
	cvEigenVV( &A, &Evects, &Evals, 1e-10 );
	
	for ( i = 0 ; i < point ; i++ ) {
		x = (float) m_pVertex[i]->x; y = (float) m_pVertex[i]->y; z = (float) m_pVertex[i]->z;
		
		m_pVertex[i]->x = v[0]*x + v[1]*y + v[2]*z;
		m_pVertex[i]->y = v[3]*x + v[4]*y + v[5]*z;
		m_pVertex[i]->z = v[6]*x + v[7]*y + v[8]*z;
 	}
}

void CModel::Adjust()
{
	int i;
	double x, y, z;

	int point = m_pVertex.size();
	int polygon = m_pMesh.size();

	double x_mean_plus = 0, x_mean_minus = 0;
	double x_pow_plus = 0, x_pow_minus = 0;
	double x_std_plus = 0, x_std_minus = 0;	
	double x_sum_plus = 0, x_sum_minus = 0;

	double y_mean_plus = 0, y_mean_minus = 0;
	double y_pow_plus = 0, y_pow_minus = 0;
	double y_std_plus = 0, y_std_minus = 0;	
	double y_sum_plus = 0, y_sum_minus = 0;
	
	for ( i = 0 ; i < polygon ; i++ ) {
		x = y = z = 0;
		x = (m_pVertex[m_pMesh[i]->a]->x + m_pVertex[m_pMesh[i]->b]->x + m_pVertex[m_pMesh[i]->c]->x)/3;
		y = (m_pVertex[m_pMesh[i]->a]->y + m_pVertex[m_pMesh[i]->b]->y + m_pVertex[m_pMesh[i]->c]->y)/3;
		z = (m_pVertex[m_pMesh[i]->a]->z + m_pVertex[m_pMesh[i]->b]->z + m_pVertex[m_pMesh[i]->c]->z)/3;
		
		if ( x > 0 ) {
			x_mean_plus += (float) m_pMesh[i]->area*sqrt(pow(x,2)+pow(y,2)+pow(z,2));
			x_pow_plus += (float) m_pMesh[i]->area*(pow(x,2)+pow(y,2)+pow(z,2));			
			x_sum_plus += (float) m_pMesh[i]->area;
		} else { 
			x_mean_minus += (float) m_pMesh[i]->area*sqrt(pow(x,2)+pow(y,2)+pow(z,2));
			x_pow_minus += (float) m_pMesh[i]->area*(pow(x,2)+pow(y,2)+pow(z,2));
			x_sum_minus += (float) m_pMesh[i]->area;
		}

		if ( y > 0 ) {
			y_mean_plus += (float) m_pMesh[i]->area*sqrt(pow(x,2)+pow(y,2)+pow(z,2));
			y_pow_plus += (float) m_pMesh[i]->area*(pow(x,2)+pow(y,2)+pow(z,2));			
			y_sum_plus += (float) m_pMesh[i]->area;
		} else { 
			y_mean_minus += (float) m_pMesh[i]->area*sqrt(pow(x,2)+pow(y,2)+pow(z,2));
			y_pow_minus += (float) m_pMesh[i]->area*(pow(x,2)+pow(y,2)+pow(z,2));			
			y_sum_minus += (float) m_pMesh[i]->area;
		}
	}
	
	x_std_plus = (float) sqrt(x_pow_plus/x_sum_plus - pow(x_mean_plus/x_sum_plus,2));
	x_std_minus = (float) sqrt(x_pow_minus/x_sum_minus - pow(x_mean_minus/x_sum_minus,2));

	y_std_plus = (float) sqrt(y_pow_plus/y_sum_plus - pow(y_mean_plus/y_sum_plus,2));
	y_std_minus = (float) sqrt(y_pow_minus/y_sum_minus - pow(y_mean_minus/y_sum_minus,2));

	float x_sign = 1, y_sign = 1, z_sign = 1;

	if ( x_std_plus < x_std_minus ) x_sign = -1;
	if ( y_std_plus < y_std_minus ) y_sign = -1;
	if ( x_sign*y_sign < 0 ) z_sign = -1;

	for ( i = 0 ; i < point ; i++ ) {
		m_pVertex[i]->x = x_sign*m_pVertex[i]->x;
		m_pVertex[i]->y = y_sign*m_pVertex[i]->y;
		m_pVertex[i]->z = z_sign*m_pVertex[i]->z;
	}
}

void CModel::MakeNormal()
{
	int i;

	double x1, y1, z1;
	double x2, y2, z2;

	for ( i = 0 ; i < m_pMesh.size() ; i++ ) {	
		CMesh * pMesh = m_pMesh[i];
		x1 = m_pVertex[pMesh->b]->x-m_pVertex[pMesh->a]->x;
		x2 = m_pVertex[pMesh->c]->x-m_pVertex[pMesh->a]->x;
		y1 = m_pVertex[pMesh->b]->y-m_pVertex[pMesh->a]->y;
		y2 = m_pVertex[pMesh->c]->y-m_pVertex[pMesh->a]->y;
		z1 = m_pVertex[pMesh->b]->z-m_pVertex[pMesh->a]->z;
		z2 = m_pVertex[pMesh->c]->z-m_pVertex[pMesh->a]->z;

		pMesh->normal.x = y1*z2 - y2*z1;
		pMesh->normal.y = z1*x2 - x1*z2;
		pMesh->normal.z = x1*y2 - y1*x2;
	}
}

void CModel::Translate()
{
	int i;
	int polygon = m_pMesh.size();
	double sum = 0;	
	m_Center[0] = m_Center[1] = m_Center[2] = 0;
	for ( i = 0 ; i < polygon ; i++ ) {
		//////////////////////////////
		double x_a = m_pVertex[m_pMesh[i]->a]->x, y_a = m_pVertex[m_pMesh[i]->a]->y, z_a = m_pVertex[m_pMesh[i]->a]->z;
		double x_b = m_pVertex[m_pMesh[i]->b]->x, y_b = m_pVertex[m_pMesh[i]->b]->y, z_b = m_pVertex[m_pMesh[i]->b]->z;	
		double x_c = m_pVertex[m_pMesh[i]->c]->x, y_c = m_pVertex[m_pMesh[i]->c]->y, z_c = m_pVertex[m_pMesh[i]->c]->z;

		m_pMesh[i]->area  = pow((y_b-y_a)*(z_c-z_a)-(y_c-y_a)*(z_b-z_a),2);
		m_pMesh[i]->area += pow((z_b-z_a)*(x_c-x_a)-(z_c-z_a)*(x_b-x_a),2);
		m_pMesh[i]->area += pow((x_b-x_a)*(y_c-y_a)-(x_c-x_a)*(y_b-y_a),2);
		m_pMesh[i]->area  = sqrt(m_pMesh[i]->area)/2;		

		//////////////////////////////

		m_Center[0] += m_pMesh[i]->area*(x_a+x_b+x_c)/3;
		m_Center[1] += m_pMesh[i]->area*(y_a+y_b+y_c)/3;
		m_Center[2] += m_pMesh[i]->area*(z_a+z_b+z_c)/3;
		sum += m_pMesh[i]->area;
 	}	
	for ( i = 0 ; i < 3 ; i++ ) m_Center[i] /= sum;

	for ( i = 0 ; i < m_pVertex.size() ; i++ ) {
		m_pVertex[i]->x -= m_Center[0];
		m_pVertex[i]->y -= m_Center[1];
		m_pVertex[i]->z -= m_Center[2];
	}
}



////////////////////////////////////////////////////////////////////////////////
//from Update.cpp
/////////////////////////////////////////////////////////////////////////////
void MakeVoxel(CModel *pModel, CSplit *pSplit, int nVoxel, BOOL bVoxel)
{	
	CVertex max, min, width;
	max.x = pModel->m_Max[0]; max.y = pModel->m_Max[1]; max.z = pModel->m_Max[2];
	min.x = pModel->m_Min[0]; min.y = pModel->m_Min[1]; min.z = pModel->m_Min[2];
	width = max - min;

	BOOL boolean = FALSE;
	if ( !bVoxel ) {
		boolean = pSplit->m_Voxel.GetVoxelSizeDynamic(pModel->m_pVertex, pModel->m_pMesh, max, min, width, nVoxel);
	} else {
		boolean = pSplit->m_Voxel.GetVoxelSizeStatic(width, nVoxel);
		pSplit->m_Voxel.MakeVoxel(pModel->m_pVertex, pModel->m_pMesh, max, min);
	}
	if ( boolean ) {		
		pSplit->m_nDivX = pSplit->m_Voxel.GetXSize();
		pSplit->m_nDivY = pSplit->m_Voxel.GetYSize();
		pSplit->m_nDivZ = pSplit->m_Voxel.GetZSize();
	}
}

void MakeGraph(CSplit * pSplit, P3DS *pARG)
{
	int i, j;

	int XSize = pSplit->m_nDivX; int YSize = pSplit->m_nDivY; int ZSize = pSplit->m_nDivZ;

	int size_max = XSize > YSize ? XSize : YSize;
	size_max = size_max > ZSize ? size_max : ZSize;
	float base = (float) 1/size_max;

	int voxel = pSplit->m_Voxel.GetNumber();
	
	pARG->m_fx = (float) pSplit->m_nDivX/size_max;
	pARG->m_fy = (float) pSplit->m_nDivY/size_max;
	pARG->m_fz = (float) pSplit->m_nDivZ/size_max;
	
	pARG->m_fv = (float) pSplit->m_Voxel.GetNumber()/(pSplit->m_nDivX*pSplit->m_nDivY*pSplit->m_nDivZ);	
	
	for ( i = 0 ; i < pSplit->m_pPart.size() ; i++ ) {		
		CNode * pNode = new CNode; 
    pARG->pNodeArray.push_back(pNode);//pARG->pNodeArray.Add(pNode);
	}
	for ( i = 0 ; i < pSplit->m_pPart.size() ; i++ ) {
		CPart * pPart = pSplit->m_pPart[i];
		CNode * pNode = pARG->pNodeArray[i];

		pNode->idx = pPart->index;

		pNode->vol = (float) pPart->num/voxel;
		pNode->x = (float) pPart->mean.x/pPart->num; pNode->x *= base;
		pNode->y = (float) pPart->mean.y/pPart->num; pNode->y *= base;
		pNode->z = (float) pPart->mean.z/pPart->num; pNode->z *= base;

		pNode->x += (float) 0.5 - pARG->m_fx/2; /// 2003/11/19
		pNode->y += (float) 0.5 - pARG->m_fy/2; /// 2003/11/19
		pNode->z += (float) 0.5 - pARG->m_fz/2; /// 2003/11/19
		
		for ( j = 0 ; j < 9 ; j++ )	pNode->mat[j] = pPart->mat[j];

		pNode->var_x = (float) pPart->length.x * base;
		pNode->var_y = (float) pPart->length.y * base;
		pNode->var_z = (float) pPart->length.z * base;

		float vol = ((float) pPart->num/(pSplit->m_nDivX*pSplit->m_nDivY*pSplit->m_nDivZ));
		float ratio = (float) pow(vol/((3*PI/4)*pNode->var_x*pNode->var_y*pNode->var_z),(double) 1/3);		

		pNode->convex = (float) pPart->convexity;

		int nPart = pPart->pPart.size();
		if ( nPart ) {
			for ( j = 0 ; j < nPart ; j++ )	
        //pNode->pNode.Add(pARG->pNodeArray[pPart->pPart[j]->index-1]);
        pNode->pNode.push_back(pARG->pNodeArray[pPart->pPart[j]->index-1]);
		}
	}

	int num_of_part = pSplit->m_Part.root + pSplit->m_Part.leaf;
	if ( num_of_part != pSplit->m_pPart.size() ) {
		for ( i = 0 ; i < pSplit->m_pPart.size() ; i++ ) {
			CPart * pPart = pSplit->m_pPart[i];
			for ( j = 0 ; j < pPart->pDot.size() ; j++ ) {
				CDot * pDot = pPart->pDot[j];
				pSplit->m_Part.m_pData[pDot->x][pDot->y][pDot->z] = pPart->index;
			}
		}
		pSplit->m_Part.root = pSplit->m_pPart.size();
		pSplit->m_Part.leaf = 0;
	}
}




////////////////////////////////////////////////////////////////////////////////
//from .cpp
/////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////
//from .cpp
/////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////
//from .cpp
/////////////////////////////////////////////////////////////////////////////
