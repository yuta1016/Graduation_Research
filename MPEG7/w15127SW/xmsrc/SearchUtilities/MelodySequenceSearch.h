///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Thorsten Heinz
// Fraunhofer IIS
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
// Applicable File Name:  MelodySequenceSearch.h
//

#ifndef __MELODYSEQUENCESEARCH_H__
#define __MELODYSEQUENCESEARCH_H__

/* remove this, only for test purpose*/
#define __SearchAndRetrievalTypeApplication
//#define __ImageProcessingTypeApplication

#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class MelodySequenceSearchTool;

//=============================================================================
class MelodySequenceSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
  MelodySequenceSearchInterfaceABC();
  virtual ~MelodySequenceSearchInterfaceABC() {};

  virtual void destroy() = 0;

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aMelodySequenceDescriptionInterface) = 0;
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void) = 0;
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aMelodySequenceDescriptionInterface) = 0;

  virtual double GetDistance(void) = 0;

  /* sub-components do not have get and set functions because
     there is only one output value*/

  static const UUID myID;
};

//=============================================================================
class MelodySequenceSearchInterface: 
public MelodySequenceSearchInterfaceABC
{
public:
  MelodySequenceSearchInterface(MelodySequenceSearchTool* aTool);
  virtual ~MelodySequenceSearchInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy();

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aMelodySequenceDescriptionInterface);
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void);
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aMelodySequenceDescriptionInterface);

  virtual double GetDistance(void);

  static const UUID myID;
  static const char *myName;

private:
  MelodySequenceSearchTool *m_SearchTool;
};

//=============================================================================
class MelodySequenceSearchTool: public Search
{
friend class MelodySequenceSearchInterface;
public:
  MelodySequenceSearchTool();
  MelodySequenceSearchTool(GenericDSInterfaceABC
			*aQueryDescriptionInterface);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual MelodySequenceSearchInterfaceABC *GetInterface(void);

  virtual bool SupportsPush(void);
  virtual bool SupportsPull(void);

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aMelodySequenceDescriptionInterface);
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void);
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aMelodySequenceDescriptionInterface);

  virtual double GetDistance(void);

  static const UUID myID;
  static const char * myName;
private:
  virtual ~MelodySequenceSearchTool();

  MelodySequenceSearchInterface m_Interface;
  GenericDSInterfaceABC *m_RefDescriptionInterface;
  GenericDSInterfaceABC *m_QueryDescriptionInterface;


  //--- Descriptions
  GenericDS QueryMelodyType;
  GenericDS QueryMelodySequence;
  GenericDS QueryNoteArray;
  GenericDS QueryNote;
  GenericDS QueryInterval;

  GenericDS DBMelodyType;
  GenericDS DBMelodySequence;
  GenericDS DBNoteArray;
  GenericDS DBNote;
  GenericDS DBInterval;

  //--- Data
  unsigned int QueryNoNotes;
  vector<int>  QueryNoteValues;

  unsigned int DBNoNotes;
  vector<int>  DBNoteValues;


};

};

#endif //__MELODYSEQUENCESEARCH_H__
