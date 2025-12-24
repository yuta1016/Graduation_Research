//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Patricia Chiang 	Siemens AG	12/05/2000
// Joerg Heuer          Siemens AG      30/06/2000
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
// Applicable File Name:  TimeSearch.cpp
//

#include <stdio.h>
#include <cstring>

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"


//=============================================================================

using namespace XM;


const UUID TimeSearchInterface::myID = UUID("");
const char *TimeSearchInterface::myName = "Time Matching Interface";

const UUID TimeSearchTool::myID = UUID("");
const char *TimeSearchTool::myName = "Time Matching Tool";
const int TimeSearchTool::STRING_MAXSIZE= 4096;

const UUID TimeSearchInterfaceABC::myID = UUID();

//sth please set the define to a correct value
#define DISTFACTOR 1.0

//=============================================================================
int TimePointDC::SetAttributes(int Y, int M, int D, int h, int m, int s, int nn, int nnFraction, int TZD)
{
	m_dateY = Y;
	m_dateM = M;
	m_dateD = D;
	m_time_h = h;
	m_time_m = m;
	m_time_s = s;
	m_time_nn = nn;
	m_time_nnFraction = nnFraction;
	m_timeTZD = TZD;
	return 0;
}

//=============================================================================
int TimeUnitDC::SetAttributes(int neg, int No_D, int h, int m, int s, int nn, int nnFraction, int TZD)
{
	m_negative = neg;
	m_time_D = No_D;
	m_time_h = h;
	m_time_m = m;
	m_time_s = s;
	m_time_nn = nn;
	m_time_nnFraction = nnFraction;
	m_timeTZD =TZD;
	return 0;
}

//=============================================================================
TimeSearchInterfaceABC::TimeSearchInterfaceABC()
{
	SetInterfaceID(myID);
}


