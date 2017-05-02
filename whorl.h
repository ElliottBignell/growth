#include <vector>
static float shrinkystage = 0.99;
static float shrinkxstage = 0.75;
static float shrinkstage = 0.5;
static unsigned int cone = 10000;
static unsigned int whorls = 3;
static float R = 2.5;
static float degX = 0.05;
static float degZ = 2;
static float degY = degZ * 0.00375;
static unsigned int circle = 360 / degZ;
static float shrink = pow( shrinkstage, 1.0 / circle );
static float tY     = shrinkystage / circle * degY;
static string bezierpts = "{1.0,-13.0},{1.0,-1.8},{1.0,5.8},{0.0,5.8},{1.0,-13.0},{1.0,-1.8},{1.0,5.8},{0.0,5.8};{0.0,5.8},{-1.0,5.8},{1.0,-13.0},{1.0,-13.0}";

static float thetaX = degX / ( 180 / pi );
static float thetaY = degY / ( 180 / pi );

static float r = 1.0;

static float thetaZ = degZ / ( 180 / pi );

std::vector< matrix< float > > shape;
std::vector< matrix< float > > normals;
std::vector< matrix< float > > record;

matrix< float >  metafocus( 1, 3 );
matrix< float > translateY( 1, 3 );
matrix< float >    rotateX( 3, 3 );
matrix< float >    rotateY( 3, 3 );
matrix< float >    rotateZ( 3, 3 );
matrix< float >          p( 1, 3 );
matrix< float >     origin( 1, 3 );

//! Abstracts a poly-bezier shape repeated around a logarithmic spiral
/*! 
*/
template < typename SURF, typename COLOUR >
class shapeCurve
{
    std::shared_ptr< meshfile > meshFile;

    SURF surf;
    COLOUR col;

public:
    shapeCurve( std::shared_ptr< meshfile >  mf, const string datfile ) : meshFile( mf )
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

        it = wallPoints.begin();
        //wallStep = *it;

        //addStep( bz, wallStep, 0, section );

        //wallStep = *it;

        while ( it++ != wallPoints.end() ) {

            //wallStep = *it++;

            for ( unsigned int n = 0; n < section; n++ ) {

                matrix< float > normal(
                        shape[ ( n + section - 1 ) % section ] - 
                        shape[ ( n + section + 1 ) % section ]
                        );

                normals.push_back( prod( normal, rotateNormal ) / 2.0 );
            }
        }

        normals.push_back( normals[ 0 ] );
    }

    void addStep( bezier &bz, matrix< float >& wallSegment, const unsigned int n, const float section )
    {
        float f = static_cast< float >( n ) / section;

        matrix< float >         t( 1, 4 );
        matrix< float >         s( p    );

        t( 0, 0 ) = 1;
        t( 0, 1 ) = f;
        t( 0, 2 ) = f * f;
        t( 0, 3 ) = f * t( 0, 2 );

        matrix< float > tZ = prod( bz.get(), wallSegment );

        matrix< float > rZ( prod( t, tZ ) );

        s( 0, 0 ) = rZ( 0, 0 );
        s( 0, 1 ) = rZ( 0, 1 );
        s( 0, 2 ) = 0;

        record.push_back( origin + s );
        shape.push_back( s );
    }

    void whorl( const matrix< float > &p )
    {
        static gaussian distr( 1, 0.01 );

        matrix< float >          s( p    );
        matrix< float >       site( 1, 1 );

        unsigned int point = 0;

        matrix< float > cX = rotateX;
        matrix< float > cY = rotateY;

        const unsigned int closedcircle = shape.size();

        pigmentmap pm( closedcircle, cone * whorls );

        float ycircle = circle;

        double lshrink = pow( shrink, 1.0 / ( closedcircle - 1 ) );
        double yshrink = pow( lshrink, 1.0 / ( closedcircle - 1 ) );
        double sshrink = lshrink;
        double theta = 0.0;
        double thetaLimit = pi * 4.0 * whorls;

        unsigned int whorlstep = ( closedcircle - 4 ) * 2;

        meshpov::triangle q;

        assert( theta < thetaLimit );

        while ( theta < thetaLimit ) {

            unsigned int whorlno = 0;

            float dis = distr();

            assert( closedcircle > 0 );

            for( matrix< float >& mf: shape   ) { mf *= sshrink; }
            for( matrix< float >& mf: normals ) { mf *= sshrink; }

            for ( unsigned int whorlpos = 0; whorlpos < closedcircle; whorlpos++ ) {

                //state s( state_nums( 3, 0, whorlno, whorlpos ) );

                meshpov::triangle    pt( metafocus + shape[ whorlpos ] * dis );

                meshpov::normal  normal = normals[ whorlpos ];

                q = origin + prod( surf( pt, normal, theta, static_cast< float >( whorlpos * closedcircle ) / ( pi * 0.0 ) ), cY );

                record[ whorlpos ] = q;

                cY  = prod( cY, rotateY );

                translateY *= yshrink;

                origin += translateY * shrinkxstage;

                whorlno++;
            }

            for ( unsigned int whorlpos = 0; whorlpos < closedcircle; whorlpos++ ) {

                meshpov::normal  normal = normals[ whorlpos ];

                triangle( record[ whorlpos ], normal, point, point + 1,             point + whorlstep + 1, col( theta, thetaY ) );
                triangle( record[ whorlpos ], normal, point, point + whorlstep + 1, point + whorlstep,     col( theta, thetaY ) );

                point++;

                theta += abs( thetaY );
            }

            metafocus *= pow( yshrink, closedcircle ); 

            dis = distr();

            thetaY  /= pow( yshrink, 1.0 / ycircle );
            sshrink *= pow( yshrink, 1.0 / ycircle );

            double thetaYdis = thetaY * dis;

            rotateY( 1, 1 ) =              1;
            rotateY( 0, 0 ) =  cos( thetaYdis );
            rotateY( 2, 0 ) =  sin( thetaYdis );
            rotateY( 0, 2 ) = -sin( thetaYdis );
            rotateY( 2, 2 ) =  cos( thetaYdis );

            ycircle = 360.0 / thetaYdis * 2;

            lshrink = pow( shrink, 1.0 / ycircle );
        }

        std::vector< matrix< float > >::iterator it = normals.begin();

        for ( unsigned int whorlpos = 0; whorlpos < closedcircle; whorlpos++ ) {
            triangle( q, *it++, point, point, point, 0 );
        }

        matrix< float > end = origin + prod( metafocus, cY );

        for ( unsigned int n = 1; n < closedcircle; n++ ) {
            triangle( ( n - 1 ) % closedcircle, n % closedcircle, n, 0 );
        }
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
};
