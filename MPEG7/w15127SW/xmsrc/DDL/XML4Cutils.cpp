///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// some of the MDS Group
// DOMString_to_LocalName by Wayne Price
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
// Applicable File Name:  XML4Cutil.cpp
//



#include "XML4Cutils.h"
#include <ctype.h>
#include <string>

// Microsoft Visual C++ requires this to use
// Standard Template Library Files

#if defined(USEXML3)

XERCES_CPP_NAMESPACE_USE

string DOMString_to_string(const XMLCh *DOMstr)
{
	char *tmp;
	tmp = XMLString::transcode(DOMstr);
	string s(tmp);
	XMLString::release(&tmp);
	return s;
}

string DOMString_to_LocalName(const XMLCh *DOMstr)
{
  // Create c++ string representation of the DOMstr

  char *tmp = XMLString::transcode(DOMstr);
  string domstr(tmp);
  XMLString::release(&tmp);

  // Check that
  // - the string length is greater than 3
  //   (must be otherwise it won't be a "qN:XXXXX" string)
  // - first character is 'q'
  // - second is a digit

  if ((domstr.length() > 3) && (domstr[0] == 'q') && isdigit(domstr[1]))
    {
      // Skip over any remaining digits

      int i = 2;
      while (isdigit(domstr[i]))
	i++;

      // Ensure that the next character is ':', and if so, erase the start
      // of the string

      if (domstr[i] == ':')
	domstr.erase(0, i+1);
    }

  return domstr;
}

XMLCh *string_to_DOMString(const string& inStr)
{
    // Use a buffer where possible and allocate memory only
    // if string is very large.
    
    char buffer[2000];
    char *cStr = buffer;
    
    if(inStr.size() + 1 > sizeof(buffer))
    {
	cStr = new char[inStr.size() + 1];
    }
    inStr.copy(cStr, inStr.size());
    cStr[inStr.size()] = '\0';
	XMLCh *domString = XMLString::transcode(cStr);
    if (cStr != buffer) delete [] cStr;
    return domString;
}

void release_DOMString(XMLCh **DOMstr)
{
	if(DOMstr) XMLString::release(DOMstr);
}


#elif defined(USEXML)
//------------------------------------------------------------------------
string DOMString_to_LocalName(DOMString DOMstr)
{
  // Create c++ string representation of the DOMstr

  char *tmp = DOMstr.transcode();
  string domstr(tmp);

//Bug here: Zhang Hui 2004/02/25.
//if I remove this delete operation, it seems the program runs correctly.
  //delete[] tmp;

  // Check that
  // - the string length is greater than 3
  //   (must be otherwise it won't be a "qN:XXXXX" string)
  // - first character is 'q'
  // - second is a digit

  if ((domstr.length() > 3) && (domstr[0] == 'q') && isdigit(domstr[1]))
    {
      // Skip over any remaining digits

      int i = 2;
      while (isdigit(domstr[i]))
	i++;

      // Ensure that the next character is ':', and if so, erase the start
      // of the string

      if (domstr[i] == ':')
	domstr.erase(0, i+1);
    }

  return domstr;
}

//------------------------------------------------------------------------
DOMString string_to_DOMString(const string& inStr) {
    // Use a buffer where possible and allocate memory only
    // if string is very large.
    
    char buffer[2000];
    char *cStr = buffer;
    
    if(inStr.size() + 1 > sizeof(buffer))
    {
	cStr = new char[inStr.size() + 1];
    }
    inStr.copy(cStr, inStr.size());
    cStr[inStr.size()] = '\0';
    DOMString domString = DOMString(cStr);
    if (cStr != buffer) delete [] cStr;
    return domString;
};
#endif // USEXML3
