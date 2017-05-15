///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///

#include <iostream>
#include <stdlib.h>
#include <memory>
#include <map>
#include <functional>
#include <iterator>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include "boost/tuple/tuple.hpp"
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <getopt.h>
#include "gaussian.h"
#include "distnull.h"
#include "distflare.h"
#include "porphyry.h"
#include "curvefile.h"
#include "curveofile.h"
#include "surface.h"
#include "surfacecol.h"
#include "bezier.h"
#include "whorl.h"

using namespace std;
using namespace boost::numeric::ublas;

std::shared_ptr< meshfile > test( new meshnull );

template < typename T >
class State
{
    T n;

public:
    State( T m ) : n( m   ) {}
    State( const State& s ) : n( s.n ) {}

    template < unsigned int N >
    const unsigned int get() const { return n.get< N >(); }

    State& operator++() { n.get< 0 >()++; return *this; }
};

typedef boost::tuple< unsigned int, unsigned int, unsigned int, unsigned int > state_nums;
typedef State< state_nums > state;
typedef State< matrix< float > > mstate;

class cell
{
    static float probability;
    static gaussian gauss;

    unsigned int active;
    unsigned int inactive;
    float concentration;
    bool on;

public:
    cell()
        : active( 0 )
        , inactive( 0 )
        , concentration( 0.0 )
        , on( false )
    {
    }

    cell( const cell& c )
        : active( c.active  )
        , inactive( c.inactive  )
        , concentration( c.concentration )
        , on( c.on )
    {
    }

    ~cell()
    {
    }

    void operator=( const cell& c ) 
    {
        active        = c.active;
        inactive      = c.inactive;
        concentration = c.concentration;
        on            = c.on;
    }

    void reset()   { concentration = 0.0; active = 0; on = false; }

    void trigger() 
    { 
        if ( 0 == active && inactive <= 0 ) {

            if ( concentration > 0.1 || gauss( 0, 0 ) > probability ) {

                active = 6;
                inactive = 2;
                on = true;
            }
        }
    }

    void produce() 
    { 
        if ( active > 0 ) { 
        
            concentration += 0.7; 
            active--; 

            if ( active <= 0 ) 
                on = false;
        } 
        else if ( inactive > 0 ) {
            inactive--;
        }
    }

    void inhibit() 
    {
        if ( concentration > 0.2 ) {
            concentration -= 0.2;
        }
        else {
            concentration = 0.0;
        }
    }

    void dump()    { cout << active; }

    friend class pigmentmap;
    friend class pigmentmap_;
};

float    cell::probability = 1.0;
gaussian cell::gauss( 0.0, 0.5 );

class pigmentmap;
class pigmentmap_;

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

void init()
{
    thetaX = degX / ( 180 / pi );
    thetaY = degY * degZ / ( 180 / pi );
    thetaZ = degZ / ( 180 / pi );

    metafocus( 0, 0 ) = R;
    metafocus( 0, 1 ) = 0;
    metafocus( 0, 2 ) = 0;
//    translateX( 0, 0 ) = tX;
//    translateX( 0, 1 ) = 0;
//    translateX( 0, 2 ) = 0;
    rotateX( 0, 0 ) =              1;
    rotateX( 1, 1 ) =  cos( thetaX );
    rotateX( 2, 1 ) = -sin( thetaX );
    rotateX( 1, 2 ) =  sin( thetaX );
    rotateX( 2, 2 ) =  cos( thetaX );
    rotateY( 1, 1 ) =              1;
    rotateY( 0, 0 ) =  cos( thetaY );
    rotateY( 2, 0 ) =  sin( thetaY );
    rotateY( 0, 2 ) = -sin( thetaY );
    rotateY( 2, 2 ) =  cos( thetaY );
    rotateZ( 2, 2 ) =              1;
    rotateZ( 0, 0 ) =  cos( thetaZ );
    rotateZ( 0, 1 ) = -sin( thetaZ );
    rotateZ( 1, 0 ) =  sin( thetaZ );
    rotateZ( 1, 1 ) =  cos( thetaZ );
    p( 0, 0 ) = r;
    p( 0, 1 ) = 0;
    p( 0, 2 ) = 0;
}

