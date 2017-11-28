///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///

#include "const.h"
#include "porphyry.h"

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

ostream& operator<<( ostream& os, const state s )
{
    if ( 4 == s.get< 1 >() ) {

        if ( s.get< 0 >() == 1 ) {
            os << "texture { defTexture3" ;
        }
        else if ( s.get< 0 >() == 0 ) {
            os << "texture { defTexture2" ;
        }
        else {
            os << "texture { defTexture1" ;
        }
    }
    else {

        switch ( s.get< 0 >() ) {
        case 1:
        case 2:

            switch ( s.get< 3 >() % 2 ) {
            case 0:
                if ( s.get< 1 >() == 1 ) {
                    os << "texture { defTexture3" ;
                }
                else if ( s.get< 1 >() == 0 ) {
                    os << "texture { defTexture2" ;
                }
                else {
                    os << "texture { defTexture1" ;
                }
                break;
            case 1:
                if ( s.get< 1 >() == 1 ) {
                    os << "texture { defTexture3" ;
                }
                else if ( s.get< 1 >() != 0 ) {
                    os << "texture { defTexture2" ;
                }
                else {
                    os << "texture { defTexture1" ;
                }
                break;
            }

            break;
        case 3:
            os << "texture { insideTexture" ;
            break;
        }
    }

    os << " }"
       << endl;

    return os;
}

ostream& operator<<( ostream& os, const std::vector< matrix< float > >::reference p )
{
    os << "<" 
       << p( 0, 0 )
       << ","
       << p( 0, 1 )
       << ","
       << p( 0, 2 )
       << ">";

    return os;
}

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

colour::~colour()
{
}

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
