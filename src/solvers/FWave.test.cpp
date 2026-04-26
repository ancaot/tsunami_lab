/**
 * @section DESCRIPTION
 * Unit tests of the F-Wave solver.
 **/


//#include "../../submodules/Catch2/include/catch.hpp"
#include <catch2/catch.hpp>
#define private public
#include "FWave.h"
#undef public

TEST_CASE( "Test the derivation of the FWave speeds.", "[FWaveSpeeds]" ) {
   /* 
    * Computation identical to Roe solver, so test case for derivation of FWave speeds also identical
    * 
    * Test case:
    *  h: 10 | 9
    *  u: -3 | 3
    *
    * roe height: 9.5
    * roe velocity: (sqrt(10) * -3 + 3 * 3) / ( sqrt(10) + sqrt(9) )
    *               = -0.0790021169691720
    * roe speeds: s1 = -0.079002116969172024 - sqrt(9.80665 * 9.5) = -9.7311093998375095
    *             s2 = -0.079002116969172024 + sqrt(9.80665 * 9.5) =  9.5731051658991654
    */
  float l_waveSpeedL = 0;
  float l_waveSpeedR = 0;
  tsunami_lab::solvers::FWave::waveSpeeds( 10,
                                         9,
                                         -3,
                                         3,
                                         l_waveSpeedL,
                                         l_waveSpeedR );

  REQUIRE( l_waveSpeedL == Approx( -9.7311093998375095 ) );
  REQUIRE( l_waveSpeedR == Approx(  9.5731051658991654 ) );
}

TEST_CASE( "Test the derivation of the FWave wave speeds.", "[FWaveStrengths]" ) {
  /*
   * Test case:
   *  h:   10 | 9
   *  u:   -3 | 3
   *  hu: -30 | 27
   *
   * The derivation of the Roe speeds (s1, s2) is given above.
   *
   *  Matrix of right eigenvectors:
   *
   *      | 1   1 |
   *  R = |       |
   *      | s1 s2 |
   *
   * Inversion yields:
   *
   * wolframalpha.com query: invert {{1, 1}, {-9.7311093998375095, 9.5731051658991654}}
   *
   *        | 0.49590751974393229 -0.051802159398648326 |
   * Rinv = |                                           |
   *        | 0.50409248025606771  0.051802159398648326 |
   *
   *
   * Jump computing with flux function
   * 
   * f = | hu                      |
   *     | h * u^2 + 1/2 * g * h^2 |
   *
   * f = | -30         27       |
   *     | 580.3325  478.169325 |
   *
   * Multiplicaton with the jump in quantities gives the wave strengths:
   *
   * wolframalpha.com query: {{0.49590751974393229, -0.051802159398648326}, {0.50409248025606771, 0.051802159398648326}} * {27 - -30 , 478.169325 - 580.3325}
   *
   *        | 27 - -30              |   | 33.559 |
   * Rinv * |                       | = |        |
   *        | 478.169325 - 580.3325 |   | 23.441 |
   */
  float l_strengthL = 0;
  float l_strengthR = 0;

  tsunami_lab::solvers::FWave::waveStrengths( 10,
                                            9,
                                            -30,
                                            27,
                                            -9.7311093998375095,
                                            9.5731051658991654,
                                            10,
                                            10,
                                            l_strengthL,
                                            l_strengthR );

  REQUIRE( l_strengthL == Approx(33.559) );
  REQUIRE( l_strengthR == Approx(23.441) );
}

