
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
// Copyright (c) 1998-1999.
//  
// This notice must be included in all copies or derivative works.
// 
// RegisteredClientApplications.cpp



#include "BasicArchitecture/XMObjects.h"
#include "XMManager/ComponentDB.h"
#include "Applications/ClientApplications.h"



//----------------------------------------------------------------------------
#define ADD_ELEMENT(DSType)	\
	temp = new DSType;	\
	m_ClientApplications.push_back(temp);

using namespace XM;

//----------------------------------------------------------------------------
void ComponentDatabase::InitClients(void)
{
	// To fully implement this class factory, we need to construct all objects 
	// which are to be evaluated.

#ifdef REGISTERCOMPONENTS
	XM::ClientApplicationABC *temp;

	///////////////////////////////////////////////////////////////////////////
	// Add standard descriptors
	ADD_ELEMENT(ClientApplication);
#ifdef USEDT
	ADD_ELEMENT(DummyTypeClient);
#endif
#ifdef USEDTDS
	ADD_ELEMENT(DSDummyTypeClient);
#endif

	ADD_ELEMENT(TimeSeriesClient);

	ADD_ELEMENT(ColorLayoutClient);
 	ADD_ELEMENT(ColorStructureClient);
	ADD_ELEMENT(CTBrowsingClient);
	ADD_ELEMENT(DominantColorClient);
	ADD_ELEMENT(DominantColorTemperatureClient);
	ADD_ELEMENT(GoFGoPColorClient);
	ADD_ELEMENT(ScalableColorClient);

	ADD_ELEMENT(TextureBrowsingClient);
	ADD_ELEMENT(HomogeneousTextureClient);
	ADD_ELEMENT(EdgeHistogramClient);

	ADD_ELEMENT(ShapeSpectrumClient);
	ADD_ELEMENT(ContourShapeClient);
	ADD_ELEMENT(MultiViewClient);
	ADD_ELEMENT(Perceptual3DShapeClient);
	ADD_ELEMENT(RegionLocatorClient);
	ADD_ELEMENT(RegionShapeClient);
	ADD_ELEMENT(SpatioTemporalLocatorClient);
	ADD_ELEMENT(ShapeVariationClient);	

	ADD_ELEMENT(CameraMotionClient);
	ADD_ELEMENT(MotionActivityClient);
	ADD_ELEMENT(MotionTrajectoryClient);
	ADD_ELEMENT(ParametricObjectMotionClient);

	ADD_ELEMENT(FaceRecognitionClient);
	ADD_ELEMENT(AdvancedFaceRecognitionClient);

	ADD_ELEMENT(MultiImageClient);

	ADD_ELEMENT(ImageSignatureClient);
	ADD_ELEMENT(VideoSignatureClient);

	ADD_ELEMENT(AudioSignatureClient);
	ADD_ELEMENT(AudioSpectralFlatnessClient);
	ADD_ELEMENT(MelodySequenceClient);
	//	ADD_ELEMENT(SpokenContentClient);

	ADD_ELEMENT(AffectiveSegmentFilterClient);
	ADD_ELEMENT(DependStructureClient);
	ADD_ELEMENT(ExtendedTextualTypeClient);
	ADD_ELEMENT(GraphicalCSClient);
	ADD_ELEMENT(ImportanceHintClient);
	ADD_ELEMENT(InkSegmentClient);
	ADD_ELEMENT(MatchingHintClient);
	ADD_ELEMENT(MediaMetaClient);
	ADD_ELEMENT(MediaReviewClient);
 	ADD_ELEMENT(MosaicClient);
	ADD_ELEMENT(OrderingKeyClient);
	ADD_ELEMENT(PackageClient);
	ADD_ELEMENT(PhoneticTranslationHintsClient);
	ADD_ELEMENT(PointOfViewClient) ;         // Add by Matsushita
	ADD_ELEMENT(SegmentClient);
	ADD_ELEMENT(SegmentRelGraphClient);
	ADD_ELEMENT(SemanticClient);
	ADD_ELEMENT(SpatialResolutionHintClient);
	ADD_ELEMENT(StateMemberClient);
	ADD_ELEMENT(StructuredCollectionClient);
	ADD_ELEMENT(SummarizationClient);
	ADD_ELEMENT(SummaryPreferencesClient);
	ADD_ELEMENT(TimeClient);
	ADD_ELEMENT(UserPreferenceClient);

	//ADD_D_ELEMENT(IntelCorp::FeeDesc, IntelCorp::FeeIdent);
	// etc . . .
#endif // REGISTERCOMPONENTS
}
