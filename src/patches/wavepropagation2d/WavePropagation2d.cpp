/**
 * @section DESCRIPTION
 * Two-dimensional wave propagation patch.
 **/
#include "WavePropagation2d.h"
#include "../../solvers/Roe.h"
#include "../../solvers/FWave.h"

tsunami_lab::patches::WavePropagation2d::WavePropagation2d(t_idx i_xCells,
                                                           t_idx i_yCells,
                                                           bool  i_choice,
                                                           bool  i_choiceBoundary) {
  m_choice = i_choice;
  m_choiceBoundry = i_choiceBoundary;
  m_xCells = i_xCells;
  m_yCells = i_yCells;

  t_idx l_size = (m_xCells + 2) * (m_yCells + 2);
  for (unsigned short l_st = 0; l_st < 2; l_st++) {
    m_h[l_st] = new t_real[l_size];
    m_hu[l_st] = new t_real[l_size];
    m_hv[l_st] = new t_real[l_size];
  }
  m_b = new t_real[l_size];

  #pragma omp parallel for schedule(runtime)
  for (std::ptrdiff_t l_ce = 0; l_ce < static_cast<std::ptrdiff_t>(l_size); l_ce++) {
    t_idx l_id = static_cast<t_idx>(l_ce);
    m_b[l_id] = 0;
    for (unsigned short l_st = 0; l_st < 2; l_st++) {
      m_h[l_st][l_id] = 0;
      m_hu[l_st][l_id] = 0;
      m_hv[l_st][l_id] = 0;
    }
  }
}

tsunami_lab::patches::WavePropagation2d::~WavePropagation2d() {
  for (unsigned short l_st = 0; l_st < 2; l_st++) {
    delete[] m_h[l_st];
    delete[] m_hu[l_st];
    delete[] m_hv[l_st];
  }
  delete[] m_b;
}

inline void tsunami_lab::patches::WavePropagation2d::copyOldToNew(t_idx l_size,
                                                                  t_real const * i_hOld,
                                                                  t_real const * i_huOld,
                                                                  t_real const * i_hvOld,
                                                                  t_real       * o_hNew,
                                                                  t_real       * o_huNew,
                                                                  t_real       * o_hvNew){
  #pragma omp parallel for schedule(runtime)
  for (std::ptrdiff_t l_ce = 0; l_ce < static_cast<std::ptrdiff_t>(l_size); l_ce++) {
    t_idx l_id = static_cast<t_idx>(l_ce);
    o_hNew[l_id] = i_hOld[l_id];
    o_huNew[l_id] = i_huOld[l_id];
    o_hvNew[l_id] = i_hvOld[l_id];
  }
}

inline void tsunami_lab::patches::WavePropagation2d::computeXImpulse(t_real i_scaling,
                                                                     t_real const * i_hOld,
                                                                     t_real const * i_momentumOld,
                                                                     t_real       * io_hNew,
                                                                     t_real       * io_momentumNew){
  #pragma omp parallel for schedule(runtime)
  for (std::ptrdiff_t l_eyRaw = 1; l_eyRaw < static_cast<std::ptrdiff_t>(m_yCells + 1); l_eyRaw++) {
    t_idx l_ey = static_cast<t_idx>(l_eyRaw);
    for (t_idx l_ex = 0; l_ex < m_xCells + 1; l_ex++) {
      t_real l_netUpdates[2][2];
      t_idx l_ceL = getIndex(l_ex, l_ey);
      t_idx l_ceR = getIndex(l_ex + 1, l_ey);

      if (m_choice) {
        solvers::Roe::netUpdates(i_hOld[l_ceL],
                                 i_hOld[l_ceR],
                                 i_momentumOld[l_ceL],
                                 i_momentumOld[l_ceR],
                                 l_netUpdates[0],
                                 l_netUpdates[1]);
      }
      else {
        solvers::fwave::netUpdates(i_hOld[l_ceL],
                                   i_hOld[l_ceR],
                                   i_momentumOld[l_ceL],
                                   i_momentumOld[l_ceR],
                                   m_b[l_ceL],
                                   m_b[l_ceR],
                                   l_netUpdates[0],
                                   l_netUpdates[1]);
      }

      io_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0];
      io_momentumNew[l_ceL] -= i_scaling * l_netUpdates[0][1];
      io_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0];
      io_momentumNew[l_ceR] -= i_scaling * l_netUpdates[1][1];
    }
  }
}

