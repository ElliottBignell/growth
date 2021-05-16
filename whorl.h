///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///
/// Classes used to transform an initial cross-sectional curve
/// around a logarithmic spiral in 3 dimensions

#ifndef __WHORL_H_INCLUDED_
#define __WHORL_H_INCLUDED_

#include <boost/range/adaptors.hpp>
#include <boost/range/combine.hpp>
#include <vector>
#include "whorldata.h"
#include "shapes.h"
#include "casteljau.h"

static unsigned int whorls = 3;
static string bezierpts = "{1.0,-13.0},{1.0,-1.8},{1.0,5.8},{0.0,5.8},{1.0,-13.0},{1.0,-1.8},{1.0,5.8},{0.0,5.8};{0.0,5.8},{-1.0,5.8},{1.0,-13.0},{1.0,-13.0}";

template<class... Conts>
auto zip_range(Conts&... conts)
  -> decltype(boost::make_iterator_range(
  boost::make_zip_iterator(boost::make_tuple(conts.begin()...)),
  boost::make_zip_iterator(boost::make_tuple(conts.end()...))))
{
  return {boost::make_zip_iterator(boost::make_tuple(conts.begin()...)),
          boost::make_zip_iterator(boost::make_tuple(conts.end()...))};
}

//! Abstracts a poly-bezier shape repeated around a logarithmic spiral
/*! 
*/
template < typename SURF, typename COLOUR >
class shapeCurve : public shapes
{
    MF    rotate{ 4, 4 };
    MF translate{ 4, 4 };
    MF     scale{ 4, 4 };

    float thetaX{ whorlData::degX };
    float thetaY{ whorlData::degY };
    float thetaZ{ whorlData::degZ };

    std::vector< double > bands;
    std::shared_ptr< meshfile > meshFile;

    SURF surf;
    COLOUR colour;

    whorlData &data;

    class stepperBase {
    protected:
        shapeCurve& parent;
        bezier& bz;

    public:
        stepperBase( shapeCurve& p, bezier& b )
            : parent( p )
            , bz( b )
        {

        }

        //! Computes a line segment of the Bezier curve using matrix transformations
        virtual void addStep( MF&, const unsigned int, const float ) = 0;
    };

    class stepperPolyBezier : public stepperBase {
    public:
        stepperPolyBezier( shapeCurve& p, bezier& b ) : stepperBase( p, b ) {}

        virtual void addStep( MF&, const unsigned int, const float );
    };

    class stepperCompoundBezier : public stepperBase {
    public:
        stepperCompoundBezier( shapeCurve& p, bezier& b ) : stepperBase( p, b ) {}

        virtual void addStep( MF&, const unsigned int, const float );
    };

    typedef matrix_row< matrix< float > > row;

public:
    shapeCurve( std::shared_ptr< meshfile >, const curveDef&, whorlData& );
    ~shapeCurve();

public:
    //! Draws the entire spiral
    void whorl();

    //! Draws the triangles to link two curves into a closed strip
    double stitchToCurve( MF&, float, unsigned int& );

private:
    //! Draws closed instances of the cross-sectional curve around a whole 360° loop
    void singleWhorl( double thetaL, distribution&, double& );

    //! Outputs a triangle to the mesh based on three points and a colour
    void triangle( unsigned int, unsigned int pos1, unsigned int pos2, unsigned int colour );

    //! Outputs a triangle's vertices  to the mesh based on three points and a colour
    void trianglePoints( row& );

    //! Outputs a triangle's indices  to the mesh based on three points and a colour
    void triangleIndices( row&, unsigned int, unsigned int pos1, unsigned int pos2, unsigned int );

    //! Fills the rotation matrices based on the thetaX, -Y and -Z values
    void initRotation( float );

    //! Fills the rotation matrices based on the thetaX, -Y and -Z values
    void initScale( float );

    //! Fills the rotation matrices based on the thetaX, -Y and -Z values
    void initTranslate( float );
};

