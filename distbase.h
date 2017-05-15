
///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///

#ifndef _DISTBASE_H_INCLUDED_
#define _DISTBASE_H_INCLUDED_

#include <iostream>
#include <stdio.h>

using namespace std;

class distribution
{
public:
    distribution()
    {
    }

    virtual ~distribution() = 0;

    virtual double operator()( double, double ) = 0;
};

#endif //_DISTBASE_H_INCLUDED_