template < unsigned int n > void stateOut( pigmentmap& pm, unsigned int whorlno, unsigned int whorlpos  )
{
    int st = pm.isOn( whorlpos + n, whorlno );

    switch ( st ) {
    case 2:
        cout << state( state_nums( 0, 4, whorlno, whorlpos ) ) << endl;
        break;
    case 1:
        cout << state( state_nums( 1, 4, whorlno, whorlpos ) ) << endl;
        break;
    default:
        cout << state( state_nums( 2, 4, whorlno, whorlpos ) ) << endl;
        break;
    }
}

int main( int argc, char **argv )
{
    int c;

    string filename = "meshpov.pov" ; 

    while (1) {

        c = getopt(
            argc, 
            argv, 
            "dW:dX:dZ:tx:ty:r:R:T:C:k:b:o:?"
        );

        if (c == -1)
            break;

        float x = 0.0;

        switch (c) {
        case 't':
            break;
        case 'T':
            sscanf( optarg, "%f", &x );
            shrinkxstage = x;
            break;
        case 'W':
            sscanf( optarg, "%f", &x );
            whorls = x;
            break;
        case 'X':
            sscanf( optarg, "%f", &x );
            degX = x;
            break;
        //case 'Y':
            //sscanf( optarg, "%f", &x );
            //degY = x;
            //break;
        case 'Z':
            sscanf( optarg, "%f", &x );
            degZ = x;
            break;
        case 'x':
            sscanf( optarg, "%f", &x );
            break;
        case 'y':
            sscanf( optarg, "%f", &x );
            degY = x;
            break;
        case 'R':
            sscanf( optarg, "%f", &x );
            R = x;
            break;
        case 'r':
            sscanf( optarg, "%f", &x );
            r = -x;
            break;
        case 'C':
            sscanf( optarg, "%f", &x );
            cone = x;
            break;
        case 'k':
            sscanf( optarg, "%f", &x );
            data.shrinkstage = x;
            break;
        case 'b':
            bezierpts = optarg;
            break;
        case 'o':
            filename = optarg;
            break;
        case '?':
            break;
        default:
            printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

    circle = 360.0 / degZ;
    degY = degZ * 0.00375;
    cone = whorls * 360 / (degY * degZ);
    thetaZ = degZ / ( 180 / pi );
    //shrink = shrinkstage;
    data.shrink = data.shrinkstage;
    shrinkystage = pow( data.shrinkstage, 1.0 / circle );
    //data.tY = shrinkystage / circle * degY;
    data.tY = shrinkystage / circle * degY; // / 36000.0;

    data.update();

    cout << "Whorls: "       << whorls              << endl 
         << "Z-Resolution: " << degZ                << endl
         << "Y-Resolution: " << degY                << endl
         << "Shrink-Stage: " << data.shrinkstage    << endl
         << "Shrink-Y: "     << shrinkystage        << endl
         << "Shrink: "       << data.shrink         << endl
         << "Circle-divs: "  << circle              << endl
         << "Translate: "    << data.tY             << endl
         << "Taper: "        << shrinkxstage        << endl;

    if (optind < argc) {

        printf("non-option ARGV-elements: ");

        while (optind < argc)
            printf("%s ", argv[optind++]);

        printf("\n");

        return -1;
    }

    static const boost::regex x3dfile( ".*\\.x3d" ); 

    boost::match_results<std::string::const_iterator> results;

    if ( boost::regex_match( filename, results, x3dfile ) ) {
        test = std::shared_ptr< meshfile >( new meshx3d( filename ) );
    }
    else {
        test = std::shared_ptr< meshfile >( new meshpov( filename ) );
    }

    shapeCurve< outside, peakcol   > outer( test, "outer.dat" );
    shapeCurve<  inside, insidecol > inner( test, "inner.dat" );

    outer.stitchToCurve( inner.shape, inner.normals, 0, inner.point );
    
    init();
    outer.whorl( p );

    init();
    inner.whorl( p );

//    outer.stitchToCurve( inner.shape, inner.normals, 0, inner.point );

    test->close();

    return -1;
}
