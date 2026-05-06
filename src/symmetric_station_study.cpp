/**
 * @section DESCRIPTION
 * Runs a symmetric 1d circular-dam-break surrogate and writes station output.
 *
 * This executable is intended as a harness for later 1d-vs-2d comparison.
 **/

#include "patches/WavePropagation1d.h"
#include "setups/CircularDamBreak1d.h"
#include "io/Csv.h"
#include "io/Stations.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <cmath>

int main( int i_argc,
          char *i_argv[] ) {
  tsunami_lab::t_idx l_nx = 200;
  tsunami_lab::t_real l_endTime = 20.0;
  std::string l_stationsPath = "data/stations_symmetric.xml";
  tsunami_lab::t_real l_stationInterval = 0.25;

  if( i_argc > 1 ) l_nx = static_cast< tsunami_lab::t_idx >( std::atoi( i_argv[1] ) );
  if( i_argc > 2 ) l_endTime = static_cast< tsunami_lab::t_real >( std::atof( i_argv[2] ) );
  if( i_argc > 3 ) l_stationsPath = i_argv[3];
  if( i_argc > 4 ) l_stationInterval = static_cast< tsunami_lab::t_real >( std::atof( i_argv[4] ) );

  if( l_nx < 10 ) {
    std::cerr << "invalid nx, choose nx >= 10" << std::endl;
    return EXIT_FAILURE;
  }

  // domain and setup
  tsunami_lab::t_real const l_domainLength = 100.0;
  tsunami_lab::t_real const l_center = 50.0;
  tsunami_lab::t_real const l_radius = 10.0;
  tsunami_lab::t_real l_dxy = l_domainLength / static_cast< tsunami_lab::t_real >( l_nx );

  tsunami_lab::setups::CircularDamBreak1d l_setup( 10.0, 5.0, l_radius, l_center );
  tsunami_lab::patches::WavePropagation1d l_wave( l_nx );

  tsunami_lab::t_real l_hMax = std::numeric_limits< tsunami_lab::t_real >::lowest();
  for( tsunami_lab::t_idx l_cx = 0; l_cx < l_nx; ++l_cx ) {
    tsunami_lab::t_real l_x = (l_cx + 0.5f) * l_dxy;
    tsunami_lab::t_real l_h = l_setup.getHeight( l_x, 0 );
    l_hMax = std::max( l_hMax, l_h );
    l_wave.setHeight( l_cx, 0, l_h );
    l_wave.setMomentumX( l_cx, 0, 0 );
    l_wave.setMomentumY( l_cx, 0, 0 );
  }

  tsunami_lab::t_real l_dt = 0.5f * l_dxy / std::sqrt( 9.81f * l_hMax );
  tsunami_lab::t_real l_scaling = l_dt / l_dxy;

  tsunami_lab::io::Stations l_stations( l_stationInterval );
  l_stations.setDomainStart( 0, 0 );
  if( !l_stations.loadFromFile( l_stationsPath ) ) {
    std::cerr << "failed loading stations from " << l_stationsPath << std::endl;
    return EXIT_FAILURE;
  }
  l_stations.openFiles( "station_sym_1d" );

  tsunami_lab::t_real l_simTime = 0;
  while( l_simTime < l_endTime ) {
    l_wave.setGhostOutflow();
    l_wave.timeStep( l_scaling );

    l_simTime += l_dt;
    l_stations.sampleAndMaybeWrite( &l_wave, l_simTime, l_dxy, l_nx, 1 );
  }

  std::ofstream l_snapshot( "symmetry_1d_final.csv" );
  tsunami_lab::io::Csv::write( l_dxy,
                               l_nx,
                               1,
                               1,
                               l_wave.getHeight(),
                               l_wave.getMomentumX(),
                               nullptr,
                               l_snapshot );

  std::cout << "wrote station folder: station_sym_1d" << std::endl;
  std::cout << "wrote final snapshot: symmetry_1d_final.csv" << std::endl;
  return EXIT_SUCCESS;
}
