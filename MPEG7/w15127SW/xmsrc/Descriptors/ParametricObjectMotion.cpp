///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// Updated:
// Institut National des Telecommunications, ARTEMIS Project Unit,
//      Marius PREDA, Titus ZAHARIA (18/01/2000)
//
// Updated 12/13/00:
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - update according to LaBaule CD-text (coordinates, duration)
// - default values
// - error handling
//
// Updated 06/14/01:
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - XML-IO
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
// Applicable File Name:  ParametricObjectMotion.cpp
//
#include <stdio.h>
#include <string.h>
#include "Utilities/readparam.h"
#include "Descriptors/Descriptors.h"
#include "SearchUtilities/ParametricObjectMotionUtilities.h"

using namespace XM;


char *MotionModelNames[5] = { 
	"translational",
	"rotationOrScaling",
	"affine",
	"perspective",
	"quadratic"
};


const UUID ParametricObjectMotionDescriptorInterface::myID = UUID("8472d420-982c-11d3-922e-004005513c7a");
const char *ParametricObjectMotionDescriptorInterface::myName =
                         "Parametric Object Motion Description Interface";

const UUID ParametricObjectMotionDescriptor::myID = UUID("80ed4380-982c-11d3-922e-004005513c7a");
const char *ParametricObjectMotionDescriptor::myName = "Parametric Object Motion Descriptor";

const UUID ParametricObjectMotionDescriptor::valID = UUID("82576af0-982c-11d3-922e-004005513c7a");
const char *ParametricObjectMotionDescriptor::valName = "Parametric Object Motion";

const UUID ParametricObjectMotionDescriptorInterfaceABC::myID = UUID("");

ParametricObjectMotionDescriptorInterfaceABC::ParametricObjectMotionDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
ParametricObjectMotionDescriptorInterface::ParametricObjectMotionDescriptorInterface(ParametricObjectMotionDescriptor *aParametricObjectMotion):
m_ParametricObjectMotionDescriptor(aParametricObjectMotion)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ParametricObjectMotionDescriptorInterface::~ParametricObjectMotionDescriptorInterface()
{
}
	
//----------------------------------------------------------------------------
const UUID& ParametricObjectMotionDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ParametricObjectMotionDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void ParametricObjectMotionDescriptorInterface::addref(void)
{ m_ParametricObjectMotionDescriptor->addref(); }

//----------------------------------------------------------------------------
void ParametricObjectMotionDescriptorInterface::release(void)
{ m_ParametricObjectMotionDescriptor->release(); }

//----------------------------------------------------------------------------
void ParametricObjectMotionDescriptorInterface::
SetValuesFromParameterFile(void)
{

        m_ParametricObjectMotionDescriptor->
	  SetModelCode(atoi(getpara("MotionModel","2")));			// Aljoscha 12/13/00
                                   /* default = 2 = affine*/
#ifdef DEBUG
	fprintf(stderr,"Using: ModelCode %d\n",(int)
		m_ParametricObjectMotionDescriptor->
		GetModelCode());
#endif
}

// Aljoscha 12/13/00----------------------------------------------------------
unsigned long ParametricObjectMotionDescriptorInterface::GetCoordFlag(void)
{
	return m_ParametricObjectMotionDescriptor->GetCoordFlag();
}

// Aljoscha 12/13/00----------------------------------------------------------
unsigned long ParametricObjectMotionDescriptorInterface::SetCoordFlag(unsigned long value)	
{
	return m_ParametricObjectMotionDescriptor->SetCoordFlag(value);
}

// Aljoscha 12/13/00----------------------------------------------------------
unsigned long ParametricObjectMotionDescriptorInterface::GetCoordRef(void)
{
	return m_ParametricObjectMotionDescriptor->GetCoordRef();
}

// Aljoscha 12/13/00----------------------------------------------------------
unsigned long ParametricObjectMotionDescriptorInterface::SetCoordRef(unsigned long value)	
{
	return m_ParametricObjectMotionDescriptor->SetCoordRef(value);
}

// Aljoscha 12/13/00----------------------------------------------------------
unsigned long ParametricObjectMotionDescriptorInterface::GetSpatialReference(void)
{
	return m_ParametricObjectMotionDescriptor->GetSpatialReference();
}

