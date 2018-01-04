///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///

#ifndef __SURFACECOLO_H_INCLUDED__
#define __SURFACECOLO_H_INCLUDED__

#include "const.h"
#include "porphyry.h"

using namespace std;
using namespace boost::numeric::ublas;

typedef boost::tuple< unsigned int, unsigned int, unsigned int, unsigned int > state_nums;

template < typename T >
class State
{
    T n;

public:
    State( T m ) : n( m   ) {}
    State( const State& s ) : n( s.n ) {}

    template < unsigned int N >
    unsigned int get() const { return boost::get< N >( n ); }

    State& operator++() { boost::get< 0 >( n )++; return *this; }
};

typedef State< state_nums > state;
typedef State< matrix< float > > mstate;

ostream& operator<<( ostream& os, const state s );
ostream& operator<<( ostream& os, const std::vector< matrix< float > >::reference p );

class colour
{
protected:
    std::shared_ptr< pigmentmap > pm;

    colour() {} 
    virtual ~colour() = 0;

public:
    virtual unsigned int operator()( float, float ) = 0;

    std::shared_ptr< pigmentmap > getPigmentMap() const { return pm; }
    void setPigmentMap( std::shared_ptr< pigmentmap > p ) { pm = p; }
};

class insidecol : public colour
{
public:
    virtual ~insidecol() {}

    virtual unsigned int operator()( float /*thetaX*/, float /*theta*/ )
    {
        return 2;
    }
};

class peakcol : public colour
{
public:
    virtual ~peakcol() {}

    virtual unsigned int operator()( float /*thetaX*/, float theta )
    {
        //float wh  = 360.0 * theta  / ( 2.0 * pi );
        //float whX = 360.0 * thetaX / ( 2.0 * pi );

        //return ( (int)( thetaX * 100 ) % 20 ) > 10 ? 0 : 1;
        return ( (int)( theta * 20 ) % 5 ) < 4 ? 0 : 1;
    }
};

class troughcol : public colour
{
public:
    virtual ~troughcol() {}

    virtual unsigned int operator()( float /*thetaX*/, float theta )
    {
        //float wh  = 360.0 * theta  / ( 2.0 * pi );
        //float whX = 360.0 * thetaX / ( 2.0 * pi );

        //return ( (int)( thetaX * 10 ) % 20 ) > 1 ? 0 : 1;
        return ( (int)( theta * 20 ) % 5 ) < 4 ? 0 : 1;
    }
};

class wedgescol : public colour
{
    int n{ 0 };

public:
    virtual ~wedgescol() {}

    virtual unsigned int operator()( float thetaX, float theta )
    {
        if ( pm ) {

            int st = pm->isOn( thetaX, theta );

            switch ( st ) {
            case 2:
                return 0x00; //state( state_nums( 2, 4, wh, whX ) ).get();
            case 1:
                return 0xFF0000; //state( state_nums( 1, 4, wh, whX ) ).get();
            default:
                return 0xFFFFFF; //state( state_nums( 0, 4, wh, whX ) ).get();
            }
        }

        return 0xFFFF;
    }
};

#endif //__SURFACECOLO_H_INCLUDED__
