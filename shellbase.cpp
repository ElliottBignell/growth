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
#include <vector>
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

std::shared_ptr< meshfile > outputMeshFile;

class cell
{
    static float probability;
    static gaussian gauss;

    unsigned int active;
    unsigned int inactive;
    float concentration;
    bool on;

     whorlData &data;

public:
    cell( whorlData& d )
        : active( 0 )
        , inactive( 0 )
        , concentration( 0.0 )
        , on( false )
        , data( d )
    {
    }

    cell( const cell& c, whorlData& d )
        : active( c.active  )
        , inactive( c.inactive  )
        , concentration( c.concentration )
        , on( c.on )
        , data( d )
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

typedef std::list<std::string> StrList;

void tokenise(const std::string& in, const std::string& delims, StrList& tokens)
{
    tokens.clear();

    std::string::size_type pos_begin  , pos_end  = 0;
    std::string input = in;

    input.erase(std::remove_if(input.begin(),
                              input.end(),
                              [](auto x){return std::isspace(x);}),input.end());

    while ((pos_begin = input.find_first_not_of(delims,pos_end)) != std::string::npos)
    {
        pos_end = input.find_first_of(delims,pos_begin);
        if (pos_end == std::string::npos) pos_end = input.length();

        tokens.push_back( input.substr(pos_begin,pos_end-pos_begin) );
    }
}

int main( int argc, char **argv )
{
    feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW );

    int c;

    string filename   = "meshpov.pov" ;
    string configFile;

    static string bezierpts = "{1.0,-13.0},{1.0,-1.8},{1.0,5.8},{0.0,5.8},{1.0,-13.0},{1.0,-1.8},{1.0,5.8};{0.0,5.8},{-1.0,5.8},{1.0,-13.0},{1.0,-13.0}";

    StrList vtrToken;

    tokenise( bezierpts, ";" , vtrToken);

    std::vector< shared_ptr< curveExpression > > curveDefinitions;

    for (auto &subExpression : vtrToken) {
        curveDefinitions.push_back( make_shared< curveExpression >( subExpression ) );
    }

    whorlData data;

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
                curveDefinitions.clear();

                for (auto &subExpression : vtrToken) {
                    curveDefinitions.push_back( make_shared< curveExpression >( subExpression ) );
                }

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

    cout << "Whorls: "       << whorls                   << endl 
        << "Z-Resolution: " << whorlData::degZ           << endl
        << "Y-Resolution: " << whorlData::degY           << endl
        << "Shrink-Stage: " << data.shrinkstage          << endl
        << "Shrink-Y: "     << whorlData::shrinkystage   << endl
        << "Shrink: "       << data.shrink               << endl
        << "Circle-divs: "  << whorlData::circle         << endl
        << "Translate: "    << data.tY                   << endl
        << "Taper: "        << whorlData::shrinkxstage   << endl;

    if (optind < argc) {

        printf("non-option ARGV-elements: ");

        while (optind < argc)
            printf("%s ", argv[optind++]);

        printf("\n");

        return -1;
    }

    static const boost::regex x3dfile( ".*\\.x3d" ); 

    boost::match_results< string::const_iterator > results;

    if ( boost::regex_match( filename, results, x3dfile ) ) {
        outputMeshFile = shared_ptr< meshfile >( make_shared< meshx3d >( filename ) );
    }
    else {
        outputMeshFile = shared_ptr< meshfile >( make_shared< meshpov >( filename ) );
    }

    for ( auto curve: curveDefinitions ) {

        shapeCurve< outside, wedgescol   > outer( outputMeshFile, *curve, data );
        outer.whorl();
    }

    //shapeCurve< outside, peakcol   > outer( outputMeshFile, curveFile( "outer.dat" ) );
    // shapeCurve<  inside, insidecol > inner( outputMeshFile, curveFile( "inner.dat" ) );

    //    outer.stitchToCurve( inner.shape, inner.normals, 0, outer.point );
    //    outer.stitchToCurve( inner.shape, inner.normals, 0, inner.point );

    if ( !configFile.empty() ) {

        GetConfig appConfig;

        appConfig.readConfigFile( configFile );
        appConfig.speak( outputMeshFile, data );
        appConfig.clear();
    
        cout << "Done" << endl;

        /* xmlDeriv< xml, curve > elem2; */
        /* elem2.speak(); */

        /* auto t2 = translate< xml, rendering >( elem2 ); */

        /* t2->speak(); */
    }

    outputMeshFile->close();

    return -1;
}
