/**
 * @author 
 *
 * @section DESCRIPTION
 * One-dimensional hydraulic jump setup over a hump.
 **/
#include "HydraulicJump1d.h"

#include <cmath>

namespace {
  constexpr tsunami_lab::t_real g_gravity = 9.81f;
}

tsunami_lab::t_real tsunami_lab::setups::HydraulicJump1d::getBathymetry( t_real i_x ) const {
  if( i_x > m_humpCenter - m_humpHalfWidth && i_x < m_humpCenter + m_humpHalfWidth ) {
    t_real const l_offset = i_x - m_humpCenter;
    return m_bathymetryOutside - m_bathymetryCurvature * l_offset * l_offset;
  }
  return m_bathymetryOutside;
}

tsunami_lab::t_real tsunami_lab::setups::HydraulicJump1d::getHeight( t_real i_x,
                                                                      t_real ) const {
  return -getBathymetry( i_x );
}

tsunami_lab::t_real tsunami_lab::setups::HydraulicJump1d::getMomentumX( t_real,
                                                                         t_real ) const {
  return 0;
}

tsunami_lab::t_real tsunami_lab::setups::HydraulicJump1d::getMomentumY( t_real,
                                                                         t_real ) const {
  return 0;
}

tsunami_lab::setups::HydraulicJumpSubcritical1d::HydraulicJumpSubcritical1d() {
  m_bathymetryOutside = -2.0f;
  m_momentumX = 4.42f;
}

tsunami_lab::t_real tsunami_lab::setups::HydraulicJumpSubcritical1d::getHeight( t_real i_x,
                                                                                t_real i_y ) const {
  return HydraulicJump1d::getHeight( i_x, i_y );
}

tsunami_lab::t_real tsunami_lab::setups::HydraulicJumpSubcritical1d::getMomentumX( t_real i_x,
                                                                                   t_real i_y ) const {
  (void) i_x;
  (void) i_y;
  return m_momentumX;
}

tsunami_lab::t_real tsunami_lab::setups::HydraulicJumpSubcritical1d::getMomentumY( t_real i_x,
                                                                                   t_real i_y ) const {
  return HydraulicJump1d::getMomentumY( i_x, i_y );
}

tsunami_lab::setups::HydraulicJumpSupercritical1d::HydraulicJumpSupercritical1d() {
  m_bathymetryOutside = -0.33f;
  m_momentumX = 0.18f;
}

tsunami_lab::t_real tsunami_lab::setups::HydraulicJumpSupercritical1d::getHeight( t_real i_x,
                                                                                  t_real i_y ) const {
  return HydraulicJump1d::getHeight( i_x, i_y );
}

tsunami_lab::t_real tsunami_lab::setups::HydraulicJumpSupercritical1d::getMomentumX( t_real i_x,
                                                                                     t_real i_y ) const {
  (void) i_x;
  (void) i_y;
  return m_momentumX;
}

tsunami_lab::t_real tsunami_lab::setups::HydraulicJumpSupercritical1d::getMomentumY( t_real i_x,
                                                                                     t_real i_y ) const {
  return HydraulicJump1d::getMomentumY( i_x, i_y );
}