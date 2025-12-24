/********************************************************************************
//	Program: To Decode a given AccessUnit file into the respective description file.
//	Author : Ali. J. Tabatabai
//  Company: (c) Sony Electronics, Inc.,2001.


This software module was originally developed by Sony Electronics Inc.
("Sony") in the course of development of the MPEG-7 Systems (ISO/IEC
15938-1) standard.

This software module is an implementation of a part of one or more
MPEG-7 Systems (ISO/IEC 15938-1) tools as specified by the MPEG-7 Systems
ISO/IEC 15938-1) standard.

ISO/IEC gives users of the MPEG-7 Systems (ISO/IEC 15938-1) free license to
this software module or modifications thereof solely for testing and
evaluation purposes for use in hardware or software products claiming
conformance to the MPEG-7 Systems (ISO/IEC 15938-1).

Those intending to use this software module in hardware or software products
are advised that its use may infringe existing patents.

The original developer of this software module and his/her company, the
Subsequent editors and their companies, and ISO/IEC have no liability for
use of this software module or modifications thereof in an implementation.
This software module is provided AS IS.

Copyright is not released for non MPEG-7 Systems (ISO/IEC 15938-1)
conforming products.

Sony retains full right to use the code for his/her own purpose, assign or
donate the code to a third party and to inhibit third parties from using the
code for non MPEG-7 Systems (ISO/IEC 15938-1) conforming products.

This copyright notice must be included in all copies or derivative works.


Copyright Sony © 2001.
****************************************************************************/



#ifndef AUHANDLER_HPP
#define AUHANDLER_HPP

#include  "StdAfx.h"
#include  <stdlib.h>
#include  <iostream>
#include  <assert.h>

#include <parsers/DOMParser.hpp>
#include <dom/DOM.hpp>
#include <framework/MemBufInputSource.hpp>

#include "util/XMLUniDefs.hpp"
#include "util/PlatformUtils.hpp"
#include "util/XMLException.hpp"
#include "util/XMLString.hpp"

#include  <sax2/DefaultHandler.hpp>
#include  <framework/XMLFormatter.hpp>

#include "xpathtype.hpp"

//STH moved to source file file
//static const char*  mID = "MPEG-7AU";//sth to the cpp file?? 

class AUHandler : public DefaultHandler, private XMLFormatTarget {
friend class XMLString;
public:


    AUHandler (
        const   char* const                 encodingName
        , const XMLFormatter::UnRepFlags    unRepFlags
		, const bool						expandNamespaces
    );
    ~AUHandler();

    void writeChars
    (
        const   XMLByte* const  toWrite
    );

    void writeChars
    (
        const   XMLByte* const  toWrite
        , const unsigned int    count
        , XMLFormatter* const   formatter
    );

    void endDocument();

    void endElement( const XMLCh* const uri, 
					 const XMLCh* const localname, 
					 const XMLCh* const qname);

    void characters(const XMLCh* const chars, const unsigned int length);

    void ignorableWhitespace
    (
        const   XMLCh* const    chars
        , const unsigned int    length
    );

    void processingInstruction
    (
        const   XMLCh* const    target
        , const XMLCh* const    data
    );

    void startDocument();

    void startElement(	const   XMLCh* const    uri,
						const   XMLCh* const    localname,
						const   XMLCh* const    qname,
					    const   Attributes&		attributes);


    void warning(const SAXParseException& exception);
    void error(const SAXParseException& exception);
    void fatalError(const SAXParseException& exception);

    void notationDecl
    (
        const   XMLCh* const    name
        , const XMLCh* const    publicId
        , const XMLCh* const    systemId
    );

    void unparsedEntityDecl
    (
        const   XMLCh* const    name
        , const XMLCh* const    publicId
        , const XMLCh* const    systemId
        , const XMLCh* const    notationName
    );

	DOM_Node& getDocument() { return targetDoc;}

	enum PARSER_STATE {
		navigation, command, payload, other
	};
	enum COMMAND_TYPE {
		add, del, rep, res, nop
	};

private :
  
    XMLFormatter    fFormatter;
	bool			fExpandNS;
	char 			orderFlg[5];
	unsigned int	sysProfLevel;
	bool			partialInstantiationFlg;

private:

	DOMParser*			parser;
	char*				payLoadBuffer;
	unsigned int		payLoadSize;
	MemBufInputSource*	memBufIS;
	PARSER_STATE		pState;
	COMMAND_TYPE		cmdType;
	bool				docFlg;
	XPathType*			xpath;

private:

	DOM_Document		targetDoc;
	DOM_Document		payLoadDoc;
	DOM_Node			targetNode;
	DOM_Element*		targetElement;
	DOM_Element			payLoadNode;

private:

	const char*  lNavigationElement;
	const char*  lUpdateCommandElement;
	const char*  lPayloadElement;
	const char*  lFragmentUpdateUnitElement;

	const char*	 lAddFragment;
	const char*	 lDeleteFragment;
	const char*	 lReplaceFragment;
	const char*	 lReset;
	
private:

	void setConstants();
	void initPayLoadBuffer();
};


#endif
