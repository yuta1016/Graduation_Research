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
// Applicable File Name:  ServerApplications.h
//
#ifndef __SERVER_APPLICATIONS_H__
#define __SERVER_APPLICATIONS_H__
#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#ifdef USEDT
#include "Applications/DummyTypeSAppl.h"
#endif
#ifdef USEDTDS
#include "Applications/DSDummyTypeSAppl.h"
#endif

#include "Applications/TimeSeriesSAppl.h"
#include "Applications/IIColorSAppl.h"

#include "Applications/ColorLayoutSAppl.h"
#include "Applications/ColorStructureSAppl.h"
#include "Applications/CTBrowsingSAppl.h"
#include "Applications/DominantColorSAppl.h"
#include "Applications/GoFGoPColorSAppl.h"
#include "Applications/ScalableColorSAppl.h"

#include "Applications/EdgeHistSAppl.h"
#include "Applications/HomoTextureSAppl.h"
#include "Applications/TextureBrowsingSAppl.h"

#include "Applications/3DShapeSpectrumSAppl.h"
#include "Applications/ContourShapeSAppl.h"
#include "Applications/MultiViewSAppl.h"
#include "Applications/Perceptual3DShapeSAppl.h"
#include "Applications/RegionLocatorSAppl.h"
#include "Applications/RegionShapeSAppl.h"
#include "Applications/SpatioTemporalLocatorSAppl.h"
#include "Applications/ShapeVariationSAppl.h"

#include "Applications/CameraMotionSAppl.h"
#include "Applications/MotionActivitySAppl.h"
#include "Applications/MotionTrajectorySAppl.h"
#include "Applications/ParametricObjectMotionSAppl.h"

#include "Applications/FaceRecognitionSAppl.h"
#include "Applications/AdvancedFaceRecognitionSAppl.h"

#include "Applications/MultiImageSAppl.h"
#include "Applications/GridLayoutSAppl.h"

#include "Applications/SpokenContentSAppl.h"

#include "Applications/HierarchicalKeyFrameSAppl.h"
#include "Applications/InkSegmentSAppl.h"
#include "Applications/MatchingHintSAppl.h"
#include "Applications/ModelSAppl.h"
#include "Applications/MosaicSAppl.h"
#include "Applications/MediaQualitySAppl.h"
#include "Applications/SegmentRelGraphSAppl.h"
//#include "Applications/SegmentSAppl.h"
//#include "Applications/SemanticSAppl.h"
#include "Applications/SeqSummaryFastMovingStoryBoardSAppl.h"
#include "Applications/SeqSummaryKeywordsSAppl.h"
#include "Applications/SequentialSummarySAppl.h"
#include "Applications/SoundPropertySAppl.h"
#include "Applications/SpaceFrequencyViewSAppl.h"
/* #include "Applications/SpatioTemporalDSSAppl.h" */
#include "Applications/StateTransitionModelSAppl.h"
#include "Applications/StructuredCollectionSAppl.h"
#include "Applications/SummarizationSAppl.h"
#include "Applications/TranscodingSAppl.h"
#include "Applications/TimeSAppl.h"
#include "Applications/VariationSAppl.h"
#include "Applications/VideoEditingSAppl.h"
#include "Applications/VideoTextSAppl.h"

#include "Applications/ImageSignatureSAppl.h"
#include "Applications/VideoSignatureSAppl.h"

#endif //__SERVER_APPLICATIONS_H__
