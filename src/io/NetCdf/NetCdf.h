/**
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

    /**
     * writing information for restarting simulation
     * 
     * @param i_solver which numerical solver is used
     * @param i_scenario which scenario/setup is used
     * @param i_waveModel which wave model is used
     * @param i_domainSizeX how many x cells total
     * @param i_domainSizeY how many y cells total
     * @param i_nx number of x cells
     * @param i_ny number of y cells
     * @param i_domainStartX lowest x-coordinate
     * @param i_domainStartY lowest y-coordinate
     * @param i_simEndTime time simulation ends
     * @param i_outputFormat which format is the output file
     * @param i_bathymetryFile which file is the bathymetry input
     * @param i_displacementFile which file is the displacement input
     * @param i_outputName name of output file
     * @param i_reflectiveBoundary is there a reflective boundary
     * @param i_hu momentum in x direction
     * @param i_hv momentum in y direction
     * @param i_h left height
     * @param i_damLocation location of dam
     * @param i_b bathymetry
     * @param i_timeStep current time step
     * @param i_stride number of cells in row/collom
     * @param i_dxy number of total cells
     *  i_time simulated time
     * @param i_filename name of output file
     * @param i_checkpointfile name of checkpoint file the information is written in
     * 
     */
    void createCheckPoint(std::string   i_solver,
                          std::string   i_scenario,
                          std::string   i_waveModel,
                          t_real        i_domainSizeX,
                          t_real        i_domainSizeY,
                          t_idx         i_nx,
                          t_idx         i_ny,
                          t_real        i_domainStartX,
                          t_real        i_domainStartY,
                          t_real        i_simEndTime,
                          std::string   i_outputFormat,
                          std::string   i_bathymetryFile,
                          std::string   i_displacementFile,
                          std::string   i_outputName,
                          bool          i_reflectiveBoundary,
                          t_real        i_simTimeLastCP,
                          t_real const * i_hu,
                          t_real const * i_hv,
                          t_real const * i_h,
                          t_real         i_damLocation,
                          t_real const * i_b,
                          t_idx         i_stride,
                          t_real        i_dxy,
                          std::string   i_checkpointfile);

    /**
     * reads a check point file for setup information
     * 
     * @param i_filename name of checkpoint file
     * other parameter based on input of writeCheckPoint and config.json 
     */
    static void readCheckPoint(const char * i_filename,
                              std::string * o_solver,
                              std::string * o_scenario,
                              std::string * o_wavemodel,
                              t_real *      o_domainSizeX,
                              t_real *      o_domainSizeY,
                              t_idx *       o_nx,
                              t_idx *       o_ny,
                              t_real *      o_domainStartX,
                              t_real *      o_domainStartY,
                              t_real *      o_simEndTime,
                              std::string * o_outputFormat,
                              std::string * o_bathymetryFile,
                              std::string * o_displacementFile,
                              std::string * o_outputName,
                              bool *        o_reflectiveBoundary,
                              t_real *      o_simTimeLastCP,
                              t_real **     o_hu,
                              t_real **     o_hv,
                              t_real **     o_h,
                              t_real *      o_damLocation,
                              t_real **     o_b
                              /*t_idx *       o_timeStep*/);

};

#endif
