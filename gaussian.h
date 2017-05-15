///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///

#ifndef _GAUSSIAN_H_INCLUDED_
#define _GAUSSIAN_H_INCLUDED_

#include "distbase.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

class gaussian : public distribution
{
    boost::mt19937 rng;
    boost::normal_distribution< double > nd;
    boost::variate_generator<
        boost::mt19937&, 
        boost::normal_distribution<> 
    > var_nor;

public:
    gaussian( double mean, double sigma )
        : nd( mean, sigma )
        , var_nor(rng, nd)
    {
        rng.seed( time( 0 ) );
    }

    virtual ~gaussian() {} 

    virtual double operator()( double, double )
    {
        return var_nor();
    }
};

#endif //_GAUSSIAN_H_INCLUDED_
