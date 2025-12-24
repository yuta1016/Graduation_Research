//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Nam Kyu Kim, Dong-Gye Sim, Hae-Kwang Kim - Hyundai
// Ajay Divakaran - Mitsubishi
// Xinding Sun  B. S. Manjunath-  UCSB
// Sharp corporation, Yasuaki Tokumo, Shuichi Watanabe.
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
// Applicable File Name:  MotionActivity.cpp
//
#include "Descriptors/Descriptors.h"                           /* Naming Convention */
#include <math.h>

using namespace XM;

const UUID MotionActivityDescriptorInterface::myID = UUID("");
const char *MotionActivityDescriptorInterface::myName = "Motion Activity Descriptor Interface";

const UUID MotionActivityDescriptor::myID = UUID("");
const char *MotionActivityDescriptor::myName = "Motion Activity Descriptor";

const UUID MotionActivityDescriptor::valID = UUID("");
const char *MotionActivityDescriptor::valName = "";

const UUID MotionActivityDescriptorInterfaceABC::myID = UUID();


MotionActivityDescriptorInterfaceABC::MotionActivityDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
MotionActivityDescriptorInterface::MotionActivityDescriptorInterface(MotionActivityDescriptor *aActivity):
m_MotionActivityDescriptor(aActivity)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MotionActivityDescriptorInterface::~MotionActivityDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void MotionActivityDescriptorInterface::addref(void)
{
m_MotionActivityDescriptor->addref();
}
//----------------------------------------------------------------------------
void MotionActivityDescriptorInterface::release(void)
{
m_MotionActivityDescriptor->release();
}

