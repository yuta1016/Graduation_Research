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
// Applicable File Name:  Perceptual3DShape_Aux.h

#ifndef __PERCEPTUAL3DSHAPE_AUX_H__
#define __PERCEPTUAL3DSHAPE_AUX_H__

#pragma warning(disable : 4786)

typedef const char* LPCTSTR;
#if !defined(BOOL)
typedef int BOOL;
#endif //

//#define ONLY_FOR_MATCHING

//////////////////////////////////////////////////////////////////////////////////////
//from P3DS.h
//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CNode

class CNode;
//typedef CTypedPtrArray<CObArray, CNode*> CNodeArray;
typedef std::vector<CNode*> CNodeArray;

class CNode {
public:
	int idx;
	
	float vol;
	float x, y, z;
	float mat[9];
	float var_x, var_y, var_z;//max_1, max_2, max_3 in standard.
	float convex;
	
	CNodeArray pNode;

    CNode();
	~CNode() {};

//zhanghui noted:
//these codes are not used, so remove them.
/*CNode(const CNode& node);
	const CNode& operator =(const CNode& node);
	bool operator ==(const CNode& node) const;
	bool operator !=(const CNode& node) const {return !(*this == node);}*/
};

/////////////////////////////////////////////////////////////////////////////
// P3DS

class CGraph;

class P3DS {
public:
	float m_fx, m_fy, m_fz;
	float m_fv;
	int m_nBody;

	CNodeArray pNodeArray;	

public:	
    P3DS();
    P3DS(const P3DS& p3ds);
	const P3DS& operator =(const P3DS& p3ds);
	~P3DS();

	bool operator ==(const P3DS& p3ds) const;
	bool operator !=(const P3DS& p3ds) const {
		return !(*this == p3ds);
	}
	
	void Clear();

	void InitCGraph();

	BOOL Load(LPCTSTR lpszPathName);
	void Save(LPCTSTR lpszPathName);

  void Save_quantize(LPCTSTR lpszPathName, int nBits);
  void Quant(int nBits);//to change the internal members by quant and re-quant.

	void AttributeCreate(CGraph * pGraph);	
	void AttributeUnary(CGraph * pGraph);
	void AttributeBinary(CGraph * pGraph);
};

//////////////////////////////////////////////////////////////////////////////////////
//from Node.h
//////////////////////////////////////////////////////////////////////////////////////

//zhanghui noted: changed back to this correct number.
#define UNARY_FEATURE 4
#define BINARY_FEATURE 3

/////////////////////////////////////////////////////////////////////////////
// CUnary

class CUnary {
public:
	static int nFeature;
	static int nStart;
	static double pMin[UNARY_FEATURE];
	static double pWeight[UNARY_FEATURE];
public:
	double f[UNARY_FEATURE];
	
    CUnary();
    CUnary(const CUnary& unary);
	const CUnary& operator =(const CUnary& unary);
	~CUnary() {};

	bool operator ==(const CUnary& unary) const;
	bool operator !=(const CUnary& unary) const {
		return !(*this == unary);
	}

	void DiffNorm2(CUnary &unary, double &diff);
	void DiffNorm2Zero(double &diff);
	void DiffNorm2Min(double &diff);	
};

/////////////////////////////////////////////////////////////////////////////
// CBinary

class CBinary {
public:
	static int nFeature;
	static double pMin[BINARY_FEATURE];
	static double pWeight[BINARY_FEATURE];
public:
	double f[BINARY_FEATURE];
	
    CBinary();
    CBinary(const CBinary& binary);
	const CBinary& operator =(const CBinary& binary);
	~CBinary() {};

	bool operator ==(const CBinary& binary) const;
	bool operator !=(const CBinary& binary) const {
		return !(*this == binary);
	}

	void DiffNorm2(CBinary &binary, double &diff);
	void DiffNorm2Zero(double &diff);
	void DiffNorm2Min(double &diff);
};


//////////////////////////////////////////////////////////////////////////////////////
//from Graph_Def.h
//////////////////////////////////////////////////////////////////////////////////////
#define _NODE_MAX 256

//#include "Node.h"

typedef struct info_dst_d1
{
    float *x;
    float *y;
    float *mat;
    int rows;
    int cols;
} info_dst_d1;

