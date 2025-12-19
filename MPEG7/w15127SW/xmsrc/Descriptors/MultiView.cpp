///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// HHI
// K. Mueller
//
// Mitsubishi Electric - Infomation Technology Europe - Visual Information Laboratory
// James Cooper
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
// Copyright (c) 2000-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MultiView.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include "Descriptors/Descriptors.h"
#include <math.h>
/*#include <memory.h>*/

using namespace XM;

//#===--- Static Variables
//sth please remove blanks in the strings (bad for command line)
const UUID MultiViewDescriptorInterface::myID = UUID("");
const char *MultiViewDescriptorInterface::myName = "MultiView Interface";

const UUID MultiViewDescriptor::myID = UUID("");
const char *MultiViewDescriptor::myName = "MultiView Descriptor";

const UUID MultiViewDescriptor::valID = UUID("");
const char *MultiViewDescriptor::valName = "MultiView Descriptor";

const UUID MultiViewDescriptorInterfaceABC::myID = UUID();

//sth delimters :-(
//#===-----------------------------------------
//#===--- MultiViewDescriptorInterfaceABC

MultiViewDescriptorInterfaceABC::MultiViewDescriptorInterfaceABC()
{
  SetInterfaceID(myID);
} // Constructor

//#===--------------------------------------
//#===--- MultiViewDescriptorInterface

MultiViewDescriptorInterface::MultiViewDescriptorInterface(MultiViewDescriptor *pMultiView)
: m_pMultiViewDescriptor(pMultiView)
{
  SetInterfaceID(myID);
}

//#===-----------------------------------------
MultiViewDescriptorInterface::~MultiViewDescriptorInterface()
{
}
	
//----------------------------------------------------------------------------
void MultiViewDescriptorInterface::addref(void)
{ m_pMultiViewDescriptor->addref(); }

//----------------------------------------------------------------------------
void MultiViewDescriptorInterface::release(void)
{ m_pMultiViewDescriptor->release(); }

//#===-----------------------------------------
const UUID& MultiViewDescriptorInterface::GetInterfaceID(void)
{
  return myID;
}

//#===-----------------------------------------
const char *MultiViewDescriptorInterface::GetName(void)
{
  return myName;
}

//----------------------------------------------------------------------------
SUB_DESCRIPTOR_INTERFACE_ABC *MultiViewDescriptorInterface::Get2DDescriptorInterface(int i)
{
	return m_pMultiViewDescriptor->Get2DDescriptorInterface(i);
}

//----------------------------------------------------------------------------
unsigned long MultiViewDescriptorInterface::
Set2DDescriptorInterface(SUB_DESCRIPTOR_INTERFACE_ABC *interface2d, int i)	
{
	return m_pMultiViewDescriptor->Set2DDescriptorInterface(interface2d, i);
}
//----------------------------------------------------------------------------
void MultiViewDescriptorInterface::SetNoOfViews(unsigned char count )
{
	m_pMultiViewDescriptor->SetNoOfViews(count);
}
//----------------------------------------------------------------------------
unsigned char MultiViewDescriptorInterface::GetNoOfViews(void) const
{
	return m_pMultiViewDescriptor->GetNoOfViews();
}
//----------------------------------------------------------------------------
void MultiViewDescriptorInterface::SetVisibleViewFlag(int i, bool value)
{
	m_pMultiViewDescriptor->SetVisibleViewFlag(i, value);
}
//----------------------------------------------------------------------------
bool MultiViewDescriptorInterface::GetVisibleViewFlag(int i)
{
	return m_pMultiViewDescriptor->GetVisibleViewFlag(i);
}
//----------------------------------------------------------------------------
void MultiViewDescriptorInterface::SetFixedViewsFlag(bool value)
{
	m_pMultiViewDescriptor->SetFixedViewsFlag(value);
}
//----------------------------------------------------------------------------
bool MultiViewDescriptorInterface::GetFixedViewsFlag(void)
{
	return m_pMultiViewDescriptor->GetFixedViewsFlag();
}
//-----------------------------------------------------------------------------
unsigned long MultiViewDescriptorInterface::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  return m_pMultiViewDescriptor->ExportDDL(aParentDescription);
}

