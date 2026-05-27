/*#include <catch2/catch.hpp>
#define private public
#include "fwave.h"
#undef public

TEST_CASE( "Testing The InverseMatrix ", "[InverseMatrix]" ) {

  float i_inverse[4];
  tsunami_lab::solvers::fwave::inverseMatrix( 2,3, i_inverse );
  REQUIRE( i_inverse[0] == 3 );
  REQUIRE( i_inverse[1] == -1 );
  REQUIRE( i_inverse[2] == -2 );
  REQUIRE( i_inverse[3] == 1 );
}

TEST_CASE( "Testing The Flux Function ", "[Flux]" ) {

  float i_inverse[2];
  tsunami_lab::solvers::fwave::flux( 9,7,5,4,i_inverse);
  REQUIRE( i_inverse[0] == -1 );
  REQUIRE( i_inverse[1] == Approx(-157.39846));

  tsunami_lab::solvers::fwave::flux( 2,4,3,16,i_inverse);
  REQUIRE( i_inverse[0] == 13 );
  REQUIRE( i_inverse[1] == Approx(118.3399));

}




TEST_CASE( "Testing The eigencoefficient", "[AlphaVector]" ) {

  float i_inverse[4] = {1,2,3,4};
  float i_delta_f[2] = {5,6};
  float eigencoefficients[2];
  tsunami_lab::solvers::fwave::eigencoefficientAlpha( i_inverse,i_delta_f,0,eigencoefficients);
  REQUIRE( eigencoefficients[0] == 17 );
  REQUIRE( eigencoefficients[1] == 39);
 
  
}

TEST_CASE( "Testing The Eigenvalues 1 ", "[EigenValues]" ) {

  float eigenVal1 = 0;
  float eigenVal2 = 0;
  tsunami_lab::solvers::fwave::eigenvalues( 5.3,
                                            2.6,
                                            -6.5660377358491,
                                            12.3846153846154,
                                            eigenVal1,
                                            eigenVal2 );

  REQUIRE( eigenVal1 == Approx( -4.9840244 ) );
  REQUIRE( eigenVal2 == Approx(  7.463668864 ) );

  tsunami_lab::solvers::fwave::eigenvalues( 10,
                                            9,
                                            -3,
                                            3,
                                            eigenVal1,
                                            eigenVal2);

  REQUIRE( eigenVal1 == Approx( -9.7311093998375095 ) );
  REQUIRE( eigenVal2 == Approx(  9.5731051658991654 ) );


}


TEST_CASE( "Test the derivation of the Fwave net-updates ", "[Zp - Vectors]" ) {
  float i_alphas[] = {17,39};
  float i_eigens[] = {-9.73,9.57};
  float i_minus_A_deltaQ[2];
  float i_plus_A_deltaQ[2];
  tsunami_lab::solvers::fwave::decompose(i_alphas,
                                        i_eigens,
                                         i_minus_A_deltaQ,
                                         i_plus_A_deltaQ );

  REQUIRE( i_minus_A_deltaQ[0] == Approx( 19.01562f) );
  REQUIRE( i_minus_A_deltaQ[1] == Approx(-161.40999f) );

  REQUIRE( i_plus_A_deltaQ[0] == 39 );
  

  REQUIRE( i_plus_A_deltaQ[1] ==  Approx(  373.23 ) );
}


TEST_CASE( "Testing The Decomposition ", "[Zp - Vectors]" ) {


  float l_netUpdatesL[2];
  float l_netUpdatesR[2];
  tsunami_lab::solvers::fwave::netUpdates( 10,
                                         10,
                                         0,
                                         0,0,0,
                                         l_netUpdatesL,
                                         l_netUpdatesR );

  REQUIRE( l_netUpdatesL[0] == Approx(0) );
  REQUIRE( l_netUpdatesL[1] == Approx(0) );

  REQUIRE( l_netUpdatesR[0] == Approx(0) );
  REQUIRE( l_netUpdatesR[1] == Approx(0) );


  tsunami_lab::solvers::fwave::netUpdates( 10,
                                         8,
                                         0,
                                         0,
                                         0,
                                         0,                                         l_netUpdatesL,
                                         l_netUpdatesR ); 

  REQUIRE( l_netUpdatesL[0] ==  Approx(9.394671362) );
  REQUIRE( l_netUpdatesL[1] ==  -Approx(88.25985)    );

  REQUIRE( l_netUpdatesR[0] == -Approx(9.394671362) );
  REQUIRE( l_netUpdatesR[1] == -Approx(88.25985)    );


tsunami_lab::solvers::fwave::netUpdates( 10,
                                         9,
                                         -30,
                                         27,
                                         0,
                                         0,
                                         l_netUpdatesL,
                                         l_netUpdatesR ); 

  REQUIRE( l_netUpdatesL[0] == Approx( 33.5590017014261447899292 ) );
  REQUIRE( l_netUpdatesL[1] == Approx( -326.56631690591093200508 ) );

  REQUIRE( l_netUpdatesR[0] == Approx( 23.4409982985738561366777 ) );
  REQUIRE( l_netUpdatesR[1] == Approx( 224.403141905910928927533 ) );

  tsunami_lab::solvers::fwave::netUpdates( 2.8,
                                         9.6,
                                         41.9,
                                         37.7,0,0,
                                         l_netUpdatesL,
                                         l_netUpdatesR ); 

  REQUIRE( l_netUpdatesL[0] == Approx(0) );
  REQUIRE( l_netUpdatesL[1] == Approx(0) );

  REQUIRE( l_netUpdatesR[0] == Approx(  -4.19969));
  REQUIRE( l_netUpdatesR[1] == Approx( -65.504));

//test for the Bathymetry with net-update
  tsunami_lab::solvers::fwave::netUpdates( 5,
                                         6,
                                         0,
                                         0,
                                         -10,
                                         -4,
                                         l_netUpdatesL,
                                         l_netUpdatesR ); 

  REQUIRE( l_netUpdatesL[0] == Approx( -25.7045 ) );
  REQUIRE( l_netUpdatesL[1] == Approx( 188.7780125) );

  REQUIRE( l_netUpdatesR[0] == Approx( 25.7045) );
  REQUIRE( l_netUpdatesR[1] == Approx( 188.7780125 ) );


  tsunami_lab::solvers::fwave::netUpdates( 5,
                                         6,
                                         4,
                                         3,
                                         -14,
                                         -7,
                                         l_netUpdatesL,
                                         l_netUpdatesR ); 

  REQUIRE( l_netUpdatesL[0] == Approx( -29.80466) );
  REQUIRE( l_netUpdatesL[1] == Approx( 199.720) );

  REQUIRE( l_netUpdatesR[0] == Approx( 28.80465) );
  REQUIRE( l_netUpdatesR[1] == Approx( 230.072 ) );




}*/