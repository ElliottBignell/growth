#include "const.h"

class surface
{
protected:
    surface() {} 
    virtual ~surface() = 0;

public:
    virtual matrix< float > operator()( meshfile::triangle&, meshfile::normal&, float, float ) = 0;
};

surface::~surface()
{
}

class outside : public surface
{
public:
    virtual ~outside() {}

    virtual matrix< float > operator()( meshfile::triangle& point, meshfile::normal& normals, float, float )
    {
        meshfile::normal bump( normals );

        bump( 0, 0 ) = 0.0;
        bump( 0, 1 ) = 0.0;
        bump( 0, 2 ) = 0.0;

        return point + bump;
    }
};

class inside : public surface
{
public:
    virtual ~inside() {}

    virtual matrix< float > operator()( meshfile::triangle& point, meshfile::normal&, float thetaX, float theta )
    {
        meshfile::normal bump( point );

        bump *= ( (int)( theta * 20.0 ) % 5 ) * 0.0; //25;

        return point + bump;

        //float wh  = 360.0 * theta  / ( 2.0 * pi );
        //float whX = 360.0 * thetaX / ( 2.0 * pi );

//        bump *= 0.005 + sin( wh  / 12.0 * pi ) * 0.04
//                      + sin( whX / 6.0  * pi ) * 0.02;

                //( (int)theta  % 8 ) * 0.4;

    }
};
