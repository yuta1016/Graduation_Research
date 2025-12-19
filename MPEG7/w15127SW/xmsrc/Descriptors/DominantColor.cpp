///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// University of California Santa Barbara, Kapil Chhabra, Yining Deng.
// Mitsubishi Electric ITE-VIL, Leszek Cieplinski.
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
// Applicable File Name:  DominantColor.cpp
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include "Utilities/readparam.h"
#include "Descriptors/Descriptors.h"


namespace XM {
//#define DEBUG

const UUID DominantColorDescriptorInterface::myID = UUID("");
const char *DominantColorDescriptorInterface::myName =
                         "Dominant Color Description Interface";

const UUID DominantColorDescriptor::myID = UUID("");
const char *DominantColorDescriptor::myName = "Dominant Color Descriptor";

const UUID DominantColorDescriptor::valID = UUID("");
const char *DominantColorDescriptor::valName = "Dominant Color";

const UUID DominantColorDescriptorInterfaceABC::myID = UUID();

//const int DominantColorDescriptor::DCNMAX;

//=============================================================================
//----------------------------------------------------------------------------
DominantColorDescriptorInterfaceABC::DominantColorDescriptorInterfaceABC()
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
DominantColorDescriptorInterface::DominantColorDescriptorInterface(DominantColorDescriptor *aDomCol):
m_DominantColorDescriptor(aDomCol)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
DominantColorDescriptorInterface::~DominantColorDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void DominantColorDescriptorInterface::addref()
{
	m_DominantColorDescriptor->addref();
}

//----------------------------------------------------------------------------
void DominantColorDescriptorInterface::release()
{
  m_DominantColorDescriptor->release();
}

//----------------------------------------------------------------------------
const UUID& DominantColorDescriptorInterface::GetInterfaceID(void)
{
  return myID;
}

//----------------------------------------------------------------------------
const char *DominantColorDescriptorInterface::GetName(void)
{
  return myName;
}

//----------------------------------------------------------------------------
void DominantColorDescriptorInterface::SetValuesFromParameterFile(void)
{
  bool colspcpres = (bool)atoi(getpara("ColorSpacePresent","0"));
  bool colquantpres = (bool)atoi(getpara("ColorQuantizationPresent","0"));
	
  if(colspcpres) GetColorSpaceDescriptorInterface()->
		   SetValuesFromParameterFile();
  if(colquantpres) GetColorQuantizerDescriptorInterface()->
		     SetValuesFromParameterFile();
}

//----------------------------------------------------------------------------
ColorQuantizerDescriptorInterfaceABC *DominantColorDescriptorInterface::
GetColorQuantizerDescriptorInterface(void)
{
  return m_DominantColorDescriptor->
    GetColorQuantizerDescriptorInterface();
}

//----------------------------------------------------------------------------
unsigned long DominantColorDescriptorInterface::
SetColorQuantizerDescriptorInterface(ColorQuantizerDescriptorInterfaceABC
				     *quantizer)	
{
  return m_DominantColorDescriptor->
    SetColorQuantizerDescriptorInterface(quantizer);
}

//----------------------------------------------------------------------------
ColorSpaceDescriptorInterfaceABC
*DominantColorDescriptorInterface::GetColorSpaceDescriptorInterface(void)
{
  return m_DominantColorDescriptor->GetColorSpaceDescriptorInterface();
}

//----------------------------------------------------------------------------
unsigned long DominantColorDescriptorInterface::
SetColorSpaceDescriptorInterface(ColorSpaceDescriptorInterfaceABC
				 *aColorSpaceDescriptorInterface)	
{
  return m_DominantColorDescriptor->
    SetColorSpaceDescriptorInterface(aColorSpaceDescriptorInterface);
}

//----------------------------------------------------------------------------
int DominantColorDescriptorInterface::GetDominantColorsNumber()
{
  return m_DominantColorDescriptor->GetDominantColorsNumber();
}

//----------------------------------------------------------------------------
bool DominantColorDescriptorInterface::GetColorSpacePresent()
{
  return m_DominantColorDescriptor->GetColorSpacePresent();
}

//----------------------------------------------------------------------------
bool DominantColorDescriptorInterface::GetColorQuantizationPresent()
{
  return m_DominantColorDescriptor->GetColorQuantizationPresent();
}

//----------------------------------------------------------------------------
bool DominantColorDescriptorInterface::GetVariancePresent()
{
  return m_DominantColorDescriptor->GetVariancePresent();
}

//----------------------------------------------------------------------------
int DominantColorDescriptorInterface::GetSpatialCoherency()	
{
  return m_DominantColorDescriptor->GetSpatialCoherency();
}

//----------------------------------------------------------------------------
dc_t* DominantColorDescriptorInterface::GetDominantColors()
{
  return m_DominantColorDescriptor->GetDominantColors();
}

//-----------------------------------------------------------------------------
void DominantColorDescriptorInterface::SetDominantColorsNumber(int dc_number)
{
  m_DominantColorDescriptor->
    SetDominantColorsNumber(dc_number);
}

//----------------------------------------------------------------------------
void DominantColorDescriptorInterface::SetColorSpacePresent(bool colspcpres)
{
  m_DominantColorDescriptor->SetColorSpacePresent(colspcpres);
}

//----------------------------------------------------------------------------
void DominantColorDescriptorInterface::
SetColorQuantizationPresent(bool colquantpres)
{
  m_DominantColorDescriptor->SetColorQuantizationPresent(colquantpres);
}

//----------------------------------------------------------------------------
void DominantColorDescriptorInterface::SetVariancePresent(bool varpres)
{
  m_DominantColorDescriptor->SetVariancePresent(varpres);
}

//----------------------------------------------------------------------------
void DominantColorDescriptorInterface::SetSpatialCoherency(int sc)
{
  m_DominantColorDescriptor->SetSpatialCoherency(sc);
}

//---------------------------------------------------------------------------
void DominantColorDescriptorInterface::SetDominantColors(int *percents, int **colors, int **variances)
{
  m_DominantColorDescriptor->SetDominantColors(percents,colors,variances);
}

//----------------------------------------------------------------------------
void DominantColorDescriptorInterface::Print(void)
{
  m_DominantColorDescriptor->Print();
}

//----------------------------------------------------------------------------
unsigned long DominantColorDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_DominantColorDescriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long DominantColorDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_DominantColorDescriptor->ImportDDL(aDescription); }

