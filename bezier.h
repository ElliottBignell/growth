//#include <fstream>
//#include <sstream>
//#include <string>
//
class bezier
{
private:
    matrix< float > transform;

public:
    bezier() 
        : transform( 4, 4 )
    {   
        static float transforms[] = {
            1, 0, 0, 0,
           -3, 3, 0, 0,
            3,-6, 3, 0,
           -1, 3,-3, 1
        };

        unsigned int n = 0;

        for ( unsigned int j = 0; j < 4; j++ ) {

            for ( unsigned int k = 0; k < 4; k++ ) {
                transform( j, k ) = transforms[ n++ ];
            }
        }
    }

    const matrix< float >& get() { return transform; }
};
