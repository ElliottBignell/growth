#include <memory>
#include <vector>
#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include "boost/tuple/tuple.hpp"
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include "const.h"
#include "whorldata.h"

using namespace std;
using namespace boost::numeric::ublas;

float whorlData::shrinkystage{ 0.99 };
float whorlData::shrinkxstage{ 0.75 };
float whorlData::degX{ 0.05 };
float whorlData::degZ{ 2.0f * pi / 180.0f };
float whorlData::degY{ degZ * 0.00375f / ( 2.0f * pi / 360.0f ) };

unsigned int whorlData::circle{ static_cast< unsigned int >( 2.0f * pi / degZ ) };