//=============================================================================
TimeSearchInterface::TimeSearchInterface(TimeSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TimeSearchInterface::~TimeSearchInterface()
{
}

//----------------------------------------------------------------------------
void TimeSearchInterface::destroy(void)
{
  delete m_SearchTool;
}

//----------------------------------------------------------------------------
int TimeSearchInterface::
SetDescriptionInterface(GenericDSInterfaceABC
		       *GenericDSInterface)
{
  return m_SearchTool->
    SetDescriptionInterface(GenericDSInterface);
}

//sth delimiter??
int TimeSearchInterface::SetSourceQuery(FILE *fpQuery)
{
	return m_SearchTool->SetSourceQuery(fpQuery);
}

//----------------------------------------------------------------------------
double TimeSearchInterface::GetDistance(void)
{
  return m_SearchTool->GetDistance();
}

//----------------------------------------------------------------------------
int TimeSearchInterface::GetSegmentID(char *pcSegmentID)
{
  return m_SearchTool->GetSegmentID(pcSegmentID);
}

//----------------------------------------------------------------------------
const UUID& TimeSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeSearchInterface::GetName(void)
{ return myName; }

//=============================================================================
TimeSearchTool::TimeSearchTool():
  m_Interface(this)/*//sth ,
m_DescriptionInterface(0)*/
{
	SetExposedInterface(&m_Interface);
	m_distance = 1.0*DISTFACTOR;
	m_pRefTimePoint = NULL;
	m_pRefTimeUnit = NULL;
	m_pcSegmentID = new char[STRING_MAXSIZE];
}

//----------------------------------------------------------------------------
TimeSearchTool::TimeSearchTool(GenericDSInterfaceABC *theParent):
m_Interface(this)/*,//sth
m_DescriptionInterface(0)*/
{


        SetDescriptionInterface(theParent);

	SetExposedInterface(&m_Interface);
	m_distance = 1.0*DISTFACTOR;
	m_pRefTimePoint = NULL;
	m_pRefTimeUnit = NULL;
	m_pcSegmentID = new char[STRING_MAXSIZE];
}

//----------------------------------------------------------------------------
TimeSearchTool::~TimeSearchTool()
{
	delete [] m_pcSegmentID;
	if(m_pRefTimePoint)
	  delete m_pRefTimePoint;
	if(m_pRefTimeUnit)
	  delete m_pRefTimeUnit;
}

//----------------------------------------------------------------------------
const UUID& TimeSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool TimeSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool TimeSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int TimeSearchTool::SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
//sth compiler error  m_DescriptionInterface = aGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
int TimeSearchTool::SetSourceQuery(FILE *fpQuery)
{
	m_fpQuery = fpQuery;
	return ReadQueryFile();	
}

//----------------------------------------------------------------------------
double TimeSearchTool::GetDistance(void)
{
	// ------------------------------------------------------------------------
	// Variables declaration
	// ------------------------------------------------------------------------

	//DOM_Document domDoc;
	//DOM_Element domElem;

	// ------------------------------------------------------------------------
	// Check Descriptor schemes
	// ------------------------------------------------------------------------

  //sth compiler error	if (!m_DescriptionInterface) return m_distance;

//sth compiler error	if (strcmp(m_DescriptionInterface->GetName(),
//sth compiler error		   "GenericDSInterface") != 0) {
//sth	  return m_distance;
//sth	}
	
	// ------------------------------------------------------------------------
	// Perform matching
	// ------------------------------------------------------------------------

	GenericDS domElem = m_DSInterface->GetFirstChild();
	GenericDSInterfaceABC *pDomInterface = domElem.GetInterface();

	// Check the elements are segment relationship graphs
	if (pDomInterface->isNull()) {
		fprintf(stderr, "Document lacks child element\n");
		return m_distance;
	} 
	else {
		m_FlagAbsRel = RELATIVE;
		m_FlagRootNode = 1;
		m_FlagTimeUnit = 0;
		m_FlagnnFraction = 0;
		m_time_nnFraction = 1;
		strcpy(m_pcSegmentID,"no match");
		RecursiveTimeMatching(pDomInterface);	
	}

	//Release memory
	if(m_pRefTimePoint!=NULL)
		delete m_pRefTimePoint;
	if(m_pRefTimeUnit!=NULL)
		delete m_pRefTimeUnit;

	return m_distance;
}

//----------------------------------------------------------------------------
int TimeSearchTool::GetSegmentID(char *pcSegmentID)
{
	strcpy(pcSegmentID, m_pcSegmentID);
	return 0;
}

//----------------------------------------------------------------------------
int TimeSearchTool::ReadQueryFile()
{
	int iStartTime_s, iStartTime_nn, iDuration_s, iDuration_nn, innFraction;
	if(m_fpQuery == NULL){
		fprintf(stderr, "ReadQueryFile: No file pointer");
		return -1;
	}

	if(!(fscanf(m_fpQuery, "%d %d %d %d %d", &iStartTime_s, &iStartTime_nn, &iDuration_s, &iDuration_nn, &innFraction) == 5)){
		fprintf(stderr, "ReadQueryFile: Query file has less than expected number of elements\n");
		return -1;
	}
	if(innFraction<1){
		fprintf(stderr, "ReadQueryFile: illegal value of nn_Fraction\n");
		return -1;
	}
	if(!((iStartTime_nn<=0 && iStartTime_s<=0) || (iStartTime_nn>=0 && iStartTime_s>=0))){
		fprintf(stderr, "ReadQueryFile: sign for start time in fraction counts and seconds must be consistent\n");
		return -1;
	}
	if(!((iDuration_nn<=0 && iDuration_s<=0) || (iDuration_nn>=0 && iDuration_s>=0))){
		fprintf(stderr, "ReadQueryFile: sign for duration in fraction counts and seconds must be consistent\n");
		return -1;
	}
/*	if(iDuration_s==0 && iDuration_nn==0){
		fprintf(stderr, "ReadQueryFile: duration cannot be zero\n");
		return -1;
	}
*/	if(innFraction<=iDuration_nn || innFraction<=iStartTime_nn){
		fprintf(stderr, "ReadQueryFile: fraction counts must be less than total fractions\n");
		return -1;
	}
	m_Query_time_nnFraction = innFraction;
	m_QueryStartIncrTime = iStartTime_s*innFraction + iStartTime_nn;
	if(iDuration_s==0 && iDuration_nn==0)
		m_QueryIncrDuration = 1;
	else
		m_QueryIncrDuration = iDuration_s*innFraction + iDuration_nn;

	return 0;
	
}

//----------------------------------------------------------------------------
int TimeSearchTool::RecursiveTimeMatching(GenericDSInterfaceABC *pDomInterface)
{
	string domElemName = pDomInterface->GetDSName();
	
	if(!(domElemName=="VideoSegment" || domElemName=="MovingRegion" || domElemName=="StillRegion")){
			fprintf(stderr, "ERROR(SegmentDecomposition): Unexpected Segment Type found\n");
			return -1;
		}

	GenericDS timeNode = pDomInterface->GetFirstChild();
	GenericDSInterfaceABC *pTimeInterface = timeNode.GetInterface();
	
	if(!timeNode.isNull()){
		string nodeName = pTimeInterface->GetDSName();
		
		if(nodeName=="MediaTime"){
			
			int iStartIncrTime, iIncrDuration, SegmentQueryRelation;
			
			iStartIncrTime=0;
			iIncrDuration=1;
			SegmentQueryRelation = EXCLUSIVE;
			
			if(MediaTimeDOMDecode(pTimeInterface, &iStartIncrTime, &iIncrDuration)<0) return -1;
			SegmentQueryRelation = CalculateDistance(&iStartIncrTime, &iIncrDuration, pTimeInterface);
			
			switch(SegmentQueryRelation){
				case EXACT:
					return -1;
					break;
				case SUBSET:
					return 0;
					break;
				case SUPERSET:
					// continue
					break;
				case OVERLAP:
					//continue
					break; 
				case EXCLUSIVE:
					return 0;
					break;
			}
			if(m_FlagRootNode) m_FlagRootNode = 0;
			
			GenericDS segDecomNode = pTimeInterface->GetNextSibling();
			GenericDSInterfaceABC *pSegDecomInterface = segDecomNode.GetInterface();
			
			if(!pSegDecomInterface->isNull()){
				nodeName = pSegDecomInterface->GetDSName();
				if(nodeName=="SegmentDecomposition"){
					GenericDS childNode = pSegDecomInterface->GetFirstChild();
					GenericDSInterfaceABC *pChildInterface = childNode.GetInterface();
					
					while(!pChildInterface->isNull()){
						if(RecursiveTimeMatching(pChildInterface)<0) return -1;
						childNode = pChildInterface->GetNextSibling();
					}
					GenericDS tmpNode = pSegDecomInterface->GetNextSibling();
					GenericDSInterfaceABC *pTmpInterface = tmpNode.GetInterface();
					
					if(!pTmpInterface->isNull()){
						fprintf(stderr, "ERROR(Segment): Unexpected Element found\n");
					}
				}
				else{
					fprintf(stderr, "ERROR(Segment): Unexpected Element found "
					"(instead of SegmentDecomposition)\n");
					return -1;
				}
			}
		}
		else{
			fprintf(stderr, "ERROR(Segment): TimeDS not found\n");
			return -1;
		}
	}
	else{
		fprintf(stderr,"ERROR(Segment): TimeDS not found\n");
		return -1;
	}	
	return 0;
}

//----------------------------------------------------------------------------
int TimeSearchTool::CalculateDistance(int *piStartIncrTime, int *piIncrDuration, GenericDSInterfaceABC *pTimeInterface)
{
	static int factorRef = 1;
	static int firstTime = 1;
	int iStartIncrTime, iEndIncrTime, iQueryStartIncrTime, iQueryEndIncrTime;
	int SegmentQueryRelation = -1;
	double distance = 1.0*DISTFACTOR;
	
	if(m_Query_time_nnFraction != m_time_nnFraction && firstTime){
		int LCM = findLCM(m_Query_time_nnFraction, m_time_nnFraction);
		factorRef = LCM/m_time_nnFraction;
		int factorQuery = LCM/m_Query_time_nnFraction;
		m_QueryStartIncrTime = factorQuery*m_QueryStartIncrTime;
		m_QueryIncrDuration = factorQuery*m_QueryIncrDuration;
		m_Query_time_nnFraction = factorQuery*m_Query_time_nnFraction;
		firstTime = 0;
	}
	
	*piStartIncrTime = factorRef*(*piStartIncrTime);
	*piIncrDuration = factorRef*(*piIncrDuration);
	
	if(*piIncrDuration<0){
		iStartIncrTime = *piStartIncrTime + *piIncrDuration + 1;
		iEndIncrTime = *piStartIncrTime;
	}
	else{
		iStartIncrTime = *piStartIncrTime;
		iEndIncrTime = *piStartIncrTime + *piIncrDuration - 1;
	}
	
	if(m_QueryIncrDuration<0){
		iQueryStartIncrTime = m_QueryStartIncrTime + m_QueryIncrDuration + 1;
		iQueryEndIncrTime = m_QueryStartIncrTime;
	}
	else{
		iQueryStartIncrTime = m_QueryStartIncrTime;
		iQueryEndIncrTime = m_QueryStartIncrTime + m_QueryIncrDuration - 1;
	}
 	
	if(iStartIncrTime==iQueryStartIncrTime){
		if(iEndIncrTime==iQueryEndIncrTime)	SegmentQueryRelation=EXACT;
		else if(iEndIncrTime<iQueryEndIncrTime)	SegmentQueryRelation=SUBSET;
		else if(iEndIncrTime>iQueryEndIncrTime)	SegmentQueryRelation=SUPERSET;
	}
	else if(iStartIncrTime<iQueryStartIncrTime){
		if(iEndIncrTime<iQueryEndIncrTime){
			if(iEndIncrTime<iQueryStartIncrTime)	SegmentQueryRelation=EXCLUSIVE;
			else SegmentQueryRelation = OVERLAP;
		}
		else if(iEndIncrTime>=iQueryEndIncrTime)	SegmentQueryRelation=SUPERSET;
	}
	else if(iStartIncrTime>iQueryStartIncrTime){
		if(iEndIncrTime<=iQueryEndIncrTime)	SegmentQueryRelation=SUBSET;
		else if(iEndIncrTime>iQueryEndIncrTime){
			if(iStartIncrTime>iQueryEndIncrTime)	SegmentQueryRelation=EXCLUSIVE;
			else SegmentQueryRelation = OVERLAP;
		}
	
	}
	
	switch(SegmentQueryRelation){
		case EXACT: 
			distance = 0.0*DISTFACTOR;
			break;
		case SUPERSET:
			distance = DISTFACTOR*(abs(*piIncrDuration)-abs(m_QueryIncrDuration))/abs(*piIncrDuration);
			break;
		case SUBSET:
			distance = DISTFACTOR*(abs(m_QueryIncrDuration)-abs(*piIncrDuration))/abs(m_QueryIncrDuration);
			break;
		case OVERLAP:
			if(iStartIncrTime<iQueryStartIncrTime)
				distance = DISTFACTOR*(abs(iQueryEndIncrTime-iStartIncrTime)-abs(iEndIncrTime-iQueryStartIncrTime))
					/abs(iQueryEndIncrTime-iStartIncrTime);
			else
				distance = DISTFACTOR*(abs(iEndIncrTime-iQueryStartIncrTime)-abs(iQueryEndIncrTime-iStartIncrTime))
					/abs(iEndIncrTime-iQueryStartIncrTime);
			break;
		default:
			break;
	}
	
	if(distance<m_distance){
		 m_distance = distance;
	
		 string sDomID;
		 (pTimeInterface->GetParent()).GetInterface()->GetTextAttribute("id", sDomID);
		 
		 char str[STRING_MAXSIZE];
		 
		 if(sDomID==""){
		  	GenericDS parentElem = (pTimeInterface->GetParent()).GetInterface()->GetParent();
		  	GenericDSInterfaceABC *pParentInterface = parentElem.GetInterface();
		  	GenericDS parentElem1 = pParentInterface->GetParent();
		  	GenericDSInterfaceABC *pParentInterface1 = parentElem1.GetInterface();
		  	string sParentID1;
		  	pParentInterface1->GetTextAttribute("id", sParentID1);
		  	int length = pParentInterface->GetNumberOfChildren();
		  	int index=0;
		  	for(index=0;index<length/2 && pParentInterface->GetChild(index)!=pTimeInterface->GetParent(); ++index);  	
		  	sprintf(str, "%s/leafnode[%d]", sParentID1.c_str(), index);
		 }
		 else{
		 	GenericDS childNode = pTimeInterface->GetNextSibling();
		 	GenericDSInterfaceABC *pChildInterface = childNode.GetInterface();
		 	if(!pChildInterface->isNull()){
		 		if(pChildInterface->GetDSName()=="SegmentDecomposition"){
		 			string sGap;
		 			pChildInterface->GetTextAttribute("Gap", sGap);
		 			sprintf(str, "%s gap is %s", sDomID.c_str(), sGap.c_str());
		 		}
			}
		}
	 	strcpy(m_pcSegmentID, str);		 	
	}
	
	return SegmentQueryRelation;	
}

//----------------------------------------------------------------------------
int TimeSearchTool::MediaTimeDOMDecode(GenericDSInterfaceABC *pTimeInterface, int *piStartIncrTime, int *piIncrDuration)
{
	int flagError=0;
	
	GenericDS childNode = pTimeInterface->GetFirstChild();
	GenericDSInterfaceABC *pChildInterface = childNode.GetInterface();
	string childNodeName;
	
	if(!pChildInterface->isNull()){
		
		childNodeName = pChildInterface->GetDSName();
		
		if(childNodeName=="MediaTimePoint"){
			flagError = DOMDecodeTimePoint(pChildInterface, piStartIncrTime);
		}
		else if(childNodeName=="MediaRelTimePoint"){
			flagError = DOMDecodeRelTimePoint(pChildInterface, piStartIncrTime);
		} 
		else if(childNodeName=="MediaRelIncrTimePoint"){
			flagError = DOMDecodeRelIncrTimePoint(pChildInterface, piStartIncrTime);
		} 
		else{
			fprintf(stderr, "ERROR(MediaTimeDOMDecode) : Invalid Time Point Element\n");
			return -1;
		}
		if(flagError==-1) return -1;
	}
	else{
		fprintf(stderr, "ERROR(MediaTimeDOMDecode) : Invalid Time Point Element\n");
		return -1;
	}
	childNode = pChildInterface->GetNextSibling();
	pChildInterface = childNode.GetInterface();
	
	if(!pChildInterface->isNull()){
		
		childNodeName = pChildInterface->GetDSName();
		
		if(childNodeName=="MediaDuration"){
			flagError = DOMDecodeDuration(pChildInterface, piIncrDuration);
		}
		else if(childNodeName=="MediaIncrDuration"){
			flagError = DOMDecodeIncrDuration(pChildInterface, piIncrDuration);
		} 
		else{
			fprintf(stderr, "ERROR(MediaTimeDOMDecode) : Invalid Time Duration Element\n");
			return -1;
		}
		if(flagError==-1) return -1;
	}
	else{
		fprintf(stderr, "ERROR(MediaTimeDOMDecode) : Invalid Time Duration Element\n");
		return -1;
	}	
	return 0;
}

//----------------------------------------------------------------------------
int TimeSearchTool::DOMDecodeTimePoint(GenericDSInterfaceABC *pTimePointInterface, int *piStartIncrTime)
{
	
	int bTime, bDay, bMonth, bYear;
	int flagnnFraction;
	char cf[2]=" ",num[11]="0123456789",lexical[16]="0123456789TF:-+";
	
	char timepoint[256];
	string timepoint_str;
	char tc_element[256];
	char *p_timepoint;

	int ti_element[10];
	int i;
	size_t number_length;

	for(i=0;i<10;i++)
	  ti_element[i]=0;

	ti_element[9] = m_time_nnFraction;
	bTime=bDay=bMonth=bYear=0;
	flagnnFraction = 0;
	
	pTimePointInterface->GetTextValue(timepoint_str);
	strcpy(timepoint,timepoint_str.data());
	timepoint[strspn(timepoint,lexical)]='\000';
		 // due to malfunction of DOM_Node.GetValue

	p_timepoint=timepoint+strspn(timepoint,cf);
	i=0;

	while(strlen(p_timepoint) && i<10){
	  if(*p_timepoint=='F' || *p_timepoint=='T' || *p_timepoint==':' || *p_timepoint=='-' || *p_timepoint=='+'){
	    number_length=strspn(p_timepoint+1,num);
	    strncpy(tc_element,p_timepoint+1,number_length);
	  }
	  else{
	    number_length=strspn(p_timepoint,num);
	    strncpy(tc_element,p_timepoint,number_length);
	  }
	  tc_element[number_length]='\n';
	  ti_element[i]=(int)strtol(tc_element,NULL,0);
	  
	  switch(*(p_timepoint)){
	  case '-':
	    if(i!=0 && *(p_timepoint+number_length+1)=='-'){
	      //date value but not year
	      i++;
	      p_timepoint+=number_length+1;
	      break;
	    }
	    if(i!=0 && (*(p_timepoint+number_length+1)=='T' || *(p_timepoint+number_length+1)=='\n')){
	      //date value but not year
	      i=3;
	      p_timepoint+=number_length+1;
	      break;
	    }
	    if(*(p_timepoint+number_length+1)==':'){
	      //first component of the TZD value
	      if(i!=7){
		ti_element[7]=-ti_element[i];
		ti_element[i]=0;
	      }
	      else
		ti_element[i]*=-1;
	      i=8;
	      p_timepoint+=number_length+1;
	      break;
	    }
	    if(i==0 && *(p_timepoint+number_length+1)!=':'){
	      //Year value
	      ti_element[i]*=-1;
	      i++;
	      p_timepoint+=number_length+1;
	    }
	    break;

	  case 'F':
	    if(i!=9){
	      ti_element[9]=ti_element[i];
	      ti_element[i]=0;
	    }
	    i=10;
	    p_timepoint+=number_length+1;
	    // N was not counted!
	    flagnnFraction = 1;
	    break;

	  case 'T':
	    if(i!=3){
	      ti_element[3]=ti_element[i];
	      ti_element[i]=0;
	    }
	    i=4;
	    p_timepoint+=number_length+1;
	    // T was not counted!
	    break;
	    
	  case ':':
	    i++;
	    p_timepoint+=number_length+1;
	    // : was not counted!
	    break;
	    
	  default:
	    if(i==0){
	      p_timepoint+=number_length;
	      i++;
	    }
	    else {
	      fprintf(stderr,"DOMDecodeTimePoint: an invalid separator occurred!\n");
	      return -1;
	    }
	    break;
	}
	}
	
	ti_element[7]=ti_element[7]<0?ti_element[7]*100-ti_element[8]:ti_element[7]*100+ti_element[8];
	// TZD value expressed as 4 digit number
	     	
	if(m_FlagRootNode){
		*piStartIncrTime=0;
		m_FlagAbsRel = ABSOLUTE;
		if(!m_FlagnnFraction){
			m_FlagnnFraction = 1;
			m_time_nnFraction = ti_element[9];
		}
		if(flagnnFraction && m_time_nnFraction != ti_element[9]){
			fprintf(stderr, "DOMDecodeTimePoint: Base for fraction of second should be consistent in time description\n");
			return -1;
		}
		m_pRefTimePoint = new TimePointDC();
		m_pRefTimePoint->SetAttributes(ti_element[0],ti_element[1],ti_element[2],ti_element[3],ti_element[4],ti_element[5],ti_element[6],ti_element[7],ti_element[9]);
	}
	else{
		if(flagnnFraction && m_time_nnFraction != ti_element[9]){
			fprintf(stderr, "DOMDecodeTimePoint: Base for fraction of second should be consistent in time description\n");
			return -1;
		}
		if(m_FlagAbsRel == ABSOLUTE){
			struct tm *pRefTime, *pThisTime;
			pRefTime = (struct tm *)calloc(1, sizeof(struct tm));
			pRefTime->tm_year = m_pRefTimePoint->m_dateY-1900;
			pRefTime->tm_mon = m_pRefTimePoint->m_dateM-1;
			pRefTime->tm_mday = m_pRefTimePoint->m_dateD;
			pRefTime->tm_hour = m_pRefTimePoint->m_time_h;
			pRefTime->tm_min = m_pRefTimePoint->m_time_m;
			pRefTime->tm_sec = m_pRefTimePoint->m_time_s;

			pThisTime = (struct tm *)calloc(1, sizeof(struct tm));
			pThisTime->tm_year = ti_element[0]-1900;
			pThisTime->tm_mon = ti_element[1]-1;
			pThisTime->tm_mday = ti_element[2];
			pThisTime->tm_hour = ti_element[3];
			pThisTime->tm_min = ti_element[4];
			pThisTime->tm_sec = ti_element[5];
			
			*piStartIncrTime = (int)(difftime(mktime(pThisTime), mktime(pRefTime)));
			
			*piStartIncrTime = (*piStartIncrTime)*m_pRefTimePoint->m_time_nnFraction 
						+ ti_element[6] - m_pRefTimePoint->m_time_nn;
			free(pRefTime);
			free(pThisTime);
		}
		else{
			fprintf(stderr, "DOMDecodeTimePoint: No Time Point representations "
				"should be specified since root segment has a relative time "
				"representation\n");
		}
	}
	return 0;
}

//----------------------------------------------------------------------------
int TimeSearchTool::DOMDecodeRelTimePoint(GenericDSInterfaceABC *pRelTimeInterface, int *piStartIncrTime)
{
	string sTimeBase;
	pRelTimeInterface->GetTextAttribute("TimeBase", sTimeBase);
	
	if(m_FlagRootNode){
		*piStartIncrTime = 0;
		m_FlagAbsRel = RELATIVE;
	}
	else{
		if(sTimeBase!=""){
			fprintf(stderr, "DOMDecodeRelTime: No TimeBase should be specified "
			"as feature is not implemented wrt matching\n");
			return -1;
		}
		else
			DOMDecodeTimePeriod(pRelTimeInterface, piStartIncrTime);
	}
	
	return 0;
}

//----------------------------------------------------------------------------
int TimeSearchTool::DOMDecodeRelIncrTimePoint(GenericDSInterfaceABC *pRelIncrTimeInterface, int *piStartIncrTime)
{
	string sTimeBase;
	pRelIncrTimeInterface->GetTextAttribute("TimeBase", sTimeBase);
	
	if(m_FlagRootNode){
		*piStartIncrTime=0;
		m_FlagAbsRel = RELATIVE;
	}
	else{
		if(sTimeBase!=""){
			fprintf(stderr, "DOMDecodeRelIncrTime: No Time Base should be specified"
			"as feature is not implemented wrt matching\n");
			return -1;
		}
		else
			DOMDecodeIncrTime(pRelIncrTimeInterface, piStartIncrTime);
	}
	
	return 0;
}

//----------------------------------------------------------------------------
int TimeSearchTool::DOMDecodeDuration(GenericDSInterfaceABC *pDurationInterface, int *piIncrDuration)
{
	DOMDecodeTimePeriod(pDurationInterface, piIncrDuration);
	if(*piIncrDuration==0){
		/*fprintf(stderr, "DOMDecodeDuration: Duration must be more than zero\n");
		return -1;*/
		*piIncrDuration=1;
	}
	
	return 0;
}

//----------------------------------------------------------------------------
int TimeSearchTool::DOMDecodeIncrDuration(GenericDSInterfaceABC *pIncrDurationInterface, int *piIncrDuration)
{
	DOMDecodeIncrTime(pIncrDurationInterface, piIncrDuration);
	if(*piIncrDuration==0){
		*piIncrDuration=1;
		/*fprintf(stderr, "DOMDecodeIncrDuration: Duration must be more than zero\n");
		return -1;*/
	}
	
	return 0;
}

//----------------------------------------------------------------------------
int TimeSearchTool::DOMDecodeTimePeriod(GenericDSInterfaceABC *pTimePeriodInterface, int *piTimePeriod)
{
	int  iNegative;
	int bTime;
	int flagnnFraction;
	
	
	char timeperiod[256];

	string timePeriodName;
	string timeperiod_str;

	int ti_element[8];
	int i,period;
	for(i=0;i<8;i++)
	  ti_element[i]=0;

	iNegative=-1;
	ti_element[7]=m_time_nnFraction;
	bTime=0;
	flagnnFraction=0;
	*piTimePeriod=0;

	
	pTimePeriodInterface->GetTextValue(timeperiod_str);
	strcpy(timeperiod,timeperiod_str.data());
	
	bTime=DecodeDurationLexical(timeperiod,ti_element,&iNegative,&flagnnFraction);
	if(bTime==-1)
	  return -1;
	
	timePeriodName = pTimePeriodInterface->GetDSName();

	if(bTime==0){
		if(timePeriodName=="RelTime")
			fprintf(stderr, "DOMDecodeRelTime: Invalid Rel Time specified\n");
		else if(timePeriodName=="Duration")
			fprintf(stderr, "DOMDecodeDuration: Invalid Duration specified\n");
		return -1;
	}
	
	if(m_FlagRootNode){
		if(!m_FlagnnFraction){
			m_FlagnnFraction = 1;
			m_time_nnFraction = ti_element[7];
		}
	}
	
	if(flagnnFraction && m_time_nnFraction != ti_element[7]){
		fprintf(stderr, "DOMDecodeTimePeriod: Base for fraction of second should be consistent in time description\n");
		return -1;
	}
	
	period = (-1*iNegative)*(ti_element[4]+m_time_nnFraction*(ti_element[3]+60*(ti_element[2]+60*(ti_element[1]+ti_element[0]*24))));
	*piTimePeriod=period;
	return 0;

}

//----------------------------------------------------------------------------
int TimeSearchTool::DOMDecodeIncrTime(GenericDSInterfaceABC *pIncrTimeInterface, int *piIncrTime)
{
	int iNegative,  iIncrTime;
	int bTime;
	int flagTimeUnit, flagnnFraction;
	int iTimeUnit;
	int ti_element[8];
	int i;
	char timeunit[256];
	string timeunit_str;

	for(i=0;i<8;i++)
	  ti_element[i]=0;

	iNegative = -1;
	ti_element[7] = m_time_nnFraction;
	bTime=iIncrTime=0;
	flagTimeUnit=iTimeUnit=1;
	flagnnFraction=0;
	*piIncrTime=0;

	if(pIncrTimeInterface->GetTextAttribute("timeunit", timeunit_str)==-2)
	  flagTimeUnit = 0;
	else{
	  strcpy(timeunit,timeunit_str.data());
	  bTime=DecodeDurationLexical(timeunit,ti_element,&iNegative,&flagnnFraction);
	  if(bTime==-1)
	    return -1;
	}

	string incrTimeName;
	incrTimeName = pIncrTimeInterface->GetDSName();

	
	if(pIncrTimeInterface->GetIntValue(iIncrTime)<0){
		if(incrTimeName=="RelIncrTime")
			fprintf(stderr, "DOMDecodeTimeUnit: neg. RelIncrTime are not considdered\n");
		else if(incrTimeName=="IncrDuration")
			fprintf(stderr, "DOMDecodeTimeUnit: neg. IncrDuration are not considdered\n");
		return -1;
	}
	
	if(m_FlagRootNode){
		if(!m_FlagnnFraction){
			m_FlagnnFraction = 1;
			m_time_nnFraction = ti_element[7];
		}
	}
	
	if(flagnnFraction && m_time_nnFraction != ti_element[7]){
		fprintf(stderr, "DOMDecodeTimeUnit: Base for fraction of second should be consistent in time description\n");
		return -1;
	}
	
	if(flagTimeUnit){
		iTimeUnit = (-1*iNegative)*(ti_element[4]+m_time_nnFraction*(ti_element[3]+60*(ti_element[2]+60*(ti_element[1]+ti_element[0]*24))));

		if(iTimeUnit==0){
			fprintf(stderr, "DOMDecodeTimeUnit: TimeUnit must not be zero\n");
			return -1;
		}
		
		if(m_FlagRootNode && !m_FlagTimeUnit){
			m_FlagTimeUnit = 1;
			m_pRefTimeUnit = new TimeUnitDC();
			m_pRefTimeUnit->SetAttributes(iNegative, ti_element[0], ti_element[1], ti_element[2], ti_element[3], ti_element[4], ti_element[7], ti_element[5]);
		}
	}
	else{
		if(m_FlagTimeUnit){
			iTimeUnit = (-1*(m_pRefTimeUnit->m_negative))*(m_pRefTimeUnit->m_time_nn +m_pRefTimeUnit->m_time_nnFraction*(m_pRefTimeUnit->m_time_s +60*(m_pRefTimeUnit->m_time_m+60*(m_pRefTimeUnit->m_time_h+m_pRefTimeUnit->m_time_D*24))));
		}
		else{
			if(m_FlagRootNode)
				fprintf(stderr, "DOMDecodeTimeUnit: TimeUnit must be specified in the root node\n");
			else
				fprintf(stderr, "DOMDecodeTimeUnit: TimeUnit must be explicitly specified "
				"as root segment has no timeunit\n");
			return -1;
		}		
	}

	*piIncrTime = iIncrTime*iTimeUnit;
	return 0;
}
//----------------------------------------------------------------------------
int TimeSearchTool::DecodeDurationLexical(char *timeperiod, int *ti_element, int *neg, int *flagnnFraction)
{

        int bTime=0;

	char cf[2]=" ",num[11]="0123456789",lexical[22]="0123456789NZFDTHMS:+-";
	int i,separator;
	size_t number_length;
	char *p_timeperiod;
	char tc_element[256];

	p_timeperiod=timeperiod+strspn(timeperiod,cf);
	i=0;
	if(*p_timeperiod=='-'){
	  *neg=1;
	  p_timeperiod++;
	}
	if(*p_timeperiod!='P'){
	  fprintf(stderr,"DOMDecodeTimePeriod: invalid lexical!\n");
	  return -1;
	}
	p_timeperiod++;

	p_timeperiod[strspn(p_timeperiod,lexical)]='\000';
		 // due to malfunction of DOM_Node.GetValue

	while(strlen(p_timeperiod) && i<8){

	  if(*p_timeperiod=='T' || *p_timeperiod==':'){
	    separator=1;
	    number_length=strspn(p_timeperiod+1,num);
	    strncpy(tc_element,p_timeperiod+1,number_length);
	  }
	  else{
	    separator=0;
	    number_length=strspn(p_timeperiod,num);
	    strncpy(tc_element,p_timeperiod,number_length);
	  }
	  tc_element[number_length]='\n';
	  ti_element[i]=(int)strtol(tc_element,NULL,0);
	  
	  switch(*(p_timeperiod+number_length+separator)){
	  case 'D':
	    //number of days value
	    i++;
	    p_timeperiod+=number_length+separator+1;
	    break;

	  case 'H':
    	    //number of hours value
	    if(i!=1){
	      ti_element[1]=ti_element[i];
	      ti_element[i]=0;
	      i=2;
	    }
	    else{
	      i++;
	    }
	    p_timeperiod+=number_length+separator+1;
	    bTime++;
	    break;

	  case 'M':
	    //number of minutes value
	    if(i!=2){
	      ti_element[2]=ti_element[i];
	      ti_element[i]=0;
	      i=3;
	    }
	    else{
	      i++;
	    }
	    p_timeperiod+=number_length+separator+1;
	    bTime++;
	    break;

	  case 'S':  
	    //number of seconds value
	    if(i!=3){
	      ti_element[3]=ti_element[i];
	      ti_element[i]=0;
	      i=4;
	    }
	    else{
	      i++;
	    }
	    p_timeperiod+=number_length+separator+1;
	    bTime++;
	    break;

	  case 'N':	    
	    //number of fractions value
	    if(i!=4){
	      ti_element[4]=ti_element[i];
	      ti_element[i]=0;
	      i=5;
	    }
	    else{
	      i++;
	    }
	    p_timeperiod+=number_length+separator+1;
	    bTime++;
	    break;

	  case ':':	    
	    //first part of TZD value
	    if(i!=5){
	      ti_element[5]=ti_element[i];
	      ti_element[i]=0;
	      i=6;
	    }
	    else{
	      i++;
	    }
	    p_timeperiod+=number_length+separator+1;
	    break;

	  case 'Z':	    
	    //second part of TZD value
	    if(i!=6){
	      ti_element[6]=ti_element[i];
	      ti_element[i]=0;
	      i=7;
	    }
	    else{
	      i++;
	    }
	    p_timeperiod+=number_length+separator+1;
	    break;

	  case 'F':	    
	    //second part of TZD value
	    if(i!=7){
	      ti_element[7]=ti_element[i];
	      ti_element[i]=0;
	      i=8;
	    }
	    else{
	      i++;
	    }
	    p_timeperiod+=number_length+separator+1;
	    *flagnnFraction=1;
	    break;

	  default:
	    fprintf(stderr,"DOMDecodeTimePoint: an invalid separator occurred!\n");
	    return -1;
	    break;
	  }

	}
	
	ti_element[5]=ti_element[5]<0?ti_element[5]*100-ti_element[6]:ti_element[5]*100+ti_element[6];
	// TZD value expressed as 4 digit number

	return bTime;
}

//----------------------------------------------------------------------------
GenericDS TimeSearchTool::GetChild(GenericDSInterfaceABC *pDomInterface, const string& aName)
{
	GenericDS childNode = pDomInterface->GetFirstChild();
	GenericDSInterfaceABC *pChildInterface = childNode.GetInterface();
	string childName;
	
	while(!pChildInterface->isNull()){
		childName = pChildInterface->GetDSName();
		if(childName==aName)
			return childNode;
		childNode = pChildInterface->GetNextSibling();
		pChildInterface = childNode.GetInterface();
	}
	return childNode;
}

//----------------------------------------------------------------------------
int TimeSearchTool::findLCM(int	num1, int num2)
{
	int i;
	if(num1<num2){ i=num1; num1=num2; num2=i; }
	for(i=1; (num1*i)%num2!=0 && i<=num2; ++i);
	return (num1*i);
}

//----------------------------------------------------------------------------
TimeSearchInterfaceABC *TimeSearchTool::GetInterface(void)
{ return &m_Interface; }

