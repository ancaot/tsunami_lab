/**
 * @author
 *
 * @section DESCRIPTION
 * Unit tests for the one-dimensional wave propagation patch.
 *
 * With possibility to switch between Roe and F-Wave solvers
 *
 **/
#include "../macros.h"
#include "../../submodules/Catch2/include/catch.hpp"
//#include <catch2/catch.h>
#include "WavePropagation1d.h"


  //fwave test case
TEST_CASE( "Test the 1d wave propagation solver.", "[WaveProp1d]" ) {

  // construct solver and setup a dambreak problem
  tsunami_lab::patches::WavePropagation1d m_waveProp( 100 );

  switch(MODE) {
    case MODEFWAVE:
      /*
      * Test case for Roe solver:
      *
      *   Single dam break problem between cell 49 and 50.
      *     left | right
      *       10 | 8
      *        0 | 0
      *
      *   Elsewhere steady state.
      *
      * The net-updates at the respective edge are given as
      * (see derivation in Roe solver):
      *    left                 | right
      *      9.39467851749      |  -9.39467851749
      *    −88.259917223468     | −88.259917223468
      */

      for( std::size_t l_ce = 0; l_ce < 50; l_ce++ ) {
        m_waveProp.setHeight( l_ce,
                              0,
                              10 );
        m_waveProp.setMomentumX( l_ce,
                                0,
                                0 );
      }
      for( std::size_t l_ce = 50; l_ce < 100; l_ce++ ) {
        m_waveProp.setHeight( l_ce,
                              0,
                              8 );
        m_waveProp.setMomentumX( l_ce,
                                0,
                                0 );
      }

      // set outflow boundary condition
      m_waveProp.setGhostOutflow();

      // perform a time step
      m_waveProp.timeStep( 0.1 );

      // steady state
      for( std::size_t l_ce = 0; l_ce < 49; l_ce++ ) {
        REQUIRE( m_waveProp.getHeight()[l_ce]   == Approx(10) );
        REQUIRE( m_waveProp.getMomentumX()[l_ce] == Approx(0) );
      }

      // dam-break
      REQUIRE( m_waveProp.getHeight()[49]   == Approx(10 - 0.1 * 9.39467851749) );
      REQUIRE( m_waveProp.getMomentumX()[49] == Approx( 0 + 0.1 * 88.259917223468) );

      REQUIRE( m_waveProp.getHeight()[50]   == Approx(8 + 0.1 * 9.39467851749) );
      REQUIRE( m_waveProp.getMomentumX()[50] == Approx(0 + 0.1 * 88.259917223468) );

      // steady state
      for( std::size_t l_ce = 51; l_ce < 100; l_ce++ ) {
        REQUIRE( m_waveProp.getHeight()[l_ce]   == Approx(8) );
        REQUIRE( m_waveProp.getMomentumX()[l_ce] == Approx(0) );
      }
      break;
    case MODEROE:
      /*
      * Test case for Roe solver:
      *
      *   Single dam break problem between cell 49 and 50.
      *     left | right
      *       10 | 8
      *        0 | 0
      *
      *   Elsewhere steady state.
      *
      * The net-updates at the respective edge are given as
      * (see derivation in Roe solver):
      *    left          | right
      *      9.394671362 | -9.394671362
      *    -88.25985     | -88.25985
      */

      for( std::size_t l_ce = 0; l_ce < 50; l_ce++ ) {
        m_waveProp.setHeight( l_ce,
                              0,
                              10 );
        m_waveProp.setMomentumX( l_ce,
                                0,
                                0 );
      }
      for( std::size_t l_ce = 50; l_ce < 100; l_ce++ ) {
        m_waveProp.setHeight( l_ce,
                              0,
                              8 );
        m_waveProp.setMomentumX( l_ce,
                                0,
                                0 );
      }

      // set outflow boundary condition
      m_waveProp.setGhostOutflow();

      // perform a time step
      m_waveProp.timeStep( 0.1 );

      // steady state
      for( std::size_t l_ce = 0; l_ce < 49; l_ce++ ) {
        REQUIRE( m_waveProp.getHeight()[l_ce]   == Approx(10) );
        REQUIRE( m_waveProp.getMomentumX()[l_ce] == Approx(0) );
      }

      // dam-break
      REQUIRE( m_waveProp.getHeight()[49]   == Approx(10 - 0.1 * 9.394671362) );
      REQUIRE( m_waveProp.getMomentumX()[49] == Approx( 0 + 0.1 * 88.25985) );

      REQUIRE( m_waveProp.getHeight()[50]   == Approx(8 + 0.1 * 9.394671362) );
      REQUIRE( m_waveProp.getMomentumX()[50] == Approx(0 + 0.1 * 88.25985) );

      // steady state
      for( std::size_t l_ce = 51; l_ce < 100; l_ce++ ) {
        REQUIRE( m_waveProp.getHeight()[l_ce]   == Approx(8) );
        REQUIRE( m_waveProp.getMomentumX()[l_ce] == Approx(0) );
      }
      break;
  }
}