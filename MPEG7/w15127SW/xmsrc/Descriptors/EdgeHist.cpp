///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// ETRI / Dongguk University, Seoul
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
// Applicable File Name:  EdgeHist.cpp
//

#include <stdio.h>
#include <memory.h>
#include "Descriptors/Descriptors.h"


using namespace XM;

const UUID EdgeHistogramDescriptorInterface::myID = UUID("");
const char *EdgeHistogramDescriptorInterface::myName =
"EdgeHistogramDescriptionInterface";

const UUID EdgeHistogramDescriptor::myID = UUID("");
const char *EdgeHistogramDescriptor::myName = "EdgeHistogramDescriptor";

const UUID EdgeHistogramDescriptor::valID = UUID("");
const char *EdgeHistogramDescriptor::valName = "";

const UUID EdgeHistogramDescriptorInterfaceABC::myID = UUID();

EdgeHistogramDescriptorInterfaceABC::EdgeHistogramDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
// Quantization quantization tables
double EdgeHistogramDescriptor::QuantTable[5][8] = { 
  {0.010867,0.057915,0.099526,0.144849,0.195573,0.260504,0.358031,0.530128}, 
  {0.012266,0.069934,0.125879,0.182307,0.243396,0.314563,0.411728,0.564319},
  {0.004193,0.025852,0.046860,0.068519,0.093286,0.123490,0.161505,0.228960},
  {0.004174,0.025924,0.046232,0.067163,0.089655,0.115391,0.151904,0.217745},
  {0.006778,0.051667,0.108650,0.166257,0.224226,0.285691,0.356375,0.450972},
};

//=============================================================================
EdgeHistogramDescriptorInterface::
EdgeHistogramDescriptorInterface(EdgeHistogramDescriptor *aHisto):
m_Descriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
EdgeHistogramDescriptorInterface::~EdgeHistogramDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void EdgeHistogramDescriptorInterface::addref(void)
{ m_Descriptor->addref(); }

//----------------------------------------------------------------------------
void EdgeHistogramDescriptorInterface::release(void)
{ m_Descriptor->release(); }

//----------------------------------------------------------------------------
const UUID& EdgeHistogramDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *EdgeHistogramDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void EdgeHistogramDescriptorInterface::
SetEdgeHistogram(EHD* pEdge_Histogram)
{
  m_Descriptor->SetEdgeHistogram(pEdge_Histogram);	
}

//----------------------------------------------------------------------------
void EdgeHistogramDescriptorInterface::
SetEdgeHistogramElement(char* pEdge_Histogram)
{
  m_Descriptor->SetEdgeHistogramElement(pEdge_Histogram);	
}

//----------------------------------------------------------------------------
EHD* EdgeHistogramDescriptorInterface::GetEdgeHistogram()
{
  return m_Descriptor->GetEdgeHistogram();	
}

//----------------------------------------------------------------------------
char*	EdgeHistogramDescriptorInterface::GetEdgeHistogramElement()
{
  return m_Descriptor->GetEdgeHistogramElement();	
}

//----------------------------------------------------------------------------
unsigned long EdgeHistogramDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_Descriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long EdgeHistogramDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_Descriptor->ImportDDL(aDescription); }


