///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Hui Zhang, In Kyu Park, *Duck Hoon Kim, **Il Dong Yun
// Samsung Advanced Institute of Technology, *Seoul National University, **Hankuk University of Foreign Studies
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
// Applicable File Name:  Perceptual3DShape.cpp
//
/* Usage:
-p Parfiles/Perceptual3DShape.par -a Perceptual3DShapeServer     (for server application, to generate descriptors for all 3d models in the database)
-p Parfiles/Perceptual3DShape.par -a Perceptual3DShapeClient -q testset/3d/Dragonfly.wrl     (for client application, to do the retrieval with a query 3d model)
*/

#pragma warning(disable : 4786)

#include <stdio.h>
#include "Descriptors/Descriptors.h"

#include <assert.h>

using namespace XM;

const UUID Perceptual3DShapeDescriptorInterface::myID = UUID("");
const char *Perceptual3DShapeDescriptorInterface::myName = "Perceptual3DShapeDescriptionInterface";

const UUID Perceptual3DShapeDescriptor::myID = UUID("");
const char *Perceptual3DShapeDescriptor::myName = "Perceptual3DShapeDescriptor";

const UUID Perceptual3DShapeDescriptor::valID = UUID("");
const char *Perceptual3DShapeDescriptor::valName = "";

const UUID Perceptual3DShapeDescriptorInterfaceABC::myID = UUID();

Perceptual3DShapeDescriptorInterfaceABC::Perceptual3DShapeDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
Perceptual3DShapeDescriptorInterface::
Perceptual3DShapeDescriptorInterface(Perceptual3DShapeDescriptor *aDescriptor):
m_Perceptual3DShapeDescriptor(aDescriptor)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
Perceptual3DShapeDescriptorInterface::~Perceptual3DShapeDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void Perceptual3DShapeDescriptorInterface::addref(void)
{ m_Perceptual3DShapeDescriptor->addref(); }

