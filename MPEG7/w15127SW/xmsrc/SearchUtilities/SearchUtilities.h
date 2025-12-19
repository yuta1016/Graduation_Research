///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Intel Corporation
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
// Applicable File Name: SearchUtilities.h
//
#ifndef __SEARCHUTILITIES_H__
#define __SEARCHUTILITIES_H__
#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"

//#include "SearchUtilities/MatchList.h"
/*implemented search untilities*/
#ifdef USEDT
#include "SearchUtilities/DummyTypeSearch.h"
#endif
#ifdef USEDTDS
#include "SearchUtilities/DSDummyTypeSearch.h"
#endif

#include "SearchUtilities/TimeSeriesSearch.h"

#include "SearchUtilities/ColorLayoutSearch.h"
#include "SearchUtilities/ColorStructureSearch.h"
#include "SearchUtilities/CTBrowsingSearch.h"

#include "SearchUtilities/DominantColorSearch.h"
#include "SearchUtilities/DominantColorTemperatureSearch.h"
#include "SearchUtilities/GoFGoPColorSearch.h"
#include "SearchUtilities/ScalableColorSearch.h"

#include "SearchUtilities/EdgeHistSearch.h"
#include "SearchUtilities/HomoTextureSearch.h"
#include "SearchUtilities/TextureBrowsingSearch.h"

#include "SearchUtilities/3DShapeSpectrumSearch.h"
#include "SearchUtilities/ContourShapeSearch.h"
#include "SearchUtilities/MultiViewSearch.h"
#include "SearchUtilities/Perceptual3DShapeSearch.h"
#include "SearchUtilities/RegionShapeSearch.h"
#include "SearchUtilities/SpatioTemporalLocatorSearch.h"
#include "SearchUtilities/ShapeVariationSearch.h"

#include "SearchUtilities/CameraMotionSearch.h"
#include "SearchUtilities/MotionActivitySearch.h"
#include "SearchUtilities/MotionTrajectorySearch.h"
#include "SearchUtilities/ParametricObjectMotionSearch.h"

#include "SearchUtilities/FaceRecognitionSearch.h"
#include "SearchUtilities/AdvancedFaceRecognitionSearch.h"

#include "SearchUtilities/MultiImageSearch.h"

#include "SearchUtilities/AudioSignatureSearch.h"
#include "SearchUtilities/AudioSpectralFlatnessSearch.h"
#include "SearchUtilities/MelodySequenceSearch.h"
#include "SearchUtilities/SpokenContentSearch.h"

#include "SearchUtilities/AffectiveSegmentFilterSearch.h"
#include "SearchUtilities/DependStructureSearch.h"
#include "SearchUtilities/ExtendedTextualTypeSearch.h"
#ifndef _VISUALC_
#include "SearchUtilities/GraphicalCSSearch.h"
#endif
#include "SearchUtilities/ImportanceHintSearch.h"
#include "SearchUtilities/InkSegmentSearch.h"
#include "SearchUtilities/MatchingHintSearch.h"
#include "SearchUtilities/MediaMetaSearch.h"
#include "SearchUtilities/MediaReviewSearch.h"
#include "SearchUtilities/MosaicSearch.h"
#include "SearchUtilities/OrderingKeySearch.h"
#include "SearchUtilities/PackageSearch.h"
#include "SearchUtilities/PhoneticTranslationHintsSearch.h"
#include "SearchUtilities/PointOfViewSearch.h"
//#include "SearchUtilities/SegmentSearch.h"
#include "SearchUtilities/SegmentRelGraphSearch.h"
#ifndef _VISUALC_
#include "SearchUtilities/SemanticSearch.h"
#endif
#include "SearchUtilities/SpatialResolutionHintSearch.h"
#ifndef _VISUALC_
#include "SearchUtilities/StateMemberSearch.h"
#endif
#include "SearchUtilities/StateTransitionModelSearch.h"
#include "SearchUtilities/StructuredCollectionSearch.h"
#include "SearchUtilities/SummarizationSearch.h"
#include "SearchUtilities/SummaryPreferencesSearch.h"
#include "SearchUtilities/TimeSearch.h"
#include "SearchUtilities/UserPreferenceSearch.h"

#include "SearchUtilities/ImageSignatureSearch.h"
#include "SearchUtilities/VideoSignatureSearch.h"



#endif //__SEARCHUTILITIES_H__