#ifndef _INTEGER_ARRAY
#define _INTEGER_ARRAY
//typedef CArray<int,int> CInteger;
//typedef CTypedPtrArray<CObArray, CInteger*> CIntegerArray;
typedef std::vector<int> CInteger;
typedef std::vector<CInteger*> CIntegerArray;
#endif

class CGraph {
public:
	CIntegerArray pIndexArray;

	CUnary * pUnary; CBinary ** pBinary;

//zhanghui noted: no need define them.
//	char m_strName[255];
//	char m_strPath[255];

	double diff;	

	// for nEMD
	info_dst_d1 mat_d1;	

	static float wEarth[_NODE_MAX]; static float uEarth[_NODE_MAX];
	static float InnerDist[_NODE_MAX*_NODE_MAX];
	static float OuterDist[_NODE_MAX*_NODE_MAX];

	//constructor from standard data members.
	CGraph(Perceptual3DShapeDescriptorInterfaceABC *);

public:
    CGraph();
	~CGraph();

//zhanghui noted:
//these codes are not used, so remove them.
/*
  CGraph(const CGraph& graph);
	const CGraph& operator =(const CGraph& graph);
	bool operator ==(const CGraph& graph) const;
	bool operator !=(const CGraph& graph) const {
		return !(*this == graph);
	}*/
	
	void Clear();

	void AttributeFree();

	double MatchSingleGraph(CGraph *pGraph);
	
	double MatchFastEMDApart(CGraph *pModel, float *cost, float *flow, double penalty, double weight);	
	void   MatchFastEMDApartInnerNextImagine(CGraph *pModel, double penalty);	
	void   CostFastEMDApart(double & similarity, CGraph *pModel, float *flow, double & penalty, double & weight);
};


//////////////////////////////////////////////////////////////////////////////////////
//from Polygon.h
//////////////////////////////////////////////////////////////////////////////////////

#define PI 3.141592654
#define INF 1e10

class CVertex;
class CMesh;

//typedef CTypedPtrArray<CObArray, CVertex*> CVertexArray;
//typedef CTypedPtrArray<CObArray, CMesh*> CMeshArray;
typedef std::vector<CVertex*> CVertexArray;
typedef std::vector<CMesh*> CMeshArray;

class CVertex{
public:
	double x, y, z;
	int nIndex; int nCheck;

	CMeshArray pFace;
	
	CVertex() {
		x = 0; y = 0; z = 0;
		nIndex = 0; nCheck = 0;
	}
	CVertex(double a, double b, double c) {
		x = a; y = b; z = c;
		nIndex = 0; nCheck = 0;
	}
	CVertex(const CVertex& vertex) {
		x = vertex.x; y = vertex.y; z = vertex.z;
		nIndex = vertex.nIndex; nCheck = vertex.nCheck;
	}

	const CVertex& operator =(const CVertex& vertex) {
		x = vertex.x; y = vertex.y; z = vertex.z;
		nIndex = vertex.nIndex; nCheck = vertex.nCheck;
		return *this;
	}

	BOOL operator ==(const CVertex& vertex) const {
		if ( (x == vertex.x) && (y == vertex.y) && (z == vertex.z) )
			return TRUE;
		else
			return FALSE;
	}

	BOOL operator !=(const CVertex& vertex) const {
		return !(*this == vertex);
	}	

	CVertex operator +(const CVertex& vertex) const {
		CVertex temp;
		temp.x = x + vertex.x;
		temp.y = y + vertex.y;
		temp.z = z + vertex.z;
		return temp;
	}

	CVertex operator -(const CVertex& vertex) const {
		CVertex temp;
		temp.x = x - vertex.x;
		temp.y = y - vertex.y;
		temp.z = z - vertex.z;
		return temp;
	}

	double operator *(const CVertex& vertex) {
		return x * vertex.x + y * vertex.y + z * vertex.z;		
	}
	
	double Distance();
};

class CMesh {
public:
	int a, b, c;
	int nIndex; int nCheck;
	double area;
	CVertex normal;

