///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///
/// Classes used to transform an initial cross-sectional curve
/// around a logarithmic spiral in 3 dimensions

template < typename SURF, typename COLOUR >
void shapeCurve< SURF, COLOUR >::initRotation()
{
    rotateX( 0, 0 ) =              1;
    rotateX( 1, 1 ) =  cos( thetaX );
    rotateX( 2, 1 ) = -sin( thetaX );
    rotateX( 1, 2 ) =  sin( thetaX );
    rotateX( 2, 2 ) =  cos( thetaX );
    rotateY( 1, 1 ) =              1;
    rotateY( 0, 0 ) =  cos( thetaY );
    rotateY( 2, 0 ) =  sin( thetaY );
    rotateY( 0, 2 ) = -sin( thetaY );
    rotateY( 2, 2 ) =  cos( thetaY );
    rotateZ( 2, 2 ) =              1;
    rotateZ( 0, 0 ) =  cos( thetaZ );
    rotateZ( 0, 1 ) = -sin( thetaZ );
    rotateZ( 1, 0 ) =  sin( thetaZ );
    rotateZ( 1, 1 ) =  cos( thetaZ );

    //MF rotateNormal( 3, 3 );

    //rotateNormal( 2, 2 ) =              1;
    //rotateNormal( 0, 0 ) =  cos( pi / 2.0 );
    //rotateNormal( 0, 1 ) = -sin( pi / 2.0 );
    //rotateNormal( 1, 0 ) =  sin( pi / 2.0 );
    //rotateNormal( 1, 1 ) =  cos( pi / 2.0 );
}
