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
// RegisteredExtractionUtilities.cpp


#include "BasicArchitecture/XMObjects.h"
#include "XMManager/ComponentDB.h"
#include "ExtractionUtilities/ExtractionUtilities.h"


//----------------------------------------------------------------------------
#define ADD_ELEMENT(DSType)				\
										\
	temp = new DSType;					\
	m_DescriptorExtractors.push_back(temp);

using namespace XM;

//----------------------------------------------------------------------------
void ComponentDatabase::InitExtractors(void)
{
	// To fully implement this class factory, we need to construct all objects 
	// which are to be evaluated.

#ifdef REGISTERCOMPONENTS
	DescriptorExtractorABC *temp;

	///////////////////////////////////////////////////////////////////////////
	// Add standard descriptors
	ADD_ELEMENT(DescriptorExtractor);
#ifdef USEDT
	ADD_ELEMENT(DummyTypeExtractionTool);
#endif
#ifdef USEDTDS
	ADD_ELEMENT(DSDummyTypeExtractionTool);
#endif

	ADD_ELEMENT(IIColorExtractionTool);
	ADD_ELEMENT(TimeSeriesExtractionTool);

	ADD_ELEMENT(ColorLayoutExtractionTool);
	ADD_ELEMENT(ColorStructureExtractionTool);
	ADD_ELEMENT(CTBrowsingExtractionTool);
	ADD_ELEMENT(DominantColorExtractionTool);
	ADD_ELEMENT(GoFGoPColorExtractionTool);
	ADD_ELEMENT(ScalableColorExtractionTool);
	ADD_ELEMENT(TemporalInterpolationExtractionTool);

	ADD_ELEMENT(EdgeHistogramExtractionTool);
	ADD_ELEMENT(HomogeneousTextureExtractionTool);
	ADD_ELEMENT(TextureBrowsingExtractionTool);

	ADD_ELEMENT(ShapeSpectrumExtractionTool);
	ADD_ELEMENT(ContourShapeExtractionTool);
	ADD_ELEMENT(MultiViewExtractionTool);
	ADD_ELEMENT(Perceptual3DShapeExtractionTool);
	ADD_ELEMENT(RegionLocatorExtractionTool);
	ADD_ELEMENT(RegionShapeExtractionTool);
	ADD_ELEMENT(SpatioTemporalLocatorExtractionTool);
	ADD_ELEMENT(ShapeVariationExtractionTool);

	ADD_ELEMENT(CameraMotionExtractionTool);
	ADD_ELEMENT(MotionActivityExtractionTool);
	ADD_ELEMENT(MotionTrajectoryExtractionTool);
	ADD_ELEMENT(ParametricObjectMotionExtractionTool);

	ADD_ELEMENT(FaceRecognitionExtractionTool);
	ADD_ELEMENT(AdvancedFaceRecognitionExtractionTool);

	ADD_ELEMENT(MultiImageExtractionTool);

	ADD_ELEMENT(ImageSignatureExtractionTool);
	ADD_ELEMENT(VideoSignatureExtractionTool);

	ADD_ELEMENT(SpokenContentExtractionTool);

	ADD_ELEMENT(HierarchicalKeyFrameExtractionTool);
	ADD_ELEMENT(InkSegmentExtractionTool);
	ADD_ELEMENT(MediaQualityExtractionTool);
	ADD_ELEMENT(MatchingHintExtractionTool);
	ADD_ELEMENT(ModelExtractionTool);
	ADD_ELEMENT(MosaicExtractionTool);
//	ADD_ELEMENT(SegmentExtractionTool);
	ADD_ELEMENT(SegmentRelGraphExtractionTool);
//	ADD_ELEMENT(SemanticExtractionTool);
	ADD_ELEMENT(SeqSummaryFastMovingStoryBoardExtractionTool);
	ADD_ELEMENT(SeqSummaryKeywordsExtractionTool);
	ADD_ELEMENT(SequentialSummaryExtractionTool);
	ADD_ELEMENT(SoundPropertyExtractionTool);
	ADD_ELEMENT(SpaceFrequencyViewExtractionTool);
// 	ADD_ELEMENT(SpatioTemporalDSExtractionTool);
	ADD_ELEMENT(StateTransitionModelExtractionTool);
	ADD_ELEMENT(StructuredCollectionExtractionTool);
	ADD_ELEMENT(SummarizationExtractionTool);
	ADD_ELEMENT(TimeExtractionTool);
	ADD_ELEMENT(TranscodingExtractionTool);
	ADD_ELEMENT(VariationExtractionTool);
	ADD_ELEMENT(VideoEditingExtractionTool);
	ADD_ELEMENT(VideoTextExtractionTool);
	//ADD_D_ELEMENT(IntelCorp::FeeDesc, IntelCorp::FeeIdent);
	// etc . . .
#endif // REGISTERCOMPONENTS
}

