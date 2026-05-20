/**
 * netCDF writer for wave-field snapshots.
 **/
#ifndef TSUNAMI_LAB_IO_NETCDF
#define TSUNAMI_LAB_IO_NETCDF

#include "../constants.h"
#include <string>
<<<<<<< HEAD
=======
#include <vector>
>>>>>>> origin/feat/added-documentation

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
    // Constructor for writing new files
    NetCdf( std::string const & i_path,
            t_real              i_dxy,
            t_idx               i_nx,
            t_idx               i_ny,
            bool                i_includeMomentumY );

    // Constructor for reading existing files
    NetCdf( std::string const & i_path );

    ~NetCdf();

    bool isOpen() const {
      return m_isOpen;
    }

    bool isReadMode() const {
      return m_readMode;
    }

    // Write interface
    bool writeTimeStep( t_real               i_time,
                        t_idx                i_stride,
                        t_real       const * i_h,
                        t_real       const * i_hu,
                        t_real       const * i_hv,
                        t_idx                i_ghostX = 1,
                        t_idx                i_ghostY = 0 );

<<<<<<< HEAD
=======
    // Read interface for bathymetry/displacement (existing)
    static void read( std::string const & i_path,
                      t_idx             & o_nx,
                      t_idx             & o_ny,
                      t_real           ** o_x,
                      t_real           ** o_y,
                      t_real           ** o_z );

    // Read interface from the sphingx-tsunami documentation
    static int read( const char* filename,
                     const char* varname,
                     std::vector<t_real> &data );

>>>>>>> origin/feat/added-documentation
    // Read interface
    bool readTimeStep( t_idx                i_timeStep,
                       t_real       * o_h,
                       t_real       * o_hu,
                       t_real       * o_hv );

    t_idx getTimeStepCount() const {
      return m_numTimeSteps;
    }

    t_idx getNx() const {
      return m_nx;
    }

    t_idx getNy() const {
      return m_ny;
    }

    t_real getTime( t_idx i_timeStep ) const;
};

#endif
