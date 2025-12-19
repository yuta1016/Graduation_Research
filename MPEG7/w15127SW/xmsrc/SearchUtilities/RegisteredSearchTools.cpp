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
// RegisteredSearchTools.cpp

#include "BasicArchitecture/XMObjects.h"
#include "XMManager/ComponentDB.h"
#include "SearchUtilities/SearchUtilities.h"


//----------------------------------------------------------------------------
#define ADD_ELEMENT(DSType)				\
							\
	temp = new DSType;				\
	m_SearchUtilities.push_back(temp);

using namespace XM;

//----------------------------------------------------------------------------
void ComponentDatabase::InitSearch(void)
{
	// To fully implement this class factory, we need to construct all objects 
	// which are to be evaluated.

#ifdef REGISTERCOMPONENTS
	SearchABC *temp;

	///////////////////////////////////////////////////////////////////////////
	// Add standard descriptors
	ADD_ELEMENT(Search);
#ifdef USEDT
	ADD_ELEMENT(DummyTypeSearchTool);
#endif
#ifdef USEDTDS
	ADD_ELEMENT(DSDummyTypeSearchTool);
#endif

	ADD_ELEMENT(TimeSeriesSearchTool);

	ADD_ELEMENT(ColorLayoutSearchTool);
	ADD_ELEMENT(ColorStructureSearchTool);
	ADD_ELEMENT(CTBrowsingSearchTool);
	ADD_ELEMENT(DominantColorSearchTool);
	ADD_ELEMENT(DominantColorTemperatureSearchTool);
	ADD_ELEMENT(GoFGoPColorSearchTool);
	ADD_ELEMENT(ScalableColorSearchTool);

	ADD_ELEMENT(EdgeHistogramSearchTool);
	ADD_ELEMENT(HomogeneousTextureSearchTool);
	ADD_ELEMENT(TextureBrowsingSearchTool);

	ADD_ELEMENT(ShapeSpectrumSearchTool);
	ADD_ELEMENT(ContourShapeSearchTool);
	ADD_ELEMENT(MultiViewSearchTool);
	ADD_ELEMENT(Perceptual3DShapeSearchTool);
	ADD_ELEMENT(RegionShapeSearchTool);
	ADD_ELEMENT(SpatioTemporalLocatorSearchTool);
	ADD_ELEMENT(ShapeVariationSearchTool);

	ADD_ELEMENT(CameraMotionSearchTool);
	ADD_ELEMENT(MotionActivitySearchTool);
	ADD_ELEMENT(MotionTrajectorySearchTool);
	ADD_ELEMENT(ParametricObjectMotionSearchTool);

	ADD_ELEMENT(FaceRecognitionSearchTool);
	ADD_ELEMENT(AdvancedFaceRecognitionSearchTool);

	ADD_ELEMENT(MultiImageSearchTool);

	ADD_ELEMENT(ImageSignatureSearchTool);
	ADD_ELEMENT(VideoSignatureSearchTool);

	ADD_ELEMENT(AudioSignatureSearchTool);
	ADD_ELEMENT(AudioSpectralFlatnessSearchTool);
	ADD_ELEMENT(MelodySequenceSearchTool);
	ADD_ELEMENT(SpokenContentSearchTool);


	ADD_ELEMENT(AffectiveSegmentFilterTool);
	ADD_ELEMENT(DependStructureSearchTool);
	ADD_ELEMENT(ExtendedTextualTypeSearchTool);
#ifndef _VISUALC_
	ADD_ELEMENT(GraphicalCSSearchTool);
#endif
	ADD_ELEMENT(InkSegmentSearchTool);
	ADD_ELEMENT(ImportanceHintSearchTool);
	ADD_ELEMENT(MatchingHintSearchTool);
	ADD_ELEMENT(MediaMetaSearchTool);
	ADD_ELEMENT(MediaReviewSearchTool);
	ADD_ELEMENT(MosaicSearchTool);
	ADD_ELEMENT(OrderingKeySearchTool);
	ADD_ELEMENT(PackageSearchTool);
	ADD_ELEMENT(PhoneticTranslationHintsSearchTool);
#ifndef _VISUALC_
	ADD_ELEMENT(PointOfViewSearchTool);
#endif
//	  ADD_ELEMENT(SegmentSearchTool);
	ADD_ELEMENT(SegmentRelGraphSearchTool);
#ifndef _VISUALC_
	ADD_ELEMENT(SemanticSearchTool);
#endif
	ADD_ELEMENT(SpatialResolutionHintSearchTool);
#ifndef _VISUALC_
	ADD_ELEMENT(StateMemberSearchTool);
#endif
	ADD_ELEMENT(StructuredCollectionSearchTool);
	ADD_ELEMENT(SummarizationSearchTool);
	ADD_ELEMENT(SummaryPreferencesSearchTool);
	ADD_ELEMENT(TimeSearchTool);
	ADD_ELEMENT(UserPreferenceSearchTool);
	//ADD_D_ELEMENT(IntelCorp::FeeDesc, IntelCorp::FeeIdent);
	// etc . . .
#endif // REGISTERCOMPONENTS
}

