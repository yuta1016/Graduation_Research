///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
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
// Applicable File Name:  XML4Cutil.h
//

#ifndef __DOM_utils_hh_
#define __DOM_utils_hh_

#if defined(USEXML) || defined(USEXML3)
#ifdef USEXML3
#include <xercesc/util/XMLString.hpp>
#else
#include <dom/DOMString.hpp>
#endif // USEXML3
#include <string>

//#ifdef WIN32
// Make the string datatype available from the STL std namespace.
using std::string;	
//#endif

#ifdef USEXML3
// Function to deal with XMLCh to string convertion
extern string DOMString_to_string(const XMLCh *ch);
extern string DOMString_to_LocalName(const XMLCh *DOMstr);
extern XMLCh *string_to_DOMString(const string& inStr);
extern void release_DOMString(XMLCh **DOMstr);
#else
// Macro and function to deal with DOMString to string convertion
#define DOMString_to_string(DOMstr) (string(DOMstr.transcode()))
extern string DOMString_to_LocalName(DOMString DOMstr);
extern DOMString string_to_DOMString(const string& inStr);
#endif // USEXML3
#endif // USEXML || USEXML3

#endif /* __DOM_utils_hh_ */
