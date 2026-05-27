/**
 * @author Khaled minawe
 *
 * @section DESCRIPTION
 * IO-routines for writing NetCDF files.
 **/
#ifndef TSUNAMI_LAB_IO_NETCDF
#define TSUNAMI_LAB_IO_NETCDF

#include "../../constants.h"
#include <netcdf.h>
#include <string>

namespace tsunami_lab {
  namespace io {
    class NetCdf;
  }
}

class tsunami_lab::io::NetCdf {
  private:
    int m_varIdX, m_varIdY, m_varIdHeight, m_varIdTime, m_varIdBathymetry, m_varIdImpulseX, m_varIdImpulseY;

    static void writeInteriorGrid(t_real const * i_grid,
                                  t_idx          i_nx,
                                  t_idx          i_ny,
                                  t_idx          i_stride,
                                  t_idx          i_timeStep,
                                  bool           i_twoDimensionsOnly,
                                  int            i_varId,
                                  int            i_ncId);

  public:
    static void checkNcErr(int i_err, char const * i_file, int i_line);

    NetCdf(t_idx i_nx,
           t_idx i_ny,
           const char * i_filename);

    void fillConstants(t_idx          i_nx,
                       t_idx          i_ny,
                       t_idx          i_stride,
                       t_real         i_dxy,
                       t_real         i_domainStartX,
                       t_real         i_domainStartY,
                       t_real const * i_b,
                       const char *   i_filename);

    void updateFile(t_idx          i_nx,
                    t_idx          i_ny,
                    t_idx          i_stride,
                    t_idx          i_timeStep,
                    t_real         i_time,
                    t_real const * i_h,
                    t_real const * i_hu,
                    t_real const * i_hv,
                    const char *   i_filename);

    static void read(const char * i_filename,
                     const char * i_varname,
                     t_idx &      o_nx,
                     t_idx &      o_ny,
                     t_real **    o_zdata,
                     t_real **    o_xdata,
                     t_real **    o_ydata);
};

#endif
