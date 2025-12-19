//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Author : Bugatti Alessandro
// Company : University of Brescia
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  OrderingKeySearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include "Descriptors/Descriptors.h"
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
	

using namespace XM;


const UUID OrderingKeySearchInterface::myID = UUID("");
const char *OrderingKeySearchInterface::myName = "OrderingKeyMatchingInterface";

const UUID OrderingKeySearchTool::myID = UUID("");
const char *OrderingKeySearchTool::myName = "OrderingKeyMatchingTool";

const UUID OrderingKeySearchInterfaceABC::myID = UUID();

//=============================================================================
OrderingKeySearchInterfaceABC::OrderingKeySearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
OrderingKeySearchInterface::OrderingKeySearchInterface(OrderingKeySearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
OrderingKeySearchInterface::~OrderingKeySearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& OrderingKeySearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *OrderingKeySearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void OrderingKeySearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int OrderingKeySearchInterface::
SetRefDescriptorInterface(GenericDSInterfaceABC
		       *RefOrderingKeyDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefOrderingKeyDescriptorInterface);
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* OrderingKeySearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int OrderingKeySearchInterface::
SetQueryDescriptorInterface(GenericDSInterfaceABC
		       *QueryOrderingKeyDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryOrderingKeyDescriptorInterface);
}

//----------------------------------------------------------------------------
double OrderingKeySearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

int OrderingKeySearchInterface::GetOrderedList(void)

{
  return m_SearchTool->GetOrderedList();
}


//=============================================================================
OrderingKeySearchTool::OrderingKeySearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
  SetExposedInterface(&m_Interface);
}

