/**
 * @section DESCRIPTION
 * Two-dimensional wave propagation patch.
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

 tsunami_lab::patches::WavePropagation2d::WavePropagation2d(t_idx i_nx, t_idx i_ny){
    m_nx = i_nx;
    m_ny = i_ny;

    // allocate memory including a single ghost cell on each side
    t_idx l_stride = m_nx + 2; // include ghost cells in x

    for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
        m_h[l_st] = new t_real[ (m_ny + 2) * l_stride ];
        m_hu[l_st] = new t_real[ (m_ny + 2) * l_stride ];
        m_hv[l_st] = new t_real[ (m_ny + 2) * l_stride ];
    }
    m_b = new t_real[ (m_ny + 2) * l_stride ];


    m_compactH = new t_real[ m_ny * m_nx ];
    m_compactHu = new t_real[ m_ny * m_nx ];
    m_compactHv = new t_real[ m_ny * m_nx ];
    m_compactB = new t_real[ m_ny * m_nx ];
    // init to zero
    for( t_idx l_ce = 0; l_ce < ((m_ny + 2) * l_stride); l_ce++ ) {
        for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
          m_h[l_st][l_ce] = 0;
          m_hu[l_st][l_ce] = 0;
          m_hv[l_st][l_ce] = 0;
        }
        m_b[l_ce] = 0;
    }
}

tsunami_lab::patches::WavePropagation2d::~WavePropagation2d() {
    for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
        delete[] m_h[l_st];
        delete[] m_hu[l_st];
        delete [] m_hv[l_st];
    }
    delete[] m_b;
    delete[] m_compactH;
    delete[] m_compactHu;
    delete[] m_compactHv;
    delete[] m_compactB;
}

void tsunami_lab::patches::WavePropagation2d::timeStep(t_real i_scaling){
    t_idx l_stride = m_nx + 2; // include ghost cells in x

    // pointers to old and new data
    t_real * l_hOld = m_h[m_step];
    t_real * l_huOld = m_hu[m_step];
    t_real * l_hvOld = m_hv[m_step];

    m_step = (m_step+1) % 2;

    t_real * l_hNew =  m_h[m_step];
    t_real * l_huNew = m_hu[m_step];
    t_real * l_hvNew = m_hv[m_step];

    // init new cell quantities
    for( t_idx l_ce = 0; l_ce < (m_ny + 2) * l_stride; l_ce++ ) {
        l_hNew[l_ce] = l_hOld[l_ce];
        l_huNew[l_ce] = l_huOld[l_ce];
        l_hvNew[l_ce] = l_hvOld[l_ce];
    }

    // iterate over X edges and update with Riemann solutions
    for( t_idx l_row = 1; l_row <= m_ny; l_row++ ){
      t_idx l_off = l_row * l_stride;
      for( t_idx l_col = 0; l_col < m_nx + 1; l_col++ ) {
        t_idx l_ceL = l_col;
        t_idx l_ceR = l_col+1;

        t_real l_netUpdates[2][2];
        solvers::FWave::netUpdates( l_hOld[l_off + l_ceL],
                              l_hOld[l_off + l_ceR],
                              l_huOld[l_off + l_ceL],
                              l_huOld[l_off + l_ceR],
                              m_b[l_off + l_ceL],
                              m_b[l_off + l_ceR],
                              l_netUpdates[0],
                              l_netUpdates[1] );

        l_hNew[l_off + l_ceL]  -= i_scaling * l_netUpdates[0][0];
        l_huNew[l_off + l_ceL] -= i_scaling * l_netUpdates[0][1];

        l_hNew[l_off + l_ceR]  -= i_scaling * l_netUpdates[1][0];
        l_huNew[l_off + l_ceR] -= i_scaling * l_netUpdates[1][1];
      }
    }

    // copy x-updated h, hu, hv back to old for the y-sweep
    for( t_idx l_ce = 0; l_ce < (m_ny+2) * l_stride; l_ce++ ) {
        l_hOld[l_ce] = l_hNew[l_ce];
        l_huOld[l_ce] = l_huNew[l_ce];
        l_hvOld[l_ce] = l_hvNew[l_ce];
    }
    
    // update top/bottom ghost boundaries for Y sweep
    for( t_idx l_col = 0; l_col < l_stride; l_col++ ) {
      l_hOld[l_col] = l_hOld[l_stride + l_col];
      l_huOld[l_col] = l_huOld[l_stride + l_col];
      l_hvOld[l_col] = l_hvOld[l_stride + l_col];
      
      l_hOld[(m_ny + 1) * l_stride + l_col] = l_hOld[m_ny * l_stride + l_col];
      l_huOld[(m_ny + 1) * l_stride + l_col] = l_huOld[m_ny * l_stride + l_col];
      l_hvOld[(m_ny + 1) * l_stride + l_col] = l_hvOld[m_ny * l_stride + l_col];
    }

    // iterate over Y edges
    for( t_idx l_row = 0; l_row < m_ny + 1; l_row++ ){
      t_idx l_offL = l_row * l_stride;
      t_idx l_offR = (l_row+1) * l_stride;
      for( t_idx l_col = 1; l_col <= m_nx; l_col++ ) {
        t_real l_netUpdates[2][2];
        solvers::FWave::netUpdates( l_hOld[l_offL + l_col],
                              l_hOld[l_offR + l_col],
                              l_hvOld[l_offL + l_col],
                              l_hvOld[l_offR + l_col],
                              m_b[l_offL + l_col],
                              m_b[l_offR + l_col],
                              l_netUpdates[0],
                              l_netUpdates[1] );

        l_hNew[l_offL + l_col]  -= i_scaling * l_netUpdates[0][0];
        l_hvNew[l_offL + l_col] -= i_scaling * l_netUpdates[0][1];

        l_hNew[l_offR + l_col]  -= i_scaling * l_netUpdates[1][0];
        l_hvNew[l_offR + l_col] -= i_scaling * l_netUpdates[1][1];
      }
    }

  // fill compact arrays (strip ghost cells)
  for( t_idx l_row = 0; l_row < m_ny; l_row++ ){
    t_idx l_off = (l_row + 1) * l_stride; // shift by 1 for Y ghost
    for( t_idx l_col = 0; l_col < m_nx; l_col++ ){
      t_idx l_src = l_off + 1 + l_col;
      t_idx l_dst = l_row * m_nx + l_col;
      m_compactH[l_dst] = l_hNew[l_src];
      m_compactHu[l_dst] = l_huNew[l_src];
      m_compactHv[l_dst] = l_hvNew[l_src];
      m_compactB[l_dst] = m_b[l_src];
    }
  }

}

void tsunami_lab::patches::WavePropagation2d::setGhostOutflow() {

  t_idx l_stride = m_nx + 2;
  t_real * l_h = m_h[m_step];
  t_real * l_hu = m_hu[m_step];
  t_real * l_hv = m_hv[m_step];
  
  for( t_idx l_row = 1; l_row <= m_ny; l_row++ ){
    t_idx l_off = l_row * l_stride;
    // left ghost
    l_h[l_off + 0] = l_h[l_off + 1];
    l_hu[l_off + 0] = l_hu[l_off + 1];
    l_hv[l_off + 0] = l_hv[l_off + 1];
    m_b[l_off + 0] = m_b[l_off + 1];
    // right ghost
    l_h[l_off + m_nx + 1] = l_h[l_off + m_nx];
    l_hu[l_off + m_nx + 1] = l_hu[l_off + m_nx];
    l_hv[l_off + m_nx + 1] = l_hv[l_off + m_nx];
    m_b[l_off + m_nx + 1] = m_b[l_off + m_nx];
  }

  // top and bottom ghosts
  for( t_idx l_col = 0; l_col < l_stride; l_col++ ) {
    // bottom ghost (row 0)
    l_h[l_col] = l_h[l_stride + l_col];
    l_hu[l_col] = l_hu[l_stride + l_col];
    l_hv[l_col] = l_hv[l_stride + l_col];
    m_b[l_col] = m_b[l_stride + l_col];

    // top ghost (row m_ny + 1)
    l_h[(m_ny + 1) * l_stride + l_col] = l_h[m_ny * l_stride + l_col];
    l_hu[(m_ny + 1) * l_stride + l_col] = l_hu[m_ny * l_stride + l_col];
    l_hv[(m_ny + 1) * l_stride + l_col] = l_hv[m_ny * l_stride + l_col];
    m_b[(m_ny + 1) * l_stride + l_col] = m_b[m_ny * l_stride + l_col];
  }
}

tsunami_lab::t_idx tsunami_lab::patches::WavePropagation2d::getStride(){
  return m_nx;
}

tsunami_lab::t_real const * tsunami_lab::patches::WavePropagation2d::getHeight(){
  return m_compactH;
}

tsunami_lab::t_real const * tsunami_lab::patches::WavePropagation2d::getMomentumX(){
  return m_compactHu;
}

tsunami_lab::t_real const * tsunami_lab::patches::WavePropagation2d::getMomentumY(){
  return m_compactHv;
}

tsunami_lab::t_real const * tsunami_lab::patches::WavePropagation2d::getBathymetry(){
  return m_compactB;
}

void tsunami_lab::patches::WavePropagation2d::setHeight( t_idx i_ix, t_idx i_iy, t_real i_h ){
  t_idx l_stride = m_nx + 2;
  m_h[m_step][ (i_iy + 1) * l_stride + 1 + i_ix ] = i_h;
  m_compactH[ i_iy * m_nx + i_ix ] = i_h;
}

void tsunami_lab::patches::WavePropagation2d::setMomentumX( t_idx i_ix, t_idx i_iy, t_real i_hu ){
  t_idx l_stride = m_nx + 2;
  m_hu[m_step][ (i_iy + 1) * l_stride + 1 + i_ix ] = i_hu;
  m_compactHu[ i_iy * m_nx + i_ix ] = i_hu;
}

void tsunami_lab::patches::WavePropagation2d::setMomentumY( t_idx i_ix, t_idx i_iy, t_real i_hv ){
  t_idx l_stride = m_nx + 2;
  m_hv[m_step][ (i_iy + 1) * l_stride + 1 + i_ix ] = i_hv;
  m_compactHv[ i_iy * m_nx + i_ix ] = i_hv;
}
void tsunami_lab::patches::WavePropagation2d::setBathymetry( t_idx i_ix, t_idx i_iy, t_real i_b ){
  t_idx l_stride = m_nx + 2;
  m_b[ (i_iy + 1) * l_stride + 1 + i_ix ] = i_b;
  if(!m_compactB) m_compactB = new t_real[m_nx * m_ny];
  m_compactB[ i_iy * m_nx + i_ix ] = i_b;
}
