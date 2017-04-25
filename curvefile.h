///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///
///@section LICENSE
///
///This program is free software; you can redistribute it and/or
///modify it under the terms of the GNU General Public License as
///published by the Free Software Foundation; either version 2 of
///the License, or (at your option) any later version.
///
///This program is distributed in the hope that it will be useful, but
///WITHOUT ANY WARRANTY; without even the implied warranty of
///MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
///General Public License for more details at
///https://www.gnu.org/copyleft/gpl.html
///
///@section DESCRIPTION
///
///

#include <fstream>
#include <sstream>
#include <string>

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
