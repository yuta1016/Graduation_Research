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
// Applicable File Name:  ClientApplications.h
//
#ifndef __CLIENT_APPLICATIONS_H__
#define __CLIENT_APPLICATIONS_H__
#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#ifdef USEDT
#include "Applications/DummyTypeCAppl.h"
#endif
#ifdef USEDTDS
#include "Applications/DSDummyTypeCAppl.h"
#endif

#include "Applications/TimeSeriesCAppl.h"

#include "Applications/ColorLayoutCAppl.h"
#include "Applications/ColorStructureCAppl.h"
#include "Applications/CTBrowsingCAppl.h"
#include "Applications/DominantColorCAppl.h"
#include "Applications/DominantColorTemperatureCAppl.h"
#include "Applications/GoFGoPColorCAppl.h"
#include "Applications/ScalableColorCAppl.h"

#include "Applications/EdgeHistCAppl.h"
#include "Applications/HomoTextureCAppl.h"
#include "Applications/TextureBrowsingCAppl.h"

#include "Applications/3DShapeSpectrumCAppl.h"
#include "Applications/ContourShapeCAppl.h"
#include "Applications/MultiViewCAppl.h"
#include "Applications/Perceptual3DShapeCAppl.h"
#include "Applications/RegionLocatorCAppl.h"
#include "Applications/RegionShapeCAppl.h"
#include "Applications/SpatioTemporalLocatorCAppl.h"
#include "Applications/ShapeVariationCAppl.h"


#include "Applications/CameraMotionCAppl.h"
#include "Applications/MotionActivityCAppl.h"
#include "Applications/MotionTrajectoryCAppl.h"
#include "Applications/ParametricObjectMotionCAppl.h"

#include "Applications/FaceRecognitionCAppl.h"
#include "Applications/AdvancedFaceRecognitionCAppl.h"

#include "Applications/MultiImageCAppl.h"

#include "Applications/AudioSignatureCAppl.h"
#include "Applications/AudioSpectralFlatnessCAppl.h"
#include "Applications/MelodySequenceCAppl.h"
#include "Applications/SpokenContentCAppl.h"

#include "Applications/AffectiveSegmentFilterCAppl.h"
#include "Applications/DependStructureCAppl.h"
#include "Applications/ExtendedTextualTypeCAppl.h"
#include "Applications/GraphicalCSCAppl.h"
#include "Applications/ImportanceHintCAppl.h"
#include "Applications/InkSegmentCAppl.h"
#include "Applications/MatchingHintCAppl.h"
#include "Applications/MediaMetaCAppl.h"
#include "Applications/MediaReviewCAppl.h"
#include "Applications/MosaicCAppl.h"
#include "Applications/OrderingKeyCAppl.h"
#include "Applications/PackageCAppl.h"
#include "Applications/PhoneticTranslationHintsCAppl.h"
#include "Applications/PointOfViewCAppl.h"
#include "Applications/SegmentCAppl.h"
#include "Applications/SegmentRelGraphCAppl.h"
#include "Applications/SemanticCAppl.h"
#include "Applications/SpatialResolutionHintCAppl.h"
#include "Applications/StateMemberCAppl.h"
#include "Applications/StateTransitionModelCAppl.h"
#include "Applications/StructuredCollectionCAppl.h"
#include "Applications/SummarizationCAppl.h"
#include "Applications/SummaryPreferencesCAppl.h"
#include "Applications/TimeCAppl.h"
#include "Applications/UserPreferenceCAppl.h"
//#include "Applications/VariationCAppl.h"

#include "Applications/ImageSignatureCAppl.h"
#include "Applications/VideoSignatureCAppl.h"

#endif //__CLIENT_APPLICATIONS_H__
