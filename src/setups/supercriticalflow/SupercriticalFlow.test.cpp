/**
 * @author Ward Tammaa
 *
 * @section DESCRIPTION
 * test unit for the SupercriticalFlow.
 **/
#include <catch2/catch.hpp>
#include "SupercriticalFlow.h"

TEST_CASE( "Test the SupercriticalFlow setup.", "[SupercriticalFlow]" ) {
  tsunami_lab::setups::SupercriticalFlow l_supercriticalFlow;

  // left side
  REQUIRE( l_supercriticalFlow.getHeight( 2, 0 ) == 0.33f );

  REQUIRE( l_supercriticalFlow.getMomentumX( 2, 0 ) == 0.18f);

  REQUIRE( l_supercriticalFlow.getMomentumY( 2, 0 ) == 0 );

  REQUIRE( l_supercriticalFlow.getBathymetry( 2, 0 ) == -0.33f );



  REQUIRE( l_supercriticalFlow.getHeight( 2, 5 ) == 0.33f );

  REQUIRE( l_supercriticalFlow.getMomentumX( 2, 5 ) == 0.18f);

  REQUIRE( l_supercriticalFlow.getMomentumY( 2, 2 ) == 0 );

  REQUIRE( l_supercriticalFlow.getBathymetry( 10, 0 ) == -0.13f );

  // right side



  REQUIRE( l_supercriticalFlow.getHeight( 10, 0 ) == 0.13f);

  REQUIRE( l_supercriticalFlow.getMomentumX( 4, 0 ) == 0.18f);

  REQUIRE( l_supercriticalFlow.getMomentumY( 4, 0 ) == 0 );

  REQUIRE( l_supercriticalFlow.getBathymetry( 2, 0 ) == -0.33f );




  REQUIRE( l_supercriticalFlow.getHeight( 4, 5 ) == 0.33f );

  REQUIRE( l_supercriticalFlow.getMomentumX( 4, 5 ) == 0.18f);

  REQUIRE( l_supercriticalFlow.getMomentumY( 4, 2 ) == 0 );  

  REQUIRE( l_supercriticalFlow.getBathymetry( 10, 0 ) == -0.13f );

}