//-----------------------------------------------------------------------------
unsigned long MultiViewDescriptorInterface::ImportDDL(GenericDSInterfaceABC *aDescription)
{
  return m_pMultiViewDescriptor->ImportDDL(aDescription);
}

//#===-----------------------------------------
//#===------------------------------
//#===--- MultiViewDescriptor

MultiViewDescriptor::MultiViewDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_iNumberOfViews(0),
m_bFixedViewsFlag(true),
m_p2DDescriptorInterface(0)
{
  SetExposedInterface(&m_Interface);
	m_p2DDescriptorInterface = new SUB_DESCRIPTOR_INTERFACE_ABC*[TotNumberOfViews];

	//set all the flags to true by default(ie a 3d model)
	for( int i = 0; i < 8; i++)
		m_pVisibleViewFlags[i] = true;
}

//----------------------------------------------------------------------------
MultiViewDescriptor::
MultiViewDescriptor(SUB_DESCRIPTOR_INTERFACE_ABC **descriptor2d):
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_iNumberOfViews(0),
m_bFixedViewsFlag(true),
m_p2DDescriptorInterface(0)
{
	int i;
  SUB_DESCRIPTOR **m_2DDescriptor;

	m_p2DDescriptorInterface = new SUB_DESCRIPTOR_INTERFACE_ABC*[TotNumberOfViews];

#ifdef VERBOSE
        fprintf(stderr,"Create sub descriptor array\n");
#endif

	if (!descriptor2d) {

	  m_2DDescriptor = new SUB_DESCRIPTOR*[TotNumberOfViews];
		descriptor2d = new SUB_DESCRIPTOR_INTERFACE_ABC*[TotNumberOfViews];

		for( i = 0; i < TotNumberOfViews; i++) {
		  m_2DDescriptor[i] = new SUB_DESCRIPTOR;
			descriptor2d[i] = m_2DDescriptor[i]->GetInterface();
			m_p2DDescriptorInterface[i] = 0;
			if(descriptor2d[i]) 
				Set2DDescriptorInterface(descriptor2d[i], i);
		}
		delete [] m_2DDescriptor;
		delete [] descriptor2d;

	}

	else 
	{
		for( i = 0; i < TotNumberOfViews; i++) 
			Set2DDescriptorInterface(descriptor2d[i], i);
	}

	//set all the flags to true by default(ie a 3d model)
	for( i = 0; i < 8; i++)
		m_pVisibleViewFlags[i] = true;


	SetExposedInterface(&m_Interface);
}


//#===-----------------------------------------
MultiViewDescriptor::~MultiViewDescriptor()
{
  // Free peak memory (if allocated)
	for( int i = 0; i < TotNumberOfViews; i++) {
		if(m_p2DDescriptorInterface[i]) m_p2DDescriptorInterface[i]->release();
	}

  if (m_p2DDescriptorInterface)  delete [] m_p2DDescriptorInterface;
}

//#===-----------------------------------------
const UUID& MultiViewDescriptor::GetObjectID(void)
{
  return myID;
}

//#===-----------------------------------------
const char *MultiViewDescriptor::GetName(void)
{
  return myName;
}

//#===-----------------------------------------
const UUID& MultiViewDescriptor::GetValueID(void)
{
  return valID;
}

//#===-----------------------------------------
const char* MultiViewDescriptor::GetValueName(void)
{
  return valName;
}

//#===-----------------------------------------
bool MultiViewDescriptor::IsProprietary(void)
{
  return m_isProprietary;
}
//----------------------------------------------------------------------------
void MultiViewDescriptor::addref(void)

{
#ifdef VERBOSE
       fprintf(stderr,"MultiView connect\n");
#endif
       m_refcount++;
}

//----------------------------------------------------------------------------
void MultiViewDescriptor::release(void)

{
#ifdef VERBOSE
       fprintf(stderr,"MultiView release\n");
#endif
       if (!(--m_refcount)) delete this;
}

//#===-----------------------------------------
MultiViewDescriptorInterfaceABC *MultiViewDescriptor::GetInterface(void)
{
  return &m_Interface;
}

