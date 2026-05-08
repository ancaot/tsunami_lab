#include "NetCdf.h"

#ifdef USE_NETCDF

#include <netcdf.h>

#include <iostream>
#include <vector>

bool tsunami_lab::io::NetCdf::check( int i_errorCode,
                                     std::string const & i_operation ) const {
  if( i_errorCode == NC_NOERR ) return true;

  std::cerr << "netCDF error in " << i_operation << ": "
            << nc_strerror( i_errorCode ) << std::endl;
  return false;
}

bool tsunami_lab::io::NetCdf::defineFileLayout( t_real i_dxy ) {
  if( !check( nc_def_dim( m_fileId, "time", NC_UNLIMITED, &m_dimTimeId ), "nc_def_dim(time)" ) ) return false;
  if( !check( nc_def_dim( m_fileId, "y", static_cast< size_t >( m_ny ), &m_dimYId ), "nc_def_dim(y)" ) ) return false;
  if( !check( nc_def_dim( m_fileId, "x", static_cast< size_t >( m_nx ), &m_dimXId ), "nc_def_dim(x)" ) ) return false;

  if( !check( nc_def_var( m_fileId, "time", NC_FLOAT, 1, &m_dimTimeId, &m_varTimeId ), "nc_def_var(time)" ) ) return false;
  if( !check( nc_def_var( m_fileId, "y", NC_FLOAT, 1, &m_dimYId, &m_varYId ), "nc_def_var(y)" ) ) return false;
  if( !check( nc_def_var( m_fileId, "x", NC_FLOAT, 1, &m_dimXId, &m_varXId ), "nc_def_var(x)" ) ) return false;

  int l_dims3[3] = { m_dimTimeId, m_dimYId, m_dimXId };

  if( !check( nc_def_var( m_fileId, "height", NC_FLOAT, 3, l_dims3, &m_varHeightId ), "nc_def_var(height)" ) ) return false;
  if( !check( nc_def_var( m_fileId, "momentum_x", NC_FLOAT, 3, l_dims3, &m_varMomentumXId ), "nc_def_var(momentum_x)" ) ) return false;
  if( m_includeMomentumY ) {
    if( !check( nc_def_var( m_fileId, "momentum_y", NC_FLOAT, 3, l_dims3, &m_varMomentumYId ), "nc_def_var(momentum_y)" ) ) return false;
  }

  char const * l_conventions = "COARDS";
  char const * l_title = "Tsunami Lab wave simulation";
  if( !check( nc_put_att_text( m_fileId, NC_GLOBAL, "Conventions", sizeof("COARDS") - 1, l_conventions ), "nc_put_att_text(Conventions)" ) ) return false;
  if( !check( nc_put_att_text( m_fileId, NC_GLOBAL, "title", sizeof("Tsunami Lab wave simulation") - 1, l_title ), "nc_put_att_text(title)" ) ) return false;

  if( !check( nc_put_att_text( m_fileId, m_varTimeId, "units", sizeof("seconds since the earthquake event") - 1, "seconds since the earthquake event" ), "nc_put_att_text(time:units)" ) ) return false;
  if( !check( nc_put_att_text( m_fileId, m_varTimeId, "long_name", sizeof("simulation time") - 1, "simulation time" ), "nc_put_att_text(time:long_name)" ) ) return false;

  if( !check( nc_put_att_text( m_fileId, m_varXId, "units", sizeof("m") - 1, "m" ), "nc_put_att_text(x:units)" ) ) return false;
  if( !check( nc_put_att_text( m_fileId, m_varXId, "long_name", sizeof("x coordinate of cell") - 1, "x coordinate of cell" ), "nc_put_att_text(x:long_name)" ) ) return false;

  if( !check( nc_put_att_text( m_fileId, m_varYId, "units", sizeof("m") - 1, "m" ), "nc_put_att_text(y:units)" ) ) return false;
  if( !check( nc_put_att_text( m_fileId, m_varYId, "long_name", sizeof("y coordinate of cell") - 1, "y coordinate of cell" ), "nc_put_att_text(y:long_name)" ) ) return false;

  if( !check( nc_put_att_text( m_fileId, m_varHeightId, "units", sizeof("m") - 1, "m" ), "nc_put_att_text(height:units)" ) ) return false;
  if( !check( nc_put_att_text( m_fileId, m_varHeightId, "long_name", sizeof("water height") - 1, "water height" ), "nc_put_att_text(height:long_name)" ) ) return false;

  if( !check( nc_put_att_text( m_fileId, m_varMomentumXId, "units", sizeof("m^2/s") - 1, "m^2/s" ), "nc_put_att_text(momentum_x:units)" ) ) return false;
  if( !check( nc_put_att_text( m_fileId, m_varMomentumXId, "long_name", sizeof("momentum in x-dir") - 1, "momentum in x-dir" ), "nc_put_att_text(momentum_x:long_name)" ) ) return false;

  if( m_includeMomentumY ) {
    if( !check( nc_put_att_text( m_fileId, m_varMomentumYId, "units", sizeof("m^2/s") - 1, "m^2/s" ), "nc_put_att_text(momentum_y:units)" ) ) return false;
    if( !check( nc_put_att_text( m_fileId, m_varMomentumYId, "long_name", sizeof("momentum in y-dir") - 1, "momentum in y-dir" ), "nc_put_att_text(momentum_y:long_name)" ) ) return false;
  }

  if( !check( nc_enddef( m_fileId ), "nc_enddef" ) ) return false;

  return writeCoordinates( i_dxy );
}

