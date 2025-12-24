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
// Applicable File Name:  RegionLocatorCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "string.h"
#include "Utilities/BitBuffer.h"
#include <sys/types.h>

#ifndef _VISUALC_
#include <netinet/in.h>
#endif


#ifndef ABS
#define  ABS(x)                (((x) < 0) ? -(x) : (x))
#endif


using namespace XM;

const UUID RegionLocatorCSInterface::myID = UUID("");
const char *RegionLocatorCSInterface::myName =
                         "Region Locator Coding Engine Interface";

const UUID RegionLocatorCS::myID = UUID("");
const char *RegionLocatorCS::myName = "Region Locator Coding Scheme";

const UUID RegionLocatorCS::valID = UUID("");
const char *RegionLocatorCS::valName = "";

const UUID RegionLocatorCSInterfaceABC::myID = UUID("");

RegionLocatorCSInterfaceABC::RegionLocatorCSInterfaceABC()

{
  SetInterfaceID(myID);
}

//=============================================================================
RegionLocatorCSInterface::RegionLocatorCSInterface(RegionLocatorCS *aRegionLocator):
m_RegionLocatorCS(aRegionLocator)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
RegionLocatorCSInterface::~RegionLocatorCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& RegionLocatorCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RegionLocatorCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void RegionLocatorCSInterface::destroy(void)
{ delete m_RegionLocatorCS; }

