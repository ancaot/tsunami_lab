#include "NetCdf.h"

#include <cstring>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

tsunami_lab::io::NetCdf::NetCdf(t_idx i_nx,
                                t_idx i_ny,
                                const char * i_filename) {
  int l_ncId, l_err;
  int l_dimXId, l_dimYId, l_dimTimeId;
  int l_dim2Ids[2];
  int l_dim3Ids[3];

  l_err = nc_create(i_filename, NC_CLOBBER, &l_ncId);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_dim(l_ncId, "x", i_nx, &l_dimXId);
  checkNcErr(l_err, __FILE__, __LINE__);

  l_err = nc_def_dim(l_ncId, "y", i_ny, &l_dimYId);
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

  l_err = nc_def_var(l_ncId, "b", NC_FLOAT, 2, l_dim2Ids, &m_varIdBathymetry);
  checkNcErr(l_err, __FILE__, __LINE__);
  const char * l_unitsBathymetry = "meters";
  nc_put_att_text(l_ncId, m_varIdBathymetry, "units", strlen(l_unitsBathymetry), l_unitsBathymetry);

  l_dim3Ids[0] = l_dimTimeId;
  l_dim3Ids[1] = l_dimYId;
  l_dim3Ids[2] = l_dimXId;

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

  std::vector<t_real> l_coordinateX(i_nx);
  std::vector<t_real> l_coordinateY(i_ny);

  for (t_idx l_iy = 0; l_iy < i_ny; l_iy++) {
    l_coordinateY[l_iy] = ((l_iy + 0.5f) * i_dxy) + i_domainStartY;
  }

  l_err = nc_put_var_float(l_ncId, m_varIdY, l_coordinateY.data());
  checkNcErr(l_err, __FILE__, __LINE__);

  for (t_idx l_ix = 0; l_ix < i_nx; l_ix++) {
    l_coordinateX[l_ix] = ((l_ix + 0.5f) * i_dxy) + i_domainStartX;
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
  std::vector<t_real> l_grid(i_nx * i_ny);

  for (t_idx l_iy = 0; l_iy < i_ny; l_iy++) {
    for (t_idx l_ix = 0; l_ix < i_nx; l_ix++) {
      l_grid[l_iy * i_nx + l_ix] = i_grid[(l_iy + 1) * i_stride + (l_ix + 1)];
    }
  }

  std::vector<size_t> l_start;
  std::vector<size_t> l_count;
  std::vector<ptrdiff_t> l_stride;

  if (i_twoDimensionsOnly) {
    l_start = {0, 0};
    l_count = {i_ny, i_nx};
    l_stride = {1, 1};
  }
  else {
    l_start = {i_timeStep, 0, 0};
    l_count = {1, i_ny, i_nx};
    l_stride = {1, 1, 1};
  }

  int l_err = nc_put_vars_float(i_ncId, i_varId, l_start.data(), l_count.data(), l_stride.data(), l_grid.data());
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
