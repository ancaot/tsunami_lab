/**
 * @author Ward Tammaa
 *
 * @section DESCRIPTION
 * test unit for the SupercriticalFlow.
 **/
#include <catch2/catch.hpp>
#include "TsunamiEvent1d.h"

TEST_CASE( "Test the TsunamiEvent1d setup.", "[TsunamiEvent1d]" ) {
  tsunami_lab::setups::TsunamiEvent1d l_tsunamiEvent1d;

  // left side
  REQUIRE( l_tsunamiEvent1d.getHeight( 2, 0 ) == 20.0f );

  REQUIRE( l_tsunamiEvent1d.getMomentumX( 2, 0 ) == 0);

  REQUIRE( l_tsunamiEvent1d.getMomentumY( 2, 0 ) == 0 );

  REQUIRE( l_tsunamiEvent1d.getBathymetry( 2, 0 ) == -20.0f );



  REQUIRE( l_tsunamiEvent1d.getHeight( 2, 5 ) == 20.0f );

  REQUIRE( l_tsunamiEvent1d.getMomentumX( 2, 5 ) == 0);

  REQUIRE( l_tsunamiEvent1d.getMomentumY( 2, 2 ) == 0 );

  REQUIRE( l_tsunamiEvent1d.getBathymetry( 10, 0 ) ==  -20.0f);

  // right side



  REQUIRE( l_tsunamiEvent1d.getHeight( 10, 0 ) == 20.0f);

  REQUIRE( l_tsunamiEvent1d.getMomentumX( 4, 0 ) == 0);

  REQUIRE( l_tsunamiEvent1d.getMomentumY( 4, 0 ) == 0 );

  REQUIRE( l_tsunamiEvent1d.getBathymetry( 2, 0 ) == -20.0f );



  REQUIRE( l_tsunamiEvent1d.getHeight( 4, 5 ) == 20.0f );

  REQUIRE( l_tsunamiEvent1d.getMomentumX( 4, 5 ) == 0);

  REQUIRE( l_tsunamiEvent1d.getMomentumY( 4, 2 ) == 0 );  

  REQUIRE( l_tsunamiEvent1d.getBathymetry( 10, 0 ) == -20.0f );

}