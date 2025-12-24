///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Intel Corporation
// TU-Munich, S.Herrmann
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
// Applicable File Name:  ExtractionUtilities.h
//
#ifndef __EXTRACTIONUTILITIES_H__
#define __EXTRACTIONUTILITIES_H__
#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"

#include <inter.h>
#include <intra.h>
#include <fifo.h>
#include <vopio.h>
#ifdef EVAL
  #include <eval_functions.h>
  #include "eval_funcdefs.h"
#endif

/*implemented extraction untilities*/
#include "Descriptors/Descriptors.h"
#ifdef USEDT
#include "ExtractionUtilities/DummyTypeExtraction.h"
#endif
#ifdef USEDTDS
#include "ExtractionUtilities/DSDummyTypeExtraction.h"
#endif

#include "ExtractionUtilities/ColorSpaceExtraction.h"
#include "ExtractionUtilities/ColorQuantExtraction.h"
#include "ExtractionUtilities/CTBrowsingExtraction.h"
#include "ExtractionUtilities/IIColorExtraction.h"
#include "ExtractionUtilities/TemporalInterpolationExtraction.h"

#include "ExtractionUtilities/ColorLayoutExtraction.h"
#include "ExtractionUtilities/ColorStructureExtraction.h"
#include "ExtractionUtilities/DominantColorExtraction.h"
#include "ExtractionUtilities/GoFGoPColorExtraction.h"
#include "ExtractionUtilities/ScalableColorExtraction.h"

#include "ExtractionUtilities/EdgeHistExtraction.h"
#include "ExtractionUtilities/HomoTextureExtraction.h"
#include "ExtractionUtilities/TextureBrowsingExtraction.h"

#include "ExtractionUtilities/3DShapeSpectrumExtraction.h"
#include "ExtractionUtilities/ContourShapeExtraction.h"
#include "ExtractionUtilities/MultiViewExtraction.h"
#include "ExtractionUtilities/Perceptual3DShapeExtraction.h"
#include "ExtractionUtilities/RegionLocatorExtraction.h"
#include "ExtractionUtilities/RegionShapeExtraction.h"
#include "ExtractionUtilities/SpatioTemporalLocatorExtraction.h"
#include "ExtractionUtilities/ShapeVariationExtraction.h"

#include "ExtractionUtilities/CameraMotionExtraction.h"
#include "ExtractionUtilities/MotionActivityExtraction.h"
#include "ExtractionUtilities/MotionTrajectoryExtraction.h"
#include "ExtractionUtilities/ParametricObjectMotionExtraction.h"
#include "ExtractionUtilities/ParametricObjectMotionExtraction2.h"

#include "ExtractionUtilities/FaceRecognitionExtraction.h"
#include "ExtractionUtilities/AdvancedFaceRecognitionExtraction.h"

#include "ExtractionUtilities/MultiImageExtraction.h"
#include "ExtractionUtilities/GridLayoutExtraction.h"

#include "ExtractionUtilities/TimeSeriesExtraction.h"

// Audio implemented MDS stuff
#include "ExtractionUtilities/ProbabilityVectorExtraction.h"
#include "ExtractionUtilities/ProbabilityMatrixExtraction.h"

// Spoken content stuff
#include "ExtractionUtilities/SpokenContentIndexEntryExtraction.h"
#include "ExtractionUtilities/SpokenContentLexiconExtraction.h"
#include "ExtractionUtilities/SpokenContentWordLexiconExtraction.h"
#include "ExtractionUtilities/SpokenContentLinkExtraction.h"
#include "ExtractionUtilities/SpokenContentWordLinkExtraction.h"
#include "ExtractionUtilities/SpokenContentPhoneLinkExtraction.h"
#include "ExtractionUtilities/SpokenContentConfusionInfoExtraction.h"
#include "ExtractionUtilities/SpokenContentPhoneLexiconExtraction.h"
#include "ExtractionUtilities/SpokenContentPhoneIndexEntryExtraction.h"
#include "ExtractionUtilities/SpokenContentWordIndexEntryExtraction.h"
#include "ExtractionUtilities/SpokenContentSpeakerExtraction.h"
#include "ExtractionUtilities/SpokenContentHeaderExtraction.h"
#include "ExtractionUtilities/SpokenContentNodeExtraction.h"
#include "ExtractionUtilities/SpokenContentBlockExtraction.h"
#include "ExtractionUtilities/SpokenContentLatticeExtraction.h"
#include "ExtractionUtilities/SpokenContentExtraction.h"

#include "ExtractionUtilities/HierarchicalKeyFrameExtraction.h"
#include "ExtractionUtilities/InkSegmentExtraction.h"
#include "ExtractionUtilities/MatchingHintExtraction.h"
#include "ExtractionUtilities/MediaQualityExtraction.h"
#include "ExtractionUtilities/ModelExtraction.h"
#include "ExtractionUtilities/MosaicExtraction.h"
//#include "ExtractionUtilities/SegmentExtraction.h"
#include "ExtractionUtilities/SegmentRelGraphExtraction.h"
#include "ExtractionUtilities/SoundPropertyExtraction.h"
#include "ExtractionUtilities/SpaceFrequencyViewExtraction.h"
#include "ExtractionUtilities/SeqSummaryFastMovingStoryBoardExtraction.h"
#include "ExtractionUtilities/SeqSummaryKeywordsExtraction.h"
#include "ExtractionUtilities/SequentialSummaryExtraction.h"
#include "ExtractionUtilities/StateTransitionModelExtraction.h"
#include "ExtractionUtilities/StructuredCollectionExtraction.h"
#include "ExtractionUtilities/SummarizationExtraction.h"
#include "ExtractionUtilities/TranscodingHintsExtraction.h"
#include "ExtractionUtilities/TimeExtraction.h"
#include "ExtractionUtilities/VariationExtraction.h"
#include "ExtractionUtilities/VideoEditingExtraction.h"
#include "ExtractionUtilities/VideoTextExtraction.h"

#include "ExtractionUtilities/ImageSignatureExtraction.h"
#include "ExtractionUtilities/VideoSignatureExtraction.h"

#endif //__EXTRACTIONUTILITIES_H__
