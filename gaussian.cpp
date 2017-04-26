///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///
#include <iostream>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <stdio.h>

using namespace std;

class gaussian
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

    double operator()()
    {
        return var_nor();
    }
};
