///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///

#ifndef _FLARE_H_INCLUDED_
#define _FLARE_H_INCLUDED_

#include "distbase.h"

class flare : public distribution
{
    unsigned int cnt;
    unsigned int halving;

public:
    flare( unsigned int h ) : cnt ( 0 ), halving( h )
    {
    }

    virtual ~flare() {} 

    virtual double operator()( double, double )
    {
        return 1.0 + 0.2 * pow( 0.5, static_cast< float >( ++cnt ) / halving );
    }
};

#endif //_FLARE_H_INCLUDED_

