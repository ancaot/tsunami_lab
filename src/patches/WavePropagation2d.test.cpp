/**
 * @author
 *
 * @section DESCRIPTION
 * Unit tests for the two-dimensional wave propagation patch.
 *
 *
 **/
//#include "../../submodules/Catch2/include/catch.hpp"
#include <catch2/catch.hpp>
#include "WavePropagation2d.h"

TEST_CASE( "Test the 2d wave propagation solver.", "[WaveProp2d]" ) {

  // construct solver and setup a dambreak problem
  tsunami_lab::patches::WavePropagation2d m_waveProp( 100 , 100);

      /*
      * Test case:
      *
      *   Single dam break problem between cell 49 and 50.
      *     left | right
      *       10 | 8
      *        0 | 0
      *        0 | 0
      *
      *   Elsewhere steady state.
      *
      * The net-updates at the respective edge are given as):
      *    left                 | right
      *      h     |  h
      *      hu     | hu
      *      hv     | hv
      */
    for( std::size_t l_ceY = 0; l_ceY < 100; l_ceY++){
        for( std::size_t l_ceX = 0; l_ceX < 50; l_ceX++ ) {
        m_waveProp.setHeight( l_ceX,
                              l_ceY,
                              10 );
        m_waveProp.setMomentumX( l_ceX,
                                l_ceY,
                                0 );
        m_waveProp.setMomentumY( l_ceX,
                                l_ceY,
                                0 );
      }
    }

    for( std::size_t l_ceY = 0; l_ceY <100; l_ceY++){ 
      for( std::size_t l_ceX = 50; l_ceX < 100; l_ceX++ ) {
        m_waveProp.setHeight( l_ceX,
                              l_ceY,
                              8 );
        m_waveProp.setMomentumX( l_ceX,
                                l_ceY,
                                0 );
        m_waveProp.setMomentumY( l_ceX,
                                l_ceY,
                                0 );
      }
    } 

      // set outflow boundary condition
      m_waveProp.setGhostOutflow();

      // perform a time step
      m_waveProp.timeStep( 0.1 );

      // steady state
      for( std::size_t l_ce = 0; l_ce < 49; l_ce++ ) {
        REQUIRE( m_waveProp.getHeight()[l_ce]   == Approx(10) );
        REQUIRE( m_waveProp.getMomentumX()[l_ce] == Approx(0).margin(1e-5) );
        REQUIRE( m_waveProp.getMomentumY()[l_ce] == Approx(0).margin(1e-5) );
      }

      // dam-break
      REQUIRE( m_waveProp.getHeight()[49]   == Approx(10.0f ) );
      REQUIRE( m_waveProp.getMomentumX()[49] == Approx( 0.0f).margin(1e-5) );
      REQUIRE( m_waveProp.getMomentumY()[49] == Approx( 0.0f).margin(1e-5) );

      REQUIRE( m_waveProp.getHeight()[50]   == Approx(8.0f) );
      REQUIRE( m_waveProp.getMomentumX()[50] == Approx(0.0f).margin(1e-5) );
      REQUIRE( m_waveProp.getMomentumY()[50] == Approx(0.0f).margin(1e-5) );

      // steady state
      for( std::size_t l_ce = 51; l_ce < 100; l_ce++ ) {
        REQUIRE( m_waveProp.getHeight()[l_ce]   == Approx(8) );
        REQUIRE( m_waveProp.getMomentumX()[l_ce] == Approx(0).margin(1e-5) );
        REQUIRE( m_waveProp.getMomentumY()[l_ce] == Approx(0).margin(1e-5) );
      }

}