bool tsunami_lab::io::NetCdf::writeCoordinates( t_real i_dxy ) {
  std::vector< t_real > l_x( m_nx );
  std::vector< t_real > l_y( m_ny );

  for( t_idx l_ix = 0; l_ix < m_nx; l_ix++ ) {
    l_x[l_ix] = ( static_cast< t_real >( l_ix ) + static_cast< t_real >( 0.5 ) ) * i_dxy;
  }

  for( t_idx l_iy = 0; l_iy < m_ny; l_iy++ ) {
    l_y[l_iy] = ( static_cast< t_real >( l_iy ) + static_cast< t_real >( 0.5 ) ) * i_dxy;
  }

  if( !check( nc_put_var_float( m_fileId, m_varXId, l_x.data() ), "nc_put_var_float(x)" ) ) return false;
  if( !check( nc_put_var_float( m_fileId, m_varYId, l_y.data() ), "nc_put_var_float(y)" ) ) return false;

  return true;
}

tsunami_lab::io::NetCdf::NetCdf( std::string const & i_path,
                                 t_real              i_dxy,
                                 t_idx               i_nx,
                                 t_idx               i_ny,
                                 bool                i_includeMomentumY ) {
  m_nx = i_nx;
  m_ny = i_ny;
  m_includeMomentumY = i_includeMomentumY;
  m_readMode = false;

  int l_err = nc_create( i_path.c_str(), NC_CLOBBER, &m_fileId );
  if( !check( l_err, "nc_create" ) ) {
    m_isOpen = false;
    return;
  }

  m_isOpen = defineFileLayout( i_dxy );

  if( !m_isOpen && m_fileId >= 0 ) {
    nc_close( m_fileId );
    m_fileId = -1;
  }
}

