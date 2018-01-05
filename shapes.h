///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///
/// Class encapsulating a shape with matrices of coordinates
/// for points, normals and a record of the previous state

#ifndef __SHAPES_H_INCLUDED_
#define __SHAPES_H_INCLUDED_

using namespace boost::numeric::ublas;

//! Abstracts a shape or aggregate of shapes combined into a single render
/*! 
*/
class shapes
{
protected:
    matrix< float > shape;
    matrix< float > record;
    matrix< float > normals;

public:
    static float R;
    static float r;
    unsigned int point{ 0 };

public:
    virtual ~shapes() = 0;
};

#endif // __SHAPES_H_INCLUDED_
