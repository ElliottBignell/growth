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
