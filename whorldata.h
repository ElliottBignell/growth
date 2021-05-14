///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///
/// Classes encapsulating the scaling of a logarithmic whorl

#ifndef __WHORLDATA_H_INCLUDED_
#define __WHORLDATA_H_INCLUDED_

#define _GNU_SOURCE 1

#include <fenv.h>

int feenableexcept(int excepts);

using namespace std;
using namespace boost::numeric::ublas;

//! Data for configuring a logarithmic spiral
/*! 
*/
class whorlData
{
public:
    static float shrinkystage;
    static float shrinkxstage;
    static float degX;
    static float degZ;
    static float degY;
    static unsigned int circle;

    matrix< float > translateY;
    matrix< float > offset;
    float shrinkstage;
    float shrink;
    double yshrink;
    float tY;

public:
    whorlData()  
        : translateY( 1, 3 )
        , offset( 1, 3 )
        , shrinkstage( 0.5 )
        , shrink( pow( shrinkstage, 1.0 / circle ) )
        , yshrink( 1.0 )
        , tY( shrinkystage / circle * degY )
    {
    };

    void update()
    {
        translateY( 0, 0 ) = 0;
        translateY( 0, 1 ) = tY;
        translateY( 0, 2 ) = 0;

        offset( 0, 0 ) = 0;
        offset( 0, 1 ) = 0;
        offset( 0, 2 ) = 0;
    }
};

#endif //__WHORLDATA_H_INCLUDED_
