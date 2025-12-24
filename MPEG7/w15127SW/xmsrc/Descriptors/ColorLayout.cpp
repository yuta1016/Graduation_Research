///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// NEC Corp., Akio Yamada, Eiji Kasutani.
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
#include "Descriptors/Descriptors.h"
/*#include "imgutil.h"*/
#include <vector>

using namespace XM;

const UUID ColorLayoutDescriptorInterface::myID = UUID("");
const char *ColorLayoutDescriptorInterface::myName =
                         "Color Layout Description Interface";

const UUID ColorLayoutDescriptor::myID = UUID("");
const char *ColorLayoutDescriptor::myName = "Color Layout Descriptor";
const UUID ColorLayoutDescriptor::valID = UUID("");
const char *ColorLayoutDescriptor::valName = "Color Layout";

const UUID ColorLayoutDescriptorInterfaceABC::myID = UUID();

ColorLayoutDescriptorInterfaceABC::ColorLayoutDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
ColorLayoutDescriptorInterface::ColorLayoutDescriptorInterface(ColorLayoutDescriptor *aColorLayout):
m_ColorLayoutDescriptor(aColorLayout)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ColorLayoutDescriptorInterface::~ColorLayoutDescriptorInterface()
{
}

// start modification for version 3.0	
//----------------------------------------------------------------------------
void ColorLayoutDescriptorInterface::addref(void)
{ m_ColorLayoutDescriptor->addref(); }

//----------------------------------------------------------------------------
void ColorLayoutDescriptorInterface::release(void)
{ m_ColorLayoutDescriptor->release(); }

//----------------------------------------------------------------------------
void ColorLayoutDescriptorInterface::SetValuesFromParameterFile(void)

{
}
// end modification for version 3.0	

//----------------------------------------------------------------------------
const UUID& ColorLayoutDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorLayoutDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
int ColorLayoutDescriptorInterface::GetNumberOfYCoeff()
{
	return m_ColorLayoutDescriptor->GetNumberOfYCoeff();
}

//----------------------------------------------------------------------------
int ColorLayoutDescriptorInterface::GetNumberOfCCoeff()
{
	return m_ColorLayoutDescriptor->GetNumberOfCCoeff();
}

//----------------------------------------------------------------------------
int* ColorLayoutDescriptorInterface::GetYCoeff()
{
	return m_ColorLayoutDescriptor->GetYCoeff();
}

//----------------------------------------------------------------------------
int* ColorLayoutDescriptorInterface::GetCbCoeff()
{
	return m_ColorLayoutDescriptor->GetCbCoeff();
}

//----------------------------------------------------------------------------
int* ColorLayoutDescriptorInterface::GetCrCoeff()
{
	return m_ColorLayoutDescriptor->GetCrCoeff();
}

//-----------------------------------------------------------------------------
void ColorLayoutDescriptorInterface::
SetNumberOfYCoeff(int NumberOfYCoeff)
{
    m_ColorLayoutDescriptor->
      SetNumberOfYCoeff(NumberOfYCoeff);
}

//-----------------------------------------------------------------------------
void ColorLayoutDescriptorInterface::
SetNumberOfCCoeff(int NumberOfCCoeff)
{
    m_ColorLayoutDescriptor->
      SetNumberOfCCoeff(NumberOfCCoeff);
}

//---------------------------------------------------------------------------
void ColorLayoutDescriptorInterface::SetYCoeff(int *YCoeff)
{
    m_ColorLayoutDescriptor->SetYCoeff(YCoeff);
}

//---------------------------------------------------------------------------
void ColorLayoutDescriptorInterface::SetCbCoeff(int *CbCoeff)
{
    m_ColorLayoutDescriptor->SetCbCoeff(CbCoeff);
}

//---------------------------------------------------------------------------
void ColorLayoutDescriptorInterface::SetCrCoeff(int *CrCoeff)
{
    m_ColorLayoutDescriptor->SetCrCoeff(CrCoeff);
}
//----------------------------------------------------------------------------
unsigned long ColorLayoutDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{ return m_ColorLayoutDescriptor->ExportDDL(aParentDescription); }

//----------------------------------------------------------------------------
unsigned long ColorLayoutDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *DescriptorRoot)//sth see header
{ return m_ColorLayoutDescriptor->ImportDDL(DescriptorRoot); }

