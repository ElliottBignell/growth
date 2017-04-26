///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///

#include "const.h"

///The surface class is the base class for surface contour classes
///!The surface class is the base class for surface contour classes.
///Specialisations of the base class implement contours to adjust the geometry
///of generated shapes. These classes are simple operators with a function-call
///member taking a triangle, normal and two angles for X and Y. Based on these they return an adjusted  
///point value representing a change in the height or other position of the
/// input position. For instance, a ridge could be added at particular spacings alonf the surface.
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

///The outside class is the operator defining the shell outer surface
/*!The outside class is the operator defining any ridges and other features 
on the shell's outer surface
*/
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

///The inside class is the operator defining the shell inner surface
///!The inside class is the operator defining the ridges or any other features 
///on the shell's inner surface
class inside : public surface
{
public:
    virtual ~inside() {}

    virtual matrix< float > operator()( meshfile::triangle& point, meshfile::normal&, float thetaX, float theta )
    {
        meshfile::normal bump( point );

        bump *= 0.005 - 
                pow( 
                    pow( ( (int)( theta  * 60.0 ) % 15 ), 2.0 )
                  + pow( ( (int)( thetaX * 60.0 ) % 15 ), 2.0 )
                  , 0.5
                  )
             * 0.0025;

        return point + bump;

        //float wh  = 360.0 * theta  / ( 2.0 * pi );
        //float whX = 360.0 * thetaX / ( 2.0 * pi );

//        bump *= 0.005 + sin( wh  / 12.0 * pi ) * 0.04
//                      + sin( whX / 6.0  * pi ) * 0.02;

                //( (int)theta  % 8 ) * 0.4;

    }
};