template < typename SURF, typename COLOUR >
shapeCurve< SURF, COLOUR >::shapeCurve( std::shared_ptr< meshfile >  mf, const curveDef& wall, whorlData& d ) 
    : meshFile( mf )
    , data( d )
{
    static bezier bz;

    initRotation( thetaY );

    static const float xlim = abs( r );
    //    static const float ylim = xlim / 2.0;

    std::vector < MF > wallPoints;

    record.resize(  whorlData::circle + 2, 4 );
    shape.resize(   whorlData::circle + 2, 4 );
    normals.resize( whorlData::circle + 2, 4 );

    unsigned int m = 0;

    unsigned int segments = wall.size();

    MF wallSegment( 4, 2 );

    for ( unsigned int segment = 0; segment < segments; segment++ ) {

        for ( unsigned int n = 0; n < 4; n++ ) {

            wallSegment( n, 0 ) =  wall[ m   ].first * xlim;
            wallSegment( n, 1 ) =  wall[ m++ ].second;
        }

        m--;

        wallPoints.push_back( wallSegment );
    }

    const float halfcircle   = static_cast< float >( whorlData::circle ) / segments;

    unsigned int index = 0;
    unsigned int startOfSection = halfcircle;

    unique_ptr< stepperBase > curveStepper( make_unique< stepperCompoundBezier >( *this, bz ) );

    for( MF& wallStep: wallPoints ) { 

        for ( unsigned int n = 0; n <= halfcircle; n++ ) {

            if ( index > whorlData::circle ) {
                break;
            }

            curveStepper->addStep( wallStep, index++, startOfSection );
        }

        startOfSection += halfcircle;
    }

}

/*! 
    TODO
*/
template < typename SURF, typename COLOUR >
shapeCurve< SURF, COLOUR >::~shapeCurve() 
{
    ofstream vectors( "/home/elliott/shells/lines.txt", std::ofstream::out | std::ofstream::app );

    for ( size_t it = 0; it != shape.size1(); it++ ) {
        vectors << shape( it, 0 ) << " " << shape( it, 1 ) << std::endl;
    }
}

/*! Set up the colour and ridge mappings, calls the whorls and then closes the end of the 
    shell
*/
template < typename SURF, typename COLOUR >
void shapeCurve< SURF, COLOUR >::whorl()
{
    static gaussian dgauss( 1, 0.01 );
    static distnull dnull;

    const unsigned int closedcircle = shape.size1();

    thetaX = whorlData::degX;
    thetaY = whorlData::degZ;
    thetaZ = whorlData::degZ;

    const float sparsity = 1.0;

    data.yshrink = pow( ( whorlData::shrinkxstage / ( whorlData::shrinkxstage + 1 ) ) / sparsity, 1.0f / static_cast< float >( pi * 4.0 / whorlData::degZ ) );

    std::cout << "Shrinking: " <<  pi * 2.0 / whorlData::degZ  << std::endl;
    std::cout << "Shrinking: " <<  data.yshrink << std::endl;
    std::cout << "Shrinking: " <<  pow( data.yshrink, pi * 2.0 / whorlData::degZ ) << std::endl;

    initRotation( thetaY );
    initScale( data.yshrink );
    initTranslate( data.tY * whorlData::circle * sparsity );

    //static distribution &ridgedistr = dgauss;
    static distribution &banddistr  = dgauss;
    //static distribution &banddistr  = dnull;

    MF       site( 1, 1 );

    bands.resize( closedcircle );

    for( double& b: bands ) { b = banddistr( 0, 0 ); }

    // colour.setPigmentMap( std::make_shared< pigmentmap >( closedcircle, cone * whorls ) );

    const float jerk = 1.0;
    //const float jerk = 1.0 / 3.0;
    double theta = 0.0;
    double thetaLimit = 0.0;

    for ( unsigned int whorlno = 0; whorlno < whorls / jerk; whorlno++ ) {

        distnull f;
        //flare f( closedcircle / 4.0 );

        thetaLimit += pi * 2.0 * jerk;
        singleWhorl( thetaLimit, banddistr, theta );
    }
}