#ifdef DEBUG_FOR_CL_DDL
//----------------------------------------------------------------------------
void ColorLayoutDescriptorInterface::
Print(FILE *fp)
{ m_ColorLayoutDescriptor->Print(fp); }

//----------------------------------------------------------------------------
bool ColorLayoutDescriptorInterface::
IsEqual(int num_y, int num_c, int *y_coeff, int *cb_coeff, int *cr_coeff)
{ m_ColorLayoutDescriptor->IsEqual(num_y, num_c, y_coeff, cb_coeff, cr_coeff); }
#endif

//===========================================================================
ColorLayoutDescriptor::ColorLayoutDescriptor():
// start modification for version 3.0
m_refcount(0),
// end modification for version 3.0
m_Interface(this),
m_isProprietary(false)
{
	m_NumberOfYCoeff=6;
	m_NumberOfCCoeff=3;
	m_y_coeff=new int[m_NumberOfYCoeff];
	m_cb_coeff=new int[m_NumberOfCCoeff];
	m_cr_coeff=new int[m_NumberOfCCoeff];
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ColorLayoutDescriptor::~ColorLayoutDescriptor()
{
	if(m_y_coeff) delete [] m_y_coeff;
	if(m_cb_coeff) delete [] m_cb_coeff;
	if(m_cr_coeff) delete [] m_cr_coeff;
}

//----------------------------------------------------------------------------
const UUID& ColorLayoutDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorLayoutDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ColorLayoutDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ColorLayoutDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
unsigned long ColorLayoutDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
	if(!aParentDescription) return (unsigned long) -1;

	GenericDS DescriptorRoot;
	DescriptorRoot=aParentDescription->CreateChild("Descriptor"); /* Yamada's memo: Why you can say Descriptor? */

 	DescriptorRoot.SetAttribute("xsi:type","ColorLayoutType");

	int num;
	int *coeff, i;
	char name[256];

	GenericDS  YDCCoeff_element = DescriptorRoot.CreateChild("YDCCoeff");
	coeff=GetYCoeff();
	YDCCoeff_element.SetValue(coeff[0]);
	GenericDS  CbDCCoeff_element = DescriptorRoot.CreateChild("CbDCCoeff");
	coeff=GetCbCoeff();
	CbDCCoeff_element.SetValue(coeff[0]);
	GenericDS  CrDCCoeff_element = DescriptorRoot.CreateChild("CrDCCoeff");
	coeff=GetCrCoeff();
	CrDCCoeff_element.SetValue(coeff[0]);


	num=GetNumberOfYCoeff();	
	coeff=GetYCoeff();
	vector<int> AC_Y(num-1);
	for(i=1; i<num; i++) AC_Y[i-1]=coeff[i];
	sprintf(name, "YACCoeff%d", num-1);
	GenericDS  YACCoeff_element = DescriptorRoot.CreateChild(name);
	YACCoeff_element.SetValue(AC_Y);

	num=GetNumberOfCCoeff();
	vector<int> AC_C(num-1);

	coeff=GetCbCoeff();
	for(i=1; i<num; i++) AC_C[i-1]=coeff[i];
	sprintf(name, "CbACCoeff%d", num-1);
	GenericDS  CbACCoeff_element = DescriptorRoot.CreateChild(name);
	CbACCoeff_element.SetValue(AC_C);

	coeff=GetCrCoeff();
	for(i=1; i<num; i++) AC_C[i-1]=coeff[i];
	sprintf(name, "CrACCoeff%d", num-1);
	GenericDS  CrACCoeff_element = DescriptorRoot.CreateChild(name);
	CrACCoeff_element.SetValue(AC_C);

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ColorLayoutDescriptor::
ImportDDL(GenericDSInterfaceABC *DescriptorRoot)
{
	int i;
	int val;
	string text;
	if(!DescriptorRoot) return (unsigned long)-1;

	/*
	DescriptorRoot->GetTextAttribute("xsi:type", text);
	if(text!="ColorLayoutType") return (unsigned long)-2;
	*/

	GenericDS l_DLLDescription;
	GenericDSInterfaceABC *l_DDLDescriptionInterface=NULL;

	if(DescriptorRoot->GetDSName()=="Descriptor"){
		/* memo again: Why you can restrict the name only to Descriptor? */
		DescriptorRoot->GetTextAttribute("xsi:type", text);
		if(text=="ColorLayoutType"){
			l_DDLDescriptionInterface=DescriptorRoot;
		}
	}
	if(!l_DDLDescriptionInterface){
		l_DLLDescription=DescriptorRoot->GetDescription("Descriptor");
		/* search for correct xsi type*/
		while(!l_DLLDescription.isNull()){
			l_DLLDescription.GetTextAttribute("xsi:type", text);
			if(text=="ColorLayoutType") break;
			l_DLLDescription=l_DLLDescription.GetNextSibling("Descriptor");
		}
		/* DummyType not found*/
		if(l_DLLDescription.isNull()) return (unsigned long)-1;
		/* DummyType found*/
		l_DDLDescriptionInterface=l_DLLDescription.GetInterface();
	}

//sth if parent node gives itself check if an apropriate child node exists

	GenericDS YDCCoeff_element  = l_DDLDescriptionInterface->GetDescription("YDCCoeff");
	GenericDS CbDCCoeff_element  = l_DDLDescriptionInterface->GetDescription("CbDCCoeff");
	GenericDS CrDCCoeff_element  = l_DDLDescriptionInterface->GetDescription("CrDCCoeff");

	GenericDS YACCoeff2_element  = l_DDLDescriptionInterface->GetDescription("YACCoeff2");
	GenericDS YACCoeff5_element  = l_DDLDescriptionInterface->GetDescription("YACCoeff5");
	GenericDS YACCoeff9_element  = l_DDLDescriptionInterface->GetDescription("YACCoeff9");
	GenericDS YACCoeff14_element  = l_DDLDescriptionInterface->GetDescription("YACCoeff14");
	GenericDS YACCoeff20_element  = l_DDLDescriptionInterface->GetDescription("YACCoeff20");
	GenericDS YACCoeff27_element  = l_DDLDescriptionInterface->GetDescription("YACCoeff27");
	GenericDS YACCoeff63_element  = l_DDLDescriptionInterface->GetDescription("YACCoeff63");
	GenericDS *YACCoeff_element = &YACCoeff2_element;
	int numOfYCoeff=3;
	if(YACCoeff_element->isNull()){ YACCoeff_element=&YACCoeff5_element; numOfYCoeff=6; }
	if(YACCoeff_element->isNull()){ YACCoeff_element=&YACCoeff9_element; numOfYCoeff=10; }
	if(YACCoeff_element->isNull()){ YACCoeff_element=&YACCoeff14_element; numOfYCoeff=15; }
	if(YACCoeff_element->isNull()){ YACCoeff_element=&YACCoeff20_element; numOfYCoeff=21; }
	if(YACCoeff_element->isNull()){ YACCoeff_element=&YACCoeff27_element; numOfYCoeff=28; }
	if(YACCoeff_element->isNull()){ YACCoeff_element=&YACCoeff63_element; numOfYCoeff=64; }

	GenericDS CbACCoeff2_element  = l_DDLDescriptionInterface->GetDescription("CbACCoeff2");
	GenericDS CbACCoeff5_element  = l_DDLDescriptionInterface->GetDescription("CbACCoeff5");
	GenericDS CbACCoeff9_element  = l_DDLDescriptionInterface->GetDescription("CbACCoeff9");
	GenericDS CbACCoeff14_element  = l_DDLDescriptionInterface->GetDescription("CbACCoeff14");
	GenericDS CbACCoeff20_element  = l_DDLDescriptionInterface->GetDescription("CbACCoeff20");
	GenericDS CbACCoeff27_element  = l_DDLDescriptionInterface->GetDescription("CbACCoeff27");
	GenericDS CbACCoeff63_element  = l_DDLDescriptionInterface->GetDescription("CbACCoeff63");
	GenericDS *CbACCoeff_element = &CbACCoeff2_element;
	int numOfCCoeff=3;
	if(CbACCoeff_element->isNull()){ CbACCoeff_element=&CbACCoeff5_element; numOfCCoeff=6; }
	if(CbACCoeff_element->isNull()){ CbACCoeff_element=&CbACCoeff9_element; numOfCCoeff=10; }
	if(CbACCoeff_element->isNull()){ CbACCoeff_element=&CbACCoeff14_element; numOfCCoeff=15; }
	if(CbACCoeff_element->isNull()){ CbACCoeff_element=&CbACCoeff20_element; numOfCCoeff=21; }
	if(CbACCoeff_element->isNull()){ CbACCoeff_element=&CbACCoeff27_element; numOfCCoeff=28; }
	if(CbACCoeff_element->isNull()){ CbACCoeff_element=&CbACCoeff63_element; numOfCCoeff=64; }

	GenericDS CrACCoeff2_element  = l_DDLDescriptionInterface->GetDescription("CrACCoeff2");
	GenericDS CrACCoeff5_element  = l_DDLDescriptionInterface->GetDescription("CrACCoeff5");
	GenericDS CrACCoeff9_element  = l_DDLDescriptionInterface->GetDescription("CrACCoeff9");
	GenericDS CrACCoeff14_element  = l_DDLDescriptionInterface->GetDescription("CrACCoeff14");
	GenericDS CrACCoeff20_element  = l_DDLDescriptionInterface->GetDescription("CrACCoeff20");
	GenericDS CrACCoeff27_element  = l_DDLDescriptionInterface->GetDescription("CrACCoeff27");
	GenericDS CrACCoeff63_element  = l_DDLDescriptionInterface->GetDescription("CrACCoeff63");
	GenericDS *CrACCoeff_element;
	switch(numOfCCoeff){
	case  3: CrACCoeff_element = &CrACCoeff2_element; break;
	case  6: CrACCoeff_element = &CrACCoeff5_element; break;
	case 10: CrACCoeff_element = &CrACCoeff9_element; break;
	case 15: CrACCoeff_element = &CrACCoeff14_element; break;
	case 21: CrACCoeff_element = &CrACCoeff20_element; break;
	case 28: CrACCoeff_element = &CrACCoeff27_element; break;
	case 63: CrACCoeff_element = &CrACCoeff63_element; break;
	default: fprintf(stderr, "Invalid number of C Coeff.[%d]\n", numOfCCoeff);
	}
	if(CrACCoeff_element->isNull()){
		fprintf(stderr, "Invalid instance. The number of Cb and Cr coefficients should be synchronized.\n");
		exit(-1);
	}

	int *y_coeff=new int[numOfYCoeff];
	YDCCoeff_element.GetIntValue(val);
	y_coeff[0]=val;
	if(!YACCoeff_element->isNull()){
		vector<int> AC;
		if( YACCoeff_element->GetIntVector(AC)==0 ){
			for(i=1; i<numOfYCoeff; i++) y_coeff[i]=AC[i-1];
		}
	}
	SetNumberOfYCoeff(numOfYCoeff);
	SetYCoeff(y_coeff);
	delete [] y_coeff;

	int *c_coeff=new int[numOfCCoeff];

	CbDCCoeff_element.GetIntValue(val);
	c_coeff[0]=val;
	if(!CbACCoeff_element->isNull()){
		vector<int> AC;
		if( CbACCoeff_element->GetIntVector(AC)==0 ){
			for(i=1; i<numOfCCoeff; i++) c_coeff[i]=AC[i-1];
		}
	}
	SetNumberOfCCoeff(numOfCCoeff);
	SetCbCoeff(c_coeff);

	CrDCCoeff_element.GetIntValue(val);
	c_coeff[0]=val;
	if(!CrACCoeff_element->isNull()){
		vector<int> AC;
		if( CrACCoeff_element->GetIntVector(AC)==0 ){
			for(i=1; i<numOfCCoeff; i++) c_coeff[i]=AC[i-1];
		}
	}
	SetCrCoeff(c_coeff);

	delete [] c_coeff;

	return 0;
}

//----------------------------------------------------------------------------
bool ColorLayoutDescriptor::IsProprietary(void)
{ return m_isProprietary; }

// start modification for version 3.0
//----------------------------------------------------------------------------
void ColorLayoutDescriptor::addref(void)

{
#ifdef VERBOSE
       fprintf(stderr,"ColorLayout connect\n");
#endif
       m_refcount++;
}

//----------------------------------------------------------------------------
void ColorLayoutDescriptor::release(void)

{
#ifdef VERBOSE
       fprintf(stderr,"ColorLayout release\n");
#endif
       if (!(--m_refcount)) delete this;
}
// end modification for version 3.0

//----------------------------------------------------------------------------
int ColorLayoutDescriptor::GetNumberOfYCoeff()
{
	return (m_NumberOfYCoeff);
}

//----------------------------------------------------------------------------
int ColorLayoutDescriptor::GetNumberOfCCoeff()
{
	return (m_NumberOfCCoeff);
}

//----------------------------------------------------------------------------
int* ColorLayoutDescriptor::GetYCoeff()
{
	return (m_y_coeff);
}

//----------------------------------------------------------------------------
int* ColorLayoutDescriptor::GetCbCoeff()
{
	return (m_cb_coeff);
}

//----------------------------------------------------------------------------
int* ColorLayoutDescriptor::GetCrCoeff()
{
	return (m_cr_coeff);
}

//----------------------------------------------------------------------------
void ColorLayoutDescriptor::
SetNumberOfYCoeff(int NumberOfYCoeff)
{
    switch(NumberOfYCoeff){
    case 3: case 6: case 10: case 15: case 21: case 28: case 64:
	m_NumberOfYCoeff=NumberOfYCoeff;
	if(m_y_coeff) delete [] m_y_coeff;
	m_y_coeff=new int[m_NumberOfYCoeff];
	break;
    default:
        fprintf(stderr, "Invalid Number of Y Coeff. [%d]\n", NumberOfYCoeff);
    }
}

//----------------------------------------------------------------------------
void ColorLayoutDescriptor::
SetNumberOfCCoeff(int NumberOfCCoeff)
{
    switch(NumberOfCCoeff){
    case 3: case 6: case 10: case 15: case 21: case 28: case 64:
	m_NumberOfCCoeff=NumberOfCCoeff;
	if(m_cb_coeff) delete [] m_cb_coeff;
	if(m_cr_coeff) delete [] m_cr_coeff;
	m_cb_coeff=new int[m_NumberOfCCoeff];
	m_cr_coeff=new int[m_NumberOfCCoeff];
	break;
    default:
        fprintf(stderr, "Invalid Number of C Coeff. [%d]\n", NumberOfCCoeff);
    }
}

//----------------------------------------------------------------------------
void ColorLayoutDescriptor::SetYCoeff(int *YCoeff)
{
	int i;
	for(i=0; i<m_NumberOfYCoeff; i++){
		m_y_coeff[i]=YCoeff[i];
        }
}

//----------------------------------------------------------------------------
void ColorLayoutDescriptor::SetCbCoeff(int *CbCoeff)
{
	int i;
	for(i=0; i<m_NumberOfCCoeff; i++){
		m_cb_coeff[i]=CbCoeff[i];
	}
}

//----------------------------------------------------------------------------
void ColorLayoutDescriptor::SetCrCoeff(int *CrCoeff)
{
	int i;
	for(i=0; i<m_NumberOfCCoeff; i++){
		m_cr_coeff[i]=CrCoeff[i];
	}
}

#ifdef DEBUG_FOR_CL_DDL
//----------------------------------------------------------------------------
void ColorLayoutDescriptor::Print(FILE *fp)
{
	int i;
	fprintf(fp, "Y[%d]", m_NumberOfYCoeff);
	for(i=0; i<m_NumberOfYCoeff; i++) fprintf(fp, " %d", m_y_coeff[i]);
	fprintf(fp, " \t");
	fprintf(fp, "Cb[%d]", m_NumberOfCCoeff);
	for(i=0; i<m_NumberOfCCoeff; i++) fprintf(fp, " %3d", m_cb_coeff[i]);
	fprintf(fp, " \t");
	fprintf(fp, "Cr[%d]", m_NumberOfCCoeff);
	for(i=0; i<m_NumberOfCCoeff; i++) fprintf(fp, " %3d", m_cr_coeff[i]);
	fprintf(fp, "\n");
};

//----------------------------------------------------------------------------
bool ColorLayoutDescriptor::IsEqual(int num_y, int num_c, int *y_coeff, int *cb_coeff, int *cr_coeff)
{
	int i;
	if(m_NumberOfYCoeff!=num_y) return false;
	for(i=0; i<num_y; i++){
		if(m_y_coeff[i]!=y_coeff[i]) return false;
	}

	if(m_NumberOfCCoeff!=num_c) return false;
	for(i=0; i<num_c; i++){
		if(m_cb_coeff[i]!=cb_coeff[i]) return false;
		if(m_cr_coeff[i]!=cr_coeff[i]) return false;
	}
	return true;
}
#endif

//----------------------------------------------------------------------------
ColorLayoutDescriptorInterfaceABC *ColorLayoutDescriptor::GetInterface(void)
{ return &m_Interface; }
//----------------------------------------------------------------------------


