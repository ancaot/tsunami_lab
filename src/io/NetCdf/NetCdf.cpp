#include "NetCdf.h"

#include <cstring>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {
  void replaceFileAtomically(const std::string & i_source,
                             const std::string & i_target) {
#ifdef _WIN32
    if (!MoveFileExA(i_source.c_str(),
                     i_target.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
      throw std::runtime_error("Unable to replace checkpoint file.");
    }
#else
    std::filesystem::rename(i_source, i_target);
#endif
  }
}

tsunami_lab::io::NetCdf::NetCdf(t_idx i_nx,
                                t_idx i_ny,
                                t_idx i_k,
                                const char * i_filename) {
  m_k = i_k > 0 ? i_k : 1;
  t_idx out_nx = std::ceil(i_nx / (t_real)m_k);
  t_idx out_ny = std::ceil(i_ny / (t_real)m_k);

  int l_ncId, l_err;
  int l_dimXId, l_dimYId, l_dimTimeId;
  int l_dim2Ids[2];

  if (std::filesystem::exists(i_filename)) {
    l_err = nc_open(i_filename, NC_WRITE, &l_ncId);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_inq_varid(l_ncId, "x", &m_varIdX);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_inq_varid(l_ncId, "y", &m_varIdY);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_inq_varid(l_ncId, "time", &m_varIdTime);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_inq_varid(l_ncId, "b", &m_varIdBathymetry);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_inq_varid(l_ncId, "h", &m_varIdHeight);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_inq_varid(l_ncId, "hu", &m_varIdImpulseX);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_inq_varid(l_ncId, "hv", &m_varIdImpulseY);
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_close(l_ncId);
    checkNcErr(l_err, __FILE__, __LINE__);
    return;
  }

  l_err = nc_create(i_filename, NC_CLOBBER, &l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_dim(l_ncId, "x", out_nx, &l_dimXId);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_dim(l_ncId, "y", out_ny, &l_dimYId);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_dim(l_ncId, "time", NC_UNLIMITED, &l_dimTimeId);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "x", NC_FLOAT, 1, &l_dimXId, &m_varIdX);
  checkNcErr(l_err, __FILE__, __LINE__);
  const char * l_unitsX = "meter";
  nc_put_att_text(l_ncId, m_varIdX, "units", strlen(l_unitsX), l_unitsX);

  l_err = nc_def_var(l_ncId, "y", NC_FLOAT, 1, &l_dimYId, &m_varIdY);
  checkNcErr(l_err, __FILE__, __LINE__);
  const char * l_unitsY = "meter";
  nc_put_att_text(l_ncId, m_varIdY, "units", strlen(l_unitsY), l_unitsY);

  l_err = nc_def_var(l_ncId, "time", NC_FLOAT, 1, &l_dimTimeId, &m_varIdTime);
  checkNcErr(l_err, __FILE__, __LINE__);
  const char * l_unitsTime = "seconds";
  nc_put_att_text(l_ncId, m_varIdTime, "units", strlen(l_unitsTime), l_unitsTime);

  l_dim2Ids[0] = l_dimYId;
  l_dim2Ids[1] = l_dimXId;

  int l_dim3Ids[3];
  l_dim3Ids[0] = l_dimTimeId;
  l_dim3Ids[1] = l_dimYId;
  l_dim3Ids[2] = l_dimXId;

  l_err = nc_def_var(l_ncId, "b", NC_FLOAT, 2, l_dim2Ids, &m_varIdBathymetry);
  checkNcErr(l_err, __FILE__, __LINE__);
  const char * l_unitsBathymetry = "meters";
  nc_put_att_text(l_ncId, m_varIdBathymetry, "units", strlen(l_unitsBathymetry), l_unitsBathymetry);

  l_err = nc_def_var(l_ncId, "h", NC_FLOAT, 3, l_dim3Ids, &m_varIdHeight);
  checkNcErr(l_err, __FILE__, __LINE__);
  const char * l_unitsHeight = "meters";
  nc_put_att_text(l_ncId, m_varIdHeight, "units", strlen(l_unitsHeight), l_unitsHeight);

  l_err = nc_def_var(l_ncId, "hu", NC_FLOAT, 3, l_dim3Ids, &m_varIdImpulseX);
  checkNcErr(l_err, __FILE__, __LINE__);
  const char * l_unitsImpulseX = "meters";
  nc_put_att_text(l_ncId, m_varIdImpulseX, "units", strlen(l_unitsImpulseX), l_unitsImpulseX);

  l_err = nc_def_var(l_ncId, "hv", NC_FLOAT, 3, l_dim3Ids, &m_varIdImpulseY);
  checkNcErr(l_err, __FILE__, __LINE__);
  const char * l_unitsImpulseY = "meters";
  nc_put_att_text(l_ncId, m_varIdImpulseY, "units", strlen(l_unitsImpulseY), l_unitsImpulseY);

  l_err = nc_enddef(l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_close(l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);
}

void tsunami_lab::io::NetCdf::fillConstants(t_idx          i_nx,
                                            t_idx          i_ny,
                                            t_idx          i_stride,
                                            t_real         i_dxy,
                                            t_real         i_domainStartX,
                                            t_real         i_domainStartY,
                                            t_real const * i_b,
                                            const char *   i_filename) {
  int l_ncId, l_err;
  l_err = nc_open(i_filename, NC_WRITE, &l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);

  t_idx out_nx = std::ceil(i_nx / (t_real)m_k);
  t_idx out_ny = std::ceil(i_ny / (t_real)m_k);

  std::vector<t_real> l_coordinateX(out_nx);
  std::vector<t_real> l_coordinateY(out_ny);

  for (t_idx l_iy = 0; l_iy < out_ny; l_iy++) {
    l_coordinateY[l_iy] = ((l_iy * m_k + (m_k - 1) / 2.0f + 0.5f) * i_dxy) + i_domainStartY;
  }

  l_err = nc_put_var_float(l_ncId, m_varIdY, l_coordinateY.data());
  checkNcErr(l_err, __FILE__, __LINE__);

  for (t_idx l_ix = 0; l_ix < out_nx; l_ix++) {
    l_coordinateX[l_ix] = ((l_ix * m_k + (m_k - 1) / 2.0f + 0.5f) * i_dxy) + i_domainStartX;
  }

  l_err = nc_put_var_float(l_ncId, m_varIdX, l_coordinateX.data());
  checkNcErr(l_err, __FILE__, __LINE__);

  writeInteriorGrid(i_b, i_nx, i_ny, i_stride, 0, true, m_varIdBathymetry, l_ncId);

  l_err = nc_close(l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);
}

void tsunami_lab::io::NetCdf::updateFile(t_idx          i_nx,
                                         t_idx          i_ny,
                                         t_idx          i_stride,
                                         t_idx          i_timeStep,
                                         t_real         i_time,
                                         t_real const * i_h,
                                         t_real const * i_hu,
                                         t_real const * i_hv,
                                         const char *   i_filename) {
  int l_ncId, l_err;
  l_err = nc_open(i_filename, NC_WRITE, &l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);

  writeInteriorGrid(i_h, i_nx, i_ny, i_stride, i_timeStep, false, m_varIdHeight, l_ncId);
  writeInteriorGrid(i_hu, i_nx, i_ny, i_stride, i_timeStep, false, m_varIdImpulseX, l_ncId);
  writeInteriorGrid(i_hv, i_nx, i_ny, i_stride, i_timeStep, false, m_varIdImpulseY, l_ncId);

  l_err = nc_put_var1_float(l_ncId, m_varIdTime, &i_timeStep, &i_time);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_sync(l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_close(l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);
}

void tsunami_lab::io::NetCdf::writeInteriorGrid(t_real const * i_grid,
                                                t_idx          i_nx,
                                                t_idx          i_ny,
                                                t_idx          i_stride,
                                                t_idx          i_timeStep,
                                                bool           i_twoDimensionsOnly,
                                                int            i_varId,
                                                int            i_ncId) {
  t_idx out_nx = std::ceil(i_nx / (t_real)m_k);
  t_idx out_ny = std::ceil(i_ny / (t_real)m_k);
  std::vector<t_real> l_grid(out_nx * out_ny, 0.0);

  for (t_idx l_iy = 0; l_iy < out_ny; l_iy++) {
    for (t_idx l_ix = 0; l_ix < out_nx; l_ix++) {
      t_real l_sum = 0;
      t_idx l_avg_count = 0;
      for (t_idx l_ky = 0; l_ky < m_k; l_ky++) {
        for (t_idx l_kx = 0; l_kx < m_k; l_kx++) {
          t_idx l_iy_actual = l_iy * m_k + l_ky;
          t_idx l_ix_actual = l_ix * m_k + l_kx;
          if (l_iy_actual < i_ny && l_ix_actual < i_nx) {
            l_sum += i_grid[(l_iy_actual + 1) * i_stride + (l_ix_actual + 1)];
            l_avg_count++;
          }
        }
      }
      l_grid[l_iy * out_nx + l_ix] = l_sum / l_avg_count;
    }
  }

  std::vector<size_t> l_start;
  std::vector<size_t> l_count;
  std::vector<ptrdiff_t> l_stride_nc;

  if (i_twoDimensionsOnly) {
    l_start = {0, 0};
    l_count = {out_ny, out_nx};
    l_stride_nc = {1, 1};
  }
  else {
    l_start = {i_timeStep, 0, 0};
    l_count = {1, out_ny, out_nx};
    l_stride_nc = {1, 1, 1};
  }

  int l_err = nc_put_vars_float(i_ncId, i_varId, l_start.data(), l_count.data(), l_stride_nc.data(), l_grid.data());
  checkNcErr(l_err, __FILE__, __LINE__);
}

void tsunami_lab::io::NetCdf::read(const char * i_filename,
                                   const char * i_varname,
                                   t_idx &      o_nx,
                                   t_idx &      o_ny,
                                   t_real **    o_zdata,
                                   t_real **    o_xdata,
                                   t_real **    o_ydata) {
  if (!std::filesystem::exists(i_filename)) {
    if (std::getenv("GITHUB_ACTIONS") == nullptr) {
      throw std::runtime_error(std::string("File does not exist: ") + i_filename);
    }
    std::cerr << "Warning: File does not exist: " << i_filename << '\n';
    return;
  }

  int l_ncId, l_err = 0;
  int l_varIdZ, l_varIdY, l_varIdX, l_dimX, l_dimY;

  l_err = nc_open(i_filename, NC_NOWRITE, &l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_inq_dimid(l_ncId, "x", &l_dimX);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_inq_dimlen(l_ncId, l_dimX, &o_nx);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_inq_dimid(l_ncId, "y", &l_dimY);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_inq_dimlen(l_ncId, l_dimY, &o_ny);
  checkNcErr(l_err, __FILE__, __LINE__);

  if (o_xdata != nullptr) {
    l_err = nc_inq_varid(l_ncId, "x", &l_varIdX);
    checkNcErr(l_err, __FILE__, __LINE__);
    *o_xdata = new t_real[o_nx];
    l_err = nc_get_var_float(l_ncId, l_varIdX, *o_xdata);
    checkNcErr(l_err, __FILE__, __LINE__);
  }

  if (o_ydata != nullptr) {
    l_err = nc_inq_varid(l_ncId, "y", &l_varIdY);
    checkNcErr(l_err, __FILE__, __LINE__);
    *o_ydata = new t_real[o_ny];
    l_err = nc_get_var_float(l_ncId, l_varIdY, *o_ydata);
    checkNcErr(l_err, __FILE__, __LINE__);
  }

  if (o_zdata != nullptr) {
    l_err = nc_inq_varid(l_ncId, i_varname, &l_varIdZ);
    checkNcErr(l_err, __FILE__, __LINE__);
    *o_zdata = new t_real[o_ny * o_nx];
    l_err = nc_get_var_float(l_ncId, l_varIdZ, *o_zdata);
    checkNcErr(l_err, __FILE__, __LINE__);
  }

  l_err = nc_close(l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);
}

void tsunami_lab::io::NetCdf::checkNcErr(int i_err, char const * i_file, int i_line) {
  if (i_err) {
    std::cerr << "Error: " << nc_strerror(i_err) << " , in " << i_file << " , line " << i_line << std::endl;
    exit(2);
  }
}


void tsunami_lab::io::NetCdf::createCheckPoint(std::string   i_solver,
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
                          std::string  i_checkpointfile){
  int l_ncId, l_err;
  //writing checkpoint files
  std::string l_basePath = "tsunami_lab/outputs";
  std::string l_path = l_basePath + "/checkpoints";
  std::string l_cpPath = l_path + "/" + i_checkpointfile;
  std::string l_tmpPath = l_cpPath + ".tmp";

  if (!std::filesystem::exists(l_basePath)) {
    std::filesystem::create_directory(l_basePath);
  }
  if (!std::filesystem::exists(l_path)) {
    std::filesystem::create_directory(l_path);
  }

  std::filesystem::remove(l_tmpPath);
  
  l_err = nc_create(l_tmpPath.data(), NC_CLOBBER | NC_NETCDF4, &l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);

  //defining dimensions
  int l_dimXId, l_dimYId, l_dimTimeId, l_dimStrId;
  l_err = nc_def_dim(l_ncId, "x", i_nx, &l_dimXId);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_dim(l_ncId, "y", i_ny, &l_dimYId);
  checkNcErr(l_err, __FILE__, __LINE__);
  
  l_err = nc_def_dim(l_ncId, "time", NC_UNLIMITED, &l_dimTimeId);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_dim(l_ncId, "str_len", 256, &l_dimStrId);
  checkNcErr(l_err, __FILE__, __LINE__);

  int l_dim2Ids[2];

  l_dim2Ids[0] = l_dimYId;
  l_dim2Ids[1] = l_dimXId;

  //defining variables ids
  int l_var_solver, l_var_scenario, l_var_waveModel, l_var_domainSizeX, l_var_domainSizeY, 
    l_var_domainStartX, l_var_domainStartY, l_var_simEndTime, l_var_outputFormat, l_var_bathFile, l_var_dispFile,
    l_var_outputName, l_var_reflectiveBoundary, l_var_simTimeLastCP, l_var_damLocation, /*l_var_timeStep,*/ l_var_stride, l_var_dxy,
    l_var_h, l_var_hu, l_var_hv, l_var_b;
  
  l_err = nc_def_var(l_ncId, "solver", NC_CHAR, 1, &l_dimStrId, &l_var_solver);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "scenario", NC_CHAR, 1, &l_dimStrId, &l_var_scenario);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "wave_model", NC_CHAR, 1, &l_dimStrId, &l_var_waveModel);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "x_domain_size", NC_FLOAT, 0, nullptr, &l_var_domainSizeX);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "y_domain_size", NC_FLOAT, 0, nullptr, &l_var_domainSizeY);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "x", NC_FLOAT, 1, &l_dimXId, &m_varIdX);
  checkNcErr(l_err, __FILE__, __LINE__);
  const char * l_unitsX = "meter";
  nc_put_att_text(l_ncId, m_varIdX, "units", strlen(l_unitsX), l_unitsX);

  l_err = nc_def_var(l_ncId, "y", NC_FLOAT, 1, &l_dimYId, &m_varIdY);
  checkNcErr(l_err, __FILE__, __LINE__);
  const char * l_unitsY = "meter";
  nc_put_att_text(l_ncId, m_varIdY, "units", strlen(l_unitsY), l_unitsY);

  l_err = nc_def_var(l_ncId, "x_domain_start", NC_FLOAT, 0, nullptr, &l_var_domainStartX);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "y_domain_start", NC_FLOAT, 0, nullptr, &l_var_domainStartY);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "simulation_end_time", NC_FLOAT, 0, nullptr, &l_var_simEndTime);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "output_format", NC_CHAR, 1, &l_dimStrId, &l_var_outputFormat);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "bathymetry_file", NC_CHAR, 1, &l_dimStrId, &l_var_bathFile);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "displacement_file", NC_CHAR, 1, &l_dimStrId, &l_var_dispFile);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "output_file_name", NC_CHAR, 1, &l_dimStrId, &l_var_outputName);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "reflective_boundary", NC_INT, 0, nullptr, &l_var_reflectiveBoundary);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "simulation_time_last_check", NC_FLOAT, 0, nullptr, &l_var_simTimeLastCP);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "momentum_x", NC_FLOAT, 2, l_dim2Ids, &l_var_hu);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "momentum_y", NC_FLOAT, 2, l_dim2Ids, &l_var_hv);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "height", NC_FLOAT, 2, l_dim2Ids, &l_var_h);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "damlocation", NC_FLOAT, 0, nullptr, &l_var_damLocation);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "bathymetry", NC_FLOAT, 2, l_dim2Ids, &l_var_b);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "stride", NC_FLOAT, 0, nullptr, &l_var_stride);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_var(l_ncId, "dxy", NC_FLOAT, 0, nullptr, &l_var_dxy);
  checkNcErr(l_err, __FILE__, __LINE__);

  /*l_err = nc_def_var(l_ncId, "time", NC_FLOAT, 1, &l_dimTimeId, &l_var_timeStep);
  checkNcErr(l_err, __FILE__, __LINE__);
  const char * l_unitsTime = "seconds";
  nc_put_att_text(l_ncId, l_var_timeStep, "units", strlen(l_unitsTime), l_unitsTime);*/

  l_err = nc_enddef(l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);

  //filling variables accordingly

  char l_solver[256] = {};
  strncpy(l_solver, i_solver.c_str(), sizeof(l_solver) - 1);
  l_err = nc_put_var_text(l_ncId, l_var_solver, l_solver);
  checkNcErr(l_err, __FILE__, __LINE__);

  char l_scenario[256] = {};
  strncpy(l_scenario, i_scenario.c_str(), sizeof(l_scenario) - 1);
  l_err = nc_put_var_text(l_ncId, l_var_scenario, l_scenario);
  checkNcErr(l_err, __FILE__, __LINE__);

  char l_waveModel[256] = {};
  strncpy(l_waveModel, i_waveModel.c_str(), sizeof(l_waveModel) - 1);
  l_err = nc_put_var_text(l_ncId, l_var_waveModel, l_waveModel);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_put_var_float(l_ncId, l_var_domainSizeX, &i_domainSizeX);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_put_var_float(l_ncId, l_var_domainSizeY, &i_domainSizeY);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_put_var_float(l_ncId, l_var_domainSizeX, &i_domainSizeX);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_put_var_float(l_ncId, l_var_domainStartX, &i_domainStartX);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_put_var_float(l_ncId, l_var_domainStartY, &i_domainStartY);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_put_var_float(l_ncId, l_var_simEndTime, &i_simEndTime);
  checkNcErr(l_err, __FILE__, __LINE__);
  
  l_err = nc_put_var_float(l_ncId, l_var_damLocation, &i_damLocation);
  checkNcErr(l_err, __FILE__, __LINE__); 

  char l_outputFormat[256] = {};
  strncpy(l_outputFormat, i_outputFormat.c_str(), sizeof(l_outputFormat) - 1);
  l_err = nc_put_var_text(l_ncId, l_var_outputFormat, l_outputFormat);
  checkNcErr(l_err, __FILE__, __LINE__);

  char l_bathFile[256] = {};
  strncpy(l_bathFile, i_bathymetryFile.c_str(), sizeof(l_bathFile) - 1);
  l_err = nc_put_var_text(l_ncId, l_var_bathFile, l_bathFile);
  checkNcErr(l_err, __FILE__, __LINE__);

  char l_dispFile[256] = {};
  strncpy(l_dispFile, i_displacementFile.c_str(), sizeof(l_dispFile) - 1);
  l_err = nc_put_var_text(l_ncId, l_var_dispFile, l_dispFile);
  checkNcErr(l_err, __FILE__, __LINE__);

  char l_outputName[256] = {};
  strncpy(l_outputName, i_outputName.c_str(), sizeof(l_outputName) - 1);
  l_err = nc_put_var_text(l_ncId, l_var_outputName, l_outputName);
  checkNcErr(l_err, __FILE__, __LINE__);

  int l_boundary = 0;
  if (i_reflectiveBoundary == true) l_boundary = 1;
  l_err = nc_put_var_int(l_ncId, l_var_reflectiveBoundary, &l_boundary);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_put_var_float(l_ncId, l_var_simTimeLastCP, &i_simTimeLastCP);
  checkNcErr(l_err, __FILE__, __LINE__);

  t_idx l_stride_idx = i_stride;
  t_real l_stride_real = static_cast<t_real>(l_stride_idx);
  l_err = nc_put_var_float(l_ncId, l_var_stride, &l_stride_real);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_put_var_float(l_ncId, l_var_dxy, &i_dxy);
  checkNcErr(l_err, __FILE__, __LINE__);

  //impulses and height
  std::vector<size_t> l_start     = {0, 0};
  std::vector<size_t> l_count       = {1,i_nx};
  std::vector<ptrdiff_t> l_stride = {1,1};

  for(l_start[0] = 0; l_start[0] < i_ny; l_start[0]++){
    l_err = nc_put_vars_float(l_ncId, l_var_hu, l_start.data(), l_count.data(), l_stride.data(), (i_hu + (l_start[0]+1) * i_stride + 1));
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_put_vars_float(l_ncId, l_var_hv, l_start.data(), l_count.data(), l_stride.data(), (i_hv + (l_start[0]+1) * i_stride + 1));
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_put_vars_float(l_ncId, l_var_h, l_start.data(), l_count.data(), l_stride.data(), (i_h + (l_start[0]+1) * i_stride + 1));
    checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_put_vars_float(l_ncId, l_var_b, l_start.data(), l_count.data(), l_stride.data(), (i_b + (l_start[0]+1) * i_stride + 1));
    checkNcErr(l_err, __FILE__, __LINE__);
  }

  l_err = nc_sync(l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_close(l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);

  replaceFileAtomically(l_tmpPath, l_cpPath);
}

