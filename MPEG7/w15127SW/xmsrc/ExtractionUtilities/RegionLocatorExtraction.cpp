//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Siemens AG, J. Heuer, F. Sanahuja
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
// Applicable File Name:  RegionLocatorExtraction.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <inter.h>
#include <intra.h>
#include <fifo.h>
#include <math.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "RegionLocatorExtraction.h"

#ifdef USEXML3
#define Float float
#define Double double
#endif // USEXML3

using namespace std;
using namespace XM;

const UUID RegionLocatorExtractionInterface::myID = UUID("");
const char *RegionLocatorExtractionInterface::myName = "Region Locator Extractor Interface";

const UUID RegionLocatorExtractionTool::myID = UUID("");
const char *RegionLocatorExtractionTool::myName = "Region Locator Extractor";

const UUID RegionLocatorExtractionInterfaceABC::myID = UUID();

//=============================================================================
RegionLocatorExtractionInterfaceABC::RegionLocatorExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
RegionLocatorExtractionInterface::RegionLocatorExtractionInterface(RegionLocatorExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
RegionLocatorExtractionInterface::~RegionLocatorExtractionInterface()
{
}

//----------------------------------------------------------------------------
int RegionLocatorExtractionInterface::SetSourceMedia(MomVop *media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
RegionLocatorDescriptorInterfaceABC* RegionLocatorExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int RegionLocatorExtractionInterface::
SetDescriptorInterface(RegionLocatorDescriptorInterfaceABC
		       *aRegionLocatorDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(aRegionLocatorDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorExtractionInterface::StartExtracting(void){
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
int RegionLocatorExtractionInterface::InitExtracting(void){
  return m_ExtractionTool->InitExtracting();
}


//----------------------------------------------------------------------------
int RegionLocatorExtractionInterface::dumpList(const char *fileName, \
                                               int BoxPoly, polyline BoxData,\
					       polyline PolyData){
    return m_ExtractionTool->dumpList(fileName, BoxPoly, BoxData, PolyData);
}
//----------------------------------------------------------------------------
int RegionLocatorExtractionInterface::drawLine(MomImage *&outImage, \
                                               unsigned long int *p1, \
                                               unsigned long int *p2, int val){
    return m_ExtractionTool->drawLine(outImage, p1, p2, val);
}
//----------------------------------------------------------------------------
int RegionLocatorExtractionInterface::drawLine(MomImage *&outImage, \
                                               short *p1, \
                                               short *p2, int val){
    return m_ExtractionTool->drawLine(outImage, p1, p2, val);
}
//----------------------------------------------------------------------------
int RegionLocatorExtractionInterface::SetImage(MomImage *Image,polyline PolyData){
    return m_ExtractionTool->SetImage(Image,PolyData);
}

//----------------------------------------------------------
int RegionLocatorExtractionInterface::writePnmImage(const char *name, \
                                                    MomImage *image){
    
    return m_ExtractionTool->writePnmImage(name, image);
}

//----------------------------------------------------------------------------
int RegionLocatorExtractionInterface::getImageFromVertices(vertices *v, \
                                                           MomImage *im){
    return m_ExtractionTool->getImageFromVertices(v, im);
}
//----------------------------------------------------------------------------
float RegionLocatorExtractionInterface::ComputeAreaError(MomImage *im, \
                                                         MomImage *im2 = NULL){
    return m_ExtractionTool->ComputeAreaError(im, im2);
}
//----------------------------------------------------------------------------
int  RegionLocatorExtractionInterface::printResults(char *fileName, \
                                                    char *imageName,\
						    int DLength,\
						    float AreaError){
    return m_ExtractionTool->printResults(fileName, imageName,DLength, AreaError);
}


//----------------------------------------------------------------------------
const UUID& RegionLocatorExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RegionLocatorExtractionInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void RegionLocatorExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }


//=============================================================================
RegionLocatorExtractionTool::RegionLocatorExtractionTool():
m_Interface(this),
m_DescriptorInterface(0),
m_Media(0)

{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
RegionLocatorExtractionTool::
RegionLocatorExtractionTool(RegionLocatorDescriptorInterfaceABC
			     *regionLocator):
m_Interface(this),
m_DescriptorInterface(0),
m_Media(0)

{
	/* create descriptor if it does not exist*/
	if (!regionLocator) {
	  RegionLocatorDescriptor *descriptor =
	    new RegionLocatorDescriptor();
	  regionLocator=descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetDescriptorInterface(regionLocator);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
RegionLocatorExtractionTool::~RegionLocatorExtractionTool()
{
	/* release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release(); 
}

//----------------------------------------------------------------------------
bool RegionLocatorExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool RegionLocatorExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int RegionLocatorExtractionTool::SetSourceMedia(MomVop *media)
{
  if (!media) return -1;

  m_Media=media;
  return 0;
}

//----------------------------------------------------------------------------
RegionLocatorDescriptorInterfaceABC* RegionLocatorExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int RegionLocatorExtractionTool::
SetDescriptorInterface(RegionLocatorDescriptorInterfaceABC
		       *aRegionLocatorDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == 
	    aRegionLocatorDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_DescriptorInterface) 
	  m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = 
	  aRegionLocatorDescriptorInterface;
	if (m_DescriptorInterface) {
	  m_DescriptorInterface->addref();

	  /* conect sub descriptors with sub coding schemes*/
	  /* not available*/
	}
	else {
	  /* if no descriptor, release also descriptor interfaces from
	     sub extraction tools schemes*/
	  /* not available*/
	  return -1;
	}
	
	
	return 0; 
}

//---------------------------------------------------------------------------
int RegionLocatorExtractionTool::InitExtracting(void){

    char 	*paramValue;
    
   /* read parameters from param file */
        
    paramValue = getpara("EvolutionCost", "0.3");
    sscanf(paramValue, "%f", &m_EvolutionCost);
    
    paramValue = getpara("EndVertices", "3");
    sscanf(paramValue, "%d", &m_EndVertices);
    
    paramValue = getpara("UseBoxPoly", "2");
    sscanf(paramValue, "%d", &m_BoxPoly);
           
    m_DescriptorInterface->ResetDescriptor();

    return 0;
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorExtractionTool::StartExtracting(void)
{
  
    vertices *imVertices, *newVertices;
    struct loc 	*locator;
    int    i;
    MomImage	*inShape;
    Short 	**vl;

        /* check for descriptor*/
    if (!m_DescriptorInterface) return (unsigned long)-1;
    
    if (strcmp(m_DescriptorInterface->GetName(), \
               "Region Locator Descriptor Interface") != 0)
        return (unsigned long)-1;

    // extracs the required parameters
    InitExtracting();
    
    /* check for media*/
    if (!m_Media) return (unsigned long)-1;
    inShape = m_Media->y_chan;

    if(m_BoxPoly & 1){
        // generate the bounding box
        imVertices = (vertices *) ecalloc(1, sizeof(vertices));
        
        //extract the contour of the MomVop m_Media
	//sth shape should be in a_chan.JH: masks read are also stored as image. May change in future if different formats are needed.
        getVerticesFromImage(m_Media->y_chan, imVertices);
        imVertices->oriImage = inShape;

        m_DescriptorInterface->SetRepresentation( getRangeExt(inShape->x), \
                                                  getRangeExt(inShape->y));
    
        unsigned long maxX = 0;
        unsigned long maxY = 0;
        unsigned long minX = inShape->x;
        unsigned long minY = inShape->y;
        
        for(int i = 0; i < imVertices->nVertices; i++){
            if(imVertices->VL[i][0] > (int) maxX) maxX = imVertices->VL[i][0];
            if(imVertices->VL[i][1] > (int) maxY) maxY = imVertices->VL[i][1];
            if(imVertices->VL[i][0] < (int) minX) minX = imVertices->VL[i][0];
            if(imVertices->VL[i][1] < (int) minY) minY = imVertices->VL[i][1];
        }
        
        unsigned int *vert = new unsigned int[2*2];
        vert[0] = minX;		vert[1] = minY;
        vert[2] = maxX;		vert[3] = maxY;

        m_DescriptorInterface->SetLocator( 0, (bool) 0, (int) 2, vert);
        delete [] vert;
        free((vertices *) imVertices);

    }
    if(m_BoxPoly & 2){
        imVertices = (vertices *) ecalloc(1, sizeof(vertices));
        
        //extract the contour of the MomVop m_Media 
        getVerticesFromImage(m_Media->y_chan, imVertices);
//        printVertices("afterReading", imVertices);
        imVertices->oriImage = m_Media->y_chan;
        
        //do simplification
        classicEvolution(imVertices, &newVertices, \
                         m_EvolutionCost, m_EndVertices);
        
        
        //convert struct vertices to struct locator
        locator = (struct loc *) VerticesToLocator(newVertices);
     
        //free vertices structures !!!
        for( i = 0; i < imVertices->nVertices; i++){
            free((Short *) imVertices->VL[i]);
        }
        free((Short **) imVertices->VL);
        vl = newVertices->VL;
        for( i = 0; i < newVertices->nVertices; i++){
            free((Short *) newVertices->VL[i]);
        }
        free((Short **) newVertices->VL);
        
        // and now, before storing the locator
        // set the values maxX maxY to LocatorDescriptor
        
        m_DescriptorInterface->SetRepresentation( getRangeExt(inShape->x), \
                                                  getRangeExt(inShape->y));
        
        //store Locator descriptor
        m_DescriptorInterface->SetLocator(1, (bool) 0, (int) locator->NoVert, \
                                          (unsigned int *)locator->Vertices);
        
    }
    return 0;
}

//---------------------------------------------------------------------------
float RegionLocatorExtractionTool::SetEvolutionCost(float cost){
    return m_EvolutionCost = cost;
}
//----------------------------------------------------------------------------
float RegionLocatorExtractionTool::GetEvolutionCost(void){
    return m_EvolutionCost;
}
//----------------------------------------------------------------------------
int RegionLocatorExtractionTool::SetEndVertices(int EndVertices){
    return m_EndVertices = EndVertices;
}
//----------------------------------------------------------------------------
int RegionLocatorExtractionTool::GetEndVertices(void){
    return m_EndVertices;
}
//----------------------------------------------------------------------------
const UUID& RegionLocatorExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RegionLocatorExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
RegionLocatorExtractionInterfaceABC *RegionLocatorExtractionTool::GetInterface(void)
{ return &m_Interface; }





//----------------------------------------------------------------------------
int RegionLocatorExtractionTool::dumpList(const char *fileName, int BoxPoly, polyline BoxData, polyline PolyData){
    
    ofstream file(fileName);
    
    cout << "Writing file " << fileName << endl;

    if(BoxPoly&1){
        if(BoxData.NoVert == 0){
            cout << "No vertices in structure BoxData" << endl;
            return -1;
        }
        
        file << "#Box representation generated by XM3.0 (" << 
            BoxData.NoVert << " points)" << endl;
        for(unsigned int i = 0; i < BoxData.NoVert - 1; i++){
            file << BoxData.Vertices[2*i] << " " << 
                BoxData.Vertices[2*i + 1] << " " <<
                BoxData.Vertices[2*i + 2] << " " <<
                BoxData.Vertices[2*i + 3] << " " <<
                endl;
        }
    }
    
    if(BoxPoly&2){
        if(PolyData.NoVert == 0){
            cout << "No vertices in structure PolyData" << endl;
            return -1;
        }
    
        file << "#Polygonal representation generated by XM3.0 (" <<
            PolyData.NoVert << " points)" << endl;
        for(unsigned int i = 0; i < PolyData.NoVert - 1; i++){
            file << PolyData.Vertices[2*i] << " " << 
                PolyData.Vertices[2*i + 1] << " " <<
                PolyData.Vertices[2*i + 2] << " " <<
                PolyData.Vertices[2*i + 3] << " " <<
                endl;
        }
    }

    return 0;
}
//----------------------------------------------------------
int RegionLocatorExtractionTool::SetImage(MomImage *Image,polyline PolyData){

    unsigned int i;

    // only gets the perimeter of the image
    if(!Image) return 1;

    Image->version = 0;
    Image->upperodd = 0;
    Image->grid = 's';
    Image->type = UCHAR_TYPE;

    // at the moment use a box which includes the vertices
    unsigned int maxX = 0;
    unsigned int maxY = 0;
    for(i = 0; i < PolyData.NoVert; i++){
        if( maxX < PolyData.Vertices[2*i] )
            maxX = PolyData.Vertices[2*i];
        if( maxY < PolyData.Vertices[2*i + 1] ) 
            maxY = PolyData.Vertices[2*i + 1];
    }
        
    Image->x = maxX + 10;
    Image->y = maxY + 10;
    Image->data = (MomImageData *) calloc(1, sizeof(MomImageData));
    Image->data->u = (unsigned char *) calloc(Image->x * Image->y, \
                                                sizeof(char));

    // displace m_PolyData in order to center the shape
    for(i = 0; i < 2*PolyData.NoVert; i++)
        PolyData.Vertices[i] += 5;
    
    for(i = 1; i < PolyData.NoVert; i++){
        drawLine(Image, &PolyData.Vertices[2*(i-1)], \
                 &PolyData.Vertices[2*i], 1);
    }
    // last line
    drawLine(Image, &PolyData.Vertices[2*(i-1)], \
             &PolyData.Vertices[0], 1);
    return 0;
}
//------------------------------------------------------------

int RegionLocatorExtractionTool::drawLine(MomImage *&outImage, \
                                      short *p1, \
                                      short *p2, \
                                      int val){
    unsigned long int p1l[2], p2l[2];
    p1l[0] = (unsigned long int) p1[0];
    p1l[1] = (unsigned long int) p1[1];
    p2l[0] = (unsigned long int) p2[0];
    p2l[1] = (unsigned long int) p2[1];

    return drawLine( outImage,p1l, p2l, val);
    
}
//------------------------------------------------------------

int RegionLocatorExtractionTool::drawLine(MomImage *&outImage, \
                                      unsigned long int *p1, \
                                      unsigned long int *p2, \
                                      int val){

    Float 	xdiff = (float)p2[0] - (float)p1[0];
    Float 	ydiff = (float)p2[1] - (float)p1[1];
    Int		i, j; 
    Int		sign;

    if(outImage == NULL)
        cerr << "drawLine: Image to be drawn is NULL." << endl;
    if(xdiff == 0 && ydiff == 0) 
    	return 0;
    
    if( xdiff == 0){
        sign = (Int) (ydiff / ABS(ydiff));
        for(j = 0; ABS(j) <= ABS(ydiff); j += sign){
            PIX_xy(outImage, p1[0], p1[1] + j) = (UChar) val;
        }
    }
    else if( ydiff == 0){
        sign = (Int) (xdiff / ABS(xdiff));
        for(i = 0; ABS(i) <= ABS(xdiff); i += sign){
            PIX_xy(outImage, p1[0] + i, p1[1]) = (UChar) val;
        }
    }
    else{ 
        /* choose always minimum slope */
        if( ABS(xdiff) > ABS(ydiff) ){
            sign = (Int) (xdiff / ABS(xdiff));
            for(i = 0; ABS(i) <= ABS(xdiff); i += sign){
                PIX_xy(outImage, p1[0] + i, \
                       (Int) (p1[1] + (ydiff/xdiff)*i + 0.5) ) = (UChar) val;
                PIX_xy(outImage, p2[0] - i, \
                       (Int) (p2[1] + (ydiff/xdiff)*(-i) + 0.5) ) = (UChar) val;

            }
        }
        else{
            sign = (Int) (ydiff / ABS(ydiff));
            for(j = 0; ABS(j) <= ABS(ydiff); j += sign){
                PIX_xy(outImage, (Int) (p1[0] + 0.5 + (xdiff/ydiff)*j),\
                       p1[1] + j ) = (UChar) val;
                PIX_xy(outImage, (Int) (p2[0] + 0.5 + (xdiff/ydiff)*(-j)),\
                       p2[1] - j ) = (UChar) val;
                
            }
        }
    }
    return (int) ((ABS(xdiff)> ABS(ydiff))? ABS(xdiff):ABS(ydiff));
}



//----------------------------------------------------------
int RegionLocatorExtractionTool::writePnmImage(const char *name, MomImage *outImage){

    ofstream	imageFile(name);
    unsigned char	*ptrC, *ptrEndC;
    short int	*ptrI, *ptrEndI;
    float	*ptrF, *ptrEndF;
    unsigned short int	maxVal;

    imageFile << "P5" << endl;
    imageFile << "## Some comments..... " << endl;
    imageFile << outImage->x << " " << outImage->y << endl;
    cout << "Writing image " << name << ": [" << outImage->x << "][" <<
        outImage->y << "]...." << endl;

   
    switch(outImage->type){
    case UCHAR_TYPE:
        ptrC = (unsigned char *) outImage->data->u;
        ptrEndC = ptrC + (outImage->x * outImage->y);
        /* Searching for max value */
        maxVal = 0;
        for( ; ptrC < ptrEndC; ptrC++){
            if ( maxVal < *ptrC )	maxVal = (int) *ptrC;
        }
        imageFile << (int) maxVal << endl;
    
        /* Writing data values */
        ptrC = (unsigned char *) outImage->data->u;
        for( ; ptrC < ptrEndC; ptrC++){
            imageFile << *ptrC;
        }
        break;
    case SHORT_TYPE:
        ptrI = (short int *) outImage->data->s;
        ptrEndI = ptrI + (outImage->x * outImage->y);
        /* Searching for max value */
        maxVal = 0;
        for( ; ptrI < ptrEndI; ptrI++){
            if ( maxVal < *ptrI )	maxVal = (int)*ptrI;
        }
        imageFile << (int) maxVal << endl;
        
        /* Writing data values */
        ptrI = (short int *) outImage->data->s;
        for( ; ptrI < ptrEndI; ptrI++){
            imageFile << (char)*ptrI;
        }
        break;
    case FLOAT_TYPE:
        ptrF = (float *) outImage->data->u;
        ptrEndF = ptrF + (outImage->x * outImage->y);
        /* Searching for max value */
        maxVal = 0;
        for( ; ptrF < ptrEndF; ptrF++){
            if ( maxVal < *ptrF )	maxVal =(int) *ptrF;
        }
        imageFile << (int) maxVal << endl;
        
        /* Writing data values */
        ptrF = (float *) outImage->data->f;
        for( ; ptrF < ptrEndF; ptrF++){
            imageFile << (char)*ptrF;
        }     
        break;
    }
    
    
    return maxVal;
}


//----------------------------------------------------------------------------
void RegionLocatorExtractionTool::fillPerimeterColor( MomImage *outImage, \
                                                  unsigned int outValue){
    
    unsigned char 	actValue = 0;
    unsigned int	x, y;
    
    for(y = 0; y < outImage->y; y++){
        actValue = 0;
        for( x = 0; x < outImage->x; x++){
            if(PIX_xy(outImage, x, y) == 0){
                PIX_xy(outImage, x, y) = actValue;
            }
            else if(PIX_xy(outImage, x, y) == 2){
                PIX_xy(outImage, x, y) = actValue = outValue;
            }
            else if(PIX_xy(outImage, x, y) == 3){
                PIX_xy(outImage, x, y) = outValue;
                actValue = 0;
            }
        }
     }
    
}
//----------------------------------------------------------------------------
segPos	RegionLocatorExtractionTool::checkNextSegment(int startPoint, \
                                                  vertices *imVertices, \
                                                  segPos actual){

    Double	X1Diff, Y1Diff, X2Diff, Y2Diff;
    Double	slope1, slope2, invSlope1, invSlope2;
    Int		sX1Diff, sY1Diff, sX2Diff, sY2Diff;
    Short	*P0, *P1, *P2;
    Int		nV = (Int) imVertices->nVertices;

    P0 = imVertices->VL[startPoint];
    P1 = imVertices->VL[(startPoint + 1 )%nV];
    P2 = imVertices->VL[(startPoint + 2 )%nV];
    
    X1Diff = P1[0] - P0[0];
    Y1Diff = P1[1] - P0[1];
    X2Diff = P2[0] - P1[0];
    Y2Diff = P2[1] - P1[1];
    
    sX1Diff = SIGN(X1Diff);
    sY1Diff = SIGN(Y1Diff);
    sX2Diff = SIGN(X2Diff);
    sY2Diff = SIGN(Y2Diff);

    /* check for a total overlaped segment */
    if ((Y1Diff != 0) && (Y2Diff != 0) ){
        invSlope1 = (X1Diff/Y1Diff);
        invSlope2 = (X2Diff/Y2Diff);
        /* a very special case */
        if(invSlope1 == invSlope2)
            return(checkNextSegment((++startPoint)%nV, imVertices, actual));  
    }
    else if( (X1Diff != 0) && (X2Diff != 0) ){
        slope1 = (Y1Diff/X1Diff);
        slope2 = (Y2Diff/X2Diff);
        if(slope1 == slope2)
            return(checkNextSegment((++startPoint)%nV, imVertices, actual));  
    }

    /* and now check normal cases */
    if( (sY1Diff != sY2Diff) && (Y1Diff != 0) && (Y2Diff != 0) ){
        if( sX1Diff == sX2Diff ){
            if( X1Diff >= 0)
                return RIGHT;
            else 
                return LEFT;
        }
        else if( (Y1Diff != 0) && (Y2Diff != 0) ){
                    
            if( X1Diff < 0 ){
                if( Y1Diff < 0 ){
                    if( invSlope2 > invSlope1 )
                        return RIGHT;
                    else 
                        return LEFT;
                }
                else{
                    if( invSlope2 < invSlope1 )
                        return RIGHT;
                    else 
                        return LEFT;
                }
            }
            else{ /* X1Diff > 0 or 0*/
                if( Y1Diff < 0 ){
                    if( invSlope2 < invSlope1 )
                        return LEFT;
                    else 
                        return RIGHT;
                }
                else{
                    if( invSlope2 > invSlope1 )
                        return LEFT;
                    else 
                        return RIGHT;
                }
            }
        }
    }
    else if( Y2Diff == 0){
        if( X2Diff < 0)
            return LEFT;
        else 
            return RIGHT;
    }
    else if( Y1Diff == 0){
        if( X1Diff > 0)
            return RIGHT;
        else 
            return LEFT;
    }
    else if( sY1Diff == sY2Diff)
        return actual;

    /* never reaches here, just to avoid warnings */
    return actual;
}

//----------------------------------------------------------------------------
void	RegionLocatorExtractionTool::VerticesToImagePerimeterColor(vertices *imVertices, MomImage *outImage){

    
    Int		i, yMinPos, nLines = 0;
    Short	**VL;
    Int		nV = (Int) imVertices->nVertices;
    segPos	next;
    Int		point, startPoint, backPoint, frontPoint;
    
    yMinPos = 0;
    VL = imVertices->VL;
    for( i = 0; i< nV; i++){
        if( VL[yMinPos][1] > VL[i][1])       yMinPos = i;
    }
    
    startPoint = yMinPos + nV - 1;
    while (checkNextSegment( (++startPoint)%nV, imVertices, LEFT) == LEFT );
    
    
    point = startPoint + nV;
    do{
        next = checkNextSegment(point%nV, imVertices, RIGHT);
        if( next == RIGHT ){
            /* thre's no change of direction, => draw line */
            if(VL[(point+1)%nV][1] > VL[point%nV][1])
                drawLine(outImage, VL[(point+1)%nV], VL[point%nV], 2);
            else  if(VL[(point+1)%nV][1] < VL[point%nV][1])
                drawLine(outImage, VL[(point+1)%nV], VL[point%nV], 3);
            else  if(VL[(point+1)%nV][1] == VL[point%nV][1])
                drawLine(outImage, VL[(point+1)%nV], VL[point%nV], 3);
            point++;
            nLines++;
        }
        else{
            /* store actual point (backPoint), search until 
               a different segment from next (frontPoint), 
               and draw backwards, and then  jump to frontpoint*/

            backPoint = point%nV;
            while( checkNextSegment((++point)%nV, imVertices, \
                                    LEFT) == LEFT);
            /* check for final exit */
            if( (((point + 1)%nV)-backPoint-1) + nLines <= nV && \
                point%nV != startPoint%nV)
                point++;
            frontPoint = (point)%nV;
            while( (point)%nV != backPoint ){
                /* draw lines clockwise */
                if(VL[(point-1)%nV][1] < VL[point%nV][1])
                    drawLine(outImage, VL[(point-1)%nV], VL[point%nV], 2);
                else  if(VL[(point-1)%nV][1] > VL[point%nV][1])
                    drawLine(outImage, VL[(point-1)%nV], VL[point%nV], 3);
                else  if(VL[(point-1)%nV][1] == VL[point%nV][1])
                    drawLine(outImage, VL[(point-1)%nV], VL[point%nV], 3); 
                point--;
                nLines++;
                if(nLines >= nV)
                    break;
            }
            /* once the lines have been drawn, jump to frontPoint */
            point = frontPoint + nV;
        }     

    }while(nLines < nV);

}

//----------------------------------------------------------------------------
int RegionLocatorExtractionTool::getImageFromVertices(vertices *v, \
                                                  MomImage *im){
    if(im == NULL){
        cout << "getImageFromVertices: Argument outImage NULL." << endl;
        exit(-1);
    }
    
    VerticesToImagePerimeterColor( v, im);
    fillPerimeterColor(im, 255);
    return 0;
}

//----------------------------------------------------------------------------
void 	RegionLocatorExtractionTool::binarise(MomImage *inImage, \
                                          MomImage *outImage,\
                                          int TH, int outL, \
                                          int outH){
    
    unsigned char 	*ptrOut;
    unsigned char	*ptrIn;
    unsigned int 	i;

    if(inImage->type != UCHAR_TYPE){
        cout << "binarise: Image type not supported." << endl;
        exit(-1);
    }
    
    ptrIn = inImage->data->u;
    
    if(outImage != NULL){
        if(outImage->type != UCHAR_TYPE){
            cout << "binarise: Image type not supported." << endl;
            exit(-1);
        }
        ptrOut = (UChar *)  outImage->data->u;
        for(i = 0; i < inImage->x * inImage->y; i++){
            *ptrOut = (*ptrIn < TH) ? (UChar) outL :(UChar)  outH;
            ptrIn++; ptrOut++;
        }
    }
    else{
        for(i = 0; i < inImage->x * inImage->y; i++){
            *ptrIn = (*ptrIn < TH) ? (UChar) outL : (UChar) outH;
            ptrIn++;
        }
    }   
 
}
//----------------------------------------------------------------------------
float RegionLocatorExtractionTool::ComputeAreaError(MomImage *ori, \
                                                MomImage *rec){
    unsigned char *ptr, *ptr2, *ptrEnd;
    int		nPelsOri, nPelsDiff;
    float	areaError;
    MomImage	*orib, *recb;

    if(rec == NULL || ori==NULL) return -1;
    
    if(ori->x != rec->x || ori->y != rec->y){
        cout << "ComputeAreaError: Images of different sizes" << endl;
        exit(-1);
    }

    /* construction of binary images */
    orib = (MomImage *) calloc(1, sizeof(MomImage));
    orib->x = ori->x;	orib->y = ori->y;
    orib->type = ori->type;
    orib->grid = ori->grid;
    orib->data = (MomImageData *) calloc(1, sizeof(MomImageType));
    orib->data->u = (unsigned char *) calloc(orib->x * orib->y, \
                                             sizeof(unsigned char));
    recb = (MomImage *) calloc(1, sizeof(MomImage));
    recb->x = rec->x;	recb->y = rec->y;
    recb->type = rec->type;
    recb->grid = rec->grid;
    recb->data = (MomImageData *) calloc(1, sizeof(MomImageType));
    recb->data->u = (unsigned char *) calloc(recb->x * recb->y, \
                                             sizeof(unsigned char));
    

    binarise(ori, orib, 1, 0, 1);
    binarise(rec, recb, 1, 0, 1);
    
    ptr = recb->data->u;
    ptr2 = orib->data->u;
    ptrEnd = ptr + (orib->x * orib->y);

    nPelsDiff = 0;
    nPelsOri = 0;

    while(ptr < ptrEnd){
        if(*ptr2)
            nPelsOri++;
        if(*ptr != *ptr2) 
            nPelsDiff++;
        ptr++; ptr2++;
    }

    areaError = 100 * (Float) nPelsDiff / (Float) nPelsOri;
    freeimg(orib);
    freeimg(recb);
    
    return areaError;

}

//----------------------------------------------------------------------------
int RegionLocatorExtractionTool::printResults(char *fileName, char *imageName, int DLength, float AreaError){

    ofstream file(fileName, ios::app);
    
    file << "#Info about " << imageName << ":" << endl;
    file << "Descriptor Length: " << DLength << "(" << (int) (DLength/8)
         << " bytes)" << endl;
    file << "Area error: " << AreaError << " %" << endl;
    return 0;
}




//---------------------------------------------------------------------------
struct loc * XM::VerticesToLocator(vertices *v){
    
    struct loc * locator;
    
    locator = (struct loc *) ecalloc(1, sizeof(struct loc));
    locator->NoVert = v->nVertices;
    
    locator->Vertices = (unsigned long int *) ecalloc(2*v->nVertices, sizeof(unsigned long int));
    
//    if(m_OneLocator) 
//    	  	locator->Vertices = (unsigned long int *) realloc(((2*v->nVertices) + sizeof(locator)), sizeof(unsigned long int));
       
    for(unsigned int i = 0; i < locator->NoVert; i++){
        locator->Vertices[2*i] = v->VL[i][0];
        locator->Vertices[2*i + 1] = v->VL[i][1];
    }
   
    return locator;
}


//---------------------------------------------------------------------------
vertices * XM::LocatorToVertices(struct loc * locator){

    vertices * v;
    
    v = (vertices *) calloc(1, sizeof(vertices));
    v->nVertices = locator->NoVert;
    v->VL = (short **) calloc(v->nVertices, sizeof(short *));
    
    for(unsigned int i = 0; i < locator->NoVert; i++){
      v->VL[i] = new short [2];//sth use new
        v->VL[i][0] = locator->Vertices[2*i];
        v->VL[i][1] = locator->Vertices[2*i + 1];
    }
    return v;

}
//------------------------------------------------------------------------
// calloc with checking error
char * XM::ecalloc(Int n, unsigned int s){

  Char  *p;
        
  if(s <= 0 || n <= 0) 
      cerr << "ERROR(ecalloc): Wrong size: <= 0." << endl;
  p = (Char *) calloc((unsigned int)n,(unsigned int)s);//sth use new.JH: low level extraction tools are wirtten in C.
  if(p == NULL)
      cerr <<  "ERROR(ecalloc): memory allocation error." << endl;
        
  return p;
}





///////////////////////////////////
// Previous Extern "C" functions //
///////////////////////////////////


void RegionLocatorExtractionTool::printVertices(Char *fileName, vertices *imVertices){
    
    FILE 	*file;
       
    fprintf(stderr, "Writing file: %s...\n", fileName);

    file = fopen(fileName, "w");
    fprintf(file,"#Number of vertices : %d\n", imVertices->nVertices);
    for(int i = 0; i < imVertices->nVertices - 1; i++){
        fprintf(file, "%d %d %d %d\n", imVertices->VL[i][0],\
                imVertices->VL[i][1], imVertices->VL[i+1][0], \
                imVertices->VL[i+1][1]);
    }
    fclose(file);
}
//----------------------------------------------------------------------------

double RegionLocatorExtractionTool::vectProd2D(Int *v1, Int *v2){
    return (v1[0]*v2[1] - v1[1]*v2[0]);
}

//----------------------------------------------------------------------------
int  RegionLocatorExtractionTool::getContour(MomImage *inShape, MomImage **outImage, \
                           unsigned int outVal, Int **positions){

    
    Int         nachbarn[8][2]={{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};

    Int        i, j, count;
    UChar       outcol;
    Int         sx, sy, x, y;
    Int         dir, d, dx, dy, startx, starty;
    Int         lastx,lasty;     
    UShort      flag;
    
    sx = inShape->x;
    sy = inShape->y;
    if(outImage){
//        (*outImage) = AllocSameImage(inShape);
    }  
    /* find the lowest value of the image and use it as a shape container */
    outcol = 255;
    for(i = 0; i < sx; i++){
        for(j = 0; j < sy; j++){
            if( PIX_xy(inShape, i, j) < outcol )
                outcol = PIX_xy(inShape, i, j);
        }
    }
   
    flag = 0;
    /* search for a good start pixel of the shape*/
    for(y=0;y<sy;y++){
        for(x=0;x<sx;x++){
            if(PIX_xy(inShape, x, y) != outcol){        /* a candidate to start point */
                flag = 1;
                break;
            }
        }
        if(flag) break;
    }

    startx=x;
    starty=y;

    lastx = x = startx;
    lasty = y = starty;
    count = 0;
    dir = 5;
    do{
        for(d=8;d>0;d--){
            dx=nachbarn[(d+dir)%8][0];
            dy=nachbarn[(d+dir)%8][1];
            if(x+dx>=sx) continue;
            if(x+dx<0) continue;
            if(y+dy>=sy) continue;
            if(y+dy<0) continue;
            if((x+dx==startx)&&(y+dy==starty)){
                d=0;
            }
            if(PIX_xy(inShape, x+dx, y+dy) != outcol){
                if(outImage){
//                    PIX_xy((*outImage), x+dx, y+dy) = outVal;
                }
                count++;
                dir=(dir+d-5+8)%8;
                x=x+dx;
                y=y+dy;
                break;
            }
        }
    }while(d>0);


    /* added on 11th january 2K */
    /* this part returns the contour positions in an unidimensional array */
    if(positions){
        (*positions) = (Int *) ecalloc( 2*(count + 1), sizeof(Int));

        lastx = x = startx;
        lasty = y = starty;
        dir = 5;
        i = 0;
        do{
            for(d=8;d>0;d--){
                dx=nachbarn[(d+dir)%8][0];
                dy=nachbarn[(d+dir)%8][1];
                if(x+dx>=sx) continue;
                if(x+dx<0) continue;
                if(y+dy>=sy) continue;
                if(y+dy<0) continue;
                if((x+dx==startx)&&(y+dy==starty)){
                    d=0;
                }
                if(PIX_xy(inShape, x+dx, y+dy) != outcol){
                    (*positions)[2*i] = x+dx;
                    (*positions)[2*i + 1] = y+dy;
                    i++;
                    dir=(dir+d-5+8)%8;
                    x=x+dx;
                    y=y+dy;
                    break;
                }
            }
        }while(d>0);
        (*positions)[2*i] = -1;
        (*positions)[2*i + 1] = -1;

    }   /* end filling positions array */

    return count;

}


//----------------------------------------------------------------------------
int RegionLocatorExtractionTool::getImageArea(MomImage *inShape){

  UChar *ptr, *ptrEnd;
  Int count;

  if (inShape->type !=  UCHAR_TYPE){
      fprintf(stderr, "ERROR(getImageArea): only UCHAR_TYPE supported.");
      exit(-1);
  }
  count = 0;
  ptr = inShape->data->u;
  ptrEnd = ptr + (inShape->x * inShape->y);
  while(ptr != ptrEnd){
      if( *ptr ) count++;
      ptr++;
  }
  return count;
}

//----------------------------------------------------------------------------
int	RegionLocatorExtractionTool::drawLine(MomImage *outImage, Short *p1, Short *p2, unsigned int val){

    Float 	xdiff = p2[0] - p1[0];
    Float 	ydiff = p2[1] - p1[1];
    Int		i, j; 
    Int		sign;

    if(outImage == NULL)
        fprintf(stderr, "drawLine: Image to be drawn is NULL.\n");
    
    if( xdiff == 0){
        sign = (Int) (ydiff / ABS(ydiff));
        for(j = 0; ABS(j) <= ABS(ydiff); j += sign){
            PIX_xy(outImage, p1[0], p1[1] + j) = (UChar) val;
        }
    }
    else if( ydiff == 0){
        sign = (Int) (xdiff / ABS(xdiff));
        for(i = 0; ABS(i) <= ABS(xdiff); i += sign){
            PIX_xy(outImage, p1[0] + i, p1[1]) = (UChar) val;
        }
    }
    else{ 
        /* choose always minimum slope */
        if( ABS(xdiff) > ABS(ydiff) ){
            sign = (Int) (xdiff / ABS(xdiff));
            for(i = 0; ABS(i) <= ABS(xdiff); i += sign){
                PIX_xy(outImage, p1[0] + i, \
                       (Int) floor(p1[1] + (ydiff/xdiff)*i + 0.5) ) = (UChar) val;
                PIX_xy(outImage, p2[0] - i, \
 (Int) floor(p2[1] + (ydiff/xdiff)*(-i) + 0.5) ) = (UChar) val;

            }
        }
        else{
            sign = (Int) (ydiff / ABS(ydiff));
            for(j = 0; ABS(j) <= ABS(ydiff); j += sign){
                PIX_xy(outImage, (Int) floor(p1[0] + 0.5 + (xdiff/ydiff)*j),\
                       p1[1] + j ) = (UChar) val;
                PIX_xy(outImage, (Int) floor(p2[0] + 0.5 + (xdiff/ydiff)*(-j)),\
                       p2[1] - j ) = (UChar) val;
                
            }
        }
    }
    return (unsigned int) MAX(ABS(xdiff), ABS(ydiff));
}


//----------------------------------------------------------------------------
void	RegionLocatorExtractionTool::VerticesToImagePerimeter( vertices *imVertices, \
                                          MomImage *outImage, Int value){
    Int i;
    Short	**vl;

    vl = imVertices->VL;
    for (i = 1; i < imVertices->nVertices; i++){
        drawLine(outImage, vl[i-1], vl[i], value);
    }
    /* and now draw the line between the last vertice and the first */
    drawLine(outImage, vl[i-1], vl[0], value);

}


//----------------------------------------------------------------------------
void RegionLocatorExtractionTool::getVerticesFromImage( MomImage *inShape, vertices *imVertices){
    
    Int 	nachbarn[8][2]={{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};

    Int		i=0, j;
    UChar 	outcol;
    Int 	sx, sy, x, y;
    Int 	dir=0, d, dx, dy,  startx, starty;
    Int 	lastx,lasty;     
    UShort	flag, loopEnd;
    Short	**tmpVL;

    sx = inShape->x;
    sy = inShape->y;

    tmpVL = (Short **) ecalloc(MAX_VERTICES, sizeof(Short *));
        
    /* find the lowest value of the image and use it as a shape container */
    flag = 0;
    outcol = 255;
    for(i = 0; i < sx; i++){
        for(j = 0; j < sy; j++){
            if( PIX_xy(inShape, i, j) < outcol )
                outcol = PIX_xy(inShape, i, j);
            if(outcol == 0){
                flag = 1;
                break;
            }
        }
        if(flag) break;
    }
   
    /* search for a pixel of the shape */
    flag = 0;
    for(y=0;y<sy;y++){
      	for(x=0;x<sx;x++){
            if(PIX_xy(inShape, x, y) != outcol){	/* a candidate to start point */
                flag = 1;
                break;
            }
        }
        if(flag) break;
    }

    startx=x;
    starty=y;
    
    lastx = x = startx;
    lasty = y = starty;
    i = 0;
    loopEnd = 0;
    dir = 5;
    do{
	for(d=8;d>0;d--){
	    dx=nachbarn[(d+dir)%8][0];
	    dy=nachbarn[(d+dir)%8][1];
	    if(x+dx>=sx) continue;
	    if(x+dx<0) continue;
	    if(y+dy>=sy) continue;
	    if(y+dy<0) continue;
	    if((x+dx==startx)&&(y+dy==starty)){
		loopEnd = 1;
                break;
            }
	    if(PIX_xy(inShape, x+dx, y+dy) != outcol){
                tmpVL[i] = (Short *) ecalloc(4, sizeof(Short));
                tmpVL[i][0] = x;
                tmpVL[i][1] = y;
                tmpVL[i][2] = x+dx;
                tmpVL[i][3] = y+dy;
//                fprintf(slf,"%d %d %d %d\n",x,sy-y,x+dx,sy-(y+dy));
                i++;
                if(i > MAX_VERTICES) fprintf(stderr, \
                                             "getVerticesFromImage: Too many vertices\n");
		dir=(dir+d-5+8)%8;
		x=x+dx;
		y=y+dy;
		break;
	    }
	}
    }while( !loopEnd );
   
    imVertices->nVertices = i + 1;
    imVertices->VL = (Short **) ecalloc(imVertices->nVertices, sizeof(Short *));
    imVertices->VL[0] = (Short *) ecalloc(2, sizeof(Short));
    imVertices->VL[0][0] = (Short) tmpVL[0][0];
    imVertices->VL[0][1] = (Short) tmpVL[0][1];
    for( i = 1; i < imVertices->nVertices; i++){
        imVertices->VL[i] = (Short *) ecalloc(2, sizeof(Short));
        imVertices->VL[i][0] = (Short) tmpVL[i-1][2];
        imVertices->VL[i][1] = (Short) tmpVL[i-1][3];
        free((Short *) tmpVL[i-1]);
    }
    free((Short **) tmpVL);
    
}

//----------------------------------------------------------------------------
/******************CommentBegin****************************************
 *
 *  Routine Name:  getAngle
 *
 *        Syntax:  Float	getAngle(Short *VL1, Short *VL2)
 *
 *       Purpose:  calculate the angle of two segments
 *
 *         Input:  Short *VL1 : End of the segment with zero origin
 *		   Short *VL2 : End of the segment with zero origin
 *        Output:  
 *
 *       Returns:  the angle between both segments (0-360)
 *
 *          Date:  
 * Modifications:  
 *
 *      Comments:   when VL2 is null, the angle returned it the one 
 *		    with xaxis
 *
******************CommentEnd******************************************/

float	RegionLocatorExtractionTool::getAngle(Short *VL1, Short *VL2){
    
    Float	angle;
    Float	dx, dy;
    Float 	at;

    if(VL2 == NULL){
       angle = atan2((double)VL1[1], (double)VL1[0]);
       angle *= (180.0/PI);
       
    }
    else{

        dx = VL2[0]-VL1[0];
        dy = VL2[1]-VL1[1];
        
        if (dx==0){
            if (dy>0) angle=90;
            else angle=-90;
        }
        else{
            at=atan((Double)dy/(Double)dx)*180.0/PI;
            if (dx>0){
                angle = at;
                if(at>0) angle = at;
                else angle=(360.0 + at);
            }
            else
                angle=(180.0 + at);
        }

    }
    return(angle);
}

//----------------------------------------------------------------------------
/******************CommentBegin****************************************
 *
 *  Routine Name:  initTriangles
 *
 *        Syntax:  triangle * initializeTriangles(vertices *imVertices)
 *
 *       Purpose:  create triangle structure and link them, and map the position
 *		   of the vertices inside
 *
 *         Input:  vertices *imVertices: vertices to be mapped
 *
 *        Output:  
 *
 *       Returns:  triangle *: the first triangle;
 *
 *          Date:  
 * Modifications:  
 *
 *      Comments:   
 *
******************CommentEnd******************************************/


Triangle ** RegionLocatorExtractionTool::initTriangles(vertices *imVertices){


    Int		i;
    Triangle	**triangles;
    Triangle	*first, *actual, *next;

    triangles = (Triangle **) ecalloc(imVertices->nVertices, sizeof(Triangle *));

    triangles[0] = (Triangle *) ecalloc(1, sizeof(Triangle));
    first = triangles[0];
    first->whoami = 0;
    first->x = imVertices->VL[0][0];
    first->y = imVertices->VL[0][1];
    first->area = 0;
    first->deleted = 0;
    
    /* create as many trinagles as vertices, and link them */
    
    actual = first;
    for(i = 1; i < imVertices->nVertices; i++){
        triangles[i] = (Triangle *) ecalloc(1, sizeof(Triangle));
        next = triangles[i];
        next->whoami = i;
        next->x = imVertices->VL[i][0];
        next->y = imVertices->VL[i][1];
        next->area = 0;
        next->deleted = 0;

        actual->next = next;
        next->prev = actual;
        
        actual = next;
    }
    /* link last vertice with the first one */
    actual->next = first;
    first->prev = next;
    
    return triangles;
}


//----------------------------------------------------------------------------
/******************CommentBegin****************************************
 *
 *  Routine Name:  FreeTriangles
 *
 *        Syntax:  Void	FreeTriangles(Triangle **t, Int nVert)
 *
 *       Purpose:  to free structures Triangle
 *
 *         Input:  Triangle **t: the index of the matrix
 *		   Int nVert: the number of structures in this matrix
 *
 *        Output:  
 *
 *       Returns:  
 *
 *          Date:  
 * Modifications:  
 *
 *      Comments:   
 *
******************CommentEnd******************************************/

void	RegionLocatorExtractionTool::FreeTriangles(Triangle **t, Int nVert){

    Int		i;
    
    for(i = 0; i < nVert; i++)
        free((Triangle *) t[i]);
    free((Triangle **) t);
}

//----------------------------------------------------------------------------
/******************CommentBegin****************************************
 *
 *  Routine Name: FreeCostItems 
 *
 *        Syntax:  Void FreeCostItems(CostItem *startCost)
 *
 *       Purpose:  to free the reaming costs
 *
 *         Input:  CostItem *startCost: any element of the list
 *
 *        Output:  
 *
 *       Returns:  
 *
 *          Date:  
 * Modifications:  
 *
 *      Comments: the input can be any elelment because they're all linked
 *
******************CommentEnd******************************************/

void RegionLocatorExtractionTool::FreeCostItems(CostItem *startCost){

    CostItem 	*actual, *next;
    
    actual = startCost;
    do{
        next = actual->next;
        free((CostItem *) actual);
        actual = next;
    }while(actual != startCost);
}    

//----------------------------------------------------------------------------
/******************CommentBegin****************************************
 *
 *  Routine Name:  printCostItems
 *
 *        Syntax: Void printCostItems(CostItem *startCost) 
 *
 *       Purpose: to print the elements of the cost list
 *
 *         Input: CostItem *startCost: the first element of the list 
 *
 *        Output:  
 *
 *       Returns:  
 *
 *          Date:  
 * Modifications:  
 *
 *      Comments: If the input is not the first element, it will print 
 *		  until the element with the higher cost. The output is 
 *		  a file called CostItems.txt
 *
******************CommentEnd******************************************/

void RegionLocatorExtractionTool::printCostItems(CostItem *startCost){
    

    CostItem 	*actual;
    FILE	*f;


    f = fopen("CostItems.txt", "w");
    fprintf(f, "## Costs to remove vertices:\n\n");

    actual = startCost;
    do{
        fprintf(f, "%f\t%d\t%d\t%d\n", actual->cost, actual->who->whoami, \
                actual->who->x, actual->who->y);
        actual = actual->next;
    }while(actual->who != NULL);

    fclose(f);
}

//----------------------------------------------------------------------------
/******************CommentBegin****************************************
 *
 *  Routine Name:  fillArea
 *
 *        Syntax:  Void 	fillArea(Triangle *actual)
 *
 *       Purpose: to fill the parameter area in a Triangle structure 
 *
 *         Input: Triangle *actual: stuct whose area need to be filled out
 *
 *        Output:  
 *
 *       Returns:  
 *
 *          Date:  
 * Modifications:  
 *
 *      Comments: the value of the newly computed area is always added
 *		  to the old one.
 *
******************CommentEnd******************************************/

void 	RegionLocatorExtractionTool::fillArea(Triangle *actual){

    Short	A[2], B[2];
    Float	angle;
    
    
    A[0] = actual->prev->x - actual->x;
    A[1] = actual->prev->y - actual->y;
    
    B[0] = actual->next->x - actual->x;
    B[1] = actual->next->y - actual->y;
    
    angle = ABS(getAngle(A, NULL) - getAngle(B, NULL)) * PI/180.0;
    
    actual->area += actual->distPrev * actual->distNext * ABS(sin(angle)) / 2.0;
    
}

//----------------------------------------------------------------------------
/******************CommentBegin****************************************
 *
 *  Routine Name:  fillDist
 *
 *        Syntax: Void 	fillDist(Triangle *actual) 
 *
 *       Purpose: to fill out the distance parameters in the 
 *		  Triangle struct  
 *
 *         Input:  Triangle *actual: struct whose distance fields 
 *		   need to be filled out
 *
 *        Output:  
 *
 *       Returns:  
 *
 *          Date:  
 * Modifications:  
 *
 *      Comments:   
 *
******************CommentEnd******************************************/

void RegionLocatorExtractionTool::fillDist(Triangle *actual){


   Int		xD, yD;

   xD = actual->prev->x - actual->x;
   yD = actual->prev->y - actual->y;
   actual->distPrev = sqrt((float)(xD*xD + yD*yD));
   
   xD = actual->next->x - actual->x;
   yD = actual->next->y - actual->y;
   actual->distNext = sqrt((float)(xD*xD + yD*yD));

}


//----------------------------------------------------------------------------
/******************CommentBegin****************************************
 *
 *  Routine Name: fillTriangles
 *
 *        Syntax:  Void 	fillTriangleDist(Triangle **listTriangles)
 *
 *       Purpose:  fill the distance fields and also the area field
 *
 *         Input:  Triangle **listTriangles: the list of elements
 *
 *        Output:  
 *
 *       Returns:  
 *
 *          Date:  
 * Modifications:  
 *
 *      Comments:   
 *
******************CommentEnd******************************************/

void RegionLocatorExtractionTool::fillTriangles(Triangle **listTriangles){

    Triangle 	*first, *actual;
 
    
    first = (Triangle *) listTriangles[0];
    actual = first;

    do{    
        
        /* compute now the area */
        /* tomorrow and look if we can do it with the vectorial product...*/
        /* distances required for the area */
        fillDist(actual);
        fillArea(actual);

        actual = actual->next;
    }while(actual != first);

}


//----------------------------------------------------------------------------
/******************CommentBegin****************************************
 *
 *  Routine Name:  sortAreas
 *
 *        Syntax:  CostItem * sortAreas(Triangle **listTriangles, Int nVert)
 *
 *       Purpose:  to sort the areas of the Triangles structures
 *		   and produce a CostItem list with them sorted, 
 *		   from the minor till the biggest
 *
 *         Input:  Triangle **listTriangles: list of Triangle structs
 *		   Int nVert: number of structs in the list
 *
 *        Output:  
 *
 *       Returns:  CostItem *: a pointer to the first element of the list
 *		   	       containing the smallest cost
 *
 *          Date:  
 * Modifications:  
 *
 *      Comments:   
 *
******************CommentEnd******************************************/

CostItem * RegionLocatorExtractionTool::sortAreas(Triangle **listTriangles, Int nVert){

    CostItem	*actual, *first, *last, *tmp;
    Int		i;

    /* sort down->top */
        
    first = (CostItem *) ecalloc(1, sizeof(CostItem));
    last = (CostItem *) ecalloc(1, sizeof(CostItem));
    first->next = last;
    first->prev = last;
    last->prev = first;
    last->next = first;
    last->cost = 1e20;

    first->cost = listTriangles[0]->area;
    first->who = listTriangles[0];

    for(i = 1; i < nVert; i++){
        actual = last->next;
        while(listTriangles[i]->area > actual->cost)
            actual = actual->next;
        /* fill new structure */
        tmp = (CostItem *) ecalloc(1, sizeof(CostItem));
        tmp->cost =  listTriangles[i]->area;
        tmp->who = listTriangles[i];
        /* insert before actual */
        tmp->next = actual;
        tmp->prev = actual->prev;
        actual->prev->next = tmp;
        actual->prev = tmp;
    }


    return last->next;
    
}


//----------------------------------------------------------------------------
/******************CommentBegin****************************************
 *
 *  Routine Name:   modifyCost
 *
 *        Syntax:   Void modifyCost(CostItem *startCost, Triangle *actual) 
 *
 *       Purpose:  to insert the area of the Triangle struct in the list
 *		   of costs whose first element is startCost
 *
 *         Input: CostItem *startCost: first element of the cost list
 *		  Triangle *actual: struct containing the area that 
 *				    need to be inserted
 *
 *        Output:  
 *
 *       Returns:  
 *
 *          Date:  
 * Modifications:  
 *
 *      Comments:   the searching algorithm could be improved
 *
******************CommentEnd******************************************/

void 	RegionLocatorExtractionTool::modifyCost(CostItem *startCost, Triangle *actual){

    Int		flagPosOld, flagPosNew;
    CostItem 	*posOld, *posNew;
    CostItem	*costS;
    

    flagPosOld = 0;
    costS = startCost;

    /* search where we insert the new cost, and where the old cost is */
    while(costS->who != actual)
        costS = costS->next;
    posOld = costS;
/*
    do{
        if(costS->who == actual){
            posOld = costS;
            flagPosOld = 1;
        }
        costS = costS->next;
    }while(costS != NULL && flagPosOld == 0);
*/    
    /* put the correct value under cost field */
    posOld->cost = actual->area;
    
    /* place the cost */
    /* arrange the old position */
    posOld->prev->next = posOld->next;
    posOld->next->prev = posOld->prev;


    flagPosNew = 0;
    costS = startCost;
    
    while(actual->area > costS->cost)
        costS = costS->next;
    posNew = costS;
    /* the new position is before pos new */
    posNew = posNew->prev;
    /* now is behind posNew */
    
/*
    do{
        if(actual->area > costS->cost && actual->area <= costS->next->cost ){
            posNew = costS;
            flagPosNew = 1;
        }
        costS = costS->next;
    }while(costS != NULL && flagPosNew == 0);
*/

    /* the new position to put them is behind posNew */
    posOld->prev = posNew;
    posOld->next = posNew->next;
    
    posOld->next->prev = posOld;
    posNew->next = posOld;

}


//----------------------------------------------------------------------------
/******************CommentBegin****************************************
 *
 *  Routine Name: canBeRemoved 
 *
 *        Syntax:  Int	canBeRemoved(Triangle *toBeRemoved)
 *
 *       Purpose: to check if a Triangle struct can be removed 
 *		  during evolution
 *
 *         Input:  Triangle *toBeRemoved: the candidate to be removed
 *
 *        Output: 
 *
 *       Returns:  Int : 0 when it cannot be removed
 *		         1 when can be removed
 *
 *          Date:  
 * Modifications:  
 *
 *      Comments:  A vertex cannot be removed if another vertex lies 
 *		   on the triangle that will be removed. This avoid 
 *		   crossings
 *
******************CommentEnd******************************************/

int	RegionLocatorExtractionTool::canBeRemoved(Triangle *toBeRemoved){

    Triangle	*prev, *next, *actual;
    Float	vpAB, vpBC, vpCA;
    Int		A[2], B[2], C[2];

    prev = toBeRemoved->prev;
    next = toBeRemoved->next;

    /* we have to check all the vertices until 
     * the prev vertex 
     */
    
    actual = next->next;
    while(actual != prev){
        A[0] = actual->x - prev->x;
        A[1] = actual->y - prev->y;
        B[0] = actual->x - toBeRemoved->x;
        B[1] = actual->y - toBeRemoved->y;
        C[0] = actual->x - next->x;
        C[1] = actual->y - next->y;      

        vpAB = vectProd2D(A, B);
        vpBC = vectProd2D(B, C);
        vpCA = vectProd2D(C, A);
        if(vpAB + vpBC + vpCA != 0){
            if(vpAB == 0){
                if(vpBC != 0)
                    vpAB = vpBC;
                else
                    vpAB = vpCA;
            }
            if(vpBC == 0){
                if(vpCA != 0)
                    vpBC = vpCA;
                else
                    vpBC = vpAB;
            }
            if(vpCA == 0){
                if(vpAB != 0)
                    vpCA = vpAB;
                else
                    vpCA = vpBC;
            }
            
            if(SIGN(vpAB) == SIGN(vpBC))
                if(SIGN(vpCA) == SIGN(vpBC))
                    return 0;
        }
        actual = actual->next;
    }
    return 1;

}


//----------------------------------------------------------------------------
/******************CommentBegin****************************************
 *
 *  Routine Name: deleteLowest 
 *
 *        Syntax:  CostItem * deleteLowest( CostItem * startCost)
 *
 *       Purpose: to delete the Triangle with the lowest cost 
 *
 *         Input:  CostItem * startCost: the lowest cost of the list
 *
 *        Output:  
 *
 *       Returns:  CostItem *: the new lowest cost of the list
 *
 *          Date:  
 * Modifications:  
 *
 *      Comments: if a Triangle cannot be removed, its cost is changed
 *		  to the highest possible, and reallocated in the cost 
 *		  list
 *
******************CommentEnd******************************************/

CostItem * RegionLocatorExtractionTool::deleteLowest( CostItem * startCost){

    Triangle	*actual, *prev, *next;
    CostItem	*last;

    /* if the cost is over a certain TH, then check that no vertex 
       lays on the removed area 
       */

//    last = startCost->prev;

    if(startCost->cost >= 3){
        if(!canBeRemoved(startCost->who)){
//   printCostItems(startCost);
            startCost->next->prev = startCost->prev;
            startCost->prev->next = startCost->next;         
            
            last = startCost->prev;

            last->prev->next = startCost;
            startCost->prev = last->prev;
            startCost->next = last;
            last->prev = startCost;

            startCost->cost = last->cost;
            startCost->who->area = startCost->cost;

            return last->next;

        }
    }

    actual = startCost->who;
    actual->deleted = 1;
    prev = actual->prev;
    next = actual->next;
    
    prev->next = actual->next;
    next->prev = actual->prev;

    /* arrange the new cost area for the neighbours */
    
    fillDist(prev);
    //    prev->area = actual->area;
    fillArea(prev);
    fillDist(next);
    //    next->area = actual->area;
    fillArea(next);

    modifyCost(startCost, prev);
    modifyCost(startCost, next);
    
    startCost->next->prev = startCost->prev;
    startCost->prev->next = startCost->next;
    last = startCost->prev;
    free((CostItem *)startCost);
    return last->next;
}


//----------------------------------------------------------------------------
/******************CommentBegin****************************************
 *
 *  Routine Name: TrianglesToVertices 
 *
 *        Syntax: Void	TrianglesToVertices(Triangle *first, vertices **imVertices) 
 *
 *       Purpose: to map structure Triangle into structure vertices
 *
 *         Input: Triangle *first: any VALID element of the Triangle struct
 *
 *        Output: vertices **imVertices: the address of the struct vertices,
 *		  	space is created inside
 *
 *       Returns:  
 *
 *          Date:  
 * Modifications:  
 *
 *      Comments: by VALID element it is undertood a Triangle that still
 *		  belongs to the contour.
 *
******************CommentEnd******************************************/

void	RegionLocatorExtractionTool::TrianglesToVertices(Triangle *first, vertices **imVertices){

    Triangle 	*actual;
    Short	**vl;
    Int		i;

    actual = first;
    (*imVertices) = (vertices *) ecalloc(1, sizeof(vertices));
    (*imVertices)->VL = (Short **) ecalloc(MAX_VERTICES, sizeof(Short *));
    vl = (*imVertices)->VL;
    
    vl[0] = (Short *) ecalloc(2, sizeof(Short));
    vl[0][0] = actual->x;
    vl[0][1] = actual->y;
    actual = actual->next;
    i = 0;
    do{
        if(actual->x != vl[i][0] || actual->y != vl[i][1]){
            i++;
            vl[i] = (Short *) ecalloc(2, sizeof(Short));
            vl[i][0] = actual->x;
            vl[i][1] = actual->y;
        }
        actual = actual->next;
    }while(actual != first);
    
    (*imVertices)->nVertices = i+1;
}


//----------------------------------------------------------------------------
/******************CommentBegin****************************************
 *
 *  Routine Name:  classicalEvolution
 *
 *        Syntax:  Void classicEvolution(vertices *imVertices,  \
 *					 vertices **outVertices, \
 *			                 Float costTH, Int vertTH)
 *
 *       Purpose:  to evolute vertices
 *
 *         Input:  vertices *imVertices: original vertices
 *	           Float costTH: evolv up to this area cost
 *		   Int vertTH: evolv un to this number of vertices
 *
 *        Output:  vertices **outVertices: to store the remaining vertices.
 *		           		   Allocation inside. If NULL ignored.
 *
 *  Input/Output:  
 *
 *       Returns:  
 *
 *          Date:  
 * Modifications:  
 *
 *      Comments:  the original vertices are evolved up to nVertTH or
 *		   costTH, whichever the most restrictive. 
 *
******************CommentEnd******************************************/

void RegionLocatorExtractionTool::classicEvolution(vertices *imVertices, vertices **outVertices, \
                                 Float costTH, Int vertTH){


    Triangle	**listTriangles;
    Int		nOriTriangles;
    Int		nActTriangles;
    CostItem	*startCost;
    Float	areaObject, contourLength;

    listTriangles = initTriangles(imVertices);
    nOriTriangles = imVertices->nVertices;
    fillTriangles(listTriangles);
 
    startCost = sortAreas(listTriangles, nOriTriangles);
    areaObject = getImageArea(imVertices->oriImage);
    contourLength = getContour(imVertices->oriImage, NULL, 0, NULL);

    /* start deleting till .... */
    
    nActTriangles = nOriTriangles;
    while(startCost->cost < costTH*contourLength/*areaObject*/ && nActTriangles > vertTH){
        /* delete the lowest cost */
        startCost = deleteLowest(startCost);
        nActTriangles--;
    }

    if(outVertices != NULL){
        TrianglesToVertices(startCost->who, outVertices);
        (*outVertices)->oriImage = imVertices->oriImage;
        (*outVertices)->recImage = imVertices->recImage;
    }
    
    FreeTriangles(listTriangles, nOriTriangles);
    FreeCostItems(startCost);


}
//---------------------------------------------------
int RegionLocatorExtractionTool::getRangeExt(int x){

    if( x == 0)
        return (0);
    else if( x <= 1 )
        return (1);
    else if( x <= 3 )
        return (2);
    else if( x <= 7 )
        return (3);
    else if( x <= 15 )
        return (4);
    else if( x <= 31 )
        return (5);
    else if( x <= 63 )
        return (6);
    else if( x <= 127 )
        return (7);
    else if( x <= 255 )
        return (8);
    else if( x <= 511 )
        return (9);
    else if( x <= 1023 )
        return (10);
    else if( x <= 2047 )
        return (11);
    else if( x <= 4095 )
        return (12);
    else if( x <= 8191 )
        return (13);
    else if( x <= 16383 )
        return (14);
    else if( x <= 32767 )
        return (15);
    else if( x <= 65535 )
        return (16);

    fprintf(stderr, "getRange: Value %d out of range.\n", x);

    return (-1);
}