//=============================================================================
OrderingKeySearchTool::
OrderingKeySearchTool(GenericDSInterfaceABC
		    *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{

  /* connect Descritors with Searchs*/
  SetQueryDescriptorInterface(aQueryDescriptorInterface);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
OrderingKeySearchTool::~OrderingKeySearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& OrderingKeySearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *OrderingKeySearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool OrderingKeySearchTool::SupportsPush(void)
{ return true; }//???????

//----------------------------------------------------------------------------
bool OrderingKeySearchTool::SupportsPull(void)
{ return false; }//??????

//----------------------------------------------------------------------------
int OrderingKeySearchTool::
SetRefDescriptorInterface(GenericDSInterfaceABC
		       *aRefOrderingKeyDescriptorInterface)

{
  m_RefDescriptionInterface = aRefOrderingKeyDescriptorInterface;
  return 0;
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* OrderingKeySearchTool::
GetQueryDescriptorInterface(void)

{
  return m_QueryDescriptionInterface;
}

//----------------------------------------------------------------------------
int  OrderingKeySearchTool::
SetQueryDescriptorInterface(GenericDSInterfaceABC
		       *aQueryOrderingKeyDescriptorInterface)

{
  m_QueryDescriptionInterface = aQueryOrderingKeyDescriptorInterface; 
  return 0;
}

//----------------------------------------------------------------------------
double OrderingKeySearchTool::GetDistance(void)
{
  return 0;
}

//sth use GetDistance for this
//----------------------------------------------------------------------------
int OrderingKeySearchTool::GetOrderedList(void)
{
   
   GenericDS dsUnit, dsOrdering, dsPath;
   string Selector;
   string Field;
   char temp[200];
   bool flagFind = false;
   GenericDS dsEntitiesSet, dsValueSet;
   elem_list *previous;
   elem_list *actual;
   elem_list *first;
   int num_elem_list = 0;
   string Value;
   //Variables for the ordering

    elem_list *SearchedPrevious;
    elem_list *RightPrevious;
    elem_list *Searched;
    elem_list *NextElement;
    elem_list *RightFirst;
    elem_list *PreviousFirst=NULL;
    string minimum;
    string comparison;
    double dMinimum;
    double dComparison;
    int i,j;

  // Check it's all set up ok
  if(m_RefDescriptionInterface==NULL) {
	  return 0;
  }
  if(m_QueryDescriptionInterface==NULL) {
	  return 0;
  }

  if(strcmp(m_RefDescriptionInterface->GetName(),
	  "GenericDSInterface") != 0) {
		return 0;
  }
  if(strcmp(m_QueryDescriptionInterface->GetName(),
	  "GenericDSInterface") != 0) { 
	  return 0;
  }

  dsUnit = m_QueryDescriptionInterface->GetFirstChild();
  if (dsUnit.isNull())
  {
	fprintf(stderr,"Null query\n");
		return 0;
	}
	if (dsUnit.GetDSName() != "Mpeg7Unit")
  {
      fprintf(stderr,"It is not a valid Mpeg7 unit\n");
		return 0;
	}
  dsOrdering=dsUnit.GetFirstChild();
	 if (dsOrdering.isNull())
  {
	fprintf(stderr,"Null query\n");
		return 0;
	}
	if (dsOrdering.GetDSName() != "OrderingKey")
  {
      fprintf(stderr,"Wrong DS name: OrderingKey expected\n");
		return 0;
	}
  dsPath = dsOrdering.GetFirstChild();
	//Read the selector
	for (;!dsPath.isNull();)
				{
		  if (dsPath.GetDSName() == "Selector")
			{
				dsPath.GetTextValue(Selector);
				flagFind = true;	
			      break;
			}
		  dsPath=dsPath.GetNextSibling();
      }
	if (flagFind == false)
		{
		fprintf(stderr,"Selector not found\n");
				return 0;
		}
	flagFind = false;
  
	//Read the field
  dsPath = dsOrdering.GetFirstChild();
	for (;!dsPath.isNull();)
				{
		  if (dsPath.GetDSName() == "Field")
			{
				dsPath.GetTextValue(Field);
				flagFind = true;	
			      break;
			}
        dsPath=dsPath.GetNextSibling();
      }
	if (flagFind == false)
		{
		fprintf(stderr,"Field not found\n");
				return 0;
		}
      flagFind=false;

//Check if the path of the field is contained inside the selector path
//In affermative case, change the content of the field removing the selector path
if (Field.find(Selector,0)==0) Field.replace(0,Selector.size(),"");

//Go to the node set inside the description where there are the entities that have to be 
//ordered
dsEntitiesSet = m_RefDescriptionInterface->GetFirstChild();
  if (dsEntitiesSet.isNull())
  {
	fprintf(stderr,"Null description\n");
		return 0;
	}
	if (dsEntitiesSet.GetDSName() != "Mpeg7Unit")
  {
      fprintf(stderr,"It is not a valid Mpeg7 unit\n");
		return 0;
	}
while (ScanString(&Selector,temp)==1)
  {
	dsEntitiesSet = dsEntitiesSet.GetFirstChild();
	for (;!dsEntitiesSet.isNull();)
				{
		  if (dsEntitiesSet.GetDSName() == temp)
			{
				flagFind = true;	
			      break;
			}
        dsEntitiesSet=dsEntitiesSet.GetNextSibling();
      }
	if (flagFind == false)
		{
		fprintf(stderr,"Selector not found\n");
				return 0;
		}
      dsEntitiesSet.GetFirstChild();
      flagFind = false;
  }
//For each element contained in the node set dsEntities it finds the field and 
//adds an element to a list
previous=NULL;
while	(!dsEntitiesSet.isNull())
   {
	dsValueSet = dsEntitiesSet;
	while (ScanString(&Field,temp)==1)
        {
	     dsValueSet = dsValueSet.GetFirstChild();
		    for (;!dsValueSet.isNull();)
				{
		               if (dsValueSet.GetDSName() == temp)
			              {
				            flagFind = true;	
			                  break;
			              }
        			   dsValueSet=dsValueSet.GetNextSibling();
      			}
			if (flagFind == false)
				{
					fprintf(stderr,"Field not found\n");
					return 0;
				}
      		flagFind = false;
        }
	actual = new elem_list;
//sth	actual->insertID(dsValueSet.GetDSName());
	actual->insertID(dsValueSet.GetDSName().c_str());//sth is this correct?
	dsValueSet.GetTextValue(Value);
	actual->insertValue(Value.c_str());
	if (previous!=NULL)
		{
			previous->append(actual);
			previous = actual;
		}
      else first = actual;
	num_elem_list++;
    }

//Ordering the list

    for (j=1;j<num_elem_list;j++)
    {
    minimum = first->getValue();
    actual=first->next_Elem();
    SearchedPrevious = first;
    RightPrevious = NULL;
    Searched=first;
    bool temporary;
    for(i=j;i<num_elem_list;i++)
    	{
            dMinimum = atof(minimum.c_str());
            if (dMinimum == 0)
            {
        	  if((actual->getValue()) < minimum)
            	{
                     Searched = actual;
                     RightPrevious=SearchedPrevious;
                     minimum = actual->getValue();
                }
            }
            else
            {
              comparison=actual->getValue();
              dComparison = atof(comparison.c_str());
              if(dComparison < dMinimum)
            	{
                     Searched = actual;
                     RightPrevious=SearchedPrevious;
                     minimum = actual->getValue();
                }
            }
            SearchedPrevious = actual;
            actual = actual->next_Elem();
        }
    if (RightPrevious != NULL)
    	{
        	RightPrevious->append(first);
            NextElement=first->next_Elem();
            first->append(Searched->next_Elem());
            Searched->append(NextElement);
        }
    if (j==1)
    {
    	RightFirst=Searched;
        PreviousFirst=Searched;
    }
    if (j!=1)
    {
    PreviousFirst->append(Searched);
    PreviousFirst=Searched;
	}
    first=Searched->next_Elem();
    }

//Print the ordered list

    actual=RightFirst;
    for (i=0; i<num_elem_list ; i++)
    {
        actual->printBoth();
        if (actual->next_Elem() != NULL ) actual=actual->next_Elem();
    }
    return 0;
}

//----------------------------------------------------------------------------
int OrderingKeySearchTool :: ScanString(string* str, char *result)
{
    char change[100];
    const char *p;
    p=str->c_str();
    int i = 1,j=0;
    if (*p != '/') return 0;
    p++;
	while ( (*p != '/') && (str->size())>i )
    	{
            result[i-1] = *p;
            i++;
            p++;
        }
    result[i-1]='\0';
    while ( i < str->size() )
    	{
        	change[j]= *p;
            i++;
            j++;
            p++;
        }
    change[j]='\0';
#ifdef _VISUALC_
    str->swap((string)change);
#else
    string tmpstr;
    tmpstr=change;
    str->swap(tmpstr);//sth is correct? I don`t understand the memory concept!!
#endif
    return 1;
}

//----------------------------------------------------------------------------
OrderingKeySearchInterfaceABC *OrderingKeySearchTool::GetInterface(void)
{ return &m_Interface; }

//sth maybe this functionality is already included in the MatchList
//----------------------------------------------------------------------------
elem_list :: elem_list()
{
	next_elem_list = NULL;
}

//----------------------------------------------------------------------------
elem_list :: ~elem_list()
{
}

//----------------------------------------------------------------------------
void elem_list :: insertID(const char *str)
{
    	ID = str;
}
    
//----------------------------------------------------------------------------
void elem_list :: insertValue(const char *str)
{
    	Value = str;
}

//----------------------------------------------------------------------------
string elem_list :: getValue()
{
     	return Value;
}

//----------------------------------------------------------------------------
void elem_list :: append (elem_list *p)
{
	next_elem_list=p;
}

//----------------------------------------------------------------------------
elem_list* elem_list :: next_Elem()
{
    	return next_elem_list;
}

//----------------------------------------------------------------------------
void elem_list :: printBoth()
{
    	printf("%s %s\n", ID.c_str(), Value.c_str());
}
