/**
 * @section DESCRIPTION
 * Two-dimensional wave propagation patch.
 **/
#include "WavePropagation2d.h"
#include "../../solvers/Roe.h"
#include "../../solvers/fwave.h"

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
    m_h[l_st] = new t_real[l_size]{};
    m_hu[l_st] = new t_real[l_size]{};
    m_hv[l_st] = new t_real[l_size]{};
  }
  m_b = new t_real[l_size]{};
}

tsunami_lab::patches::WavePropagation2d::~WavePropagation2d() {
  for (unsigned short l_st = 0; l_st < 2; l_st++) {
    delete[] m_h[l_st];
    delete[] m_hu[l_st];
    delete[] m_hv[l_st];
  }
  delete[] m_b;
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

  for (t_idx l_ce = 0; l_ce < l_size; l_ce++) {
    l_hNew[l_ce] = l_hOld[l_ce];
    l_huNew[l_ce] = l_huOld[l_ce];
    l_hvNew[l_ce] = l_hvOld[l_ce];
  }

  for (t_idx l_ex = 0; l_ex < m_xCells + 1; l_ex++) {
    for (t_idx l_ey = 1; l_ey < m_yCells + 1; l_ey++) {
      t_real l_netUpdates[2][2];
      t_idx l_ceL = getIndex(l_ex, l_ey);
      t_idx l_ceR = getIndex(l_ex + 1, l_ey);

      if (m_choice) {
        solvers::Roe::netUpdates(l_hOld[l_ceL],
                                 l_hOld[l_ceR],
                                 l_huOld[l_ceL],
                                 l_huOld[l_ceR],
                                 l_netUpdates[0],
                                 l_netUpdates[1]);
      }
      else {
        solvers::fwave::netUpdates(l_hOld[l_ceL],
                                   l_hOld[l_ceR],
                                   l_huOld[l_ceL],
                                   l_huOld[l_ceR],
                                   m_b[l_ceL],
                                   m_b[l_ceR],
                                   l_netUpdates[0],
                                   l_netUpdates[1]);
      }

      l_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0];
      l_huNew[l_ceL] -= i_scaling * l_netUpdates[0][1];
      l_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0];
      l_huNew[l_ceR] -= i_scaling * l_netUpdates[1][1];
    }
  }

  setGhostCollumn();

  l_hOld = m_h[m_step];
  l_huOld = m_hu[m_step];
  l_hvOld = m_hv[m_step];

  m_step = (m_step + 1) % 2;
  l_hNew = m_h[m_step];
  l_huNew = m_hu[m_step];
  l_hvNew = m_hv[m_step];

  for (t_idx l_ce = 0; l_ce < l_size; l_ce++) {
    l_hNew[l_ce] = l_hOld[l_ce];
    l_huNew[l_ce] = l_huOld[l_ce];
    l_hvNew[l_ce] = l_hvOld[l_ce];
  }

  for (t_idx l_ex = 1; l_ex < m_xCells + 1; l_ex++) {
    for (t_idx l_ey = 0; l_ey < m_yCells + 1; l_ey++) {
      t_real l_netUpdates[2][2];
      t_idx l_ceL = getIndex(l_ex, l_ey);
      t_idx l_ceR = getIndex(l_ex, l_ey + 1);

      if (m_choice) {
        solvers::Roe::netUpdates(l_hOld[l_ceL],
                                 l_hOld[l_ceR],
                                 l_hvOld[l_ceL],
                                 l_hvOld[l_ceR],
                                 l_netUpdates[0],
                                 l_netUpdates[1]);
      }
      else {
        solvers::fwave::netUpdates(l_hOld[l_ceL],
                                   l_hOld[l_ceR],
                                   l_hvOld[l_ceL],
                                   l_hvOld[l_ceR],
                                   m_b[l_ceL],
                                   m_b[l_ceR],
                                   l_netUpdates[0],
                                   l_netUpdates[1]);
      }

      l_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0];
      l_hvNew[l_ceL] -= i_scaling * l_netUpdates[0][1];
      l_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0];
      l_hvNew[l_ceR] -= i_scaling * l_netUpdates[1][1];
    }
  }

  setGhostCollumn();
}

void tsunami_lab::patches::WavePropagation2d::setGhostCollumn() {
  t_real * l_h = m_h[m_step];
  t_real * l_hu = m_hu[m_step];
  t_real * l_hv = m_hv[m_step];

  for (t_idx l_g = 1; l_g < m_yCells + 1; l_g++) {
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

  for (t_idx l_g = 1; l_g < m_xCells + 1; l_g++) {
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