//----------------------------------------------------------------------------
BitBuffer *RegionLocatorCSInterface::GetEncoderStreamBuffer(void)
{
  return m_RegionLocatorCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int RegionLocatorCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_RegionLocatorCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *RegionLocatorCSInterface::GetDecoderStreamBuffer(void)
{
        return m_RegionLocatorCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int RegionLocatorCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_RegionLocatorCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
RegionLocatorDescriptorInterfaceABC * RegionLocatorCSInterface::
GetDescriptorInterface(void){
        return m_RegionLocatorCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int RegionLocatorCSInterface::
SetDescriptorInterface(RegionLocatorDescriptorInterfaceABC
		       *aRegionLocatorDescriptorInterface)
{
        return m_RegionLocatorCS->
	  SetDescriptorInterface(aRegionLocatorDescriptorInterface);
}

//----------------------------------------------------------------------------
int RegionLocatorCSInterface::SetEncoderFileName(char *filename)
{
        return m_RegionLocatorCS->SetEncoderFileName(filename);
}

//----------------------------------------------------------------------------
int RegionLocatorCSInterface::SetDecoderFileName(char *filename)
{
        return m_RegionLocatorCS->SetDecoderFileName(filename);
}

//----------------------------------------------------------------------------
int RegionLocatorCSInterface::PostEncode(void)
{
        return m_RegionLocatorCS->PostEncode();
}

//----------------------------------------------------------------------------
int RegionLocatorCSInterface::PreDecode(void)
{
        return m_RegionLocatorCS->PreDecode();
}


//----------------------------------------------------------------------------
int RegionLocatorCSInterface::StartEncode(void)
{
        return m_RegionLocatorCS->StartEncode();
}

//----------------------------------------------------------------------------
int RegionLocatorCSInterface::StartDecode(void)
{
        return m_RegionLocatorCS->StartDecode();
}

//=============================================================================
RegionLocatorCS::RegionLocatorCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL),
m_DSInterface(NULL),
m_DSCSInterface(NULL)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
RegionLocatorCS::RegionLocatorCS(RegionLocatorDescriptorInterfaceABC *regionLocator):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL),
m_DSInterface(NULL),
m_DSCSInterface(NULL)
{
	/* create descriptor if it does not exist*/
	if (!regionLocator) {
	  RegionLocatorDescriptor *descriptor =
	    new RegionLocatorDescriptor();
	  regionLocator=descriptor->GetInterface();
	}

	/* connect descritors with coding schemes*/
	SetDescriptorInterface(regionLocator);
        
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
RegionLocatorCS::~RegionLocatorCS()
{
	/* release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& RegionLocatorCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RegionLocatorCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& RegionLocatorCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* RegionLocatorCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool RegionLocatorCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * RegionLocatorCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int RegionLocatorCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * RegionLocatorCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int RegionLocatorCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
RegionLocatorDescriptorInterfaceABC * RegionLocatorCS::GetDescriptorInterface(void)
{
        return m_DescriptorInterface;
}
//----------------------------------------------------------------------------
RegionLocatorCSInterfaceABC * RegionLocatorCS::GetInterface(void){
    return &m_Interface;
}

//----------------------------------------------------------------------------
int RegionLocatorCS::
SetDescriptorInterface(RegionLocatorDescriptorInterfaceABC
	      *aRegionLocatorDescriptorInterface)
{
        if (!aRegionLocatorDescriptorInterface) return 0;
        if (aRegionLocatorDescriptorInterface == m_DescriptorInterface) return 0;

        if (m_DescriptorInterface) m_DescriptorInterface->release();

        m_DescriptorInterface = aRegionLocatorDescriptorInterface;
        if (m_DescriptorInterface) {
	  m_DescriptorInterface->addref();
	  return 0;
	}
	else 
	  return -1;

}

//----------------------------------------------------------------------------
unsigned short ld(unsigned long Pixellength)
{

	unsigned short Exponent = 0;

	while(Pixellength){
	      Pixellength>>=1;
	      Exponent++;
	}
return Exponent;
}

//----------------------------------------------------------------------------
int RegionLocatorCS::PostEncode(void)//sth whats that?
{
  return 0;
}


//----------------------------------------------------------------------------
int RegionLocatorCS::PreDecode(void)//sth whats that?JH: Descriptor contains two parts: a reference to a coordinate system and the descriptor itself. This way first the reference is decoded and proper action can be taken to decode the descriptor. In Principle this can also be applied to the encoder.
{
  m_DSize=0;
  long int coord_flag, spatialreference, XRepr, YRepr;

  m_DecoderBuffer->GetBitsLong(&coord_flag,1UL);
  m_DecoderBuffer->TakeBits(1UL);
  m_DescriptorInterface->SetCoordFlag(coord_flag);
  
  
  
  
  if (coord_flag!=0) {

    int esc=0;
    char *coordRef=new char[256];
    int charIndex=0;
    

    while(!esc){
      m_DecoderBuffer->GetBits(&coordRef[charIndex], 8UL);
      m_DecoderBuffer->TakeBits(8UL);
      if (coordRef[charIndex]=='\0')
	esc=1;
      charIndex++;
      if (charIndex>256)
	return 0;
    }
    m_DescriptorInterface->SetCoordRef(coordRef);
    
    m_DecoderBuffer->GetBitsLong(&spatialreference, 1UL);
    m_DecoderBuffer->TakeBits(1UL);
    m_DescriptorInterface->SetSpatialReference(spatialreference);		
    
    m_DSize+=(charIndex-1)*8+2;
    return 1;
  }
  else{
    m_DecoderBuffer->GetBitsLong(&XRepr,8UL);				
    m_DecoderBuffer->TakeBits(8UL);					
    m_DecoderBuffer->GetBitsLong(&YRepr,8UL);				
    m_DecoderBuffer->TakeBits(8UL);	
    
    m_DescriptorInterface->SetRepresentation(XRepr,YRepr);				
    
    m_DSize+=17;			
  }
  return m_DSize;
}

//----------------------------------------------------------------------------
int RegionLocatorCS::SetEncoderFileName(char *filename)
{
	// Create new root document
	m_DSDocument = GenericDS::CreateDescriptionRoot();
	m_DSInterface = m_DSDocument.GetInterface();

	// Create new CS
	GenericDSCS *theCS = new GenericDSCS;
	m_DSCSInterface = theCS->GetInterface();

	// Connect things
	if (m_DSInterface && m_DSCSInterface)
	{
		// Connect DS to CS
		m_DSCSInterface->SetDescriptionInterface(m_DSInterface);

		// Connect CS to file
		m_DSCSInterface->SetEncoderFileName(filename);
		return 0;
	}
	
	return 1;
}

//----------------------------------------------------------------------------
int RegionLocatorCS::SetDecoderFileName(char *filename)
{
	// Create new root document	
	m_DSInterface = m_DSDocument.GetInterface();

	// Create new CS
	GenericDSCS *theCS = new GenericDSCS;
	m_DSCSInterface = theCS->GetInterface();

	// Connect things
	if (m_DSInterface && m_DSCSInterface)
	{
		// Connect DS to CS
		m_DSCSInterface->SetDescriptionInterface(m_DSInterface);

		// Connect CS to file
		m_DSCSInterface->SetDecoderFileName(filename);

		return 0;
	}
	
	return 1;
}

//----------------------------------------------------------------------------
int RegionLocatorCS::StartEncode(void)
{

	unsigned short XRepr, YRepr, Use3P;							
        unsigned short NumOfBoxes, NumOfPolygons, ContainedLocatorTypes;		
	unsigned long i;
	long z;
	unsigned long  xrange, yrange, coord_flag, FirstVertexX, FirstVertexY, XDynamicRange, YDynamicRange;
        unsigned short xSrcSize, ySrcSize;
        unsigned short majorbits=0;
	unsigned short oct;
        unsigned long major, minor, spatialreference;
        unsigned short whichMajor;
	char *coordRef;
	long int *vert_diff;			
	struct loc locator;


	/* check if Descriptor and stream buffers are linked to coding scheme*/
	if (!m_EncoderBuffer || !m_DescriptorInterface) {
	  return -1;
	}


	NumOfBoxes=m_DescriptorInterface->GetNumberOfBoxes();
	NumOfPolygons=m_DescriptorInterface->GetNumberOfPolygons();
	if(NumOfPolygons==0 && NumOfBoxes==0)
	  return 0;

	coord_flag = m_DescriptorInterface->GetCoordFlag();
	m_EncoderBuffer->PutBitsLong(coord_flag,1UL);


	if (coord_flag==0) {

		XRepr=m_DescriptorInterface->GetRepresentation(0);
		m_EncoderBuffer->PutBitsLong(XRepr,8UL);

		YRepr=m_DescriptorInterface->GetRepresentation(1);
		m_EncoderBuffer->PutBitsLong(YRepr,8UL);
	}
	else{
 		coordRef=m_DescriptorInterface->GetCoordRef();
		m_EncoderBuffer->PutBits(coordRef, strlen(coordRef)+1);
		delete coordRef;

		spatialreference=m_DescriptorInterface->GetSpatialReference();
		m_EncoderBuffer->PutBitsLong(spatialreference, 1UL);
	}




	ContainedLocatorTypes=(2*(NumOfPolygons>0)+(NumOfBoxes>0));
	m_EncoderBuffer->PutBitsLong(ContainedLocatorTypes,2UL);

	if(ContainedLocatorTypes&1){

		NumOfBoxes=m_DescriptorInterface->GetNumberOfBoxes();

		if(getRangeExt(NumOfBoxes)<=8){

        		m_EncoderBuffer->PutBitsLong(0,1UL);
	    		m_EncoderBuffer->PutBitsLong(NumOfBoxes,8UL);
	    	}
    		else if(getRangeExt(NumOfBoxes)<=16){
        		m_EncoderBuffer->PutBitsLong(1,1UL);
	    		m_EncoderBuffer->PutBitsLong(0,1UL);
	    		m_EncoderBuffer->PutBitsLong(NumOfBoxes,16UL);
	    	}
    		else if(getRangeExt(NumOfBoxes)<=24){
        		m_EncoderBuffer->PutBitsLong(1,1UL);
	    		m_EncoderBuffer->PutBitsLong(1,1UL);
	    		m_EncoderBuffer->PutBitsLong(0,1UL);
	    		m_EncoderBuffer->PutBitsLong(NumOfBoxes,24UL);
	    	}
    		else if(getRangeExt(NumOfBoxes)<=32){
        		m_EncoderBuffer->PutBitsLong(1,1UL);
        		m_EncoderBuffer->PutBitsLong(1,1UL);
	    		m_EncoderBuffer->PutBitsLong(1,1UL);
	    		m_EncoderBuffer->PutBitsLong(0,1UL);
	    		m_EncoderBuffer->PutBitsLong(NumOfBoxes,32UL);
	    	}
	    	else return 0;

	  for(i=0;i<NumOfBoxes;i++){
	    locator=m_DescriptorInterface->GetLocator(0,i);
	    m_EncoderBuffer->PutBitsLong(locator.invers,1UL);
	    if(locator.NoVert==2){
	    	Use3P=0;
	      	m_EncoderBuffer->PutBitsLong(0UL,1UL);
	      }
	    else{
	      	Use3P=1;
	      	m_EncoderBuffer->PutBitsLong(1UL,1UL);
	      }

		// 2013/02/28 bug fix (KW) vert_diff pointer was used without memory allocation
		vert_diff = (long int *) calloc(2*(2+Use3P), sizeof(long int));

	    for(z=0; z<(2+Use3P); z++){

			
	      		vert_diff[2*z]=locator.Vertices[2*z+2]-locator.Vertices[2*z];
	      	
      			if(xrange < (unsigned long) ABS(vert_diff[2*z])){
				xrange = ABS(vert_diff[2*z]);
				xSrcSize = ld(xrange);
			}			
			else{
				xrange = 0;
			}
			
			
	      		vert_diff[2*z+1]=locator.Vertices[2*z+3]-locator.Vertices[2*z+1];
	      		
	      		if(yrange < (unsigned long) ABS(vert_diff[2*z+1])){
	      			yrange = ABS(vert_diff[2*z+1]);
	      			ySrcSize = ld(yrange);
	      		}
	      		else{
	      			yrange = 0;
	      		}
	    	}	      
			free(vert_diff);
	      
	      
	      
	      
	      
	    for(z=0; z<(2+Use3P); z++){								
	    	if (coord_flag!=0) {
	      		m_EncoderBuffer->PutBitsLong(locator.Vertices[2*z],(unsigned long)xSrcSize);				
	      		m_EncoderBuffer->PutBitsLong(locator.Vertices[2*z+1],(unsigned long)ySrcSize);		
	      	}
	      	else{
	      		m_EncoderBuffer->PutBitsLong(locator.Vertices[2*z],(unsigned long)XRepr);		
	      		m_EncoderBuffer->PutBitsLong(locator.Vertices[2*z+1],(unsigned long)YRepr);		
	      	}
	    }
	  }
	}




	if(ContainedLocatorTypes&2){

		NumOfPolygons=m_DescriptorInterface->GetNumberOfPolygons();



		if(getRangeExt(NumOfPolygons)<=8){

        		m_EncoderBuffer->PutBitsLong(0,1UL);
	    		m_EncoderBuffer->PutBitsLong(NumOfPolygons,8UL);
	    	}
    		else if(getRangeExt(NumOfPolygons)<=16){
        		m_EncoderBuffer->PutBitsLong(1,1UL);
	    		m_EncoderBuffer->PutBitsLong(0,1UL);
	    		m_EncoderBuffer->PutBitsLong(NumOfPolygons,16UL);
	    	}
    		else if(getRangeExt(NumOfPolygons)<=24){
        		m_EncoderBuffer->PutBitsLong(1,1UL);
	    		m_EncoderBuffer->PutBitsLong(1,1UL);
	    		m_EncoderBuffer->PutBitsLong(0,1UL);
	    		m_EncoderBuffer->PutBitsLong(NumOfPolygons,24UL);
	    	}
    		else if(getRangeExt(NumOfPolygons)<=32){
        		m_EncoderBuffer->PutBitsLong(1,1UL);
        		m_EncoderBuffer->PutBitsLong(1,1UL);
	    		m_EncoderBuffer->PutBitsLong(1,1UL);
	    		m_EncoderBuffer->PutBitsLong(0,1UL);
	    		m_EncoderBuffer->PutBitsLong(NumOfPolygons,32UL);
	    	}
	    	else return (0);


	  for(i=0;i<NumOfPolygons;i++){


	    	locator=m_DescriptorInterface->GetLocator(1,i);
	   	 m_EncoderBuffer->PutBitsLong(locator.invers,1UL);



	    	 if(getRangeExt(locator.NoVert)<=8){

        		m_EncoderBuffer->PutBitsLong(0,1UL);
	    		m_EncoderBuffer->PutBitsLong(locator.NoVert,8UL);
	    	}
    		else if(getRangeExt(locator.NoVert)<=16){
        		m_EncoderBuffer->PutBitsLong(1,1UL);
	    		m_EncoderBuffer->PutBitsLong(0,1UL);
	    		m_EncoderBuffer->PutBitsLong(locator.NoVert,16UL);
	    	}
    		else if(getRangeExt(locator.NoVert)<=24){
        		m_EncoderBuffer->PutBitsLong(1,1UL);
	    		m_EncoderBuffer->PutBitsLong(1,1UL);
	    		m_EncoderBuffer->PutBitsLong(0,1UL);
	    		m_EncoderBuffer->PutBitsLong(locator.NoVert,24UL);
	    	}
    		else if(getRangeExt(locator.NoVert)<=32){
        		m_EncoderBuffer->PutBitsLong(1,1UL);
        		m_EncoderBuffer->PutBitsLong(1,1UL);
	    		m_EncoderBuffer->PutBitsLong(1,1UL);
	    		m_EncoderBuffer->PutBitsLong(0,1UL);
	    		m_EncoderBuffer->PutBitsLong(locator.NoVert,32UL);
	    	}
	    	else return 0;

	    	FirstVertexX=locator.Vertices[0];
	    	FirstVertexY=locator.Vertices[1];


	    	if(coord_flag!=0){


	   		m_EncoderBuffer->PutBitsLong(locator.Vertices[0],(unsigned long)xSrcSize);

	    		m_EncoderBuffer->PutBitsLong(locator.Vertices[1],(unsigned long)ySrcSize);
		}
		else{
			m_EncoderBuffer->PutBitsLong(FirstVertexX,(unsigned long)XRepr);

	    		m_EncoderBuffer->PutBitsLong(FirstVertexY,(unsigned long)YRepr);
		}


	    	vert_diff=(long int *) calloc(2*(locator.NoVert-1), sizeof(long int));

	    	xrange = 0; yrange = 0;

	    	for(z=0;z<(long)(locator.NoVert - 1);z++){


	      		vert_diff[2*z]=locator.Vertices[2*z+2]-locator.Vertices[2*z];



	      		if(xrange < (unsigned long) ABS(vert_diff[2*z])){
				xrange = ABS(vert_diff[2*z]);
				XDynamicRange = ld(xrange);
			}
						
	      		vert_diff[2*z+1]=locator.Vertices[2*z+3]-locator.Vertices[2*z+1];
	      		
	      		
	      		
	      		if(yrange < (unsigned long) ABS(vert_diff[2*z+1])){
	      			yrange = ABS(vert_diff[2*z+1]);
	      			YDynamicRange = ld(yrange);
	      		}
	      		
		}
		
		m_EncoderBuffer->PutBitsLong(XDynamicRange,4UL);
		
		m_EncoderBuffer->PutBitsLong(YDynamicRange,4UL);


	    for(z=0;z<(long)(locator.NoVert - 1);z++){	    	    		    		    		    	
	    	
	    	

	      	oct=getOctant(vert_diff[2*z], vert_diff[2*z+1]);
	      	m_EncoderBuffer->PutBitsLong(oct,3UL);

	      	whichMajor=getMajorAndMinorComponents(vert_diff[2*z],vert_diff[2*z+1],oct,&major,&minor);

	      if(whichMajor!=0 && whichMajor!=1)
		  {
			  // 2013/02/28 - fix, free memory
			  free(vert_diff);
			return 0;
		  }

	      if(whichMajor){

			m_EncoderBuffer->PutBitsLong(major,XDynamicRange);
                	majorbits = 0;
			while(major){
		  		majorbits++;
		 		major>>=1;
			}
			m_EncoderBuffer->PutBitsLong(minor,(majorbits<YDynamicRange?majorbits:YDynamicRange));
	      }
	      else{
			m_EncoderBuffer->PutBitsLong(major,YDynamicRange);
                	majorbits = 0;
			while(major){
		  		majorbits++;
		  		major>>=1;
			}
			m_EncoderBuffer->PutBitsLong(minor,(majorbits<XDynamicRange?majorbits:XDynamicRange));
	      }

	    }
		free(vert_diff);
	  }

	}
	return 1;
}

//----------------------------------------------------------------------------
int RegionLocatorCS::StartDecode(void)
{

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_DescriptorInterface) {
	  return -1;
	}

	long XRepr, YRepr, NumOfBoxes, NumOfPolygons, NumOfVertices ,ContainedLocatorTypes;			
        long j, i, z;
        long XDynamicRange=0, YDynamicRange=0;
	long oct;
        long invers, Use3P=0;
        long buffer;
		//2013/03/03 - fix invalid pointer conversion from unsigned long* to unsigned int*
	//unsigned long *vertices;
	unsigned int *vertices;
	long vertDiff[2], NumOfBoxeslength, NumOfPolygonslength, NumOfVerticeslength;

	//2013/03/01 fix
	NumOfBoxeslength=0;
	
	
	/* PreDecode is required to call prior to decode!!	
	m_DecoderBuffer->GetBitsLong(&coord_flag,1UL);
	m_DecoderBuffer->TakeBits(1UL);
	m_DescriptorInterface->SetCoordFlag(coord_flag);




	if (coord_flag!=0) {
	        int esc=0;
		char *coordRef=new char[256];
		int charIndex=0;
	        while(!esc){
		  m_DecoderBuffer->GetBits(&coordRef[charIndex], 8UL);
		  m_DecoderBuffer->TakeBits(8UL);
		  if (coordRef[charIndex]=='\0')
		    esc=1;
		  charIndex++;
		  if (charIndex>256)
		    return 0;
		}
		m_DescriptorInterface->SetCoordRef(coordRef);
		  
		m_DecoderBuffer->GetBitsLong(&spatialreference, 1UL);
		m_DecoderBuffer->TakeBits(1UL);
		m_DescriptorInterface->SetSpatialReference(spatialreference);

		m_DSize+=(charIndex-1)*8+2;

	}
	else{
		m_DecoderBuffer->GetBitsLong(&XRepr,8UL);				
		m_DecoderBuffer->TakeBits(8UL);					
		m_DecoderBuffer->GetBitsLong(&YRepr,8UL);				
		m_DecoderBuffer->TakeBits(8UL);	
		
		m_DescriptorInterface->SetRepresentation(XRepr,YRepr);				
		
		m_DSize+=17;			
	}
	*/
	
	m_DecoderBuffer->GetBitsLong(&ContainedLocatorTypes,2UL);			
	m_DecoderBuffer->TakeBits(2UL);	
        
        m_DSize+=2;
      
	XRepr=m_DescriptorInterface->GetRepresentation(0);
	YRepr=m_DescriptorInterface->GetRepresentation(1); 	

        if(ContainedLocatorTypes&1){	
        	
        	j=0;
        	if(NumOfBoxeslength==0) NumOfBoxeslength=1;
        	while(NumOfBoxeslength){
        		m_DecoderBuffer->GetBitsLong(&NumOfBoxeslength,1UL);		
        		m_DecoderBuffer->TakeBits(1UL);        	        	
        		m_DSize+=1;
        		j++;
        	}	
        		j=j*8;
        		m_DecoderBuffer->GetBitsLong(&NumOfBoxes,(unsigned long)j);
	    		m_DecoderBuffer->TakeBits((unsigned long)j);
        		m_DSize+=j;
    	  	
				//2013/03/01 fix -allocate memory
				vertices=(unsigned int *) calloc(3*2, sizeof(unsigned int));
	  	
        		for(i=0;i<NumOfBoxes;i++){
          			m_DecoderBuffer->GetBitsLong(&invers,1UL);
	    			m_DecoderBuffer->TakeBits(1UL);
	    			m_DecoderBuffer->GetBitsLong(&Use3P,1UL);
	    			m_DecoderBuffer->TakeBits(1UL);
	      
	    			m_DSize += 2;

	   			for(z=0;z<3;z++){
	   			
	   				m_DecoderBuffer->GetBitsLong(&buffer,XRepr);					
	   				m_DecoderBuffer->TakeBits(XRepr);						
              				vertices[2*z]=(unsigned long)buffer;	
              			
	    				m_DecoderBuffer->GetBitsLong(&buffer,YRepr);					
	    				m_DecoderBuffer->TakeBits(YRepr); 						
             				vertices[2*z+1]=(unsigned long)buffer;	
             			
             				m_DSize += (XRepr + YRepr);
	    			}
	    										 
	    											    			
	    			m_DescriptorInterface->SetLocator(0,(bool)invers,(int)2+Use3P,(unsigned int *)vertices);
	    		}

	  	  free(vertices); 
	  }

	




	if(ContainedLocatorTypes&2){							
	  
	  
	  	j=0;
	  	if(NumOfPolygonslength==0) NumOfPolygonslength=1;
        	while(NumOfPolygonslength){
        		m_DecoderBuffer->GetBitsLong(&NumOfPolygonslength,1UL);			
        		m_DecoderBuffer->TakeBits(1UL);        	        	
        		m_DSize+=1;
        		j++;
        	}
        	j=j*8;
        	m_DecoderBuffer->GetBitsLong(&NumOfPolygons,(unsigned long)j);			
	    	m_DecoderBuffer->TakeBits((unsigned long)j); 					
        	m_DSize+=j;
	  
	  			
	 	for(i=0;i<NumOfPolygons;i++){		  
	  
	  		m_DecoderBuffer->GetBitsLong(&invers,1UL);				
	    		m_DecoderBuffer->TakeBits(1UL);
	  
	  	
	  		j=0;
	  		if(NumOfVerticeslength==0) NumOfVerticeslength=1;
        		while(NumOfVerticeslength){
        			m_DecoderBuffer->GetBitsLong(&NumOfVerticeslength,1UL);		
        			m_DecoderBuffer->TakeBits(1UL);        	        	
        			m_DSize+=1;
        			j++;
        		}
        		j=j*8;
        		m_DecoderBuffer->GetBitsLong(&NumOfVertices,(unsigned long)j);		
	    		m_DecoderBuffer->TakeBits((unsigned long)j); 				
        		m_DSize+=j;
	  	
				// 2013/03/01 fix
	    		//vertices=(unsigned long *) calloc(2*NumOfVertices, sizeof(long));
	    		vertices=(unsigned int *) calloc(2*NumOfVertices, sizeof(unsigned int));



			m_DecoderBuffer->GetBitsLong(&buffer,(unsigned long)XRepr);
			m_DecoderBuffer->TakeBits((unsigned long)XRepr);
			vertices[0]=buffer;
			m_DecoderBuffer->GetBitsLong(&buffer,(unsigned long)YRepr);
			m_DecoderBuffer->TakeBits((unsigned long)YRepr);
			vertices[1]=buffer;
			m_DSize += XRepr + YRepr;							
		
	    		m_DecoderBuffer->GetBitsLong(&XDynamicRange,4UL);
	    		m_DecoderBuffer->TakeBits(4UL);
	    		m_DecoderBuffer->GetBitsLong(&YDynamicRange,4UL);
	    		m_DecoderBuffer->TakeBits(4UL);	    
            		m_DSize += 8;
            
            		for(j = 1; j < NumOfVertices; j++){
            			
            			
                		m_DecoderBuffer->GetBitsLong(&oct,3UL);
                		m_DecoderBuffer->TakeBits(3UL);
                		m_DSize += 3;
                
	     
				
				m_DSize +=readComponents(vertDiff, oct , XDynamicRange, YDynamicRange);
           			vertices[2*j] = vertices[2*j - 2] + vertDiff[0];
           			vertices[2*j + 1] = vertices[2*j - 1] + vertDiff[1];				

			}
            		
		
	    	m_DescriptorInterface->SetLocator(1,(bool)invers,(int)NumOfVertices,(unsigned int *)vertices);

	    	free(vertices);
	    	
		}
	
	}
	return m_DSize;
}


//----------------------------------------------------------------------------
unsigned short RegionLocatorCS::getMajorAndMinorComponents( long x, long y, unsigned short octant, unsigned long *majorComponent, unsigned long *minorComponent)
{ 
    switch( octant ){
    case 0:
    case 1:
    case 2:
    case 3:
        *majorComponent = (unsigned long) ABS(x);
        *minorComponent = (unsigned long) ABS(y);
        return (1);
        
    case 4:
    case 5:
    case 6:
    case 7:
        *majorComponent = (unsigned long) ABS(y);
        *minorComponent = (unsigned long) ABS(x);
        return (0);
        
    default:
		std::cout << "getMajorAndMinorComponets: " <<
			octant << " is not a valid octant number.\n" << std::endl;
        
    }
    return(2);

}
//----------------------------------------------------------------------------
int RegionLocatorCS::readComponents(long *vertex, long octant, \
                                    long xDinRangeInd, \
                                    long yDinRangeInd){
    
    unsigned long int	readX, readY;
    long	bitsMinor;
    long	value[2];

    switch( octant ){
    case 0: 		/* x major; x,y positive */
        readX = xDinRangeInd ;
        m_DecoderBuffer->GetBitsLong( &value[0], readX );
        m_DecoderBuffer->TakeBits(readX);
        bitsMinor = getRangeExt(value[0]);
        readY = bitsMinor < yDinRangeInd ? bitsMinor : yDinRangeInd;
        m_DecoderBuffer->GetBitsLong( &value[1], readY );
	m_DecoderBuffer->TakeBits(readY);
        vertex[0] = value[0];
        vertex[1] = value[1];
        return ( readX + readY );
    case 4:		/* y major; x,y positive */
        readY = yDinRangeInd ;
        m_DecoderBuffer->GetBitsLong( &value[1], readY );
        m_DecoderBuffer->TakeBits(readY);
        bitsMinor = getRangeExt(value[1]);
        readX = bitsMinor < xDinRangeInd ? bitsMinor : xDinRangeInd;
        m_DecoderBuffer->GetBitsLong( &value[0], readX );
        m_DecoderBuffer->TakeBits(readX);
        vertex[0] = value[0];
        vertex[1] = value[1];
        return ( readX + readY );
    case 6:		/* y major; x negative, y positive */
        readY = yDinRangeInd;
        m_DecoderBuffer->GetBitsLong( &value[1], readY );
        m_DecoderBuffer->TakeBits(readY);
        bitsMinor = getRangeExt(value[1]);
        readX = bitsMinor < xDinRangeInd ? bitsMinor : xDinRangeInd;
        m_DecoderBuffer->GetBitsLong( &value[0], readX );
        m_DecoderBuffer->TakeBits(readX);
        vertex[0] = -value[0];
        vertex[1] = value[1];
        return ( readX + readY );
    case 1:		/* x major; x negative, y positive */
        readX = xDinRangeInd;
        m_DecoderBuffer->GetBitsLong( &value[0], readX );
        m_DecoderBuffer->TakeBits(readX);
        bitsMinor = getRangeExt(value[0]);
        readY = bitsMinor < yDinRangeInd ? bitsMinor : yDinRangeInd;
        m_DecoderBuffer->GetBitsLong( &value[1], readY );
        m_DecoderBuffer->TakeBits(readY);
        vertex[0] = -value[0];
        vertex[1] = value[1];
        return ( readX + readY );
    case 3:		/* x major; x negative, y negative */
        readX = xDinRangeInd;
        m_DecoderBuffer->GetBitsLong( &value[0], readX );
        m_DecoderBuffer->TakeBits(readX);
        bitsMinor = getRangeExt(value[0]);
        readY = bitsMinor < yDinRangeInd ? bitsMinor : yDinRangeInd;
        m_DecoderBuffer->GetBitsLong( &value[1], readY );
        m_DecoderBuffer->TakeBits(readY);
        vertex[0] = -value[0];
        vertex[1] = -value[1];
        return ( readX + readY );
    case 7:		/* y major; x negative, y negative */
        readY = yDinRangeInd;
        m_DecoderBuffer->GetBitsLong( &value[1], readY );
        m_DecoderBuffer->TakeBits(readY);
        bitsMinor = getRangeExt(value[1]);
        readX = bitsMinor < xDinRangeInd ? bitsMinor : xDinRangeInd;
        m_DecoderBuffer->GetBitsLong( &value[0], readX );
        m_DecoderBuffer->TakeBits(readX);
        vertex[0] = -value[0];
        vertex[1] = -value[1];
        return ( readX + readY );
    case 5:		/* y major; x positive, y negative */
        readY = yDinRangeInd;
        m_DecoderBuffer->GetBitsLong( &value[1], readY );
        m_DecoderBuffer->TakeBits(readY);
        bitsMinor = getRangeExt(value[1]);
        readX = bitsMinor < xDinRangeInd ? bitsMinor : xDinRangeInd;
        m_DecoderBuffer->GetBitsLong( &value[0], readX );
        m_DecoderBuffer->TakeBits(readX);
        vertex[0] = value[0];
        vertex[1] = -value[1];
        return ( readX + readY );
    case 2:		/* x major; x positive, y negative */
        readX = xDinRangeInd;
        m_DecoderBuffer->GetBitsLong( &value[0], readX );
        m_DecoderBuffer->TakeBits(readX);
        bitsMinor = getRangeExt(value[0]);
        readY = bitsMinor < yDinRangeInd ? bitsMinor : yDinRangeInd;
        m_DecoderBuffer->GetBitsLong( &value[1], readY );
        m_DecoderBuffer->TakeBits(readY);
        vertex[0] = value[0];
        vertex[1] = -value[1];
        return ( readX + readY );
    }
    return -1;
}


//----------------------------------------------------------------------------
unsigned short RegionLocatorCS::getOctant(long x, long y)
{
  //walks through the octants counter clock wise
    if( ((y >= 0) && (x > y)) || ( (x == 0) && (y == 0)) )
        return 0;
    else if( (y >= x) && (x > 0) )
        return 4;
    else if( (y > (-x)) && (x <= 0) )
        return 6;
    else if( (y <= (-x)) && (y > 0) )
        return 1;
    else if( (y <= 0) && (y > x) )
        return 3;
    else if( (x < 0) && (y <= x) )
        return 7;
    else if( (y < (-x)) && (x >= 0) )
        return 5;
    else if( (y >= (-x)) && (y < 0) )
        return 2;

    return(20);
}

//----------------------------------------------------------------------------
int RegionLocatorCS::getRangeExt(int x){

    if( x == 0)
        return (0);
    else if( x <= 1 )
        return (1);
    else if( x <= 3 )
        return (2);
    else if( x <= 7 )
        return (3);
    else if( x <= 15 )
        return (4);
    else if( x <= 31 )
        return (5);
    else if( x <= 63 )
        return (6);
    else if( x <= 127 )
        return (7);
    else if( x <= 255 )
        return (8);
    else if( x <= 511 )
        return (9);
    else if( x <= 1023 )
        return (10);
    else if( x <= 2047 )
        return (11);
    else if( x <= 4095 )
        return (12);
    else if( x <= 8191 )
        return (13);
    else if( x <= 16383 )
        return (14);
    else if( x <= 32767 )
        return (15);
    else if( x <= 65535 )
        return (16);

    fprintf(stderr, "getRange: Value %d out of range.\n", x);

    return (-1);
}