	CMesh() {
		a = b = c = 0;
		nIndex = 0; nCheck = 0;
		area = 0;
	};
	CMesh(int x, int y, int z) {
		a = x; b = y; c = z;
		nIndex = 0; nCheck = 0;
		area = 0;
	}
	CMesh(const CMesh& mesh) {
		a = mesh.a; b = mesh.b; c = mesh.c;
		nIndex = mesh.nIndex; nCheck = mesh.nCheck;
		area = mesh.area;
		normal = mesh.normal;
	}	

	const CMesh& operator =(const CMesh& mesh) {
		a = mesh.a; b = mesh.b; c = mesh.c;
		nIndex = mesh.nIndex; nCheck = mesh.nCheck;
		area = mesh.area;
		normal = mesh.normal;
		return *this;
	}

	BOOL operator ==(const CMesh& mesh) const {
		if ( (a == mesh.a) && (b == mesh.b) && (c == mesh.c) ||
            (a == mesh.a) && (c == mesh.b) && (b == mesh.c) ||
            (b == mesh.a) && (c == mesh.b) && (a == mesh.c) ||
            (b == mesh.a) && (a == mesh.b) && (c == mesh.c) ||
            (c == mesh.a) && (a == mesh.b) && (b == mesh.c) ||
            (c == mesh.a) && (b == mesh.b) && (a == mesh.c) )
			return TRUE;
		else
			return FALSE;
	}

	BOOL operator !=(const CMesh& mesh) const {
		return !(*this == mesh);
	}
};

//////////////////////////////////////////////////////////////////////////////////////
//from Dot.h
//////////////////////////////////////////////////////////////////////////////////////
#define PI 3.141592654
#define INF 1e10

//#include "../Model/Polygon.h"

class CDot {
public:
	int x, y, z;
	int temp;

	CDot() {
		x = y = z = 0;        
		temp = 0;
	};
	CDot(int a, int b, int c) {
		x = a; y = b; z = c;
		temp = 0;
	}
	CDot(const CDot& dot) {
		x = dot.x; y = dot.y; z = dot.z;
		temp = dot.temp;
	}

	const CDot& operator =(const CDot& dot) {
		x = dot.x; y = dot.y; z = dot.z;
		temp = dot.temp;
		return *this;
	}

	bool operator ==(const CDot& dot) const {
		if ( (x == dot.x) && (y == dot.y) && (z == dot.z) )
			return true;
		else
			return false;
	}

	bool operator !=(const CDot& dot) const {
		return !(*this == dot);
	}

    double Distance();
};

//typedef CTypedPtrArray<CObArray, CDot*> CDotArray;
typedef std::vector<CDot*> CDotArray;

//typedef CTypedPtrArray<CObArray, CDotArray*> CSetArray;
typedef std::vector<CDotArray*> CSetArray;

class CVoxel
{
public:
	int root;
	int link;
	int leaf;
	
	double convexity;

//private:
	int *** m_pData;
	
	int m_nXSize;
	int m_nYSize;
	int m_nZSize;

public:
	BOOL GetVoxelSizeDynamic(CVertexArray & pVertex, CMeshArray & pMesh, CVertex & max, CVertex & min, CVertex & width, int nVoxel);
	BOOL GetVoxelSizeStatic(CVertex & width, int nVoxel);
	BOOL ComputeVoxelSize(CVertex & width, int size);
	int MakeVoxel(CVertexArray & pVertex, CMeshArray & pMesh, CVertex & max, CVertex & min);
	void GetVoxelLine(CVertex & start, CVertex & end, CVertex & max, CVertex & min);
	void GetVoxelLine(CVertex & Left, CVertex & Mid, CVertex & Right, CVertex & max, CVertex & min);
	void GetVoxel(CVertex & A, CVertex & B, CVertex & C, CVertex & max, CVertex & min);
	int Simply();
	void Filling();
	int MakeHull();
	int Hull();
	int Hull(CDotArray & pArray);
	int Component();
	int Boundary();
	void Free();
	CVoxel();
	CVoxel(int XSize, int YSize, int ZSize);
	CVoxel(CVoxel & Source);
	virtual ~CVoxel();

	void Allocate();
	void Allocate(int XSize, int YSize, int ZSize);

	int GetXSize() const;
	int GetYSize() const;
	int GetZSize() const;

