///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric R&D Centre Europe
// P. Brasnett, W. Price
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
// purpose, assign or donate the code to a third party and inhibit third parties// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ImageSignature.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID ImageSignatureDescriptorInterface::myID = UUID("");
const char *ImageSignatureDescriptorInterface::myName = "Image Signature Description Interface";

const UUID ImageSignatureDescriptor::myID = UUID("");
const char *ImageSignatureDescriptor::myName = "Image Signature Descriptor";

const UUID ImageSignatureDescriptor::valID = UUID("");
const char *ImageSignatureDescriptor::valName = "Image Signature Descriptor";

const UUID ImageSignatureDescriptorInterfaceABC::myID = UUID();

ImageSignatureDescriptorInterfaceABC::ImageSignatureDescriptorInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================

ImageSignatureDescriptorInterface::ImageSignatureDescriptorInterface(ImageSignatureDescriptor *pImageSignature)
: m_pImageSignatureDescriptor(pImageSignature)
{
  SetInterfaceID(myID);
}

//-----------------------------------------------------------------------------
ImageSignatureDescriptorInterface::~ImageSignatureDescriptorInterface()
{
}

//-----------------------------------------------------------------------------
void ImageSignatureDescriptorInterface::addref(void)
{
  m_pImageSignatureDescriptor->addref();
}

//-----------------------------------------------------------------------------
void ImageSignatureDescriptorInterface::release(void)
{
  m_pImageSignatureDescriptor->release();
}

//-----------------------------------------------------------------------------
const UUID& ImageSignatureDescriptorInterface::GetInterfaceID(void)
{
  return myID;
}

//-----------------------------------------------------------------------------
const char *ImageSignatureDescriptorInterface::GetName(void)
{
  return myName;
}

//-----------------------------------------------------------------------------
void ImageSignatureDescriptorInterface::SetGlobalSignature(ImageSignatureGlobalBits bits)
{
  m_pImageSignatureDescriptor->SetGlobalSignature(bits);
}

//-----------------------------------------------------------------------------
void ImageSignatureDescriptorInterface::GetGlobalSignature(ImageSignatureGlobalBits &bits) const
{
  m_pImageSignatureDescriptor->GetGlobalSignature(bits);
}

//-----------------------------------------------------------------------------
void ImageSignatureDescriptorInterface::SetAspectSignature(ImageSignatureGlobalBits bits)
{
  m_pImageSignatureDescriptor->SetAspectSignature(bits);
}

//-----------------------------------------------------------------------------
void ImageSignatureDescriptorInterface::GetAspectSignature(ImageSignatureGlobalBits &bits) const
{
  m_pImageSignatureDescriptor->GetAspectSignature(bits);
}
//-----------------------------------------------------------------------------
void ImageSignatureDescriptorInterface::SetFeatureSignature(ImageSignatureBits bits)
{
  m_pImageSignatureDescriptor->SetFeatureSignature(bits);
}

//-----------------------------------------------------------------------------
void ImageSignatureDescriptorInterface::GetFeatureSignature(ImageSignatureBits &bits) const
{
  m_pImageSignatureDescriptor->GetFeatureSignature(bits);
}

//-----------------------------------------------------------------------------
unsigned long ImageSignatureDescriptorInterface::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  return m_pImageSignatureDescriptor->ExportDDL(aParentDescription);
}

//-----------------------------------------------------------------------------
unsigned long ImageSignatureDescriptorInterface::ImportDDL(GenericDSInterfaceABC *aDescription)
{
  return m_pImageSignatureDescriptor->ImportDDL(aDescription);
}

//=============================================================================

ImageSignatureDescriptor::ImageSignatureDescriptor()
: m_refcount(0)
, m_Interface(this)
, m_isProprietary(false)
{
  memset(m_bits, 0, sizeof(m_bits));
  memset(m_aspect_bits, 0, sizeof(m_aspect_bits));
  memset(m_feature_bits, 0, sizeof(m_feature_bits));
  SetExposedInterface(&m_Interface);
}

//-----------------------------------------------------------------------------
ImageSignatureDescriptor::~ImageSignatureDescriptor()
{
}

//-----------------------------------------------------------------------------
const UUID& ImageSignatureDescriptor::GetObjectID(void)
{
  return myID;
}

//-----------------------------------------------------------------------------
const char *ImageSignatureDescriptor::GetName(void)
{
  return myName;
}

//-----------------------------------------------------------------------------
const UUID& ImageSignatureDescriptor::GetValueID(void)
{
  return valID;
}

//-----------------------------------------------------------------------------
const char* ImageSignatureDescriptor::GetValueName(void)
{
  return valName;
}

