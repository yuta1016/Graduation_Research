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
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
// 
// RegisteredDecoders.cpp



#include "BasicArchitecture/XMObjects.h"
#include "XMManager/ComponentDB.h"


//----------------------------------------------------------------------------
#define ADD_ELEMENT(DSType)				\
										\
	temp = new DSType;					\
	m_Decoders.push_back(temp);

using namespace XM;

//----------------------------------------------------------------------------
void ComponentDatabase::InitDecoders(void)
{
	// To fully implement this class factory, we need to construct all objects 
	// which are to be evaluated.

#ifdef REGISTERCOMPONENTS
	DecoderABC *temp;

	///////////////////////////////////////////////////////////////////////////
	// Add standard descriptors
	ADD_ELEMENT(Decoder);
	//ADD_D_ELEMENT(IntelCorp::FeeDesc, IntelCorp::FeeIdent);
	// etc . . .
#endif // REGISTERCOMPONENTS
}