	int FastErode(CVoxel & SE);
	int FastDilate(CVoxel & SE);
	
	int Erode(CVoxel & SE);	
	int GetNumber();
};

//typedef CTypedPtrArray<CObArray, CVoxel*> CVoxelArray;
typedef std::vector<CVoxel*> CVoxelArray;


//////////////////////////////////////////////////////////////////////////////////////
//from Part.h
//////////////////////////////////////////////////////////////////////////////////////
//typedef CTypedPtrArray<CObArray, CPart*> CPartArray;
class CPart;
typedef std::vector<CPart*> CPartArray;

//#include "Dot.h"

class CPart {
public:
	CDotArray pDot;

	int index;

	int num;
	CDot mean;
	float mat[9];
	CVertex length;
	double convexity;
	
	int form;
	bool modify;

	CPartArray pPart;

    CPart() {index = -1; form = -1; num = 0; modify = true;}
    CPart(const CPart& part);
	const CPart& operator =(const CPart& part);
	~CPart() {};

	bool operator ==(const CPart& part) const;
	bool operator !=(const CPart& part) const {
		return !(*this == part);
	}
};

typedef struct info_mrg
{
    CPart * p0;
    CPart * p1;
    double msr;
} info_mrg;

int compare_mrg( const void *arg1, const void *arg2 );


//////////////////////////////////////////////////////////////////////////////////////
//from Split.h
//////////////////////////////////////////////////////////////////////////////////////
#define RANK 20

class CSplit 
{
// Attributes
public:
    int m_nDivX;
	int m_nDivY;
	int m_nDivZ;

	CPartArray m_pArray[RANK];
	CPartArray m_pPart;
	
	int m_nDepth;

	CVoxel m_Voxel;
	CVoxel m_Part;

	static CVoxel SE[RANK];

// Implementation
public:
	CSplit();

//zhanghui noted:
//these codes are not used, so remove them.
/*	BOOL LoadBinary(LPCTSTR lpszPathName);
	BOOL LoadGray(LPCTSTR lpszPathName);
	void SaveBinary(LPCTSTR lpszPathName);
	void SaveGray(LPCTSTR lpszPathName);*/

	void ModelAssay(CPart * pPart);

	void PartSplit();
	void PartMerge();

	void Clear();

	static void MakeSE();
	void MergePart();
	void EarlyProcess(CVoxel &body, CVoxel &branch, int iteration);
	void Residual(CVoxel &body, CVoxel &branch, CVoxel &source);
	bool VerifyBranch(CVoxel &source);
	bool TrimPart(CVoxel &source);
	bool VerifyPart(CVoxel &part);
	void Flat(CVoxel &source);
	bool Partition(CVoxel &source, int size);
	bool SplitBranch(CVoxel &source);
	bool SplitBody(CVoxel &source);
	bool Selection(CVoxel &source);
	bool Decompose();
	void MakeGraph();
	void Initialize();
	virtual ~CSplit();
};


//////////////////////////////////////////////////////////////////////////////////////
//from Model.h
//////////////////////////////////////////////////////////////////////////////////////
class CModel{
public:
	CVertexArray m_pVertex;
	CMeshArray m_pMesh;

	BOOL m_bOpen;

	int m_nPoint;
	int m_nPolygon;

	double m_Center[3];
	double m_Max[3], m_Min[3];
	double m_dDiagonal;

public:		
	CModel();
	~CModel();
	void Clear();
	BOOL Open(MediaThreeD* media3d);//(LPCTSTR lpszPathName);
	BOOL Save(LPCTSTR lpszPathName);

	BOOL ReadWrl(MediaThreeD* media3d);//(LPCTSTR lpszPathName);	

	void MakeNormal();
	void Adjust();
	void Align();
	void Translate();
};


//////////////////////////////////////////////////////////////////////////////////////
//from .h
//////////////////////////////////////////////////////////////////////////////////////
void MakeVoxel(CModel *pModel, CSplit *pSplit, int nVoxel, BOOL bVoxel);
void MakeGraph(CSplit * pSplit, P3DS *pARG);

#endif //__PERCEPTUAL3DSHAPE_AUX_H__
