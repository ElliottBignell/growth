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

//#include <fstream>
//#include <sstream>
//#include <string>
//
class bezier
{
private:
    matrix< float > transform;

public:
    bezier() 
        : transform( 4, 4 )
    {   
        static float transforms[] = {
            1, 0, 0, 0,
           -3, 3, 0, 0,
            3,-6, 3, 0,
           -1, 3,-3, 1
        };

        unsigned int n = 0;

        for ( unsigned int j = 0; j < 4; j++ ) {

            for ( unsigned int k = 0; k < 4; k++ ) {
                transform( j, k ) = transforms[ n++ ];
            }
        }
    }

    const matrix< float >& get() { return transform; }
};
