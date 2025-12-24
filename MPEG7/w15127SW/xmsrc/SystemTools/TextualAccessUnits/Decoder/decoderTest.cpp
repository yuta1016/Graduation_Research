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
 */


#include "StdAfx.h"
#include "audecoder.hpp"

// ---------------------------------------------------------------------------
//  Usage()
// ---------------------------------------------------------------------------
void usage()
{
    cout << "\nUsage: decoderTest <encoded_file> <output_file>\n"
            "This program invokes the Textual Access Decoder and builds the\n"
            "description.\n"
          <<  endl;
}


int main(int argc, char* argv[]) 
{
	if (argc < 3){
			usage();
			return 1;
		}
	AUDecoder dec;
	dec.decodeAU(argv[1]);
	dec.writeToFile(argv[2]);
	return 0;

}
