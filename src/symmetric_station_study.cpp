/**
 * @section DESCRIPTION
 * Symmetric study: Compare 1D and 2D solvers on a circular dam break problem
 * at a set of measurement stations.
 *
 * This program runs both 1D and 2D solvers with the same setup, samples at the
 * same stations, and writes results to separate files:
 *   station_1d_name.csv
 *   station_2d_name.csv
 * 
 * This harness validates solver symmetry and accuracy at measurement points.
 **/

#include "patches/WavePropagation1d.h"
#include "patches/WavePropagation2d.h"
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
  tsunami_lab::t_idx l_nx = 100;
  tsunami_lab::t_idx l_ny = 100;
  tsunami_lab::t_real l_endTime = 30.0;
  std::string l_stationsPath = "data/stations_symmetric.xml";
  tsunami_lab::t_real l_stationInterval = 0.5;

  if( i_argc > 1 ) l_nx = static_cast< tsunami_lab::t_idx >( std::atoi( i_argv[1] ) );
  if( i_argc > 2 ) l_endTime = static_cast< tsunami_lab::t_real >( std::atof( i_argv[2] ) );
  if( i_argc > 3 ) l_stationsPath = i_argv[3];
  if( i_argc > 4 ) l_stationInterval = static_cast< tsunami_lab::t_real >( std::atof( i_argv[4] ) );

  if( l_nx < 10 ) {
    std::cerr << "invalid nx, choose nx >= 10" << std::endl;
    return EXIT_FAILURE;
  }

  // for symmetric 2D: square domain
  l_ny = l_nx;

  // domain and setup
  tsunami_lab::t_real const l_domainLength = 100.0;
  tsunami_lab::t_real const l_center = 50.0;
  tsunami_lab::t_real const l_radius = 10.0;
  tsunami_lab::t_real l_dxy = l_domainLength / static_cast< tsunami_lab::t_real >( l_nx );

  std::cout << "Symmetric 1D vs 2D Study" << std::endl;
  std::cout << "  Grid: " << l_nx << " x " << l_ny << std::endl;
  std::cout << "  dx, dy: " << l_dxy << " m" << std::endl;
  std::cout << "  End time: " << l_endTime << " s" << std::endl;
  std::cout << "  Station interval: " << l_stationInterval << " s" << std::endl;

  tsunami_lab::setups::CircularDamBreak1d l_setup( 10.0, 5.0, l_radius, l_center );

  // ===== 1D Solver =====
  std::cout << "\nInitializing 1D solver..." << std::endl;
  tsunami_lab::patches::WavePropagation1d l_wave1d( l_nx );
  tsunami_lab::t_real l_hMax = std::numeric_limits< tsunami_lab::t_real >::lowest();

  for( tsunami_lab::t_idx l_cx = 0; l_cx < l_nx; ++l_cx ) {
    tsunami_lab::t_real l_x = (l_cx + 0.5f) * l_dxy;
    tsunami_lab::t_real l_h = l_setup.getHeight( l_x, 0 );
    l_hMax = std::max( l_hMax, l_h );
    l_wave1d.setHeight( l_cx, 0, l_h );
    l_wave1d.setMomentumX( l_cx, 0, 0 );
    l_wave1d.setMomentumY( l_cx, 0, 0 );
  }

  // ===== 2D Solver =====
  std::cout << "Initializing 2D solver..." << std::endl;
  tsunami_lab::patches::WavePropagation2d l_wave2d( l_nx, l_ny );

  for( tsunami_lab::t_idx l_cy = 0; l_cy < l_ny; ++l_cy ) {
    tsunami_lab::t_real l_y = (l_cy + 0.5f) * l_dxy;
    for( tsunami_lab::t_idx l_cx = 0; l_cx < l_nx; ++l_cx ) {
      tsunami_lab::t_real l_x = (l_cx + 0.5f) * l_dxy;
      tsunami_lab::t_real l_h = l_setup.getHeight( l_x, l_y );
      l_wave2d.setHeight( l_cx, l_cy, l_h );
      l_wave2d.setMomentumX( l_cx, l_cy, 0 );
      l_wave2d.setMomentumY( l_cx, l_cy, 0 );
    }
  }

  tsunami_lab::t_real l_dt = 0.5f * l_dxy / std::sqrt( 9.81f * l_hMax );
  tsunami_lab::t_real l_scaling = l_dt / l_dxy;

  std::cout << "  Time step: " << l_dt << " s" << std::endl;

  // ===== Stations for both solvers =====
  tsunami_lab::io::Stations l_stations1d( l_stationInterval );
  tsunami_lab::io::Stations l_stations2d( l_stationInterval );

  l_stations1d.setDomainStart( 0, 0 );
  l_stations2d.setDomainStart( 0, 0 );

  if( !l_stations1d.loadFromFile( l_stationsPath ) ) {
    std::cerr << "failed loading stations from " << l_stationsPath << std::endl;
    return EXIT_FAILURE;
  }
  if( !l_stations2d.loadFromFile( l_stationsPath ) ) {
    std::cerr << "failed loading stations from " << l_stationsPath << std::endl;
    return EXIT_FAILURE;
  }

  l_stations1d.openFiles( "station_sym_1d" );
  l_stations2d.openFiles( "station_sym_2d" );

  std::cout << "\nRunning simulation..." << std::endl;

  // ===== Time loop =====
  tsunami_lab::t_real l_simTime = 0;
  tsunami_lab::t_idx  l_step = 0;

  while( l_simTime < l_endTime ) {
    if( l_step % 100 == 0 ) {
      std::cout << "  t = " << l_simTime << " s" << std::endl;
    }

    // update both solvers
    l_wave1d.setGhostOutflow();
    l_wave1d.timeStep( l_scaling );

    l_wave2d.setGhostOutflow();
    l_wave2d.timeStep( l_scaling );

    l_simTime += l_dt;
    l_step++;

    // sample stations
    l_stations1d.sampleAndMaybeWrite( &l_wave1d, l_simTime, l_dxy, l_nx, 1 );
    l_stations2d.sampleAndMaybeWrite( &l_wave2d, l_simTime, l_dxy, l_nx, l_ny );
  }

  // ===== Write final snapshots =====
  std::ofstream l_snapshot1d( "symmetry_1d_final.csv" );
  tsunami_lab::io::Csv::write( l_dxy,
                               l_nx,
                               1,
                               1,
                               l_wave1d.getHeight(),
                               l_wave1d.getMomentumX(),
                               nullptr,
                               nullptr,
                               l_snapshot1d );

  std::ofstream l_snapshot2d( "symmetry_2d_final.csv" );
  tsunami_lab::io::Csv::write( l_dxy,
                               l_nx,
                               l_ny,
                               l_nx,
                               l_wave2d.getHeight(),
                               l_wave2d.getMomentumX(),
                               l_wave2d.getMomentumY(),
                               nullptr,
                               l_snapshot2d );

  std::cout << "\nDone." << std::endl;
  std::cout << "Station output 1D: station_sym_1d_*.csv" << std::endl;
  std::cout << "Station output 2D: station_sym_2d_*.csv" << std::endl;
  std::cout << "Final snapshot 1D: symmetry_1d_final.csv" << std::endl;
  std::cout << "Final snapshot 2D: symmetry_2d_final.csv" << std::endl;
  std::cout << "\nCompare station outputs to validate solver symmetry!" << std::endl;
  return EXIT_SUCCESS;
}
