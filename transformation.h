///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///
/// Classes encapsulating a generalised transformation in 3 dimensions

#ifndef __TRANSFORMATION_H_INCLUDED_
#define __TRANSFORMATION_H_INCLUDED_

#include "whorldata.h"

//! Abstracts a general transformation in 3D space
/*! 
*/
template < typename T >
class transformationBase
{
protected:
    T& t;

public:
    transformationBase( T& rt ) : t( rt) {}
    virtual ~transformationBase() = 0;
};

template < typename T >
transformationBase< T >::~transformationBase() 
{
}

whorlData data;

//! Abstracts a transformation in 3D space
/*! 
*/
class transformation : public transformationBase< whorlData >
{
public:
    transformation( whorlData& data ) 
        : transformationBase< whorlData >( data )
    {
    } 

    matrix< float > operator()( matrix< float > m ) { return m + t.offset; }

    void operator++(int) 
    {
        t.translateY *= t.yshrink;
        t.offset += t.translateY * whorlData::shrinkxstage;
    }
};


#endif // __TRANSFORMATION_H_INCLUDED_
