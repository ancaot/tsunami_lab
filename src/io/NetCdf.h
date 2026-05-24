/**
 * netCDF writer for wave-field snapshots.
 **/
#ifndef TSUNAMI_LAB_IO_NETCDF
#define TSUNAMI_LAB_IO_NETCDF

#include "../constants.h"
#include <string>
#include <vector>

namespace tsunami_lab {
  namespace io {
    class NetCdf;
  }
}

class tsunami_lab::io::NetCdf {
  private:
    int m_fileId = -1;

    int m_dimTimeId = -1;
    int m_dimYId = -1;
    int m_dimXId = -1;

    int m_varTimeId = -1;
    int m_varYId = -1;
    int m_varXId = -1;
    int m_varHeightId = -1;
    int m_varMomentumXId = -1;
    int m_varMomentumYId = -1;

    t_idx m_nx = 0;
    t_idx m_ny = 0;
    t_idx m_timeStep = 0;
    t_idx m_numTimeSteps = 0;

    bool m_includeMomentumY = false;
    bool m_isOpen = false;
    bool m_readMode = false;

    bool check( int i_errorCode,
                std::string const & i_operation ) const;

    bool defineFileLayout( t_real i_dxy );
    bool writeCoordinates( t_real i_dxy );

  public:
    /**
     * @brief Constructor for writing new NetCDF files.
     * 
     * @param i_path Path to the new file.
     * @param i_dxy Cell size.
     * @param i_nx Number of x-cells.
     * @param i_ny Number of y-cells.
     * @param i_includeMomentumY Whether to include y-momentum.
     */
    NetCdf( std::string const & i_path,
            t_real              i_dxy,
            t_idx               i_nx,
            t_idx               i_ny,
            bool                i_includeMomentumY );

    /**
     * @brief Constructor for reading existing NetCDF files.
     * 
     * @param i_path Path to the existing NetCDF file.
     */
    NetCdf( std::string const & i_path );

    ~NetCdf();

    /**
     * @brief Checks if the NetCDF file is successfully opened.
     * 
     * @return true if successful.
     */
    bool isOpen() const {
      return m_isOpen;
    }

    /**
     * @brief Checks if the file is in read mode.
     * 
     * @return true if read mode.
     */
    bool isReadMode() const {
      return m_readMode;
    }

    /**
     * @brief Write interface to write a single timestep.
     * 
     * @param i_time Simulation time.
     * @param i_stride Array stride.
     * @param i_h Water height array.
     * @param i_hu Momentum in x direction array.
     * @param i_hv Momentum in y direction array (optional).
     * @param i_ghostX Number of ghost cells in x.
     * @param i_ghostY Number of ghost cells in y.
     * @return true if successful.
     */
    bool writeTimeStep( t_real               i_time,
                        t_idx                i_stride,
                        t_real       const * i_h,
                        t_real       const * i_hu,
                        t_real       const * i_hv,
                        t_idx                i_ghostX = 1,
                        t_idx                i_ghostY = 0 );

    /**
     * @brief Read interface for bathymetry and displacement.
     * 
     * @param i_path Path to the NetCDF file.
     * @param o_nx Returns number of x grids.
     * @param o_ny Returns number of y grids.
     * @param o_x Returns pointer to x coordinates array.
     * @param o_y Returns pointer to y coordinates array.
     * @param o_z Returns pointer to z values array.
     */
    static void read( std::string const & i_path,
                      t_idx             & o_nx,
                      t_idx             & o_ny,
                      t_real           ** o_x,
                      t_real           ** o_y,
                      t_real           ** o_z );

    /**
     * @brief Read interface from sphinx-tsunami documentation.
     * 
     * @param filename Path to the NetCDF file.
     * @param varname Variable name to read.
     * @param data Vector that will contain the output data.
     * @return 0 if successful, -1 otherwise.
     */
    static int read( const char* filename,
                     const char* varname,
                     std::vector<t_real> &data );

    /**
     * @brief Reads a time step from an existing NetCDF file.
     * 
     * @param i_timeStep Time step index to read.
     * @param o_h Pointer to store height data.
     * @param o_hu Pointer to store momentum-x data.
     * @param o_hv Pointer to store momentum-y data (optional).
     * @return true if successful.
     */
    bool readTimeStep( t_idx                i_timeStep,
                       t_real       * o_h,
                       t_real       * o_hu,
                       t_real       * o_hv );

    /**
     * @brief Returns the total number of timesteps.
     * @return Number of timesteps.
     */
    t_idx getTimeStepCount() const {
      return m_numTimeSteps;
    }

    /**
     * @brief Returns the number of x-cells.
     * @return nx.
     */
    t_idx getNx() const {
      return m_nx;
    }

    /**
     * @brief Returns the number of y-cells.
     * @return ny.
     */
    t_idx getNy() const {
      return m_ny;
    }

    /**
     * @brief Returns the simulation time of a timestep.
     * @param i_timeStep index.
     * @return simulation time.
     */
    t_real getTime( t_idx i_timeStep ) const;
};

#endif