//===========================================================================
DominantColorDescriptor::DominantColorDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_DominantColorsNumber(0),
m_ColorSpacePresent(0),
m_ColorQuantizationPresent(0),
m_VariancePresent(1),
m_SpatialCoherency(0),
//m_dc(0),
m_ColorSpaceInterface(0),
m_QuantizerInterface(0)
{  
  ColorSpaceDescriptor *m_ColorSpaceDescriptor= new ColorSpaceDescriptor;
  SetColorSpaceDescriptorInterface(m_ColorSpaceDescriptor->GetInterface());

  ColorQuantizerDescriptor *m_ColorQuantizerDescriptor= new ColorQuantizerDescriptor;
  SetColorQuantizerDescriptorInterface(m_ColorQuantizerDescriptor->GetInterface());
  
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
DominantColorDescriptor::DominantColorDescriptor(
			ColorSpaceDescriptorInterfaceABC *colorspace,
			ColorQuantizerDescriptorInterfaceABC *quantizer):
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_DominantColorsNumber(0),
m_ColorSpacePresent(0),
m_ColorQuantizationPresent(0),
m_VariancePresent(1),
m_SpatialCoherency(0),
//m_dc(0),
m_ColorSpaceInterface(0),
m_QuantizerInterface(0)
{
  ColorSpaceDescriptor *m_ColorSpaceDescriptor;
  ColorQuantizerDescriptor *m_ColorQuantizerDescriptor;

#ifdef VERBOSE
  fprintf(stderr,"Create sub descriptors\n");
#endif

  if (!colorspace) {
    m_ColorSpaceDescriptor= new ColorSpaceDescriptor;
    colorspace=m_ColorSpaceDescriptor->GetInterface();
  }
  if (colorspace) SetColorSpaceDescriptorInterface(colorspace);

  if (!quantizer) {
    m_ColorQuantizerDescriptor= new ColorQuantizerDescriptor;
    quantizer=m_ColorQuantizerDescriptor->GetInterface();
  }
  if (quantizer) SetColorQuantizerDescriptorInterface(quantizer);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
DominantColorDescriptor::~DominantColorDescriptor()
{
  if (m_ColorSpaceInterface)
    m_ColorSpaceInterface->release();
  if (m_QuantizerInterface)
    m_QuantizerInterface->release();
  //if (m_dc)
  //  delete [] m_dc;
  //m_dc = 0;
}

//----------------------------------------------------------------------------
const UUID& DominantColorDescriptor::GetObjectID(void)
{
  return myID;
}

//----------------------------------------------------------------------------
const char *DominantColorDescriptor::GetName(void)
{
  return myName;
}

//----------------------------------------------------------------------------
const UUID& DominantColorDescriptor::GetValueID(void)
{
  return valID;
}

//----------------------------------------------------------------------------
const char* DominantColorDescriptor::GetValueName(void)
{
  return valName;
}

//----------------------------------------------------------------------------
unsigned long DominantColorDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  int i, j;
  GenericDS l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface;
  GenericDS SpatCoher, V, Per, ColValInd, Var;

  if(!aParentDescription) {
    fprintf(stderr, "Error(DC::EDDL): parent missing\n");
    return (unsigned long) -1;
  }

#ifdef DEBUG
  fprintf(stderr, "Info(DC::EDDL): starting\n");
#endif

  l_DDLDescription = aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","DominantColorType");
  int dc_number = GetDominantColorsNumber();

  l_DDLDescriptionInterface = l_DDLDescription.GetInterface();

  bool colspcpres = GetColorSpacePresent();
  if(colspcpres) m_ColorSpaceInterface->ExportDDL(l_DDLDescriptionInterface);

  bool colquantpres = GetColorQuantizationPresent();
  if(colquantpres) m_QuantizerInterface->ExportDDL(l_DDLDescriptionInterface);

  bool varpres = GetVariancePresent();

  SpatCoher = l_DDLDescription.CreateChild("SpatialCoherency");
  int spatcoher = GetSpatialCoherency();
  SpatCoher.SetValue(spatcoher);

  //DC = GetDominantColors();

  vector<int> cvi(3);
  vector<int> var(3);
  for(i=0; i<dc_number; i++) {
    V = l_DDLDescription.CreateChild("Value");

    Per = V.CreateChild("Percentage");
    Per.SetValue(m_dc[i].weight);

    ColValInd = V.CreateChild("Index");
    for(j=0; j<3; j++)
    cvi[j] = m_dc[i].centroid[j];
    ColValInd.SetValue(cvi);

    if(varpres==true) {
      Var = V.CreateChild("ColorVariance");
      for(j=0; j<3; j++)
        var[j] = m_dc[i].variance[j];
      Var.SetValue(var);
    }
  }

#ifdef DEBUG
  fprintf(stderr, "Info(DC::EDDL): finished [%d/%d/%d]\n", dc_number, spatcoher, varpres);
#endif

  return 0;
}

//----------------------------------------------------------------------------
unsigned long DominantColorDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  int i, j, dc_number, spatcoher, per;
  string xsitype, colspcpres, colquantpres, varpres;

  GenericDS l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;

  GenericDS V, Per, ColValInd, Var;
  int *percents, **colors, **variances;

  if (!aDescription) {
    fprintf(stderr, "Error(DC::IDDL): null DI\n");
    return (unsigned long) -1;
  }

#ifdef DEBUG
  fprintf(stderr, "Info(DC::IDDL): starting\n");
#endif

  // if aDescription is of correct type
  if(aDescription->GetDSName() == "Descriptor") {
    aDescription->GetTextAttribute("xsi:type", xsitype);
    if(xsitype == "DominantColorType") {
      l_DDLDescriptionInterface = aDescription;
    }
  }

  // else search for DominantColorType as a child
  if(!l_DDLDescriptionInterface) {
    l_DDLDescription = aDescription->GetDescription("Descriptor");

    // search for correct xsi type
    while(!l_DDLDescription.isNull()) {
      l_DDLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype == "DominantColorType")
        break;
      l_DDLDescription.GetNextSibling("Descriptor");
    }

    // DominantColorType not found
    if (l_DDLDescription.isNull()) {
      fprintf(stderr, "Error: (DC::IDDL): descriptor not found\n");
      return (unsigned long)-1;
    }
    l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
  }

  // count colours
  GenericDS dcel = l_DDLDescriptionInterface->GetDescription("Value");
  dc_number = 1;
  while(1) {
    dcel = dcel.GetNextSibling("Value");
    if(!dcel.isNull())
      dc_number++;
    else
      break;
  }
  SetDominantColorsNumber(dc_number);