/*! Draws the curve point-by-point, stitches it back to the record of the previous
    point and then scales the curve down prior to the next iteration. Iterates round
    the primary axis until a whole whorl is completed.
*/
template < typename SURF, typename COLOUR >
void shapeCurve< SURF, COLOUR >::singleWhorl( double thetaLimit, distribution&, double& theta )
{
    while ( theta < thetaLimit ) {

        meshpov::triangle simple( prod( rotate, scale ) );
        meshpov::triangle composite( prod( translate, simple ) );

        record  = prod( record,  composite );
        normals = prod( normals, composite );

        stitchToCurve( record, theta, point );

        theta += whorlData::degZ;
    }
}

/*! 
    Draws alternating triangles between the corresponding points of two curves, 
    taking the colour from a mapping based on the X and Y positions
*/
template < typename SURF, typename COLOUR >
double shapeCurve< SURF, COLOUR >::stitchToCurve( MF& curve, float cell, unsigned int& index )
{
    unsigned int size = curve.size1();
    unsigned int step = size * 2;

    for ( unsigned int whorlpos = 0; whorlpos < size; whorlpos++ ) {

        float angleX = static_cast< float >( whorlpos ) / size * pi * 2.0;

        row mc( curve, whorlpos );

        trianglePoints( mc );

        if ( index > step ) {

            unsigned int nextPoint     = index - 1;
            unsigned int diagonalPoint = index - step - 1;
            unsigned int facingPoint   = index - step;

            matrix_row< MF > mr( curve, whorlpos );

            triangleIndices( mr, index, diagonalPoint, facingPoint,   colour( angleX, cell / whorls ) );
            triangleIndices( mr, index, nextPoint,     diagonalPoint, colour( angleX, cell / whorls ) );
        }

        index++;

        cell += abs( thetaY );
    }

    return cell;
}

/*! 
*/
template < typename SURF, typename COLOUR >
void shapeCurve< SURF, COLOUR >::triangle( unsigned int pos, unsigned int pos1, unsigned int pos2, unsigned int colour  )
{
    matrix< int > indices( 1, 4 );

    indices( 0, 0 ) = pos;
    indices( 0, 1 ) = pos1;
    indices( 0, 2 ) = pos2;
    indices( 0, 3 ) = 1;

    (*meshFile) << meshpov::index( indices, colour );
}

/*! 
*/
template < typename SURF, typename COLOUR >
void shapeCurve< SURF, COLOUR >::trianglePoints( row& q )
{
    matrix< float  > Q( 1, 4 );

    Q( 0, 0 ) = q( 0 );
    Q( 0, 1 ) = q( 1 );
    Q( 0, 2 ) = q( 2 );
    Q( 0, 3 ) = 1;

    (*meshFile) << Q;
}

/*!
*/
template < typename SURF, typename COLOUR >
void shapeCurve< SURF, COLOUR >::triangleIndices( row& n, unsigned int pos, unsigned int pos1, unsigned int pos2, unsigned int colour )
{
    matrix< int > indices( 1, 4 );
    matrix< double > N( 1, 4 );

    indices( 0, 0 ) = pos;
    indices( 0, 1 ) = pos1;
    indices( 0, 2 ) = pos2;
    indices( 0, 3 ) = 1;

    N( 0, 0 ) = n( 0 );
    N( 0, 1 ) = n( 1 );
    N( 0, 2 ) = n( 2 );
    N( 0, 3 ) = 1;

    (*meshFile) << N << meshpov::index( indices, colour );
}

