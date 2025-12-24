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

#include "StdAfx.h"
#include <string.h>
#include <util/XMLUniDefs.hpp>
#include <sax2/Attributes.hpp>
#include "auwrite.hpp"
#include "auhandler.hpp"

#define INITSIZE 1000

//STH moved from header file
static const char*  mID = "MPEG-7AU";//sth to the cpp file?? 

static DOMParser::ValSchemes    gValScheme             = DOMParser::Val_Auto;

/*static const XMLCh  gEndElement[] = { chOpenAngle, chForwardSlash, chNull };
static const XMLCh  gEndPI[] = { chQuestion, chCloseAngle, chNull };
static const XMLCh  gStartPI[] = { chOpenAngle, chQuestion, chNull };
static const XMLCh  gXMLDecl1[] =
{
        chOpenAngle, chQuestion, chLatin_x, chLatin_m, chLatin_l
    ,   chSpace, chLatin_v, chLatin_e, chLatin_r, chLatin_s, chLatin_i
    ,   chLatin_o, chLatin_n, chEqual, chDoubleQuote, chDigit_1, chPeriod
    ,   chDigit_0, chDoubleQuote, chSpace, chLatin_e, chLatin_n, chLatin_c
    ,   chLatin_o, chLatin_d, chLatin_i, chLatin_n, chLatin_g, chEqual
    ,   chDoubleQuote, chNull
};

static const XMLCh  gXMLDecl2[] =
{
        chDoubleQuote, chQuestion, chCloseAngle
    ,   chCR, chLF, chNull
};*/

AUHandler::AUHandler( const   char* const encodingName,
                        const XMLFormatter::UnRepFlags unRepFlags,
						const bool expandNamespaces) :

    fFormatter
    (
        encodingName
        , this
        , XMLFormatter::NoEscapes
        , unRepFlags
    ),
	fExpandNS ( expandNamespaces ), parser(new DOMParser()),
        payLoadBuffer(0), payLoadSize(0), memBufIS(0) {

		setConstants();
		initPayLoadBuffer();
		pState = other;
		parser->setValidationScheme(gValScheme);
        parser->setDoSchema(true);
		parser->setDoNamespaces(true);
		docFlg = false;
}

AUHandler::~AUHandler() {

	delete parser;
}

void AUHandler::unparsedEntityDecl(const     XMLCh* const name
                                          , const   XMLCh* const publicId
                                          , const   XMLCh* const systemId
                                          , const   XMLCh* const notationName)
{
    // Not used at this time
}


void AUHandler::notationDecl(const   XMLCh* const name
                                    , const XMLCh* const publicId
                                    , const XMLCh* const systemId)
{
    // Not used at this time
}

void AUHandler::writeChars(const XMLByte* const toWrite) {
}

void AUHandler::writeChars(const XMLByte* const toWrite,
                                   const unsigned int count,
                                   XMLFormatter* const formatter) {
  int newLen = strlen(payLoadBuffer) + count;
  if(newLen >= payLoadSize) {
	  char* tempBuf = new char [1 + newLen];
	  assert(tempBuf != 0);
	  {
	    int i;
	    for(i = 0; payLoadBuffer[i] != '\0'; i++)
	      tempBuf[i] = payLoadBuffer[i];
	    tempBuf[i] = '\0';
	  }
	  delete [] payLoadBuffer;
	  payLoadSize = 1 + newLen;
	  payLoadBuffer = tempBuf;
  }
  int i = strlen(payLoadBuffer);
  for(int j = 0; toWrite[j] != '\0'; j++) payLoadBuffer[i++] = toWrite[j];
  payLoadBuffer[i] = '\0';
}


void AUHandler::error(const SAXParseException& e) {
	char* idMessage = XMLString::transcode(e.getSystemId());
	char* errMessage = XMLString::transcode(e.getMessage());
    cerr << "\nError at file " << idMessage
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << errMessage << endl;
	delete [] errMessage;
	delete [] idMessage;
}

void AUHandler::fatalError(const SAXParseException& e) {
	char* idMessage = XMLString::transcode(e.getSystemId());
	char* errMessage = XMLString::transcode(e.getMessage());
    cerr << "\nFatal Error at file " << idMessage
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << errMessage << endl;
	delete [] errMessage;
	delete [] idMessage;
}

void AUHandler::warning(const SAXParseException& e) {
	char* idMessage = XMLString::transcode(e.getSystemId());
	char* errMessage = XMLString::transcode(e.getMessage());
    cerr << "\nWarning at file " << idMessage
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << errMessage << endl;
	delete [] errMessage;
	delete [] idMessage;
}


