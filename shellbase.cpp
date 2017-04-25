#include <iostream>
#include <stdlib.h>
#include <memory>
#include <map>
#include <functional>
#include <iterator>
#include <iostream>
#include <vector>
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
#include "porphyry.h"
#include "curvefile.h"
#include "curveofile.h"
#include "surface.h"
#include "surfacecol.h"
#include "bezier.h"

using namespace std;
using namespace boost::numeric::ublas;

static float shrinkystage = 0.99;
static float shrinkxstage = 0.75;
static float shrinkstage = 0.5;
static unsigned int cone = 10000;
static unsigned int whorls = 3;
static float R = 2.5;
static float r = 1.0;
static float degX = 0.05;
static float degZ = 2;
static float degY = degZ * 0.00375;
static unsigned int circle = 360 / degZ;
static float shrink = pow( shrinkstage, 1.0 / circle );
static float tY     = shrinkystage / circle * degY;
static string bezierpts = "{1.0,-13.0},{1.0,-1.8},{1.0,5.8},{0.0,5.8},{1.0,-13.0},{1.0,-1.8},{1.0,5.8},{0.0,5.8};{0.0,5.8},{-1.0,5.8},{1.0,-13.0},{1.0,-13.0}";

static float thetaX = degX / ( 180 / pi );
static float thetaY = degY / ( 180 / pi );
static float thetaZ = degZ / ( 180 / pi );

matrix< float >  metafocus( 1, 3 );
matrix< float > translateY( 1, 3 );
matrix< float >    rotateX( 3, 3 );
matrix< float >    rotateY( 3, 3 );
matrix< float >    rotateZ( 3, 3 );
matrix< float >          p( 1, 3 );
matrix< float >     origin( 1, 3 );

