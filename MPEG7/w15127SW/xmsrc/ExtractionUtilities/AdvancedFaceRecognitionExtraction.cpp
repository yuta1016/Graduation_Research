//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Toshio Kamei, NEC Corporation.
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
// Copyright (c) 1998-2002.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  AdvancedFaceRecognitionExtraction.cpp
//

#include <stdio.h>
#include <math.h>
#include "momusys.h"
#include "inter.h"
#include "intra.h"
#include "fifo.h"
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/Utilities.h"

//=============================================================================

using namespace XM;

const UUID AdvancedFaceRecognitionExtractionInterface::myID = UUID("");
const char *AdvancedFaceRecognitionExtractionInterface::myName = "AdvancedFaceRecognitionDescriptorExtractorInterface";

const UUID AdvancedFaceRecognitionExtractionTool::myID = UUID("");
const char *AdvancedFaceRecognitionExtractionTool::myName = "AdvancedFaceRecognitionDescriptorExtractor";

const UUID AdvancedFaceRecognitionExtractionInterfaceABC::myID = UUID();

// AdvancedFaceRecognition parameters
#include "ExtractionUtilities/AdvancedFaceRecognition/HolisticDiscriminantMatrix.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/HolisticDiscriminantMatrixPW.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/HolisticDiscriminantMatrixRI.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/HolisticMeanVectorPW.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/HolisticMeanVectorRI.h"
		      
#include "ExtractionUtilities/AdvancedFaceRecognition/CentralDiscriminantMatrix.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/CentralDiscriminantMatrixPW.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/CentralDiscriminantMatrixRI.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/CentralMeanVectorPW.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/CentralMeanVectorRI.h"

#include "ExtractionUtilities/AdvancedFaceRecognition/eCentralDiscriminantMatrix1.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/eCentralDiscriminantMatrix2.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/eCentralDiscriminantMatrix3.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/eCentralDiscriminantMatrix4.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/eCentralDiscriminantMatrix5.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/eCentralMeanVector1.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/eCentralMeanVector2.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/eCentralMeanVector3.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/eCentralMeanVector4.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/eCentralMeanVector5.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/eCentralMergeMatrix.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/eHolisticDiscriminantMatrix.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/eHolisticMeanVector.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/eHolisticMergeMatrix.h"

#include "ExtractionUtilities/AdvancedFaceRecognition/PosePCASet_0001.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/PosePCASet_0002.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/PosePCASet_0003.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/PosePCASet_0004.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/PosePCASet_0005.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/PosePCASet_0006.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/PosePCASet_0007.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/PosePCASet_0008.h"
#include "ExtractionUtilities/AdvancedFaceRecognition/PosePCASet_0009.h"

