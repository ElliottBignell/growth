class pigmentmap
{
   const unsigned int width;
   const unsigned int length;

   std::vector< float > activation;

public:
    pigmentmap( unsigned int circle, unsigned int cone ) ;

    void pattern();
    unsigned int isOn( unsigned int x, unsigned int y );
};