void AUHandler::characters(const     XMLCh* const    chars
							, const   unsigned int    length) {
    XMLString::trim(const_cast<XMLCh*>(chars));
	char* c = XMLString::transcode(chars);
	switch(pState) {
	case navigation:
		xpath = new XPathType(c);
		xpath->getNavigationNodes();
		break;
	case command:
		if(!XMLString::compareString(lAddFragment, c)) cmdType = add;
		if(!XMLString::compareString(lDeleteFragment, c)) cmdType = del;
		if(!XMLString::compareString(lReplaceFragment, c)) cmdType = rep;
		if(!XMLString::compareString(lReset, c)) cmdType = res;
		break;
	default:fFormatter.formatBuf(chars, length, XMLFormatter::CharEscapes);
	}
	delete [] c;
}


void AUHandler::endDocument() {
}


void AUHandler::endElement(const XMLCh* const uri, 
								 const XMLCh* const localname, 
								 const XMLCh* const qname) {
	char* c = XMLString::transcode(localname);
	if(!XMLString::compareString(c, lPayloadElement)) {
		pState = other;
		return;
	}
	if(pState == payload) {
		fFormatter << XMLFormatter::NoEscapes << gEndElement ;
		if(fExpandNS ) {
			if(XMLString::compareIString(uri,XMLUni::fgEmptyString) != 0) fFormatter  << uri << chColon;
			fFormatter << localname << chCloseAngle;
		} else fFormatter << qname << chCloseAngle;		
		return;
	}
    if(!strcmp(c, lFragmentUpdateUnitElement)) {
		if(!xpath->isAttributeNode()) {
			try {
				memBufIS = new MemBufInputSource((const XMLByte*)(payLoadBuffer),  payLoadSize, mID, false);
				// Namespaces is set to false over here because here fragments are parsed and hence cannot be checked for namespace validity. 
				// Later when subsequent AU are decoded, namespace is set to true to check the validity.
				parser->setDoNamespaces(false);
				parser->parse(*memBufIS);
				if(!docFlg) {
					targetDoc = parser->getDocument();
					docFlg = true;
				}
			} catch (const XMLException& e) {
				char* errMessage = XMLString::transcode(e.getMessage()); 
				cerr << "\nError during parsing memory stream:\n"
				<< "Exception message is:  \n"
				<< errMessage << "\n" << endl;
				delete errMessage;
				return;
			}
			if(xpath->isAbsAddress()) targetNode = targetDoc.getDocumentElement();
			targetNode  = xpath->getTargetNode(targetNode);
		} else {
			if(xpath->isAbsAddress()) targetNode = targetDoc.getDocumentElement();
				targetNode  = xpath->getTargetNode(targetNode);
			(DOM_Node*&)targetElement = &targetNode;
		}
		DOM_Node node;
		switch(cmdType) {
		case add:
			if(!xpath->isRoot()) {
				if(!xpath->isAttributeNode()) {
					payLoadDoc = parser->getDocument();
					payLoadNode = payLoadDoc.getDocumentElement();
					node = targetDoc.importNode(payLoadNode, true);
					cout << "Current Node : " << node.getNodeName().transcode() << "\t" << " TargetNode :" << targetNode.getNodeName().transcode() << endl;
					targetNode.appendChild(node);
				} else {
					XMLString::trim(payLoadBuffer);
					targetElement->setAttribute(xpath->getAttributeName(), payLoadBuffer);
				}
			}
			break;
		case del:
			if(!xpath->isAttributeNode()) {
				node = targetNode.getParentNode();
				cout << "Before Delete tNode" << targetNode.getNodeName().transcode();
				node.removeChild(targetNode);
				targetNode = node;	//set the target node to parent
				cout << "Delete " << targetNode.getNodeName().transcode();
			} else {
				cout << targetNode.getNodeType();
				targetElement->removeAttribute(xpath->getAttributeName());
			}
			break;
		case rep:
			if(!xpath->isAttributeNode()) {
				payLoadDoc = parser->getDocument();
				payLoadNode = payLoadDoc.getDocumentElement();
				node = targetNode.getParentNode();
				node.replaceChild(targetDoc.importNode(payLoadNode, true), (DOM_Node)targetNode);
				targetNode = node;	//set the target node to parent
			} else {
				XMLString::trim(payLoadBuffer);
				targetElement->setAttribute(xpath->getAttributeName(), payLoadBuffer);
			}
			break;
		case res:
			// Reset Implementation resets the context node, using the navigation command.
			break;
		default:
			cerr << "NOT A VALID COMMAND: PLEASE CHECK THE SYNTAX" << endl;
		}
		delete memBufIS;
		memBufIS = 0;
		delete xpath;
		xpath = 0;
		initPayLoadBuffer();
		cmdType = nop;
	}
	pState = other;
	if( !strcmp(c,"AccessUnit") ){
		cout << " Writing the temp file " << endl;
		try{
			DOM_Node docNode = this->getDocument();
			DOMPrintFormatTarget* formatTarget = new DOMPrintFormatTarget("tempfile.xml");
			formatTarget->writeDomToFile(docNode);
			delete formatTarget;
			// Namespace set to true to check namespace validity
			parser->setDoNamespaces(true);
			parser->parse("tempfile.xml");
			targetDoc = parser->getDocument();
		}
		catch(XMLException& e){
			cerr << "An error occurred during creation of output transcoder. Msg is:"
				 << endl
				 << DOMString(e.getMessage()) << endl;
		}
		cout << "AccessUnit Ends" << endl;
	}
	delete [] c;
	return;
}


