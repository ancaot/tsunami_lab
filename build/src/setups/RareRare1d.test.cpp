/**
 * @section DESCRIPTION
 * Tests the rare-rare setup.
 **/
#include <catch2/catch.hpp>
#include "RareRare1d.h"

TEST_CASE( "Test the one-dimensional rare-rare setup.", "[RareRare1d]" ) {
	tsunami_lab::setups::RareRare1d l_rareRare( 10,
																							3,
																							5 );

	// left side: q_r = [h, -h*u]
	REQUIRE( l_rareRare.getHeight( 4, 0 ) == Approx(10) );
	REQUIRE( l_rareRare.getMomentumX( 4, 0 ) == Approx(-30) );
	REQUIRE( l_rareRare.getMomentumY( 4, 0 ) == Approx(0) );

	// right side: q_l = [h, +h*u]
	REQUIRE( l_rareRare.getHeight( 6, 0 ) == Approx(10) );
	REQUIRE( l_rareRare.getMomentumX( 6, 0 ) == Approx(30) );
	REQUIRE( l_rareRare.getMomentumY( 6, 0 ) == Approx(0) );
}
