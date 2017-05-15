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

public:
    flare() : cnt ( 0 )
    {
    }

    virtual ~flare() {} 

    virtual double operator()( double, double )
    {
        return 1.0 + pow( 0.1, ++cnt );
    }
};

#endif //_FLARE_H_INCLUDED_