// Aljoscha 12/13/00----------------------------------------------------------
unsigned long ParametricObjectMotionDescriptorInterface::SetSpatialReference(unsigned long value)	
{
	return m_ParametricObjectMotionDescriptor->SetSpatialReference(value);
}

//----------------------------------------------------------------------------
int ParametricObjectMotionDescriptorInterface::GetModelCode(void)
{
	return m_ParametricObjectMotionDescriptor->GetModelCode();
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionDescriptorInterface::SetModelCode(int value)	
{
	return m_ParametricObjectMotionDescriptor->SetModelCode(value);
}

// Aljoscha 12/13/00----------------------------------------------------------
int ParametricObjectMotionDescriptorInterface::GetDuration(void)
{
	return m_ParametricObjectMotionDescriptor->GetDuration();
}

// Aljoscha 12/13/00----------------------------------------------------------
unsigned long ParametricObjectMotionDescriptorInterface::SetDuration(int value)	
{
	return m_ParametricObjectMotionDescriptor->SetDuration(value);
}

//----------------------------------------------------------------------------
double *ParametricObjectMotionDescriptorInterface::GetMotionParameters(void)
{
	return m_ParametricObjectMotionDescriptor->GetMotionParameters();
}

//----------------------------------------------------------------------------
double ParametricObjectMotionDescriptorInterface::GetXOrigin(void)
{
	return m_ParametricObjectMotionDescriptor->GetXOrigin();
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionDescriptorInterface::SetXOrigin(double value)	
{
	return m_ParametricObjectMotionDescriptor->SetXOrigin(value);
}

//----------------------------------------------------------------------------
double ParametricObjectMotionDescriptorInterface::GetYOrigin(void)
{
	return m_ParametricObjectMotionDescriptor->GetYOrigin();
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionDescriptorInterface::SetYOrigin(double value)	
{
	return m_ParametricObjectMotionDescriptor->SetYOrigin(value);
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionDescriptorInterface::GetMotionParameterValue(int index,
								       double& value)
{
	return m_ParametricObjectMotionDescriptor->GetMotionParameterValue(index,value);
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionDescriptorInterface::SetMotionParameterValue(int index,
										 double value)
{
	return m_ParametricObjectMotionDescriptor->SetMotionParameterValue(index, value);
}


//-----------------------------------------------------------------------------
unsigned long ParametricObjectMotionDescriptorInterface::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  return m_ParametricObjectMotionDescriptor->ExportDDL(aParentDescription);
}

//-----------------------------------------------------------------------------
unsigned long ParametricObjectMotionDescriptorInterface::ImportDDL(GenericDSInterfaceABC *aDescription)
{
  return m_ParametricObjectMotionDescriptor->ImportDDL(aDescription);
}

//=============================================================================
ParametricObjectMotionDescriptor::ParametricObjectMotionDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
m_CoordFlag(0),							// Aljoscha 12/13/00
m_CoordRef(0),							// Aljoscha 12/13/00
m_SpatialReference(0),			// Aljoscha 12/13/00
m_ModelCode(2),							// Aljoscha 12/13/00
m_Duration(1),							// Aljoscha 12/13/00
m_MotionParameters(0),
m_XOrigin(0.0),
m_YOrigin(0.0),
m_NoOfMotionParameters(6)		// Aljoscha 12/13/00
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ParametricObjectMotionDescriptor::~ParametricObjectMotionDescriptor()
{
        if (m_MotionParameters) {
	  delete[] m_MotionParameters;
	}

#ifdef VERBOSE
       fprintf(stderr,"ParametricObjectMotion delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& ParametricObjectMotionDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ParametricObjectMotionDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ParametricObjectMotionDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ParametricObjectMotionDescriptor::GetValueName(void)
{ return valName; }

//-----------------------------------------------------------------------------
unsigned long ParametricObjectMotionDescriptor::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  char str[4096];

  int mCode;
  GenericDS l_Duration, l_Parameters;
  int noofpar;	
  unsigned long CoordFlag;
  double *p_param,xorg,yorg;
  int motionModelType;

  if (!aParentDescription)
    return (unsigned long)-1;

  GenericDS l_DDLDescription;
  l_DDLDescription = aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type", "ParametricMotionType");

  mCode=GetModelCode();
  if(mCode < 0) mCode = 0;	// Prevent crash
  if(mCode > 4) mCode = 4;
  l_DDLDescription.SetAttribute("motionModel", MotionModelNames[mCode]);

  CoordFlag=GetCoordFlag(); 

  if ( CoordFlag )
    {
	  GenericDS l_CoordRef;
	  l_CoordRef = l_DDLDescription.CreateChild("CoordRef");
	  l_CoordRef.SetAttribute("ref", (int)GetCoordRef());
      l_CoordRef.SetAttribute("spatialRef", (int)GetSpatialReference());
    }
  else
    {
	  GenericDS l_CoordDef;
	  l_CoordDef = l_DDLDescription.CreateChild("CoordDef");
 	  xorg = GetXOrigin();
	  sprintf( str, "%f", xorg);
	  l_CoordDef.SetAttribute("originX", str);

	  yorg = GetYOrigin();
	  sprintf( str, "%f", yorg);
	  l_CoordDef.SetAttribute("originY", str);
	}

  l_Duration = l_DDLDescription.CreateChild("MediaDuration");
  l_Duration.SetValue((unsigned short int)GetDuration());
 
  switch (mCode){
  case Translational:noofpar=2;;break;
  case Rotation_Scale:noofpar=4;;break;
  case Affine:noofpar=6;;break;
  case Perspective:noofpar=8;;break;
  case Quadric:noofpar=12;;break;
  default:noofpar=0;
  }

  p_param=GetMotionParameters();

  vector<double> Parameters(&p_param[0],&p_param[noofpar]);

  l_Parameters = l_DDLDescription.CreateChild("Parameters");
  l_Parameters.SetValue(Parameters);

  return 0;
}

//-----------------------------------------------------------------------------
unsigned long ParametricObjectMotionDescriptor::ImportDDL(GenericDSInterfaceABC *aDescription)
{
  if (!aDescription)
    return (unsigned long)-1;

  int spatialReference;
  int dur;
  int mCode = 0;
  int noofpar, i;	
  vector<double> Parameters;
  double xorg,yorg;
  GenericDS l_Duration, l_Parameters;
  GenericDS l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;

  string xsitype;
  if (aDescription->GetDSName() == "Descriptor")
  {
    aDescription->GetTextAttribute("xsi:type", xsitype);
    if (xsitype == "ParametricMotionType")
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
      if (xsitype == "ParametricMoptionType")
        break;
      l_DDLDescription = l_DDLDescription.GetNextSibling("Descriptor");
    }

    if (l_DDLDescription.isNull())
      return (unsigned long)-1;

    l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
  }

  std::string mmodel;
  l_DDLDescriptionInterface->GetTextAttribute("motionModel", mmodel);
  for(i = 0; i < 4; i++)
  {
	  if(strcmp(mmodel.c_str(), MotionModelNames[i]) == 0)
	  {
		  mCode = i;
		  break;
	  }
  }
  SetModelCode(mCode);

   GenericDS l_CoordDef;
   l_CoordDef = l_DDLDescriptionInterface->GetDescription("CoordDef");
   if(l_CoordDef.isNull())
   {
		int CoordRef;
		std::string ref;
		GenericDS l_CoordRef;
		l_CoordRef = l_DDLDescriptionInterface->GetDescription("CoordRef");
		l_CoordRef.GetTextAttribute("ref", ref );
		l_CoordRef.GetIntAttribute("spatialRef", spatialReference );
		CoordRef = atoi(ref.c_str());	// This may need fixing.
		SetCoordRef( CoordRef );
		SetSpatialReference( spatialReference );	
   }
   else
   {
		l_CoordDef = l_DDLDescriptionInterface->GetDescription("CoordDef");
		l_CoordDef.GetRealAttribute("originX", xorg);
		l_CoordDef.GetRealAttribute("originY", yorg);
		SetXOrigin(xorg);
		SetYOrigin(yorg);
   }
  
  l_Duration = l_DDLDescriptionInterface->GetDescription("MediaDuration");
  l_Duration.GetIntValue(dur);
  SetDuration(dur);

  switch (mCode){
  case Translational:noofpar=2;;break;
  case Rotation_Scale:noofpar=4;;break;
  case Affine:noofpar=6;;break;
  case Perspective:noofpar=8;;break;
  case Quadric:noofpar=12;;break;
  default:noofpar=0;
  }

  l_Parameters = l_DDLDescriptionInterface->GetDescription("Parameters");
  
  l_Parameters.GetRealVector(Parameters);

  for (i=0;i<noofpar;i++)
	SetMotionParameterValue( i, Parameters[i] );

  return 0;
}

//----------------------------------------------------------------------------
bool ParametricObjectMotionDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void ParametricObjectMotionDescriptor::addref(void)

{
#ifdef VERBOSE
       fprintf(stderr,"ParametricObjectMotion connect\n");
#endif
       m_refcount++;
}

//----------------------------------------------------------------------------
void ParametricObjectMotionDescriptor::release(void)

{
#ifdef VERBOSE
       fprintf(stderr,"ParametricObjectMotion release\n");
#endif
       if (!(--m_refcount)) delete this;
}

// Aljoscha 12/13/00----------------------------------------------------------
unsigned long ParametricObjectMotionDescriptor::GetCoordFlag(void)
{
	return m_CoordFlag;
}

// Aljoscha 12/13/00----------------------------------------------------------
unsigned long ParametricObjectMotionDescriptor::SetCoordFlag(unsigned long value)
{
        m_CoordFlag = value;

	return 0;
}

// Aljoscha 12/13/00----------------------------------------------------------
unsigned long ParametricObjectMotionDescriptor::GetCoordRef(void)
{
	return m_CoordRef;
}

// Aljoscha 12/13/00----------------------------------------------------------
unsigned long ParametricObjectMotionDescriptor::SetCoordRef(unsigned long value)
{
        m_CoordRef = value;

	return 0;
}

// Aljoscha 12/13/00----------------------------------------------------------
unsigned long ParametricObjectMotionDescriptor::GetSpatialReference(void)
{
	return m_SpatialReference;
}

// Aljoscha 12/13/00----------------------------------------------------------
unsigned long ParametricObjectMotionDescriptor::SetSpatialReference(unsigned long value)
{
        m_SpatialReference = value;

	return 0;
}

//----------------------------------------------------------------------------
int ParametricObjectMotionDescriptor::GetModelCode(void)
{
	return m_ModelCode;
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionDescriptor::SetModelCode(int value)
{
  if ( ( value < 0 ) || ( value > 4 ) )
		{																			// Aljoscha 12/13/00, error handling
			fprintf(stderr,"Inavlid model code (must be within [0,4]): %d\n",value);
			fprintf(stderr,"using default: 2 = affine\n");

			m_ModelCode = 2;
		}
	else
	  m_ModelCode = value;

	if (m_MotionParameters != 0)
		delete [] m_MotionParameters;

	if ( m_ModelCode == 4 )
	  m_NoOfMotionParameters = 12;
	else	  
	  m_NoOfMotionParameters = 2*(m_ModelCode+1);

	m_MotionParameters = new double [m_NoOfMotionParameters];

	return 0;
}

// Aljoscha 12/13/00----------------------------------------------------------
int ParametricObjectMotionDescriptor::GetDuration(void)
{
	return m_Duration;
}

// Aljoscha 12/13/00----------------------------------------------------------
unsigned long ParametricObjectMotionDescriptor::SetDuration(int value)
{
        m_Duration = value;

	return 0;
}

//----------------------------------------------------------------------------
double *ParametricObjectMotionDescriptor::GetMotionParameters(void)
{
	return m_MotionParameters;
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionDescriptor::SetMotionParameterValue(int index, double value)
{
	if (index < 0 || index >= m_NoOfMotionParameters)
		return (unsigned long)-1;

	m_MotionParameters[index] = value;

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionDescriptor::GetMotionParameterValue(int index, double& value )
{
	if (index < 0 || index >= m_NoOfMotionParameters)
		return (unsigned long)-1;

	value = m_MotionParameters[index];

	return 0;
}


//----------------------------------------------------------------------------
double ParametricObjectMotionDescriptor::GetXOrigin(void)
{
	return m_XOrigin;
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionDescriptor::SetXOrigin(double value)
{
        m_XOrigin = value;

	return 0;
}

//----------------------------------------------------------------------------
double ParametricObjectMotionDescriptor::GetYOrigin(void)
{
	return m_YOrigin;
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionDescriptor::SetYOrigin(double value)
{
        m_YOrigin = value;

	return 0;
}


//----------------------------------------------------------------------------
ParametricObjectMotionDescriptorInterfaceABC *ParametricObjectMotionDescriptor::GetInterface(void)
{ return &m_Interface; }



