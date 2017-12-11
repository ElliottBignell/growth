#ifndef XML_PARSER_HPP
#define XML_PARSER_HPP
/**
 *  @file
 *  Class "GetConfig" provides the functions to read the XML data.
 *  @version 1.0
 */
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <string>
#include <stdexcept>

using namespace xercesc;

// Error codes

enum {
   ERROR_ARGS = 1, 
   ERROR_XERCES_INIT,
   ERROR_PARSE,
   ERROR_EMPTY_DOCUMENT
};

class GetConfig
{
public:
    GetConfig();
    ~GetConfig();
    void readConfigFile(std::string&) throw(std::runtime_error);

    char *getWhorls() { return m_Whorls; }
    char *getTaper()  { return m_Taper;  }
    char *getPoints() { return m_Points; }
    char *getType()   { return m_Type;   }

private:

    void parse( DOMElement& );

    xercesc::XercesDOMParser *m_ConfigFileParser;
    char* m_Whorls;
    char* m_Taper;
    char* m_Points;
    char* m_Type;

    // Internal class use only. Hold Xerces data in UTF-16 SMLCh type.

    XMLCh* TAG_root;

    XMLCh* TAG_Spiral;
    XMLCh* ATTR_Whorls;
    XMLCh* ATTR_Taper;

    XMLCh* TAG_Curve;
    XMLCh* ATTR_Points;
    XMLCh* ATTR_Type;

    XMLCh* TAG_Point;
    XMLCh* ATTR_x;
    XMLCh* ATTR_y;
};
#endif
