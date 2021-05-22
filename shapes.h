///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///
/// Class encapsulating a shape with matrices of coordinates
/// for points, normals and a record of the previous state

#ifndef __SHAPES_H_INCLUDED_
#define __SHAPES_H_INCLUDED_

#include <boost/range/adaptors.hpp>
#include <boost/range/combine.hpp>

using namespace boost::numeric::ublas;

typedef matrix< float > MF;

//! Abstracts a shape or aggregate of shapes combined into a single render
/*! 
*/
class shapes
{
public:
    matrix< float > shape;
    matrix< float > record;
    matrix< float > normals;

    static float R;
    static float r;
    unsigned int point{ 0 };

public:
    virtual ~shapes() = 0;

    typedef matrix_row<    matrix< float > > row;
    typedef matrix_column< matrix< float > > col;
};

#endif // __SHAPES_H_INCLUDED_