#ifdef DEBUG
  fprintf(stderr, "Info(DC::IDDL): desc size: %d\n", dc_number);
#endif
#ifdef DEBUG
  fprintf(stderr, "Info(DC::IDDL): sc: %d\n",
          spatcoher);
#endif

  GenericDS ColSpc = l_DDLDescriptionInterface->GetDescription("ColorSpace");
  if(!ColSpc.isNull()) {
    SetColorSpacePresent(true);
    m_ColorSpaceInterface->ImportDDL(l_DDLDescriptionInterface);
  }
  else
    SetColorSpacePresent(false);
#ifdef DEBUG
  fprintf(stderr, "Info(DC::IDDL): sc: %d\n",
          spatcoher);
#endif

  GenericDS ColQuant = l_DDLDescriptionInterface->GetDescription("ColorQuantization");
  if(!ColQuant.isNull()) {
    SetColorQuantizationPresent(true);
    m_QuantizerInterface->ImportDDL(l_DDLDescriptionInterface);
  }
  else
    SetColorQuantizationPresent(false);
#ifdef DEBUG
  fprintf(stderr, "Info(DC::IDDL): cs: %d, cq: %d, sc: %d\n",
          !ColSpc.isNull(), !ColQuant.isNull(), spatcoher);
#endif

  GenericDS SpatCoher = l_DDLDescriptionInterface->GetDescription("SpatialCoherency");
  SpatCoher.GetIntValue(spatcoher);
  SetSpatialCoherency(spatcoher);

