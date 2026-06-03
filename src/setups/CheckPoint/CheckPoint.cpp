#include "CheckPoint.h"
#include <cmath>

tsunami_lab::setups::CheckPoint::CheckPoint(t_real i_domainSizeX,
                                            t_real i_domainSizeY,
                                            t_idx i_nx,
                                            t_idx i_ny,
                                            t_real i_domainStartX,
                                            t_real i_domainStartY,
                                            t_real* i_h,
                                            t_real* i_hu,
                                            t_real* i_hv,
                                            t_real* i_b) {
  m_domainSizeX = i_domainSizeX;
  m_domainSizeY = i_domainSizeY;
  m_nx = i_nx;
  m_ny = i_ny;
  m_domainStartX = i_domainStartX;
  m_domainStartY = i_domainStartY;
  m_h = i_h;
  m_hu = i_hu;
  m_hv = i_hv;
  m_b = i_b;
  m_dxy = m_domainSizeX / m_nx;
}

tsunami_lab::setups::CheckPoint::~CheckPoint() {
  delete[] m_h;
  delete[] m_hu;
  delete[] m_hv;
  delete[] m_b;
}

tsunami_lab::t_real tsunami_lab::setups::CheckPoint::getHeight(t_real i_x, t_real i_y) const {
  if (i_x < m_domainStartX || i_x > m_domainStartX + m_domainSizeX ||
      i_y < m_domainStartY || i_y > m_domainStartY + m_domainSizeY) {
    return 0;
  }
  t_idx l_ix = (i_x - m_domainStartX) / m_dxy;
  t_idx l_iy = (i_y - m_domainStartY) / m_dxy;
  if (l_ix >= m_nx) l_ix = m_nx - 1;
  if (l_iy >= m_ny) l_iy = m_ny - 1;

  return m_h[l_iy * m_nx + l_ix];
}

tsunami_lab::t_real tsunami_lab::setups::CheckPoint::getMomentumX(t_real i_x, t_real i_y) const {
  if (i_x < m_domainStartX || i_x > m_domainStartX + m_domainSizeX ||
      i_y < m_domainStartY || i_y > m_domainStartY + m_domainSizeY) {
    return 0;
  }
  t_idx l_ix = (i_x - m_domainStartX) / m_dxy;
  t_idx l_iy = (i_y - m_domainStartY) / m_dxy;
  if (l_ix >= m_nx) l_ix = m_nx - 1;
  if (l_iy >= m_ny) l_iy = m_ny - 1;

  return m_hu[l_iy * m_nx + l_ix];
}

tsunami_lab::t_real tsunami_lab::setups::CheckPoint::getMomentumY(t_real i_x, t_real i_y) const {
  if (i_x < m_domainStartX || i_x > m_domainStartX + m_domainSizeX ||
      i_y < m_domainStartY || i_y > m_domainStartY + m_domainSizeY) {
    return 0;
  }
  t_idx l_ix = (i_x - m_domainStartX) / m_dxy;
  t_idx l_iy = (i_y - m_domainStartY) / m_dxy;
  if (l_ix >= m_nx) l_ix = m_nx - 1;
  if (l_iy >= m_ny) l_iy = m_ny - 1;

  return m_hv[l_iy * m_nx + l_ix];
}

tsunami_lab::t_real tsunami_lab::setups::CheckPoint::getBathymetry(t_real i_x, t_real i_y) const {
  if (i_x < m_domainStartX || i_x > m_domainStartX + m_domainSizeX ||
      i_y < m_domainStartY || i_y > m_domainStartY + m_domainSizeY) {
    return 0;
  }
  t_idx l_ix = (i_x - m_domainStartX) / m_dxy;
  t_idx l_iy = (i_y - m_domainStartY) / m_dxy;
  if (l_ix >= m_nx) l_ix = m_nx - 1;
  if (l_iy >= m_ny) l_iy = m_ny - 1;

  return m_b[l_iy * m_nx + l_ix];
}
