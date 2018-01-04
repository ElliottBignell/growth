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
#include <iostream>
#include <memory>
#include <map>

#include "curveofile.h"

using namespace xercesc;
using namespace std;

class curveLiteral;

// Error codes

enum {
   ERROR_ARGS = 1, 
   ERROR_XERCES_INIT,
   ERROR_PARSE,
   ERROR_EMPTY_DOCUMENT
};

enum types {
    xml,
    rendering
};

enum nodes {
    node,
    spiral,
    point,
    curve
};

template < types t >
class visitor;
typedef std::unique_ptr< XMLCh, void(*)( XMLCh*& ) > xmlString;

void releaseXmlString( XMLCh*& s );

template < types t >
class xmlBase
{
protected:
    DOMElement& domelem; 
    xmlBase< t >* parent{ nullptr };
    string name;
    list< std::shared_ptr< xmlBase > > children;

    typedef typename std::shared_ptr< xmlBase >(*maker)( DOMElement& );
    static map< string, typename xmlBase< t >::maker > registry;

public:
    xmlBase( DOMElement& p ) : domelem( p ) {}
    xmlBase(const xmlBase&) = delete;
    xmlBase& operator=(const xmlBase&) = delete;
    virtual ~xmlBase() = default;

    virtual void parse( DOMElement& ) = 0;

    static void Register( string s, maker m ) { registry[ s ] = m; }
    static std::shared_ptr< xmlBase > make( string s, DOMElement& elem ) { return registry[ s ]( elem ); }

    virtual void setparent( xmlBase< t >* r ) { parent = r; }
    virtual void speak( std::shared_ptr< meshfile > ) = 0;

    void setchild(  std::shared_ptr< xmlBase > r ) { children.push_back( r ); }

    void clear() 
    { 
        for ( std::shared_ptr< xmlBase >& p: children ) { p->clear(); }
        children.clear();
    }

    string getName()  { return name;  }
};

template< types t >
map< string, typename xmlBase< t >::maker > xmlBase< t >::registry;

template < types t, nodes n >
class xmlNodeBase : public xmlBase< t >
{
public:
    xmlNodeBase( DOMElement& p ) : xmlBase< t >( p ) {}

    static constexpr nodes index{ n };
    static constexpr types  type{ t };

    template <types t2 >
    auto accept( visitor< t2 >& v ) -> std::shared_ptr< xmlBase< t2 > >
        { return v.visit( *this ); }
};

template < types t, nodes n >
class xmlDeriv : public xmlNodeBase< t, n >
{
};

template <>
class xmlDeriv< xml, node > : public xmlNodeBase< xml, node >
{
public:
    xmlDeriv( DOMElement& p ) : xmlNodeBase( p ) {}
    virtual ~xmlDeriv() {}

    virtual void parse( DOMElement& elem ) { xmlBase::parse( elem ); }

    virtual void speak( std::shared_ptr< meshfile > mf ) 
    { 
        cout << "Root" << endl; 

        for ( std::shared_ptr< xmlBase >& p: children ) { p->speak( mf ); }
    }
};

template <>
class xmlDeriv< xml, curve > : public xmlNodeBase< xml, curve >
{
    xmlString   TAG_Curve{ XMLString::transcode( "Curve" ),  releaseXmlString };
    xmlString ATTR_Points{ XMLString::transcode( "points" ), releaseXmlString };
    xmlString   ATTR_Type{ XMLString::transcode( "type" ),   releaseXmlString };

    unique_ptr< curveLiteral > expr;

public:
    xmlDeriv( DOMElement& p );
    virtual ~xmlDeriv() {}

    xmlDeriv< type, index >& operator <<( pair< float, float > p );

    virtual void parse( DOMElement& elem ) { xmlBase::parse( elem ); }

    virtual void speak( std::shared_ptr< meshfile > );
};

template <>
class xmlDeriv< xml, spiral > : public xmlNodeBase< xml, spiral >
{
    xmlString  TAG_Spiral{ XMLString::transcode( "Spiral" ), releaseXmlString };
    xmlString ATTR_Whorls{ XMLString::transcode( "whorls" ), releaseXmlString };
    xmlString  ATTR_Taper{ XMLString::transcode( "taper" ),  releaseXmlString };

public:
    xmlDeriv( DOMElement& p ) : xmlNodeBase( p ) {}
    virtual ~xmlDeriv() {}

    virtual void parse( DOMElement& elem ) { xmlBase::parse( elem ); }

    virtual void speak( std::shared_ptr< meshfile > );
};

template <>
class xmlDeriv< xml, point > : public xmlNodeBase< xml, point >
{
    xmlString TAG_Point{ XMLString::transcode( "point" ), releaseXmlString };
    xmlString    ATTR_x{ XMLString::transcode( "x" ),     releaseXmlString };
    xmlString    ATTR_y{ XMLString::transcode( "y" ),     releaseXmlString };

public:
    xmlDeriv( DOMElement& p ) : xmlNodeBase( p ) {}
    virtual ~xmlDeriv() {}

    virtual void parse( DOMElement& elem );

    virtual void speak( std::shared_ptr< meshfile > );
};

class GetConfig
{
public:
    GetConfig();
    ~GetConfig();
    void readConfigFile(string&);

    void parse( DOMElement& );
    void speak( std::shared_ptr< meshfile > );
    void clear();

    char* m_Whorls{ nullptr };

private:

    xercesc::XercesDOMParser *m_ConfigFileParser;
    std::shared_ptr< xmlDeriv< xml, node > > theRoot;

    // Internal class use only. Hold Xerces data in UTF-16 SMLCh type.

    XMLCh* TAG_root;
};
#endif

template < types t, nodes n >
class deriv : public xmlNodeBase< t, n >
{
};

template <>
class deriv< rendering, node > : public xmlNodeBase< rendering, node >
{
public:
    virtual void parse( DOMElement& elem ) { xmlBase::parse( elem ); }

    void speak( std::shared_ptr< meshfile > ) { cout << "Unrelated 3" << endl;  }
};

template <>
class deriv< rendering, curve > : public xmlNodeBase< rendering, curve >
{
public:
    virtual void parse( DOMElement& elem ) { xmlBase::parse( elem ); }

    void speak( std::shared_ptr< meshfile > ) { cout << "Unrelated 4" << endl; }
};

template < types n >
class visitor
{
public:
    template < typename T >
    auto visit( T& t ) -> std::shared_ptr< deriv< n, T::index > > 
    {
        return make_shared< deriv< n, T::index > >();
    }
};

template < types T1, types T2, nodes n >
auto translate(  xmlNodeBase< T1, n >& elem ) -> std::shared_ptr< xmlNodeBase< T2, n > >
{
    visitor< T2 > v;

    return elem.accept( v );
}

template < typename T >
class xmlRegistry
{
public:
    xmlRegistry( string name ) 
    {
        xmlBase< xml >::Register( name, &makeInstance );
    };

    static std::shared_ptr< xmlBase< xml > > makeInstance( DOMElement& n ) 
    { 
        return make_shared< T >( n ); 
    }
};
