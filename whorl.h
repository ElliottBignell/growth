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

typedef matrix< float > MF;

whorlData data;

static unsigned int cone = 10000;
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

public:
    shapeCurve( std::shared_ptr< meshfile >, const curveDef& );
    ~shapeCurve();

public:
    //! Draws the entire spiral
    void whorl();

    //! Draws the triangles to link two curves into a closed strip
    double stitchToCurve( std::vector< MF >&, std::vector< MF >&, float, unsigned int& );

private:
    //! Draws closed instances of the cross-sectional curve around a whole 360° loop
    void singleWhorl( double thetaL, distribution&, double& );

    //! Outputs a triangle to the mesh based on three points and a colour
    void triangle( unsigned int, unsigned int pos1, unsigned int pos2, unsigned int colour );

    //! Outputs a triangle and normal  to the mesh based on three points and a colour
    void triangle( meshpov::triangle&, const meshpov::normal&, unsigned int, unsigned int pos1, unsigned int pos2, unsigned int );

    //! Computes a line segment of the Bezier curve using matrix transformations
    void addStep( bezier &, MF& wallS, const unsigned int, const float );

    //! Fills the rotation matrices based on the thetaX, -Y and -Z values
    void initRotation( float );

    //! Fills the rotation matrices based on the thetaX, -Y and -Z values
    void initScale( float );

    //! Fills the rotation matrices based on the thetaX, -Y and -Z values
    void initTranslate( float );
};

#include "whorl.cpp"

#endif // __WHORL_H_INCLUDED_