TEST_CASE( "Test the derivation of the FWave net-updates.", "[FWaveUpdates]" ) {
  /*
   * Test case:
   *
   *      left | right
   *  h:    10 | 9
   *  u:    -3 | 3
   *  hu:  -30 | 27
   *
   * The derivation of the Roe speeds (s1, s2) and wave strengths (a1, a1) is given above.
   *
   * The net-updates are given through the scaled eigenvectors.
   *
   *                  |  1 |   |   33.559                  |
   * update #1:  a1 * |    | = |                           |
   *                  | s1 |   | -326.5663003491469813105  |
   *
   *                  |  1 |   |  23.441                  |
   * update #2:  a2 * |    | = |                          |
   *                  | s2 |   | 224.4031581938423361414  |
   */
  float l_netUpdatesL[2] = { -5, 3 };
  float l_netUpdatesR[2] = {  4, 7 };

  tsunami_lab::solvers::FWave::netUpdates( 10,
                                         9,
                                         -30,
                                         27,
                                         10,
                                         10,
                                         l_netUpdatesL,
                                         l_netUpdatesR );

  REQUIRE( l_netUpdatesL[0] == Approx( 33.559 ) );
  REQUIRE( l_netUpdatesL[1] == Approx( -326.5663003491469813105 ) );

  REQUIRE( l_netUpdatesR[0] == Approx( 23.441 ) );
  REQUIRE( l_netUpdatesR[1] == Approx( 224.4031581938423361414 ) );

  /*
   * Test case (dam break):
   *
   *     left | right
   *   h:  10 | 8
   *   hu:  0 | 0
   *
   * Roe speeds are given as:
   *
   *   s1 = -sqrt(9.80665 * 9)
   *   s2 =  sqrt(9.80665 * 9)
   *
   * Inversion of the matrix of right Eigenvectors:
   * 
   *   wolframalpha.com query: invert {{1, 1}, {-sqrt(9.80665 * 9), sqrt(9.80665 * 9)}}
   *
   *          | 0.5 -0.0532217 |
   *   Rinv = |                |
   *          | 0.5  0.0532217 |
   *
   * Multiplicaton with the jump (computed with flux function) in quantities gives the wave strengths:
   *
   *        |  0 - 0    |   | -1 |   | a1 |
   * Rinv * |           | = |    | = |    |
   *        | -176.5197 |   | -1 |   | a2 |
   *
   * The net-updates are given through the scaled eigenvectors.
   *
   *                  |  1 |   |   9.39467851749  |
   * update #1:  a1 * |    | = |                  |
   *                  | s1 |   | −88.259917223468 |
   *
   *                  |  1 |   |  −9.39467851749  |
   * update #2:  a2 * |    | = |                  |
   *                  | s2 |   | −88.259917223468 |
   */
  tsunami_lab::solvers::FWave::netUpdates( 10,
                                         8,
                                         0,
                                         0,
                                         10,
                                         10,
                                         l_netUpdatesL,
                                         l_netUpdatesR ); 

  REQUIRE( l_netUpdatesL[0] ==  Approx(9.394671362) );
  REQUIRE( l_netUpdatesL[1] == -Approx(88.25985)    );

  REQUIRE( l_netUpdatesR[0] == -Approx(9.394671362) );
  REQUIRE( l_netUpdatesR[1] == -Approx(88.25985)    );

   /*
    * Test case (Two positive eigenvectors, supersonic problem):
    *  h:  10  | 9
    *  u:  20  | 20
    *  hu: 200 | 180
    *
    * roe height: 9.5
    * roe velocity: (sqrt(10) * 20 + 3 * 20) / ( sqrt(10) + sqrt(9) )
    *               = 20
    * roe speeds: s1 = 20 - sqrt(9.80665 * 9.5) = 10.3479
    *             s2 = 20 + sqrt(9.80665 * 9.5) = 29.6521
    * 
    * Inversion of the matrix of right Eigenvectors:
    * 
    *   wolframalpha.com query: invert {{1, 1}, {10.3479, 29.6521}}
    *
    *          |  1.5360439697060742 -0.051802198485303694 |
    *   Rinv = |                                           |
    *          | -0.5360439697060743  0.051802198485303715 |
    *
    * Multiplicaton with the jump (computed with flux function) in quantities gives the wave strengths:
    *
    *        |         180 - 200       |          |  -20     |   |  -5.17394 |
    * Rinv * |                         | = Rinv * |          | = |           |
    *        | 3997.169325 - 4490.3325 |          | -493.163 |   | -14.8261  |
    *
    *
    *
    *        |         180 - 200       |   |  -5.17394 |   | a1 |
    * Rinv * |                         | = |           | = |    |
    *        | 3997.169325 - 4490.3325 |   | -14.8261  |   | a2 |
    *
    * The net-updates are given through the scaled eigenvectors.
    *
    *                  |  1 |   |  -5.17395 |
    * update #1:  a1 * |    | = |           |
    *                  | s1 |   | -53.5394  |
    *
    *                  |  1 |   |  -14.8261 |
    * update #2:  a2 * |    | = |           |
    *                  | s2 |   | -439.624  |
    *
    */

  tsunami_lab::solvers::FWave::netUpdates( 10,
                                         9,
                                         200,
                                         180,
                                         10,
                                         10,
                                         l_netUpdatesL,
                                         l_netUpdatesR ); 

  REQUIRE( l_netUpdatesL[0] == Approx(0) );
  REQUIRE( l_netUpdatesL[1] == Approx(0) );

  REQUIRE( l_netUpdatesR[0] == -Approx(20.) );
  REQUIRE( l_netUpdatesR[1] == -Approx(493.163) );

  /*
   * Test case (trivial steady state):
   *
   *     left | right
   *   h:  10 | 10
   *  hu:   0 |  0
   */
  tsunami_lab::solvers::FWave::netUpdates( 10,
                                         10,
                                         0,
                                         0,
                                         10,
                                         10,
                                         l_netUpdatesL,
                                         l_netUpdatesR );

  REQUIRE( l_netUpdatesL[0] == Approx(0) );
  REQUIRE( l_netUpdatesL[1] == Approx(0) );

  REQUIRE( l_netUpdatesR[0] == Approx(0) );
  REQUIRE( l_netUpdatesR[1] == Approx(0) );
}