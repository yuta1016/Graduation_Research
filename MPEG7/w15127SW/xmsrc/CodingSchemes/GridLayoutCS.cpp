///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Akio YAMADA  NEC Corp.
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  GridLayoutCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Descriptors/VisualDescriptorID.h"
#include "Utilities/BitBuffer.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef WIN32
#include <netinet/in.h>
#endif
 

using namespace XM;

const UUID GridLayoutCSInterface::myID = UUID("");
const char *GridLayoutCSInterface::myName =
                         "GridLayoutCodingSchemeInterface";

// create a UUID for your type
const UUID GridLayoutCS::myID = UUID("");
const char *GridLayoutCS::myName = "GridLayoutCodingScheme";

const UUID GridLayoutCS::valID = UUID("");
const char *GridLayoutCS::valName = "";

const UUID GridLayoutCSInterfaceABC::myID = UUID("");

GridLayoutCSInterfaceABC::GridLayoutCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
GridLayoutCSInterface::GridLayoutCSInterface(GridLayoutCS *aGridLayout):
m_GridLayoutCS(aGridLayout)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
GridLayoutCSInterface::~GridLayoutCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& GridLayoutCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GridLayoutCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void GridLayoutCSInterface::destroy(void)
{ delete m_GridLayoutCS; }

