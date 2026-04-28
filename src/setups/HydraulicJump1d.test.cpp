/**
 * @section DESCRIPTION
 * Tests the hydraulic jump setups.
 **/

#include "HydraulicJump1d.h"

#include <catch2/catch.hpp>

TEST_CASE( "Test the subcritical hydraulic jump setup.", "[HydraulicJumpSubcritical1d]" ) {
  tsunami_lab::setups::HydraulicJumpSubcritical1d l_setup;

  REQUIRE( l_setup.getHeight( 10.0f, 0.0f ) == Approx( 1.8f ) );
  REQUIRE( l_setup.getHeight( 0.0f, 0.0f ) == Approx( 2.0f ) );
  REQUIRE( l_setup.getMomentumX( 0.0f, 0.0f ) == Approx( 4.42f ) );
  REQUIRE( l_setup.getMomentumY( 0.0f, 0.0f ) == Approx( 0.0f ) );
}

TEST_CASE( "Test the supercritical hydraulic jump setup.", "[HydraulicJumpSupercritical1d]" ) {
  tsunami_lab::setups::HydraulicJumpSupercritical1d l_setup;

  REQUIRE( l_setup.getHeight( 10.0f, 0.0f ) == Approx( 0.13f ) );
  REQUIRE( l_setup.getHeight( 0.0f, 0.0f ) == Approx( 0.33f ) );
  REQUIRE( l_setup.getMomentumX( 0.0f, 0.0f ) == Approx( 0.18f ) );
  REQUIRE( l_setup.getMomentumY( 0.0f, 0.0f ) == Approx( 0.0f ) );
}