//=============================================================================
EdgeHistogramDescriptor::EdgeHistogramDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false)
{
  m_pEdge_Histogram=NULL;
  m_pEdge_HistogramElement=NULL;
  m_pEdge_Histogram = new EHD[1];
  m_pEdge_HistogramElement = new char[80];
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
EdgeHistogramDescriptor::~EdgeHistogramDescriptor()
{
  if( m_pEdge_Histogram )
    delete [] m_pEdge_Histogram;
  if( m_pEdge_HistogramElement )
    delete [] m_pEdge_HistogramElement;

#ifdef VERBOSE
  fprintf(stderr,"Edge Histogram delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& EdgeHistogramDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *EdgeHistogramDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& EdgeHistogramDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* EdgeHistogramDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool EdgeHistogramDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
unsigned long EdgeHistogramDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  GenericDS l_DDLDescription;
  int i;

  if(!aParentDescription) return (unsigned long) -1;

  l_DDLDescription=aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","EdgeHistogramType");

  vector<int> BinCounts(80);
  for(i=0; i<80; i++)
    BinCounts[i]=m_pEdge_HistogramElement[i];
  GenericDS EdgeHistogramDDLDescription=l_DDLDescription.CreateChild("BinCounts");
  EdgeHistogramDDLDescription.SetValue(BinCounts);

  return 0;
}

//-----------------------------------------------------------------------------
unsigned long EdgeHistogramDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  string xsitype;

  GenericDS l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface    = NULL;

  if (!aDescription) return (unsigned long) -1;

  /* if aDescriptor is of correct type*/
  if (aDescription->GetDSName() == "Descriptor") {
    aDescription->GetTextAttribute("xsi:type",xsitype);
    if (xsitype == "EdgeHistogramType") {
      l_DDLDescriptionInterface=aDescription;
    }
  }

  /* else search for DummyType as a child*/
  if (!l_DDLDescriptionInterface) {
    l_DDLDescription=aDescription->GetDescription("Descriptor");
    
    /* search for correct xsi type*/
    while (!l_DDLDescription.isNull()) {
      l_DDLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype=="EdgeHistogramType") break;
      l_DDLDescription=
	l_DDLDescription.GetNextSibling("Descriptor");
    }
      

    /* DummyType not found*/
    if (l_DDLDescription.isNull()) return (unsigned long)-1;
    /* DummyType found*/
    l_DDLDescriptionInterface=
      l_DDLDescription.GetInterface();
  }

  char tempBins[80];
  vector<int> BinCounts;

  GenericDS EdgeHistogramDDLDescription=l_DDLDescriptionInterface->GetDescription("BinCounts");
  EdgeHistogramDDLDescription.GetIntVector(BinCounts);
  for(int i=0; i<80; i++) tempBins[i] = (char)BinCounts[i];
    SetEdgeHistogramElement(tempBins);
  return 0;
}

//----------------------------------------------------------------------------
void EdgeHistogramDescriptor::addref(void)
{
#ifdef VERBOSE
       fprintf(stderr,"EdgeHistogram connect\n");
#endif
       m_refcount++;
}

//----------------------------------------------------------------------------
void EdgeHistogramDescriptor::release(void)

{
#ifdef VERBOSE
       fprintf(stderr,"EdgeHistogram release\n");
#endif
       if (!(--m_refcount)) delete this;
}

//----------------------------------------------------------------------------
EHD*	EdgeHistogramDescriptor::GetEdgeHistogram()
{
	return	m_pEdge_Histogram;
}

//----------------------------------------------------------------------------
char*	EdgeHistogramDescriptor::GetEdgeHistogramElement()
{
	return	m_pEdge_HistogramElement;
}

//----------------------------------------------------------------------------
void	EdgeHistogramDescriptor::SetEdgeHistogramElement(char*	pEdgeHistogram)
{
	int i;
	for( i=0; i < 80; i++ )
		m_pEdge_Histogram->Local_Edge[i] = QuantTable[ i%5 ][ pEdgeHistogram[i] ];

	memcpy(m_pEdge_HistogramElement, pEdgeHistogram, 80);
}

//----------------------------------------------------------------------------
void	EdgeHistogramDescriptor::SetEdgeHistogram(EHD*	pEdge_Histogram)
{
  int i, j;
  double iQuantValue;

  for( i=0; i < 80; i++ ) {
    j=0;
    while(1){
      if( j < 7 ) // SIZI-1 
        iQuantValue = (QuantTable[i%5][j]+QuantTable[i%5][j+1])/2.0;
      else 
        iQuantValue = 1.0;
      if(pEdge_Histogram->Local_Edge[i] <= iQuantValue)
        break;
      j++;
    }
    m_pEdge_HistogramElement[i] = j;
  }
  for( i=0; i < 80; i++ ){
    m_pEdge_Histogram->Local_Edge[i] = QuantTable[ i%5 ][ m_pEdge_HistogramElement[i] ];
  }
}


//-----------------------------------------------------------------------------
EdgeHistogramDescriptorInterfaceABC *EdgeHistogramDescriptor::GetInterface(void)
{ return &m_Interface; }
