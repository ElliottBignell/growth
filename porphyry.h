///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///

class pigmentmap
{
   const unsigned int width;
   const unsigned int length;

   std::vector< float > activation;

public:
    pigmentmap( unsigned int circle, unsigned int cone ) ;

    void pattern();
    unsigned int isOn( float x, float y );
};


