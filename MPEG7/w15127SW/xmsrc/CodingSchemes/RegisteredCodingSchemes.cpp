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
// RegisteredCodingSchemes.cpp



#include "BasicArchitecture/XMObjects.h"
#include "XMManager/ComponentDB.h"
#include "CodingSchemes/CodingSchemes.h"


//----------------------------------------------------------------------------
#define ADD_ELEMENT(DSType)				\
										\
	temp = new DSType;					\
	m_CSs.push_back(temp);

using namespace XM;

//----------------------------------------------------------------------------
void ComponentDatabase::InitCSs(void)
{
	// To fully implement this class factory, we need to construct all objects 
	// which are to be evaluated.

#ifdef REGISTERCOMPONENTS
	DescriptionCodingEngineABC *temp;

	///////////////////////////////////////////////////////////////////////////
	// Add standard descriptors
	ADD_ELEMENT(DescriptionCodingEngine);
#ifdef USEDT
	ADD_ELEMENT(DummyTypeCS);
#endif

	ADD_ELEMENT(TimeSeriesCS);
	ADD_ELEMENT(IIColorCS);

	ADD_ELEMENT(ColorSpaceCS);
	ADD_ELEMENT(ColorQuantizerCS);
	ADD_ELEMENT(GridLayoutCS);
	ADD_ELEMENT(Spatial2DCoordinatesCS);
	ADD_ELEMENT(TemporalInterpolationCS);

	ADD_ELEMENT(ColorLayoutCS);
	ADD_ELEMENT(ColorStructureCS);
	ADD_ELEMENT(CTBrowsingCS);
	ADD_ELEMENT(DominantColorCS);
	ADD_ELEMENT(GoFGoPColorCS);
	ADD_ELEMENT(GofGopFeatureCS);
	ADD_ELEMENT(ScalableColorCS);

	ADD_ELEMENT(TextureBrowsingCS);
	ADD_ELEMENT(HomogeneousTextureCS);
	ADD_ELEMENT(EdgeHistogramCS);

	ADD_ELEMENT(ShapeSpectrumCS);
	ADD_ELEMENT(ContourShapeCS);
	ADD_ELEMENT(MultiViewCS);
	ADD_ELEMENT(Perceptual3DShapeCS);
	ADD_ELEMENT(RegionLocatorCS);
	ADD_ELEMENT(RegionShapeCS);
	ADD_ELEMENT(SpatioTemporalLocatorCS);
	ADD_ELEMENT(ShapeVariationCS);

	ADD_ELEMENT(CameraMotionCS);
	ADD_ELEMENT(MotionActivityCS);
	ADD_ELEMENT(MotionTrajectoryCS);
	ADD_ELEMENT(ParametricObjectMotionCS);

	ADD_ELEMENT(FaceRecognitionCS);
	ADD_ELEMENT(AdvancedFaceRecognitionCS);

	ADD_ELEMENT(MultiImageCS);

	//	ADD_ELEMENT(SpokenContentCS);

	// DS elemets
	ADD_ELEMENT(TimePointCS);
	ADD_ELEMENT(TimeUnitCS);
	ADD_ELEMENT(RelTimeCS);
	ADD_ELEMENT(RelIncrTimeCS);
	ADD_ELEMENT(DurationCS);
	ADD_ELEMENT(IncrDurationCS);

	ADD_ELEMENT(GenericDSCS);
//	ADD_ELEMENT(SummarizationCS);

	//ADD_D_ELEMENT(IntelCorp::FeeDesc, IntelCorp::FeeIdent);
	// etc . . .

#endif // REGISTERCOMPONETS
}

