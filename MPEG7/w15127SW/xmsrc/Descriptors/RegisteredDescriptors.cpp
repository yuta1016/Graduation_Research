///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Intel Corporation
// TU-Munich, S. Herrmann
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
// RegisteredDescriptors.cpp



#include "BasicArchitecture/XMObjects.h"
#include "XMManager/ComponentDB.h"
#include "Descriptors/Descriptors.h"

//----------------------------------------------------------------------------
#define ADD_ELEMENT(DSType)				\
										\
	temp = new DSType;					\
	m_Ds.push_back(temp);

using namespace XM;

//----------------------------------------------------------------------------
void ComponentDatabase::InitDs(void)
{
	// To fully implement this class factory, we need to construct all objects 
	// which are to be evaluated.

#ifdef REGISTERCOMPONENTS
	DescriptorABC *temp;

	///////////////////////////////////////////////////////////////////////////
	// Add standard descriptors
	ADD_ELEMENT(Descriptor);
#ifdef USEDT
	ADD_ELEMENT(DummyTypeDescriptor);
#endif

	ADD_ELEMENT(TimeSeriesDescriptor );

	ADD_ELEMENT(ColorSpaceDescriptor);
	ADD_ELEMENT(ColorQuantizerDescriptor);
	ADD_ELEMENT(GridLayoutDescriptor);
	ADD_ELEMENT(Spatial2DCoordinatesDescriptor );
	ADD_ELEMENT(TemporalInterpolationDescriptor );

	ADD_ELEMENT(ColorLayoutDescriptor);
	ADD_ELEMENT(ColorStructureDescriptor);
	ADD_ELEMENT(CTBrowsingDescriptor);
	ADD_ELEMENT(DominantColorDescriptor);
	ADD_ELEMENT(GoFGoPColorDescriptor);
	ADD_ELEMENT(GofGopFeatureDescriptor);
	ADD_ELEMENT(IIColorDescriptor);
	ADD_ELEMENT(ScalableColorDescriptor);

	ADD_ELEMENT(TextureBrowsingDescriptor);
	ADD_ELEMENT(HomogeneousTextureDescriptor);
	ADD_ELEMENT(EdgeHistogramDescriptor);

	ADD_ELEMENT(ShapeSpectrumDescriptor);
	ADD_ELEMENT(ContourShapeDescriptor);
	ADD_ELEMENT(MultiViewDescriptor);
	ADD_ELEMENT(Perceptual3DShapeDescriptor);
	ADD_ELEMENT(RegionLocatorDescriptor );
	ADD_ELEMENT(RegionShapeDescriptor );
	ADD_ELEMENT(SpatioTemporalLocatorDescriptor );
	ADD_ELEMENT(ShapeVariationDescriptor);

	ADD_ELEMENT(CameraMotionDescriptor);
	ADD_ELEMENT(MotionActivityDescriptor);
	ADD_ELEMENT(MotionTrajectoryDescriptor);
	ADD_ELEMENT(ParametricObjectMotionDescriptor);

	ADD_ELEMENT(FaceRecognitionDescriptor);
	ADD_ELEMENT(AdvancedFaceRecognitionDescriptor);

	ADD_ELEMENT(MultiImageDescriptor);

	ADD_ELEMENT(ImageSignatureDescriptor);
	ADD_ELEMENT(VideoSignatureDescriptor);

	ADD_ELEMENT(SpokenContentDescriptor);

	// DS elemets
	ADD_ELEMENT(TimePointDescriptor);
	ADD_ELEMENT(TimeUnitAttrGroup);
	//ADD_ELEMENT(PointOfViewDescriptor) ; // Add by Matsushita
	ADD_ELEMENT(RelTimeDescriptor);
	ADD_ELEMENT(RelIncrTimeDescriptor);
	ADD_ELEMENT(DurationDescriptor);
	ADD_ELEMENT(IncrDurationDescriptor);

	//ADD_D_ELEMENT(IntelCorp::FeeDesc, IntelCorp::FeeIdent);
	// etc . . .
#endif // REGISTERCOMPONENTS
}

