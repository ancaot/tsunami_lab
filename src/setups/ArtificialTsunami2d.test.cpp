/**
 * @section DESCRIPTION
 * Tests for the artifical tsunami setup
 **/

#include "ArtificialTsunami2d.h"

#include <catch2/catch.hpp>

TEST_CASE( "Test the artificial tsunami setup.", "[ArtificialTsunami2d]" ) {
  tsunami_lab::setups::ArtificialTsunami2d l_setup(10, 0);

  REQUIRE( l_setup.getHeight( 0.0f, 0.0f ) == Approx( 10.0f ) );
  REQUIRE( l_setup.getHeight( 10.0f, 0.0f ) == Approx( 10.0f ) );
  REQUIRE( l_setup.getBathymetry( 10.0f, 10.0f ) == -Approx( 0.313827f ).margin(1e-4f) );
  REQUIRE( l_setup.getBathymetry( 1.0f, 1.0f ) == -Approx( 0.03142f ).margin(1e-4f) );
}