/*!
*/
template < typename SURF, typename COLOUR >
void shapeCurve< SURF, COLOUR >::stepperPolyBezier::addStep( MF& wallSegment, const unsigned int n, const float section )
{
    float f = static_cast< float >( n ) / section;

    MF         t( 1, 4 );
    row s( this->parent.shape,   n );
    row r( this->parent.record,  n );
    row N( this->parent.normals, n );

    t( 0, 0 ) = 1;
    t( 0, 1 ) = f;
    t( 0, 2 ) = f * f;
    t( 0, 3 ) = f * t( 0, 2 );

    MF tZ = prod( this->bz.get(), wallSegment );
    MF rZ( prod( t, tZ ) );

    r( 0 ) = s( 0 ) = rZ( 0, 0 );
    r( 1 ) = s( 1 ) = rZ( 0, 1 );
    r( 2 ) = s( 2 ) = 0;
    r( 3 ) = s( 3 ) = 1;

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
template < typename SURF, typename COLOUR >
void shapeCurve< SURF, COLOUR >::stepperCompoundBezier::addStep(  MF& wallSegment, const unsigned int n, const float section )
{
    float f = static_cast< float >( n ) / section;

    MF         t( 1, 4 );
    row s( this->parent.shape,   n );
    row r( this->parent.record,  n );
    row N( this->parent.normals, n );

    t( 0, 0 ) = 1;
    t( 0, 1 ) = f;
    t( 0, 2 ) = f * f;
    t( 0, 3 ) = f * t( 0, 2 );

    MF tZ = prod( this->bz.get(), wallSegment );
    MF rZ( prod( t, tZ ) );

    r( 0 ) = s( 0 ) = rZ( 0, 0 );
    r( 1 ) = s( 1 ) = rZ( 0, 1 );
    r( 2 ) = s( 2 ) = 0;
    r( 3 ) = s( 3 ) = 1;

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
template < typename SURF, typename COLOUR >
void shapeCurve< SURF, COLOUR >::initRotation( float theta )
{
    rotate( 0, 0 ) =  cos( theta );
    rotate( 1, 0 ) =             0;
    rotate( 2, 0 ) =  sin( theta );
    rotate( 3, 0 ) =             0;
    rotate( 0, 1 ) =             0;
    rotate( 1, 1 ) =             1;
    rotate( 2, 1 ) =             0;
    rotate( 3, 1 ) =             0;
    rotate( 0, 2 ) = -rotate( 2, 0 );
    rotate( 1, 2 ) =             0;
    rotate( 2, 2 ) =  rotate( 0, 0 );
    rotate( 3, 2 ) =             0;
    rotate( 0, 3 ) =             0;
    rotate( 1, 3 ) =             0;
    rotate( 2, 3 ) =             0;
    rotate( 3, 3 ) =             1;
}

/*! 
*/
template < typename SURF, typename COLOUR >
void shapeCurve< SURF, COLOUR >::initTranslate( float f )
{
    translate( 0, 0 ) =  1;
    translate( 1, 0 ) =  0;
    translate( 2, 0 ) =  0;
    translate( 3, 0 ) =  0;
    translate( 0, 1 ) =  0;
    translate( 1, 1 ) =  1;
    translate( 2, 1 ) =  0;
    translate( 3, 1 ) =  f;
    translate( 0, 2 ) =  0;
    translate( 1, 2 ) =  0;
    translate( 2, 2 ) =  1;
    translate( 3, 2 ) =  0;
    translate( 0, 3 ) =  0;
    translate( 1, 3 ) =  0;
    translate( 2, 3 ) =  0;
    translate( 3, 3 ) =  1;
}

/*! 
*/
template < typename SURF, typename COLOUR >
void shapeCurve< SURF, COLOUR >::initScale( float f )
{
    scale( 0, 0 ) = f;
    scale( 1, 0 ) = 0;
    scale( 2, 0 ) = 0;
    scale( 3, 0 ) = 0;
    scale( 0, 1 ) = 0;
    scale( 1, 1 ) = f;
    scale( 2, 1 ) = 0;
    scale( 3, 1 ) = 0;
    scale( 0, 2 ) = 0;
    scale( 1, 2 ) = 0;
    scale( 2, 2 ) = f;
    scale( 3, 2 ) = 0;
    scale( 0, 3 ) = 0;
    scale( 1, 3 ) = 0;
    scale( 2, 3 ) = 0;
    scale( 3, 3 ) = 1;
}
#endif // __WHORL_H_INCLUDED_
