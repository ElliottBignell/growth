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

#ifndef __BEZIER_H__
#define __BEZIER_H__

#include "shapes.h"

using namespace std;
using namespace boost::numeric::ublas;

//! The bezier class abstracts a two-dimensional matrix of float
/*! values used to compute the points along a first-order bezier
    curve
*/
class bezier
{
private:
    matrix< float > transform; ///< Two-dimensional matrix of constant numeric values used to compute a bezier curve
    matrix< float > normal;    ///< Two-dimensional matrix of constant numeric values used to compute a bezier curve's normals

public:
    //! The constructor simply packs the values for the transformation into the
    //! initialised two-dimensional matrix
    bezier() : transform( 4, 4 ), normal( 3, 3 )
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

        static float normals[] = {
            1, 0, 0,
           -2, 2, 0,
            1,-2, 1
        };

        n = 0;

        for ( unsigned int j = 0; j < 3; j++ ) {

            for ( unsigned int k = 0; k < 3; k++ ) {
                normal( j, k ) = normals[ n++ ];
            }
        }
    }

    const matrix< float >& get()       { return transform; } ///<Simple const getter to access the matrix as read-only
    const matrix< float >& getNormal() { return normal; }    ///<Simple const getter to access the normals matrix as read-only

    const matrix< float > getPowers( float f )
    {
        MF t( 1, 4 );

        t( 0, 0 ) = 1;
        t( 0, 1 ) = f;
        t( 0, 2 ) = f * f;
        t( 0, 3 ) = f * t( 0, 2 );

        return t;
    }
};

#endif //__BEZIER_H__
