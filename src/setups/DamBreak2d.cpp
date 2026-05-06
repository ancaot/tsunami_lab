/**
 * @section DESCRIPTION
 * Two-dimensional dam break problem.
 **/
#include "DamBreak2d.h"
#include <cmath>
#include <math.h>

tsunami_lab::setups::DamBreak2d::DamBreak2d( t_real i_heightLeft,
                                             t_real i_heightRight,
                                             t_real i_momentumXLeft,
                                             t_real i_momentumXRight,
                                             t_real i_momentumYLeft,
                                             t_real i_momentumYRight,
                                             t_real i_locationXDam,
                                             t_real i_locationYDam ) {
  m_heightLeft = i_heightLeft;
  m_heightRight = i_heightRight;
  m_momentumXLeft = i_momentumXLeft;
  m_momentumXRight = i_momentumXRight;
  m_momentumYLeft = i_momentumYLeft;
  m_momentumYRight = i_momentumYRight;
  m_locationXDam = i_locationXDam;
  m_locationYDam = i_locationYDam;
}

tsunami_lab::t_real tsunami_lab::setups::DamBreak2d::getHeight( t_real i_x,
                                                                t_real i_y ) const {
  t_real l_compare = std::sqrt((i_x * i_x) + (i_y * i_y));
  if( l_compare < 10 ) {
    return m_heightLeft;
  }
  else {
    return m_heightRight;
  }
}

tsunami_lab::t_real tsunami_lab::setups::DamBreak2d::getMomentumX( t_real i_x,
                                                                   t_real i_y) const {
  t_real l_compare = std::sqrt((i_x * i_x) + (i_y * i_y));
  if ( l_compare < 10 ) {
    return m_momentumXLeft;
  }
  else {
    return m_momentumXRight;
  }
}

tsunami_lab::t_real tsunami_lab::setups::DamBreak2d::getMomentumY( t_real i_x,
                                                                   t_real i_y) const {
  t_real l_compare = std::sqrt((i_x * i_x) + (i_y * i_y));
  if ( l_compare < 10 ) {
    return m_momentumYLeft;
  }
  else {
    return m_momentumYRight;
  }
}