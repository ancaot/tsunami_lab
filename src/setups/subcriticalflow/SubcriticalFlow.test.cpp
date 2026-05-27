/**
 * @author Ward Tammaa 
 *
 * @section DESCRIPTION
 * Tests SubcriticalFlow.
 **/
#include <catch2/catch.hpp>
#include "SubcriticalFlow.h"

TEST_CASE( "Test the Subcritical flow setup.", "[SubcriticalFlow]" ) {
  tsunami_lab::setups::SubcriticalFlow l_subcriticalFlow;

  // left side
  REQUIRE( l_subcriticalFlow.getHeight( 2, 0 ) == 2 );

  REQUIRE( l_subcriticalFlow.getMomentumX( 2, 0 ) == 4.42f);

  REQUIRE( l_subcriticalFlow.getMomentumY( 2, 0 ) == 0 );

  REQUIRE( l_subcriticalFlow.getBathymetry( 2, 0 ) == -2 );



  REQUIRE( l_subcriticalFlow.getHeight( 2, 5 ) == 2 );

  REQUIRE( l_subcriticalFlow.getMomentumX( 2, 5 ) == 4.42f);

  REQUIRE( l_subcriticalFlow.getMomentumY( 2, 2 ) == 0 );

  REQUIRE( l_subcriticalFlow.getBathymetry( 10, 0 ) == -1.8f );

  // right side



  REQUIRE( l_subcriticalFlow.getHeight( 10, 0 ) == 1.8f);

  REQUIRE( l_subcriticalFlow.getMomentumX( 4, 0 ) == 4.42f);

  REQUIRE( l_subcriticalFlow.getMomentumY( 4, 0 ) == 0 );

  REQUIRE( l_subcriticalFlow.getBathymetry( 2, 0 ) == -2.0f );




  REQUIRE( l_subcriticalFlow.getHeight( 4, 5 ) == 2 );

  REQUIRE( l_subcriticalFlow.getMomentumX( 4, 5 ) == 4.42f);

  REQUIRE( l_subcriticalFlow.getMomentumY( 4, 2 ) == 0 );  

  REQUIRE( l_subcriticalFlow.getBathymetry( 10, 0 ) == -1.8f );

}