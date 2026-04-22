/**
 * @section DESCRIPTION
 * Tests the shock-shock setup.
 **/
#include <catch2/catch.hpp>
#include "ShockShock1d.h"

TEST_CASE( "Test the one-dimensional shock-shock setup.", "[ShockShock1d]" ) {
  tsunami_lab::setups::DamBreak1d l_damBreak( 10,
                                              3,
                                               5 );

  // left side: q_l = [h, +h*u]
	REQUIRE( l_rareRare.getHeight( 4, 0 ) == Approx(10) );
	REQUIRE( l_rareRare.getMomentumX( 4, 0 ) == Approx(30) );
	REQUIRE( l_rareRare.getMomentumY( 4, 0 ) == Approx(0) );

	// right side: q_r = [h, -h*u]
	REQUIRE( l_rareRare.getHeight( 6, 0 ) == Approx(10) );
	REQUIRE( l_rareRare.getMomentumX( 6, 0 ) == Approx(-30) );
	REQUIRE( l_rareRare.getMomentumY( 6, 0 ) == Approx(0) );  
}