///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Siemens AG, F. Sanahuja
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
// Applicable File Name:  RegionLocator.cpp

#include <fstream>
#include <iostream>
#include <math.h>
#include <cstring>

#include "Descriptors/Descriptors.h"
#include "RegionLocator.h"

using namespace XM;
								
const UUID RegionLocatorDescriptorInterface::myID = UUID("");
const char *RegionLocatorDescriptorInterface::myName = "Region Locator Descriptor Interface";

const UUID RegionLocatorDescriptor::myID = UUID("");
const char *RegionLocatorDescriptor::myName = "Region Locator Descriptor";

const UUID RegionLocatorDescriptor::valID = UUID("");
const char *RegionLocatorDescriptor::valName = "";

const UUID RegionLocatorDescriptorInterfaceABC::myID = UUID("");


//----------------------------------------------------------------------------
RegionLocatorDescriptorInterfaceABC::RegionLocatorDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//----------------------------------------------------------------------------
RegionLocatorDescriptorInterface::RegionLocatorDescriptorInterface(RegionLocatorDescriptor *aRegionLocator):
m_RegionLocatorDescriptor(aRegionLocator)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
RegionLocatorDescriptorInterface::~RegionLocatorDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void RegionLocatorDescriptorInterface::addref(void)
{ m_RegionLocatorDescriptor->addref(); }

