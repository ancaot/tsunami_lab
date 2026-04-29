/**
 *
 * @section DESCRIPTION
 * IO-routines for writing a snapshot as Comma Separated Values (CSV).
 **/
#include "Csv.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

void tsunami_lab::io::Csv::write( t_real               i_dxy,
                                  t_idx                i_nx,
                                  t_idx                i_ny,
                                  t_idx                i_stride,
                                  t_real       const * i_h,
                                  t_real       const * i_hu,
                                  t_real       const * i_hv,
                                  std::ostream       & io_stream ) {
  // write the CSV header
  io_stream << "x,y";
  if( i_h  != nullptr ) io_stream << ",height";
  if( i_hu != nullptr ) io_stream << ",momentum_x";
  if( i_hv != nullptr ) io_stream << ",momentum_y";
  io_stream << "\n";

  // iterate over all cells
  for( t_idx l_iy = 0; l_iy < i_ny; l_iy++ ) {
    for( t_idx l_ix = 0; l_ix < i_nx; l_ix++ ) {
      // derive coordinates of cell center
      t_real l_posX = (l_ix + 0.5) * i_dxy;
      t_real l_posY = (l_iy + 0.5) * i_dxy;

      t_idx l_id = l_iy * i_stride + l_ix;

      // write data
      io_stream << l_posX << "," << l_posY;
      if( i_h  != nullptr ) io_stream << "," << i_h[l_id];
      if( i_hu != nullptr ) io_stream << "," << i_hu[l_id];
      if( i_hv != nullptr ) io_stream << "," << i_hv[l_id];
      io_stream << "\n";
    }
  }
  io_stream << std::flush;
}

std::vector<tsunami_lab::t_real> tsunami_lab::io::Csv::read( std::string     const & i_filename){
  //open file (and check if it works)
  std::ifstream file(i_filename);
  std::vector<t_real> o_bathymetry;

  if(!file.is_open()){
    throw std::runtime_error("Failed to open file: " + i_filename);
    return o_bathymetry;
  }

  //helper variables
  std::string line;
  t_real l_bVal;

  //read header row
  std::getline(file, line);

  //get relevant data of cells
  while (std::getline(file, line)) {
    std::stringstream l_str(line);
    std::string longitude, latitude, track_location, height;

    //skipping to height as other data is not needed
    std::getline(l_str, longitude, ',');
    std::getline(l_str, latitude, ',');
    std::getline(l_str, track_location, ',');
    std::getline(l_str, height, ',');

    //converting string to float
    try {
      l_bVal = std::stof(height);
    } catch (const std::exception& e) {
      std::cerr << "Invalid value: " << height << std::endl;
      continue;
    }

    //storing the bathymetry data
    o_bathymetry.push_back(l_bVal);
  }
  file.close();
  return o_bathymetry;

}