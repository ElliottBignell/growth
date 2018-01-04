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
#include <xercesc/util/PlatformUtils.hpp>
#include <getopt.h>
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

#ifdef MAIN_TEST
/* This main is provided for unit test of the class. */

int main()
{
   return 0;
}
#endif


using namespace std;
using namespace boost::numeric::ublas;

std::shared_ptr< meshfile > test;

extern whorlData data;

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
};

float    cell::probability = 1.0;
gaussian cell::gauss( 0.0, 0.5 );

int main( int argc, char **argv )
{
    int c;

    string filename   = "meshpov.pov" ;
    string configFile;

    std::unique_ptr< curveExpression > innerEx;
    std::unique_ptr< curveExpression > outerEx;

    while (1) {

        c = getopt(
                argc, 
                argv, 
                "dW:dX:dZ:tx:ty:r:R:T:C:k:b:p:q:o:X:?"
                );

        if (c == -1)
            break;

        float x = 0.0;

        switch (c) {
            case 't':
                break;
            case 'T':
                sscanf( optarg, "%f", &x );
                whorlData::shrinkxstage = x;
                break;
            case 'W':
                sscanf( optarg, "%f", &x );
                whorls = x;
                break;
                //         case 'X':
                //             sscanf( optarg, "%f", &x );
                //             whorlData::degX = x;
                //             break;
                //case 'Y':
                //sscanf( optarg, "%f", &x );
                //whorlData::degY = x;
                //break;
            case 'Z':
                sscanf( optarg, "%f", &x );
                whorlData::degZ = 2.0f * pi / ( 360.0f / x );
                break;
            case 'x':
                sscanf( optarg, "%f", &x );
                break;
            case 'y':
                sscanf( optarg, "%f", &x );
                whorlData::degY = x;
                break;
            case 'R':
                sscanf( optarg, "%f", &x );
                shapes::R = x;
                break;
            case 'r':
                sscanf( optarg, "%f", &x );
                shapes::r = -x;
                break;
            case 'C':
                sscanf( optarg, "%f", &x );
                cone = x;
                break;
            case 'k':
                sscanf( optarg, "%f", &x );
                data.shrinkstage = x;
                break;
            case 'p':
                bezierpts = optarg;
                innerEx = std::make_unique< curveExpression >( bezierpts );
                break;
            case 'q':
                bezierpts = optarg;
                outerEx = std::make_unique< curveExpression >( bezierpts );
                break;
            case 'X':
                configFile = optarg;
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

    whorlData::circle = static_cast< unsigned int >( 2.0f * pi / whorlData::degZ );
    whorlData::degY = whorlData::degZ * 0.00375 / ( 2.0f * pi / 360.0f );
    cone = whorls * 2.0f * pi / (whorlData::degY * whorlData::degZ);
    //shrink = shrinkstage;
    data.shrink = data.shrinkstage;
    whorlData::shrinkystage = pow( data.shrinkstage, 1.0 / whorlData::circle );
    data.tY = whorlData::shrinkystage / whorlData::circle * whorlData::degY;

    data.update();

    cout << "Whorls: "       << whorls              << endl 
        << "Z-Resolution: " << whorlData::degZ                << endl
        << "Y-Resolution: " << whorlData::degY                << endl
        << "Shrink-Stage: " << data.shrinkstage    << endl
        << "Shrink-Y: "     << whorlData::shrinkystage        << endl
        << "Shrink: "       << data.shrink         << endl
        << "Circle-divs: "  << whorlData::circle              << endl
        << "Translate: "    << data.tY             << endl
        << "Taper: "        << whorlData::shrinkxstage        << endl;

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
        test = std::shared_ptr< meshfile >( std::make_shared< meshx3d >( filename ) );
    }
    else {
        test = std::shared_ptr< meshfile >( std::make_shared< meshpov >( filename ) );
    }

    if ( outerEx ) {

        shapeCurve< outside, wedgescol   > outer( test, *outerEx );
        outer.whorl();
    }

    if ( innerEx ) {

        shapeCurve<  inside, insidecol > inner( test, *innerEx );
        inner.whorl();
    }


    //shapeCurve< outside, peakcol   > outer( test, curveFile( "outer.dat" ) );
    // shapeCurve<  inside, insidecol > inner( test, curveFile( "inner.dat" ) );

    //    outer.stitchToCurve( inner.shape, inner.normals, 0, outer.point );
    //    outer.stitchToCurve( inner.shape, inner.normals, 0, inner.point );

    if ( !configFile.empty() ) {

        GetConfig appConfig;

        appConfig.readConfigFile( configFile );
        appConfig.speak( test );
        appConfig.clear();
    
        std::cout << "Done" << std::endl;

        /* xmlDeriv< xml, curve > elem2; */
        /* elem2.speak(); */

        /* auto t2 = translate< xml, rendering >( elem2 ); */

        /* t2->speak(); */
    }

    test->close();

    return -1;
}
