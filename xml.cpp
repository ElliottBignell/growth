#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "xml.h"

using namespace std;

/**
 *  Constructor initializes xerces-C libraries.
 *  The XML tags and attributes which we seek are defined.
 *  The xerces-C DOM parser infrastructure is initialized.
 */

GetConfig::GetConfig()
{
   try
   {
      XMLPlatformUtils::Initialize();  // Initialize Xerces infrastructure
   }
   catch( XMLException& e )
   {
      char* message = XMLString::transcode( e.getMessage() );
      cerr << "XML toolkit initialization error: " << message << endl;
      XMLString::release( &message );
      // throw exception here to return ERROR_XERCES_INIT
   }

   // Tags and attributes used in XML file.
   // Can't call transcode till after Xerces Initialize()
   TAG_root        = XMLString::transcode("root");

   TAG_Spiral = XMLString::transcode("Spiral");
   ATTR_Whorls = XMLString::transcode("whorls");
   ATTR_Taper = XMLString::transcode("taper");

   TAG_Curve = XMLString::transcode("Curve");
   ATTR_Points = XMLString::transcode("points");
   ATTR_Type = XMLString::transcode("type");

   TAG_Point = XMLString::transcode("point");
   ATTR_x = XMLString::transcode("x");
   ATTR_y = XMLString::transcode("y");

   m_ConfigFileParser = new XercesDOMParser;
}

/**
 *  Class destructor frees memory used to hold the XML tag and 
 *  attribute definitions. It also terminates use of the xerces-C
 *  framework.
 */

GetConfig::~GetConfig()
{
   // Free memory

   delete m_ConfigFileParser;
   if(m_Whorls)   XMLString::release( &m_Whorls );
   if(m_Taper)   XMLString::release( &m_Taper );
   if(m_Points)   XMLString::release( &m_Points );
   if(m_Type)   XMLString::release( &m_Type );

   try
   {
      XMLString::release( &TAG_root );

      XMLString::release( &TAG_Spiral );
      XMLString::release( &ATTR_Whorls );
      XMLString::release( &ATTR_Taper );

      XMLString::release( &TAG_Curve );
      XMLString::release( &ATTR_Points );
      XMLString::release( &ATTR_Type );

      XMLString::release( &TAG_Point );
      XMLString::release( &ATTR_x );
      XMLString::release( &ATTR_y );
   }
   catch( ... )
   {
      cerr << "Unknown exception encountered in TagNamesdtor" << endl;
   }

   // Terminate Xerces

   try
   {
      XMLPlatformUtils::Terminate();  // Called after memory is released
   }
   catch( xercesc::XMLException& e )
   {
      char* message = xercesc::XMLString::transcode( e.getMessage() );

      cerr << "XML ttolkit teardown error: " << message << endl;
      XMLString::release( &message );
   }
}

void GetConfig::parse( DOMElement& element )
{
    DOMNodeList*      children = element.getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();

    // For all nodes, children of "root" in the XML tree.

    for( XMLSize_t xx = 0; xx < nodeCount; ++xx )
    {
        DOMNode* currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  // true is not NULL
                currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element 
        {
            // Found node which is an Element. Re-cast node as element
            DOMElement* currentElement
                = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if( XMLString::equals(currentElement->getTagName(), TAG_Spiral))
            {
                // Already tested node as type element and of name "Spiral".
                // Read attributes of element "Spiral".
                const XMLCh* xmlch_Whorls
                    = currentElement->getAttribute(ATTR_Whorls);
                m_Whorls = XMLString::transcode(xmlch_Whorls);

                const XMLCh* xmlch_Taper
                    = currentElement->getAttribute(ATTR_Taper);
                m_Taper = XMLString::transcode(xmlch_Taper);

                //break;  // Data found. No need to look at other elements in tree.

                cout << "Whorls="  << getWhorls()  << endl;
                cout << "Taper="   << getTaper()  << endl;
            }
            else if( XMLString::equals(currentElement->getTagName(), TAG_Curve))
            {
                std::cout << "Curve" << std::endl;
                // Already tested node as type element and of name "Curve".
                // Read attributes of element "Curve".
                const XMLCh* xmlch_Points
                    = currentElement->getAttribute(ATTR_Points);
                m_Points = XMLString::transcode(xmlch_Points);

                const XMLCh* xmlch_Type
                    = currentElement->getAttribute(ATTR_Type);
                m_Type = XMLString::transcode(xmlch_Type);

                //break;  // Data found. No need to look at other elements in tree.
                cout << "Points="  << getPoints()  << endl;
                cout << "Type="    << getType()  << endl;
            }
            else if( XMLString::equals(currentElement->getTagName(), TAG_Point))
            {
                std::cout << "Point ";
                // Already tested node as type element and of name "Point".
                // Read attributes of element "Point".
                const XMLCh* xmlch = currentElement->getAttribute(ATTR_x);
                char* x = XMLString::transcode( xmlch );

                xmlch = currentElement->getAttribute(ATTR_y);
                char* y = XMLString::transcode( xmlch );

                //break;  // Data found. No need to look at other elements in tree.
                cout << x  << ","  << getType()  << endl;

                if ( x ) XMLString::release( &x );
                if ( y ) XMLString::release( &y );
            }
        }
    }
}

/**
 *  This function:
 *  - Tests the access and availability of the XML configuration file.
 *  - Configures the xerces-c DOM parser.
 *  - Reads and extracts the pertinent information from the XML config file.
 *
 *  @param in configFile The text string name of the HLA configuration file.
 */

void GetConfig::readConfigFile(string& configFile)
        throw( std::runtime_error )
{
   // Test to see if the file is ok.

   struct stat fileStatus;

   errno = 0;
   int iretStat = stat(configFile.c_str(), &fileStatus);
   if( iretStat == -1 )
   {
      if( errno == ENOENT )        // errno declared by include file errno.h
         throw ( std::runtime_error("Path file_name does not exist, or path is an empty string.") );
      else if( errno == ENOTDIR )
         throw ( std::runtime_error("A component of the path is not a directory."));
      else if( errno == ELOOP )
         throw ( std::runtime_error("Too many symbolic links encountered while traversing the path."));
      else if( errno == EACCES )
         throw ( std::runtime_error("Permission denied."));
      else if( errno == ENAMETOOLONG )
         throw ( std::runtime_error("File can not be read\n"));
   }

   // Configure DOM parser.

   m_ConfigFileParser->setValidationScheme( XercesDOMParser::Val_Never );
   m_ConfigFileParser->setDoNamespaces( false );
   m_ConfigFileParser->setDoSchema( false );
   m_ConfigFileParser->setLoadExternalDTD( false );

   try
   {
      m_ConfigFileParser->parse( configFile.c_str() );

      // no need to free this pointer - owned by the parent parser object
      DOMDocument* xmlDoc = m_ConfigFileParser->getDocument();

      // Get the top-level element: NAme is "root". No attributes for "root"
      
      DOMElement* elementRoot = xmlDoc->getDocumentElement();
      if( !elementRoot ) throw(std::runtime_error( "empty XML document" ));

      // Parse XML file for tags of interest: "Spiral"
      // Look one level nested within "root". (child of root)

      parse( *elementRoot );
   }
   catch( xercesc::XMLException& e )
   {
      char* message = xercesc::XMLString::transcode( e.getMessage() );
      ostringstream errBuf;
      errBuf << "Error parsing file: " << message << flush;
      XMLString::release( &message );
   }
}