void tsunami_lab::io::NetCdf::readCheckPoint(const char * i_filename,
                              std::string * o_solver,
                              std::string * o_scenario,
                              std::string * o_waveModel,
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
                              /*t_idx *       o_timeStep*/){
    //checking file existence
    if (!std::filesystem::exists(i_filename)) {
    if (std::getenv("GITHUB_ACTIONS") == nullptr) {
      throw std::runtime_error(std::string("File does not exist: ") + i_filename);
    }
    std::cerr << "Warning: File does not exist: " << i_filename << '\n';
    return;
  }

  //opening file
  int l_ncId, l_err = 0;

  l_err = nc_open(i_filename, NC_NOWRITE, &l_ncId);
  if (l_err != NC_NOERR) {
    throw std::runtime_error(std::string("Unable to read checkpoint '") + i_filename + "': " + nc_strerror(l_err));
  }

  //pointers for getting variable id to get var info
  
  int l_dimXId, l_dimYId;
  int l_var_solver, l_var_scenario, l_var_waveModel, l_var_domainSizeX, l_var_domainSizeY, 
    l_var_domainStartX, l_var_domainStartY, l_var_simEndTime, l_var_outputFormat, l_var_bathFile, l_var_dispFile,
    l_var_outputName, l_var_reflectiveBoundary, l_var_simTimeLastCP, l_var_damLocation, /*l_var_timeStep,*/
    l_var_h, l_var_hu, l_var_hv, l_var_b;

  //getting variable ids and getting data of said variable
  l_err = nc_inq_varid(l_ncId, "solver", &l_var_solver);
  checkNcErr(l_err, __FILE__, __LINE__);

  char l_solver[256] = {};
  l_err = nc_get_var_text(l_ncId, l_var_solver, l_solver);
  *o_solver = std::string(l_solver, strnlen(l_solver, sizeof(l_solver)));
  checkNcErr(l_err, __FILE__, __LINE__);

  
  l_err = nc_inq_varid(l_ncId, "scenario", &l_var_scenario);
  checkNcErr(l_err, __FILE__, __LINE__);

  char l_scenario[256] = {};
  l_err = nc_get_var_text(l_ncId, l_var_scenario, l_scenario);
  *o_scenario = std::string(l_scenario, strnlen(l_scenario, sizeof(l_scenario)));
  checkNcErr(l_err, __FILE__, __LINE__);

  
  l_err = nc_inq_varid(l_ncId, "wave_model", &l_var_waveModel);
  checkNcErr(l_err, __FILE__, __LINE__);

  char l_waveModel[256] = {};
  l_err = nc_get_var_text(l_ncId, l_var_waveModel, l_waveModel);
  *o_waveModel = std::string(l_waveModel, strnlen(l_waveModel, sizeof(l_waveModel)));
  checkNcErr(l_err, __FILE__, __LINE__);


  l_err = nc_inq_varid(l_ncId, "x_domain_size", &l_var_domainSizeX);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_get_var_float(l_ncId, l_var_domainSizeX, o_domainSizeX);
  checkNcErr(l_err, __FILE__, __LINE__);


  l_err = nc_inq_varid(l_ncId, "y_domain_size", &l_var_domainSizeY);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_get_var_float(l_ncId, l_var_domainSizeY, o_domainSizeY);
  checkNcErr(l_err, __FILE__, __LINE__);


  //x and y cells
  l_err = nc_inq_dimid(l_ncId, "x", &l_dimXId);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_inq_dimlen(l_ncId, l_dimXId, o_nx);
  checkNcErr(l_err, __FILE__, __LINE__);


  l_err = nc_inq_dimid(l_ncId, "y", &l_dimYId);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_inq_dimlen(l_ncId, l_dimYId, o_ny);
  checkNcErr(l_err, __FILE__, __LINE__);


  l_err = nc_inq_varid(l_ncId, "x_domain_start", &l_var_domainStartX);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_get_var_float(l_ncId, l_var_domainStartX, o_domainStartX);
  checkNcErr(l_err, __FILE__, __LINE__);


  l_err = nc_inq_varid(l_ncId, "y_domain_start", &l_var_domainStartY);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_get_var_float(l_ncId, l_var_domainStartY, o_domainStartY);
  checkNcErr(l_err, __FILE__, __LINE__);


  l_err = nc_inq_varid(l_ncId, "simulation_end_time", &l_var_simEndTime);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_get_var_float(l_ncId, l_var_simEndTime, o_simEndTime);
  checkNcErr(l_err, __FILE__, __LINE__);


  l_err = nc_inq_varid(l_ncId, "output_format", &l_var_outputFormat);
  checkNcErr(l_err, __FILE__, __LINE__);

  char l_outputFormat[256] = {};
  l_err = nc_get_var_text(l_ncId, l_var_outputFormat, l_outputFormat);
  *o_outputFormat = std::string(l_outputFormat, strnlen(l_outputFormat, sizeof(l_outputFormat)));
  checkNcErr(l_err, __FILE__, __LINE__);


  l_err = nc_inq_varid(l_ncId, "bathymetry_file", &l_var_bathFile);
  checkNcErr(l_err, __FILE__, __LINE__);

  char l_bathFile[256] = {};
  l_err = nc_get_var_text(l_ncId, l_var_bathFile, l_bathFile);
  *o_bathymetryFile = std::string(l_bathFile, strnlen(l_bathFile, sizeof(l_bathFile)));
  checkNcErr(l_err, __FILE__, __LINE__);


  l_err = nc_inq_varid(l_ncId, "displacement_file", &l_var_dispFile);
  checkNcErr(l_err, __FILE__, __LINE__);

  char l_dispFile[256] = {};
  l_err = nc_get_var_text(l_ncId, l_var_dispFile, l_dispFile);
  *o_displacementFile = std::string(l_dispFile, strnlen(l_dispFile, sizeof(l_dispFile)));
  checkNcErr(l_err, __FILE__, __LINE__);


  l_err = nc_inq_varid(l_ncId, "output_file_name", &l_var_outputName);
  checkNcErr(l_err, __FILE__, __LINE__);

  char l_outputName[256] = {};
  l_err = nc_get_var_text(l_ncId, l_var_outputName, l_outputName);
  *o_outputName = std::string(l_outputName, strnlen(l_outputName, sizeof(l_outputName)));
  checkNcErr(l_err, __FILE__, __LINE__);

  
  l_err = nc_inq_varid(l_ncId, "reflective_boundary", &l_var_reflectiveBoundary);
  checkNcErr(l_err, __FILE__, __LINE__);

  int l_reflectiveBoundary = 0;
  l_err = nc_get_var_int(l_ncId, l_var_reflectiveBoundary, &l_reflectiveBoundary);
  checkNcErr(l_err, __FILE__, __LINE__);

  if(l_reflectiveBoundary == 0){
    *o_reflectiveBoundary = false;
  }else{
    *o_reflectiveBoundary = true;
  }


  l_err = nc_inq_varid(l_ncId, "simulation_time_last_check", &l_var_simTimeLastCP);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_get_var_float(l_ncId, l_var_simTimeLastCP, o_simTimeLastCP);
  checkNcErr(l_err, __FILE__, __LINE__);


  *o_hu = new t_real[(*o_nx)*(*o_ny)];
  *o_hv = new t_real[(*o_nx)*(*o_ny)];
  *o_h = new t_real[(*o_nx)*(*o_ny)];
  *o_b = new t_real[(*o_nx)*(*o_ny)];

  l_err = nc_inq_varid(l_ncId, "momentum_x", &l_var_hu);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_get_var_float(l_ncId, l_var_hu, *o_hu);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_inq_varid(l_ncId, "momentum_y", &l_var_hv);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_get_var_float(l_ncId, l_var_hv, *o_hv);
  checkNcErr(l_err, __FILE__, __LINE__);


  l_err = nc_inq_varid(l_ncId, "height", &l_var_h);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_get_var_float(l_ncId, l_var_h, *o_h);
  checkNcErr(l_err, __FILE__, __LINE__);


  l_err = nc_inq_varid(l_ncId, "bathymetry", &l_var_b);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_get_var_float(l_ncId, l_var_b, *o_b);
  checkNcErr(l_err, __FILE__, __LINE__);


  l_err = nc_inq_varid(l_ncId, "damlocation", &l_var_damLocation);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_get_var_float(l_ncId, l_var_damLocation, o_damLocation);
  checkNcErr(l_err, __FILE__, __LINE__);


  //l_err = nc_inq_dimid(l_ncId, "time", &l_var_timeStep);
  //checkNcErr(l_err, __FILE__, __LINE__);

  //l_err = nc_inq_dimlen(l_ncId, l_var_timeStep, o_timeStep);
  //checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_close(l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);

}