std::vector< matrix< float > > shape;
std::vector< matrix< float > > normals;
std::vector< matrix< float > > record;

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

            if ( concentration > 0.1 || gauss() > probability ) {

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
    translateY( 0, 0 ) = 0;
    translateY( 0, 1 ) = tY;
    translateY( 0, 2 ) = 0;
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

void addStep( bezier &bz, matrix< float >& wallSegment, const unsigned int n, const float section )
{
    float f = static_cast< float >( n ) / section;

    matrix< float >         t( 1, 4 );
    matrix< float >         s( p    );

    t( 0, 0 ) = 1;
    t( 0, 1 ) = f;
    t( 0, 2 ) = f * f;
    t( 0, 3 ) = f * t( 0, 2 );

    matrix< float > tZ = prod( bz.get(), wallSegment );

    matrix< float > rZ( prod( t, tZ ) );

    s( 0, 0 ) = rZ( 0, 0 );
    s( 0, 1 ) = rZ( 0, 1 );
    s( 0, 2 ) = 0;

    record.push_back( origin + s );
    shape.push_back( s );
}

unsigned int initCircle( const string datfile )
{
    static bezier bz;

    curveFile wall( datfile );    

    matrix< float > rotateNormal( 3, 3 );

    rotateNormal( 2, 2 ) =              1;
    rotateNormal( 0, 0 ) =  cos( pi / 2.0 );
    rotateNormal( 0, 1 ) = -sin( pi / 2.0 );
    rotateNormal( 1, 0 ) =  sin( pi / 2.0 );
    rotateNormal( 1, 1 ) =  cos( pi / 2.0 );

    static const float xlim = abs( r );
//    static const float ylim = xlim / 2.0;

    std::vector < matrix< float > > wallPoints;

    record.clear();
     shape.clear();
   normals.clear();

    record.reserve( circle + 2);
     shape.reserve( circle + 2 );
   normals.reserve( circle + 2 );

    unsigned int m = 0;

    unsigned int segments = wall.size();

    matrix< float > wallSegment( 4, 2 );

    for ( unsigned int segment = 0; segment < segments; segment++ ) {

        for ( unsigned int n = 0; n < 4; n++ ) {

            // Outer wall
            wallSegment( n, 0 ) =  wall[ m   ].first * xlim;
            wallSegment( n, 1 ) =  wall[ m++ ].second;
        }

        m--;

        wallPoints.push_back( wallSegment );
    }

    const float halfcircle   = static_cast< float >( circle ) / segments;
    const unsigned int section = halfcircle / 2.0;

    std::vector < matrix< float > >::iterator it = wallPoints.begin();

    matrix< float >& wallStep = *it;

    while ( it != wallPoints.end() ) {

        wallStep = *it++;

        for ( unsigned int n = 0; n < section; n++ ) {
            addStep( bz, wallStep, n, section );
        }
    }

    it = wallPoints.begin();
    //wallStep = *it;

    //addStep( bz, wallStep, 0, section );

    //wallStep = *it;

    while ( it++ != wallPoints.end() ) {

        //wallStep = *it++;

        for ( unsigned int n = 0; n < section; n++ ) {

            matrix< float > normal(
                shape[ ( n + section - 1 ) % section ] - 
                shape[ ( n + section + 1 ) % section ]
                );

            normals.push_back( prod( normal, rotateNormal ) / 2.0 );
        }
    }

    normals.push_back( normals[ 0 ] );

    return section;
}

void triangle( unsigned int pos, unsigned int pos1, unsigned int pos2, unsigned int col  )
{
    matrix< int > indices( 1, 3 );

    indices( 0, 0 ) = pos;
    indices( 0, 1 ) = pos1;
    indices( 0, 2 ) = pos2;

    (*test) << meshpov::index( indices, col );
}

void triangle( meshpov::triangle& q, meshpov::normal& norm, unsigned int pos, unsigned int pos1, unsigned int pos2, unsigned int col )
{
    matrix< int > indices( 1, 3 );

    indices( 0, 0 ) = pos;
    indices( 0, 1 ) = pos1;
    indices( 0, 2 ) = pos2;

    (*test) << q << norm << meshpov::index( indices, col );
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

void whorl( const string datfile, const matrix< float > &p, surface& surf, colour& col )
{
    static gaussian distr( 1, 0.01 );

    matrix< float >          s( p    );
    matrix< float >       site( 1, 1 );

    unsigned int n = initCircle( datfile );
    unsigned int point = 0;

    matrix< float > cX = rotateX;
    matrix< float > cY = rotateY;

    const unsigned int closedcircle = shape.size();

    pigmentmap pm( closedcircle, cone * whorls );

    float ycircle = circle;

    double lshrink = pow( shrink, 1.0 / ( closedcircle - 1 ) );
    double yshrink = pow( lshrink, 1.0 / ( closedcircle - 1 ) );
    double sshrink = lshrink;
    double theta = 0.0;
    double thetaLimit = pi * 4.0 * whorls;

    unsigned int whorlstep = ( closedcircle - 4 ) * 2;

    meshpov::triangle q;
    meshpov::normal  normal;

    assert( theta < thetaLimit );

    while ( theta < thetaLimit ) {

        unsigned int whorlno = 0;

        float dis = distr();

        assert( closedcircle > 0 );

        for ( unsigned int wh = 0; wh < closedcircle; wh++, n++ ) {

            unsigned int whorlpos = n % closedcircle;

              shape[ whorlpos ] *= sshrink;
            normals[ whorlpos ] *= sshrink;

            state s( state_nums( 3, 0, whorlno, whorlpos ) );

            meshpov::triangle    pt( metafocus + shape[ whorlpos ] * dis );
            normal = normals[ whorlpos ];

            q = origin + prod( surf( pt, normal, theta, static_cast< float >( whorlpos * closedcircle ) / ( pi * 0.0 ) ), cY );

            if ( ( whorlpos ) > ( closedcircle / 2.0 ) || n > closedcircle ) {

                triangle( q, normal, point, point + 1,             point + whorlstep + 1, col( theta, thetaY ) );
                triangle( q, normal, point, point + whorlstep + 1, point + whorlstep,     col( theta, thetaY ) );

                point++;

                record[ whorlpos ] = q;
            }

            cY  = prod( cY, rotateY );

            translateY *= yshrink;

            origin += translateY * shrinkxstage;

            theta += abs( thetaY );
    
            whorlno++;
        }

        metafocus *= pow( yshrink, closedcircle ); 

        dis = distr();

        thetaY  /= pow( yshrink, 1.0 / ycircle );
        sshrink *= pow( yshrink, 1.0 / ycircle );

        double thetaYdis = thetaY * dis;

        rotateY( 1, 1 ) =              1;
        rotateY( 0, 0 ) =  cos( thetaYdis );
        rotateY( 2, 0 ) =  sin( thetaYdis );
        rotateY( 0, 2 ) = -sin( thetaYdis );
        rotateY( 2, 2 ) =  cos( thetaYdis );

        ycircle = 360.0 / thetaYdis * 2;

        lshrink = pow( shrink, 1.0 / ycircle );
    }

    for ( unsigned int wh = 0; wh < closedcircle; wh++, n++ ) {
        triangle( q, normal, point, point, point, 0 );
    }

    matrix< float > end = origin + prod( metafocus, cY );

    for ( n = 1; n < closedcircle; n++ ) {
        triangle( ( n - 1 ) % closedcircle, n % closedcircle, n, 0 );
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
            shrinkstage = x;
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
    shrink = shrinkstage;
    shrinkystage = pow( shrinkstage, 1.0 / circle );
    //tY = shrinkystage / circle * degY;
    tY = shrinkystage / circle * degY; // / 36000.0;

    cout << "Whorls: "       << whorls         << endl 
         << "Z-Resolution: " << degZ           << endl
         << "Y-Resolution: " << degY           << endl
         << "Shrink-Stage: " << shrinkstage    << endl
         << "Shrink-Y: "     << shrinkystage   << endl
         << "Shrink: "       << shrink         << endl
         << "Circle-divs: "  << circle         << endl
         << "Translate: "    << tY             << endl
         << "Taper: "        << shrinkxstage   << endl;

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

    init();

    outside os;
    inside  is;
    insidecol ic;
    peakcol pc;

    whorl( "outer.dat", p, os, pc );
    whorl( "inner.dat", p, is, ic );

    test->close();

    return -1;
}
