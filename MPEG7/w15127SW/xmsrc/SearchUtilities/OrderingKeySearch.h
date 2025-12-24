///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Author: Alessandro Bugatti
// Company: University of Brescia
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
// Applicable File Name:  OrderingKeySearch.h
//

#ifndef __ORDERINGKEYSEARCH_H__
#define __ORDERINGKEYSEARCH_H__

#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class OrderingKeySearchTool;

//=============================================================================
class OrderingKeySearchInterfaceABC: public I_SearchInterfaceABC
{
public:
  OrderingKeySearchInterfaceABC();
  virtual ~OrderingKeySearchInterfaceABC() {};

  virtual void destroy()=0;

  virtual int SetRefDescriptorInterface
    (GenericDSInterfaceABC
     *aOrderingKeyDescriptorInterface) = 0;
  virtual GenericDSInterfaceABC*
    GetQueryDescriptorInterface(void) = 0;
  virtual int SetQueryDescriptorInterface
    (GenericDSInterfaceABC
     *aOrderingKeyDescriptorInterface) = 0;
  virtual double GetDistance(void) = 0;
  virtual int GetOrderedList(void) = 0; //sth creating the ordered list is
                                        // a task for the application

  /* sub-components do not have get and set functions because
     there is only one output value*/

  static const UUID myID;
};

//=============================================================================
class OrderingKeySearchInterface: 
public OrderingKeySearchInterfaceABC
{
public:
  OrderingKeySearchInterface(OrderingKeySearchTool* aTool);
  virtual ~OrderingKeySearchInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy();

  virtual int SetRefDescriptorInterface
    (GenericDSInterfaceABC
     *aOrderingKeyDescriptorInterface);
  virtual GenericDSInterfaceABC*
    GetQueryDescriptorInterface(void);
  virtual int SetQueryDescriptorInterface
    (GenericDSInterfaceABC
     *aOrderingKeyDescriptorInterface);
  virtual double GetDistance(void);
  virtual int GetOrderedList(void);

  static const UUID myID;
  static const char *myName;

private:
  OrderingKeySearchTool *m_SearchTool;
};

//=============================================================================
class OrderingKeySearchTool: public Search
{
friend class OrderingKeySearchInterface;
public:
  OrderingKeySearchTool();
  OrderingKeySearchTool(GenericDSInterfaceABC *aQueryDescriptorInterface);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual OrderingKeySearchInterfaceABC *GetInterface(void);

  virtual bool SupportsPush(void);
  virtual bool SupportsPull(void);

  virtual int SetRefDescriptorInterface
    (GenericDSInterfaceABC
     *aOrderingKeyDescriptorInterface);
  virtual GenericDSInterfaceABC*
    GetQueryDescriptorInterface(void);
  virtual int SetQueryDescriptorInterface
    (GenericDSInterfaceABC
     *aOrderingKeyDescriptorInterface);
  virtual double GetDistance(void);
  virtual int GetOrderedList(void);

  static const UUID myID;
  static const char * myName;
private:
  virtual ~OrderingKeySearchTool();

  OrderingKeySearchInterface m_Interface;
  GenericDSInterfaceABC *m_RefDescriptionInterface;
  GenericDSInterfaceABC *m_QueryDescriptionInterface;
  int ScanString(string*,char*);
  	
};


//=============================================================================
class elem_list
{
   string ID;
   string Value;
   elem_list *next_elem_list;
   public:
   elem_list();
   ~elem_list();
   void insertID(const char * /*string*/);
   void insertValue(const char * /*string*/);
   string getValue();
   void append(elem_list*);
   elem_list* next_Elem();
   void printBoth();
};

};

#endif //__ORDERINGKEYSEARCH_H__
