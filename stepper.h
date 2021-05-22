#include "shapes.h"
#include "bezier.h"

class stepperBase {
protected:
    shapes& parent;
    bezier& bz;
    MF rZ;
    unsigned int segments{ 0 };

public:
    std::vector < MF > wallPoints;

    virtual ~stepperBase() {}

    stepperBase( shapes& p, bezier& b )
            : parent( p )
            , bz( b )
    {

    }

    virtual void rotateDefiningPointŝ(  MF&, const unsigned int, const float ) = 0;
    virtual void addStep( const unsigned int )                                 = 0;
    virtual void portionUpWhorl( const curveDef& )                             = 0;
    virtual unsigned int getPortionCOunt()                                     = 0;
};

class stepperPolyBezier : public stepperBase {
public:
    virtual ~stepperPolyBezier() {}

    stepperPolyBezier( shapes& p, bezier& b ) : stepperBase( p, b ) {}

    virtual void rotateDefiningPointŝ(  MF&, const unsigned int, const float );
    virtual void addStep( const unsigned int );
    virtual void portionUpWhorl( const curveDef& );
    virtual unsigned int getPortionCOunt() { return segments; }
};

class stepperCompoundBezier : public stepperBase {
public:
    virtual ~stepperCompoundBezier() {}

    stepperCompoundBezier( shapes& p, bezier& b ) : stepperBase( p, b ) {}

    virtual void rotateDefiningPointŝ(  MF&, const unsigned int, const float );
    virtual void addStep( const unsigned int );
    virtual void portionUpWhorl( const curveDef& );
    virtual unsigned int getPortionCOunt() { return 1; }

private:
    float nBasedBezier( unsigned int, float, shapes::col&, unsigned int );
    float mix(float, float, float );
};
