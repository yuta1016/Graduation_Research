/*****************************************************************************
//	Program: To Encode a given number of Fragment Units into an
//      Textual AccessUnit.
//	Author : Mohammed Zubair V.
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
****************************************************************************/



/*
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 1999-2000 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Xerces" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact apache\@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation, and was
 * originally based on software copyright (c) 1999, International
 * Business Machines, Inc., http://www.ibm.com .  For more information
 * on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>
#include <dom/DOM.hpp>

#include <sax/SAXException.hpp>
#include <sax/SAXParseException.hpp>
#include <parsers/DOMParser.hpp>
#include <dom/DOM_DOMException.hpp>
#include "DOMCount.hpp"
#include "TextualAccess_Encoder.hpp"
#include <framework/XMLFormatter.hpp>
#include <util/TranscodingException.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>

// ---------------------------------------------------------------------------
//  Definitions()
// ---------------------------------------------------------------------------

void writeDOM_Document_To_File(char *filename);

//static XMLCh*                   gEncodingName          = 0;
//static XMLFormatter*            gFormatter             = 0;
//static XMLFormatter::UnRepFlags gUnRepFlags            = XMLFormatter::UnRep_CharRef;

// ---------------------------------------------------------------------------
//  Usage()
// ---------------------------------------------------------------------------
void usage()
{
    cout << "\nUsage: TextualAccess_Encoder input.parfile\n\n"
            "This program invokes the Textual Access Encoder and builds the\n"
            "access unit tree.\n"
            "Options:\n"
            "    file: input.par file specifications \n"
          <<  endl;
}

// ---------------------------------------------------------------------------
//  main
// ---------------------------------------------------------------------------
int main(int argC, char* argV[])
{
    if (argC < 2){
			usage();
			return 1;
	}
	try
    {
			XMLPlatformUtils::Initialize();
	}
	catch(const XMLException& toCatch){
			char *pMsg = XMLString::transcode(toCatch.getMessage());
			cerr << "Error during Xerces-c Initialization.\n"
	             << "  Exception message:"
			     << pMsg;
			delete pMsg;
			return 1;
	}
	
	
	writeDOM_Document_To_File(argV[1]);
	
	// And call the termination method
    XMLPlatformUtils::Terminate();
	return 0;
}

void writeDOM_Document_To_File(char *filename)
{
	FILE *temp;
	::ifstream parfile;
	char navigation_path[2000];
	char dummy[500],temp_char[500];
	char navigation_command[20];
	char fragment_component_name[64];

	bool payloadFlag = false;
	char *accessFileName = (char *)calloc(64,sizeof(char));
	strcpy(navigation_path,"\0");
	strcpy(fragment_component_name,"\0");

	strcpy(accessFileName,"TeMAccessUnit");
	strcat(accessFileName,".xml");
	ofstream outfile(accessFileName);
	
	outfile << "<AUFile>" << endl; // Just to have one root node for implementation(not part of standard)

	parfile.open(filename);
	parfile.getline(dummy,255,'\n');//DecoderInit

	parfile.getline(dummy,255,'\n');//Reading SystemsProfileLevel
	if (!strcmp(dummy,"SysProfLevel") ){
		parfile.getline(dummy,255,'\n');//Reading SystemsProfileLevel
		outfile << "<DecoderInit SystemsProfileLevelIndication = \""<< dummy << "\">" << endl;
		parfile.getline(dummy,255,'\n');//SchemaReference
	}
	
	parfile.getline(dummy,255,'\n');//Reading attrName
	if (!strcmp(dummy,"attrName") ){
		parfile.getline(dummy,255,'\n');//Reading Name attribute
		strcpy(temp_char,dummy);
		parfile.getline(dummy,255,'\n');//Location Hint / InitialDesc
	}
	
	if (!strcmp(dummy,"attrlocationHint") ){
		parfile.getline(dummy,255,'\n');//Reading locationHint attribute
		outfile << "<SchemaReference name = \""<< temp_char << "\" locationHint = \"" << dummy << "\"/>" << endl;
		parfile.getline(dummy,255,'\n');//InitialDescription
	}
	else {
		outfile << "<SchemaReference name = \""<< temp_char << "\"/>" << endl;
	}
	
	outfile << "<InitialDescription>" << endl;
	outfile << "<FragmentUpdateUnit>" << endl;

	parfile.getline(navigation_command,20,'\n');
	outfile << "<FUCommand>" << endl << navigation_command << endl << "</FUCommand>" << endl;
	
	parfile.getline(navigation_path,1000,'\n');
	outfile << "<FUContext>" << endl << navigation_path << endl << "</FUContext>" << endl;
	strcpy(navigation_path,"\0");

	parfile.getline(dummy,255,'\n');
	
	if(!strcmp(dummy,"hasDeferredNodes")){
		outfile << "<FUPayload hasDeferredNodes=\"true\">" << endl;
		parfile.getline(dummy,255,'\n');
	}
	else
		outfile << "<FUPayload hasDeferredNodes=\"false\">" << endl;

	if(!strcmp(dummy,":beginPayload"))
		payloadFlag = true;
	else
		strcpy(fragment_component_name,dummy);

	if(!payloadFlag){
		temp = fopen(fragment_component_name,"r");
		strcpy(fragment_component_name,"\0");
		while( !feof(temp) ){
			int ch;
			if ((ch=fgetc(temp)) != EOF)
				outfile << (char)ch;
		}
	}
	else{
		strcpy(fragment_component_name,"\0");
		parfile.getline(dummy,255,'\n');
		do{
			outfile << dummy;
			parfile.getline( dummy, 255, '\n' );
		}while(strcmp(dummy,":endPayload"));
	}

	outfile << "</FUPayload>" << endl;
	payloadFlag = false;
	outfile << "</FragmentUpdateUnit>" << endl;
	outfile << "</InitialDescription>" << endl;
	outfile << "</DecoderInit>" << endl;

	parfile.getline(dummy,255,'\n');//Subsequent AccessUnits	
	while(!parfile.eof())
	{
		if( !strcmp(dummy,"AccessUnit") ) 
			outfile << "<AccessUnit>" << endl;
		else
			break;
		while(!parfile.eof())
		{
			parfile.getline(navigation_command,20,'\n');
			if(!strcmp(navigation_command,"AccessUnit") ){
				strcpy(navigation_command,"\0");
				strcpy(dummy,"AccessUnit");
				break;
			}
			if(!strcmp(navigation_path,"\0"))
				parfile.getline(navigation_path,1000,'\n');
			if(!strcmp(navigation_path,"AccessUnit") ){
				strcpy(navigation_path,"\0");
				strcpy(dummy,"AccessUnit");
				break;
			}

			outfile << "<FragmentUpdateUnit>" << endl;
		
			outfile << "<FUCommand>" << endl << navigation_command << endl << "</FUCommand>" << endl;
			outfile << "<FUContext>" << endl << navigation_path << endl << "</FUContext>" << endl;

			strcpy(navigation_path,"\0");
			if(parfile.eof()) {
				outfile << "</FragmentUpdateUnit>" << endl;
				break;
			}
			else if(!strcmp(navigation_command,"deleteNode")){
				outfile << "</FragmentUpdateUnit>" << endl;
				continue;
			}
			parfile.getline(dummy,255,'\n');
			
			if(!strcmp(dummy,"hasDeferredNodes")){
				outfile << "<FUPayload hasDeferredNodes=\"true\">" << endl;
				parfile.getline(dummy,255,'\n');
			}
			else
				outfile << "<FUPayload hasDeferredNodes=\"false\">" << endl;
				
			if(!strcmp(dummy,":beginPayload"))
				payloadFlag = true;
			else
				strcpy(fragment_component_name,dummy);

			if(strcmp(navigation_command,"deleteNode"))
			{
	
				if(!strcmp(fragment_component_name,"\0") && payloadFlag == false){
					parfile.getline(fragment_component_name,64,'\n');
					if(!strcmp(fragment_component_name,":beginPayload"))
						payloadFlag = true;
				}
					
				if(!payloadFlag){ // For a filefragment
					temp = fopen(fragment_component_name,"r");
					strcpy(fragment_component_name,"\0");
					while( !feof(temp) ){
						int ch;
						if ((ch=fgetc(temp)) != EOF)
						outfile << (char)ch;
					}
				}
				else{ //for a :begin :end fragment
					strcpy(fragment_component_name,"\0");
					parfile.getline(dummy,255,'\n');
					do{
						outfile << dummy;
						parfile.getline( dummy, 255, '\n' );
					}while(strcmp(dummy,":endPayload"));
				}

				outfile << "</FUPayload>" << endl;
				payloadFlag = false;
			}
			else{
				outfile << "<FUPayload>" << endl;
				outfile << "</FUPayload>" << endl;
			}
			outfile << "</FragmentUpdateUnit>" << endl;
	
		}
		outfile << "</AccessUnit>" << endl;
	}
	outfile << "</AUFile>" << endl; 
	outfile.close();
	parfile.close();
}

