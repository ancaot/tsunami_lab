/**
 * @section DESCRIPTION
 * Tests the shock-shock setup.
 **/
#include <catch2/catch.hpp>
#include "ShockShock1d.h"

TEST_CASE( "Test the one-dimensional shock-shock setup.", "[ShockShock1d]" ) {
  tsunami_lab::setups::ShockShock1d l_shockShock( 10,
                                                  3,
                                                  5 );

  // left side: q_l = [h, +h*u]
	REQUIRE( l_shockShock.getHeight( 4, 0 ) == Approx(10) );
	REQUIRE( l_shockShock.getMomentumX( 4, 0 ) == Approx(30) );
	REQUIRE( l_shockShock.getMomentumY( 4, 0 ) == Approx(0) );

	// right side: q_r = [h, -h*u]
	REQUIRE( l_shockShock.getHeight( 6, 0 ) == Approx(10) );
	REQUIRE( l_shockShock.getMomentumX( 6, 0 ) == Approx(-30) );
	REQUIRE( l_shockShock.getMomentumY( 6, 0 ) == Approx(0) );
}