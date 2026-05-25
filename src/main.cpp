/**
 *
 * @section DESCRIPTION
 * Simulationsprogramm: Lädt Setup, integriert optional Messstationen,
 * schreibt Lösungen und Stationsdaten.
 **/

#include "patches/WavePropagation1d.h"
#include "patches/WavePropagation2d.h"
#include "setups/DamBreak1d.h"
#include "io/Csv.h"
#include "setups/DamBreak2d.h"
#include "setups/TsunamiEvent1d.h"
#include "setups/ArtificialTsunami2d.h"
#include "setups/TsunamiEvent2d.h"
#include "io/Stations.h"
#ifdef USE_NETCDF
#include "io/NetCdf.h"
#endif
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>
#include <limits>
#include <string>

int main( int   i_argc,
          char *i_argv[] ) {
  // number of cells in x- and y-direction
  tsunami_lab::t_idx l_nx = 0;
  tsunami_lab::t_idx l_ny = 50;

  // set cell size
  tsunami_lab::t_real l_dxy = 1;

  std::cout << "####################################" << std::endl;
  std::cout << "### Tsunami Lab                  ###" << std::endl;
  std::cout << "###                              ###" << std::endl;
  std::cout << "### https://scalable.uni-jena.de ###" << std::endl;
  std::cout << "####################################" << std::endl;

  if( i_argc < 2 || i_argc > 4 ) {
    std::cerr << "invalid number of arguments, usage:" << std::endl;
    std::cerr << "  ./build/tsunami_lab N_CELLS_X [stations.csv|stations.xml] [output_interval_seconds]" << std::endl;
    std::cerr << "where N_CELLS_X is the number of cells in x-direction." << std::endl;
    return EXIT_FAILURE;
  }

  l_nx = atoi( i_argv[1] );
  
  // Tohoku domain size is ~2700km x 1500km. Let's adapt aspect ratio if requested or just map everything to user's desired width bounds
  tsunami_lab::t_real l_domain_size_x = 2700000.0;
  tsunami_lab::t_real l_domain_size_y = 1500000.0;
  
  l_dxy = l_domain_size_x / l_nx;
  l_ny = l_domain_size_y / l_dxy;

  if( l_nx < 1 || l_ny < 1 ) {
    std::cerr << "invalid number of cells" << std::endl;
    return EXIT_FAILURE;
  }
  
  std::cout << "runtime configuration" << std::endl;
  std::cout << "  number of cells in x-direction: " << l_nx << std::endl;
  std::cout << "  number of cells in y-direction: " << l_ny << std::endl;
  std::cout << "  cell size:                      " << l_dxy << std::endl;

  // construct setup
  tsunami_lab::setups::Setup *l_setup;
  // TODO: paths to the actual NetCDF files need to be provided when running TsunamiEvent2d
  l_setup = new tsunami_lab::setups::TsunamiEvent2d(
      "c:/Users/R-a-h-a-f/Downloads/data_in/output/tohoku_gebco20_ucsb3_250m_bath.bin", 
      "c:/Users/R-a-h-a-f/Downloads/data_in/output/tohoku_gebco20_ucsb3_250m_displ.bin"
  );

  // construct solver
  tsunami_lab::patches::WavePropagation *l_waveProp;
  l_waveProp = new tsunami_lab::patches::WavePropagation2d(l_nx, l_ny);

  // maximum observed height in the setup
  tsunami_lab::t_real l_hMax = std::numeric_limits< tsunami_lab::t_real >::lowest();

  for( tsunami_lab::t_idx l_cy = 0; l_cy < l_ny; l_cy++ ) {
    // Tohoku min offsets
    tsunami_lab::t_real l_y = (l_cy + 0.5) * l_dxy - 750000.0; 

    for( tsunami_lab::t_idx l_cx = 0; l_cx < l_nx; l_cx++ ) {
      tsunami_lab::t_real l_x = (l_cx + 0.5) * l_dxy - 200000.0;

      // get initial values of the setup
      tsunami_lab::t_real l_h = l_setup->getHeight(l_x,
                                                    l_y );
      l_hMax = std::max( l_h, l_hMax );

      tsunami_lab::t_real l_hu = l_setup->getMomentumX( l_x,
                                                        l_y );
      tsunami_lab::t_real l_hv = l_setup->getMomentumY( l_x,
                                                        l_y );

      

      
      tsunami_lab::t_real l_b = l_setup->getBathymetry(l_x, 
                                                        l_y);

      // set initial values in wave propagation solver
      l_waveProp->setHeight( l_cx,
                             l_cy,
                             l_h );

      l_waveProp->setMomentumX( l_cx,
                                l_cy,
                                l_hu );

      l_waveProp->setMomentumY( l_cx,
                                l_cy,
                                l_hv );

      l_waveProp->setBathymetry(l_cx,
                                l_cy,
                                l_b);

    }
  }

  // derive maximum wave speed in setup; the momentum is ignored
  tsunami_lab::t_real l_speedMax = std::sqrt( 9.81 * l_hMax );

  // derive constant time step; changes at simulation time are ignored
  tsunami_lab::t_real l_dt = 0.5 * l_dxy / l_speedMax;

  // derive scaling for a time step
  tsunami_lab::t_real l_scaling = l_dt / l_dxy;

  // set up time and print control
  tsunami_lab::t_idx  l_timeStep = 0;
  tsunami_lab::t_idx  l_nOut = 0;
  tsunami_lab::t_real l_endTime = 4500; // 75 mins simulation for Soma
  tsunami_lab::t_real l_simTime = 0;

  std::cout << "entering time loop" << std::endl;

  // optional stations configuration (argv[2]) and output interval (argv[3])
  tsunami_lab::io::Stations * l_stations = nullptr;
  if( i_argc >= 3 ) {
    tsunami_lab::t_real l_interval = 1.0;
    if( i_argc == 4 ) l_interval = static_cast< tsunami_lab::t_real >( std::atof( i_argv[3] ) );

    l_stations = new tsunami_lab::io::Stations( l_interval );
    if( l_stations->loadFromFile( i_argv[2] ) ) {
      l_stations->openFiles( "station" );
      std::cout << "stations loaded from " << i_argv[2] << ", output every " << l_interval << " s" << std::endl;
    }
    else {
      std::cerr << "failed to load stations from " << i_argv[2] << std::endl;
      delete l_stations;
      l_stations = nullptr;
    }
  }

#ifdef USE_NETCDF
  tsunami_lab::io::NetCdf l_netcdfWriter( "solution.nc",
                                          l_dxy,
                                          l_nx,
                                          l_ny,
                                          false );
  bool l_netcdfEnabled = l_netcdfWriter.isOpen();
  if( l_netcdfWriter.isOpen() ) {
    l_netcdfEnabled = l_netcdfWriter.writeTimeStep( 0,
                                                    l_nx,
                                                    l_waveProp->getHeight(),
                                                    l_waveProp->getMomentumX(),
                                                    nullptr,
                                                    0,
                                                    0 );
    if( !l_netcdfEnabled ) {
      std::cerr << "  netCDF output disabled at runtime (failed writing initial step)" << std::endl;
    }
  }

  if( l_netcdfEnabled ) {
    std::cout << "  netCDF output enabled: solution.nc" << std::endl;
  }
  else if( !l_netcdfWriter.isOpen() ) {
    std::cerr << "  netCDF output disabled at runtime (failed to initialize solution.nc)" << std::endl;
  }
#endif

  // iterate over time
  while( l_simTime < l_endTime ){
    if( l_timeStep % 25 == 0 ) {
      std::cout << "  simulation time / #time steps: "
                << l_simTime << " / " << l_timeStep << std::endl;

      std::string l_path = "solution_" + std::to_string(l_nOut) + ".csv";
      std::cout << "  writing wave field to " << l_path << std::endl;

      std::ofstream l_file;
      l_file.open( l_path  );

      tsunami_lab::io::Csv::write( l_dxy,
                                   l_nx,
                                   l_ny,
                                   1,
                                   l_waveProp->getHeight(),
                                   l_waveProp->getMomentumX(),
                                   l_waveProp->getMomentumY(),
                                   l_waveProp->getBathymetry(),
                                   l_file );
      l_file.close();
      l_nOut++;
    }

    l_waveProp->setGhostOutflow();
    l_waveProp->timeStep( l_scaling );

    l_timeStep++;
    l_simTime += l_dt;

    if( l_stations != nullptr ) {
      l_stations->sampleAndMaybeWrite( l_waveProp, l_simTime, l_dxy, l_nx, l_ny );
    }

#ifdef USE_NETCDF
    if( l_netcdfEnabled ) {
      bool l_ok = l_netcdfWriter.writeTimeStep( l_simTime,
                                                l_nx,
                                                l_waveProp->getHeight(),
                                                l_waveProp->getMomentumX(),
                                                nullptr,
                                                0,
                                                0 );
      if( !l_ok ) {
        l_netcdfEnabled = false;
        std::cerr << "  netCDF output disabled at runtime (write failure)" << std::endl;
      }
    }
#endif
  }

  std::cout << "finished time loop" << std::endl;

  // free memory
  std::cout << "freeing memory" << std::endl;
  delete l_setup;
  delete l_waveProp;
  if( l_stations != nullptr ) delete l_stations;

  std::cout << "finished, exiting" << std::endl;
  return EXIT_SUCCESS;
}