//-----------------------------------------------------------------------------
unsigned long ImageSignatureDescriptor::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
unsigned int i;
  if (!aParentDescription)
    return (unsigned long)-1;

  GenericDS l_DDLDescription;
  l_DDLDescription = aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type", "ImageSignatureType");

  GenericDS ImageSignature_global = l_DDLDescription.CreateChild("GlobalSignatureA");
  vector<int> signature(IMAGESIGNATURE_BITS_GLOBAL);
  ImageSignatureGlobalBits bits;
  GetGlobalSignature(bits);
  for (i = 0; i < IMAGESIGNATURE_BITS_GLOBAL; i++)
    signature[i] = (bits[(i>>3)] >> (7 - (i & 0x7))) & 0x1;
  ImageSignature_global.SetValue(signature);

  GenericDS ImageSignature_aspect = l_DDLDescription.CreateChild("GlobalSignatureB");
  vector<int> a_signature(IMAGESIGNATURE_BITS_GLOBAL);
  ImageSignatureGlobalBits a_bits;
  GetAspectSignature(a_bits);
  for ( i = 0; i < IMAGESIGNATURE_BITS_GLOBAL; i++)
    a_signature[i] = (a_bits[(i>>3)] >> (7 - (i & 0x7))) & 0x1;
  ImageSignature_aspect.SetValue(a_signature);


  GenericDS LocalSignature = l_DDLDescription.CreateChild("LocalSignature");
  GenericDS ImageSignature_feature_cnt = LocalSignature.CreateChild("FeaturePointCount");
  
  ImageSignatureBits f_bits;
  GetFeatureSignature(f_bits);

  // 2015/02/16: KW fix - use variable number of feature points (32 - 80) instead of fixed 80.
  //int feature_cnt=(IMAGESIGNATURE_BITS/88)-32;
  int feature_cnt=0;

  for(i = 32; i < 80; i++)
  {
	  int x = f_bits[i*11];
	  if(x == 255)
		  break;
	  feature_cnt++;
  }

  ImageSignature_feature_cnt.SetValue(feature_cnt);

  int byte_cnt=0;
  for(int ft=0;ft<feature_cnt+32;ft++)
  {
	GenericDS ImageSignature_feature = LocalSignature.CreateChild("FeaturePoint");
	GenericDS xCoord = ImageSignature_feature.CreateChild("XCoord");
	GenericDS yCoord = ImageSignature_feature.CreateChild("YCoord");
	GenericDS theta = ImageSignature_feature.CreateChild("Direction");
	GenericDS localSignature = ImageSignature_feature.CreateChild("LocalSignature");
	xCoord.SetValue(f_bits[byte_cnt++]);
	yCoord.SetValue(f_bits[byte_cnt++]);
	theta.SetValue(f_bits[byte_cnt++]);
	vector<int> f_signature(60);
	for ( i = 0; i < 60; i++)
		//KW 2013/11/27 bug: f_signature[i] = (f_bits[((byte_cnt+i)>>3)] >> (7 - ((byte_cnt+i) & 0x7))) & 0x1;
		f_signature[i] = (f_bits[byte_cnt+(i>>3)] >> (7 - (i & 0x7))) & 0x1;
	byte_cnt=byte_cnt+8;
	localSignature.SetValue(f_signature);
  }
  //for ( i = 0; i < IMAGESIGNATURE_BITS; i++)
  //  f_signature[i] = (f_bits[(i>>3)] >> (7 - (i & 0x7))) & 0x1;
  
  
  return 0;
}

