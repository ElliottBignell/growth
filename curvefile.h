///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///

#include <fstream>
#include <sstream>
#include <string>

//!  curveFile class representing a curve as a slightly smart array of points
/*!
  curveFile class abstracts a text file (gnuplot .dat file) containing a curve as a series
  of x-y values. This text file defines the cross-sectional shape of a form. 
  The .dat file can be directly loaded and plotted from gnuplot
  to facilitate designing the cross-section. On construction, the class
  loads the file into vectors of numeric values for the x and y ordinates.
  The class exposes these using the element operator as if they were a simple array of points.

The file uses the .dat format accepted by gnuplot, so that one can
call up a visual representation of the cross-section from the command line.
The following command called from gnuplot will show the inner and outer 
forms on a single graph:

<i>plot  "outer.dat" using 1:2 smooth bezier title "outer" with lines, "inner.dat" using 1:2 smooth bezier title "inner" with lines</i>
*/
class curveFile
{
private:
    std::vector< float > xarray, yarray;
    std::string filename;
    ifstream file;

public:
    curveFile( const std::string name ) 
        : filename( name )
        , file( filename.c_str() )
    {   
        std::string line;
        float a, b;

        std::getline( file, line );

        while ( std::getline( file, line ) )
        {
            std::istringstream iss( line );
       
            if ( !(iss >> a >> b) )
                break;
       
            xarray.push_back( a );
            yarray.push_back( b );
        }
       
        assert(  xarray.size() == yarray.size() );
        assert( 0 != ( xarray.size() ) );
        assert( 0 != ( yarray.size() ) );
        assert( 1 == ( xarray.size() % 3 ) );
        assert( 1 == ( yarray.size() % 3 ) );
    }

    std::pair< float, float > operator[]( unsigned int n )
    {
        assert( n < xarray.size() && n < yarray.size() );

        return std::pair< float, float >( xarray[ n ], yarray[ n ] );
    }

    std::vector< float >::size_type size() const { return ( xarray.size() - 1 ) / 3.0; }
};