#ifdef DEBUG
  fprintf(stderr, "Info(DC::IDDL): cs: %d, cq: %d, sc: %d\n",
          !ColSpc.isNull(), !ColQuant.isNull(), spatcoher);
#endif

  /* dominant colors */
  /*
    percents = new int[dc_number];
    colors = new int*[dc_number];
    for(i=0;i<dc_number;i++) colors[i]= new int[3];
    variances = new int*[dc_number];
    for(i=0;i<dc_number;i++) variances[i]= new int[3];
  */

  for(i=0; i<dc_number; i++) {
    vector<int> cvi, var;

    V = l_DDLDescriptionInterface->GetDescription("Value");

    Per = V.GetDescription("Percentage");
    if(Per.isNull()) {
      fprintf(stderr, "Error(DC::IDDL): null per[%d]\n", i);
      return (unsigned long) -1;
    }
    Per.GetIntValue(per);
    m_dc[i].weight = per;

    ColValInd = V.GetDescription("Index");
    if(ColValInd.isNull()) {
      fprintf(stderr, "Error(DC::IDDL): null index[%d]\n", i);
      return (unsigned long) -1;
    }
    ColValInd.GetIntVector(cvi);
    for(j=0; j<3; j++) //colors[i][j] = cvi[j];
      m_dc[i].centroid[j] = cvi[j];

#ifdef DEBUG
    fprintf(stderr, "Info(DC::IDDL): perc[%d]: %d, color[%d]: %d/%d/%d\n",
            i, per, i, cvi[0], cvi[1], cvi[2]);
#endif

    Var = V.GetDescription("ColorVariance");
    if(!Var.isNull()) {
      SetVariancePresent(true);
      Var.GetIntVector(var);
      for(j=0; j<3; j++) //variances[i][j] = var[j];
        m_dc[i].variance[j] = var[j];
    }
    else
      SetVariancePresent(false);

    // LC: this has to stay for the XM, otherwise we get memory leak
    l_DDLDescriptionInterface->RemoveChild(V);
  }

  //SetDominantColors(percents, colors, variances);

  /*
    delete [] percents;
    for(i=0;i<dc_number;i++) delete [] colors[i];
    delete [] colors;
    for(i=0;i<dc_number;i++) delete [] variances[i];
    delete [] variances;
  */

#ifdef DEBUG
  Print();
  fprintf(stderr, "Info(DC::IDDL): finished\n");
#endif

  return 0;
}

//----------------------------------------------------------------------------
bool DominantColorDescriptor::IsProprietary(void)
{
  return m_isProprietary;
}

//----------------------------------------------------------------------------
void DominantColorDescriptor::addref(void)
{
  m_refcount++;
#ifdef VERBOSE
  fprintf(stderr, "DominantColor addref: %ld\n", m_refcount);
#endif
}

