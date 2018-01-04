#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include "shapes.h"

using namespace boost::numeric::ublas;

matrix< float > shapes::shape;
matrix< float > shapes::record;
std::vector< matrix< float > > shapes::normals;

float shapes::R{ 2.5 };
float shapes::r{ 1.0 };

shapes::~shapes()
{
}
