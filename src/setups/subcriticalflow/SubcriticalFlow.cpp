/**
 * @author Ward Tammaa 
 *
 * @section DESCRIPTION
 * SubcriticalFlow.
 **/
#include "SubcriticalFlow.h"
#include <cmath>

tsunami_lab::t_real tsunami_lab::setups::SubcriticalFlow::getHeight( t_real i_x,
                                                                t_real      ) const {
  return -getBathymetry(i_x,0);
}

tsunami_lab::t_real tsunami_lab::setups::SubcriticalFlow::getMomentumX( t_real,
                                                                   t_real ) const {
  return 4.42;
}

tsunami_lab::t_real tsunami_lab::setups::SubcriticalFlow::getMomentumY( t_real,
                                                                   t_real ) const {
  return 0;
}

//as long as the x-value stays between 8 and 12 we return (-1.8-0.05*pow((i_x-10), 2)) or else it returns -2 for the bathymetry
tsunami_lab::t_real tsunami_lab::setups::SubcriticalFlow::getBathymetry( t_real i_x,
                                                                    t_real ) const {
  if(i_x > 8 && i_x < 12){
    return (-1.8-0.05*pow((i_x-10), 2));
  }else{
    return -2;
  }

}