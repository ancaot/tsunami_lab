/**
 * Minimal 2D surrogate implementation: apply 1D updates along x for each
 * row independently. This is not a full 2D method but sufficient to
 * produce comparable time series along transects for educational tests.
 **/

#include "WavePropagation2d.h"
#include "../macros.h"
#include "../solvers/FWave.h"
#include "../solvers/Roe.h"

#include <cstring>
#include <algorithm>

tsunami_lab::patches::WavePropagation2d::WavePropagation2d( t_idx i_nx, t_idx i_ny ) {
  m_nx = i_nx;
  m_ny = i_ny;

  t_idx l_stride = m_nx + 2; // include ghost cells in x

  for( unsigned short l = 0; l < 2; l++ ){
    m_h[l] = new t_real[ (m_ny) * l_stride ];
    m_hu[l] = new t_real[ (m_ny) * l_stride ];
    m_hv[l] = new t_real[ (m_ny) * l_stride ];
  }
  m_b = new t_real[ (m_ny) * l_stride ];

  m_compactH = new t_real[ m_ny * m_nx ];
  m_compactHu = new t_real[ m_ny * m_nx ];
  m_compactHv = new t_real[ m_ny * m_nx ];

  // initialize
  std::memset( m_h[0], 0, sizeof(t_real) * m_ny * l_stride );
  std::memset( m_h[1], 0, sizeof(t_real) * m_ny * l_stride );
  std::memset( m_hu[0], 0, sizeof(t_real) * m_ny * l_stride );
  std::memset( m_hu[1], 0, sizeof(t_real) * m_ny * l_stride );
  std::memset( m_hv[0], 0, sizeof(t_real) * m_ny * l_stride );
  std::memset( m_hv[1], 0, sizeof(t_real) * m_ny * l_stride );
  std::memset( m_b, 0, sizeof(t_real) * m_ny * l_stride );
  std::memset( m_compactH, 0, sizeof(t_real) * m_ny * m_nx );
  std::memset( m_compactHu, 0, sizeof(t_real) * m_ny * m_nx );
  std::memset( m_compactHv, 0, sizeof(t_real) * m_ny * m_nx );
}

tsunami_lab::patches::WavePropagation2d::~WavePropagation2d(){
  for( unsigned short l = 0; l < 2; l++ ){
    delete[] m_h[l];
    delete[] m_hu[l];
    delete[] m_hv[l];
  }
  delete[] m_b;
  delete[] m_compactH;
  delete[] m_compactHu;
  delete[] m_compactHv;
}

void tsunami_lab::patches::WavePropagation2d::setGhostOutflow(){
  t_idx l_stride = m_nx + 2;
  t_real * l_h = m_h[m_step];
  t_real * l_hu = m_hu[m_step];
  t_real * l_hv = m_hv[m_step];

  for( t_idx l_row = 0; l_row < m_ny; l_row++ ){
    t_idx l_off = l_row * l_stride;
    // left ghost
    l_h[l_off + 0] = l_h[l_off + 1];
    l_hu[l_off + 0] = l_hu[l_off + 1];
    l_hv[l_off + 0] = l_hv[l_off + 1];
    // right ghost
    l_h[l_off + m_nx + 1] = l_h[l_off + m_nx];
    l_hu[l_off + m_nx + 1] = l_hu[l_off + m_nx];
    l_hv[l_off + m_nx + 1] = l_hv[l_off + m_nx];
  }
}