// Constructor for reading existing netCDF files
tsunami_lab::io::NetCdf::NetCdf( std::string const & i_path ) {
  m_readMode = true;
  m_numTimeSteps = 0;

  int l_err = nc_open( i_path.c_str(), NC_NOWRITE, &m_fileId );
  if( !check( l_err, "nc_open" ) ) {
    m_isOpen = false;
    return;
  }

  // Inquire dimensions
  int l_dimXId, l_dimYId, l_dimTimeId;
  size_t l_nx, l_ny, l_numTimeSteps;

  if( !check( nc_inq_dimid( m_fileId, "x", &l_dimXId ), "nc_inq_dimid(x)" ) ) {
    nc_close( m_fileId );
    m_fileId = -1;
    return;
  }

  if( !check( nc_inq_dimlen( m_fileId, l_dimXId, &l_nx ), "nc_inq_dimlen(x)" ) ) {
    nc_close( m_fileId );
    m_fileId = -1;
    return;
  }

  if( !check( nc_inq_dimid( m_fileId, "y", &l_dimYId ), "nc_inq_dimid(y)" ) ) {
    nc_close( m_fileId );
    m_fileId = -1;
    return;
  }

  if( !check( nc_inq_dimlen( m_fileId, l_dimYId, &l_ny ), "nc_inq_dimlen(y)" ) ) {
    nc_close( m_fileId );
    m_fileId = -1;
    return;
  }

  if( !check( nc_inq_dimid( m_fileId, "time", &l_dimTimeId ), "nc_inq_dimid(time)" ) ) {
    nc_close( m_fileId );
    m_fileId = -1;
    return;
  }

  if( !check( nc_inq_dimlen( m_fileId, l_dimTimeId, &l_numTimeSteps ), "nc_inq_dimlen(time)" ) ) {
    nc_close( m_fileId );
    m_fileId = -1;
    return;
  }

  // Store dimensions
  m_nx = static_cast< t_idx >( l_nx );
  m_ny = static_cast< t_idx >( l_ny );
  m_numTimeSteps = static_cast< t_idx >( l_numTimeSteps );

  // Inquire variable IDs
  int l_varTimeId;
  if( !check( nc_inq_varid( m_fileId, "time", &l_varTimeId ), "nc_inq_varid(time)" ) ) {
    nc_close( m_fileId );
    m_fileId = -1;
    return;
  }

  m_varTimeId = l_varTimeId;
  m_isOpen = true;
}


tsunami_lab::io::NetCdf::~NetCdf() {
  if( m_fileId >= 0 ) {
    nc_close( m_fileId );
    m_fileId = -1;
  }
}

bool tsunami_lab::io::NetCdf::writeTimeStep( t_real               i_time,
                                             t_idx                i_stride,
                                             t_real       const * i_h,
                                             t_real       const * i_hu,
                                             t_real       const * i_hv,
                                             t_idx                i_ghostX,
                                             t_idx                i_ghostY ) {
  if( !m_isOpen ) return false;
  if( i_h == nullptr || i_hu == nullptr ) return false;

  std::vector< t_real > l_height( m_nx * m_ny );
  std::vector< t_real > l_momentumX( m_nx * m_ny );
  std::vector< t_real > l_momentumY;

  if( m_includeMomentumY ) {
    if( i_hv == nullptr ) return false;
    l_momentumY.resize( m_nx * m_ny );
  }

  for( t_idx l_iy = 0; l_iy < m_ny; l_iy++ ) {
    t_idx l_srcRow = ( l_iy + i_ghostY ) * i_stride + i_ghostX;
    t_idx l_dstRow = l_iy * m_nx;

    for( t_idx l_ix = 0; l_ix < m_nx; l_ix++ ) {
      l_height[l_dstRow + l_ix] = i_h[l_srcRow + l_ix];
      l_momentumX[l_dstRow + l_ix] = i_hu[l_srcRow + l_ix];

      if( m_includeMomentumY ) {
        l_momentumY[l_dstRow + l_ix] = i_hv[l_srcRow + l_ix];
      }
    }
  }

  size_t l_startTime[1] = { static_cast< size_t >( m_timeStep ) };
  size_t l_countTime[1] = { 1 };

  if( !check( nc_put_vara_float( m_fileId, m_varTimeId, l_startTime, l_countTime, &i_time ), "nc_put_vara_float(time)" ) ) return false;

  size_t l_start3[3] = { static_cast< size_t >( m_timeStep ), 0, 0 };
  size_t l_count3[3] = { 1, static_cast< size_t >( m_ny ), static_cast< size_t >( m_nx ) };

  if( !check( nc_put_vara_float( m_fileId, m_varHeightId, l_start3, l_count3, l_height.data() ), "nc_put_vara_float(height)" ) ) return false;
  if( !check( nc_put_vara_float( m_fileId, m_varMomentumXId, l_start3, l_count3, l_momentumX.data() ), "nc_put_vara_float(momentum_x)" ) ) return false;

  if( m_includeMomentumY ) {
    if( !check( nc_put_vara_float( m_fileId, m_varMomentumYId, l_start3, l_count3, l_momentumY.data() ), "nc_put_vara_float(momentum_y)" ) ) return false;
  }

  if( !check( nc_sync( m_fileId ), "nc_sync" ) ) return false;

  m_timeStep++;
  return true;
}

