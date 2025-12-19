///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Siemens AG, J. Heuer, F. Sanahuja
// (contributing organizations names)
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
// Applicable File Name:  RegionLocatorExtraction.h
//
#ifndef __REGION_LOCATOR_EXTRACTION_H__
#define __REGION_LOCATOR_EXTRACTION_H__


#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include <momusys.h>
#include <intra.h>
#include "momusys.h"
#include "Utilities/readparam.h"
#include "Descriptors/Descriptors.h"

namespace XM
{

// definitions //
#ifndef PI
#define PI		(3.14159265358979)
#endif


// MACROS //
#ifndef PIX_xy
#define PIX_xy(image, a, b)( *((image->data->u) + ( (b) * (image->x) ) + (a)) )
#endif

#ifndef ABS
#define  ABS(x)                (((x) < 0) ? -(x) : (x))
#endif

#ifndef SIGN
#define  SIGN(x)               (((x) < 0) ? -1 : 1)
#endif

  // Declarations //
vertices * LocatorToVertices(struct loc * locator);
struct loc *VerticesToLocator(vertices *v);
char *ecalloc(int n, unsigned int s);

//////////////////////////////////////
// Structures used by "C" functions //
//////////////////////////////////////

#ifndef MAX_VERTICES
#define MAX_VERTICES	10000
#endif


struct t{
    Int		whoami;		/* absolut index of the vertice */
    Int		x;		/* x position */
    Int		y;		/* y position */
#ifdef USEXML3
    float	area;		/* area contained on this triangle */
    float	distPrev;	/* length of the previous segment */
    float	distNext;	/* length of the next segment */
#else
    Float	area;		/* area contained on this triangle */
    Float	distPrev;	/* length of the previous segment */
    Float	distNext;	/* length of the next segment */
#endif // USEXML3
    Int		deleted;	/* says if the vertex has been already deleted */
    struct t	*prev;		/* previous triangle structure */
    struct t	*next;		/* next triangle structure */
};
typedef struct t Triangle;

struct c{
#ifdef USEXML3
    float	cost;
#else
    Float	cost;
#endif // USEXML3
    struct c    *prev;
    struct c	*next;
    Triangle	*who;
};
typedef struct c CostItem;

// end "C" structures

// global conversion from Vstruct vertices to struct locator


// Forward Declarations:
class RegionLocatorExtractionTool;

//=============================================================================
class RegionLocatorExtractionInterfaceABC: public I_InterfaceABC
{
public:
	RegionLocatorExtractionInterfaceABC();
	virtual ~RegionLocatorExtractionInterfaceABC() {};

	virtual void destroy()=0;
	
	virtual int SetSourceMedia(MomVop *media) = 0;
	virtual RegionLocatorDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;	
	virtual int 
	  SetDescriptorInterface(RegionLocatorDescriptorInterfaceABC \
				 *aRegionLocatorDescriptorInterface) = 0;
        virtual int InitExtracting(void) = 0;
        virtual unsigned long StartExtracting(void) = 0;


        virtual int dumpList(const char *fileName, int BoxPoly, polyline BoxData, polyline PolyData) = 0;
        virtual int drawLine(MomImage *&outImage, unsigned long int *p1, \
                             unsigned long int *p2, int val) = 0; 
        virtual int drawLine(MomImage *&outImage, short *p1, \
                             short *p2, int val) = 0;
        virtual int SetImage(MomImage *Image,polyline PolyData) = 0;
        virtual int writePnmImage(const char * name, MomImage *image) = 0;




        virtual int getImageFromVertices(vertices *v, MomImage *im) = 0;
        virtual float ComputeAreaError(MomImage *im, MomImage *im2) = 0;
        virtual int printResults(char *fileName, char *imageName, int DLength, float AreaError) = 0;        

