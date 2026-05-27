/**
 * @author Ward Tammaa 
 *
 * @section DESCRIPTION
 * The Rare Rare problem.
 **/

#include "RareRare.h"


tsunami_lab::setups::RareRare::RareRare(t_real i_height,
                                        t_real i_hu,
                                        t_real i_locationDam){
  m_height      = i_height;
  m_hu          = i_hu;
  m_locationDam = i_locationDam;
}

tsunami_lab::t_real tsunami_lab::setups::RareRare::getHeight(t_real,
                                                             t_real)const{
    return m_height;                                                                                                                                                                                    
}


tsunami_lab::t_real tsunami_lab::setups::RareRare::getMomentumX(t_real i_x,
                                                                  t_real)const{
    if( i_x <= m_locationDam ) {
        return -m_hu;
    }
    else {
        return m_hu;
    }                                                                                                                         
                                                                  
}

tsunami_lab::t_real tsunami_lab::setups::RareRare::getBathymetry( t_real,
                                                                    t_real ) const {
  return 0;
}

tsunami_lab::t_real tsunami_lab::setups::RareRare::getMomentumY(t_real,
                                                                  t_real)const{
    return 0;                                                                                                                                                                      
}
