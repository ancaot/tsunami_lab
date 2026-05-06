/**
 * @section DESCRIPTION
 * Symmetric 1d circular-dam-break surrogate.
 **/
#include "CircularDamBreak1d.h"

#include <cmath>

tsunami_lab::setups::CircularDamBreak1d::CircularDamBreak1d( t_real i_heightInner,
                                                              t_real i_heightOuter,
                                                              t_real i_radius,
                                                              t_real i_center ) {
  m_heightInner = i_heightInner;
  m_heightOuter = i_heightOuter;
  m_radius = i_radius;
  m_center = i_center;
}

tsunami_lab::t_real tsunami_lab::setups::CircularDamBreak1d::getHeight( t_real i_x,
                                                                         t_real ) const {
  if( std::abs( i_x - m_center ) <= m_radius ) return m_heightInner;
  return m_heightOuter;
}

tsunami_lab::t_real tsunami_lab::setups::CircularDamBreak1d::getMomentumX( t_real,
                                                                            t_real ) const {
  return 0;
}

tsunami_lab::t_real tsunami_lab::setups::CircularDamBreak1d::getMomentumY( t_real,
                                                                            t_real ) const {
  return 0;
}
