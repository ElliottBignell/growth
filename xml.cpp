#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <list>
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include "boost/tuple/tuple.hpp"
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "gaussian.h"
#include "distnull.h"
#include "distflare.h"
#include "curvefile.h"
#include "curveofile.h"
#include "surface.h"
#include "surfacecol.h"
#include "bezier.h"
#include "whorl.h"
#include "xml.h"

using namespace std;
using namespace boost::numeric::ublas;

void releaseXmlString( XMLCh*& s )
{
    XMLString::release( &s );
}

template< types t >
void xmlBase< t >::parse( DOMElement& element )
{
    for (DOMNode* n (element.getFirstChild() );
         n != 0;
         n = n->getNextSibling ())
    {
        if ( n->getNodeType() == DOMNode::ELEMENT_NODE )
        {
            DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( n );

            auto a( currentElement->getTagName() );
            string tag = XMLString::transcode( a );

            auto c( xmlBase::make( tag, *currentElement ) );

            if ( c ) {

                c->setparent( this );
                setchild( c );

                c->parse( *currentElement );
            }
        }
    }
}

void xmlDeriv< xml, curve >::speak( std::shared_ptr< meshfile > mf ) 
{ 
    // Already tested node as type element and of name "Curve".
    // Read attributes of element "Curve".
    const XMLCh* xmlch_Points = domelem.getAttribute( &*ATTR_Points );
    char* points = XMLString::transcode(xmlch_Points);

    const XMLCh* xmlch_Type = domelem.getAttribute( &*ATTR_Type );
    char* type = XMLString::transcode(xmlch_Type);

    shapeCurve< outside, wedgescol   > outer( mf, *expr );
    outer.whorl();
}

xmlDeriv< xml, curve >::xmlDeriv( DOMElement& p ) 
    : xmlNodeBase( p )
    , expr( std::make_unique< curveLiteral >() ) 
{
}

xmlDeriv< xml, curve >& xmlDeriv< xml, curve >::operator <<( pair< float, float > p )
{
    *expr << p;
    return *this;
}

void  xmlDeriv< xml, point >::speak( std::shared_ptr< meshfile > ) 
{
}

void xmlDeriv< xml, point >::parse( DOMElement& elem )
{
    // Already tested node as type element and of name "Point".
    // Read attributes of element "Point".
    const XMLCh* xmlch = domelem.getAttribute( &*ATTR_x );
    char* x = XMLString::transcode( xmlch );

    xmlch = domelem.getAttribute( &*ATTR_y );
    char* y = XMLString::transcode( xmlch );

    xmlDeriv< type, curve >* c = dynamic_cast< xmlDeriv< type, curve >* >( parent );

    if ( c ) {
        *c << pair< float, float >( stod( x ), stod( y ) );
    }

    if ( x ) XMLString::release( &x );
    if ( y ) XMLString::release( &y );    

    xmlBase::parse( elem ); 
}

void  xmlDeriv< xml, spiral >::speak( std::shared_ptr< meshfile > ) 
{ 
    // Already tested node as type element and of name "Spiral".
    // Read attributes of element "Spiral".
    const XMLCh* xmlch_Whorls = domelem.getAttribute( &*ATTR_Whorls );
    char* whorls = XMLString::transcode(xmlch_Whorls);

    const XMLCh* xmlch_Taper = domelem.getAttribute( &*ATTR_Taper );
    char* taper = XMLString::transcode(xmlch_Taper);
}

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

   try
   {
      XMLString::release( &TAG_root );
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

void GetConfig::speak( std::shared_ptr< meshfile > mf ) 
{
    if ( theRoot ) {
        theRoot->speak( mf );
    }
}

void GetConfig::clear() 
{
    if ( theRoot ) {
        theRoot->clear();
    }
}

void GetConfig::parse( DOMElement& )
{
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
{
   // Test to see if the file is ok.

   struct stat fileStatus;

   errno = 0;
   int iretStat = stat(configFile.c_str(), &fileStatus);
   if( iretStat == -1 )
   {
      if( errno == ENOENT )        // errno declared by include file errno.h
         throw ( runtime_error("Path file_name does not exist, or path is an empty string.") );
      else if( errno == ENOTDIR )
         throw ( runtime_error("A component of the path is not a directory."));
      else if( errno == ELOOP )
         throw ( runtime_error("Too many symbolic links encountered while traversing the path."));
      else if( errno == EACCES )
         throw ( runtime_error("Permission denied."));
      else if( errno == ENAMETOOLONG )
         throw ( runtime_error("File can not be read\n"));
   }

   // Configure DOM parser.

   m_ConfigFileParser->setValidationScheme( XercesDOMParser::Val_Never );
   m_ConfigFileParser->setDoNamespaces( false );
   m_ConfigFileParser->setDoSchema( false );
   m_ConfigFileParser->setLoadExternalDTD( false );

   try
   {
      m_ConfigFileParser->parse( configFile.c_str() );

      // no need to free this pointer - owned by the domelem parser object
      DOMDocument* xmlDoc = m_ConfigFileParser->getDocument();

      // Get the top-level element: NAme is "root". No attributes for "root"
      
      DOMElement* elementRoot = xmlDoc->getDocumentElement();
      if( !elementRoot ) throw(runtime_error( "empty XML document" ));

      // Parse XML file for tags of interest: "Spiral"
      // Look one level nested within "root". (child of root)

      xmlRegistry< xmlDeriv< xml, spiral > > xml1{ string{ "Spiral" } };
      xmlRegistry< xmlDeriv< xml, curve >  > xml2{ "Curve"  };
      xmlRegistry< xmlDeriv< xml, point >  > xml3{ string{ "point" } };

      theRoot = std::make_shared< xmlDeriv< xml, node > >( *elementRoot );
      theRoot->parse( *elementRoot );
   }
   catch( xercesc::XMLException& e )
   {
      char* message = xercesc::XMLString::transcode( e.getMessage() );
      ostringstream errBuf;
      errBuf << "Error parsing file: " << message << flush;
      XMLString::release( &message );
   }
}
