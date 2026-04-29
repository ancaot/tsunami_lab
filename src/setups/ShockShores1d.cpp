/**
 * @author 
 *
 * @section DESCRIPTION
 * One-dimensional shock-shock problem.
 **/
#include "ShockShores1d.h"

tsunami_lab::setups::ShockShores1d::ShockShores1d(  t_real i_height,
                                                    t_real i_velocityLeft,
                                                    t_real i_locationDiscontinuity,
                                                    t_real i_bathymetry) {
  m_height = i_height;
  m_velocityLeft = i_velocityLeft;
  m_locationDiscontinuity = i_locationDiscontinuity;
  m_bathymetry = i_bathymetry;
}

tsunami_lab::t_real tsunami_lab::setups::ShockShores1d::getHeight( t_real i_x,
                                                                  t_real      ) const {
  if  (i_x < m_locationDiscontinuity) {
    return m_height;
  } else {
    return m_height + (m_bathymetry / 2); // bathymetry difference is added because bathymetry is negative
  }
  
}

tsunami_lab::t_real tsunami_lab::setups::ShockShores1d::getMomentumX( t_real i_x,
                                                                      t_real      ) const {
  if( i_x < m_locationDiscontinuity ) {
    // Linker Zustand q_l = [ h, +h*u ]: Stroemung nach rechts.
    return m_height * m_velocityLeft;
  }
  else {
    // No current in the shore water
    return 0;
  }
}

tsunami_lab::t_real tsunami_lab::setups::ShockShores1d::getMomentumY( t_real,
                                                                   t_real ) const {
  return 0;
}

tsunami_lab::t_real tsunami_lab::setups::ShockShores1d::getBathymetry( t_real i_x,
                                                                      t_real      ) const {
  if( i_x < m_locationDiscontinuity ) {
    // deep water
    return m_bathymetry;
  }
  else {
    // shallow water
    return m_bathymetry / 2;
  }
}