///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///
/// Constant data required globally. Kept, needless to say, to an 
/// absolute minimum.

#ifndef __CONST_H_INCLUDED_
#define __CONST_H_INCLUDED_

#include <cmath>

//! An irrational number, defined as the ratio of the circumference of a circle to its radius.
/*!
  Pi. It's PI, damn it. Don't you know anything?
*/
static const float pi = std::acos( 0.0 ) * 2.0;

#endif // __CONST_H_INCLUDED_
