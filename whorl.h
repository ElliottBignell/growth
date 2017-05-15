///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///
/// Classes used to transform an initial cross-sectional curve
/// around a logarithmic spiral in 3 dimensions

#ifndef __WHORL_H_INCLUDED_
#define __WHORL_H_INCLUDED_

#include <vector>

static float shrinkystage = 0.99;
static float shrinkxstage = 0.75;
static unsigned int cone = 10000;
static unsigned int whorls = 3;
static float R = 2.5;
static float degX = 0.05;
static float degZ = 2;
static float degY = degZ * 0.00375;
static unsigned int circle = 360 / degZ;
static string bezierpts = "{1.0,-13.0},{1.0,-1.8},{1.0,5.8},{0.0,5.8},{1.0,-13.0},{1.0,-1.8},{1.0,5.8},{0.0,5.8};{0.0,5.8},{-1.0,5.8},{1.0,-13.0},{1.0,-13.0}";

static float thetaX = degX / ( 180 / pi );
static float thetaY = degY / ( 180 / pi );

static float r = 1.0;

static float thetaZ = degZ / ( 180 / pi );

matrix< float >  metafocus( 1, 3 );
matrix< float >    rotateX( 3, 3 );
matrix< float >    rotateY( 3, 3 );
matrix< float >    rotateZ( 3, 3 );
matrix< float >          p( 1, 3 );

//! Abstracts a shape or aggregate of shapes combined into a single render
/*! 
*/
class shapes
{
public:
    static unsigned int point;

    static std::vector< matrix< float > > shape;
    static std::vector< matrix< float > > normals;
    static std::vector< matrix< float > > record;

public:
    virtual ~shapes() = 0;
};

shapes::~shapes()
{
}

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

//! Data for configuring a logarithmic spiral
/*! 
*/
class whorlData
{
public:
    matrix< float > translateY;
    matrix< float > offset;
    float shrinkstage;
    float shrink;
    double yshrink;
    float tY;

public:
    whorlData()  
        : translateY( 1, 3 )
        , offset( 1, 3 )
        , shrinkstage( 0.5 )
        , shrink( pow( shrinkstage, 1.0 / circle ) )
        , yshrink( 1.0 )
        , tY( shrinkystage / circle * degY )
    {
    };

    void update()
    {
        translateY( 0, 0 ) = 0;
        translateY( 0, 1 ) = tY;
        translateY( 0, 2 ) = 0;

        offset( 0, 0 ) = 0;
        offset( 0, 1 ) = 0;
        offset( 0, 2 ) = 0;
    }
};

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
        t.offset += t.translateY * shrinkxstage;
    }
};


//! Abstracts a poly-bezier shape repeated around a logarithmic spiral
/*! 
*/
template < typename SURF, typename COLOUR >
class shapeCurve : public shapes
{
    std::shared_ptr< meshfile > meshFile;
    transformation origin;

    SURF surf;
    COLOUR col;

public:
    shapeCurve( std::shared_ptr< meshfile >  mf, const string datfile ) 
        : meshFile( mf )
        , origin( data )
    {
        static bezier bz;

        curveFile wall( datfile );    

        matrix< float > rotateNormal( 3, 3 );

        rotateNormal( 2, 2 ) =              1;
        rotateNormal( 0, 0 ) =  cos( pi / 2.0 );
        rotateNormal( 0, 1 ) = -sin( pi / 2.0 );
        rotateNormal( 1, 0 ) =  sin( pi / 2.0 );
        rotateNormal( 1, 1 ) =  cos( pi / 2.0 );

        static const float xlim = abs( r );
        //    static const float ylim = xlim / 2.0;

        std::vector < matrix< float > > wallPoints;

        record.clear();
        shape.clear();
        normals.clear();

        record.reserve( circle + 2);
        shape.reserve( circle + 2 );
        normals.reserve( circle + 2 );

        unsigned int m = 0;

        unsigned int segments = wall.size();

        matrix< float > wallSegment( 4, 2 );

        for ( unsigned int segment = 0; segment < segments; segment++ ) {

            for ( unsigned int n = 0; n < 4; n++ ) {

                // Outer wall
                wallSegment( n, 0 ) =  wall[ m   ].first * xlim;
                wallSegment( n, 1 ) =  wall[ m++ ].second;
            }

            m--;

            wallPoints.push_back( wallSegment );
        }

        const float halfcircle   = static_cast< float >( circle ) / segments;
        const unsigned int section = halfcircle / 2.0;

        std::vector < matrix< float > >::iterator it = wallPoints.begin();

        matrix< float >& wallStep = *it;

        while ( it != wallPoints.end() ) {

            wallStep = *it++;

            for ( unsigned int n = 0; n < section; n++ ) {
                addStep( bz, wallStep, n, section );
            }
        }
    }

