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

#ifndef __AdvancedFaceRecognitionExtraction_H__
#define __AdvancedFaceRecognitionExtraction_H__

#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"
#include "Utilities/Utilities.h"

//=============================================================================

namespace XM
{

// Forward Declarations:
class AdvancedFaceRecognitionExtractionTool;

//=============================================================================
class AdvancedFaceRecognitionExtractionInterfaceABC: public I_InterfaceABC
{
public:
  AdvancedFaceRecognitionExtractionInterfaceABC();
  virtual ~AdvancedFaceRecognitionExtractionInterfaceABC() {};
  
  virtual void destroy()=0;
  
  // This informs the extractor where the source data will come from
  virtual int SetSourceMedia(MultiMediaInterfaceABC* media) = 0;

  // This is used to inform the top level module where the results
  // are written to
  virtual AdvancedFaceRecognitionDescriptorInterfaceABC
  *GetDescriptorInterface(void) = 0;
  // This informs the extraction were to write the results
  virtual int SetDescriptorInterface(AdvancedFaceRecognitionDescriptorInterfaceABC
				     *aAdvancedFaceRecognitionDescriptorInterface) = 0;
  
  // This is executed before frame based /time point) extraction, e.g.,
  // reset description or read extraction parameters
  virtual unsigned long InitExtracting(void) = 0;
  // This is the per frame (time point/ period) extraction
  virtual unsigned long StartExtracting(void) = 0;
  // This is executed after the frame based (time point) extraction
  virtual unsigned long PostExtracting(void) = 0;

	static const UUID myID;
};

//=============================================================================
class AdvancedFaceRecognitionExtractionInterface: 
  public AdvancedFaceRecognitionExtractionInterfaceABC
{
public:
  AdvancedFaceRecognitionExtractionInterface(AdvancedFaceRecognitionExtractionTool* aTool);
  virtual ~AdvancedFaceRecognitionExtractionInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);
  
  virtual void destroy();
  
  virtual int SetSourceMedia(MultiMediaInterfaceABC* media);
  virtual AdvancedFaceRecognitionDescriptorInterfaceABC
  *GetDescriptorInterface(void);
  virtual int
  SetDescriptorInterface(AdvancedFaceRecognitionDescriptorInterfaceABC
			 *aAdvancedFaceRecognitionDescriptorInterface);
  virtual unsigned long InitExtracting(void);
  virtual unsigned long StartExtracting(void);
  virtual unsigned long PostExtracting(void);
  
  static const UUID myID;
  static const char *myName;

private:
  AdvancedFaceRecognitionExtractionTool *m_ExtractionTool;
};

//=============================================================================
class AdvancedFaceRecognitionExtractionTool: public DescriptorExtractor
{
friend class AdvancedFaceRecognitionExtractionInterface;
public:
  
  AdvancedFaceRecognitionExtractionTool();
  AdvancedFaceRecognitionExtractionTool(AdvancedFaceRecognitionDescriptorInterfaceABC *aDescriptorInterface);
#define FACE_PARAM_FILE
#ifdef FACE_PARAM_FILE
  int mf_read_param1D( FILE *fp, void *buffer );
  int mf_read_param2D( FILE *fp, void *buffer );
  int mf_read_FaceParamFile( char * path );
#endif 

  void *ImAlloc( const size_t size_x );
  void **ImAlloc2D( const size_t size_x, const size_t size_y );
  int ImFree( void *ptr );
  int ImFree2D( void **p );
  int mf_set_pose_param2
  (
   double ***oAvg,  /* mean vectors(O) */
   double ****oPCA   /* pca basis matrix(O) */
   );
  /* compute DFFS */
  double computeDFFS2
  (
   const unsigned char **ip, 
   const double *Avg, 
   const double **PCA
   );
  
  /* estimate Pose Class */
  int estimatePoseClass2
  (
   const unsigned char **ip,  
   const double **Avg, 
   const double ***PCA
   );
  int assignPoseClass(char *string);
  
  void warpingAffine2
  (
   const unsigned char **src,   /* input image(I)     */
   unsigned char **dst,         /* ouput image(O)     */
   const int pose               /* pose identifier(I) */
   );
  
  /** pose compensation */
  int mf_pose_compensate2
  (
   const unsigned char **ip,  /* input image  */
   unsigned char **jp,        /* output image */
   const int fx,              /* image size   */
   const int fy,
   const double **Avg,        /* mean faces */
   const double ***PCA        /* eigenfaces */
   );

  /* clip image */
  int mf_clipd
  (
   const double **ip,    /* input image(I) */
   const int ix,        /* image size */
   const int iy,
   double **jp,          /* clipped image(O) */
   const int jx,        /* image size */
   const int jy,
   const int start_x,   /* starting point */
   const int start_y
   );
  
  /* clip image */
  int mf_clipuc
  (
   const unsigned char **ip,   /* input image(I) */
   const int ix,               /* image size */
   const int iy,
   unsigned char **jp,  /* clipped image(O) */
   const int jx,        /* image size */
   const int jy,
   const int start_x,   /* starting point */
   const int start_y
   );
  
  /* Fourier transform */
  int mf_fourier
  (
   const double **ip,  /* input image(I)    */
   double **r_re,      /* real part(O)      */
   double **r_im,      /* imaginary part(O) */
   double **r_pw,      /* power (O)         */
   const int fx,      /* image size(I)     */
   const int fy       /* image size(I)     */
   );
  
  /* convert Fourier spectra into 1D-vector by raster scanning */
  int mf_convert_vector
  (
   const double **ip,  /* input image(I) */
   double *vec,        /* feature vector */
   const int fx,      /* input image size */
   const int fy,
   const int mode     /*   0 : pixel values
		       *   1 : fourier components(real and imaginary)
		       *   2 : fourier power
		       */
   );
  
