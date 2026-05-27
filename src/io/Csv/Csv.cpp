/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * IO-routines for writing a snapshot as Comma Separated Values (CSV).
 **/
#include "Csv.h"
#include "../JsReader/Configuration.h"
void tsunami_lab::io::Csv::write( t_real               i_dxy,                     
                                  t_idx                i_nx,
                                  t_idx                i_ny,
                                  t_idx                i_stride,
                                  t_real               i_domainstart_x,
                                  t_real               i_domainstart_y,
                                  std::string          i_dem_wavepropagation,
                                  t_real       const * i_h,
                                  t_real       const * i_hu,
                                  t_real       const * i_hv,
                                  t_real       const * i_b,
                                  std::ostream       & io_stream ) {
  // write the CSV header
  io_stream << "x,y";
  if( i_h  != nullptr ) io_stream << ",height";
  if( i_hu != nullptr ) io_stream << ",momentum_x";
  if( i_hv != nullptr ) io_stream << ",momentum_y";
  if( i_b != nullptr ) io_stream <<  ",bathymetry";
  io_stream << "\n";
  
  if(i_dem_wavepropagation == "2d" ){

    for( t_idx l_iy = 1; l_iy < i_ny+1; l_iy++ ) {
      for( t_idx l_ix = 1; l_ix < i_nx+1; l_ix++ ) {
      // derive coordinates
        t_real l_posX = ((l_ix-1 + 0.5) * i_dxy )+ i_domainstart_x; 
        t_real l_posY = ((l_iy-1 + 0.5) * i_dxy )+ i_domainstart_y;

        t_idx l_id = l_iy * i_stride + l_ix;

      // write data
        io_stream << l_posX << "," << l_posY;
        if( i_h  != nullptr ) io_stream << "," << i_h[l_id];
        if( i_hu != nullptr ) io_stream << "," << i_hu[l_id];
        if( i_hv != nullptr ) io_stream << "," << i_hv[l_id];
        if( i_b  != nullptr ) io_stream << "," << i_b[l_id];
        io_stream << "\n";
      }
    }
  }else{      // iterate over all cells
    for (t_idx l_iy = 0; l_iy < i_ny; l_iy++){
      for (t_idx l_ix = 0; l_ix < i_nx; l_ix++){
        // derive coordinates of cell center
        t_real l_posX = (l_ix + 0.5) * i_dxy + i_domainstart_x;
        t_real l_posY = (l_iy + 0.5) * i_dxy;

        t_idx l_id = l_iy * i_stride + l_ix;

        // write data
        io_stream << l_posX << "," << l_posY;
        if( i_h  != nullptr ) io_stream << "," << i_h[l_id];
        if( i_hu != nullptr ) io_stream << "," << i_hu[l_id];
        if( i_hv != nullptr ) io_stream << "," << i_hv[l_id];
        if( i_b  != nullptr ) io_stream << "," << i_b[l_id];
        io_stream << "\n";
      }
    }
  }
  io_stream << std::flush;
}  // iterate over all cells
  
  /**
     * @param line every row gets saved in "line" and is overwritten in next iteration
     * @param token is the determined bathymetry value written in "selectedColumn"
     * @file  data_end.csv contains the bathymetry values
     * @brief save the bathymetry values which are located in data:end.csv in the vector
     * @return selectedColumn that contains the bathymetry values
    */
std::vector<tsunami_lab::t_real> tsunami_lab::io::Csv::read(const std::string & filename,
                                                std::size_t  columnIndex){
    //checks whether file exists 
    std::vector<t_real> selectedColumn;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return selectedColumn;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        for (std::size_t i = 0; std::getline(iss, token, ',') && i <= columnIndex; ++i) {
            if (i == columnIndex) {
                selectedColumn.push_back(std::stod(token));
                break;
            }
        }
    }

    return selectedColumn;
}
