/**
 * @author 
 *
 * @section DESCRIPTION
 * One-dimensional shock-shock problem.
 **/
#include "ShockLand1d.h"

tsunami_lab::setups::ShockLand1d::ShockLand1d( t_real i_height,
                                                 t_real i_velocityLeft,
                                                 t_real i_locationDiscontinuity ) {
  m_height = i_height;
  m_velocityLeft = i_velocityLeft;
  m_locationDiscontinuity = i_locationDiscontinuity;
}

tsunami_lab::t_real tsunami_lab::setups::ShockLand1d::getHeight( t_real i_x,
                                                                  t_real      ) const {
  if  (i_x < m_locationDiscontinuity) {
    return m_height;
  } else {
    return 0;
  }
  
}

tsunami_lab::t_real tsunami_lab::setups::ShockLand1d::getMomentumX( t_real i_x,
                                                                      t_real      ) const {
  if( i_x < m_locationDiscontinuity ) {
    // Linker Zustand q_l = [ h, +h*u ]: Stroemung nach rechts.
    return m_height * m_velocityLeft;
  }
  else {
    //Without water there is no flow
    return 0;
  }
}

tsunami_lab::t_real tsunami_lab::setups::ShockLand1d::getMomentumY( t_real,
                                                                   t_real ) const {
  return 0;
}

tsunami_lab::t_real tsunami_lab::setups::ShockLand1d::getBathymetry( t_real i_x,
                                                                      t_real      ) const {
  if( i_x < m_locationDiscontinuity ) {
    // water part
    return m_bathymetry;
  }
  else {
    // Reflective Part
    return 0;
  }
}