#include <memory>
#include <vector>
#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include "boost/tuple/tuple.hpp"
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include "surfacecol.h"

using namespace std;
using namespace boost::numeric::ublas;

colour::~colour()
{
}

ostream& operator<<( ostream& os, const state s )
{
    if ( 4 == s.get< 1 >() ) {

        if ( s.get< 0 >() == 1 ) {
            os << "texture { defTexture3" ;
        }
        else if ( s.get< 0 >() == 0 ) {
            os << "texture { defTexture2" ;
        }
        else {
            os << "texture { defTexture1" ;
        }
    }
    else {

        switch ( s.get< 0 >() ) {
        case 1:
        case 2:

            switch ( s.get< 3 >() % 2 ) {
            case 0:
                if ( s.get< 1 >() == 1 ) {
                    os << "texture { defTexture3" ;
                }
                else if ( s.get< 1 >() == 0 ) {
                    os << "texture { defTexture2" ;
                }
                else {
                    os << "texture { defTexture1" ;
                }
                break;
            case 1:
                if ( s.get< 1 >() == 1 ) {
                    os << "texture { defTexture3" ;
                }
                else if ( s.get< 1 >() != 0 ) {
                    os << "texture { defTexture2" ;
                }
                else {
                    os << "texture { defTexture1" ;
                }
                break;
            }

            break;
        case 3:
            os << "texture { insideTexture" ;
            break;
        }
    }

    os << " }"
       << endl;

    return os;
}

ostream& operator<<( ostream& os, const std::vector< matrix< float > >::reference p )
{
    os << "<" 
       << p( 0, 0 )
       << ","
       << p( 0, 1 )
       << ","
       << p( 0, 2 )
       << ">";

    return os;
}