  /* extract feature components */
  int mf_extract_feature
  (
   const double **ip,  /* input vector */
   const int fx,       /* image size */
   const int fy,
   const int ssx,      /* shift of clipping point */
   const int ssy,
   const int szx,      /* size of clipping region */
   const int szy,
   double *vec,        /* output: power spectrum vector */
   const int ndim,     /* the number of dimension */
   const int nres,     /* multiple resolution */
   const int mode,     /* 0: intensity, 1: real and imaginary , 2: power spectrum */
   const int half_flag /* half resolution */
   );
  
  /* project input vector using basis matrix */
  int mf_project_vector
  ( 
   const double *ivec,  /* input vector */
   const int idim,     /* number of dimension */
   double *ovec,        /* output vector */
   const int odim,     /* number of dimension */
   const double *matrix /* basis matrix[odim*idim] */
   );
  
  /* normalize vector to unit vector */
  int mf_unitvector
  (
   const double *ip,  /* input vector(I) */
   double *jp,        /* output unit vector(O) */
   const int size    /* number of dimension(I) */
   );
  
  /* normalize into a unit vector */
  int mf_normalize_vector
  (
   const double *ivec,  /* input vector(I) */
   double *ovec,        /* output vector(O) */
   const int ndim,     /* number of dimension */
   const double *mean   /* mean vector */
   );
	
  /* extract face descriptor(Shanghai Version) */
  int mf_extract_AdvancedFaceRecognition
  (
   const unsigned char *ip,  /* input image(I) */
   long  *ivec,               /*  face descriptor ivec[hdim+cdim+ehdim+ecdim] */
   const int hfdim,            /* numOfFourierFeature */
   const int cfdim,            /* numOfCentralFourierFeature */
   const int extensionFlag,   /* extentionFlag */
   const int hcdim,           /* numOfCompositeFeature */
   const int ccdim,           /* numOfCentralCompositeFeature */
   double **Avg,
   double ***PCA
   );
	
  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual bool SupportsPush(void);
  virtual bool SupportsPull(void);

  virtual int SetSourceMedia(MultiMediaInterfaceABC* media);
  virtual AdvancedFaceRecognitionDescriptorInterfaceABC
  *GetDescriptorInterface(void);
  virtual int
  SetDescriptorInterface(AdvancedFaceRecognitionDescriptorInterfaceABC
			 *aAdvancedFaceRecognitionDescriptorInterface);
	
  virtual unsigned long InitExtracting(void);
  virtual unsigned long StartExtracting(void);
  virtual unsigned long PostExtracting(void);

  virtual AdvancedFaceRecognitionExtractionInterfaceABC *GetInterface(void);

  // access is allowed only by class factories for this
  // object.  This avoids having to duplicate the
  // ID definition in multiple locations.  In the future, we may
  // have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
  // IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
  
  virtual bool AdvancedFaceRecognitionFeatureGeneration(unsigned char* pImage_Y,
							unsigned long image_width,
							unsigned long image_height,
							AdvancedFRD* pAdvancedFaceRecognition);
  static const UUID myID;
  static const char * myName;
  virtual ~AdvancedFaceRecognitionExtractionTool();
private:
  AdvancedFaceRecognitionExtractionInterface m_Interface;
  AdvancedFaceRecognitionDescriptorInterfaceABC *m_DescriptorInterface;
  MultiMediaInterfaceABC* m_Media;
  unsigned long m_FrameCnt;

  // Added for AdvancedFaceRecognition parameters
  // mandatory part
  static double HolisticDiscriminantMatrix[96][150];
  static double HolisticDiscriminantMatrixPW[80][856];
  static double HolisticDiscriminantMatrixRI[70][644];
  static double HolisticMeanVectorPW[80];
  static double HolisticMeanVectorRI[70];

  static double CentralDiscriminantMatrix[96][150];
  static double CentralDiscriminantMatrixPW[80][384];
  static double CentralDiscriminantMatrixRI[70][256];
  static double CentralMeanVectorPW[80];
  static double CentralMeanVectorRI[70];

        // extended part
  static float PCAMean1[1024];
  static float PCABasis1[27][1024];
  static float PCAMean2[1024];
  static float PCABasis2[27][1024];
  static float PCAMean3[1024];
  static float PCABasis3[27][1024];
  static float PCAMean4[1024];
  static float PCABasis4[27][1024];
  static float PCAMean5[1024];
  static float PCABasis5[27][1024];
  static float PCAMean6[1024];
  static float PCABasis6[27][1024];
  static float PCAMean7[1024];
  static float PCABasis7[27][1024];
  static float PCAMean8[1024];
  static float PCABasis8[27][1024];
  static float PCAMean9[1024];
  static float PCABasis9[27][1024];

  static double eHolisticDiscriminantMatrix[40][2576];
  static double eHolisticMeanVector[40];
  static double eHolisticMergeMatrix[96][190];

  static double eCentralDiscriminantMatrix1[40][783];
  static double eCentralDiscriminantMatrix2[40][504];
  static double eCentralDiscriminantMatrix3[40][504];
  static double eCentralDiscriminantMatrix4[40][576];
  static double eCentralDiscriminantMatrix5[40][576];
  static double eCentralMeanVector1[40];
  static double eCentralMeanVector2[40];
  static double eCentralMeanVector3[40];
  static double eCentralMeanVector4[40];
  static double eCentralMeanVector5[40];
  static double eCentralMergeMatrix[96][350];

};
};

#endif // __AdvancedFaceRecognitionExtraction_H__







