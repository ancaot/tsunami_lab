/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Unit tests for the one-dimensional wave propagation patch.
 **/
#include <catch2/catch.hpp>
#include "WavePropagation1d.h"

TEST_CASE( "Test the 1d wave propagation solver.", "[WaveProp1d]" ) {
  /*
   * Test case:
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

  // construct solver and setup a dambreak problem


  tsunami_lab::patches::WavePropagation1d m_waveProp( 100 , true, false );

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
  m_waveProp.setGhostCollumn();

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


//test steady state from middle stat_stsates.csv
tsunami_lab::patches::WavePropagation1d middle_states1(100, true, false);

  for (std::size_t l_ce = 0; l_ce < 50; l_ce++)
  {
    middle_states1.setHeight(l_ce,
                         0,
                         8899.326826472694);
    middle_states1.setMomentumX(l_ce,
                            0,
                            122.0337839252433);
  }
  for (std::size_t l_ce = 50; l_ce < 100; l_ce++)
  {
    middle_states1.setHeight(l_ce,
                         0,
                         8899.326826472694);
    middle_states1.setMomentumX(l_ce,
                            0,
                            -122.0337839252433);
  }

  for (int i = 0; i < 17; i++)
  {
    middle_states1.setGhostCollumn();
    middle_states1.timeStep(0.001);
  }


  REQUIRE(middle_states1.getHeight()[49] == Approx(8899.74));

  REQUIRE(middle_states1.getHeight()[50] == Approx(8899.74));

// calculated bei geogebra
  tsunami_lab::patches::WavePropagation1d middle_states2(100, true, false);

  for (std::size_t l_ce = 0; l_ce < 50; l_ce++)
  {
    middle_states1.setHeight(l_ce,
                         0,
                         9894.065328676988);
    middle_states1.setMomentumX(l_ce,
                            0,
                            763.616897222239);
  }
  for (std::size_t l_ce = 50; l_ce < 100; l_ce++)
  {
    middle_states1.setHeight(l_ce,
                         0,
                         9894.065328676988);
    middle_states1.setMomentumX(l_ce,
                            0,
                            -763.616897222239);
  }

  for (int i = 0; i < 17; i++)
  {
    middle_states1.setGhostCollumn();
    middle_states1.timeStep(0.001);
  }


  REQUIRE(middle_states1.getHeight()[49] == Approx(9896.516538751875));

  REQUIRE(middle_states1.getHeight()[50] == Approx(9896.516538751875));
 



// calculated bei geogebra
tsunami_lab::patches::WavePropagation1d middle_states3(100, false, false);

  for (std::size_t l_ce = 0; l_ce < 50; l_ce++)
  {
    middle_states1.setHeight(l_ce,
                         0,
                         1387.176994373967);
    middle_states1.setMomentumX(l_ce,
                            0,
                            -101.9619713277172);
  }
  for (std::size_t l_ce = 50; l_ce < 100; l_ce++)
  {
    middle_states1.setHeight(l_ce,
                         0,
                         1387.176994373967);
    middle_states1.setMomentumX(l_ce,
                            0,
                            101.9619713277172);
  }

  for (int i = 0; i < 40; i++)
  {
    middle_states1.setGhostCollumn();
    middle_states1.timeStep(0.001);
  }


  REQUIRE(middle_states1.getHeight()[49] == Approx(1386.303079031417));

  REQUIRE(middle_states1.getHeight()[50] == Approx(1386.303079031417));


  
 

}