//=============================================================================
AdvancedFaceRecognitionExtractionInterfaceABC::AdvancedFaceRecognitionExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
AdvancedFaceRecognitionExtractionInterface::AdvancedFaceRecognitionExtractionInterface(AdvancedFaceRecognitionExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionExtractionInterface::~AdvancedFaceRecognitionExtractionInterface()
{
}

//----------------------------------------------------------------------------
void AdvancedFaceRecognitionExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
int AdvancedFaceRecognitionExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionDescriptorInterfaceABC* AdvancedFaceRecognitionExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int AdvancedFaceRecognitionExtractionInterface::
SetDescriptorInterface(AdvancedFaceRecognitionDescriptorInterfaceABC
		       *aAdvancedFaceRecognitionDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(aAdvancedFaceRecognitionDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long AdvancedFaceRecognitionExtractionInterface::InitExtracting(void)
{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long AdvancedFaceRecognitionExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long AdvancedFaceRecognitionExtractionInterface::PostExtracting(void)

{
  return m_ExtractionTool->PostExtracting();
}

//----------------------------------------------------------------------------
const UUID& AdvancedFaceRecognitionExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *AdvancedFaceRecognitionExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
AdvancedFaceRecognitionExtractionTool::AdvancedFaceRecognitionExtractionTool():
m_Interface(this),
m_DescriptorInterface(0),
m_Media(0),
m_FrameCnt(0) // add for XM 5.0

{
  m_FrameCnt=0;
  SetExposedInterface(&m_Interface);
}
using namespace XM;
//----------------------------------------------------------------------------
AdvancedFaceRecognitionExtractionTool::
AdvancedFaceRecognitionExtractionTool(AdvancedFaceRecognitionDescriptorInterfaceABC
				      *aDescriptorInterface):
  m_Interface(this),
  m_DescriptorInterface(0),
  m_Media(0),
  m_FrameCnt(0) // add for XM 5.0
{
  /* create descriptor if it does not exist*/
  if (!aDescriptorInterface) {
    AdvancedFaceRecognitionDescriptor *descriptor =
      new AdvancedFaceRecognitionDescriptor();
    aDescriptorInterface=descriptor->GetInterface();
  }
  
  /* connect Descritors with coding schemes*/
  SetDescriptorInterface(aDescriptorInterface);
  
  /* create sub extraction tools and
     connect them with descriptor if availabe*/
  /* no sub extraction tools (see how AdvancedFaceRecognitionCS if needed)*/
  
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionExtractionTool::~AdvancedFaceRecognitionExtractionTool()
{
}

//----------------------------------------------------------------------------
bool AdvancedFaceRecognitionExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool AdvancedFaceRecognitionExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int AdvancedFaceRecognitionExtractionTool::SetSourceMedia(MultiMediaInterfaceABC *media)
{
  if (!media) return -1;

  m_Media=media;
  return 0;
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionDescriptorInterfaceABC*
AdvancedFaceRecognitionExtractionTool::
GetDescriptorInterface(void)

{
  return m_DescriptorInterface;
}


//----------------------------------------------------------------------------
int AdvancedFaceRecognitionExtractionTool::
SetDescriptorInterface(AdvancedFaceRecognitionDescriptorInterfaceABC
		       *aAdvancedFaceRecognitionDescriptorInterface)

{
  if (!aAdvancedFaceRecognitionDescriptorInterface) return -1;

  m_DescriptorInterface=aAdvancedFaceRecognitionDescriptorInterface;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long AdvancedFaceRecognitionExtractionTool::InitExtracting(void)
{
  m_FrameCnt=0;
  return 0UL;
}

//----------------------------------------------------------------------------
unsigned long AdvancedFaceRecognitionExtractionTool::StartExtracting(void)

{
  // select media type you require and remove comment
  MomVop *ImageMedia;
  //  AudioType *AudioMedia;  // type of audio media still  needs to be defined

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) return (unsigned long)-1;
  if(strcmp(m_DescriptorInterface->GetName(),
	    "AdvancedFaceRecognitionDescriptorInterface") != 0) return (unsigned long)-1;


  if (m_Media==NULL) return (unsigned long)-1;
  ImageMedia=m_Media->GetImage();
  if (ImageMedia==NULL) return (unsigned long)-1;
  // Extract the value we want. For the dummy it's just the number of frames
  // in the Sequence multiplied with the extraction parameter value
  m_FrameCnt++;

  AdvancedFRD	*pFace;
  pFace = NULL;
  pFace = new	AdvancedFRD;
  
  AdvancedFaceRecognitionFeatureGeneration(ImageMedia->y_chan->data->u,ImageMedia->width,ImageMedia->height,pFace);	
  m_DescriptorInterface->SetAdvancedFaceRecognition( pFace );
  
  delete	pFace;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long AdvancedFaceRecognitionExtractionTool::PostExtracting(void)
{
    return(0UL);
}

//----------------------------------------------------------------------------
bool AdvancedFaceRecognitionExtractionTool::
AdvancedFaceRecognitionFeatureGeneration(unsigned char* pImage_Y, unsigned long image_width,
			     unsigned long image_height, AdvancedFRD* pFace)
{
  int iw,jw;
  int fx,fy;
  int nframe;
  int nsize = 46*56;
  int numOfFourierFeature;
  int numOfCentralFourierFeature;
  int extensionFlag;
  int numOfCompositeFeature;
  int numOfCentralCompositeFeature;
  static double **pose_ave=NULL,***pose_basis=NULL;
  int ndim;
  int pos;
  long *ivec;
  long *sum_vec;

#define FACE_PARAM_FILE
#ifdef   FACE_PARAM_FILE
  static int face_param_flag = 0;
//  char FaceParamFile[1024];
#endif
  
  fx = image_width;
  fy = image_height;

  /* get parameter from Parameter File */
  numOfFourierFeature = atoi(getpara("numOfFourierFeature","24"));
  numOfCentralFourierFeature = atoi(getpara("numOfCentralFourierFeature","24"));
  extensionFlag = atoi(getpara("extensionFlag","1"));
  numOfCompositeFeature = atoi(getpara("numOfCompositeFeature","63"));
  numOfCentralCompositeFeature = atoi(getpara("numOfCentralCompositeFeature","63"));

  if (( numOfFourierFeature < 24 )||(numOfFourierFeature > 63))
    fprintf(stderr,"Error: numOfFourierFeature(%d) should be in the range [24,63]\n",
	    numOfFourierFeature);
  if (( numOfCentralFourierFeature < 24 )||(numOfCentralFourierFeature > 63))
    fprintf(stderr,"Error: numOfCentralFourierFeature(%d) should be in the range [24,63]\n",
	    numOfFourierFeature);
  if ( extensionFlag != 0 ){
    if (( numOfCompositeFeature < 0 )||(numOfCompositeFeature > 63))
      fprintf(stderr,"Error: numOfCompositeFeature(%d) should be in the range [0,63]\n",
	      numOfCompositeFeature);
    if (( numOfCentralCompositeFeature < 0 )||(numOfCentralCompositeFeature > 63))
      fprintf(stderr,"Error: numOfCentralCompositeFeature(%d) should be in the range [0,63]\n",
	      numOfCentralCompositeFeature);
  }

#ifdef FACE_PARAM_FILE
  /* set weighting parameters from file */
  if (( testpara("FaceParamFile") == 1 )&&(face_param_flag == 0 )){
    mf_read_FaceParamFile(getpara("FaceParamFile","AdvancedFaceRecognition.prm"));
    face_param_flag = 1;
  }
#endif

  /* set the number of dimension of arrays and allocate memory */
  ndim =  numOfFourierFeature + numOfCentralFourierFeature +
    numOfCompositeFeature + numOfCentralCompositeFeature;
  
  ivec = new long[ndim];
  sum_vec = new long[ndim];

  /* load pose compensation parameters */
  if ( pose_ave == NULL ){
    mf_set_pose_param2(&pose_ave,&pose_basis);
  }

  /* set the number of multiframes */
  nframe = fx*fy/nsize;
  if ((fx*fy)%nsize != 0 ) return false;
  
  if ( nframe > 1 ) 
    memset(sum_vec,0,sizeof(long)*ndim);
  
  /* extract AdvancedFaceRecognition Descriptor */
  for ( jw = 0 ; jw < nframe ; jw++ ){
    mf_extract_AdvancedFaceRecognition
      (&pImage_Y[nsize*jw],ivec,
       numOfFourierFeature,numOfCentralFourierFeature,
       extensionFlag,
       numOfCompositeFeature,numOfCentralCompositeFeature,pose_ave,pose_basis);
    if ( nframe > 1 )
      for ( iw = 0 ; iw < ndim ; iw++ ) sum_vec[iw] +=ivec[iw];
  }
  
  if ( nframe > 1 )
    for ( iw = 0 ; iw < ndim ; iw++ ) ivec[iw] = sum_vec[iw]/nframe;
  
  /* set AdvancedFaceRecognition Descriptor */
  pFace->numOfFourierFeature = numOfFourierFeature;
  pFace->numOfCentralFourierFeature = numOfCentralFourierFeature;
  pFace->extensionFlag = extensionFlag;
  pos = 0;
  for ( jw = 0 ; jw < numOfFourierFeature ; jw++ )
    pFace->FourierFeature[jw]   = ivec[pos+jw];
  pos += numOfFourierFeature;
  
  for ( jw = 0 ; jw < numOfCentralFourierFeature ; jw++ )
    pFace->CentralFourierFeature[jw]    = ivec[pos+jw];
  pos += numOfCentralFourierFeature;
  
  if ( extensionFlag ){
    pFace->numOfCompositeFeature = numOfCompositeFeature;
    pFace->numOfCentralCompositeFeature = numOfCentralCompositeFeature;
    for ( jw = 0 ; jw < numOfCompositeFeature ; jw++ )
      pFace->CompositeFeature[jw]  = ivec[pos+jw];
    pos += numOfCompositeFeature;
    
    for ( jw = 0 ; jw < numOfCentralCompositeFeature ; jw++ )
      pFace->CentralCompositeFeature[jw]   = ivec[pos+jw];
  }

  delete [] ivec;
  delete [] sum_vec;
  
  return true;
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
const UUID& AdvancedFaceRecognitionExtractionTool::GetObjectID(void)
{ return myID; }

//---------------------------------------------------------------------------
const char *AdvancedFaceRecognitionExtractionTool::GetName(void)
{ return myName; }

//---------------------------------------------------------------------------
AdvancedFaceRecognitionExtractionInterfaceABC *AdvancedFaceRecognitionExtractionTool::GetInterface(void)
{ return &m_Interface; }

//---------------------------------------------------------------------------
#ifdef FACE_PARAM_FILE
int AdvancedFaceRecognitionExtractionTool::mf_read_param1D( FILE *fp, void *buffer )
{
  int nitem;
  int size;
  fread(&nitem,sizeof(int),1,fp);
  fread(&size,sizeof(int),1,fp);
  fread(buffer,size,nitem,fp);

  return 0;
}

//---------------------------------------------------------------------------
int AdvancedFaceRecognitionExtractionTool::mf_read_param2D( FILE *fp, void *buffer )
{
  int dy,dx;
  int size;

  fread(&dy,sizeof(int),1,fp);
  fread(&dx,sizeof(int),1,fp);
  fread(&size,sizeof(int),1,fp);
  fread(buffer,size,dx*dy,fp);

  return 0;
}

//---------------------------------------------------------------------------
int AdvancedFaceRecognitionExtractionTool::mf_read_FaceParamFile( char * path )
{
  FILE *fp;
  double dummy[1024];
  /* fprintf(stderr,"mf_read_FaceParamFile(): %s\n",path); */
  fp = fopen(path,"r");
  if ( fp == NULL ){
    fprintf(stderr,"error: fopen(\"%s\",\"r\")\n",path);
    exit(1);
  }
  mf_read_param2D(fp,(void *)HolisticDiscriminantMatrix);
  mf_read_param2D(fp,(void *)HolisticDiscriminantMatrixPW);
  mf_read_param2D(fp,(void *)HolisticDiscriminantMatrixRI);
  mf_read_param1D(fp,(void *)HolisticMeanVectorPW);
  mf_read_param1D(fp,(void *)HolisticMeanVectorRI);

  mf_read_param2D(fp,(void *)CentralDiscriminantMatrix);
  mf_read_param2D(fp,(void *)CentralDiscriminantMatrixPW);
  mf_read_param2D(fp,(void *)CentralDiscriminantMatrixRI);
  mf_read_param1D(fp,(void *)CentralMeanVectorPW);
  mf_read_param1D(fp,(void *)CentralMeanVectorRI);

  mf_read_param1D(fp,(void *)dummy);
  mf_read_param1D(fp,(void *)dummy);

  mf_read_param2D(fp,(void *)PCABasis1);  mf_read_param1D(fp,(void *)PCAMean1);
  mf_read_param2D(fp,(void *)PCABasis2);  mf_read_param1D(fp,(void *)PCAMean2);
  mf_read_param2D(fp,(void *)PCABasis3);  mf_read_param1D(fp,(void *)PCAMean3);
  mf_read_param2D(fp,(void *)PCABasis4);  mf_read_param1D(fp,(void *)PCAMean4);
  mf_read_param2D(fp,(void *)PCABasis5);  mf_read_param1D(fp,(void *)PCAMean5);
  mf_read_param2D(fp,(void *)PCABasis6);  mf_read_param1D(fp,(void *)PCAMean6);
  mf_read_param2D(fp,(void *)PCABasis7);  mf_read_param1D(fp,(void *)PCAMean7);
  mf_read_param2D(fp,(void *)PCABasis8);  mf_read_param1D(fp,(void *)PCAMean8);
  mf_read_param2D(fp,(void *)PCABasis9);  mf_read_param1D(fp,(void *)PCAMean9);

  mf_read_param2D(fp,(void *)eHolisticDiscriminantMatrix);
  mf_read_param1D(fp,(void *)eHolisticMeanVector);
  mf_read_param2D(fp,(void *)eHolisticMergeMatrix);
  
  
  mf_read_param2D(fp,(void *)eCentralDiscriminantMatrix1);
  mf_read_param2D(fp,(void *)eCentralDiscriminantMatrix2);
  mf_read_param2D(fp,(void *)eCentralDiscriminantMatrix3);
  mf_read_param2D(fp,(void *)eCentralDiscriminantMatrix4);
  mf_read_param2D(fp,(void *)eCentralDiscriminantMatrix5);
  
  mf_read_param1D(fp,(void *)eCentralMeanVector1);
  mf_read_param1D(fp,(void *)eCentralMeanVector2);
  mf_read_param1D(fp,(void *)eCentralMeanVector3);
  mf_read_param1D(fp,(void *)eCentralMeanVector4);
  mf_read_param1D(fp,(void *)eCentralMeanVector5);
  
  mf_read_param2D(fp,(void *)eCentralMergeMatrix);
  
  mf_read_param1D(fp,(void *)dummy);
  mf_read_param1D(fp,(void *)dummy);
  
  fclose(fp);

  return 0;
}

#endif

//---------------------------------------------------------------------------
/* allocate 1D array */
void *AdvancedFaceRecognitionExtractionTool::ImAlloc( const size_t size_x )
{
  return(malloc(size_x));
}
/* allocate 1D array */
int AdvancedFaceRecognitionExtractionTool::ImFree( void *ptr )
{
  free(ptr);
  return(0);
}

//---------------------------------------------------------------------------
/* allocate 2D array */
void **AdvancedFaceRecognitionExtractionTool::ImAlloc2D
( const size_t size_x, const size_t size_y )
{
  void **ptr;
  size_t iw;
  if ( size_x*size_y == 0 ) fprintf(stderr,"ImAlloc2D(%d,%d)\n",size_x,size_y);
  ptr = ( void **) malloc(size_y*sizeof(void *));
  ptr[0]= ( void *) malloc( size_x * size_y );
  for( iw = 1; iw < size_y; iw++)
    ptr[iw]= &(((char *)ptr[0])[ iw * size_x]);
  return ( ptr );
}

//---------------------------------------------------------------------------
/* free 2D array */
int AdvancedFaceRecognitionExtractionTool::ImFree2D( void **p )
{
  free( (void *)(p[0]) );
  free( (void *)p );
  return(0);
}


#define FD_POSE_WIDTH 32
#define FD_POSE_HEIGHT 32
#define FD_POSE_WIDTH2 46
#define FD_POSE_HEIGHT2 56

#define FD_PCA_NUM 27

//---------------------------------------------------------------------------
int AdvancedFaceRecognitionExtractionTool::mf_set_pose_param2
(
 double ***oAvg,  /* mean vectors(O) */
 double ****oPCA   /* pca basis matrix(O) */
 )
{
  int iw,jw,kw;
  double **Avg,***PCA;
  int npose =9 ;
  
  /** allocate memory */
  Avg = (double **)ImAlloc2D(sizeof(double)*FD_POSE_WIDTH*FD_POSE_HEIGHT,npose);
  PCA = (double ***)ImAlloc(sizeof(double **)*npose);
  for ( iw = 0 ; iw < npose ; iw++ )
    PCA[iw] = (double **)ImAlloc2D(sizeof(double)*FD_POSE_WIDTH*FD_POSE_HEIGHT,FD_PCA_NUM);
  
  /* set mean face */
  for ( jw = 0 ; jw < FD_POSE_HEIGHT ; jw++ ){
    for ( iw = 0 ; iw < FD_POSE_WIDTH; iw++ ){
      Avg[0][jw*FD_POSE_WIDTH+iw] = PCAMean1[iw*FD_POSE_HEIGHT+jw];
      Avg[1][jw*FD_POSE_WIDTH+iw] = PCAMean2[iw*FD_POSE_HEIGHT+jw];
      Avg[2][jw*FD_POSE_WIDTH+iw] = PCAMean3[iw*FD_POSE_HEIGHT+jw];
      Avg[3][jw*FD_POSE_WIDTH+iw] = PCAMean4[iw*FD_POSE_HEIGHT+jw];
      Avg[4][jw*FD_POSE_WIDTH+iw] = PCAMean5[iw*FD_POSE_HEIGHT+jw];
      Avg[5][jw*FD_POSE_WIDTH+iw] = PCAMean6[iw*FD_POSE_HEIGHT+jw];
      Avg[6][jw*FD_POSE_WIDTH+iw] = PCAMean7[iw*FD_POSE_HEIGHT+jw];
      Avg[7][jw*FD_POSE_WIDTH+iw] = PCAMean8[iw*FD_POSE_HEIGHT+jw];
      Avg[8][jw*FD_POSE_WIDTH+iw] = PCAMean9[iw*FD_POSE_HEIGHT+jw];
    }
  }

  /* set basis matrix for PCA */
  for ( kw = 0 ; kw < FD_PCA_NUM ; kw++ ){
    for (jw = 0; jw < FD_POSE_HEIGHT; jw++){
      for (iw = 0; iw < FD_POSE_WIDTH; iw++){
	PCA[0][kw][jw*FD_POSE_WIDTH+iw] = PCABasis1[kw][iw*FD_POSE_HEIGHT+jw]; 
	PCA[1][kw][jw*FD_POSE_WIDTH+iw] = PCABasis2[kw][iw*FD_POSE_HEIGHT+jw]; 
	PCA[2][kw][jw*FD_POSE_WIDTH+iw] = PCABasis3[kw][iw*FD_POSE_HEIGHT+jw]; 
	PCA[3][kw][jw*FD_POSE_WIDTH+iw] = PCABasis4[kw][iw*FD_POSE_HEIGHT+jw]; 
	PCA[4][kw][jw*FD_POSE_WIDTH+iw] = PCABasis5[kw][iw*FD_POSE_HEIGHT+jw]; 
	PCA[5][kw][jw*FD_POSE_WIDTH+iw] = PCABasis6[kw][iw*FD_POSE_HEIGHT+jw]; 
	PCA[6][kw][jw*FD_POSE_WIDTH+iw] = PCABasis7[kw][iw*FD_POSE_HEIGHT+jw]; 
	PCA[7][kw][jw*FD_POSE_WIDTH+iw] = PCABasis8[kw][iw*FD_POSE_HEIGHT+jw]; 
	PCA[8][kw][jw*FD_POSE_WIDTH+iw] = PCABasis9[kw][iw*FD_POSE_HEIGHT+jw]; 
      }
    }
  }
  
  /* set return values */
  *oAvg = Avg;
  *oPCA = PCA;
  return(0);
}

//---------------------------------------------------------------------------
/* compute DFFS */
double AdvancedFaceRecognitionExtractionTool::computeDFFS2
(
 const unsigned char **ip, 
 const double *Avg, 
 const double **PCA
 )
{
  int iw, jw; 
  double dffs;
  int fx = FD_POSE_WIDTH; 
  int fy = FD_POSE_HEIGHT; 
  double norm1,norm2;
  double df,sum;

  /* length of an original vector */
  norm1 = 0.0;
  for ( iw = 0 ; iw < fx*fy ; iw ++ ){
    df = (ip[0][iw]-Avg[iw]);
    norm1 += df*df;
  }

  /* length of a projected vector */
  norm2 = 0.0;
  for ( jw = 0 ; jw < FD_PCA_NUM ; jw++ ){
    sum = 0.0;
    for ( iw = 0 ; iw < fx*fy ; iw++ ){
      sum += PCA[jw][iw]*(ip[0][iw]-Avg[iw]);
    }
    norm2 += sum*sum;
  }

  /* dff = ||x||^2 - ||Phi x||^2 */
  dffs = norm1 - norm2;
  
  return(dffs); 
}

//---------------------------------------------------------------------------
/* estimate Pose Class */
int AdvancedFaceRecognitionExtractionTool::estimatePoseClass2
(
 const unsigned char **ip,  
 const double **Avg, 
 const double ***PCA
 )
{
  int iw; 
  int est_pose_id; 
  double dffs[9];
  double dffs_min;
  
  for (iw = 0; iw < 9; iw++){
    dffs[iw] = computeDFFS2 (ip, Avg[iw], PCA[iw]);
  }
  dffs_min = dffs[0];
  est_pose_id = 0;
  for ( iw = 1; iw < 9 ; iw++ ){
    if ( dffs[iw] < dffs_min ){
      dffs_min = dffs[iw];
      est_pose_id = iw;
    }
  }

  return(est_pose_id); 
}

//---------------------------------------------------------------------------
int AdvancedFaceRecognitionExtractionTool::assignPoseClass(char *string)
{
  
  if	(!strcmp (string, "U")) return 0; 
  else if (!strcmp (string, "FLU")) return 1; 
  else if (!strcmp (string, "FU")) return 1; 
  else if (!strcmp (string, "FRU")) return 1;
  else if (!strcmp (string, "L")) return 2;
  else if (!strcmp (string, "FL")) return 3;
  else if (!strcmp (string, "F")) return 4;
  else if (!strcmp (string, "FR")) return 5;
  else if (!strcmp (string, "R")) return 6; 
  else if (!strcmp (string, "FLD")) return 7; 
  else if (!strcmp (string, "FD")) return 7; 
  else if (!strcmp (string, "FRD")) return 7; 
  else if (!strcmp (string, "D")) return 8;
  else
    {
      printf ("Assign Failure...\n"); 
      exit (0); 
    }
}

//---------------------------------------------------------------------------
void AdvancedFaceRecognitionExtractionTool::warpingAffine2
(
 const unsigned char **src,   /* input image(I)     */
 unsigned char **dst,         /* ouput image(O)     */
 const int pose               /* pose identifier(I) */
 )
{
  int width = FD_POSE_WIDTH2; 
  int height = FD_POSE_HEIGHT2; 
  double a, b, c, d, e, f; 
  double fx, fy, dx, dy;
  int x, y; 
  int i,j ; 
  double fpixel; 
  int pixel; 
  const double AffineMatrix[9][6] = {
    {0.991580,0.010128,0.074633,-0.003959,0.943660,1.515700},
    {0.970768,-0.002972,0.881806,0.003942,0.887513,2.570872},
    {1.037537,0.119828,-4.445840,-0.008323,1.242277,-5.354795},
    {0.996640,-0.073498,1.742925,0.004347,1.033041,-0.724001},
    {1.000000,0.000000,0.000000,0.000000,1.000000,0.000000},
    {0.987388,0.086766,-1.807056,-0.003484,0.998318,0.264492},
    {0.999838,-0.128101,3.728913,0.013586,1.185747,-4.659860},
    {0.984965,-0.000953,0.423803,0.002269,0.970589,0.884239},
    {0.978864,-0.003004,0.538113,0.011342,1.001916,-0.477181}
  };

  a = AffineMatrix[pose][0];
  b = AffineMatrix[pose][1];
  c = AffineMatrix[pose][2];
  d = AffineMatrix[pose][3];
  e = AffineMatrix[pose][4];
  f = AffineMatrix[pose][5];

  if (pose == assignPoseClass("F")) {
    for (j = 0; j < height; j++) {
      for (i = 0; i < width; i++) {
	dst[j][i] = src[j][i]; 
      }
    }
  }
  else {
    for (j = 0; j < height; j++) {
      for (i = 0; i < width; i++) {
	fx = a * (double)i + b * (double)j + c;
	fy = d * (double)i + e * (double)j + f;
	x = (int)floor(fx);
	y = (int)floor(fy);
	dx = fx - (double) x;
	dy = fy - (double) y;
	if (x >= 0 && x < width-1 && y >= 0 && y < height-1) {
	  fpixel = (1-dy)*((1-dx)*(double)src[y][x] + dx*(double)src[y][x+1])
	    + dy*((1-dx)*(double)src[y+1][x] + dx*(double)src[y+1][x+1]);
	  pixel = (int)floor(fpixel);
	  if (pixel < 0) dst[j][i] = 0;
	  else if (pixel >= 255) dst[j][i] = 255;
	  else dst[j][i] = pixel;
	}
	else dst[j][i] = 0; 
      }
    }
  }
}

//---------------------------------------------------------------------------
/** pose compensation */
int AdvancedFaceRecognitionExtractionTool::mf_pose_compensate2
(
 const unsigned char **ip,  /* input image  */
 unsigned char **jp,        /* output image */
 const int fx,              /* image size   */
 const int fy,
 const double **Avg,        /* mean faces */
 const double ***PCA        /* eigenfaces */
 )
{
  int est_pose_id; 
  unsigned char **ip2;

  ip2 = (unsigned char **)ImAlloc2D(sizeof(unsigned char)*32,32);
  mf_clipuc((const unsigned char **)ip,46,56,ip2,32,32,7,12);
  
  /* DFFS routine */
  est_pose_id = estimatePoseClass2 ((const unsigned char **)ip2, Avg, PCA);
  
  /* My image warping routine */
  warpingAffine2(ip,jp,est_pose_id); 

  ImFree2D((void **)ip2);
  
  return(0);
}

//---------------------------------------------------------------------------
/* clip image */
int AdvancedFaceRecognitionExtractionTool::mf_clipd
(
 const double **ip,    /* input image(I) */
 const int ix,        /* image size */
 const int iy,
 double **jp,          /* clipped image(O) */
 const int jx,        /* image size */
 const int jy,
 const int start_x,   /* starting point */
 const int start_y
 )
{
  int kx,ky;
  int px,py;

  for ( ky = 0 ; ky < jy ; ky++ ){
    for ( kx = 0 ; kx < jx ; kx++ ){
      
      px = start_x + kx;
      if ( px < 0 ) px = 0;
      else if ( px >= ix ) px = ix-1;
      
      py = start_y + ky;
      if ( py < 0 ) py = 0 ;
      else if ( py >= iy ) py = iy-1;
      
      jp[ky][kx] = ip[py][px];
    }
  }
  return(0);
}

//---------------------------------------------------------------------------
/* clip image */
int AdvancedFaceRecognitionExtractionTool::mf_clipuc
(
 const unsigned char **ip,   /* input image(I) */
 const int ix,               /* image size */
 const int iy,
 unsigned char **jp,  /* clipped image(O) */
 const int jx,        /* image size */
 const int jy,
 const int start_x,   /* starting point */
 const int start_y
 )
{
  int kx,ky;
  int px,py;

  for ( ky = 0 ; ky < jy ; ky++ ){
    for ( kx = 0 ; kx < jx ; kx++ ){
      
      px = start_x + kx;
      if ( px < 0 ) px = 0;
      else if ( px >= ix ) px = ix-1;
      
      py = start_y + ky;
      if ( py < 0 ) py = 0 ;
      else if ( py >= iy ) py = iy-1;
      
      jp[ky][kx] = ip[py][px];
    }
  }
  return(0);
}


//---------------------------------------------------------------------------
/* Fourier transform */
int AdvancedFaceRecognitionExtractionTool::mf_fourier
(
 const double **ip,  /* input image(I)    */
 double **r_re,      /* real part(O)      */
 double **r_im,      /* imaginary part(O) */
 double **r_pw,      /* power (O)         */
 const int fx,      /* image size(I)     */
 const int fy       /* image size(I)     */
 )
{
  int iw,jw,kw;
  double **re,**im;
  double **csx,**snx;
  double **csy,**sny;
  double pi = acos(-1.0);
  double re_sum = 0.0;
  double im_sum = 0.0;
  
  /* allocate memory */
  re = (double **)ImAlloc2D(sizeof(double)*fx,fy);
  im = (double **)ImAlloc2D(sizeof(double)*fx,fy);
  csx = (double **)ImAlloc2D(sizeof(double)*fx,fx);
  snx = (double **)ImAlloc2D(sizeof(double)*fx,fx);
  csy = (double **)ImAlloc2D(sizeof(double)*fy,fy);
  sny = (double **)ImAlloc2D(sizeof(double)*fy,fy);
  
  /* horizontal Fourier transform */
  for ( iw = 0; iw < fx ; iw++ ){
    for ( kw = 0 ; kw < fx ; kw++ ){
      csx[iw][kw] = cos(-2.0*pi*iw*kw/fx);
      snx[iw][kw] = sin(-2.0*pi*iw*kw/fx);
    }
  }
  for ( jw = 0 ; jw < fy ; jw++ ){
    for ( iw = 0 ; iw < fx ; iw++ ){
      re_sum = 0.0;
      im_sum = 0.0;
      for ( kw = 0 ; kw < fx ; kw++ ){
	re_sum += ip[jw][kw]*csx[iw][kw];
	im_sum += ip[jw][kw]*snx[iw][kw];
      }
      re[jw][iw] = re_sum;
      im[jw][iw] = im_sum;
    }
  }
  
  /* vertical Fourier transform */
  for ( iw = 0; iw < fy ; iw++ ){
    for ( kw = 0 ; kw < fy ; kw++ ){
      csy[iw][kw] = cos(-2.0*pi*iw*kw/fy);
      sny[iw][kw] = sin(-2.0*pi*iw*kw/fy);
    }
  }
  for ( iw = 0 ; iw < fx ; iw++ ){
    for ( jw = 0 ; jw < fy ; jw++ ){
      re_sum = 0.0;
      im_sum = 0.0;
      for ( kw = 0 ; kw < fy ; kw++ ){
	re_sum += re[kw][iw]*csy[jw][kw]-im[kw][iw]*sny[jw][kw];
	im_sum += re[kw][iw]*sny[jw][kw]+im[kw][iw]*csy[jw][kw];
      }
      r_re[jw][iw] = re_sum;
      r_im[jw][iw] = im_sum;
      r_pw[jw][iw] = sqrt(re_sum*re_sum+im_sum*im_sum);
    }
  }
  
  /* free memory */
  ImFree2D((void **)re);
  ImFree2D((void **)im);
  
  ImFree2D((void **)csx);
  ImFree2D((void **)snx);
  ImFree2D((void **)csy);
  ImFree2D((void **)sny);
  
  return(0);
}

//---------------------------------------------------------------------------
/* convert Fourier spectra into 1D-vector by raster scanning */
int AdvancedFaceRecognitionExtractionTool::mf_convert_vector
(
 const double **ip,  /* input image(I) */
 double *vec,        /* feature vector */
 const int fx,      /* input image size */
 const int fy,
 const int mode     /*   0 : pixel values
		     *   1 : fourier components(real and imaginary)
		     *   2 : fourier power
		     */
 )
{
  int iw,jw;
  double **re,**im,**pw;
  int hx,hy; 
  int hhx,hhy;
  
  if ( mode == 0 ){
    for ( jw = 0 ; jw < fy ; jw++ )
      for ( iw = 0 ; iw < fx ; iw++ ){
	vec[jw*fx+iw] = ip[jw][iw];
      }
  }
  else{

    /* allocate memory */
    re = (double **)ImAlloc2D(sizeof(double)*fx,fy);
    im = (double **)ImAlloc2D(sizeof(double)*fx,fy);
    pw = (double **)ImAlloc2D(sizeof(double)*fx,fy);
  
    /* Fourier Transform */
    mf_fourier((const double **)ip,re,im,pw,fx,fy);

    hx = fx/2; hhx = fx/4;
    hy = fy/2; hhy = fy/4;
    for ( jw = 0 ; jw < hhy ; jw++ ){
      for ( iw = 0 ; iw < hhx ; iw++ ){
	/* real and imaginary */
	if ( mode == 1 ){
	  vec[jw*hx+iw]              = re[jw][iw];
	  vec[jw*hx+(hx-1-iw)]       = re[jw][fx-1-iw];
	  vec[(hhy+jw)*hx+iw]        = im[jw][iw];
	  vec[(hhy+jw)*hx+(hx-1-iw)] = im[jw][fx-1-iw];
	}
	/* power spectrum */
	else if ( mode == 2 ){
	  vec[jw*hx+iw]             = pw[jw][iw];
	  vec[jw*hx+(hx-1-iw)]      = pw[jw][fx-1-iw];
	}
      }
      if ( hx%2 == 1 ){
	if ( mode == 1 ){
	  vec[     jw *hx+hhx] = re[jw][hhx];
	  vec[(hhy+jw)*hx+hhx] = im[jw][hhx];
	}
	else if ( mode == 2 ){
	  vec[     jw *hx+hhx] = pw[jw][hhx];
	}
      }
    }
    
    /* free memory */
    ImFree2D((void **)re);
    ImFree2D((void **)im);
    ImFree2D((void **)pw);
  }
  return(0);
}

//---------------------------------------------------------------------------
/* extract feature components */
int AdvancedFaceRecognitionExtractionTool::mf_extract_feature
(
 const double **ip,  /* input vector */
 const int fx,       /* image size */
 const int fy,
 const int ssx,      /* starting point of clipping */
 const int ssy,
 const int szx,      /* size of clipping region */
 const int szy,
 double *vec,        /* output: feature vector */
 const int ndim,     /* the number of dimension */
 const int nres,     /* multiple resolution */
 const int mode,     /* 0: intensity, 1: Fouier spectrum(Re and Im) , 2: Fourier amplitude */
 const int half_flag /* half resolution */
 )
{
  double **cip;
  int res;
  int sx,sy;
  int nx,ny;
  int px,py;
  double *pvec;
  int r2[] = {1,2,4};
  int nsize;
  
  pvec = vec;
  for ( res = 0 ; res < nres ; res++ ){
    sx = szx/r2[res];    
    sy = szy/r2[res];
    cip = (double **)ImAlloc2D(sizeof(double)*sx,sy);
    
    nsize = sx*sy;
    if ( half_flag == 1 ) nsize = (sx/2)*(sy/4)*2;
    if ( mode == 2 ) nsize /= 2;
    
    for ( ny = 0 ; ny < r2[res] ; ny++ ){
      for ( nx = 0 ; nx < r2[res] ; nx++ ){
	px = nx*sx;
	py = ny*sy;
	
	/* clip central part of original image */
	mf_clipd((const double **)ip,fx,fy,cip,sx,sy,px+ssx,py+ssy);
	
	/* convert input image into original feature vector */
	mf_convert_vector((const double **)cip,pvec,sx,sy,mode);
	pvec += nsize;
      }
    }
    ImFree2D((void **)cip);
  }
  return(0);
}

//---------------------------------------------------------------------------
/* project input vector using basis matrix */
int AdvancedFaceRecognitionExtractionTool::mf_project_vector
( 
 const double *ivec,  /* input vector */
 const int idim,      /* the number of dimension */
 double *ovec,        /* output vector */
 const int odim,      /* the number of dimension */
 const double *matrix /* basis matrix[odim*idim] */
 )
{
  int iw,jw;
  double sum;

  for ( jw = 0 ; jw < odim ; jw++ ){
    sum = 0.0;
    for ( iw = 0 ; iw < idim ; iw++ ){
      sum += ivec[iw]*matrix[jw*idim+iw];
    }
    ovec[jw] = sum;
  }
  
  return(0);
}

//---------------------------------------------------------------------------
/* normalize vector to unit vector */
int AdvancedFaceRecognitionExtractionTool::mf_unitvector
(
 const double *ip,  /* input vector(I) */
 double *jp,        /* output unit vector(O) */
 const int size    /* number of dimension(I) */
 )
{
  int iw;
  double len;
  
  len = 0.0;
  for ( iw = 0 ; iw < size ; iw++ )
    len += ip[iw]*ip[iw];
  
  len = sqrt(len);
  if ( len == 0.0 ){
    len = 1.0;
    fprintf(stderr,"mf_unitvectorf() : length error : len = %f\n",len);
  }
  else{
    len = 1.0/len;
  }
  for ( iw = 0 ; iw < size ; iw++ ){
    jp[iw] = ip[iw]*len;
  }
  
  return(0);
}

//---------------------------------------------------------------------------
/* normalize into a unit vector */
int AdvancedFaceRecognitionExtractionTool::mf_normalize_vector
(
 const double *ivec,  /* input vector(I) */
 double *ovec,        /* output vector(O) */
 const int ndim,      /* the number of dimension(I) */
 const double *mean   /* mean vector(I) */
 )
{
  int iw;
  
  for ( iw = 0 ; iw < ndim ; iw++ )
    ovec[iw] = ivec[iw]-mean[iw];
  mf_unitvector(ovec,ovec,ndim);
  
  return(0);
}

//---------------------------------------------------------------------------
/* extract face descriptor(Awaji Version) */
int AdvancedFaceRecognitionExtractionTool::mf_extract_AdvancedFaceRecognition
(
 const unsigned char *ip,  /* input image(I) */
 long  *ivec,               /* face descriptor ivec[hdim+cdim+ehdim+ecdim] */
 const int hfdim,            /* numOfFourierFeature */
 const int cfdim,            /* numOfCentralFourierFeature */
 const int extensionFlag,   /* extentionFlag */
 const int hcdim,           /* numOfCompositeFeature */
 const int ccdim,           /* numOfCentralCompositeFeature */
 double **Avg,
 double ***PCA
 )
{
  int iw,jw;
  double **im;           /* input image in floating point */
  int fx = 46, fy = 56; /* image size */
  int hx = 44, hy = 56; 
  int cx = 32, cy = 32;
  double hvec_ri[644],hvec_pw[856],hvec[150+ 40];  /* input features and intermediate features */
  double cvec_ri[256],cvec_pw[384],cvec[150+200];
  double *fvec;      /* output features in floating ponit*/
  int iwork;

  /* allocate memory */  
  fvec = (double *)ImAlloc(sizeof(double)*(hfdim+cfdim+hcdim+ccdim));
  im  = (double **)ImAlloc2D(sizeof(double)*fx,fy);
  
  /* convert unsigned char to floating */
  for ( jw = 0 ; jw < fy ; jw++ )
    for ( iw = 0 ; iw < fx ; iw++ )
      im[jw][iw] = (double)ip[jw*fx+iw];
  
  /* extract fourier features */
  mf_extract_feature((const double **)im,fx,fy,0, 0,fx,fy,hvec_ri,644,1,1,1);
  mf_extract_feature((const double **)im,fx,fy,1, 0,hx,hy,hvec_pw,856,3,2,1);
  
  mf_extract_feature((const double **)im,fx,fy,7,12,cx,cy,cvec_ri,256,1,1,1);
  mf_extract_feature((const double **)im,fx,fy,7,12,cx,cy,cvec_pw,384,3,2,1);
  
  /* project onto PCLDA subpace */
  mf_project_vector(hvec_ri,644,&hvec[ 0], 70,(const double *)HolisticDiscriminantMatrixRI);
  mf_project_vector(hvec_pw,856,&hvec[70], 80,(const double *)HolisticDiscriminantMatrixPW);

  mf_project_vector(cvec_ri,256,&cvec[ 0], 70,(const double *)CentralDiscriminantMatrixRI);
  mf_project_vector(cvec_pw,384,&cvec[70], 80,(const double *)CentralDiscriminantMatrixPW);

  /* normalize into unit vectors */
  mf_normalize_vector(&hvec[ 0],&hvec[ 0],70,HolisticMeanVectorRI);
  mf_normalize_vector(&hvec[70],&hvec[70],80,HolisticMeanVectorPW);
  mf_normalize_vector(&cvec[ 0],&cvec[ 0],70,CentralMeanVectorRI);
  mf_normalize_vector(&cvec[70],&cvec[70],80,CentralMeanVectorPW);

  /* project onto 2nd LDA subspace */
  mf_project_vector(hvec,150,&fvec[   0],hfdim,(const double *)HolisticDiscriminantMatrix);
  mf_project_vector(cvec,150,&fvec[hfdim],cfdim,(const double *)CentralDiscriminantMatrix);

  /* quantization */
  for ( iw = 0 ; iw < hfdim+cfdim ; iw++ ){
    iwork = (int)floor(fvec[iw]+16.0);
    if ( iwork < 0 )       ivec[iw] = 0;
    else if ( iwork > 31 ) ivec[iw] = 31;
    else                   ivec[iw] = iwork;
  }

  /* optional part */
  if ( extensionFlag != 0 ){
    double comp0[2576],comp1[783],comp2[504],comp3[504],comp4[576],comp5[576];
    unsigned char **ip2;
    unsigned char **cwork;

    /* allocate memory */
    ip2  = (unsigned char **)ImAlloc2D(sizeof(unsigned char)*fx,fy);
    cwork  = (unsigned char **)ImAlloc2D(sizeof(unsigned char)*fx,fy);

    memcpy(&ip2[0][0],&ip[0],sizeof(unsigned char)*fx*fy);
    
    /* pose compensation */
    mf_pose_compensate2((const unsigned char **)ip2,cwork,fx,fy,(const double **)Avg,(const double ***)PCA);
    
    /* convert unsigned char to floating */
    for ( jw = 0 ; jw < fy ; jw++ )
      for ( iw = 0 ; iw < fx ; iw++ )
	im[jw][iw] = (double)cwork[jw][iw];
    
    /* extract intensity features */
    mf_extract_feature((const double **)im,fx,fy, 0, 0,fx,fy,comp0,2576,1,0,0);
    mf_extract_feature((const double **)im,fx,fy, 9, 4,29,27,comp1, 783,1,0,0);
    mf_extract_feature((const double **)im,fx,fy, 6,16,24,21,comp2, 504,1,0,0);
    mf_extract_feature((const double **)im,fx,fy,17,16,24,21,comp3, 504,1,0,0);
    mf_extract_feature((const double **)im,fx,fy, 7,25,24,24,comp4, 576,1,0,0);
    mf_extract_feature((const double **)im,fx,fy,16,25,24,24,comp5, 576,1,0,0);

    /* project onto PCLDA subspace */
    mf_project_vector(comp0,2576,&hvec[150], 40,(const double *)eHolisticDiscriminantMatrix);
    
    mf_project_vector(comp1, 783,&cvec[150], 40,(const double *)eCentralDiscriminantMatrix1);
    mf_project_vector(comp2, 504,&cvec[190], 40,(const double *)eCentralDiscriminantMatrix2);
    mf_project_vector(comp3, 504,&cvec[230], 40,(const double *)eCentralDiscriminantMatrix3);
    mf_project_vector(comp4, 576,&cvec[270], 40,(const double *)eCentralDiscriminantMatrix4);
    mf_project_vector(comp5, 576,&cvec[310], 40,(const double *)eCentralDiscriminantMatrix5);
    
    /* normalize into unit vectors */
    mf_normalize_vector(&hvec[150],&hvec[150],40,eHolisticMeanVector);

    mf_normalize_vector(&cvec[150],&cvec[150],40,eCentralMeanVector1);
    mf_normalize_vector(&cvec[190],&cvec[190],40,eCentralMeanVector2);
    mf_normalize_vector(&cvec[230],&cvec[230],40,eCentralMeanVector3);
    mf_normalize_vector(&cvec[270],&cvec[270],40,eCentralMeanVector4);
    mf_normalize_vector(&cvec[310],&cvec[310],40,eCentralMeanVector5);
    
    /* project onto 2nd LDA subspace */
    mf_project_vector(hvec,190,&fvec[hfdim+cfdim      ],hcdim,(const double *)eHolisticMergeMatrix);
    mf_project_vector(cvec,350,&fvec[hfdim+cfdim+hcdim],ccdim,(const double *)eCentralMergeMatrix);
    
    /* quantization */
    for ( iw = (hfdim+cfdim) ; iw < (hfdim+cfdim)+(hcdim+ccdim) ; iw++ ){
      iwork = (int)floor(fvec[iw]+16.0);
      if ( iwork < 0 )       ivec[iw] = 0;
      else if ( iwork > 31 ) ivec[iw] = 31;
      else                   ivec[iw] = iwork;
    }
    
    ImFree2D((void **)ip2);
    ImFree2D((void **)cwork);
  }
  
  /* free memory */
  ImFree((void *)fvec);
  ImFree2D((void **)im);
  
  return(0);
}