//----------------------------------------------------------------------------
void RegionLocatorDescriptorInterface::release(void)
{ m_RegionLocatorDescriptor->release(); }

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptorInterface::GetCoordFlag(void)
{
	return m_RegionLocatorDescriptor->GetCoordFlag();
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptorInterface::GetOneLocator(void)
{
	return m_RegionLocatorDescriptor->GetOneLocator();
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptorInterface::SetOneLocator(unsigned long OneLocator)
{
	return m_RegionLocatorDescriptor->SetOneLocator(OneLocator);
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptorInterface::SetCoordFlag(unsigned long f)
{
	return m_RegionLocatorDescriptor->SetCoordFlag(f);
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptorInterface::GetSpatialReference(void)
{
	return m_RegionLocatorDescriptor->GetSpatialReference();
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptorInterface::SetSpatialReference(unsigned long f)
{
	return m_RegionLocatorDescriptor->SetSpatialReference(f);
}

//----------------------------------------------------------------------------
char *RegionLocatorDescriptorInterface::GetCoordRef()
{
	return m_RegionLocatorDescriptor->GetCoordRef();
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptorInterface::SetCoordRef(char *r)
{
	return m_RegionLocatorDescriptor->SetCoordRef(r);
}

//----------------------------------------------------------------------------
int RegionLocatorDescriptorInterface::GetRepresentation(int component)
{
	return m_RegionLocatorDescriptor->GetRepresentation(component);
}

//----------------------------------------------------------------------------
int RegionLocatorDescriptorInterface::SetRepresentation(int XBits, int YBits)
{
	return m_RegionLocatorDescriptor->SetRepresentation(XBits,YBits);
}

//----------------------------------------------------------------------------
void RegionLocatorDescriptorInterface::ResetDescriptor(void)
{ m_RegionLocatorDescriptor->ResetDescriptor(); }

//----------------------------------------------------------------------------
int RegionLocatorDescriptorInterface::GetNumberOfPolygons(void)
{
	return m_RegionLocatorDescriptor->GetNumberOfPolygons();
}

//----------------------------------------------------------------------------
int RegionLocatorDescriptorInterface::GetNumberOfBoxes(void)
{
	return m_RegionLocatorDescriptor->GetNumberOfBoxes();
}

//----------------------------------------------------------------------------
int RegionLocatorDescriptorInterface::SetInverseFlag(int B_P, int index, bool flag)
{
	return m_RegionLocatorDescriptor->SetInverseFlag(B_P, index, flag);
}

//----------------------------------------------------------------------------
int RegionLocatorDescriptorInterface::GetNumberOfVertices(int B_P, int index)
{
	return m_RegionLocatorDescriptor->GetNumberOfVertices(B_P, index);
}

//----------------------------------------------------------------------------
int RegionLocatorDescriptorInterface::SetLocator(int B_P, bool invers, int NoV, unsigned int *vertices)
{
	return m_RegionLocatorDescriptor->SetLocator(B_P, invers, NoV, vertices);
}

//----------------------------------------------------------------------------
struct loc RegionLocatorDescriptorInterface::GetLocator(int B_P, int index)
{
	return m_RegionLocatorDescriptor->GetLocator(B_P, index);
}

//----------------------------------------------------------------------------
const UUID& RegionLocatorDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RegionLocatorDescriptorInterface::GetName(void)
{ return myName; }
//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptorInterface::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
	return m_RegionLocatorDescriptor->ExportDDL(aParentDescription);
}
//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptorInterface::ImportDDL(GenericDSInterfaceABC *aDescription)
{
	return m_RegionLocatorDescriptor->ImportDDL(aDescription);
}


//============================================================================
RegionLocatorDescriptor::RegionLocatorDescriptor():
m_Interface(this),
m_isProprietary(false),
m_CoordFlag(0),
m_SpatialReference(0),
m_OneLocator(0),
m_CoordRef(0),
m_XRepresentation(0),m_YRepresentation(0),
m_B_P_Selector(0),
m_NoBox(0),m_NoPoly(0),
m_BoxData(0),m_PolyData(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
RegionLocatorDescriptor::~RegionLocatorDescriptor()
{
  int i;

  if(m_B_P_Selector&1){
    for(i=0;i<m_NoBox;i++)
	delete [] m_BoxData[i].Vertices;
    delete [] m_BoxData;
  }
  if(m_B_P_Selector&2){
    for(i=0;i<m_NoPoly;i++)
	delete [] m_PolyData[i].Vertices;
    delete [] m_PolyData;
  }

}

//----------------------------------------------------------------------------
RegionLocatorDescriptorInterfaceABC *RegionLocatorDescriptor::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
const char *RegionLocatorDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& RegionLocatorDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const UUID& RegionLocatorDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* RegionLocatorDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool RegionLocatorDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void RegionLocatorDescriptor::addref(void)

{
#ifdef VERBOSE
       fprintf(stderr,"RegionLocator connect\n");
#endif
       m_refcount++;
} 

//----------------------------------------------------------------------------
void RegionLocatorDescriptor::release(void)

{
#ifdef VERBOSE
       fprintf(stderr,"RegionLocator release\n");
#endif
       if (!(--m_refcount)) delete this;
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptor::GetCoordFlag(void)
{
  return m_CoordFlag;
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptor::SetCoordFlag(unsigned long c)
{
  if(c)
    m_CoordFlag=1;
  else
	m_CoordFlag=0;
  return 1;
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptor::GetSpatialReference(void)
{
  return m_SpatialReference;
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptor::SetSpatialReference(unsigned long f)
{
  if(f)
    m_SpatialReference=1;
  else
	m_SpatialReference=0;
  return 1;
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptor::GetOneLocator(void)
{
  return m_OneLocator;
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptor::SetOneLocator(unsigned long OneLocator)
{
  if(m_OneLocator)
    m_OneLocator=1;
  else
	m_OneLocator=0;
  return 1;
}

//----------------------------------------------------------------------------
char *RegionLocatorDescriptor::GetCoordRef(void)
{
  char *ret;
  ret= new char[strlen(m_CoordRef)+1];
  strcpy(ret,m_CoordRef);

  if(m_CoordFlag)
    return ret;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptor::SetCoordRef(char *r)
{
  if(r) {
        if(m_CoordRef)
	  delete m_CoordRef;
	m_CoordRef= new char[strlen(r)+1];
        strcpy(m_CoordRef,r);
        return 1;
        }
  else
        return 0;
}

//----------------------------------------------------------------------------
int RegionLocatorDescriptor::GetRepresentation(int component)
{
	if (component==0){
		m_CoordFlag=0;
		return m_XRepresentation;
	}

	if (component==1) {
		m_CoordFlag=0;
		return m_YRepresentation;
	}

  return -1;
}

//----------------------------------------------------------------------------
int RegionLocatorDescriptor::SetRepresentation(int XBits, int YBits)
{
  if(m_B_P_Selector==0){
    m_XRepresentation=XBits;
    m_YRepresentation=YBits;
    return 1;
  }
  return 0;
}
 
//----------------------------------------------------------------------------
void RegionLocatorDescriptor::ResetDescriptor()
{
  int i;

  if(m_B_P_Selector&1){
    for(i=0;i<m_NoBox;i++)
	delete [] m_BoxData[i].Vertices;
    delete [] m_BoxData;
    m_BoxData=0;
  }
  if(m_B_P_Selector&2){
    for(i=0;i<m_NoPoly;i++)
	delete [] m_PolyData[i].Vertices;
    delete [] m_PolyData;
    m_PolyData=0;
  }

  m_NoPoly=0;
  m_NoBox=0;
  m_B_P_Selector=0;
  }

//----------------------------------------------------------------------------
int RegionLocatorDescriptor::GetNumberOfPolygons(void)
{
  if(m_B_P_Selector&2)
    return m_NoPoly;
  return 0;
}

//----------------------------------------------------------------------------
int RegionLocatorDescriptor::GetNumberOfBoxes(void)
{
  if(m_B_P_Selector&1)
    return m_NoBox;
  return 0;
}

//----------------------------------------------------------------------------
int RegionLocatorDescriptor::SetInverseFlag(int B_P, int index, bool flag)
{
  if((m_B_P_Selector&1)&&(B_P==0)&&(index<m_NoBox)){
    m_BoxData[index].invers=flag;
    return 1;
  }
  if((m_B_P_Selector&2)&&(B_P==1)&&(index<m_NoPoly)){
    m_PolyData[index].invers=flag;
    return 1;
  }

  return 0;
}

//----------------------------------------------------------------------------
int RegionLocatorDescriptor::GetNumberOfVertices(int B_P, int index)
{
  if((m_B_P_Selector&1)&&(B_P==0)&&(index<m_NoBox)){
    return m_BoxData[index].NoVert;
  }
  if((m_B_P_Selector&2)&&(B_P==1)&&(index<m_NoPoly)){
    return m_PolyData[index].NoVert;
  }

  return 0;
}
//----------------------------------------------------------------------------
int RegionLocatorDescriptor::SetLocator(int B_P, bool invers, int NoV, unsigned int* vertices)
{

  int i;

  for(i=0;i<NoV;i+=2){
      if(vertices[i]>= (unsigned int) (1<<(m_XRepresentation+1))){
	//cout << i << " x " << vertices[i] << endl;
          return 0;
      }
      if(vertices[i+1]>= (unsigned int) (1<<(m_YRepresentation+1))){
	//cout << i << " y "<< vertices[i+1] << endl;
          return 0;
      }
  }
  
  if (B_P==0){
    if(NoV!=2 && NoV!=3)
      return 0;

    if(m_B_P_Selector&1){
      m_NoBox++;
      m_BoxData=(struct loc *)realloc(m_BoxData,m_NoBox*sizeof(struct loc));
    }
    else {
      m_B_P_Selector++;
      m_NoBox=1;
      m_BoxData=(struct loc *)malloc(sizeof(struct loc));
    }
    m_BoxData[m_NoBox-1].invers=invers;
    m_BoxData[m_NoBox-1].NoVert=NoV;
    m_BoxData[m_NoBox-1].Vertices=(unsigned long int *) malloc(2*NoV*sizeof(unsigned long int));

    for(i=0;i<(NoV*2);i++)
      m_BoxData[m_NoBox-1].Vertices[i]=vertices[i];

    return 1;
  }
    
  if (B_P==1){
    if(NoV<3)
      return 0;

    if(m_B_P_Selector&2){
      m_NoPoly++;
      m_PolyData=(struct loc *)realloc(m_PolyData,m_NoPoly*sizeof(struct loc));
    }
    else {
      m_B_P_Selector+=2;
      m_NoPoly=1;
      m_PolyData=(struct loc *)malloc(sizeof(struct loc));
    }
    m_PolyData[m_NoPoly-1].invers=invers;
    m_PolyData[m_NoPoly-1].NoVert=NoV;
    m_PolyData[m_NoPoly-1].Vertices=(unsigned long int *) malloc(2*NoV*sizeof(unsigned long int));

    for(i=0;i<(NoV*2);i++)
      m_PolyData[m_NoPoly-1].Vertices[i]=vertices[i];

    return 1;
  }
  
  return 0;
}

//----------------------------------------------------------------------------
struct loc RegionLocatorDescriptor::GetLocator(int B_P, int index)
{
  if((m_B_P_Selector&1)&&(B_P==0)&(index<m_NoBox)){
    return m_BoxData[index];
  }
  if((m_B_P_Selector&2)&&(B_P==1)&(index<m_NoPoly)){
    return m_PolyData[index];
  }

  // by default return PolyData
  // if well done, it never reaches here
  return  m_PolyData[index];
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptor::ExportDDL(GenericDSInterfaceABC *aParentDescription)
{
	static int p=0;
	unsigned long i;
	long z;
	unsigned short buf_lngth;
	GenericDS V, B, P, CO, CD, CS;
	struct loc locator;

#ifdef USEXML

	if (!aParentDescription) return (unsigned long) -1;
	/* Element name which contains the RegionLocator type can differ.*/
	/* Therefor the element is generated in the application*/
	/*VD = aParentDescription->CreateChild("Descriptor");
	if (VD.isNull())
	  return 1;
	  VD.SetAttribute("xsi:type","RegionLocatorType");*/
	if(GetCoordFlag()){				
	  CS = aParentDescription->CreateChild("CoordRef");
	  CS.SetAttribute("ref",GetCoordRef());
	  CS.SetAttribute("spatialRef",(GetSpatialReference()?"true":"false"));	
	}
	
	for (z=0; z<+GetNumberOfBoxes();z++){	
	
	
		
	
	  B = aParentDescription->CreateChild("Box");
	  locator = GetLocator(0,z);

	  B.SetAttribute("unlocatedRegion", (locator.invers?"true":"false"));

	  
	  // Build values list

	  vector<int> coords(2*locator.NoVert);
	  coords[0]=locator.Vertices[0];
	  coords[locator.NoVert]=locator.Vertices[1];
	  for (i = 1; i< locator.NoVert; i++)
	    coords[i]=locator.Vertices[2*i]-locator.Vertices[2*i-2];
	  for (i = 1; i< locator.NoVert; i++)
	    coords[i+locator.NoVert]=locator.Vertices[2*i+1]-locator.Vertices[2*i-1];
	  
	  char *buf;

	  B.SetValue(coords);

	  buf_lngth=(int)ceil(log10((double)locator.NoVert));
	  buf=(char *)malloc((3+buf_lngth)*sizeof(char));
	  sprintf(buf,"%ld 2", locator.NoVert);
	  B.SetAttribute("mpeg7:dim",buf);
	  free(buf);
	  
	  p++;
							
	}	



	for (z=0; z<GetNumberOfPolygons();z++){	
		
	  P = aParentDescription->CreateChild("Polygon");
	  locator = GetLocator(1,z);
	  P.SetAttribute("unlocatedRegion", (locator.invers?"true":"false"));
	  CO = P.CreateChild("Coords");
		
	  // Build values list

	  vector<int> coords(2*locator.NoVert);
	  coords[0]=locator.Vertices[0];
	  coords[locator.NoVert]=locator.Vertices[1];
	  for (i = 1; i< locator.NoVert; i++)
	    coords[i]=locator.Vertices[2*i]-locator.Vertices[2*i-2];
	  for (i = 1; i< locator.NoVert; i++)
	    coords[i+locator.NoVert]=locator.Vertices[2*i+1]-locator.Vertices[2*i-1];


	  CO.SetValue(coords);

	  char *buf;
	  buf_lngth=(int)ceil(log10((double)locator.NoVert));
	  buf=(char *)malloc((3+buf_lngth)*sizeof(char));
	  sprintf(buf,"%ld 2", locator.NoVert);
	  CO.SetAttribute("mpeg7:dim",buf);
	  free(buf);	
	  
	  p++;						
								
															
	

	}
#endif // USEXML

	return 0;
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorDescriptor::ImportDDL(GenericDSInterfaceABC *aDescription)
{

	int size,invers,dummy,i;
	int xmax=0, ymax=0, Xmax=0, Ymax=0, xrepr=0, yrepr=0;
	string vstr,name,name2;
	const string dim="mpeg7:dim", Poly="Polygon", Box="Box", uR="unlocatedRegion",CR="CoordRef",ref="ref",sR="spatialRef";
	int *vertices;

	#ifdef USEXML

	GenericDS C, V;
	
	if (!aDescription) return (unsigned long) -1;
	/* Element name which contains the RegionLocator type can differ.*/
	/* Therefor the element is queried in the application*/	
	/* if aDescriptor is of correct type*/
	/*if (aDescription->GetDSName() == "Descriptor") {
	  aDescription->GetTextAttribute("xsi:type",xsitype);
	  if (xsitype == "RegionLocatorType") {
	    l_DDLDescriptionInterface=aDescription;
	  }
	  }*/


	
	
	while(!((V = aDescription->GetChild((int)0)).isNull())){
	  if(V.GetDSName()==CR){
	    SetCoordFlag(1UL);
	    if(V.GetTextAttribute(ref,vstr)){
	      char *rstr=new char[vstr.size()+1];
	      vstr.copy(rstr,vstr.size());
	      SetCoordRef(rstr);
	    }
	    else{
	      fprintf(stderr,"RegionLocatorDescriptor::ImportDDL: ref attribute is required\n");
	    }
	    if(V.GetBoolAttribute(sR, (bool &)dummy))
	      SetSpatialReference(0UL);
	    else
	      SetSpatialReference((unsigned long) dummy);
	    
	    
	  }
	  if(V.GetDSName()==Box){
	    V.GetBoolAttribute(uR,(bool &)invers);

	    
	    V.GetTextAttribute(dim,vstr);
	    const char *cstr=vstr.c_str();
	    sscanf(cstr,"%d %d",&size,&dummy);
	    vertices= new int[size*2];
	    

	    V.GetTextValue(vstr);
	    char* cstr_v = new char[vstr.size() + 1];
	    vstr.copy(cstr_v, vstr.size());
	    cstr_v[vstr.size()] = 0;
	    
	    char *seps = " \t\n";
	    char *token = strtok(cstr_v, seps);

	    sscanf(token,"%d",&vertices[0]);
	    token = strtok( NULL, seps );	    

	    for(i=1;i<size;i++){

	      sscanf(token,"%d",&vertices[2*i]);
	      vertices[2*i]+=vertices[2*i-2];
	      token = strtok( NULL, seps );
	    }

	    sscanf(token,"%d",&vertices[1]);
	    token = strtok( NULL, seps );

	    for(i=1;i<size;i++){
	      sscanf(token,"%d",&vertices[2*i+1]);
	      vertices[2*i+1]+=vertices[2*i-1];
	      token = strtok( NULL, seps );
	    }
	    for(i=0;i<size;i++){
	      if(vertices[i*2]>xmax)
		xmax=vertices[i*2];
	      if(vertices[i*2+1]>ymax)
		ymax=vertices[i*2+1];
	    }
	    if (xmax>Xmax){
	      Xmax=xmax;
	      xrepr=0;
	      while(xmax){
		xmax>>=1;
		xrepr++;
	      }
	    }
	    if (ymax>Ymax){
	      Ymax=ymax;
	      yrepr=0;
	      while(ymax){
		ymax>>=1;
		yrepr++;
	      }
	    }
	    SetRepresentation(xrepr,yrepr);
	    SetLocator(0,(bool)invers,(int)size,(unsigned int *)vertices);	
	  }
	  if(V.GetDSName()==Poly){
	    V.GetBoolAttribute(uR,(bool &)invers);
	    
	    C=V.GetChild();
	    if (C.isNull())
	      return 1;
	    
	    C.GetTextAttribute(dim,vstr);
	    const char *cstr=vstr.c_str();
	    sscanf(cstr,"%d %d",&size,&dummy);
	    vertices= new int[size*2];
	    

	    C.GetTextValue(vstr);
	    char* cstr_v = new char[vstr.size() + 1];
	    vstr.copy(cstr_v, vstr.size());
	    cstr_v[vstr.size()] = 0;
	    
	    char *seps = " \t\n";
	    char *token = strtok(cstr_v, seps);

	    sscanf(token,"%d",&vertices[0]);
	    token = strtok( NULL, seps );	    

	    for(i=1;i<size;i++){

	      sscanf(token,"%d",&vertices[2*i]);
	      vertices[2*i]+=vertices[2*i-2];
	      token = strtok( NULL, seps );
	    }

	    sscanf(token,"%d",&vertices[1]);
	    token = strtok( NULL, seps );

	    for(i=1;i<size;i++){
	      sscanf(token,"%d",&vertices[2*i+1]);
	      vertices[2*i+1]+=vertices[2*i-1];
	      token = strtok( NULL, seps );
	    }

	    for(i=0;i<size;i++){
	      if(vertices[i*2]>xmax)
		xmax=vertices[i*2];
	      if(vertices[i*2+1]>ymax)
		ymax=vertices[i*2+1];
	    }
	    if (xmax>Xmax){
	      Xmax=xmax;
	      xrepr=0;
	      while(xmax){
		xmax>>=1;
		xrepr++;
	      }
	    }
	    if (ymax>Ymax){
	      Ymax=ymax;
	      yrepr=0;
	      while(ymax){
		ymax>>=1;
		yrepr++;
	      }
	    }
	    SetRepresentation(xrepr,yrepr);
	    SetLocator(1,(bool)invers,(int)size,(unsigned int *)vertices);		    
	  }
	  aDescription->RemoveChild(V);
	}
	#endif // USEXML


	return 0;

}



