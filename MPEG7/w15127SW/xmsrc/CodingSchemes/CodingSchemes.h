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
// Copyright (c) 1998-2004
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  CodingSchemes.h
//
#ifndef __CODING_SCHEMES_H__
#define __CODING_SCHEMES_H__
#include "BasicArchitecture/XMObjects.h"
#ifdef USEDT
#include "CodingSchemes/DummyTypeCS.h"
#endif

#include "CodingSchemes/ColorSpaceCS.h"
#include "CodingSchemes/ColorQuantCS.h"
#include "CodingSchemes/GridLayoutCS.h"
#include "CodingSchemes/IIColorCS.h"
#include "CodingSchemes/Spatial2DCoordinatesCS.h"
#include "CodingSchemes/TemporalInterpolationCS.h"
#include "CodingSchemes/TimeSeriesCS.h"

#include "CodingSchemes/ColorLayoutCS.h"
#include "CodingSchemes/ColorStructureCS.h"
#include "CodingSchemes/CTBrowsingCS.h"
#include "CodingSchemes/DominantColorCS.h"
#include "CodingSchemes/GoFGoPColorCS.h"
#include "CodingSchemes/GofGopFeatureCS.h"
#include "CodingSchemes/ScalableColorCS.h"

#include "CodingSchemes/EdgeHistCS.h"
#include "CodingSchemes/HomoTextureCS.h"
#include "CodingSchemes/TextureBrowsingCS.h"

#include "CodingSchemes/3DShapeSpectrumCS.h"
#include "CodingSchemes/ContourShapeCS.h"
#include "CodingSchemes/MultiViewCS.h"
#include "CodingSchemes/Perceptual3DShapeCS.h"
#include "CodingSchemes/RegionLocatorCS.h"
#include "CodingSchemes/RegionShapeCS.h"
#include "CodingSchemes/SpatioTemporalLocatorCS.h"
#include "CodingSchemes/ShapeVariationCS.h"

#include "CodingSchemes/CameraMotionCS.h"
#include "CodingSchemes/MotionTrajectoryCS.h"
#include "CodingSchemes/ParametricObjectMotionCS.h"
#include "CodingSchemes/MotionActivityCS.h"

#include "CodingSchemes/FaceRecognitionCS.h"
#include "CodingSchemes/AdvancedFaceRecognitionCS.h"

#include "CodingSchemes/MultiImageCS.h"

// Audio implemented MDS components
#include "CodingSchemes/ProbabilityVectorCS.h"
#include "CodingSchemes/ProbabilityMatrixCS.h"

// Spoken content stuff
#include "CodingSchemes/SpokenContentIndexEntryCS.h"
#include "CodingSchemes/SpokenContentLexiconCS.h"
#include "CodingSchemes/SpokenContentWordLexiconCS.h"
#include "CodingSchemes/SpokenContentLinkCS.h"
#include "CodingSchemes/SpokenContentWordLinkCS.h"
#include "CodingSchemes/SpokenContentPhoneLinkCS.h"
#include "CodingSchemes/SpokenContentConfusionInfoCS.h"
#include "CodingSchemes/SpokenContentPhoneLexiconCS.h"
#include "CodingSchemes/SpokenContentPhoneIndexEntryCS.h"
#include "CodingSchemes/SpokenContentWordIndexEntryCS.h"
#include "CodingSchemes/SpokenContentSpeakerCS.h"
#include "CodingSchemes/SpokenContentHeaderCS.h"
#include "CodingSchemes/SpokenContentNodeCS.h"
#include "CodingSchemes/SpokenContentBlockCS.h"
#include "CodingSchemes/SpokenContentLatticeCS.h"
#include "CodingSchemes/SpokenContentCS.h"

#include "CodingSchemes/TimePointCS.h"
#include "CodingSchemes/TimeUnitCS.h"
#include "CodingSchemes/RelTimeCS.h"
#include "CodingSchemes/RelIncrTimeCS.h"
#include "CodingSchemes/DurationCS.h"
#include "CodingSchemes/IncrDurationCS.h"

#include "CodingSchemes/ImageSignatureCS.h"
#include "CodingSchemes/VideoSignatureCS.h"

#include "CodingSchemes/GenericDSCS.h"


#endif //__CODING_SCHEMES_H__