//-----------------------------------------------------------------------------
unsigned long ImageSignatureDescriptor::ImportDDL(GenericDSInterfaceABC *aDescription)
{
  unsigned int i, j;
  if (!aDescription)
    return (unsigned long)-1;

  GenericDS l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;

  string xsitype;
  if (aDescription->GetDSName() == "Descriptor")
  {
    aDescription->GetTextAttribute("xsi:type", xsitype);
    if (xsitype == "ImageSignatureType")
    {
      l_DDLDescriptionInterface = aDescription;
    }
  }

  if (!l_DDLDescriptionInterface)
  {
    l_DDLDescription = aDescription->GetDescription("Descriptor");

    while (!l_DDLDescription.isNull())
    {
      l_DDLDescription.GetTextAttribute("xsi:type", xsitype);
      if (xsitype == "ImageSignatureType")
        break;
      l_DDLDescription = l_DDLDescription.GetNextSibling("Descriptor");
    }

    if (l_DDLDescription.isNull())
      return (unsigned long)-1;

    l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
  }

  GenericDS ImageSignature_global = l_DDLDescriptionInterface->GetDescription("GlobalSignatureA");
  vector<int> signature;
  ImageSignature_global.GetIntVector(signature);
  ImageSignatureGlobalBits bits;
  memset(bits, 0, sizeof(bits));
  for (i = 0; i < IMAGESIGNATURE_BITS_GLOBAL; i++)
    bits[(i>>3)] |= (signature[i] << (7-(i&7)));
  SetGlobalSignature(bits);

  GenericDS ImageSignature_aspect = l_DDLDescriptionInterface->GetDescription("GlobalSignatureB");
  vector<int> a_signature;
  ImageSignature_aspect.GetIntVector(a_signature);
  ImageSignatureGlobalBits a_bits;
  memset(a_bits, 0, sizeof(a_bits));
  for ( i = 0; i < IMAGESIGNATURE_BITS_GLOBAL; i++)
    a_bits[(i>>3)] |= (a_signature[i] << (7-(i&7)));
  SetAspectSignature(a_bits);

  GenericDS LocalSignature = l_DDLDescriptionInterface->GetDescription("LocalSignature");

  GenericDS ImageSignature_feature_cnt = LocalSignature.GetDescription("FeaturePointCount");
  int feature_cnt=0;
  ImageSignature_feature_cnt.GetIntValue(feature_cnt);

  ImageSignatureBits f_bits;
  memset(f_bits, 0, sizeof(f_bits));
  
  int byte_cnt=0;
  GenericDS FeaturePoint = LocalSignature.GetDescription("FeaturePoint");
  for( i=0; i<feature_cnt+32; i++)
  {
	  if(FeaturePoint.isNull()) break;
	  GenericDS ImageSignature_xCoord = FeaturePoint.GetDescription("XCoord");
	  int xCoord;
	  ImageSignature_xCoord.GetIntValue(xCoord);
	  f_bits[byte_cnt++]=(unsigned char)xCoord;
	  //printf("%d", xCoord);

	  GenericDS ImageSignature_yCoord = FeaturePoint.GetDescription("YCoord");
	  int yCoord;
	  ImageSignature_yCoord.GetIntValue(yCoord);
	  f_bits[byte_cnt++]=(unsigned char)yCoord;

	  GenericDS ImageSignature_theta = FeaturePoint.GetDescription("Direction");
	  int theta;
	  ImageSignature_theta.GetIntValue(theta);
	  f_bits[byte_cnt++]=(unsigned char)theta;


	  GenericDS ImageSignature_feature = FeaturePoint.GetDescription("LocalSignature");
	  vector<int> f_signature;
	  ImageSignature_feature.GetIntVector(f_signature);
	  for ( j = 0; j < 60; j++)
		f_bits[byte_cnt+(j>>3)] |= (f_signature[j] << (7-(j&7)));

	  byte_cnt+=8;
	  FeaturePoint = FeaturePoint.GetNextSibling("FeaturePoint");
  }
  SetFeatureSignature(f_bits);

  return 0;
}

//-----------------------------------------------------------------------------
bool ImageSignatureDescriptor::IsProprietary(void)
{
  return m_isProprietary;
}

//-----------------------------------------------------------------------------
void ImageSignatureDescriptor::addref(void)
{
#ifdef VERBOSE
  fprintf(stderr, "ImageSignature connect\n");
#endif // VERBOSE
  m_refcount++;
}

//-----------------------------------------------------------------------------
void ImageSignatureDescriptor::release(void)
{
#ifdef VERBOSE
  fprintf(stderr, "ImageSignature release\n");
#endif // VERBOSE
  if (!(--m_refcount)) delete this;
}

//-----------------------------------------------------------------------------
ImageSignatureDescriptorInterfaceABC *ImageSignatureDescriptor::GetInterface(void)
{
  return &m_Interface;
}

//-----------------------------------------------------------------------------
void ImageSignatureDescriptor::SetGlobalSignature(ImageSignatureGlobalBits bits)
{
  memcpy(m_bits, bits, sizeof(m_bits));
}

//-----------------------------------------------------------------------------
void ImageSignatureDescriptor::GetGlobalSignature(ImageSignatureGlobalBits &bits) const
{
  memcpy(bits, m_bits, sizeof(m_bits));
}

//-----------------------------------------------------------------------------
void ImageSignatureDescriptor::SetAspectSignature(ImageSignatureGlobalBits bits)
{
  memcpy(m_aspect_bits, bits, sizeof(m_aspect_bits));
}

//-----------------------------------------------------------------------------
void ImageSignatureDescriptor::GetAspectSignature(ImageSignatureGlobalBits &bits) const
{
  memcpy(bits, m_aspect_bits, sizeof(m_aspect_bits));
}
//-----------------------------------------------------------------------------
void ImageSignatureDescriptor::SetFeatureSignature(ImageSignatureBits bits)
{
  memcpy(m_feature_bits, bits, sizeof(m_feature_bits));
}

//-----------------------------------------------------------------------------
void ImageSignatureDescriptor::GetFeatureSignature(ImageSignatureBits &bits) const
{
  memcpy(bits, m_feature_bits, sizeof(m_feature_bits));
}