void tsunami_lab::patches::WavePropagation2d::timeStep( t_real i_scaling ){
  t_idx l_stride = m_nx + 2;
  t_real * l_hOld = m_h[m_step];
  t_real * l_huOld = m_hu[m_step];
  t_real * l_hvOld = m_hv[m_step];

  m_step = (m_step+1) % 2;

  t_real * l_hNew = m_h[m_step];
  t_real * l_huNew = m_hu[m_step];
  t_real * l_hvNew = m_hv[m_step];

  // copy old to new
  std::memcpy( l_hNew, l_hOld, sizeof(t_real) * m_ny * l_stride );
  std::memcpy( l_huNew, l_huOld, sizeof(t_real) * m_ny * l_stride );
  std::memcpy( l_hvNew, l_hvOld, sizeof(t_real) * m_ny * l_stride );

  // perform 1D updates on each row independently
  for( t_idx l_row = 0; l_row < m_ny; l_row++ ){
    t_idx l_off = l_row * l_stride;
    for( t_idx l_ed = 0; l_ed < m_nx+1; l_ed++ ){
      t_idx l_ceL = l_ed;
      t_idx l_ceR = l_ed+1;

      t_real l_hL = l_hOld[l_off + l_ceL];
      t_real l_hR = l_hOld[l_off + l_ceR];
      t_real l_huL = l_huOld[l_off + l_ceL];
      t_real l_huR = l_huOld[l_off + l_ceR];

      t_real l_netUpdates[2][2];

      switch(MODE){
        case MODEFWAVE:
          solvers::FWave::netUpdates( l_hL,
                                      l_hR,
                                      l_huL,
                                      l_huR,
                                      m_b[l_off + l_ceL],
                                      m_b[l_off + l_ceR],
                                      l_netUpdates[0],
                                      l_netUpdates[1] );
          break;
        case MODEROE:
          solvers::Roe::netUpdates( l_hL,
                                    l_hR,
                                    l_huL,
                                    l_huR,
                                    l_netUpdates[0],
                                    l_netUpdates[1] );
          break;
      }

      // update
      l_hNew[l_off + l_ceL]  -= i_scaling * l_netUpdates[0][0];
      l_huNew[l_off + l_ceL] -= i_scaling * l_netUpdates[0][1];

      l_hNew[l_off + l_ceR]  -= i_scaling * l_netUpdates[1][0];
      l_huNew[l_off + l_ceR] -= i_scaling * l_netUpdates[1][1];
    }
  }

  // fill compact arrays (strip ghost cells)
  for( t_idx l_row = 0; l_row < m_ny; l_row++ ){
    t_idx l_off = l_row * l_stride;
    for( t_idx l_col = 0; l_col < m_nx; l_col++ ){
      t_idx l_src = l_off + 1 + l_col;
      t_idx l_dst = l_row * m_nx + l_col;
      m_compactH[l_dst] = l_hNew[l_src];
      m_compactHu[l_dst] = l_huNew[l_src];
      m_compactHv[l_dst] = l_hvNew[l_src];
    }
  }
}

t_idx tsunami_lab::patches::WavePropagation2d::getStride(){
  return m_nx;
}

t_real const * tsunami_lab::patches::WavePropagation2d::getHeight(){
  return m_compactH;
}

t_real const * tsunami_lab::patches::WavePropagation2d::getMomentumX(){
  return m_compactHu;
}

t_real const * tsunami_lab::patches::WavePropagation2d::getMomentumY(){
  return m_compactHv;
}

void tsunami_lab::patches::WavePropagation2d::setHeight( t_idx i_ix, t_idx i_iy, t_real i_h ){
  t_idx l_stride = m_nx + 2;
  m_h[m_step][ i_iy * l_stride + 1 + i_ix ] = i_h;
}

void tsunami_lab::patches::WavePropagation2d::setMomentumX( t_idx i_ix, t_idx i_iy, t_real i_hu ){
  t_idx l_stride = m_nx + 2;
  m_hu[m_step][ i_iy * l_stride + 1 + i_ix ] = i_hu;
}

void tsunami_lab::patches::WavePropagation2d::setMomentumY( t_idx i_ix, t_idx i_iy, t_real i_hv ){
  t_idx l_stride = m_nx + 2;
  m_hv[m_step][ i_iy * l_stride + 1 + i_ix ] = i_hv;
}
