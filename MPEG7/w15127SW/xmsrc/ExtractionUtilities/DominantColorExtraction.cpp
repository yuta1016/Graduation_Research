//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// University of California Santa Barbara, Kapil Chhabra, Yining Deng.
// Mitsubishi Electric ITE-VIL, Leszek Cieplinski, LG-Elite.
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
// parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  DominantColorExtraction.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include "momusys.h"
#include "inter.h"
#include "intra.h"
#include "fifo.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/readparam.h"

#define SC_BIT	 5

#define EPSGLOB  0.01   /* global stopping criterion for distortion change */
#define EPSSPLT  0.02   /* splitting criterion for distortion change */
#define DSTMIN  255.0
#define SPLFCT   0.10
#define VARTHR   50.0
//This is now defined as a member of DominantColorDescriptor so that the binary output
//defaults to the same quantization as the extraction process!!  RJOC 04/02/05
//#define BINNUMDEF  "32" 
#define VARPRESDEF  "1"

static const double twopi=2.0*3.14159265358979323846;


void (*dominantcolor_colorspaceconversion)(TPixel*, TPixel*, TCoor, TCoor);

//=============================================================================

namespace XM {
//#define DEBUG
const double DominantColorExtractionTool::LUVX0 = (0.607+0.174+0.201);
const double DominantColorExtractionTool::LUVY0 = (0.299+0.587+0.114);
const double DominantColorExtractionTool::LUVZ0 = (      0.066+1.117);
const double DominantColorExtractionTool::LUVU20 = 4*DominantColorExtractionTool::LUVX0/(DominantColorExtractionTool::LUVX0 + 15*DominantColorExtractionTool::LUVY0 + 3*DominantColorExtractionTool::LUVZ0);
const double DominantColorExtractionTool::LUVV20 = 9*DominantColorExtractionTool::LUVY0/(DominantColorExtractionTool::LUVX0 + 15*DominantColorExtractionTool::LUVY0 + 3*DominantColorExtractionTool::LUVZ0);


const UUID DominantColorExtractionInterface::myID = UUID("");
const char *DominantColorExtractionInterface::myName =
  "Dominant Color Extraction Interface";

const UUID DominantColorExtractionTool::myID = UUID("");
const char *DominantColorExtractionTool::myName =
  "Dominant Color Descriptor Extractor";

const UUID DominantColorExtractionInterfaceABC::myID = UUID();

//=============================================================================
//----------------------------------------------------------------------------
DominantColorExtractionInterfaceABC::DominantColorExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
DominantColorExtractionInterface::DominantColorExtractionInterface(DominantColorExtractionTool* aTool):
  m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
DominantColorExtractionInterface::~DominantColorExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& DominantColorExtractionInterface::GetInterfaceID(void)
{
  return myID;
}

//----------------------------------------------------------------------------
const char *DominantColorExtractionInterface::GetName(void)
{
  return myName;
}

//----------------------------------------------------------------------------
void DominantColorExtractionInterface::destroy(void)
{
  delete m_ExtractionTool;
}

//----------------------------------------------------------------------------
int DominantColorExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC
                                                     *media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------

DominantColorDescriptorInterfaceABC*
DominantColorExtractionInterface::GetDescriptorInterface()
{
  return m_ExtractionTool->GetDescriptorInterface();
}
//----------------------------------------------------------------------------
int DominantColorExtractionInterface::SetDescriptorInterface(
DominantColorDescriptorInterfaceABC *aDominantColorDescriptorInterface)
{
  return m_ExtractionTool->
    SetDescriptorInterface(aDominantColorDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long DominantColorExtractionInterface::InitExtracting()
{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long DominantColorExtractionInterface::StartExtracting()
{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long DominantColorExtractionInterface::PostExtracting()
{
  return m_ExtractionTool->PostExtracting();
}

//----------------------------------------------------------------------------
const DCF * const DominantColorExtractionInterface::GetDCF()
{
  return m_ExtractionTool->GetDCF();
}

//----------------------------------------------------------------------------
unsigned long DominantColorExtractionInterface::DescFromDCF(const DCF *pdcf)
{
  return m_ExtractionTool->DescFromDCF(pdcf);
}

//=============================================================================
DominantColorExtractionTool::DominantColorExtractionTool():
  m_Interface(this),
  m_DescriptorInterface(0),
  m_Media(0),
  m_nummax(0),
  m_imwidth(0),
  m_imheight(0),
  m_imsize(0),
  m_image(0),
  m_closest(0),
  m_oldsize(-1)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
DominantColorExtractionTool::DominantColorExtractionTool(DominantColorDescriptorInterfaceABC *dc):
  m_Interface(this),
  m_DescriptorInterface(0),
  m_Media(0),
  m_nummax(0),
  m_imwidth(0),
  m_imheight(0),
  m_imsize(0),
  m_image(0),
  m_closest(0),
  m_oldsize(-1)
{
#ifdef VERBOSE
  fprintf(stderr, "Connect descriptor\n");
#endif

  /* create descriptor if it does not exist */
  if(!dc) {
    DominantColorDescriptor *descriptor = new DominantColorDescriptor(0,0);
    dc = descriptor->GetInterface();
  }

  /* connect descriptors with coding schemes */
  SetDescriptorInterface(dc);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
DominantColorExtractionTool::~DominantColorExtractionTool()
{
  if(m_image)
    delete m_image;
  if(m_closest)
    delete m_closest;

  /* release descriptor */
  if(m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool DominantColorExtractionTool::SupportsPush(void)
{
  return true;
}

//----------------------------------------------------------------------------
bool DominantColorExtractionTool::SupportsPull(void)
{
  return false;
}

//----------------------------------------------------------------------------
int DominantColorExtractionTool::SetSourceMedia(MultiMediaInterfaceABC *media)
{
  if (!media) return -1;

  m_Media=media;
  return 0;
}

//----------------------------------------------------------------------------
DominantColorDescriptorInterfaceABC 
*DominantColorExtractionTool::GetDescriptorInterface()
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int DominantColorExtractionTool::SetDescriptorInterface(
DominantColorDescriptorInterfaceABC *aDominantColorDescriptorInterface)
{
  /* check if new value is different from old value */
  if(m_DescriptorInterface==aDominantColorDescriptorInterface)
    return 0;

  /* release old interface */
  if(m_DescriptorInterface)
    m_DescriptorInterface->release();

  /* add new interface */
  m_DescriptorInterface = aDominantColorDescriptorInterface;
  if(m_DescriptorInterface) {
    m_DescriptorInterface->addref();
  }
  else {
    return -1;
  }

  return 0;

}

//---------------------------------------------------------------------------
unsigned long DominantColorExtractionTool::InitExtracting()
{

  m_dcf.reset();

  return 0;

} // InitExtracting

//---------------------------------------------------------------------------
unsigned long DominantColorExtractionTool::StartExtracting()
{
  MomVop *ImageMedia;
  // static int oldsize=-1; KW fix - raplaced with non-static variable m_oldsize.

  /* initialize */

  if(!m_DescriptorInterface->GetColorSpaceDescriptorInterface()) {
    fprintf(stderr, "Error: (DC_SE): could not ger ColorSpace DI\n");
    return (unsigned long)-1;
  }

  if(!m_DescriptorInterface->GetColorQuantizerDescriptorInterface()) {
    fprintf(stderr, "Error: (DC_SE): could not ger ColorQuantizer DI\n");
    return (unsigned long)-1;
  }

  if((bool)atoi(getpara("ColorSpacePresent","0"))) {
    m_DescriptorInterface->
      GetColorSpaceDescriptorInterface()->
      SetValuesFromParameterFile();
  }
  if((bool)atoi(getpara("ColorQuantizationPresent","0"))) {
    m_DescriptorInterface->
      GetColorQuantizerDescriptorInterface()->
      SetValuesFromParameterFile();
  }

  /* check for descriptor */
  if(!m_DescriptorInterface) {
    fprintf(stderr, "Error: (DC_SE): DC DI missing\n");
    return (unsigned long)-1;
  }

  /* check for media */
  if(!m_Media) {
    fprintf(stderr, "Error: (DC_SE): m_Media missing\n");
    return (unsigned long)-1;
  }
  ImageMedia = m_Media->GetImage();
  if(ImageMedia==NULL) {
    fprintf(stderr, "Error: (DC_SE): GetImage failed\n");
    return (unsigned long)-1;
  }

  m_imwidth = ImageMedia->width;
  m_imheight = ImageMedia->height;
  m_imsize = m_imwidth*m_imheight;

#ifdef DEBUG
  std::cerr << "Info(DCE::SE): checks done: " << m_oldsize << " " << m_imsize << std::endl;
#endif

  // convert to luv
  if(m_imsize != m_oldsize) {
    if(m_image)
      delete [] m_image;
    m_image = new float[3*m_imsize];
    if(m_closest)
      delete [] m_closest;
    m_closest = new int[m_imsize];
    m_oldsize = m_imsize;
  }
  RGB2LUV(ImageMedia);

#ifdef DEBUG
  std::cerr << "Info(DCE::SE): conversion done" << std::endl;
#endif

  // get segmentation mask
  unsigned char *quantAlpha=0;
  if(ImageMedia->a_chan && ImageMedia->a_chan->data)
    quantAlpha = ImageMedia->a_chan->data->u;

  // do the extraction
  Extract(quantAlpha);

#ifdef DEBUG
  m_DescriptorInterface->Print();
#endif

  return 0;
}

//---------------------------------------------------------------------------
unsigned long DominantColorExtractionTool::PostExtracting()
{

  return 0;

} // PostExtracting

//----------------------------------------------------------------------------
const UUID& DominantColorExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *DominantColorExtractionTool::GetName(void)
{ return myName; }

//---------------------------------------------------------------------------
DominantColorExtractionInterfaceABC 
*DominantColorExtractionTool::GetInterface(void)
{ return &m_Interface; }

//---------------------------------------------------------------------------
void DominantColorExtractionTool::RGB2LUV(MomVop *imrgb)
{
  double x,y,X,Y,Z,den,u2,v2,r,g,b;
  unsigned char *rpels=imrgb->v_chan->data->u;
  unsigned char *gpels=imrgb->y_chan->data->u;
  unsigned char *bpels=imrgb->u_chan->data->u;

  for(int ir=0,il=0; ir<m_imsize; ir++,il+=3) {

    if(rpels[ir] <= 20)
      r = (double)(8.715e-4*rpels[ir]);
    else
      r = (double)pow((rpels[ir]+25.245)/280.245, 2.22);

    if(gpels[ir] <= 20)
      g = (double)(8.715e-4*gpels[ir]);
    else
      g = (double)pow((gpels[ir]+25.245)/280.245, 2.22);

    if(bpels[ir] <= 20)
      b = (double)(8.715e-4*bpels[ir]);
    else
      b = (double)pow((bpels[ir]+25.245)/280.245, 2.22);

    X = 0.412453*r + 0.357580*g + 0.180423*b;
    Y = 0.212671*r + 0.715160*g + 0.072169*b;
    Z = 0.019334*r + 0.119193*g + 0.950227*b;

    if(X==0.0 && Y==0.0 && Z==0.0) {
      x=1.0/3.0;
      y=1.0/3.0;
    }
    else {
      den = X + Y + Z;
      x = X/den;
      y = Y/den;
    }

    den = -2*x + 12*y + 3;
    u2 = 4*x/den;
    v2 = 9*y/den;

    if(Y>0.008856)
      m_image[il] = (float)(116*pow(Y,1.0/3.0)-16);
    else
      m_image[il] = (float)(903.3*Y);
    m_image[il+1] = (float)(13*m_image[il]*(u2-LUVU20));
    m_image[il+2] = (float)(13*m_image[il]*(v2-LUVV20));
  }

}

//---------------------------------------------------------------------------
void DominantColorExtractionTool::Extract(unsigned char *quantImageAlpha)
{
  /*----------------------------------------------------------------------
   imdata ;  color value of input image ( LUV color space, linear array)
   imsize ; total image size ( width * height)
    
   m_MaxSize   ; Maximun DominantColor Number (initial value DCNMAX = 8)
   m_CurrSize  ; Current(final) DominantColor Number (int)
   m_Weights   ; Percentage of DominantColor (float)
   m_Centroids ; ColorValue of DominantColor (LUV color space, float)
   m_Variances ; ColorVariance of DominantColor (float)
   iwgts       ; Percentage of DominantColor (int)
   icnts       ; ColorValue of DominantColor (RGB color space, int)
   m_Coherency ; Spatial Coherency of DominantColor proposed by LG
  ------------------------------------------------------------------------*/

  double  dist, distold=FLT_MAX, distnew, eps=1.0, tmp;
  float   aglfct=DSTMIN;
  float   splfct=SPLFCT;
  int     i, j, k;

  // maximum number of colours to extract
  m_nummax = atoi(getpara("MaximumDCNumber", "8"));

  m_dcf.reset();

  /* loop:
     - cluster 
     - calculate centroids
     - calculate distortion
     - check distortion change:
       * if change small go to next iteration
       * if change large
         - if maximum number of bins reached QUIT
         - otherwise split one bin using perturbation vector */
  i = 0;
  m_dcf.resize(1); // force size to 1 -- Cluster will initialise it
  distnew = Cluster(quantImageAlpha);
  while( eps > EPSGLOB ) {

    /* find centroids */
    if(Centroids(quantImageAlpha) < 0) {
      Cluster(quantImageAlpha);
      /* we had an empty cluster so we assume the distortion was low */
      break;
    }

    /* classify bins */
    distnew = Cluster(quantImageAlpha);

    /* calculate total distortion */
    if( distold > 0.0 )
      eps = (distold - distnew)/distold;
    else
      eps = 0.0;
    distold = distnew;
    if( distnew == 0 )
      break;

    /* decide on splitting */
    if(i == 0 ||
       //(eps < EPSSPLT && m_dcf.getSize() < DominantColorDescriptor::DCNMAX)) {
       (eps < EPSSPLT && m_dcf.getSize() < m_nummax)) {
      Split(splfct, quantImageAlpha);
      distnew = Cluster(quantImageAlpha);
      eps = 1.0;
    }

    /* check for identical codevectors */
    m_dcf.verifyCentroids();

    i++;

  }
#ifdef DEBUG
  std::cerr << "Extract: iterations finished (" << i << ")" << std::endl;
#endif

  /* postprocessing: merge very close clusters
     this will change cluster boundaries, hence CCC sequence */
  m_dcf.agglom(aglfct);
  distnew = Cluster(quantImageAlpha);
  Centroids(quantImageAlpha);
  distnew = Cluster(quantImageAlpha);

  /* calculate variances */
  bool varpres = (bool)atoi(getpara("VariancePresent", VARPRESDEF));
  if(varpres)
    Vars(quantImageAlpha);

  // this can only be done after variances have been extracted
  // 03/06/05: fix normalisation for alpha mask
  int masksize=0;
  if(quantImageAlpha==0)
    masksize = m_imsize;
  else
    for(int i=0; i<m_imsize; i++)
      if(quantImageAlpha[i])
        masksize++;
  m_dcf.scaleWeights(masksize);

  /* calculate spatial coherency */
  bool spatialcoher = (bool)atoi(getpara("SpatialCoherency","0"));
  if(spatialcoher)
    m_dcf.sc = GetSpatialCoherency(3, quantImageAlpha);
  else
    m_dcf.sc = 0;

  /* quantise and set descriptor members */
  dc_t *qdesc = m_DescriptorInterface->GetDominantColors();
  for (i=0;i<m_dcf.getSize();i++) {
    qdesc[i].weight = (int)(31.9999*m_dcf.weights[i]);
    LUV2RGB(qdesc[i].centroid, m_dcf.centroids[i], 3);
#ifdef DEBUG
	std::cerr << "Extract: dc: " << i
		 << " unq: " << m_dcf.centroids[i][0] << " " << m_dcf.centroids[i][1] << " " << m_dcf.centroids[i][2] << " "
		 << " q: " << qdesc[i].centroid[0] << " " << qdesc[i].centroid[1] << " " << qdesc[i].centroid[2] << std::endl;
#endif
  }
  for(i=0; i<m_dcf.getSize(); i++) {
    for(int j=0; j<3; j++)
      qdesc[i].variance[j] = (m_dcf.variances[i][j] > VARTHR) ? 1 : 0;
  }

  /* color space conversion & quantization */
  bool cspres = (bool)atoi(getpara("ColorSpacePresent","0"));
  bool cqpres = (bool)atoi(getpara("ColorQuantizationPresent","0"));
  ConvertCSCQ(cspres, cqpres, qdesc);
#ifdef DEBUG
  for (i=0;i<m_dcf.getSize();i++) {
	std::cerr << "Extract: dc: " << i
		 << " converted: " << qdesc[i].centroid[0] << " " << qdesc[i].centroid[1] << " " << qdesc[i].centroid[2] << std::endl;
  }
#endif

  m_DescriptorInterface->SetColorSpacePresent(cspres);
  m_DescriptorInterface->SetColorQuantizationPresent(cqpres);
  m_DescriptorInterface->SetDominantColorsNumber(m_dcf.getSize());
  m_DescriptorInterface->SetVariancePresent(varpres);
  // qdesc is now a pointer inside the descriptor, so no need to copy!
  //m_DescriptorInterface->SetDominantColors(iwgts, icnts, ivars);
  m_DescriptorInterface->SetSpatialCoherency((int)m_dcf.sc);

} /* Extract */

//---------------------------------------------------------------------------
double DominantColorExtractionTool::Cluster(unsigned char *quantAlpha)
{
  int     i, j, jmin;
  double  d1, d2, d3, dist, distmin, disttot=0.0;
  float  *im1, *im2, *im3;
  int	imsize_msk;
  unsigned char *pAlpha;

  /* cluster */
  imsize_msk=0;
  for( i=0, im1=m_image, im2=m_image+1, im3=m_image+2;
       i<m_imsize;
       i++, im1+=3, im2+=3, im3+=3 ) {
    pAlpha = &quantAlpha[i];
    if(!quantAlpha || *pAlpha) {
      jmin = 0;
      distmin = FLT_MAX;
      for( j=0; j<m_dcf.getSize(); j++ ) {
	d1 = *im1 - m_dcf.centroids[j][0];
	d2 = *im2 - m_dcf.centroids[j][1];
	d3 = *im3 - m_dcf.centroids[j][2];
	dist = d1*d1 + d2*d2 + d3*d3;
	if( dist < distmin ) {
	  jmin = j;
	  distmin = dist;
	}
      }
      m_closest[i] = jmin;
      disttot += distmin;
      imsize_msk++;
    }
  }

//	return disttot/imsize;
  return disttot/imsize_msk;
} /* Cluster */

//---------------------------------------------------------------------------
int DominantColorExtractionTool::Centroids(unsigned char *quantAlpha)
{
  int     i, zeroweight=0;
  double  weight;
  float  *im1, *im2, *im3;
  unsigned char *pAlpha;

  /* reset centroids */
  int j;
  for(j=0; j<m_dcf.getSize(); j++) {
    m_dcf.weights[j] = 0.0;
    m_dcf.centroids[j][0] = 0.0;
    m_dcf.centroids[j][1] = 0.0;
    m_dcf.centroids[j][2] = 0.0;
  }

  /* calculate new centroids */
  for(i=0, im1=m_image, im2=m_image+1, im3=m_image+2;
      i<m_imsize;
      i++, im1+=3, im2+=3, im3+=3) {
    pAlpha = &quantAlpha[i];
    if(!quantAlpha || *pAlpha) {
      int ii = m_closest[i];
      m_dcf.weights[ii]++;
      m_dcf.centroids[ii][0] += *im1;
      m_dcf.centroids[ii][1] += *im2;
      m_dcf.centroids[ii][2] += *im3;
    }
  }

  for(j=0; j<m_dcf.getSize(); j++) {
    weight = m_dcf.weights[j];
    if( weight != 0.0 ) {
      m_dcf.centroids[j][0] /= weight;
      m_dcf.centroids[j][1] /= weight;
      m_dcf.centroids[j][2] /= weight;
    }
    else {
      /* this seems to be happening due to rounding error
         when there are too few too similar colours in the image;
         It might be better to check for this in GMXSplit but this
         is a less intrusive change (i.e. should not change the results
         of previous experiments by much. */
      std::cerr << "WARNING: zero weight for colour " << j << std::endl;
      if( j>0 )
        m_dcf.mergeEmpty( j, j-1 );
      else
        m_dcf.mergeEmpty( j, m_dcf.getSize()-1 );
      j--;
      zeroweight = -1;
    }
  }

  return zeroweight;

} /* Centroids */

//---------------------------------------------------------------------------
double DominantColorExtractionTool::Dist(unsigned char *quantAlpha)
{
  int     i, j;
  double  d1, d2, d3, dist=0.0;
  float   *im1, *im2, *im3;
  int 	imsize_msk;
  unsigned char *pAlpha;

  imsize_msk = 0;
  for( i=0, im1=m_image, im2=m_image+1, im3=m_image+2;
       i<m_imsize;
       i++, im1+=3, im2+=3, im3+=3 ) {
    pAlpha = &quantAlpha[i];
    if(!quantAlpha || *pAlpha) {
      j = m_closest[i];
      d1 = *im1 - m_dcf.centroids[j][0];
      d2 = *im2 - m_dcf.centroids[j][1];
      d3 = *im3 - m_dcf.centroids[j][2];
      dist += d1*d1 + d2*d2 + d3*d3;
      imsize_msk++;
    }
  }

//  return dist/imsize;
  return dist/imsize_msk;
} /* Dist */

//---------------------------------------------------------------------------
void DominantColorExtractionTool::Vars(unsigned char *quantAlpha)
{
  int     i, j;
  double  tmp;
  unsigned char *pAlpha;

  /* reset variances */
  for(i=0; i<m_dcf.getSize();i++) {
    m_dcf.variances[i][0] = 0.0;
    m_dcf.variances[i][1] = 0.0;
    m_dcf.variances[i][2] = 0.0;
  }

  /* estimate variances */
  for( i=0; i<m_imsize; i++ ) {
    pAlpha = &quantAlpha[i];
    if(!quantAlpha || *pAlpha) {
      j = m_closest[i];
      tmp = m_image[3*i] - m_dcf.centroids[j][0];
      m_dcf.variances[j][0] += tmp*tmp;
      tmp = m_image[3*i+1] - m_dcf.centroids[j][1];
      m_dcf.variances[j][1] += tmp*tmp;
      tmp = m_image[3*i+2] - m_dcf.centroids[j][2];
      m_dcf.variances[j][2] += tmp*tmp;
    }
  }

  /* normalise */
  for( j=0; j<m_dcf.getSize(); j++ ) {
    m_dcf.variances[j][0] /= m_dcf.weights[j];
    m_dcf.variances[j][1] /= m_dcf.weights[j];
    m_dcf.variances[j][2] /= m_dcf.weights[j];
  }

} /* Vars */

//---------------------------------------------------------------------------
void DominantColorExtractionTool::Split(double factor,
                                        unsigned char *quantAlpha)
{
  int     i, j, jmax=0;
  double  d1, d2, d3, diff1, diff2, diff3;
  double  ds[DominantColorDescriptor::DCNMAX][3];  /* variance components */
  double  dists[DominantColorDescriptor::DCNMAX];
  double  distmax=0.0;   /* total distortion */
  unsigned char *pAlpha;

  /* reset distortions */
  for( j=0; j<m_dcf.getSize(); j++ ) {
    ds[j][0] = 0.0;
    ds[j][1] = 0.0;
    ds[j][2] = 0.0;
    dists[j] = 0.0;
  }

  /* calculate local distortions */
  for( i=0; i<m_imsize; i++ ) {
    pAlpha = &quantAlpha[i];
    if(!quantAlpha || *pAlpha) {
      j = m_closest[i];
      d1 = m_image[3*i] - m_dcf.centroids[j][0];
      d2 = m_image[3*i+1] - m_dcf.centroids[j][1];
      d3 = m_image[3*i+2] - m_dcf.centroids[j][2];
      ds[j][0] += d1*d1;
      ds[j][1] += d2*d2;
      ds[j][2] += d3*d3;
    }
  }

  /* dists are total; variances are normalised */
  for( j=0; j<m_dcf.getSize(); j++ ) {
    dists[j] = ds[j][0] + ds[j][1] + ds[j][2];
    ds[j][0] /= m_dcf.weights[j];
    ds[j][1] /= m_dcf.weights[j];
    ds[j][2] /= m_dcf.weights[j];
  }

  /* find cluster with highest distortion */
  for( j=0; j<m_dcf.getSize(); j++ )
    if( dists[j] > distmax ) {
      jmax = j;
      distmax = dists[j];
    }

  /* split cluster with highest distortion;
     perturbance vector as in XM */
  diff1 = factor*sqrt(ds[jmax][0]);
  diff2 = factor*sqrt(ds[jmax][1]);
  diff3 = factor*sqrt(ds[jmax][2]);
  m_dcf.centroids[m_dcf.getSize()][0] = m_dcf.centroids[jmax][0] + diff1;
  m_dcf.centroids[m_dcf.getSize()][1] = m_dcf.centroids[jmax][1] + diff2;
  m_dcf.centroids[m_dcf.getSize()][2] = m_dcf.centroids[jmax][2] + diff3;
  m_dcf.centroids[jmax][0] -= diff1;
  m_dcf.centroids[jmax][1] -= diff2;
  m_dcf.centroids[jmax][2] -= diff3;
  m_dcf.incrSize();

} /* Split */

//---------------------------------------------------------------------------
int DominantColorExtractionTool::QuantizeSC( double sc )
{
  if(sc < 0.70)
    return 1;
  else
    return (int)( (sc - 0.70)/(1.0 - 0.70)*(pow(2.0,(double)SC_BIT)-3.0)+.5)+2;
}

//---------------------------------------------------------------------------
void DominantColorExtractionTool::ConvertCSCQ(bool cspres, bool cqpres,
                                              dc_t *qdesc)
{
  int component0 = 0;
  int component1 = 1;
  int component2 = 2; // default RGB
  char textBINNUMDEF[8];
  //_itoa(DominantColorDescriptor::BINNUMDEF,textBINNUMDEF,10);
  sprintf(textBINNUMDEF, "%d", DominantColorDescriptor::BINNUMDEF);
  int bin_number0 = atoi(getpara("BinNumber0", textBINNUMDEF));
  int bin_number1 = atoi(getpara("BinNumber1", textBINNUMDEF));
  int bin_number2 = atoi(getpara("BinNumber2", textBINNUMDEF));

  int max_h = 256;

  if(cspres) {
    TPixel in[25];
    TPixel res;
    float *conversionmatrix;

    for(int i=0; i<m_dcf.getSize(); i++) {
      in[12].y.i = qdesc[i].centroid[1]; // G
      in[12].u.i = qdesc[i].centroid[2]; // B
      in[12].v.i = qdesc[i].centroid[0]; // R

      switch(m_DescriptorInterface->GetColorSpaceDescriptorInterface()->
	     GetDescriptionColorSpace()) {
	case RGB:
	  component0 = 0; // R
	  component1 = 1; // G
	  component2 = 2; // B
	  break;
	case YCRCB:
	  RGB_To_YUV(&res,in,0,0);
	  component0 = 3; // Y
	  component1 = 4; // Cb
	  component2 = 5; // Cr
	  qdesc[i].centroid[0] = res.y.i;
	  qdesc[i].centroid[1] = res.u.i;
	  qdesc[i].centroid[2] = res.v.i;
	  break;
	case HSV:
	  RGB_To_HSV(&res,in,0,0);
	  component0 = 6; // H
	  component1 = 7; // S
	  component2 = 8; // V
	  qdesc[i].centroid[0] = res.u.i;
	  qdesc[i].centroid[1] = res.v.i;
	  qdesc[i].centroid[2] = res.y.i;
	  max_h = 256;
	  break;
	case HMMD:
	  RGB_To_HMMD(&res,in,0,0);
	  component0 = 6; // H
	  component1 = 12; // Sum
	  component2 = 11; // Diff
	  qdesc[i].centroid[0] = res.y.i;
	  qdesc[i].centroid[1] = res.u.i;
	  qdesc[i].centroid[2] = res.v.i;
	  max_h=360;
	  break;
	case LinearMatrix:
	  in[12].y.i = qdesc[i].centroid[0]; // R
	  in[12].u.i = qdesc[i].centroid[1]; // G
	  in[12].v.i = qdesc[i].centroid[2]; // B
	  conversionmatrix = m_DescriptorInterface->
	    GetColorSpaceDescriptorInterface()->
	    GetColorSpaceMatrix();
	  LinearTransform(&res,in,0,0,conversionmatrix);
	  qdesc[i].centroid[0] = res.y.i;
	  qdesc[i].centroid[1] = res.u.i;
	  qdesc[i].centroid[2] = res.v.i;
	  break;
	default:
	  break;
	}
    }
  }

  if(cqpres) {
    bin_number0 = m_DescriptorInterface->GetColorQuantizerDescriptorInterface()->
      GetBinNumberByComponent(component0);
    bin_number1 = m_DescriptorInterface->GetColorQuantizerDescriptorInterface()->
      GetBinNumberByComponent(component1);
    bin_number2 = m_DescriptorInterface->GetColorQuantizerDescriptorInterface()->
      GetBinNumberByComponent(component2);
#ifdef DEBUG
	std::cerr << "Info(DCE::CSCQ): binnums: "
         << bin_number0 << ", " << bin_number1 << ", " << bin_number2 << std::endl;
#endif
  }
  int i;
  for(i=0; i<m_dcf.getSize(); i++) {
    qdesc[i].centroid[0] *= double(bin_number0)/max_h;
    qdesc[i].centroid[1] *= double(bin_number1)/256;
    qdesc[i].centroid[2] *= double(bin_number2)/256;
  }

}

//---------------------------------------------------------------------------
// interface for GoFGoP descriptor collection
const DCF * const DominantColorExtractionTool::GetDCF()
{

  //m_dcf.resizeCheat(m_CurrSize);
  //m_dcf.sc = m_Coherency;

  return &m_dcf;

} // GetDCF

unsigned long DominantColorExtractionTool::DescFromDCF(const DCF *pdcf)
{
  char textBINNUMDEF[8];
  //_itoa(DominantColorDescriptor::BINNUMDEF,textBINNUMDEF,10);
  sprintf(textBINNUMDEF, "%d", DominantColorDescriptor::BINNUMDEF);
  int binnum0=atoi(getpara("BinNumber0", textBINNUMDEF));
  int binnum1=atoi(getpara("BinNumber1", textBINNUMDEF));
  int binnum2=atoi(getpara("BinNumber2", textBINNUMDEF));
  bool varpres=(bool)atoi(getpara("VariancePresent", VARPRESDEF));
  int dsize=pdcf->getSize();
  int i;

  // quantise
  dc_t *qdesc = m_DescriptorInterface->GetDominantColors();
  for(i=0; i<dsize; i++) {
    qdesc[i].weight = (int)(31.9999*pdcf->weights[i]);
    LUV2RGB(qdesc[i].centroid, pdcf->centroids[i], 3);
  }
  for(i=0; i<dsize; i++) {
    for(int j=0; j<3; j++)
      qdesc[i].variance[j] = (pdcf->variances[i][j] > VARTHR) ? 1 : 0;
  }
  for(i=0; i<dsize; i++) {
    qdesc[i].centroid[0] *= binnum0/256;
    qdesc[i].centroid[1] *= binnum1/256;
    qdesc[i].centroid[2] *= binnum2/256;
  }

  // set descriptor fields
  m_DescriptorInterface->SetColorSpacePresent(false);
  m_DescriptorInterface->SetColorQuantizationPresent(false);
  m_DescriptorInterface->SetDominantColorsNumber(dsize);
  m_DescriptorInterface->SetVariancePresent(varpres);
  // qdesc gets modified directly
  //m_DescriptorInterface->SetDominantColors(iwgts, icnts, ivars);
  m_DescriptorInterface->SetSpatialCoherency(pdcf->getSC()); // should work

  return 0;

} // DescFromDCF

// added by LG SC
//----------------------------------------------------------------------------
int DominantColorExtractionTool::GetSpatialCoherency(int dim,
                                                     unsigned char *quantAlpha)
{
  unsigned char *pAlpha;
  double CM = .0;
  int NeighborRange = 1;
  float SimColorAllow = sqrt(DSTMIN);
  bool * IVisit  = new bool [m_imsize];

  for (int x=0; x<m_imsize; x++) {
    pAlpha = &quantAlpha[x];
    if(!quantAlpha || *pAlpha) IVisit[x] = false;
    else IVisit[x] = true;
  }

  int All_Pixels = 0;
  {
    for (int x=0; x<m_imsize; x++)
      if(IVisit[x]==false) All_Pixels++;
  }
  {
    for (int i=0; i < m_dcf.getSize(); i++) {
      unsigned int Corres_Pixels=0;
      double Coherency=GetCoherencyWithColorAllow(dim, IVisit,
						  m_dcf.centroids[i][0],
						  m_dcf.centroids[i][1],
						  m_dcf.centroids[i][2],
						  SimColorAllow,
						  NeighborRange,
						  &Corres_Pixels);
      CM += Coherency * (double)Corres_Pixels/(double)(All_Pixels);
    }
  }
	
  delete []IVisit;
	
  return QuantizeSC(CM);
}

//---------------------------------------------------------------------------
double DominantColorExtractionTool::GetCoherencyWithColorAllow(int dim, bool * IVisit,
   float l, float u, float v, //not neccessary for r, g, b, would be l, u, v
   float Allow, int NeighborRange, unsigned int * OUTPUT_Corres_Pixel_Count)
{
  int count, i, j;
  int Neighbor_Count = 0;
  unsigned int Pixel_Count = 0;
  double Coherency = 0.0;
	
  int ISize = dim*m_imsize;

  for (count=0; count < ISize; count+=dim)
  {	
    i = (count/dim) % m_imwidth; //width
    j = (count/dim) / m_imwidth; //height
		
    float l1, u1, v1;
    l1 = m_image[count];
    u1 = m_image[count+1];
    v1 = m_image[count+2];		
		
    //distance 
    double distance;
    distance = sqrt(sqr(l-l1)+sqr(u-u1)+sqr(v-v1));
		
    if ((distance < Allow) && (IVisit[count/dim] == false))//no overlap checking
    {			
      IVisit[count/dim] = true;
      Pixel_Count++;
      int nSameNeighbor = 0;			
      for (int y = j - NeighborRange; y <= j + NeighborRange; y++){
        for (int x = i - NeighborRange; x <= i + NeighborRange; x++){
          if (!((i==x) && (j==y))){
            int Index = (y*m_imwidth+x)*dim;
            if ((Index >= 0) && (Index < ISize)){
              float l2 = m_image[Index];
              float u2 = m_image[Index+1];
              float v2 = m_image[Index+2];
              double distance=sqrt(sqr(l-l2)+sqr(u-u2)+sqr(v-v2));
              if (distance < Allow)
              {nSameNeighbor++;}
            }
          }
        }
      }			
      Neighbor_Count += nSameNeighbor;			
    }	
  }	
  *OUTPUT_Corres_Pixel_Count = Pixel_Count;
  int neighbor_check_window_size = NeighborRange*2+1;
  neighbor_check_window_size *= neighbor_check_window_size;
	
  if (Pixel_Count == 0) 
    Coherency = 0.0;
  else
    Coherency = (double)Neighbor_Count / (double)Pixel_Count / (double)(neighbor_check_window_size-1);
	
  return Coherency;	
}

void DominantColorExtractionTool::LUV2RGB(int *RGB, const float *LUV, int size)
{
  int i,k;
  double x,y,X,Y,Z,den,u2,v2,vec[3];

  for (i=0; i<size; i+=3) {
    if (LUV[i]>0) {
      if (LUV[i]<8.0)
        Y = ((double)LUV[i])/903.3;
      else
        Y = pow(((double)LUV[i] + 16)/116.0, 3.0);
      u2 = ((double)LUV[i+1])/13.0/((double)LUV[i]) + LUVU20;
      v2 = ((double)LUV[i+2])/13.0/((double)LUV[i]) + LUVV20;

      den = 6+3*u2-8*v2;
      //if (den<0)
      //  std::cerr << "den<0" << std::endl;
      //if (den==0)
      //  std::cerr << "den==0" << std::endl;
      x = 4.5*u2/den;
      y = 2.0*v2/den;

      X = x/y*Y;
      Z = (1-x-y)/y*Y;
    }
    else {
      X = 0.0;
      Y = 0.0;
      Z = 0.0;
    }

    vec[0] = ( 3.240479*X-1.537150*Y-0.498536*Z);
    vec[1] = (-0.969256*X+1.875992*Y+0.041556*Z);
    vec[2] = ( 0.055648*X-0.204043*Y+1.057311*Z);
    for (k=0; k<3; k++) {
      if(vec[k]<=0.018)
        vec[k] = 255*4.5*vec[k];
      else
        vec[k] = 255*(1.099*pow(vec[k],0.45)-0.099);
      if (vec[k]>255)
        vec[k] = 255;
      else if(vec[k]<0)
        vec[k] = 0;
      //RGB[i+k] = (int)round((double)vec[k]);
	  RGB[i+k] = (int)((double)vec[k]+0.5); //RJOC
    }
  }

}

bool operator< (const DCF &o1, const DCF &o2)
{
  // stub
  if(o1.getSize() < o2.getSize())
    return true;
  else
    return false;
}

bool operator> (const DCF &o1, const DCF &o2)
{
  // stub
  if(o1.getSize() > o2.getSize())
    return true;
  else
    return false;
}

bool operator== (const DCF &o1, const DCF &o2)
{
  if(o1.getSize() != o2.getSize())
    return false;
  if(o1.sc != o2.sc)
    return false;
  for(int i=0; i<o1.getSize(); i++){
    if(o1.weights[i] != o2.weights[i])
      return false;
    if(o1.centroids[i][0] != o2.centroids[i][0])
      return false;
    if(o1.centroids[i][1] != o2.centroids[i][1])
      return false;
    if(o1.centroids[i][2] != o2.centroids[i][2])
      return false;
    if(o1.variances[i][0] != o2.variances[i][0])
      return false;
    if(o1.variances[i][1] != o2.variances[i][1])
      return false;
    if(o1.variances[i][2] != o2.variances[i][2])
      return false;
  }
  return true;
}

bool operator!= (const DCF &o1, const DCF &o2)
{
  if(o1.getSize() != o2.getSize())
    return true;
  if(o1.sc != o2.sc)
    return true;
  for(int i=0; i<o1.getSize(); i++){
    if(o1.weights[i] != o2.weights[i])
      return true;
    if(o1.centroids[i][0] != o2.centroids[i][0])
      return true;
    if(o1.centroids[i][1] != o2.centroids[i][1])
      return true;
    if(o1.centroids[i][2] != o2.centroids[i][2])
      return true;
    if(o1.variances[i][0] != o2.variances[i][0])
      return true;
    if(o1.variances[i][1] != o2.variances[i][1])
      return true;
    if(o1.variances[i][2] != o2.variances[i][2])
      return true;
  }
  return false;
}

/*
 * DCF
 */

void DCF::myCopy(const DCF &dcf)
{
  memcpy(weights, dcf.weights, size*sizeof(float));
  for(int i=0; i<size; i++) {
    centroids[i][0] = dcf.centroids[i][0];
    centroids[i][1] = dcf.centroids[i][1];
    centroids[i][2] = dcf.centroids[i][2];
    variances[i][0] = dcf.variances[i][0];
    variances[i][1] = dcf.variances[i][1];
    variances[i][2] = dcf.variances[i][2];
  }
  sc = dcf.sc;
}

void DCF::reset()
{
  for(int i=0; i<DominantColorDescriptor::DCNMAX;i++) {
    weights[i] = 0.0;
    centroids[i][0] = 0.0;
    centroids[i][1] = 0.0;
    centroids[i][2] = 0.0;
    variances[i][0] = 0.0;
    variances[i][1] = 0.0;
    variances[i][2] = 0.0;
  }
  size = 0;
  sc = 0;
}

// merge clusters that are closer than distthr
void DCF::agglom(double distthr)
{
  double  d1, d2, d3, dists[8][8], distmin=0.0;
  double  w1min, w2min;
  int     ja, jb=0, jamin, jbmin;

  /* while two closest colours are closer than DISTTHR,
     merge the closest pair */
  do {

    /* initialise distance table */
    for( ja=0; ja<size; ja++ )
      for( jb=0; jb<ja; jb++ ) {
	d1 = centroids[ja][0] - centroids[jb][0];
	d2 = centroids[ja][1] - centroids[jb][1];
	d3 = centroids[ja][2] - centroids[jb][2];
	dists[ja][jb] = d1*d1 + d2*d2 + d3*d3;
      }

    /* find two closest colours */
    distmin = FLT_MAX;
    jamin = 0;
    jbmin = 0;
    for( ja=0; ja<size; ja++ )
      for( jb=0; jb<ja; jb++ )
	if( dists[ja][jb] < distmin ) {
	  distmin = dists[ja][jb];
	  jamin = ja;
	  jbmin = jb;
	}

    if( distmin > distthr )
      break;

    /* merge two closest colours */
    w1min = weights[jamin];
    w2min = weights[jbmin];
    centroids[jbmin][0] = (w1min*centroids[jamin][0] +
                           w2min*centroids[jbmin][0])/(w1min+w2min);
    centroids[jbmin][1] = (w1min*centroids[jamin][1] +
                           w2min*centroids[jbmin][1])/(w1min+w2min);
    centroids[jbmin][2] = (w1min*centroids[jamin][2] +
                           w2min*centroids[jbmin][2])/(w1min+w2min);
    weights[jbmin] += w1min;
    size--;

    /* remove jamin */
    for( jb=jamin; jb<size; jb++ ) {
      weights[jb] = weights[jb+1];
      centroids[jb][0] = centroids[jb+1][0];
      centroids[jb][1] = centroids[jb+1][1];
      centroids[jb][2] = centroids[jb+1][2];
      variances[jb][0] = variances[jb+1][0];
      variances[jb][1] = variances[jb+1][1];
      variances[jb][2] = variances[jb+1][2];
    }

  } while( size > 1 && distmin < distthr );

} /* agglom */


// merge two clusters, jam is empty
void DCF::mergeEmpty( int jam, int jbm )
{

  /* merge two closest colours */
  if( weights[jam] != 0 )
    std::cerr << "DCF::mergeEmpty: non-zero first weight" << std::endl;
  if( weights[jbm] == 0 )
    std::cerr << "DCF::mergeEmpty: two zero weights" << std::endl;
  size--;

  /* remove jam */
  for( int jb=jam; jb<size; jb++ ) {
    weights[jb] = weights[jb+1];
    centroids[jb][0] = centroids[jb+1][0];
    centroids[jb][1] = centroids[jb+1][1];
    centroids[jb][2] = centroids[jb+1][2];
    variances[jb][0] = variances[jb+1][0];
    variances[jb][1] = variances[jb+1][1];
    variances[jb][2] = variances[jb+1][2];
  }

} /* mergeEmpty */

void DCF::verifyCentroids()
{
  double dist, tmp;
  for(int j=0; j<size; j++) {
    for(int k=0; k<j; k++) {
      dist = 0.0;
      tmp = centroids[j][0] - centroids[k][0];
      dist += tmp*tmp;
      tmp = centroids[j][1] - centroids[k][1];
      dist += tmp*tmp;
      tmp = centroids[j][2] - centroids[k][2];
      dist += tmp*tmp;
      if( dist == 0.0 )
        std::cerr << "WARNING: two identical codevectors ("
             << j << ", " << k << ")" << std::endl;
    }
  }
}

// add clueter at specified position
void DCF::addCluster(const int index, const float weight,
                     const float c1, const float c2, const float c3,
                     const float v1, const float v2, const float v3)
{
  assert(index<size);

  weights[index] = weight;
  centroids[index][0] = c1;
  centroids[index][1] = c2;
  centroids[index][2] = c3;
  variances[index][0] = v1;
  variances[index][1] = v2;
  variances[index][2] = v3;
}

// distance between two DCFs, always use variance
float DCF::distance(const DCF &other) const
{
  // this is a copy of GetDistanceVariance -- should be good enough
  // quantisation probably unnecessary
  static const float WMULT=31.999999;
  static const float VARRECL=60.0;
  static const float VARRECH=90.0;
  int osize=other.getSize();
  float *wm=new float[size], *wo=new float[osize];
  float *cm0=new float[size], *cm1=new float[size], *cm2=new float[size];
  float *co0=new float[osize], *co1=new float[osize], *co2=new float[osize];
  float *vm0=new float[size], *vm1=new float[size], *vm2=new float[size];
  float *vo0=new float[osize], *vo1=new float[osize], *vo2=new float[osize];
  float d0, d1, d2, v0, v1, v2, arg1, arg2, tmp;
  float val=0.0;

  int i,i1,i2;
  // prequantise weights, centroids and variances
  for(i=0; i<size; i++) {
    wm[i] = ((int)(WMULT*weights[i])+0.5)/WMULT;
    cm0[i] = (int)(centroids[i][0]);
    cm1[i] = (int)(centroids[i][1]);
    cm2[i] = (int)(centroids[i][2]);
    vm0[i] = variances[i][0] < VARTHR ? VARRECL : VARRECH;
    vm1[i] = variances[i][1] < VARTHR ? VARRECL : VARRECH;
    vm2[i] = variances[i][2] < VARTHR ? VARRECL : VARRECH;
  }
  for(i=0; i<osize; i++) {
    wo[i] = ((int)(WMULT*other.weights[i])+0.5)/WMULT;
    co0[i] = (int)(other.centroids[i][0]);
    co1[i] = (int)(other.centroids[i][1]);
    co2[i] = (int)(other.centroids[i][2]);
    vo0[i] = other.variances[i][0] < VARTHR ? VARRECL : VARRECH;
    vo1[i] = other.variances[i][1] < VARTHR ? VARRECL : VARRECH;
    vo2[i] = other.variances[i][2] < VARTHR ? VARRECL : VARRECH;
  }

  /* the overall formula is:
     Integral of ( sum_ij f_i*f_j + sum_ij g_*g_j - 2*sum_ij f_j*g_j ) */

  /* loop for f_i*f_j */
  for(i1=0; i1<size; i1++) {
    for(i2=0; i2<size; i2++) {
      d0 = cm0[i1] - cm0[i2];
      d1 = cm1[i1] - cm1[i2];
      d2 = cm2[i1] - cm2[i2];
      v0 = vm0[i1] + vm0[i2];
      v1 = vm1[i1] + vm1[i2];
      v2 = vm2[i1] + vm2[i2];
      arg1 = (d0*d0/v0 + d1*d1/v1 + d2*d2/v2)/2.0;
      arg2 = twopi*sqrt(twopi*v0*v1*v2);
      tmp = wm[i1]*wm[i2]*exp(-arg1)/arg2;
      val += tmp;
    }
  }

  /* loop for g_i*g_j */
  for(i1=0; i1<other.size; i1++) {
    for(i2=0; i2<other.size; i2++) {
      d0 = co0[i1]- co0[i2];
      d1 = co1[i1]- co1[i2];
      d2 = co2[i1]- co2[i2];
      v0 = vo0[i1] + vo0[i2];
      v1 = vo1[i1] + vo1[i2];
      v2 = vo2[i1] + vo2[i2];
      arg1 = (d0*d0/v0 + d1*d1/v1 + d2*d2/v2)/2.0;
      arg2 = twopi*sqrt(twopi*v0*v1*v2);
      tmp = wo[i1]*wo[i2]*exp(-arg1)/arg2;
      val += tmp;
    }
  }

  /* loop for f_i*g_j */
  for(i1=0; i1<size; i1++) {
    for(i2=0; i2<other.size; i2++) {
      d0 = cm0[i1] - co0[i2];
      d1 = cm1[i1] - co1[i2];
      d2 = cm2[i1] - co2[i2];
      v0 = vm0[i1] + vo0[i2];
      v1 = vm1[i1] + vo1[i2];
      v2 = vm2[i1] + vo2[i2];
      arg1 = (d0*d0/v0 + d1*d1/v1 + d2*d2/v2)/2.0;
      arg2 = twopi*sqrt(twopi*v0*v1*v2);
      tmp = wm[i1]*wo[i2]*exp(-arg1)/arg2;
      val -= 2.0*tmp;
    }
  }
//#ifdef DEBUG
//  std::cerr << "Info(DCF::distance): val: " << val << std::endl;
//#endif

  delete wm; delete wo;
  delete cm0; delete cm1; delete cm2;
  delete co0; delete co1; delete co2;
  delete vm0; delete vm1; delete vm2;
  delete vo0; delete vo1; delete vo2;

  return 1000000000.0*val;

} // DCF::distance

//#undef DEBUG
};