inline void tsunami_lab::patches::WavePropagation2d::computeYImpulse(t_real i_scaling,
                                                                     t_real const * i_hOld,
                                                                     t_real const * i_momentumOld,
                                                                     t_real       * io_hNew,
                                                                     t_real       * io_momentumNew){
  #pragma omp parallel for schedule(runtime)
  for (std::ptrdiff_t l_exRaw = 1; l_exRaw < static_cast<std::ptrdiff_t>(m_xCells + 1); l_exRaw++) {
    t_idx l_ex = static_cast<t_idx>(l_exRaw);
    for (t_idx l_ey = 0; l_ey < m_yCells + 1; l_ey++) {
      t_real l_netUpdates[2][2];
      t_idx l_ceB = getIndex(l_ex, l_ey);
      t_idx l_ceT = getIndex(l_ex, l_ey + 1);

      if (m_choice) {
        solvers::Roe::netUpdates(i_hOld[l_ceB],
                                 i_hOld[l_ceT],
                                 i_momentumOld[l_ceB],
                                 i_momentumOld[l_ceT],
                                 l_netUpdates[0],
                                 l_netUpdates[1]);
      }
      else {
        solvers::fwave::netUpdates(i_hOld[l_ceB],
                                   i_hOld[l_ceT],
                                   i_momentumOld[l_ceB],
                                   i_momentumOld[l_ceT],
                                   m_b[l_ceB],
                                   m_b[l_ceT],
                                   l_netUpdates[0],
                                   l_netUpdates[1]);
      }

      io_hNew[l_ceB] -= i_scaling * l_netUpdates[0][0];
      io_momentumNew[l_ceB] -= i_scaling * l_netUpdates[0][1];
      io_hNew[l_ceT] -= i_scaling * l_netUpdates[1][0];
      io_momentumNew[l_ceT] -= i_scaling * l_netUpdates[1][1];
    }
  }
}

void tsunami_lab::patches::WavePropagation2d::timeStep(t_real i_scaling) {
  t_idx l_size = (m_xCells + 2) * (m_yCells + 2);

  t_real * l_hOld = m_h[m_step];
  t_real * l_huOld = m_hu[m_step];
  t_real * l_hvOld = m_hv[m_step];

  m_step = (m_step + 1) % 2;
  t_real * l_hNew = m_h[m_step];
  t_real * l_huNew = m_hu[m_step];
  t_real * l_hvNew = m_hv[m_step];

  copyOldToNew(l_size, l_hOld, l_huOld, l_hvOld, l_hNew, l_huNew, l_hvNew);
  computeXImpulse(i_scaling, l_hOld, l_huOld, l_hNew, l_huNew);
  setGhostCollumn();

  l_hOld = m_h[m_step];
  l_huOld = m_hu[m_step];
  l_hvOld = m_hv[m_step];
  m_step = (m_step + 1) % 2;
  l_hNew = m_h[m_step];
  l_huNew = m_hu[m_step];
  l_hvNew = m_hv[m_step];

  copyOldToNew(l_size, l_hOld, l_huOld, l_hvOld, l_hNew, l_huNew, l_hvNew);
  computeYImpulse(i_scaling, l_hOld, l_hvOld, l_hNew, l_hvNew);
  setGhostCollumn();
}

void tsunami_lab::patches::WavePropagation2d::setGhostCollumn() {
  t_real * l_h = m_h[m_step];
  t_real * l_hu = m_hu[m_step];
  t_real * l_hv = m_hv[m_step];

  #pragma omp parallel for schedule(runtime)
  for (std::ptrdiff_t l_gRaw = 1; l_gRaw < static_cast<std::ptrdiff_t>(m_yCells + 1); l_gRaw++) {
    t_idx l_g = static_cast<t_idx>(l_gRaw);
    l_h[getIndex(0, l_g)] = l_h[getIndex(1, l_g)];
    l_h[getIndex(m_xCells + 1, l_g)] = l_h[getIndex(m_xCells, l_g)];
    m_b[getIndex(0, l_g)] = m_b[getIndex(1, l_g)];
    m_b[getIndex(m_xCells + 1, l_g)] = m_b[getIndex(m_xCells, l_g)];

    if (m_choiceBoundry) {
      l_hu[getIndex(0, l_g)] = -l_hu[getIndex(1, l_g)];
      l_hu[getIndex(m_xCells + 1, l_g)] = -l_hu[getIndex(m_xCells, l_g)];
    }
    else {
      l_hu[getIndex(0, l_g)] = l_hu[getIndex(1, l_g)];
      l_hu[getIndex(m_xCells + 1, l_g)] = l_hu[getIndex(m_xCells, l_g)];
    }
  }

  #pragma omp parallel for schedule(runtime)
  for (std::ptrdiff_t l_gRaw = 1; l_gRaw < static_cast<std::ptrdiff_t>(m_xCells + 1); l_gRaw++) {
    t_idx l_g = static_cast<t_idx>(l_gRaw);
    l_h[getIndex(l_g, 0)] = l_h[getIndex(l_g, 1)];
    l_h[getIndex(l_g, m_yCells + 1)] = l_h[getIndex(l_g, m_yCells)];
    m_b[getIndex(l_g, 0)] = m_b[getIndex(l_g, 1)];
    m_b[getIndex(l_g, m_yCells + 1)] = m_b[getIndex(l_g, m_yCells)];

    if (m_choiceBoundry) {
      l_hv[getIndex(l_g, 0)] = -l_hv[getIndex(l_g, 1)];
      l_hv[getIndex(l_g, m_yCells + 1)] = -l_hv[getIndex(l_g, m_yCells)];
    }
    else {
      l_hv[getIndex(l_g, 0)] = l_hv[getIndex(l_g, 1)];
      l_hv[getIndex(l_g, m_yCells + 1)] = l_hv[getIndex(l_g, m_yCells)];
    }
  }
}
