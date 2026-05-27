/**
 * @author Ward Tammaa 
 *
 * @section DESCRIPTION
 * Tests the Rare Rare setup.
 **/

#include <catch2/catch.hpp>
#include "RareRare.h"

TEST_CASE( "Test the rare-rare setup.", "[RareRare]" ) {
  tsunami_lab::setups::RareRare l_rareRare( 25,55,3);

  // left side
  REQUIRE( l_rareRare.getHeight(    2, 0 ) == 25 );
  REQUIRE( l_rareRare.getMomentumX( 2, 0 ) == -55 );
  REQUIRE( l_rareRare.getMomentumY( 2, 0 ) == 0 );

  REQUIRE( l_rareRare.getHeight(  3, 0 ) == 25 );
  REQUIRE( l_rareRare.getMomentumX( 3, 0 ) == -55 );
  REQUIRE( l_rareRare.getMomentumY( 3, 0 ) == 0 );


  // right side
  REQUIRE( l_rareRare.getHeight(    5, 0 ) == 25 );
  REQUIRE( l_rareRare.getMomentumX( 5, 0 ) == 55 );
  REQUIRE( l_rareRare.getMomentumY( 5, 0 ) == 0 );

  REQUIRE( l_rareRare.getHeight(    60, 0 ) == 25 );
  REQUIRE( l_rareRare.getMomentumX( 60, 0 ) == 55 );
  REQUIRE( l_rareRare.getMomentumY( 60, 0 ) == 0 );
}