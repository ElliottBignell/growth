///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///

/// Models for the simulations of the color pattern on the shells of mollusks
/// see also Meinhardt,H. and Klingler,M. (1987) J. theor. Biol 126, 63-69
/// see also H.Meinhardt "Algorithmic beauty of sea shells"
/// (Springer Verlag) (c) H.Meinhardt, Tübingen

///This is a short version of a program for the simulations of the color
///patterns on tropical sea shells, here //Oliva porphyria'.
///An autocatalytic activator a(i) leads to a burst-like activation
///that is regulated back by the action of an inhibitor b(i). The life
///time of the inhibitor is regulated via a hormone c, that is
///homogeneously distributed along the growing edge. Whenever the number
///of activated cells cells become too small, active cells remain activated
///until backwards waves are triggered

///The program runs with the interpreter QBASIC, but this is very slow.
///Better are the compiler Power Basic, Microsoft QB 4.5, Professional
///Basic 7.1, and Visual Basic für DOS. A compiled version is included

#include <iostream>
#include <boost/math/special_functions/fpclassify.hpp>
#include "const.h"
#include "gaussian.h"
#include "porphyry.h"

using namespace std;

// i = 1...kx < imax = cells at the growing edge

pigmentmap::pigmentmap( unsigned int circle, unsigned int cone ) 
    : width( circle )
    , length( cone / width )
    , activation( width * length, 0 )
{
    std::cout << "Pattern width: " << width << std::endl;
    std::cout << "Pattern length: " << length << std::endl;
    pattern();
}

unsigned int pigmentmap::isOn( float x, float y ) 
{ 
    auto X = x / (2.0 * pi) * width;
    auto Y = y / (2.0 * pi) * length;
    auto index = X + Y * width;

    if ( index >= activation.size() ) {

        //std::cout << X << " " << Y << " " << x << " " << y << " " << activation.size() << std::endl;
        return 0;
    }

    if ( activation[ index ] > 0.5 ) {
        return 2;
    }
    else if ( activation[ index ] > 0.25 ) {
        return 1;
    }
    else {
        return 0;
    }
}

void pigmentmap::pattern()
{
    static gaussian gauss( 0.5, 0.5 );
    const int imax = width * 2;

    float ax[imax], bx[imax], zx[imax];
    //    RANDOMIZE TIMER; // By different fluctuations,
    //simulation will be slightly different
    //int KT = 460;  //Number of displays
    //          KT * KP = number of iterations in total
    int KP = 12;   //number of iterations between the displays ( = lines on the screen);
    unsigned int kx = width;  //number of cells
    //int dx = 1;    //width of a cell in pixel;   with kp=6 ; kx=315 and dx=2 =>
    //                        simulation in a smaller field
    float DA = 0.015; //Diffusion of the activator
    float RA = 0.1;   //Decay rate of the inhibitor
    //float BA = 0.1;   //Basic production of the activator
    float SA = 0.25;  //Saturation of the autocatalysis
    float DB = 0;    //Diffusion of the inhibitor
    float RB = 0.014; //Decay rate of the inhibitor
    float SB = 0.1;   //Michaelis-Menten constant of the inhibition
    float RC = 0.1;   //Decay rate of the hormone

    unsigned int i = 0;

    memset( bx, 0, sizeof( bx ) );
    memset( zx, 0, sizeof( zx ) );

    //  ----------- Initial condition  --------------------------
    for ( i = 0; i < kx; i++ ) {

        ax[ i ] = 0;    //Activator, general initial concentration
        bx[ i ] = 0.1;   //Inhibitor, general initial concentration
        zx[ i ] = RA * (0.96 + 0.08 * gauss( 0, 0 ) );//Fluctuation of the autocatalysis
    }

    float C = 0.5; //Hormone-concentration, homogeneous in all cells
    i = 10;

    while ( i < width ) { //initially active cells 

        ax[ i ] = 1;
        i = i + 5.0 * gauss( 0, 0 ) + 10.0;
    }

    static const float DAC = 1.0 - RA - 2.0 * DA; // These constant factors are used again and again
    static const float DBC = 1.0 - RB - 2.0 * DB; // therefore, they are calculated only once
    float DBCC = DBC;//             ' at the begin of the calculation

    //int x0 = 5;
    //int y1 = 475;       //Initial position of the first line
    unsigned int itot = 0;       //Number of iteration calculated so far

    vector< float >::iterator it = activation.begin();

    for ( itot = 0; itot < length; itot++ ) {

        for (  int iprint = 0; iprint < KP; iprint++ ) { // Begin of the iteration 

            //  -----  --- Boundary impermeable
            float A1 = ax[ 0 ]; //    a1 is the concentration  of the actual cell. since this
            float B1 = bx[ 0 ]; //    concentration is identical, no diffusion through the border.
            ax[ kx + 1 ] = ax[ kx ]; //          Concentration in a virtual right cell
            bx[ kx + 1 ] = bx[ kx ];
            float BSA = 0;  //    This will carry the sum of all axs of all cells

            //  ---------- Reactions  ------
            for ( i = 0; i < kx; i++ ) {// i = actual cell, kx = right cell; 

                float AF = ax[ i ]; //local activator concentration
                float BF = bx[ i ]; //local inhibitor concentration
                float AQ = zx[ i ] * AF * AF / (1.0 + SA * AF * AF);  //Saturating autocatalysis
                // Calculation of the new activator and inhibitor concentration in cell i

                ax[ i ] = AF * DAC + DA * (A1 + ax[ i + 1 ]) + AQ / (SB + BF);
                // 1/BF => Action of the inhibitor; SB = Michaelis-Menten constant
                bx[ i ] = BF * DBCC + DB * (B1 + bx[ i + 1 ]) + AQ; //new inhibitor conc.
                BSA = BSA + RC * AF; //Hormone production -> Sum of axs
                A1 = AF; //    actual concentration will be concentration in left cell
                B1 = BF; //    in the concentration change of the next cell
            }

            C = C * (1.0 - RC) + BSA / kx; // New hormone concentration , 1/kx=normalization
            float RBB = RB / C;                 // on total number of cells
            //RBB => Effective life time of the inhibitor

            DBCC = 1.0 - 2.0 * DB - RBB;      // Change in a cell by diffusion
        }

        // lifetime of the inhibitor changes
        for ( unsigned int ix = 0; ix < kx; ix++ ) {  //Pigment is drawn when a is higher than a threshold 
            *it++ = ax[ ix ];
        }

//             if ( ax[ ix ] > 0.25 ) {
//                 cout << "1";
//             }
//             else {
//                 cout << " ";
//             }
//         cout << endl;
    }
}