//----------------------------------------------------------------------------
int MotionActivityDescriptorInterface::GetStartFrameNum(void)
{
	return m_MotionActivityDescriptor->GetStartFrameNum();
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptorInterface::SetStartFrameNum(int frameindex)
{
	return m_MotionActivityDescriptor->SetStartFrameNum(frameindex);
}

//----------------------------------------------------------------------------
int MotionActivityDescriptorInterface::GetEndFrameNum(void)
{
	return m_MotionActivityDescriptor->GetEndFrameNum();
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptorInterface::SetEndFrameNum(int frameindex)
{
	return m_MotionActivityDescriptor->SetEndFrameNum(frameindex);
}

//----------------------------------------------------------------------------
int MotionActivityDescriptorInterface::GetIntensity(void)
{
	return m_MotionActivityDescriptor->GetIntensity();
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptorInterface::SetIntensity(int intensity)
{
	return m_MotionActivityDescriptor->SetIntensity(intensity);
}

 //----------------------------------------------------------------------------
bool MotionActivityDescriptorInterface::GetDirectionFlag(void)
{
	return m_MotionActivityDescriptor->GetDirectionFlag();
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptorInterface::SetDirectionFlag(bool flag)
{
	return m_MotionActivityDescriptor->SetDirectionFlag(flag);
}

//----------------------------------------------------------------------------
bool MotionActivityDescriptorInterface::GetSpatialDistributionFlag(void)
{
	return m_MotionActivityDescriptor->GetSpatialDistributionFlag();
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptorInterface::SetSpatialDistributionFlag(bool flag)
{
	return m_MotionActivityDescriptor->SetSpatialDistributionFlag(flag);
}

//------------------------------------------------------------------- for SLMA
bool MotionActivityDescriptorInterface::
GetSpatialLocalizedDistributionFlag(void)
{
	return m_MotionActivityDescriptor->
	  GetSpatialLocalizedDistributionFlag();
}
//------------------------------------------------------------------- for SLMA
unsigned long MotionActivityDescriptorInterface::
SetSpatialLocalizedDistributionFlag(bool flag)
{
	return m_MotionActivityDescriptor->
	  SetSpatialLocalizedDistributionFlag(flag);
}

//----------------------------------------------------------------------------
bool MotionActivityDescriptorInterface::GetTemporalDistributionFlag(void)
{
	return m_MotionActivityDescriptor->GetTemporalDistributionFlag();
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptorInterface::SetTemporalDistributionFlag(bool flag)
{
	return m_MotionActivityDescriptor->SetTemporalDistributionFlag(flag);
}

//----------------------------------------------------------------------------
int MotionActivityDescriptorInterface::GetDominantDirection(void)
{
	return m_MotionActivityDescriptor->GetDominantDirection();
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptorInterface::SetDominantDirection(int value)
{
	return m_MotionActivityDescriptor->SetDominantDirection(value);
}

//----------------------------------------------------------------------------
int MotionActivityDescriptorInterface::GetSpatialParameters(unsigned long index)
{
	return m_MotionActivityDescriptor->GetSpatialParameters(index);
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptorInterface::SetSpatialParameters( unsigned long index, int value )
{
	return m_MotionActivityDescriptor->SetSpatialParameters( index, value );
}

///*
//------------------------------------------------------------------- for SLMA
int MotionActivityDescriptorInterface::GetSpaLocNumber(void)
{
	return m_MotionActivityDescriptor->GetSpaLocNumber();
}
//------------------------------------------------------------------- for SLMA
unsigned long MotionActivityDescriptorInterface::SetSpaLocNumber(int num)
{
	return m_MotionActivityDescriptor->SetSpaLocNumber(num);
}
//*/
//------------------------------------------------------------------- for SLMA
int MotionActivityDescriptorInterface::
GetSpatialLocalizedParameter(unsigned long index)
{
	return m_MotionActivityDescriptor->
	  GetSpatialLocalizedParameter(index);
}
//------------------------------------------------------------------- for SLMA
unsigned long MotionActivityDescriptorInterface::
SetSpatialLocalizedParameter(unsigned long index, int value)
{
	return m_MotionActivityDescriptor->
	  SetSpatialLocalizedParameter(index, value);
}

//----------------------------------------------------------------------------
int MotionActivityDescriptorInterface::GetTemporalParameters(unsigned long index)
{
	return m_MotionActivityDescriptor->GetTemporalParameters(index);
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptorInterface::SetTemporalParameters( unsigned long index, int value )
{
	return m_MotionActivityDescriptor->SetTemporalParameters( index, value );
}


//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptorInterface::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
 
  return m_MotionActivityDescriptor->ExportDDL(aParentDescription);
}

//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptorInterface::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  return m_MotionActivityDescriptor->ImportDDL(aDescription);
}

//----------------------------------------------------------------------------
const UUID& MotionActivityDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionActivityDescriptorInterface::GetName(void)
{ return myName; }

//=============================================================================
MotionActivityDescriptor::MotionActivityDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MotionActivityDescriptor::~MotionActivityDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"MotionActivity delete\n");
#endif
}

//----------------------------------------------------------------------------
const UUID& MotionActivityDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionActivityDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& MotionActivityDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* MotionActivityDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool MotionActivityDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void MotionActivityDescriptor::addref(void)

{
#ifdef VERBOSE
       fprintf(stderr,"MotoionActivity connect\n");
#endif
       m_refcount++;
}

//----------------------------------------------------------------------------
void MotionActivityDescriptor::release(void)

{
#ifdef VERBOSE
       fprintf(stderr,"MotionActivity release\n");
#endif
       if (!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
int MotionActivityDescriptor::GetStartFrameNum(void)
{
	return m_StartFrameNum;
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptor::SetStartFrameNum(int frameindex)
{
	m_StartFrameNum = frameindex;

	return 0;
}

//----------------------------------------------------------------------------
int MotionActivityDescriptor::GetEndFrameNum(void)
{
	return m_EndFrameNum;
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptor::SetEndFrameNum(int frameindex)
{
	m_EndFrameNum = frameindex;

	return 0;
}

//----------------------------------------------------------------------------
int MotionActivityDescriptor::GetIntensity(void)
{
	return m_Intensity;
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptor::SetIntensity(int intensity)
{
	m_Intensity = intensity;

	return 0;
}

 //----------------------------------------------------------------------------
bool MotionActivityDescriptor::GetDirectionFlag(void)
{
	return m_DirectionFlag;
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptor::SetDirectionFlag(bool flag)
{
	m_DirectionFlag = flag;

	return 0;
}

//----------------------------------------------------------------------------
bool MotionActivityDescriptor::GetSpatialDistributionFlag(void)
{
	return m_SpatialDistributionFlag;
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptor::SetSpatialDistributionFlag(bool flag)
{
	m_SpatialDistributionFlag = flag;

	return 0;
}

//------------------------------------------------------------------- for SLMA
bool MotionActivityDescriptor::GetSpatialLocalizedDistributionFlag(void)
{
	return m_SpatialLocalizedDistributionFlag;
}
//------------------------------------------------------------------- for SLMA
unsigned long MotionActivityDescriptor::
SetSpatialLocalizedDistributionFlag(bool flag)
{
	m_SpatialLocalizedDistributionFlag = flag;
	return 0;
}

//----------------------------------------------------------------------------
bool MotionActivityDescriptor::GetTemporalDistributionFlag(void)
{
	return m_TemporalDistributionFlag;
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptor::SetTemporalDistributionFlag(bool flag)
{
	m_TemporalDistributionFlag = flag;

	return 0;
}

//----------------------------------------------------------------------------
int MotionActivityDescriptor::GetDominantDirection(void)
{
	return m_DominantDirection;
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptor::SetDominantDirection(int value)
{
	m_DominantDirection = value;

	return 0;
}

//----------------------------------------------------------------------------
int MotionActivityDescriptor::GetSpatialParameters(unsigned long index)
{
	if (index < 0 || index >= NUM_Nsr_Nmr_Nlr )
	  return (unsigned long)-1;

	return m_SpatialParameters[index];
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptor::SetSpatialParameters( unsigned long index, int value )
{
	if (index < 0 || index >= NUM_Nsr_Nmr_Nlr )
	  return (unsigned long)-1;

	m_SpatialParameters[index] = value;

	return 0;
}


//------------------------------------------------------------------- for SLMA
int MotionActivityDescriptor::GetSpaLocNumber(void)
{
	return m_SpaLocNumber;
}

//------------------------------------------------------------------- for SLMA
unsigned long MotionActivityDescriptor::SetSpaLocNumber(int num)
{
	m_SpaLocNumber = num;
	return 0;
}

//------------------------------------------------------------------- for SLMA
int MotionActivityDescriptor::GetSpatialLocalizedParameter(unsigned long index)
{
	if (index < 0 || index >= MAX_DIVISION_BLOCKS )
		return (unsigned long)-1;
	return m_SpatialLocalizedParameter[index];
}

//------------------------------------------------------------------- for SLMA
unsigned long MotionActivityDescriptor::
SetSpatialLocalizedParameter(unsigned long index, int value)
{
	if (index < 0 || index >= MAX_DIVISION_BLOCKS )
		return (unsigned long)-1;
	m_SpatialLocalizedParameter[index] = value;

	return 0;
}

//----------------------------------------------------------------------------
int MotionActivityDescriptor::GetTemporalParameters(unsigned long index)
{
	if (index < 0 || index >= NUM_N0_N1_N2_N3_N4 )
	  return (unsigned long)-1;

	return m_TemporalParameters[index];
}
//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptor::SetTemporalParameters( unsigned long index, int value )
{
	if (index < 0 || index >= NUM_N0_N1_N2_N3_N4 )
	  return (unsigned long)-1;

	m_TemporalParameters[index] = value;

	return 0;
}

//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptor::
ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
  // KW 2013/07/25 - modified according to the specification.
  GenericDS l_DDLDescription;
  long DirectionFlag,
		SpatialDistributionFlag,
		SpatialLocalizedDistributionFlag,
		TemporalDistributionFlag;


  if (!aParentDescription) return (unsigned long) -1;

  l_DDLDescription=aParentDescription->CreateChild("Descriptor");
  l_DDLDescription.SetAttribute("xsi:type","MotionActivityType");

  int num;
  num=(int)(pow(4.0, (double)(GetSpaLocNumber()+1.0)));
  vector<int> SLMA(num);
  
  DirectionFlag = GetDirectionFlag();
  SpatialDistributionFlag = GetSpatialDistributionFlag();
  SpatialLocalizedDistributionFlag = GetSpatialLocalizedDistributionFlag();
  TemporalDistributionFlag = GetTemporalDistributionFlag();


  // Intensity is a mandatory field.
  // The other fields are optional and hence their flags need to be tested

  GenericDS Intensity_element = l_DDLDescription.CreateChild("Intensity");
  Intensity_element.SetValue(GetIntensity());

  if (DirectionFlag != 0)
  {
    GenericDS DominantDirection_element = l_DDLDescription.CreateChild("DominantDirection");
    DominantDirection_element.SetValue(GetDominantDirection());
  }


  if (SpatialDistributionFlag != 0)
  {
    GenericDS SpatialParameters_element = l_DDLDescription.CreateChild("SpatialDistributionParams");
	SpatialParameters_element.SetAttribute("numOfShortRuns",GetSpatialParameters(0));
	SpatialParameters_element.SetAttribute("numOfMediumRuns",GetSpatialParameters(1));
	SpatialParameters_element.SetAttribute("numOfLongRuns",GetSpatialParameters(2));
  }



  if (SpatialLocalizedDistributionFlag != 0)
  {
    GenericDS SpatialLocalizationParameters_element = l_DDLDescription.CreateChild("SpatialLocalizationParams");
	GenericDS SpatialLocalizationParameters_vector;
	if(num == 4)
	{
		SpatialLocalizationParameters_vector = SpatialLocalizationParameters_element.CreateChild("Vector4");
	}
	else if(num == 16)
	{
		SpatialLocalizationParameters_vector = SpatialLocalizationParameters_element.CreateChild("Vector16");
	}
	else if(num == 64)
	{
		SpatialLocalizationParameters_vector = SpatialLocalizationParameters_element.CreateChild("Vector64");
	}
	else if(num == 128)
	{
		SpatialLocalizationParameters_vector = SpatialLocalizationParameters_element.CreateChild("Vector256");
	}
	else
	{
		return (unsigned long)-1;
	}
	
	int nslma;
	for (nslma=0;nslma<num;nslma++)
	{
		SLMA[nslma] = GetSpatialLocalizedParameter(nslma);
	}
	SpatialLocalizationParameters_vector.SetValue(SLMA);
  }



  if (TemporalDistributionFlag != 0)
  {
    vector<int>T_Par(5);
    int ntpar;
    for (ntpar=0;ntpar<5;ntpar++)
      {
	T_Par[ntpar]=GetTemporalParameters(ntpar);
      }
    GenericDS TemporalParameters_element = l_DDLDescription.CreateChild("TemporalParams");
    TemporalParameters_element.SetValue(T_Par);
  }
  return 0;
}

//----------------------------------------------------------------------------
unsigned long MotionActivityDescriptor::
ImportDDL(GenericDSInterfaceABC *aDescription)
{
  // 2013/07/25 KW - modified according to DDL specification.

  string xsitype;

  GenericDS l_DLLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface    = NULL;

  if (!aDescription) return (unsigned long) -1;

  /* if aDescriptor is of correct type*/
  if (aDescription->GetDSName() == "Descriptor") {
    aDescription->GetTextAttribute("xsi:type",xsitype);
    if (xsitype == "MotionActivityType") {
      l_DDLDescriptionInterface=aDescription;
    }
  }

  /* else search for MotionActivity as a child*/
  if (!l_DDLDescriptionInterface) {
    l_DLLDescription=aDescription->GetDescription("Descriptor");
    
    /* search for correct xsi type*/
    while (!l_DLLDescription.isNull()) {
      l_DLLDescription.GetTextAttribute("xsi:type", xsitype);
      if(xsitype=="MotionActivityType") break;
      l_DLLDescription=
	l_DLLDescription.GetNextSibling("Descriptor");
    }
      

    /* MotionActivity not found*/
    if (l_DLLDescription.isNull()) return (unsigned long)-1;
    /* MotionActivity found*/
    l_DDLDescriptionInterface=
      l_DLLDescription.GetInterface();
  }

  GenericDS Intensity_element = l_DDLDescriptionInterface->GetDescription("Intensity");
  int ma_intensity;
  bool generic_flag;
  Intensity_element.GetIntValue(ma_intensity); 
  SetIntensity(ma_intensity);

  GenericDS DominantDirection_element = l_DDLDescriptionInterface->GetDescription("DominantDirection");
  if (!DominantDirection_element.isNull())
    {
      int ma_dominantdirection;
      DominantDirection_element.GetIntValue(ma_dominantdirection);
      generic_flag=1;
      SetDirectionFlag(generic_flag); 
      SetDominantDirection(ma_dominantdirection);
    }
  else
    {
      generic_flag=0;
      SetDirectionFlag(generic_flag);
    }


  GenericDS SpatialParameters_element = l_DDLDescriptionInterface->GetDescription("SpatialDistributionParams");

  if (!SpatialParameters_element.isNull())

    {
	  int s = 0;
      generic_flag=1;
      SetSpatialDistributionFlag(generic_flag);
      int nsr,nmr,nlr;
	  s = SpatialParameters_element.GetIntAttribute("numOfShortRuns", nsr);
      SetSpatialParameters(0,nsr);

	  s = SpatialParameters_element.GetIntAttribute("numOfMediumRuns", nmr);
      SetSpatialParameters(1,nmr);

	  s = SpatialParameters_element.GetIntAttribute("numOfLongRuns", nlr);
      SetSpatialParameters(2,nlr);
    }

  else

    {
      generic_flag=0;
      SetSpatialDistributionFlag(generic_flag);

    }

  GenericDS SpatialLocalizationParameters_element = l_DDLDescriptionInterface->GetDescription("SpatialLocalizationParams");
  GenericDS SpatialLocalizationParameters_vector;

  if (!SpatialLocalizationParameters_element.isNull())
  {
	  SpatialLocalizationParameters_vector = SpatialLocalizationParameters_element.GetDescription("Vector4");
	  if(SpatialLocalizationParameters_vector.isNull())
		  SpatialLocalizationParameters_vector = SpatialLocalizationParameters_element.GetDescription("Vector16");
	  if(SpatialLocalizationParameters_vector.isNull())
		  SpatialLocalizationParameters_vector = SpatialLocalizationParameters_element.GetDescription("Vector64");
	  if(SpatialLocalizationParameters_vector.isNull())
		  SpatialLocalizationParameters_vector = SpatialLocalizationParameters_element.GetDescription("Vector256");
  }
  if (!SpatialLocalizationParameters_vector.isNull())
  {
      int num;
      int nslma;

	  generic_flag=1;
      SetSpatialLocalizedDistributionFlag(generic_flag);

      vector<int> SLMA;
      SpatialLocalizationParameters_vector.GetIntVector(SLMA);
	  num = SLMA.size();
      for (nslma=0;nslma<num;nslma++)
	{

	  SetSpatialLocalizedParameter(nslma,SLMA[nslma]);
	}
	  if(num == 4)
		  num = 0;
	  if(num == 16)
		  num = 1;
	  else if(num == 64)
		  num = 2;
	  else if(num == 256)
		  num = 3;
	  SetSpaLocNumber(num);
    }

  else

    {
      generic_flag=0;
      SetSpatialLocalizedDistributionFlag(generic_flag);
    }


  GenericDS TemporalParameters_element = l_DDLDescriptionInterface->GetDescription("TemporalParams");

  if (!TemporalParameters_element.isNull())
    {
      generic_flag=1;
      SetTemporalDistributionFlag(generic_flag);
      vector<int>T_Par;
      TemporalParameters_element.GetIntVector(T_Par);
      int ntpar;
      for (ntpar=0;ntpar<5;ntpar++)
	{
	  SetTemporalParameters(ntpar,T_Par[ntpar]);
	}

    }

  else

    {
      generic_flag=0;
      SetTemporalDistributionFlag(generic_flag);
    }
  return 0;
}

//----------------------------------------------------------------------------
MotionActivityDescriptorInterfaceABC *MotionActivityDescriptor::GetInterface(void)
{ return &m_Interface; }
