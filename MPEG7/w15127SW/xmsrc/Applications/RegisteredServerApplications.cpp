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
// RegisteredServerApplications



#include "BasicArchitecture/XMObjects.h"
#include "XMManager/ComponentDB.h"
#include "Applications/ServerApplications.h"


//----------------------------------------------------------------------------
#define ADD_ELEMENT(DSType)				\
										\
	temp = new DSType;					\
	m_ServerApplications.push_back(temp);

using namespace XM;

//----------------------------------------------------------------------------
void ComponentDatabase::InitServers(void)
{
	// To fully implement this class factory, we need to construct all objects 
	// which are to be evaluated.

#ifdef REGISTERCOMPONENTS
	ServerApplicationABC *temp;

	///////////////////////////////////////////////////////////////////////////
	// Add standard descriptors
	ADD_ELEMENT(ServerApplication);
#ifdef USEDT
	ADD_ELEMENT(DummyTypeServer);
#endif
#ifdef USEDTDS
	ADD_ELEMENT(DSDummyTypeServer);
#endif

	ADD_ELEMENT(TimeSeriesServer);

	ADD_ELEMENT(IIColorServer);
	ADD_ELEMENT(ColorLayoutServer);
	ADD_ELEMENT(ColorStructureServer);
	ADD_ELEMENT(CTBrowsingServer);
	ADD_ELEMENT(DominantColorServer);
	ADD_ELEMENT(GoFGoPColorServer);
	ADD_ELEMENT(ScalableColorServer);

	ADD_ELEMENT(EdgeHistogramServer);
	ADD_ELEMENT(HomogeneousTextureServer);
	ADD_ELEMENT(TextureBrowsingServer);

	ADD_ELEMENT(ShapeSpectrumServer); 
	ADD_ELEMENT(ContourShapeServer);
	ADD_ELEMENT(MultiViewServer);
	ADD_ELEMENT(Perceptual3DShapeServer);
	ADD_ELEMENT(RegionLocatorServer);
	ADD_ELEMENT(RegionShapeServer);
	ADD_ELEMENT(SpatioTemporalLocatorServer);
	ADD_ELEMENT(ShapeVariationServer);

	ADD_ELEMENT(CameraMotionServer);
	ADD_ELEMENT(MotionActivityServer);
	ADD_ELEMENT(MotionTrajectoryServer);
	ADD_ELEMENT(ParametricObjectMotionServer);

	ADD_ELEMENT(FaceRecognitionServer);
	ADD_ELEMENT(AdvancedFaceRecognitionServer);

	ADD_ELEMENT(MultiImageServer);

	ADD_ELEMENT(ImageSignatureServer);
	ADD_ELEMENT(VideoSignatureServer);

	ADD_ELEMENT(SpokenContentServer);

	ADD_ELEMENT(HierarchicalKeyFrameServer);
	ADD_ELEMENT(InkSegmentServer);
	ADD_ELEMENT(MatchingHintServer);
	ADD_ELEMENT(MediaQualityServer);
	ADD_ELEMENT(ModelServer);
	ADD_ELEMENT(MosaicServer);
//	ADD_ELEMENT(SegmentServer);
	ADD_ELEMENT(SegmentRelGraphServer);
//	ADD_ELEMENT(SemanticServer);
	ADD_ELEMENT(SoundPropertyServer);
	ADD_ELEMENT(SpaceFrequencyViewServer);
// 	ADD_ELEMENT(SpatioTemporalDSServer);
	ADD_ELEMENT(StateTransitionModelServer);
	ADD_ELEMENT(SeqSummaryFastMovingStoryBoardServer);
	ADD_ELEMENT(SeqSummaryKeywordsServer);
	ADD_ELEMENT(SequentialSummaryServer);
	ADD_ELEMENT(StructuredCollectionServer);
	ADD_ELEMENT(SummarizationServer);
	ADD_ELEMENT(TranscodingServer);
	ADD_ELEMENT(TimeServer);
	ADD_ELEMENT(VariationServer);
	ADD_ELEMENT(VideoEditingServer);
	ADD_ELEMENT(VideoTextServer);
	//ADD_D_ELEMENT(IntelCorp::FeeDesc, IntelCorp::FeeIdent);
	// etc . . .
#endif // REGISTERCOMPONENTS
}

