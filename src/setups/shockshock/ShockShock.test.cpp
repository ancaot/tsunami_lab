/**
 * @author Ward Tammaa 
 *
 * @section DESCRIPTION
 * Tests the Shock Shock setup.
 **/
#include <catch2/catch.hpp>
#include "ShockShock.h"

TEST_CASE( "Test the shock-shock setup.", "[ShockShock]" ) {
  tsunami_lab::setups::ShockShock l_shockShock( 25, 55, 3 );

  // left side
  REQUIRE( l_shockShock.getHeight(    2, 0 ) == 25 );
  REQUIRE( l_shockShock.getMomentumX( 2, 0 ) == 55 );
  REQUIRE( l_shockShock.getMomentumY( 2, 0 ) == 0 );

  REQUIRE( l_shockShock.getHeight(  3, 0 ) == 25 );
  REQUIRE( l_shockShock.getMomentumX( 3, 0 ) == 55 );
  REQUIRE( l_shockShock.getMomentumY( 3, 0 ) == 0 );


  // left side
  REQUIRE( l_shockShock.getHeight(    5, 0 ) == 25 );
  REQUIRE( l_shockShock.getMomentumX( 5, 0 ) == -55 );
  REQUIRE( l_shockShock.getMomentumY( 5, 0 ) == 0 );

  REQUIRE( l_shockShock.getHeight(    60, 0 ) == 25 );
  REQUIRE( l_shockShock.getMomentumX( 60, 0 ) == -55 );
  REQUIRE( l_shockShock.getMomentumY( 60, 0 ) == 0 );
}