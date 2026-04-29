/**
* @section DESCRIPTION
* Setup for the Tsunami simulation.
*/

#include "TsunamiEvent1d.h"
#include <algorithm>
#include <vector> 
#include "../io/Csv.h"
#include <cmath>

using tsunami_lab::t_real;

tsunami_lab::setups::TsunamiEvent1d::TsunamiEvent1d( t_real i_height,
                                                    t_real i_momentum) {
    m_height = i_height;
    m_momentum = i_momentum;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getDisplacement(t_real i_x) const{
    //get displacement
    if (175000 < i_x && i_x < 250000) {
        t_real l_disTemp = (i_x - 175000);
        l_disTemp /= 37500;
        l_disTemp *= M_PI;
        l_disTemp += M_PI;
        return 10 * sin(l_disTemp);
    }else{
        return 0;
    }
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::extractBathymetry(t_real i_x) const{
    
    if(i_x < bathymetryData.size()){
        return bathymetryData.at(i_x);
    }else{
        return 0;
    }
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getBathymetry(t_real i_x,
                                                    t_real ) const{
    t_real l_extBat = extractBathymetry(i_x);
    t_real l_displ = getDisplacement(i_x);
    //get the bathymetry for given point
    if(l_extBat < 0){
        t_real l_min = std::min(l_extBat, -(m_delta)) + l_displ;
        return l_min;
    }else {
        t_real l_max = std::max(l_extBat, m_delta) + l_displ; 
        return l_max;
    }
}

t_real tsunami_lab::setups::TsunamiEvent1d::getHeight(t_real i_x, t_real ) const {
    // chosen based on extracted bathymetry
    t_real l_extBat = extractBathymetry(i_x);
    if (l_extBat < 0) {
        t_real l_max = std::max(-(l_extBat), m_delta);
        return l_max;
    }else {
        return 0;
    }
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getMomentumX( t_real,
                                                                   t_real ) const {
  return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getMomentumY( t_real,
                                                                   t_real ) const {
  return 0;
}