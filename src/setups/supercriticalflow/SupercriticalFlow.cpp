/**
 * @author Ward Tammaa
 *
 * @section DESCRIPTION
 *  SupercriticalFlow
 **/
#include "SupercriticalFlow.h"
#include <cmath>


tsunami_lab::t_real tsunami_lab::setups::SupercriticalFlow::getHeight( t_real i_x,
                                                                t_real      ) const {
  return -getBathymetry(i_x,0);
}

tsunami_lab::t_real tsunami_lab::setups::SupercriticalFlow::getMomentumX( t_real,
                                                                   t_real ) const {
  return 0.18;
}

tsunami_lab::t_real tsunami_lab::setups::SupercriticalFlow::getMomentumY( t_real,
                                                                   t_real ) const {
  return 0;
}
//as long as the x-value stays between 8 and 12 we return -0.13-0.05*((i_x-10)*(i_x-10)) or else it is -0.33
tsunami_lab::t_real tsunami_lab::setups::SupercriticalFlow::getBathymetry( t_real i_x,
                                                                           t_real ) const {
  if(i_x > 8 && i_x < 12){
    return -0.13-0.05*((i_x-10)*(i_x-10));
  }else{
    return -0.33;
  }

}