	static const UUID myID;
};

class RegionLocatorExtractionTool;

//=============================================================================
class RegionLocatorExtractionInterface: 
  public RegionLocatorExtractionInterfaceABC
{
public:
	RegionLocatorExtractionInterface(RegionLocatorExtractionTool* aTool);
	virtual ~RegionLocatorExtractionInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetSourceMedia(MomVop *media);
	virtual RegionLocatorDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int
	  SetDescriptorInterface(RegionLocatorDescriptorInterfaceABC
					  *aRegionLocatorDescriptorInterface);
        virtual int InitExtracting(void);
	virtual unsigned long StartExtracting(void);

        virtual int dumpList(const char *fileName, int BoxPoly, polyline BoxData, polyline PolyData);
        virtual int drawLine(MomImage *&outImage, unsigned long int *p1, \
                             unsigned long int *p2, int val);
        virtual int drawLine(MomImage *&outImage, short *p1, \
                             short *p2, int val);
        virtual int writePnmImage(const char * name, MomImage *image);
        

        virtual int SetImage(MomImage *Image,polyline PolyData);

        virtual int getImageFromVertices(vertices *v, MomImage *im);
        virtual float ComputeAreaError(MomImage *im, MomImage *im2);
        virtual int printResults(char *fileName, char *imageName, int DLength, float AreaError);

	static const UUID myID;
	static const char *myName;

private:
	RegionLocatorExtractionTool *m_ExtractionTool;
};

//=============================================================================
class RegionLocatorExtractionTool: public DescriptorExtractor
{
friend class RegionLocatorExtractionInterface; 
public:
        RegionLocatorExtractionTool();
        RegionLocatorExtractionTool(RegionLocatorDescriptorInterfaceABC *aDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(MomVop *media);
	virtual RegionLocatorDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(RegionLocatorDescriptorInterfaceABC
				   * aRegionLocatorDescriptorInterface);
	
        virtual int InitExtracting(void);	// reads parameters
	virtual unsigned long StartExtracting(void);


        virtual int dumpList(const char *fileName, int BoxPoly, polyline BoxData, polyline PolyData);
        virtual int drawLine(MomImage *&outImage, unsigned long int *p1, \
                             unsigned long int *p2, int val);
        virtual int drawLine(MomImage *&outImage, short *p1, \
                             short *p2, int val);
        virtual int writePnmImage(const char * name, MomImage *image);


        virtual int SetImage(MomImage *Image,polyline PolyData);


        virtual int getImageFromVertices(vertices *v, MomImage *im);
        virtual float ComputeAreaError(MomImage *im, MomImage *im2);
        virtual int printResults(char *fileName, char *imageName, int DLength, float AreaError);


	virtual RegionLocatorExtractionInterfaceABC *GetInterface(void);


	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:

	virtual ~RegionLocatorExtractionTool(); 


        void binarise(MomImage *inImage, MomImage *outImage,\
                 int TH, int outL, int outH);
        void VerticesToImagePerimeterColor(vertices *imVertices, \
                                      MomImage *outImage);
        segPos	checkNextSegment(int startPoint, vertices *imVertices, \
                                 segPos actual);
        void fillPerimeterColor( MomImage *outImage, \
                                 unsigned int value);

        // private methods from "C" functions

        virtual float SetEvolutionCost(float cost);
        virtual float GetEvolutionCost(void);
        virtual int SetEndVertices(int EndVertices);
        virtual int GetEndVertices(void);
       
        void printVertices(char *fileName, vertices *imVertices);
        int getRangeExt(int x);
        double vectProd2D(int *v1, int *v2);
        int  getContour(MomImage *inShape, MomImage **outImage, \
                        unsigned int outVal, int **positions);
        int getImageArea(MomImage *inShape);
        int	drawLine(MomImage *outImage, Short *p1, Short *p2, unsigned int val);
        void	VerticesToImagePerimeter( vertices *imVertices, \
                                          MomImage *outImage, int value);
        void getVerticesFromImage( MomImage *inShape, vertices *imVertices);
        float	getAngle(Short *VL1, Short *VL2);
        Triangle ** initTriangles(vertices *imVertices);
        void	FreeTriangles(Triangle **t, int nVert);
        void FreeCostItems(CostItem *startCost);
        void printCostItems(CostItem *startCost);
        void 	fillArea(Triangle *actual);
        void fillDist(Triangle *actual);
        void fillTriangles(Triangle **listTriangles);
        CostItem * sortAreas(Triangle **listTriangles, int nVert);
        void 	modifyCost(CostItem *startCost, Triangle *actual);
        int	canBeRemoved(Triangle *toBeRemoved);
        CostItem * deleteLowest( CostItem * startCost);
        void	TrianglesToVertices(Triangle *first, vertices **imVertices);
        void classicEvolution(vertices *imVertices, vertices **outVertices, \
                              float costTH, int vertTH);
        
        // ---------------- //
	RegionLocatorExtractionInterface m_Interface;
	RegionLocatorDescriptorInterfaceABC *m_DescriptorInterface;
	MomVop *m_Media;

        // actual values
        float m_EvolutionCost;
        int   m_EndVertices;
        int   m_BoxPoly;
        char  *m_ImageFileName;
	
};

};

#endif //__REGION_LOCATOR_EXTRACTION_H__