    void singleWhorl( std::vector< double >& bands, matrix< float >&cY, distribution  &ridgedistr, meshpov::triangle& q, double& theta, double thetaLimit, double lshrink )
    {
        const unsigned int closedcircle = bands.size();

        float ycircle = circle;
        double sshrink = lshrink;

        while ( theta < thetaLimit ) {

            float ridgedis = ridgedistr( 0, 0 );

            assert( closedcircle > 0 );

            for( matrix< float >& mf: shape   ) { mf *= sshrink; }
            for( matrix< float >& mf: normals ) { mf *= sshrink; }

            unsigned int whorlpos = 0;

            for ( ; whorlpos < closedcircle; whorlpos++ ) {

                meshpov::triangle    pt( metafocus + shape[ whorlpos ] * ridgedis * bands[ whorlpos ] );

                meshpov::normal  normal = normals[ whorlpos ];

                q = prod( surf( pt, normal, theta, static_cast< float >( whorlpos * closedcircle ) / ( pi * 0.0 ) ), cY );
                q = origin( q );

                record[ whorlpos ] = q;

                cY  = prod( cY, rotateY );

                origin++;
            }

            record.resize( whorlpos );

            theta = stitchToCurve( record, normals, theta, point );

            metafocus *= pow( data.yshrink, closedcircle ); 

            ridgedis = ridgedistr( 0, 0 );

            thetaY  /= pow( data.yshrink, 1.0 / ycircle );
            sshrink *= pow( data.yshrink, 1.0 / ycircle );

            double thetaYdis = thetaY * ridgedis;

            rotateY( 1, 1 ) =              1;
            rotateY( 0, 0 ) =  cos( thetaYdis );
            rotateY( 2, 0 ) =  sin( thetaYdis );
            rotateY( 0, 2 ) = -rotateY( 2, 0 );
            rotateY( 2, 2 ) =  rotateY( 0, 0 );

            ycircle = 360.0 / thetaYdis * 2;

            lshrink = pow( data.shrink, 1.0 / ycircle );
        }
    }

    void whorl( const matrix< float > &p )
    {
        static gaussian dgauss( 1, 0.01 );
        static distnull dnull;

        static distribution &ridgedistr = dgauss;
        static distribution &banddistr  = dgauss;

        matrix< float >          s( p    );
        matrix< float >       site( 1, 1 );

        matrix< float > cX = rotateX;

        const unsigned int closedcircle = shape.size();

        std::vector< double > bands( closedcircle );

        for( double& b: bands ) { b = banddistr( 0, 0 ); }

        pigmentmap pm( closedcircle, cone * whorls );

        double lshrink = pow( data.shrink, 1.0 / ( closedcircle - 1 ) );

        data.yshrink = pow( lshrink, 1.0 / ( closedcircle - 1 ) );

        meshpov::triangle q;

        const float jerk = 3.0 / 8.0;
        double theta = 0.0;
        double thetaLimit = 0.0;

        matrix< float > cY = rotateY;

        for ( unsigned int whorlno = 0; whorlno < whorls / jerk; whorlno++ ) {

            flare f;

            thetaLimit += pi * 2.0 * jerk;
            singleWhorl( bands, cY, f, q, theta, thetaLimit, lshrink );
        }

        std::vector< matrix< float > >::iterator it = normals.begin();

        for ( unsigned int whorlpos = 0; whorlpos < closedcircle; whorlpos++ ) {
            triangle( q, *it++, point, point, point, 0 );
        }

        for ( unsigned int n = 1; n < closedcircle; n++ ) {
            triangle( ( n - 1 ) % closedcircle, n % closedcircle, n, 0 );
        }
    }

    double stitchToCurve( std::vector< matrix< float > >& curve, std::vector< matrix< float > >& norms, float angle, unsigned int& index )
    {
        unsigned int size = curve.size();
        unsigned int step = size * 2;

        for ( unsigned int whorlpos = 0; whorlpos < size; whorlpos++ ) {

            meshpov::normal  normal = norms[ whorlpos ];

            triangle( curve[ whorlpos ], normal, index, index + 1,        index + step + 1, col( angle, thetaY ) );
            triangle( curve[ whorlpos ], normal, index, index + step + 1, index + step,     col( angle, thetaY ) );

            index++;

            angle += abs( thetaY );
        }

        return angle;
    }

private:
    void triangle( unsigned int pos, unsigned int pos1, unsigned int pos2, unsigned int col  )
    {
        matrix< int > indices( 1, 3 );

        indices( 0, 0 ) = pos;
        indices( 0, 1 ) = pos1;
        indices( 0, 2 ) = pos2;

        (*meshFile) << meshpov::index( indices, col );
    }

    void triangle( meshpov::triangle& q, const meshpov::normal& norm, unsigned int pos, unsigned int pos1, unsigned int pos2, unsigned int col )
    {
        matrix< int > indices( 1, 3 );

        indices( 0, 0 ) = pos;
        indices( 0, 1 ) = pos1;
        indices( 0, 2 ) = pos2;

        (*meshFile) << q << norm << meshpov::index( indices, col );
    }

    void addStep( bezier &bz, matrix< float >& wallSegment, const unsigned int n, const float section )
    {
        float f = static_cast< float >( n ) / section;

        matrix< float >         t( 1, 4 );
        matrix< float >         s( p    );
        matrix< float >         N( 1, 3 );

        t( 0, 0 ) = 1;
        t( 0, 1 ) = f;
        t( 0, 2 ) = f * f;
        t( 0, 3 ) = f * t( 0, 2 );

        matrix< float > tZ = prod( bz.get(), wallSegment );
        matrix< float > rZ( prod( t, tZ ) );

        s( 0, 0 ) = rZ( 0, 0 );
        s( 0, 1 ) = rZ( 0, 1 );
        s( 0, 2 ) = 0;

        float deriv_x = rZ( 0, 0 );
        float deriv_y = rZ( 0, 1 );
        float atan_t = pow( pow( deriv_x, 2 + pow( deriv_y, 2) ), 0.5 );

        N( 0, 0 ) = deriv_x / atan_t;
        N( 0, 1 ) = deriv_y / atan_t;
        N( 0, 2 ) = 0;

        record.push_back( origin( s ) );
        shape.push_back( s );
        normals.push_back( N );
    }
};

unsigned int shapes::point = 0;
std::vector< matrix< float > > shapes::shape;
std::vector< matrix< float > > shapes::normals;
std::vector< matrix< float > > shapes::record;

#endif // __WHORL_H_INCLUDED_
