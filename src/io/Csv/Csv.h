/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * IO-routines for writing a snapshot as Comma Separated Values (CSV).
 **/
#ifndef TSUNAMI_LAB_IO_CSV
#define TSUNAMI_LAB_IO_CSV

#include "../../constants.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstddef> 

namespace tsunami_lab {
  namespace io {
    class Csv;
  }
}

class tsunami_lab::io::Csv {
  public:
    /**
     * @brief Writes the data as CSV to the given stream.
     *
     * @param i_dxy width of a cell
     * @param i_nx number of cells in x-direction.
     * @param i_ny number of cells in y-direction.
     * @param i_stride stride of the data arrays in y-direction (x is assumed to be stride-1).
     * @param i_domainstart_x the offset of the domain in x direction.
     * @param i_domainstart_y the offset of the domain in y direction.
     * @param i_dem_wavepropagation tells us whether we use 1D or 2D.
     * @param i_h water height of the cells; optional: use nullptr if not required.
     * @param i_hu momentum in x-direction of the cells; optional: use nullptr if not required.
     * @param i_hv momentum in y-direction of the cells; optional: use nullptr if not required.
     * @param i_b bathymetry of the cell
     * @param io_stream stream to which the CSV-data is written.
     **/
    static void write( t_real               i_dxy,
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
                       std::ostream       & io_stream );

    /**
    * @brief Reads the csv file and stores the data into a vector (in our case only bathymetry values).
    * @param filename location of the csv file.
    * @param columnIndex is after how many commas "," the value you want to read is located.
    * @return vector<t_real> that contains a collumn of the csv file (in our case only bathymetry values).
    */
    static std::vector<t_real> read(const std::string & filename,
                                    std::size_t         columnIndex);
};

#endif