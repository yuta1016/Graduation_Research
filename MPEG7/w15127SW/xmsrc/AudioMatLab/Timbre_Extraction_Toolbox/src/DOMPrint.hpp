#ifndef DOMPRINT_HPP_
#define DOMPRINT_HPP_ 1

// ---------------------------------------------------------------------------
/// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>
#include <util/XMLUniDefs.hpp>
#include <framework/XMLFormatter.hpp>
#include <util/TranscodingException.hpp>

#include <dom/DOM_DOMException.hpp>

#include <parsers/DOMParser.hpp>
#include <dom/DOM.hpp>
#include "DOMTreeErrorReporter.hpp"
#include <string.h>
#include <stdlib.h>

void usage(void);
void readTree(DOM_Node rootNode, DOM_Element rootElem, DOM_Document doc);
void printNodeType(DOM_Node child, DOM_Document doc, DOM_Element rootElem);
int DOMPrintInit(DOM_Node doc, XMLCh* EncodingName, XMLFormatter::UnRepFlags gUnRepFlags);
int DOMPrintToFile(DOM_Node doc, XMLCh* EncodingName, XMLFormatter::UnRepFlags gUnRepFlags, char *outFileName);
#endif // DOMPRINT_HPP_
