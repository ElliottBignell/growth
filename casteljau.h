#include "shapes.h"
#include <list>

class Bezier
{
    typedef pair< MF&, MF& > Line;

    MF P1{ 1, 4 };   // Begin Point
    MF P2{ 1, 4 };   // Control Point
    MF P3{ 1, 4 };   // Control Point
    MF P4{ 1, 4 };   // End Point

    MF P12{ 1, 4 };
    MF P23{ 1, 4 };
    MF P34{ 1, 4 };

    MF P123{ 1, 4 };
    MF P234{ 1, 4 };

    MF P1234{ 1, 4 };

public:
    Bezier( MF p1, MF p2, MF p3, MF p4 )
    {
        P1 = p1; P2 = p2; P3 = p3; P4 = p4;
    }

    /// <summary>
    /// Consider the classic Casteljau diagram
    /// with the bezier points p1, p2, p3, p4 and lines l12, l23, l34
    /// and their midpoint of line l12 being p12 ...
    /// and the line between p12 p23 being L1223
    /// and the midpoint of line L1223 being P1223 ...
    /// </summary>
    /// <param name="lines"></param>
    void SplitBezier( std::list<Line> lines )
    {
        Line L12( P1, P2 );
        Line L23( P2, P3 );
        Line L34( P3, P4 );

        P12 = MidPoint( L12 );
        P23 = MidPoint( L23 );
        P34 = MidPoint( L34 );

        Line L1223( P12, P23 );
        Line L2334( P23, P34 );

        P123 = MidPoint( L1223 );
        P234 = MidPoint( L2334 );

        Line L123234(  P123, P234 );
        MF P1234( MidPoint( L123234 ) );

        if ( CurveIsFlat() )
        {
            lines.push_back( Line(P1, P4) );
            return;
        }
        else
        {
            Bezier(P1, P12, P123, P1234).SplitBezier(lines);
            Bezier(P1234, P234, P34, P4).SplitBezier(lines);
        }

        return;
    }

    /// <summary>
    /// Check if points P1, P1234 and P2 are colinear (enough).
    /// This is very simple-minded algo... there are better...
    /// </summary>
    /// <returns></returns>
    bool CurveIsFlat()
    {
        float t1 = ( P2( 0, 1 ) - P1( 0, 1 ) ) * ( P3( 0, 0 ) - P2( 0, 0 ) );
        float t2 = ( P3( 0, 1 ) - P2( 0, 1 ) ) * ( P2( 0, 0 ) - P1( 0, 0 ) );

        float delta = abs( t1 - t2 );

        return delta < 0.1; // Hard-coded constant
    }

    void main() {

        MF p1, p2, p3, p4;

        std::list< Line > lines;
        Bezier( p1, p2, p3, p4 ).SplitBezier( lines );
    }

private:
    MF MidPoint( Line l ) 
    {
        MF p1( l.first );
        MF p2( l.second );

        return MF(( p1( 0, 0 ) + p2( 0, 0 ) ) / 2.0f, ( p1( 0, 1 ) + p2( 0, 1 ) ) / 2.0f );
    }
};
