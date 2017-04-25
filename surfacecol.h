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

#include "const.h"

class colour
{
protected:
    colour() {} 
    virtual ~colour() = 0;

public:
    virtual unsigned int operator()( float, float ) = 0;
};

colour::~colour()
{
}

class insidecol : public colour
{
public:
    virtual ~insidecol() {}

    virtual unsigned int operator()( float thetaX, float theta )
    {
        return 2;
    }
};

class peakcol : public colour
{
public:
    virtual ~peakcol() {}

    virtual unsigned int operator()( float thetaX, float theta )
    {
        //float wh  = 360.0 * theta  / ( 2.0 * pi );
        //float whX = 360.0 * thetaX / ( 2.0 * pi );

        //return ( (int)( thetaX * 100 ) % 20 ) > 10 ? 0 : 1;
        return ( (int)( theta * 20 ) % 5 ) < 4 ? 0 : 1;
    }
};

class troughcol : public colour
{
public:
    virtual ~troughcol() {}

    virtual unsigned int operator()( float thetaX, float theta )
    {
        //float wh  = 360.0 * theta  / ( 2.0 * pi );
        //float whX = 360.0 * thetaX / ( 2.0 * pi );

        //return ( (int)( thetaX * 10 ) % 20 ) > 1 ? 0 : 1;
        return ( (int)( theta * 20 ) % 5 ) < 4 ? 0 : 1;
    }
};

