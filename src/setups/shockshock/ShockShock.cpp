/**
 * @author Ward Tammaa 
 *
 * @section DESCRIPTION
 * The Shock Shock problem.
 **/
#include "ShockShock.h"


tsunami_lab::setups::ShockShock::ShockShock(t_real i_height,
                                            t_real i_hu,
                                            t_real i_locationDam){
  m_height = i_height;
  m_hu = i_hu;
  m_locationDam = i_locationDam;
}

tsunami_lab::t_real tsunami_lab::setups::ShockShock::getHeight( t_real,
                                                                t_real)const{
    return m_height;                                                                                                                      
                                                                  
}


tsunami_lab::t_real tsunami_lab::setups::ShockShock::getMomentumX(t_real i_x,
                                                                  t_real)const{
    if( i_x <= m_locationDam ) {
        return m_hu;
    }
    else {
        return -m_hu;
    }                                                                                                                         
                                                                  
}

tsunami_lab::t_real tsunami_lab::setups::ShockShock::getMomentumY(t_real,
                                                                  t_real)const{
    return 0;                                                                                                                                                                      
}

tsunami_lab::t_real tsunami_lab::setups::ShockShock::getBathymetry( t_real,
                                                                    t_real ) const {
  return 0;
}