bool tsunami_lab::io::NetCdf::readTimeStep( t_idx                i_timeStep,
                                            t_real       * o_h,
                                            t_real       * o_hu,
                                            t_real       * o_hv ) {
  if( !m_isOpen || !m_readMode ) return false;
  if( i_timeStep >= m_numTimeSteps ) return false;
  if( o_h == nullptr || o_hu == nullptr ) return false;

  int l_varHeightId, l_varMomentumXId, l_varMomentumYId = -1;

  // Inquire variable IDs for this timestep
  if( !check( nc_inq_varid( m_fileId, "height", &l_varHeightId ), "nc_inq_varid(height)" ) ) return false;
  if( !check( nc_inq_varid( m_fileId, "momentum_x", &l_varMomentumXId ), "nc_inq_varid(momentum_x)" ) ) return false;

  // momentum_y is optional
  bool l_hasMomentumY = ( nc_inq_varid( m_fileId, "momentum_y", &l_varMomentumYId ) == NC_NOERR );

  // Read data for this timestep
  size_t l_start[3] = { static_cast< size_t >( i_timeStep ), 0, 0 };
  size_t l_count[3] = { 1, static_cast< size_t >( m_ny ), static_cast< size_t >( m_nx ) };

  if( !check( nc_get_vara_float( m_fileId, l_varHeightId, l_start, l_count, o_h ), "nc_get_vara_float(height)" ) ) return false;
  if( !check( nc_get_vara_float( m_fileId, l_varMomentumXId, l_start, l_count, o_hu ), "nc_get_vara_float(momentum_x)" ) ) return false;

  if( o_hv != nullptr && l_hasMomentumY ) {
    if( !check( nc_get_vara_float( m_fileId, l_varMomentumYId, l_start, l_count, o_hv ), "nc_get_vara_float(momentum_y)" ) ) return false;
  }

  return true;
}

tsunami_lab::t_real tsunami_lab::io::NetCdf::getTime( t_idx i_timeStep ) const {
  if( !m_isOpen || !m_readMode ) return 0.0;
  if( i_timeStep >= m_numTimeSteps ) return 0.0;

  t_real l_time = 0.0;
  size_t l_start[1] = { static_cast< size_t >( i_timeStep ) };
  size_t l_count[1] = { 1 };

  int l_err = nc_get_vara_float( m_fileId, m_varTimeId, l_start, l_count, &l_time );
  if( l_err != NC_NOERR ) {
    std::cerr << "netCDF error in getTime: " << nc_strerror( l_err ) << std::endl;
    return 0.0;
  }

  return l_time;
}

#else

#include <iostream>

tsunami_lab::io::NetCdf::NetCdf( std::string const &,
                                 t_real,
                                 t_idx,
                                 t_idx,
                                 bool ) {
  std::cerr << "netCDF support is disabled at compile time." << std::endl;
}

tsunami_lab::io::NetCdf::NetCdf( std::string const & ) {
  std::cerr << "netCDF support is disabled at compile time." << std::endl;
}

tsunami_lab::io::NetCdf::~NetCdf() {}

bool tsunami_lab::io::NetCdf::check( int,
                                     std::string const & ) const {
  return false;
}

bool tsunami_lab::io::NetCdf::defineFileLayout( t_real ) {
  return false;
}

bool tsunami_lab::io::NetCdf::writeCoordinates( t_real ) {
  return false;
}

bool tsunami_lab::io::NetCdf::writeTimeStep( t_real,
                                             t_idx,
                                             t_real const *,
                                             t_real const *,
                                             t_real const *,
                                             t_idx,
                                             t_idx ) {
  return false;
}

bool tsunami_lab::io::NetCdf::readTimeStep( t_idx,
                                            t_real *,
                                            t_real *,
                                            t_real * ) {
  return false;
}

tsunami_lab::t_real tsunami_lab::io::NetCdf::getTime( t_idx ) const {
  return 0.0;
}

#endif
