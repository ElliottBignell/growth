///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///
/// Classes used to transform an initial cross-sectional curve
/// around a logarithmic spiral in 3 dimensions

#include <memory>
#include <vector>
#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include "boost/tuple/tuple.hpp"
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include "const.h"
#include "bezier.h"
#include "gaussian.h"
#include "porphyry.h"
#include "distnull.h"
#include "distbase.h"
#include "filecurve.h"
#include "filecurveo.h"
#include "whorl.h"
#include "whorldata.h"

using namespace std;
using namespace boost::numeric::ublas;

