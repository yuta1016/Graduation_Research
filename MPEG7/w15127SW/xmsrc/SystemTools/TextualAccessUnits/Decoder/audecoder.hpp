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

#ifndef AUDECODER_HPP
#define AUDECODER_HPP

#include <util/TransService.hpp>
#include <sax2/SAX2XMLReader.hpp>
#ifdef INFINITY
#undef INFINITY
#endif
#include <sax2/XMLReaderFactory.hpp>

#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>
#include <util/XMLUniDefs.hpp>
#include <framework/XMLFormatter.hpp>
#include <util/TranscodingException.hpp>

#include "auhandler.hpp"


class AUDecoder {

public:

	AUDecoder();

	~AUDecoder() {
		delete parser;
		delete handler;
		XMLPlatformUtils::Terminate();
	}

	int decodeAU(char*);
	DOM_Node getDOM_DocumentNode() {return handler->getDocument();}
	void writeToFile(char *);

private:

	bool			doValidation;
    bool			doNamespaces;
	SAX2XMLReader*	parser;
	AUHandler*		handler;

private:

	const char*     encodingName;
	const XMLFormatter::UnRepFlags unRepFlags;
	const bool		expandNamespaces;
private:

	int initParser();
};


#endif
