#include <boost/numeric/ublas/matrix.hpp>
#include <boost/regex.hpp>
#include "gaussian.h"
#include "distnull.h"
#include "curvefile.h"
#include "curveofile.h"
#include "surface.h"
#include "whorl.h"

static const float xlim = abs( shapes::r );

/*!
*/
void stepperPolyBezier::portionUpWhorl( const curveDef& wall )
{
    unsigned int m = 0;

    segments = wall.size();

    MF wallSegment( 4, 2 );

    for ( unsigned int segment = 0; segment < segments; segment++ ) {

        for ( unsigned int n = 0; n < 4; n++ ) {

            wallSegment( n, 0 ) =  wall[ m   ].first * xlim;
            wallSegment( n, 1 ) =  wall[ m++ ].second;
        }

        m--;

        wallPoints.push_back( wallSegment );
    }
}

void stepperPolyBezier::rotateDefiningPointŝ(  MF& wallSegment, const unsigned int n, const float section )
{
    shapes::row s( this->parent.shape,   n );
    matrix_row< matrix< float >> r( this->parent.record,  n );

    auto t( bz.getPowers( static_cast< float >( n ) / section ) );

    MF tZ = prod( this->bz.get(), wallSegment );
    rZ = prod( t, tZ );

    r( 0 ) = s( 0 ) = rZ( 0, 0 );
    r( 1 ) = s( 1 ) = rZ( 0, 1 );
    r( 2 ) = s( 2 ) = 0;
    r( 3 ) = s( 3 ) = 1;
}

/*!
*/
void stepperPolyBezier::addStep( const unsigned int n )
{
    shapes::row N( this->parent.normals, n );

    float deriv_x = rZ( 0, 0 );
    float deriv_y = rZ( 0, 1 );
    float atan_t = pow( pow( deriv_x, 2 ) + pow( deriv_y, 2 ), 0.5 );

    if ( atan_t != 0 ) {

        N(0) = deriv_x / atan_t;
        N(1) = deriv_y / atan_t;
    }
    else {

        N(0) = 1;
        N(1) = 1;
    }
    N( 2 ) = 0;
    N( 3 ) = 1;
}

/*!
*/
void stepperCompoundBezier::portionUpWhorl( const curveDef& wall )
{
    segments = wall.count();

    MF wallSegment( segments, 2 );

    for ( unsigned int n = 0; n < segments; n++ ) {

        wallSegment( n, 0 ) =  wall[ n ].first * xlim;
        wallSegment( n, 1 ) =  wall[ n ].second;
    }

    wallPoints.push_back( wallSegment );
}

/*!
*/
void stepperCompoundBezier::rotateDefiningPointŝ(  MF& wallSegment, const unsigned int n, const float section )
{
    shapes::row s( this->parent.shape,   n );
    matrix_row< matrix< float >> r( this->parent.record,  n );

    float f = static_cast< float >( n ) / section;

    int points = wallSegment.size1();

    shapes::col x( wallSegment,  0 );
    shapes::col y( wallSegment,  1 );

    rZ = matrix< float >( 1, 4 );

    r( 0 ) = s( 0 ) = rZ( 0, 0 ) = nBasedBezier( points, f, x, 0 );
    r( 1 ) = s( 1 ) = rZ( 0, 0 ) = nBasedBezier( points, f, y, 0 );
    r( 2 ) = s( 2 ) = 0;
    r( 3 ) = s( 3 ) = 1;
}

/*!
*/
void stepperCompoundBezier::addStep(  const unsigned int n )
{
    shapes::row N( this->parent.normals, n );

    float deriv_x = rZ( 0, 0 );
    float deriv_y = rZ( 0, 1 );
    float atan_t = pow( pow( deriv_x, 2 ) + pow( deriv_y, 2 ), 0.5 );

    if ( atan_t != 0 ) {

        N(0) = deriv_x / atan_t;
        N(1) = deriv_y / atan_t;
    }
    else {

        N(0) = 1;
        N(1) = 1;
    }
    N( 2 ) = 0;
    N( 3 ) = 1;
}

float stepperCompoundBezier::nBasedBezier( unsigned int index, float t, shapes::col& r, unsigned int offset )
{
    float f1;
    float f2;

    if ( 4 > index ) {

        double val1 = r( offset + 0 );
        double val2 = r( offset + 1 );;
        double val3 = r( offset + 2 );

        f1 = mix( val1, val2, t );
        f2 = mix( val2, val3, t );
    }
    else {

        f1 = nBasedBezier( index - 1, t, r, offset + 0 );
        f2 = nBasedBezier( index - 1, t, r, offset + 1 );
    }

    return mix( f1, f2, t );
}

float stepperCompoundBezier::mix(float a, float b, float t)
{
    return a * ( 1.0f - t ) + b * t;
}