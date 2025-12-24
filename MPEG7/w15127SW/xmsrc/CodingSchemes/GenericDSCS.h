///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Ana B. Benitez		- Columbia University.
// Javier Ruiz Hidalgo	- Universidad Politecnica de Catalunya.
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
// Applicable File Name:  GenericDSCS.h
#ifndef __GenericDSCS__
#define __GenericDSCS__

#if defined(USEXML3)
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
XERCES_CPP_NAMESPACE_USE
#elif defined(USEXML)
#include <util/PlatformUtils.hpp>
#include <parsers/DOMParser.hpp>
#include <dom/DOM_Node.hpp>
#include <dom/DOM_NamedNodeMap.hpp>
#endif // USEXML3

#include <string.h>
#include <stdlib.h>
#include "CodingSchemes/CodingSchemes.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
//#include "Utilities/BitBuffer.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef WIN32
#include <netinet/in.h>
#endif

//#include <IOstream.h>


namespace XM
{


//=============================================================================
class GenericDSCSInterfaceABC: public I_InterfaceABC
{
public:
	GenericDSCSInterfaceABC();
	virtual ~GenericDSCSInterfaceABC() {}

	static const UUID myID;

	virtual void destroy(void) = 0;

	virtual GenericDSInterfaceABC *GetDescriptionInterface(void) = 0;
	virtual int SetDescriptionInterface(GenericDSInterfaceABC
				    *aGenericDSInterface) = 0;

	virtual int SetEncoderFileName(char *filename) = 0;
	virtual int SetDecoderFileName(char *filename) = 0;

/* 	virtual ostream& GetXMLStreamO(void) =0; */
/* 	virtual ostream& GetXMLStreamO(void) =0; */
/* 	virtual int SetXMLStreamO(ostream& stream)=0; */
/* 	virtual istream& GetXMLStreamI(void)=0; */
/* 	virtual int SetXMLStreamI(istream& stream)=0; */
/* 	virtual char* GetXMLFileI(void)=0; */
/* 	virtual int SetXMLFileI(char *file)=0; */

	virtual int StartEncode() = 0;
	virtual int StartDecode() = 0;
};

class GenericDSCS;

//=============================================================================
class GenericDSCSInterface: public GenericDSCSInterfaceABC
{
public:
	GenericDSCSInterface(GenericDSCS *anCS);
	virtual ~GenericDSCSInterface();
	
	static const UUID myID;
	static const char *myName;

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy(void);

	virtual int SetEncoderFileName(char *filename);
	virtual int SetDecoderFileName(char *filename);

/* 	virtual ostream& GetXMLStreamO(void); */
/* 	virtual int SetXMLStreamO(ostream& stream); */
/* 	virtual istream& GetXMLStreamI(void); */
/* 	virtual int SetXMLStreamI(istream& stream); */
/* 	virtual char* GetXMLFileI(void); */
/* 	virtual int SetXMLFileI(char *file); */


	virtual GenericDSInterfaceABC *GetDescriptionInterface(void);
	virtual int SetDescriptionInterface(GenericDSInterfaceABC
				    *aGenericDSInterface);
	virtual int StartEncode();
	virtual int StartDecode();

private:
	GenericDSCS *m_GenericDSCS;	
};

class BitBuffer;
class GenericDS;

//=============================================================================
class GenericDSCS: public DescriptionCodingEngine
{
friend class GenericDSCSInterface;
public:
	GenericDSCS();
	GenericDSCS(GenericDSInterfaceABC *aDescription);

	// actual D or DS methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual GenericDSCSInterfaceABC *GetInterface(void);
	
	// accessor methods

	virtual int SetEncoderFileName(char *filename);
	virtual int SetDecoderFileName(char *filename);

/* 	virtual ostream& GetXMLStreamO(void); */
/* 	virtual int SetXMLStreamO(ostream& stream); */
/* 	virtual istream& GetXMLStreamI(void); */
/* 	virtual int SetXMLStreamI(istream& stream); */
/* 	virtual char* GetXMLFileI(void); */
/* 	virtual int SetXMLFileI(char *file); */

	virtual GenericDSInterfaceABC *GetDescriptionInterface(void);
	virtual int SetDescriptionInterface(GenericDSInterfaceABC 
					    *aGenericDescriptionInterface);
	virtual int StartEncode();
	virtual int StartDecode();

private:
	virtual ~GenericDSCS();

#if defined(USEXML3)
	int PrintDOMNode(std::ostream &out, const DOMNode& toWrite);
	void PrintDOMNode(std::ostream &out, const XMLCh *s);
	void outputContent(std::ostream &out, const XMLCh *toWrite,
			   bool doEscapes);
#elif defined(USEXML)
	int PrintDOMNode(std::ostream &out, DOM_Node& toWrite);
	void PrintDOMNode(std::ostream &out, const DOMString& s);
	void outputContent(std::ostream &out, const DOMString &toWrite,
			   bool doEscapes);
#endif // USEXML3

	GenericDSCSInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// descriptor data
	GenericDSInterfaceABC *m_DescriptionInterface;

	
	// The IBM DOM XML Parser do *not* support a stream as input for its
	// Parser class, as a result, we needed to specify the (char*) input
	// name as well. A more elegant solution should be found in the
	// possibly near future.
	
	char *m_XMLFileI;
	std::ostream *m_XMLStreamO;
/*	istream *m_XMLStreamI;*/
};

};

#endif //__GenericDSCS__