//----------------------------------------------------------------------------
SUB_DESCRIPTOR_INTERFACE_ABC *MultiViewDescriptor::Get2DDescriptorInterface(int i)
{
	return m_p2DDescriptorInterface[i];
}

//----------------------------------------------------------------------------
unsigned long MultiViewDescriptor::Set2DDescriptorInterface
        (SUB_DESCRIPTOR_INTERFACE_ABC *Interface2d, int i )	
{
       if (!Interface2d) return (unsigned long) -1;
       if (m_p2DDescriptorInterface[i] == Interface2d) return 0;
       if (m_p2DDescriptorInterface[i]) m_p2DDescriptorInterface[i]->release();
       m_p2DDescriptorInterface[i] = Interface2d;
       if (m_p2DDescriptorInterface[i]) m_p2DDescriptorInterface[i]->addref();
       else return (unsigned long) -1;

       return 0;
}
//----------------------------------------------------------------------------
void MultiViewDescriptor::SetNoOfViews(unsigned char count )
{
	m_iNumberOfViews = count;
}
//----------------------------------------------------------------------------
unsigned char MultiViewDescriptor::GetNoOfViews(void) const
{
	return m_iNumberOfViews;
}
//----------------------------------------------------------------------------
void MultiViewDescriptor::SetVisibleViewFlag(int i, bool value)
{
	if(i < 8)
		m_pVisibleViewFlags[i] = value;
}
//----------------------------------------------------------------------------
bool MultiViewDescriptor::GetVisibleViewFlag(int i)
{
	if(i < 8)
		return m_pVisibleViewFlags[i];
	
	return false;
}
//----------------------------------------------------------------------------
void MultiViewDescriptor::SetFixedViewsFlag(bool value)
{
	m_bFixedViewsFlag = value;
}
//----------------------------------------------------------------------------
bool MultiViewDescriptor::GetFixedViewsFlag(void)
{
	return m_bFixedViewsFlag;
}

//-----------------------------------------------------------------------------
unsigned long MultiViewDescriptor::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  if (!aParentDescription)
    return (unsigned long)-1;

  aParentDescription->SetAttribute("fixedViewsFlag", GetFixedViewsFlag());

  int noOfViews = GetNoOfViews();
  for( int i = 0; i < noOfViews; i++)
  {
	  if (m_p2DDescriptorInterface[i])
	  {
		  bool isVisible = GetVisibleViewFlag(i);
		  GenericDS DDL_IsVisible = aParentDescription->CreateChild("IsVisible");
		  DDL_IsVisible.SetValue(isVisible ? "true" : "false");
		  m_p2DDescriptorInterface[i]->ExportDDL(aParentDescription);
	  }
  }

  return 0;
}
	

//-----------------------------------------------------------------------------
unsigned long MultiViewDescriptor::ImportDDL(GenericDSInterfaceABC *aDescription)
{
  if (!aDescription)
    return (unsigned long)-1;

  //GenericDS l_DDLDescription;
  //GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;

  bool viewFlag;
  int numberOfViews;
  aDescription->GetBoolAttribute("fixedViewsFlag", viewFlag);
  SetFixedViewsFlag(viewFlag);

  std::vector<GenericDS> vecIsVisible = aDescription->GetAllDescriptions("IsVisible");
  std::vector<GenericDS> vecDescriptors = aDescription->GetAllDescriptions("Descriptor");

  numberOfViews = vecDescriptors.size();
  if(vecIsVisible.size()  != numberOfViews)
	  return  (unsigned long)-1;

  SetNoOfViews(numberOfViews);

  GenericDS SubDescription = aDescription->GetDescription("Descriptor");

  for( int i = 0; i < numberOfViews; i++ )
  {
	  if (m_p2DDescriptorInterface[i]) 
	  {
		  if (!SubDescription.isNull()) 
		  {

			  int flag;
			  vecIsVisible[i].GetIntValue(flag);
			  SetVisibleViewFlag(i, flag);
			  m_p2DDescriptorInterface[i]->ImportDDL(SubDescription.GetInterface());
		  }
		  SubDescription = SubDescription.GetNextSibling("Descriptor");
		}
	}

  return 0;
}

