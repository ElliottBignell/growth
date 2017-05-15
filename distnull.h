///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///

#ifndef _DISTNULL_H_INCLUDED_
#define _DISTNULL_H_INCLUDED_

#include "distbase.h"

class distnull : public distribution
{
public:
    distnull()
    {
    }

    virtual ~distnull() {} 

    virtual double operator()( double, double )
    {
        return 1.0;
    }
};

#endif //_DISTNULL_H_INCLUDED_