//    class pigmentmap_
//    {
//       const unsigned int width;
//       const unsigned int length;
//    
//       std::vector< cell > activation;
//    
//    public:
//        pigmentmap_() 
//            : width( circle )
//            , length( cone / width )
//            , activation( width * length )
//        {
//            for_each( activation.begin(), activation.end(), std::mem_fun_ref( &cell::reset ) );
//    
//            std::vector< cell >::iterator it = activation.begin();
//    
//            for ( unsigned int y = 0; y < length; y++ ) {
//    
//                std::vector< cell >::iterator itcopy = it;
//    
//                for_each( it, it + circle, std::mem_fun_ref( &cell::trigger ) );
//                for_each( it, it + circle, std::mem_fun_ref( &cell::inhibit ) );
//                for_each( it, it + circle, std::mem_fun_ref( &cell::produce ) );
//    
//                it += circle;
//    
//    //            cell::probability /= shrink;
//    
//                if ( y < length - 1 ) {
//    
//                    std::vector< cell >::iterator itold  = it;
//    
//                    for ( unsigned int x = 0; x < circle; x++ ) {
//    
//                        *itold = *itcopy;
//    
//                        itold->concentration = ( 
//                            ( itcopy - 1 )->concentration + 
//                              itcopy      ->concentration + 
//                            ( itcopy + 1 )->concentration 
//                        ) / 3.0;
//    
//                        itold++;
//                        itcopy++;
//                    }
//                }
//    
//            }
//        }
//    
//        bool isOn( unsigned int x, unsigned int y ) { return activation[ ( length - y ) * ( circle - 1 ) + x ].active > 3; }
//    
//        void dump( unsigned int x, unsigned int y ) 
//        {
//            ( activation.begin() + y * circle + x )->dump();
//            cout << endl;
//        }
//    
//        void dump() 
//        {
//            std::vector< cell >::iterator it = activation.begin();
//    
//            for ( unsigned int y = 0; y < length; y++ ) {
//    
//                for ( unsigned int x = 0; x < circle; x++ ) {
//                    it++->dump();
//                }
//                cout << endl;
//            }
//        }
//    };