//----------------------------------------------------------------------------
void Perceptual3DShapeDescriptorInterface::release(void)
{ m_Perceptual3DShapeDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& Perceptual3DShapeDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Perceptual3DShapeDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
int Perceptual3DShapeDescriptorInterface::GetPerceptual3DBitsPerAttribute(void)
{ return m_Perceptual3DShapeDescriptor->GetPerceptual3DBitsPerAttribute(); }

//----------------------------------------------------------------------------
int Perceptual3DShapeDescriptorInterface::SetPerceptual3DBitsPerAttribute(int nBitsPerAttribute)
{return m_Perceptual3DShapeDescriptor->SetPerceptual3DBitsPerAttribute(nBitsPerAttribute); }

//----------------------------------------------------------------------------
int Perceptual3DShapeDescriptorInterface::GetPerceptual3DShapeNNodes(void)
{ return m_Perceptual3DShapeDescriptor->GetPerceptual3DShapeNNodes(); }

//----------------------------------------------------------------------------
int Perceptual3DShapeDescriptorInterface::SetPerceptual3DShapeNNodes(int nNodes)
{return m_Perceptual3DShapeDescriptor->SetPerceptual3DShapeNNodes(nNodes); }

long* Perceptual3DShapeDescriptorInterface::GetPerceptual3DShapeIsConnected(void)
{return m_Perceptual3DShapeDescriptor->GetPerceptual3DShapeIsConnected();}

P3DS_COMPONENTS* Perceptual3DShapeDescriptorInterface::GetPerceptual3DShapeNodesAll(void)
{return m_Perceptual3DShapeDescriptor->GetPerceptual3DShapeNodesAll();}

//----------------------------------------------------------------------------
unsigned long Perceptual3DShapeDescriptorInterface::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_Perceptual3DShapeDescriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long Perceptual3DShapeDescriptorInterface::ImportDDL(GenericDSInterfaceABC *aDescription)
{ return m_Perceptual3DShapeDescriptor->ImportDDL(aDescription); }

//=============================================================================
Perceptual3DShapeDescriptor::Perceptual3DShapeDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_Perceptual3DShape_numberOfNodes(0),
m_Perceptual3DShape_IsConnected(0),
m_Perceptual3DShape_NodesAll(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
Perceptual3DShapeDescriptor::~Perceptual3DShapeDescriptor()
{
  if (m_Perceptual3DShape_IsConnected) delete [] m_Perceptual3DShape_IsConnected;
  if (m_Perceptual3DShape_NodesAll) delete [] m_Perceptual3DShape_NodesAll;
#ifdef VERBOSE
  fprintf(stderr,"Perceptual3DShape delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& Perceptual3DShapeDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Perceptual3DShapeDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& Perceptual3DShapeDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* Perceptual3DShapeDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool Perceptual3DShapeDescriptor::IsProprietary(void)
{ return m_isProprietary; }

int Perceptual3DShapeDescriptor::GetPerceptual3DBitsPerAttribute(void)
{ return m_Perceptual3DShape_BitsPerAttribute; }
int Perceptual3DShapeDescriptor::SetPerceptual3DBitsPerAttribute(int nBitsPerAttribute)
{m_Perceptual3DShape_BitsPerAttribute = nBitsPerAttribute;return 0;}

//----------------------------------------------------------------------------
int Perceptual3DShapeDescriptor::GetPerceptual3DShapeNNodes(void)
{ return m_Perceptual3DShape_numberOfNodes; }

int Perceptual3DShapeDescriptor::SetPerceptual3DShapeNNodes(int nNodes)
{
  int error=0;

  m_Perceptual3DShape_numberOfNodes = (unsigned long)nNodes;
 
  if (m_Perceptual3DShape_IsConnected) {delete [] m_Perceptual3DShape_IsConnected;m_Perceptual3DShape_IsConnected=0;}
  if (m_Perceptual3DShape_NodesAll) {delete [] m_Perceptual3DShape_NodesAll;m_Perceptual3DShape_NodesAll=0;}
  
  if (nNodes <= 0) {
    m_Perceptual3DShape_numberOfNodes = 0;
    error=-1;
  }
  else
  {
	  int nNum = m_Perceptual3DShape_numberOfNodes*(m_Perceptual3DShape_numberOfNodes-1)/2;
	  if(nNum>0) m_Perceptual3DShape_IsConnected = new long[nNum];
	  m_Perceptual3DShape_NodesAll = new P3DS_COMPONENTS[m_Perceptual3DShape_numberOfNodes];
  }

  return error;
}

long* Perceptual3DShapeDescriptor::GetPerceptual3DShapeIsConnected(void)
{return m_Perceptual3DShape_IsConnected;}
P3DS_COMPONENTS* Perceptual3DShapeDescriptor::GetPerceptual3DShapeNodesAll(void)
{return m_Perceptual3DShape_NodesAll;}




unsigned long Perceptual3DShapeDescriptor::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  GenericDS l_DDLDescription;

  int i;
  GenericDS l_BitsPerAttribute;

  if (!aParentDescription) return (unsigned long) -1;

  l_DDLDescription=aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","Perceptual3DShapeType");

  l_BitsPerAttribute = l_DDLDescription.CreateChild("BitsPerAttribute");
  l_BitsPerAttribute.SetValue((int)m_Perceptual3DShape_BitsPerAttribute);

  int nNodes = (int)m_Perceptual3DShape_numberOfNodes;

  P3DS_COMPONENTS* pComps = GetPerceptual3DShapeNodesAll();

  //IsConnected
  int nSizeAdj = nNodes*(nNodes-1)/2;
  if(nSizeAdj>0)
  {
    GenericDS l_Adj = l_DDLDescription.CreateChild("IsConnected");
    int* pAdj = new int[nSizeAdj];
    long* pAdjOrg = GetPerceptual3DShapeIsConnected();
    for(i=0;i<nSizeAdj;i++) pAdj[i] = (int)(pAdjOrg[i]);
    vector<int> Adj(&pAdj[0],&pAdj[nSizeAdj]);  
    l_Adj.SetValue(Adj); 
    delete []pAdj;
  }

//#define CreateNodeMacro(name)   GenericDS Node_##name = l_DDLDescription.CreateChild("##name");Node_##name.SetValue((int)pComps[i].##name); 

  //other properties.
  //GenericDS l_Node;
  for(i=0;i<nNodes;i++)
  {
	  GenericDS Node_Volume = l_DDLDescription.CreateChild("Volume");Node_Volume.SetValue((int)pComps[i].Volume); 
	  GenericDS Node_Center_X = l_DDLDescription.CreateChild("Center_X");Node_Center_X.SetValue((int)pComps[i].Center_X); 
	  GenericDS Node_Center_Y = l_DDLDescription.CreateChild("Center_Y");Node_Center_Y.SetValue((int)pComps[i].Center_Y); 
	  GenericDS Node_Center_Z = l_DDLDescription.CreateChild("Center_Z");Node_Center_Z.SetValue((int)pComps[i].Center_Z); 
	  GenericDS Node_PCA_Axis_1X = l_DDLDescription.CreateChild("PCA_Axis_1X");Node_PCA_Axis_1X.SetValue((int)pComps[i].PCA_Axis_1X); 
	  GenericDS Node_PCA_Axis_1Y = l_DDLDescription.CreateChild("PCA_Axis_1Y");Node_PCA_Axis_1Y.SetValue((int)pComps[i].PCA_Axis_1Y); 
	  GenericDS Node_PCA_Axis_1Z = l_DDLDescription.CreateChild("PCA_Axis_1Z");Node_PCA_Axis_1Z.SetValue((int)pComps[i].PCA_Axis_1Z); 
	  GenericDS Node_PCA_Axis_2X = l_DDLDescription.CreateChild("PCA_Axis_2X");Node_PCA_Axis_2X.SetValue((int)pComps[i].PCA_Axis_2X); 
	  GenericDS Node_PCA_Axis_2Y = l_DDLDescription.CreateChild("PCA_Axis_2Y");Node_PCA_Axis_2Y.SetValue((int)pComps[i].PCA_Axis_2Y); 
	  GenericDS Node_PCA_Axis_2Z = l_DDLDescription.CreateChild("PCA_Axis_2Z");Node_PCA_Axis_2Z.SetValue((int)pComps[i].PCA_Axis_2Z); 
	  GenericDS Node_Max_1 = l_DDLDescription.CreateChild("Max_1");Node_Max_1.SetValue((int)pComps[i].Max_1); 
	  GenericDS Node_Max_2 = l_DDLDescription.CreateChild("Max_2");Node_Max_2.SetValue((int)pComps[i].Max_2); 
	  GenericDS Node_Max_3 = l_DDLDescription.CreateChild("Max_3");Node_Max_3.SetValue((int)pComps[i].Max_3); 
	  GenericDS Node_Convexity = l_DDLDescription.CreateChild("Convexity");Node_Convexity.SetValue((int)pComps[i].Convexity); 
    /*
	  GenericDS Node = l_DDLDescription.CreateChild("Node");
    Node.SetAttribute("Volume", (int)(pComps[i].Volume));
    Node.SetAttribute("Center_X", (int)(pComps[i].Center_X));
    Node.SetAttribute("Center_Y", (int)(pComps[i].Center_Y));
    Node.SetAttribute("Center_Z", (int)(pComps[i].Center_Z));
    Node.SetAttribute("PCA_Axis_1X", (int)(pComps[i].PCA_Axis_1X));
    Node.SetAttribute("PCA_Axis_1Y", (int)(pComps[i].PCA_Axis_1Y));
    Node.SetAttribute("PCA_Axis_1Z", (int)(pComps[i].PCA_Axis_1Z));
    Node.SetAttribute("PCA_Axis_2X", (int)(pComps[i].PCA_Axis_2X));
    Node.SetAttribute("PCA_Axis_2Y", (int)(pComps[i].PCA_Axis_2Y));
    Node.SetAttribute("PCA_Axis_2Z", (int)(pComps[i].PCA_Axis_2Z));
    Node.SetAttribute("Max_1", (int)(pComps[i].Max_1));
    Node.SetAttribute("Max_2", (int)(pComps[i].Max_2));
    Node.SetAttribute("Max_3", (int)(pComps[i].Max_3));
    Node.SetAttribute("Convexity", (int)(pComps[i].Convexity));
	*/
  }

  return 0;
}

#include <math.h>
int CalcNodeNum(int nNumAdj)
{
//   nNodes*(nNodes-1)/2==nNumAdj;
   if(nNumAdj<=0) return 1;
   int nNodes;
   int nMin = (int)sqrt((double)nNumAdj*2)-1;
   for(nNodes=nMin;nNodes*(nNodes-1)/2<nNumAdj;nNodes++);
   assert(nNodes*(nNodes-1)/2==nNumAdj);
   return nNodes;
}

//----------------------------------------------------------------------------
unsigned long Perceptual3DShapeDescriptor::ImportDDL(GenericDSInterfaceABC *aDescription)
{
  string xsitype;

  GenericDS l_DLLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface    = NULL;

  if (!aDescription) return (unsigned long) -1;

  /* if aDescriptor is of correct type*/
  if (aDescription->GetDSName() == "Descriptor") {
    aDescription->GetTextAttribute("xsi:type",xsitype);
    if (xsitype == "Perceptual3DShapeType") {
      l_DDLDescriptionInterface=aDescription;
    }
  }

  /* else search for Perceptual3DShape as a child*/
  if (!l_DDLDescriptionInterface) {
    l_DLLDescription=aDescription->GetDescription("Descriptor");
    
    /* search for correct xsi type*/
    while (!l_DLLDescription.isNull()) {
      l_DLLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype=="Perceptual3DShapeType") break;
      l_DLLDescription=l_DLLDescription.GetNextSibling("Descriptor");
    }
      

    /* Perceptual3DShape not found*/
    if (l_DLLDescription.isNull()) return (unsigned long)-1;
    /* Perceptual3DShape found*/
    l_DDLDescriptionInterface = l_DLLDescription.GetInterface();
  }

  GenericDS l_Adj, l_BitsPerAttribute;

  int i, nNodes, nBitsPerAttribute; 
  vector<int> Adj;

  l_BitsPerAttribute = l_DDLDescriptionInterface->GetDescription("BitsPerAttribute");
  l_BitsPerAttribute.GetIntValue(nBitsPerAttribute); 
  SetPerceptual3DBitsPerAttribute(nBitsPerAttribute);

  //IsConnected for the first one.
  l_Adj = l_DDLDescriptionInterface->GetDescription("IsConnected");
  l_Adj.GetIntVector(Adj);
  nNodes = CalcNodeNum(Adj.size());
  SetPerceptual3DShapeNNodes(nNodes);
  long* pAdj = GetPerceptual3DShapeIsConnected();
  for (i=0;i<nNodes*(nNodes-1)/2;i++) pAdj[i] = Adj[i];


  P3DS_COMPONENTS* pComps = GetPerceptual3DShapeNodesAll();
  //read all nodes.
  int tmp;i=0;
  GenericDS l_Volume = l_DDLDescriptionInterface->GetDescription("Volume");l_Volume.GetIntValue(tmp);pComps[i].Volume = tmp;
  GenericDS l_Center_X = l_DDLDescriptionInterface->GetDescription("Center_X");l_Center_X.GetIntValue(tmp);pComps[i].Center_X = tmp;
  GenericDS l_Center_Y = l_DDLDescriptionInterface->GetDescription("Center_Y");l_Center_Y.GetIntValue(tmp);pComps[i].Center_Y = tmp;
  GenericDS l_Center_Z = l_DDLDescriptionInterface->GetDescription("Center_Z");l_Center_Z.GetIntValue(tmp);pComps[i].Center_Z = tmp;
  GenericDS l_PCA_Axis_1X = l_DDLDescriptionInterface->GetDescription("PCA_Axis_1X");l_PCA_Axis_1X.GetIntValue(tmp);pComps[i].PCA_Axis_1X = tmp;
  GenericDS l_PCA_Axis_1Y = l_DDLDescriptionInterface->GetDescription("PCA_Axis_1Y");l_PCA_Axis_1Y.GetIntValue(tmp);pComps[i].PCA_Axis_1Y = tmp;
  GenericDS l_PCA_Axis_1Z = l_DDLDescriptionInterface->GetDescription("PCA_Axis_1Z");l_PCA_Axis_1Z.GetIntValue(tmp);pComps[i].PCA_Axis_1Z = tmp;
  GenericDS l_PCA_Axis_2X = l_DDLDescriptionInterface->GetDescription("PCA_Axis_2X");l_PCA_Axis_2X.GetIntValue(tmp);pComps[i].PCA_Axis_2X = tmp;
  GenericDS l_PCA_Axis_2Y = l_DDLDescriptionInterface->GetDescription("PCA_Axis_2Y");l_PCA_Axis_2Y.GetIntValue(tmp);pComps[i].PCA_Axis_2Y = tmp;
  GenericDS l_PCA_Axis_2Z = l_DDLDescriptionInterface->GetDescription("PCA_Axis_2Z");l_PCA_Axis_2Z.GetIntValue(tmp);pComps[i].PCA_Axis_2Z = tmp;
  GenericDS l_Max_1 = l_DDLDescriptionInterface->GetDescription("Max_1");l_Max_1.GetIntValue(tmp);pComps[i].Max_1 = tmp;
  GenericDS l_Max_2 = l_DDLDescriptionInterface->GetDescription("Max_2");l_Max_2.GetIntValue(tmp);pComps[i].Max_2 = tmp;
  GenericDS l_Max_3 = l_DDLDescriptionInterface->GetDescription("Max_3");l_Max_3.GetIntValue(tmp);pComps[i].Max_3 = tmp;
  GenericDS l_Convexity = l_DDLDescriptionInterface->GetDescription("Convexity");l_Convexity.GetIntValue(tmp);pComps[i].Convexity = tmp;

  for(i=1;i<nNodes;i++)
  {
    int tmp;
	  l_Volume = l_Volume.GetNextSibling("Volume");l_Volume.GetIntValue(tmp);pComps[i].Volume = tmp;
	  l_Center_X = l_Center_X.GetNextSibling("Center_X");l_Center_X.GetIntValue(tmp);pComps[i].Center_X = tmp;
	  l_Center_Y = l_Center_Y.GetNextSibling("Center_Y");l_Center_Y.GetIntValue(tmp);pComps[i].Center_Y = tmp;
	  l_Center_Z = l_Center_Z.GetNextSibling("Center_Z");l_Center_Z.GetIntValue(tmp);pComps[i].Center_Z = tmp;
	  l_PCA_Axis_1X = l_PCA_Axis_1X.GetNextSibling("PCA_Axis_1X");l_PCA_Axis_1X.GetIntValue(tmp);pComps[i].PCA_Axis_1X = tmp;
	  l_PCA_Axis_1Y = l_PCA_Axis_1Y.GetNextSibling("PCA_Axis_1Y");l_PCA_Axis_1Y.GetIntValue(tmp);pComps[i].PCA_Axis_1Y = tmp;
	  l_PCA_Axis_1Z = l_PCA_Axis_1Z.GetNextSibling("PCA_Axis_1Z");l_PCA_Axis_1Z.GetIntValue(tmp);pComps[i].PCA_Axis_1Z = tmp;
	  l_PCA_Axis_2X = l_PCA_Axis_2X.GetNextSibling("PCA_Axis_2X");l_PCA_Axis_2X.GetIntValue(tmp);pComps[i].PCA_Axis_2X = tmp;
	  l_PCA_Axis_2Y = l_PCA_Axis_2Y.GetNextSibling("PCA_Axis_2Y");l_PCA_Axis_2Y.GetIntValue(tmp);pComps[i].PCA_Axis_2Y = tmp;
	  l_PCA_Axis_2Z = l_PCA_Axis_2Z.GetNextSibling("PCA_Axis_2Z");l_PCA_Axis_2Z.GetIntValue(tmp);pComps[i].PCA_Axis_2Z = tmp;
	  l_Max_1 = l_Max_1.GetNextSibling("Max_1");l_Max_1.GetIntValue(tmp);pComps[i].Max_1 = tmp;
	  l_Max_2 = l_Max_2.GetNextSibling("Max_2");l_Max_2.GetIntValue(tmp);pComps[i].Max_2 = tmp;
	  l_Max_3 = l_Max_3.GetNextSibling("Max_3");l_Max_3.GetIntValue(tmp);pComps[i].Max_3 = tmp;
	  l_Convexity = l_Convexity.GetNextSibling("Convexity");l_Convexity.GetIntValue(tmp);pComps[i].Convexity = tmp;
  }

/*
  l_Node = l_DDLDescriptionInterface->GetDescription("Node");
  i=0;
  while (!l_Node.isNull())
  {
    int tmp;
    l_Node.GetIntAttribute("Volume", tmp);pComps[i].Volume = tmp;
    l_Node.GetIntAttribute("Center_X", tmp);pComps[i].Center_X = tmp;
    l_Node.GetIntAttribute("Center_Y", tmp);pComps[i].Center_Y = tmp;
    l_Node.GetIntAttribute("Center_Z", tmp);pComps[i].Center_Z = tmp;
    l_Node.GetIntAttribute("PCA_Axis_1X", tmp);pComps[i].PCA_Axis_1X = tmp;
    l_Node.GetIntAttribute("PCA_Axis_1Y", tmp);pComps[i].PCA_Axis_1Y = tmp;
    l_Node.GetIntAttribute("PCA_Axis_1Z", tmp);pComps[i].PCA_Axis_1Z = tmp;
    l_Node.GetIntAttribute("PCA_Axis_2X", tmp);pComps[i].PCA_Axis_2X = tmp;
    l_Node.GetIntAttribute("PCA_Axis_2Y", tmp);pComps[i].PCA_Axis_2Y = tmp;
    l_Node.GetIntAttribute("PCA_Axis_2Z", tmp);pComps[i].PCA_Axis_2Z = tmp;
    l_Node.GetIntAttribute("Max_1", tmp);pComps[i].Max_1 = tmp;
    l_Node.GetIntAttribute("Max_2", tmp);pComps[i].Max_2 = tmp;
    l_Node.GetIntAttribute("Max_3", tmp);pComps[i].Max_3 = tmp;
    l_Node.GetIntAttribute("Convexity", tmp);pComps[i].Convexity = tmp;
    l_Node = l_Node.GetNextSibling("Node");i++;
  }
*/

  return 0;
}

//zhanghui noted: the DDL structure here is changed.
/*this is the old one, I think the new one is more similar with the DDL structure.
//----------------------------------------------------------------------------
unsigned long Perceptual3DShapeDescriptor::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  GenericDS l_DDLDescription;

  int i;
  GenericDS l_Adj, l_Volume, l_Center_X, l_Center_Y, l_Center_Z, 
	  l_PCA_Axis_1X, l_PCA_Axis_1Y, l_PCA_Axis_1Z, l_PCA_Axis_2X, l_PCA_Axis_2Y, l_PCA_Axis_2Z, 
	  l_Max_1, l_Max_2, l_Max_3, l_Convexity, l_NNodes, l_BitsPerAttribute;

  if (!aParentDescription) return (unsigned long) -1;

  l_DDLDescription=aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","Perceptual3DShapeType");

  l_NNodes = l_DDLDescription.CreateChild("numberOfNodes");
  l_NNodes.SetValue((int)m_Perceptual3DShape_numberOfNodes);
  
  l_BitsPerAttribute = l_DDLDescription.CreateChild("BitsPerAttribute");
  l_BitsPerAttribute.SetValue((int)m_Perceptual3DShape_BitsPerAttribute);

  //IsConnected
  l_Adj = l_DDLDescription.CreateChild("IsConnected");
  int nNodes = GetPerceptual3DShapeNNodes();
  int nSizeAdj = nNodes*(nNodes-1)/2;
  int* pAdj = new int[nSizeAdj];
  long* pAdjOrg = GetPerceptual3DShapeIsConnected();
  for(i=0;i<nSizeAdj;i++) pAdj[i] = (int)(pAdjOrg[i]);
  vector<int> Adj(&pAdj[0],&pAdj[nSizeAdj]);  
  l_Adj.SetValue(Adj); 
  delete []pAdj;

  P3DS_COMPONENTS* pComps = GetPerceptual3DShapeNodesAll();

  //Volume
  l_Volume = l_DDLDescription.CreateChild("Volume");
  int* pVolume = new int[nNodes];
  for(i=0;i<nNodes;i++) pVolume[i] = (int)(pComps[i].Volume);
  vector<int> Volume(&pVolume[0],&pVolume[nNodes]);  
  l_Volume.SetValue(Volume); 
  delete []pVolume;

  //Center_X
  l_Center_X = l_DDLDescription.CreateChild("Center_X");
  int* pCenter_X = new int[nNodes];
  for(i=0;i<nNodes;i++) pCenter_X[i] = (int)(pComps[i].Center_X);
  vector<int> Center_X(&pCenter_X[0],&pCenter_X[nNodes]);  
  l_Center_X.SetValue(Center_X); 
  delete []pCenter_X;
  //Center_Y
  l_Center_Y = l_DDLDescription.CreateChild("Center_Y");
  int* pCenter_Y = new int[nNodes];
  for(i=0;i<nNodes;i++) pCenter_Y[i] = (int)(pComps[i].Center_Y);
  vector<int> Center_Y(&pCenter_Y[0],&pCenter_Y[nNodes]);  
  l_Center_Y.SetValue(Center_Y); 
  delete []pCenter_Y;
  //Center_Z
  l_Center_Z = l_DDLDescription.CreateChild("Center_Z");
  int* pCenter_Z = new int[nNodes];
  for(i=0;i<nNodes;i++) pCenter_Z[i] = (int)(pComps[i].Center_Z);
  vector<int> Center_Z(&pCenter_Z[0],&pCenter_Z[nNodes]);  
  l_Center_Z.SetValue(Center_Z); 
  delete []pCenter_Z;

  //PCA_Axis_1X
  l_PCA_Axis_1X = l_DDLDescription.CreateChild("PCA_Axis_1X");
  int* pPCA_Axis_1X = new int[nNodes];
  for(i=0;i<nNodes;i++) pPCA_Axis_1X[i] = (int)(pComps[i].PCA_Axis_1X);
  vector<int> PCA_Axis_1X(&pPCA_Axis_1X[0],&pPCA_Axis_1X[nNodes]);  
  l_PCA_Axis_1X.SetValue(PCA_Axis_1X); 
  delete []pPCA_Axis_1X;
  //PCA_Axis_1Y
  l_PCA_Axis_1Y = l_DDLDescription.CreateChild("PCA_Axis_1Y");
  int* pPCA_Axis_1Y = new int[nNodes];
  for(i=0;i<nNodes;i++) pPCA_Axis_1Y[i] = (int)(pComps[i].PCA_Axis_1Y);
  vector<int> PCA_Axis_1Y(&pPCA_Axis_1Y[0],&pPCA_Axis_1Y[nNodes]);  
  l_PCA_Axis_1Y.SetValue(PCA_Axis_1Y); 
  delete []pPCA_Axis_1Y;
  //PCA_Axis_1Z
  l_PCA_Axis_1Z = l_DDLDescription.CreateChild("PCA_Axis_1Z");
  int* pPCA_Axis_1Z = new int[nNodes];
  for(i=0;i<nNodes;i++) pPCA_Axis_1Z[i] = (int)(pComps[i].PCA_Axis_1Z);
  vector<int> PCA_Axis_1Z(&pPCA_Axis_1Z[0],&pPCA_Axis_1Z[nNodes]);  
  l_PCA_Axis_1Z.SetValue(PCA_Axis_1Z); 
  delete []pPCA_Axis_1Z;
  //PCA_Axis_2X
  l_PCA_Axis_2X = l_DDLDescription.CreateChild("PCA_Axis_2X");
  int* pPCA_Axis_2X = new int[nNodes];
  for(i=0;i<nNodes;i++) pPCA_Axis_2X[i] = (int)(pComps[i].PCA_Axis_2X);
  vector<int> PCA_Axis_2X(&pPCA_Axis_2X[0],&pPCA_Axis_2X[nNodes]);  
  l_PCA_Axis_2X.SetValue(PCA_Axis_2X); 
  delete []pPCA_Axis_2X;
  //PCA_Axis_2Y
  l_PCA_Axis_2Y = l_DDLDescription.CreateChild("PCA_Axis_2Y");
  int* pPCA_Axis_2Y = new int[nNodes];
  for(i=0;i<nNodes;i++) pPCA_Axis_2Y[i] = (int)(pComps[i].PCA_Axis_2Y);
  vector<int> PCA_Axis_2Y(&pPCA_Axis_2Y[0],&pPCA_Axis_2Y[nNodes]);  
  l_PCA_Axis_2Y.SetValue(PCA_Axis_2Y); 
  delete []pPCA_Axis_2Y;
  //PCA_Axis_2Z
  l_PCA_Axis_2Z = l_DDLDescription.CreateChild("PCA_Axis_2Z");
  int* pPCA_Axis_2Z = new int[nNodes];
  for(i=0;i<nNodes;i++) pPCA_Axis_2Z[i] = (int)(pComps[i].PCA_Axis_2Z);
  vector<int> PCA_Axis_2Z(&pPCA_Axis_2Z[0],&pPCA_Axis_2Z[nNodes]);  
  l_PCA_Axis_2Z.SetValue(PCA_Axis_2Z); 
  delete []pPCA_Axis_2Z;

  //Max_1
  l_Max_1 = l_DDLDescription.CreateChild("Max_1");
  int* pMax_1 = new int[nNodes];
  for(i=0;i<nNodes;i++) pMax_1[i] = (int)(pComps[i].Max_1);
  vector<int> Max_1(&pMax_1[0],&pMax_1[nNodes]);  
  l_Max_1.SetValue(Max_1); 
  delete []pMax_1;
  //Max_2
  l_Max_2 = l_DDLDescription.CreateChild("Max_2");
  int* pMax_2 = new int[nNodes];
  for(i=0;i<nNodes;i++) pMax_2[i] = (int)(pComps[i].Max_2);
  vector<int> Max_2(&pMax_2[0],&pMax_2[nNodes]);  
  l_Max_2.SetValue(Max_2); 
  delete []pMax_2;
  //Max_3
  l_Max_3 = l_DDLDescription.CreateChild("Max_3");
  int* pMax_3 = new int[nNodes];
  for(i=0;i<nNodes;i++) pMax_3[i] = (int)(pComps[i].Max_3);
  vector<int> Max_3(&pMax_3[0],&pMax_3[nNodes]);  
  l_Max_3.SetValue(Max_3); 
  delete []pMax_3;

  //Convexity
  l_Convexity = l_DDLDescription.CreateChild("Convexity");
  int* pConvexity = new int[nNodes];
  for(i=0;i<nNodes;i++) pConvexity[i] = (int)(pComps[i].Convexity);
  vector<int> Convexity(&pConvexity[0],&pConvexity[nNodes]);  
  l_Convexity.SetValue(Convexity); 
  delete []pConvexity;

  return 0;
}

//----------------------------------------------------------------------------
unsigned long Perceptual3DShapeDescriptor::ImportDDL(GenericDSInterfaceABC *aDescription)
{
  int val;
  string xsitype;

  GenericDS l_DLLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface    = NULL;

  if (!aDescription) return (unsigned long) -1;

  // if aDescriptor is of correct type
  if (aDescription->GetDSName() == "Descriptor") {
    aDescription->GetTextAttribute("xsi:type",xsitype);
    if (xsitype == "Perceptual3DShapeType") {
      l_DDLDescriptionInterface=aDescription;
    }
  }

  // else search for Perceptual3DShape as a child
  if (!l_DDLDescriptionInterface) {
    l_DLLDescription=aDescription->GetDescription("Descriptor");
    
    // search for correct xsi type
    while (!l_DLLDescription.isNull()) {
      l_DLLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype=="Perceptual3DShapeType") break;
      l_DLLDescription=l_DLLDescription.GetNextSibling("Descriptor");
    }
      

    // Perceptual3DShape not found
    if (l_DLLDescription.isNull()) return (unsigned long)-1;
    // Perceptual3DShape found
    l_DDLDescriptionInterface=
      l_DLLDescription.GetInterface();
  }

  GenericDS l_Adj, l_Volume, l_Center_X, l_Center_Y, l_Center_Z, 
	  l_PCA_Axis_1X, l_PCA_Axis_1Y, l_PCA_Axis_1Z, l_PCA_Axis_2X, l_PCA_Axis_2Y, l_PCA_Axis_2Z, 
	  l_Max_1, l_Max_2, l_Max_3, l_Convexity, l_NNodes, l_BitsPerAttribute;

  int i, nNodes, nBitsPerAttribute; 
  vector<int> Adj, Volume, Center_X, Center_Y, Center_Z, 
	  PCA_Axis_1X, PCA_Axis_1Y, PCA_Axis_1Z, PCA_Axis_2X, PCA_Axis_2Y, PCA_Axis_2Z, 
	  Max_1, Max_2, Max_3, Convexity;

  l_NNodes = l_DDLDescriptionInterface->GetDescription("numberOfNodes");
  l_NNodes.GetIntValue(nNodes); 
  SetPerceptual3DShapeNNodes(nNodes);

  l_BitsPerAttribute = l_DDLDescriptionInterface->GetDescription("BitsPerAttribute");
  l_BitsPerAttribute.GetIntValue(nBitsPerAttribute); 
  SetPerceptual3DBitsPerAttribute(nBitsPerAttribute);

  //IsConnected
  long* pAdj = GetPerceptual3DShapeIsConnected();
  l_Adj = l_DDLDescriptionInterface->GetDescription("IsConnected");
  l_Adj.GetIntVector(Adj);
  for (i=0;i<nNodes*(nNodes-1)/2;i++) pAdj[i] = Adj[i];

  P3DS_COMPONENTS* pComps = GetPerceptual3DShapeNodesAll();

  //Volume
  l_Volume = l_DDLDescriptionInterface->GetDescription("Volume");
  l_Volume.GetIntVector(Volume);
  for (i=0;i<nNodes;i++) pComps[i].Volume = Volume[i];

  //Center_X
  l_Center_X = l_DDLDescriptionInterface->GetDescription("Center_X");
  l_Center_X.GetIntVector(Center_X);
  for (i=0;i<nNodes;i++) pComps[i].Center_X = Center_X[i];
  //Center_Y
  l_Center_Y = l_DDLDescriptionInterface->GetDescription("Center_Y");
  l_Center_Y.GetIntVector(Center_Y);
  for (i=0;i<nNodes;i++) pComps[i].Center_Y = Center_Y[i];
  //Center_Z
  l_Center_Z = l_DDLDescriptionInterface->GetDescription("Center_Z");
  l_Center_Z.GetIntVector(Center_Z);
  for (i=0;i<nNodes;i++) pComps[i].Center_Z = Center_Z[i];

  //PCA_Axis_1X
  l_PCA_Axis_1X = l_DDLDescriptionInterface->GetDescription("PCA_Axis_1X");
  l_PCA_Axis_1X.GetIntVector(PCA_Axis_1X);
  for (i=0;i<nNodes;i++) pComps[i].PCA_Axis_1X = PCA_Axis_1X[i];
  //PCA_Axis_1Y
  l_PCA_Axis_1Y = l_DDLDescriptionInterface->GetDescription("PCA_Axis_1Y");
  l_PCA_Axis_1Y.GetIntVector(PCA_Axis_1Y);
  for (i=0;i<nNodes;i++) pComps[i].PCA_Axis_1Y = PCA_Axis_1Y[i];
  //PCA_Axis_1Z
  l_PCA_Axis_1Z = l_DDLDescriptionInterface->GetDescription("PCA_Axis_1Z");
  l_PCA_Axis_1Z.GetIntVector(PCA_Axis_1Z);
  for (i=0;i<nNodes;i++) pComps[i].PCA_Axis_1Z = PCA_Axis_1Z[i];
  //PCA_Axis_2X
  l_PCA_Axis_2X = l_DDLDescriptionInterface->GetDescription("PCA_Axis_2X");
  l_PCA_Axis_2X.GetIntVector(PCA_Axis_2X);
  for (i=0;i<nNodes;i++) pComps[i].PCA_Axis_2X = PCA_Axis_2X[i];
  //PCA_Axis_2Y
  l_PCA_Axis_2Y = l_DDLDescriptionInterface->GetDescription("PCA_Axis_2Y");
  l_PCA_Axis_2Y.GetIntVector(PCA_Axis_2Y);
  for (i=0;i<nNodes;i++) pComps[i].PCA_Axis_2Y = PCA_Axis_2Y[i];
  //PCA_Axis_2Z
  l_PCA_Axis_2Z = l_DDLDescriptionInterface->GetDescription("PCA_Axis_2Z");
  l_PCA_Axis_2Z.GetIntVector(PCA_Axis_2Z);
  for (i=0;i<nNodes;i++) pComps[i].PCA_Axis_2Z = PCA_Axis_2Z[i];

  //Max_1
  l_Max_1 = l_DDLDescriptionInterface->GetDescription("Max_1");
  l_Max_1.GetIntVector(Max_1);
  for (i=0;i<nNodes;i++) pComps[i].Max_1 = Max_1[i];
  //Max_2
  l_Max_2 = l_DDLDescriptionInterface->GetDescription("Max_2");
  l_Max_2.GetIntVector(Max_2);
  for (i=0;i<nNodes;i++) pComps[i].Max_2 = Max_2[i];
  //Max_3
  l_Max_3 = l_DDLDescriptionInterface->GetDescription("Max_3");
  l_Max_3.GetIntVector(Max_3);
  for (i=0;i<nNodes;i++) pComps[i].Max_3 = Max_3[i];
  
  //Convexity
  l_Convexity = l_DDLDescriptionInterface->GetDescription("Convexity");
  l_Convexity.GetIntVector(Convexity);
  for (i=0;i<nNodes;i++) pComps[i].Convexity = Convexity[i];

  return 0;
}
*/

//----------------------------------------------------------------------------
void Perceptual3DShapeDescriptor::addref(void)
{
#ifdef VERBOSE
       fprintf(stderr,"Perceptual 3D Shape connect\n");
#endif
  m_refcount++;
}

//----------------------------------------------------------------------------
void Perceptual3DShapeDescriptor::release(void)
{
#ifdef VERBOSE
       fprintf(stderr,"Perceptual 3D Shape release\n");
#endif
  if (!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
Perceptual3DShapeDescriptorInterfaceABC *Perceptual3DShapeDescriptor::GetInterface(void)
{ return &m_Interface; }


