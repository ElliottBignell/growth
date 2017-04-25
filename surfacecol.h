#include "const.h"

class colour
{
protected:
    colour() {} 
    virtual ~colour() = 0;

public:
    virtual unsigned int operator()( float, float ) = 0;
};

colour::~colour()
{
}

class insidecol : public colour
{
public:
    virtual ~insidecol() {}

    virtual unsigned int operator()( float thetaX, float theta )
    {
        return 2;
    }
};

class peakcol : public colour
{
public:
    virtual ~peakcol() {}

    virtual unsigned int operator()( float thetaX, float theta )
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

    virtual unsigned int operator()( float thetaX, float theta )
    {
        //float wh  = 360.0 * theta  / ( 2.0 * pi );
        //float whX = 360.0 * thetaX / ( 2.0 * pi );

        //return ( (int)( thetaX * 10 ) % 20 ) > 1 ? 0 : 1;
        return ( (int)( theta * 20 ) % 5 ) < 4 ? 0 : 1;
    }
};