//----------------------------------------------------------------------------
void DominantColorDescriptor::release(void)
{
  m_refcount--;
#ifdef VERBOSE
  fprintf(stderr, "DominantColor release: %ld\n", m_refcount);
#endif
  if( m_refcount < 1 )
    delete this;
}

//----------------------------------------------------------------------------
ColorQuantizerDescriptorInterfaceABC * DominantColorDescriptor::
GetColorQuantizerDescriptorInterface(void)
{
  return m_QuantizerInterface;
}

//----------------------------------------------------------------------------
unsigned long DominantColorDescriptor::SetColorQuantizerDescriptorInterface
        (ColorQuantizerDescriptorInterfaceABC *quantizer)	
{
  if (!quantizer) return (unsigned long) -1;
  if (m_QuantizerInterface == quantizer) return 0;
  if (m_QuantizerInterface) m_QuantizerInterface->release();
  m_QuantizerInterface = quantizer;
  if (m_QuantizerInterface) m_QuantizerInterface->addref();
  else return (unsigned long) -1;

  return 0;
}

//----------------------------------------------------------------------------
ColorSpaceDescriptorInterfaceABC * DominantColorDescriptor::
    GetColorSpaceDescriptorInterface(void)
{
  return m_ColorSpaceInterface;
}

//----------------------------------------------------------------------------
unsigned long DominantColorDescriptor::
SetColorSpaceDescriptorInterface(ColorSpaceDescriptorInterfaceABC *colorspace)	
{
  if (m_ColorSpaceInterface == colorspace) return 0;
  if (m_ColorSpaceInterface) m_ColorSpaceInterface->release();
  m_ColorSpaceInterface = colorspace;
  if (m_ColorSpaceInterface) m_ColorSpaceInterface->addref();
  else return (unsigned long) -1;

  return 0;
}

//----------------------------------------------------------------------------
int DominantColorDescriptor::GetDominantColorsNumber()
{
  return (m_DominantColorsNumber);
}

//----------------------------------------------------------------------------
int DominantColorDescriptor::GetSpatialCoherency()
{
  return (m_SpatialCoherency);
}

//----------------------------------------------------------------------------
dc_t* DominantColorDescriptor::GetDominantColors()
{
  return (m_dc);
}

//----------------------------------------------------------------------------
void DominantColorDescriptor::SetDominantColorsNumber(int dc_number)
{
  //if (dc_number != m_DominantColorsNumber) {
  //  if (m_dc)
  //    delete [] m_dc;
  //  m_dc = new DOMCOL[dc_number];
  //}
  m_DominantColorsNumber = dc_number;
}

//----------------------------------------------------------------------------
void DominantColorDescriptor::SetSpatialCoherency(int sc)
{
  m_SpatialCoherency = sc;
}

//----------------------------------------------------------------------------
void DominantColorDescriptor::SetDominantColors(int *percents, int **colors, int **variances)
{
  int i, j;

  for(i=0;i<m_DominantColorsNumber;i++) {
    m_dc[i].weight = percents[i];
  }
  for(i=0;i<m_DominantColorsNumber;i++) {
    for(j=0;j<3;j++) {
      m_dc[i].centroid[j] = colors[i][j];
    }
  }
  if( m_VariancePresent ) {
    for(i=0;i<m_DominantColorsNumber;i++) {
      for(j=0;j<3;j++) {
        m_dc[i].variance[j] = variances[i][j];
      }
    }
  }
}

//----------------------------------------------------------------------------
void DominantColorDescriptor::Print()
{
  int i;

  std::cerr << "size:        " << m_DominantColorsNumber << std::endl;
  std::cerr << "sc:          " << m_SpatialCoherency << std::endl;
  for(i=0; i<m_DominantColorsNumber; i++)
    std::cerr << "value: " << m_dc[i].weight << " "
         << m_dc[i].centroid[0] << " "
         << m_dc[i].centroid[1] << " "
         << m_dc[i].centroid[2] << " "
         << m_dc[i].variance[0] << " "
         << m_dc[i].variance[1] << " "
         << m_dc[i].variance[2] << std::endl;

}

//----------------------------------------------------------------------------
DominantColorDescriptorInterfaceABC *DominantColorDescriptor::GetInterface(void)
{
  return &m_Interface;
}

//#undef DEBUG
};
