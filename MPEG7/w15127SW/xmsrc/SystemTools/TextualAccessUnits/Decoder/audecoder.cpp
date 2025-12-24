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
#include <stdio.h>
#include "auwrite.hpp"
#include "audecoder.hpp"


AUDecoder::AUDecoder(): doValidation(false), doNamespaces(false), handler(0), encodingName("LATIN1"),
		   unRepFlags(XMLFormatter::UnRep_CharRef), expandNamespaces(false) {
	initParser();
	handler = new AUHandler(encodingName, unRepFlags, expandNamespaces);
	assert(handler);
	parser = XMLReaderFactory::createXMLReader();
	parser->setFeature(XMLString::transcode("http://xml.org/sax/features/validation"), true);
	parser->setFeature(XMLString::transcode("http://apache.org/xml/features/validation/dynamic"), true);
	parser->setFeature(XMLString::transcode("http://xml.org/sax/features/namespaces"), false);	
	parser->setFeature(XMLString::transcode("http://apache.org/xml/features/validation/schema"), false);	
}

int AUDecoder::decodeAU(char* xmlInFile) {

	try {
        parser->setContentHandler(handler);
        parser->setErrorHandler(handler);
        parser->parse(xmlInFile);
	} catch(XMLException& e)  {
		char *eMessage = XMLString::transcode(e.getMessage());
		cerr << "An error occurred during creation of output transcoder: " << eMessage << endl;
		delete [] eMessage;
		XMLPlatformUtils::Terminate();
		return -1;
	}
	return 0;
}

int AUDecoder::initParser() {
	
	try {

		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch) {
        char *eMessage = XMLString::transcode(toCatch.getMessage());
        fprintf(stderr, "Error during XMLPlatformUtils::Initialize(). \n"
                        "  Message is: %s\n", eMessage);
        delete [] eMessage;
		XMLPlatformUtils::Terminate();
        return -1;
    }
	return 0;
}

void AUDecoder::writeToFile(char *filename)
{
	try
	{
		DOM_Node docNode = this->getDOM_DocumentNode();
		DOMPrintFormatTarget* formatTarget = new DOMPrintFormatTarget(filename);
		formatTarget->writeDomToFile(docNode);
		delete formatTarget;
	}
	catch(XMLException& e)
	{
		cerr << "An error occurred during creation of output transcoder. Msg is:"
             << endl
             << DOMString(e.getMessage()) << endl;
	}
}

