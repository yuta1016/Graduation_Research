///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Intel Corporation
// TU-Munich, S.Herrmann
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
// Copyright (c) 1998-2004.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  Descriptors.h
#ifndef __DESCRIPTORS__
#define __DESCRIPTORS__
#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#ifdef USEDT
#include "Descriptors/DummyType.h"
#endif

//#include "Descriptors/TemporalInter.h"
#include "Descriptors/TimeSeries.h"
#include "Descriptors/ColorSpace.h"
#include "Descriptors/ColorQuant.h"
#include "Descriptors/Spatial2DCoordinates.h"
#include "Descriptors/TemporalInterpolation.h"
//#include "Descriptors/TimeSeries.h"

#include "Descriptors/ColorLayout.h"
#include "Descriptors/ColorStructure.h"
#include "Descriptors/CTBrowsing.h"
#include "Descriptors/DominantColor.h"
#include "Descriptors/GoFGoPColor.h"
#include "Descriptors/GofGopFeature.h"
#include "Descriptors/IIColor.h"
#include "Descriptors/ScalableColor.h"

#include "Descriptors/TextureBrowsing.h"
#include "Descriptors/GridLayout.h"
#include "Descriptors/HomoTexture.h"
#include "Descriptors/EdgeHist.h"

#include "Descriptors/3DShapeSpectrum.h"
#include "Descriptors/ContourShape.h"
#include "Descriptors/MultiView.h"
#include "Descriptors/Perceptual3DShape.h"
#include "Descriptors/RegionLocator.h"
#include "Descriptors/RegionShape.h"
#include "Descriptors/SpatioTemporalLocator.h"
#include "Descriptors/ShapeVariation.h"

#include "Descriptors/CameraMotion.h"
#include "Descriptors/MotionTrajectory.h"
#include "Descriptors/ParametricObjectMotion.h"
#include "Descriptors/MotionActivity.h"

#include "Descriptors/FaceRecognition.h"
#include "Descriptors/AdvancedFaceRecognition.h"

#include "Descriptors/MultiImage.h"

// Audio defined MDS components
#include "Descriptors/ProbabilityVector.h"
#include "Descriptors/ProbabilityMatrix.h"

// Spoken content stuff
#include "Descriptors/SpokenContentIndexEntry.h"
#include "Descriptors/SpokenContentLexicon.h"
#include "Descriptors/SpokenContentWordLexicon.h"
#include "Descriptors/SpokenContentLink.h"
#include "Descriptors/SpokenContentWordLink.h"
#include "Descriptors/SpokenContentPhoneLink.h"
#include "Descriptors/SpokenContentConfusionInfo.h"
#include "Descriptors/SpokenContentPhoneLexicon.h"
#include "Descriptors/SpokenContentPhoneIndexEntry.h"
#include "Descriptors/SpokenContentWordIndexEntry.h"
#include "Descriptors/SpokenContentSpeaker.h"
#include "Descriptors/SpokenContentHeader.h"
#include "Descriptors/SpokenContentNode.h"
#include "Descriptors/SpokenContentBlock.h"
#include "Descriptors/SpokenContentLattice.h"
#include "Descriptors/SpokenContent.h"

#include "Descriptors/TimePoint.h"
#include "Descriptors/TimeUnit.h"
//#include "Descriptors/PointOfView.h"
#include "Descriptors/RelTime.h"
#include "Descriptors/RelIncrTime.h"
#include "Descriptors/Duration.h"
#include "Descriptors/IncrDuration.h"


#include "Descriptors/ImageSignature.h"
#include "Descriptors/VideoSignature.h"

#endif //__DESCRIPTORS__