void AUHandler::ignorableWhitespace( const   XMLCh* const chars
                                            ,const  unsigned int length) {
    fFormatter.formatBuf(chars, length, XMLFormatter::NoEscapes);
}

void AUHandler::processingInstruction(const  XMLCh* const target
                                           , const XMLCh* const data) {

}


void AUHandler::startDocument() {
}


void AUHandler::startElement(const   XMLCh* const    uri,
									const   XMLCh* const    localname,
									const   XMLCh* const    qname,
                                    const   Attributes&		attributes) {
	char* c = XMLString::transcode(localname);

	if(!strcmp(c, "DecoderInit"))
			sysProfLevel = atoi( XMLString::transcode(attributes.getValue((unsigned int)0)) );
	//if(!strcmp(c, "AccessUnit"))
	//		strcpy(orderFlg,XMLString::transcode(attributes.getValue((unsigned int)0)));
	//if(!strcmp(c, "FUPayload"))
	//{
	//	if ( !strcmp("false", XMLString::transcode(attributes.getValue((unsigned int)0))) )
	//		partialInstantiationFlg = false;
	//	else
	//		partialInstantiationFlg = true;
	//}
	if(pState == payload) {
		fFormatter  << XMLFormatter::NoEscapes << chOpenAngle ;
		if ( fExpandNS ) {
			if (XMLString::compareIString(uri,XMLUni::fgEmptyString) != 0)
				fFormatter  << uri << chColon;
			fFormatter << localname ;
		}
		else
			fFormatter << qname ;

		unsigned int len = attributes.getLength();
			for (unsigned int index = 0; index < len; index++) {
			fFormatter  << XMLFormatter::NoEscapes << chSpace ;
			if ( fExpandNS ) {
					if (XMLString::compareIString(attributes.getURI(index),XMLUni::fgEmptyString) != 0)
						fFormatter  << attributes.getURI(index) << chColon;
				fFormatter  << attributes.getLocalName(index);
			}
			else
				fFormatter  << attributes.getQName(index) ;
			fFormatter  << chEqual << chDoubleQuote
                    << XMLFormatter::AttrEscapes
		            << attributes.getValue(index)
                    << XMLFormatter::NoEscapes
                    << chDoubleQuote;
			}
			fFormatter << chCloseAngle;
	}
	if(!strcmp(c, lNavigationElement)) pState = navigation;
    if(!strcmp(c, lUpdateCommandElement)) pState = command;
	if(!strcmp(c, lPayloadElement)) pState = payload;
	delete [] c;
}

void AUHandler::initPayLoadBuffer() {

	delete [] payLoadBuffer;
	payLoadSize = INITSIZE + 1;
	payLoadBuffer = new char [INITSIZE + 1];
	for(int i = 0; i < payLoadSize; i++) payLoadBuffer[i] = '\0';
	return;
}

void AUHandler::setConstants() {

	lNavigationElement = "FUContext";
	lUpdateCommandElement = "FUCommand";
	lPayloadElement = "FUPayload";
	lFragmentUpdateUnitElement = "FragmentUpdateUnit";
	
	lAddFragment = "addNode";
	lDeleteFragment = "deleteNode";
	lReplaceFragment = "replaceNode";
	lReset = "reset";

}