//----------------------------------------------------------------------------
BitBuffer *GridLayoutCSInterface::GetEncoderStreamBuffer(void)
{
  return m_GridLayoutCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int GridLayoutCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_GridLayoutCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *GridLayoutCSInterface::GetDecoderStreamBuffer(void)
{
  return m_GridLayoutCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int GridLayoutCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_GridLayoutCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
GridLayoutDescriptorInterfaceABC
*GridLayoutCSInterface::GetDescriptorInterface(void)

{
  return m_GridLayoutCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int GridLayoutCSInterface::
SetDescriptorInterface(GridLayoutDescriptorInterfaceABC
		       *aGridLayoutDescriptorInterface)
{
  return m_GridLayoutCS->
    SetDescriptorInterface(aGridLayoutDescriptorInterface);
}

//----------------------------------------------------------------------------
int GridLayoutCSInterface::StartEncode(void)
{
  return m_GridLayoutCS->StartEncode();
}

//----------------------------------------------------------------------------
int GridLayoutCSInterface::StartDecode(void)
{
  return m_GridLayoutCS->StartDecode();
}

//=============================================================================
GridLayoutCS::GridLayoutCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GridLayoutCS::GridLayoutCS(GridLayoutDescriptorInterfaceABC* aGridLayout):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL)
{
  /* create descriptor if it does not exist*/
  if (!aGridLayout) {
    GridLayoutDescriptor *descriptor = new GridLayoutDescriptor;
      aGridLayout=descriptor->GetInterface();
  }

  /* connect descritors with CS*/
  SetDescriptorInterface(aGridLayout);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GridLayoutCS::~GridLayoutCS()
{
  /* release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& GridLayoutCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GridLayoutCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& GridLayoutCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* GridLayoutCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool GridLayoutCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * GridLayoutCS::GetEncoderStreamBuffer(void)
{
  return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int GridLayoutCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_EncoderBuffer = aBuffer;
  return 0;
}

//----------------------------------------------------------------------------
BitBuffer * GridLayoutCS::GetDecoderStreamBuffer(void)
{
  return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int GridLayoutCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_DecoderBuffer = aBuffer;
  return 0;
}

//----------------------------------------------------------------------------
GridLayoutDescriptorInterfaceABC *GridLayoutCS::
GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int GridLayoutCS::
SetDescriptorInterface(GridLayoutDescriptorInterfaceABC
		       *aGridLayoutDescriptorInterface)
{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == 
      aGridLayoutDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) 
    m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = 
    aGridLayoutDescriptorInterface;
  if (m_DescriptorInterface) {
    m_DescriptorInterface->addref();

  } else {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int GridLayoutCS::StartEncode(void)
{
  long v;

  // Is the file IO set up? If not return


  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"GridLayoutCS::StartEncode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"GridLayoutCS::StartEncode: Interface not initialised\n");
    return -1;
  }

  int part_number_v = GetDescriptorInterface()->GetPartNumberV();
  int part_number_h = GetDescriptorInterface()->GetPartNumberH();
  //int descriptor_mask[part_number_v*part_number_h];
  // Fix - set the table to have the maximum size
  int descriptor_mask[256*256];
  int descriptor_mask_present;

  int x, y;

  descriptor_mask_present=0;
  for(y=0; y<part_number_v; y++){
    for(x=0; x<part_number_h; x++){
      if(GetDescriptorInterface()->GetContents(x, y)){
	descriptor_mask[y*part_number_h+x]=1;
      } else {
        descriptor_mask[y*part_number_h+x]=0;
        descriptor_mask_present=1;
      }
    }
  }

  char *content_name=GetDescriptorInterface()->GetContentName();

  // node: It is strongly requested to harmonize the name of each descriptor; Descriptor::valName
  int descriptor_id=0xffff;
  if(strcmp(content_name, "DominantColorType")==0) descriptor_id=ID_DominantColor;
  else if(strcmp(content_name, "ScalableColorType")==0) descriptor_id=ID_ScalableColor;
  else if(strcmp(content_name, "ColorLayoutType")==0) descriptor_id=ID_ColorLayout;
  else if(strcmp(content_name, "ColorStructureType")==0) descriptor_id=ID_ColorStructure;
  else if(strcmp(content_name, "GoFColorHistogramType")==0) descriptor_id=ID_GoFGoPColor;
  else if(strcmp(content_name, "HomogeneousTextureType")==0) descriptor_id=ID_HomogeneousTexture;
  else if(strcmp(content_name, "TextureBrowsingType")==0) descriptor_id=ID_TextureBrowsing;
  else if(strcmp(content_name, "EdgeHistogramType")==0) descriptor_id=ID_EdgeHistogram;
  else if(strcmp(content_name, "RegionShapeType")==0) descriptor_id=ID_RegionShape;
  else if(strcmp(content_name, "ContourShapeType")==0) descriptor_id=ID_ContourShape;
  else if(strcmp(content_name, "Shape3DType")==0) descriptor_id=ID_Shape3D;
  else {
    fprintf(stderr, "Check the descriptor name (%s)\n", content_name);
    exit(0);
  }

  m_EncoderBuffer->PutBitsLong(descriptor_id, 8);
  m_EncoderBuffer->PutBitsLong(part_number_h, 8);
  m_EncoderBuffer->PutBitsLong(part_number_v, 8);
  m_EncoderBuffer->PutBitsLong(descriptor_mask_present, 1);
  if(descriptor_mask_present){
    for(y=0; y<part_number_v; y++){
      for(x=0; x<part_number_h; x++){
	  m_EncoderBuffer->PutBitsLong(descriptor_mask[y*part_number_h+x], 1);
      }
    }
  }
  for(y=0; y<part_number_v; y++){
    for(x=0; x<part_number_h; x++){
      if(descriptor_mask[y*part_number_h+x]){
	switch(descriptor_id){
	case ID_DominantColor:
	  {
	    DominantColorDescriptorInterfaceABC *theDescriptorInterface=(DominantColorDescriptorInterfaceABC *)GetDescriptorInterface()->GetContents(x, y);
	    DominantColorCS *theCS = new DominantColorCS(theDescriptorInterface);
	    DominantColorCSInterfaceABC *theCSInterface=theCS->GetInterface();
	    theCSInterface->SetEncoderStreamBuffer(m_EncoderBuffer);
	    theCSInterface->StartEncode();
        theCSInterface->destroy();
	    break;
	  }
	case ID_ScalableColor:
	  {
	    ScalableColorDescriptorInterfaceABC *theDescriptorInterface=(ScalableColorDescriptorInterfaceABC*)GetDescriptorInterface()->GetContents(x, y); 
	    ScalableColorCS *theCS = new ScalableColorCS(theDescriptorInterface);
	    ScalableColorCSInterfaceABC *theCSInterface=theCS->GetInterface();
	    theCSInterface->SetEncoderStreamBuffer(m_EncoderBuffer);
	    theCSInterface->StartEncode();
        theCSInterface->destroy();
	    break;
	  }
	case ID_ColorLayout:
	  {
	    ColorLayoutDescriptorInterfaceABC *theDescriptorInterface=(ColorLayoutDescriptorInterfaceABC *)GetDescriptorInterface()->GetContents(x, y);
	    ColorLayoutCS *theCS = new ColorLayoutCS(theDescriptorInterface);
	    ColorLayoutCSInterfaceABC *theCSInterface=theCS->GetInterface();
	    theCSInterface->SetEncoderStreamBuffer(m_EncoderBuffer);
	    theCSInterface->StartEncode();
        theCSInterface->destroy();
	    break;
	  }
	case ID_ColorStructure:
	  {
	    ColorStructureDescriptorInterfaceABC *theDescriptorInterface=(ColorStructureDescriptorInterfaceABC *)GetDescriptorInterface()->GetContents(x, y);
	    ColorStructureCS *theCS = new ColorStructureCS(theDescriptorInterface);
	    ColorStructureCSInterfaceABC *theCSInterface=theCS->GetInterface();
	    theCSInterface->SetEncoderStreamBuffer(m_EncoderBuffer);
	    theCSInterface->StartEncode();
        theCSInterface->destroy();
	    break;
	  }
	case ID_GoFGoPColor:
	  {
	    GoFGoPColorDescriptorInterfaceABC *theDescriptorInterface=(GoFGoPColorDescriptorInterfaceABC *)GetDescriptorInterface()->GetContents(x, y);
	    GoFGoPColorCS *theCS = new GoFGoPColorCS(theDescriptorInterface);
	    GoFGoPColorCSInterfaceABC *theCSInterface=theCS->GetInterface();
	    theCSInterface->SetEncoderStreamBuffer(m_EncoderBuffer);
	    theCSInterface->StartEncode();
        theCSInterface->destroy();
	    break;
	  }
	case ID_HomogeneousTexture:
	  {
	    HomogeneousTextureDescriptorInterfaceABC *theDescriptorInterface=(HomogeneousTextureDescriptorInterfaceABC *)GetDescriptorInterface()->GetContents(x, y);
	    HomogeneousTextureCS *theCS = new HomogeneousTextureCS(theDescriptorInterface);
	    HomogeneousTextureCSInterfaceABC *theCSInterface=theCS->GetInterface();
	    theCSInterface->SetEncoderStreamBuffer(m_EncoderBuffer);
	    theCSInterface->StartEncode();
        theCSInterface->destroy();
	    break;
	  }
	case ID_TextureBrowsing:
	  {
	    TextureBrowsingDescriptorInterfaceABC *theDescriptorInterface=(TextureBrowsingDescriptorInterfaceABC *)GetDescriptorInterface()->GetContents(x, y);
	    TextureBrowsingCS *theCS = new TextureBrowsingCS(theDescriptorInterface);
	    TextureBrowsingCSInterfaceABC *theCSInterface=theCS->GetInterface();
	    theCSInterface->SetEncoderStreamBuffer(m_EncoderBuffer);
	    theCSInterface->StartEncode();
        theCSInterface->destroy();
	    break;
	  }
	case ID_EdgeHistogram:
	  {
	    EdgeHistogramDescriptorInterfaceABC *theDescriptorInterface=(EdgeHistogramDescriptorInterfaceABC *)GetDescriptorInterface()->GetContents(x, y);
	    EdgeHistogramCS *theCS = new EdgeHistogramCS(theDescriptorInterface);
	    EdgeHistogramCSInterfaceABC *theCSInterface=theCS->GetInterface();
	    theCSInterface->SetEncoderStreamBuffer(m_EncoderBuffer);
	    theCSInterface->StartEncode();
        theCSInterface->destroy();
	    break;
	  }
	case ID_RegionShape:
	  {
	    RegionShapeDescriptor *theDescriptor=(RegionShapeDescriptor *)GetDescriptorInterface()->GetContents(x, y);
	    RegionShapeDescriptorInterfaceABC *theDescriptorInterface=theDescriptor->GetInterface(); 
	    RegionShapeCS *theCS = new RegionShapeCS(theDescriptorInterface);
	    RegionShapeCSInterfaceABC *theCSInterface=theCS->GetInterface();
	    theCSInterface->SetEncoderStreamBuffer(m_EncoderBuffer);
	    theCSInterface->StartEncode();
        theCSInterface->destroy();
	    break;
	  }
	case ID_ContourShape:
	  {
	    ContourShapeDescriptorInterfaceABC *theDescriptorInterface=(ContourShapeDescriptorInterfaceABC *)GetDescriptorInterface()->GetContents(x, y);
	    ContourShapeCS *theCS = new ContourShapeCS(theDescriptorInterface);
	    ContourShapeCSInterfaceABC *theCSInterface=theCS->GetInterface();
	    theCSInterface->SetEncoderStreamBuffer(m_EncoderBuffer);
	    theCSInterface->StartEncode();
        theCSInterface->destroy();
	    break;
	  }
	case ID_Shape3D:
	  {
		ShapeSpectrumDescriptorInterfaceABC *theDescriptorInterface=(ShapeSpectrumDescriptorInterfaceABC *)GetDescriptorInterface()->GetContents(x, y);
	    ShapeSpectrumCS *theCS = new ShapeSpectrumCS(theDescriptorInterface);
	    ShapeSpectrumCSInterfaceABC *theCSInterface=theCS->GetInterface();
	    theCSInterface->SetEncoderStreamBuffer(m_EncoderBuffer);
	    theCSInterface->StartEncode();
        theCSInterface->destroy();
	    break;
	  }
	default:
          fprintf(stderr, "The content type %d is not supported\n", descriptor_id);
        }
      }
    }
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int GridLayoutCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"GridLayoutCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"GridLayoutCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Read 32 bits the data in from the bit stream
  /* style for reading the bits interpreted as nummeric data type*/

  long int descriptor_id;
  long int part_num_h, part_num_v;
  long int descriptor_mask_present;
  long int descriptor_mask=0xffff;

  m_DecoderBuffer->GetBitsLong(&descriptor_id, 8); m_DecoderBuffer->TakeBits(16);
  m_DecoderBuffer->GetBitsLong(&part_num_h, 8); m_DecoderBuffer->TakeBits(8);
  m_DecoderBuffer->GetBitsLong(&part_num_v, 8); m_DecoderBuffer->TakeBits(8);
  GetDescriptorInterface()->SetPartNumber(part_num_h, part_num_v);

  m_DecoderBuffer->GetBitsLong(&descriptor_mask_present, 1); m_DecoderBuffer->TakeBits(1);
  if(descriptor_mask_present){
    m_DecoderBuffer->GetBitsLong(&descriptor_mask, part_num_h*part_num_v);
    m_DecoderBuffer->TakeBits(part_num_h*part_num_v);
  }

  unsigned int mask=1;
  int x, y;
  for(y=0; y<part_num_v; y++){
    for(x=0; x<part_num_h; x++){
      mask = mask<<1;
      if(mask&descriptor_mask){
	switch(descriptor_id){
	case ID_DominantColor:
	  {
	    DominantColorDescriptor *ptr=new DominantColorDescriptor();
	    DominantColorDescriptorInterfaceABC *theDescriptorInterface = ptr->GetInterface();
	    DominantColorCS *theCS = new DominantColorCS;
	    DominantColorCSInterfaceABC *theCSinterface=theCS->GetInterface();
	    theCSinterface->SetDescriptorInterface(theDescriptorInterface);
	    theCSinterface->SetDecoderStreamBuffer(m_DecoderBuffer);
	    theCSinterface->StartDecode();
	    theCSinterface->destroy();
		GetDescriptorInterface()->SetContents(x, y, (GL_SUB_DESCRIPTOR_INTERFACE*)theDescriptorInterface);
          }
	case ID_ColorLayout:
	  {
	    ColorLayoutDescriptor *ptr=new ColorLayoutDescriptor();
	    ColorLayoutDescriptorInterfaceABC *theDescriptorInterface = ptr->GetInterface();
	    ColorLayoutCS *theCS = new ColorLayoutCS;
	    ColorLayoutCSInterfaceABC *theCSinterface=theCS->GetInterface();
	    theCSinterface->SetDescriptorInterface(theDescriptorInterface);
	    theCSinterface->SetDecoderStreamBuffer(m_DecoderBuffer);
	    theCSinterface->StartDecode();
	    theCSinterface->destroy();
	    GetDescriptorInterface()->SetContents(x, y, (GL_SUB_DESCRIPTOR_INTERFACE*)ptr);
          }
	/*case ID_ColorStructure:
	  {
	    ColorStructureDescriptor *ptr=new ColorStructureDescriptor();
	    ColorStructureDescriptorInterfaceABC *theDescriptorInterface = ptr->GetInterface();
	    ColorStructureCS *theCS = new ColorStructureCS;
	    ColorStructureCSInterfaceABC *theCSinterface=theCS->GetInterface();
	    theCSinterface->SetDescriptorInterface(theDescriptorInterface);
	    theCSinterface->SetDecoderStreamBuffer(m_DecoderBuffer);
	    theCSinterface->StartDecode();
	    theCSinterface->destroy();
	    GetDescriptorInterface()->SetContents(x, y, ptr);
          }
	case ID_HomogeneousTexture:
	  {
	    HomogeneousTextureDescriptor *ptr=new HomogeneousTextureDescriptor();
	    HomogeneousTextureDescriptorInterfaceABC *theDescriptorInterface = ptr->GetInterface();
	    HomogeneousTextureCS *theCS = new HomogeneousTextureCS;
	    HomogeneousTextureCSInterfaceABC *theCSinterface=theCS->GetInterface();
	    theCSinterface->SetDescriptorInterface(theDescriptorInterface);
	    theCSinterface->SetDecoderStreamBuffer(m_DecoderBuffer);
	    theCSinterface->StartDecode();
	    theCSinterface->destroy();
	    GetDescriptorInterface()->SetContents(x, y, ptr);
          }
	case ID_TextureBrowsing:
	  {
	    TextureBrowsingDescriptor *ptr=new TextureBrowsingDescriptor();
	    TextureBrowsingDescriptorInterfaceABC *theDescriptorInterface = ptr->GetInterface();
	    TextureBrowsingCS *theCS = new TextureBrowsingCS;
	    TextureBrowsingCSInterfaceABC *theCSinterface=theCS->GetInterface();
	    theCSinterface->SetDescriptorInterface(theDescriptorInterface);
	    theCSinterface->SetDecoderStreamBuffer(m_DecoderBuffer);
	    theCSinterface->StartDecode();
	    theCSinterface->destroy();
	    GetDescriptorInterface()->SetContents(x, y, ptr);
          }
	case ID_RegionShape:
	  {
	    RegionShapeDescriptor *ptr=new RegionShapeDescriptor();
	    RegionShapeDescriptorInterfaceABC *theDescriptorInterface = ptr->GetInterface();
	    RegionShapeCS *theCS = new RegionShapeCS;
	    RegionShapeCSInterfaceABC *theCSinterface=theCS->GetInterface();
	    theCSinterface->SetDescriptorInterface(theDescriptorInterface);
	    theCSinterface->SetDecoderStreamBuffer(m_DecoderBuffer);
	    theCSinterface->StartDecode();
	    theCSinterface->destroy();
	    GetDescriptorInterface()->SetContents(x, y, ptr);
          }
	case ID_ContourShape:
	  {
	    ContourShapeDescriptor *ptr=new ContourShapeDescriptor();
	    ContourShapeDescriptorInterfaceABC *theDescriptorInterface = ptr->GetInterface();
	    ContourShapeCS *theCS = new ContourShapeCS;
	    ContourShapeCSInterfaceABC *theCSinterface=theCS->GetInterface();
	    theCSinterface->SetDescriptorInterface(theDescriptorInterface);
	    theCSinterface->SetDecoderStreamBuffer(m_DecoderBuffer);
	    theCSinterface->StartDecode();
	    theCSinterface->destroy();
	    GetDescriptorInterface()->SetContents(x, y, ptr);
          }/*
  /*
	case ID_ScalableColor:
	case ID_ColorStructure:
	case ID_GoFGoPColor:
	case ID_EdgeHistogram:
	case ID_Shape3D:
  */
	default:
          fprintf(stderr, "The content type %ld is not supported\n",
		  descriptor_id);
	}
      }
    }
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
GridLayoutCSInterfaceABC *GridLayoutCS::GetInterface(void)
{ return &m_Interface; }



