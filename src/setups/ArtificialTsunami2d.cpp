/**
 * @section DESCRIPTION
 * Setup for the two-dimensional artificial Tsunami simulation.
 *
*/
#include "ArtificialTsunami2d.h"
#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


//Constructor
tsunami_lab::setups::ArtificialTsunami2d::ArtificialTsunami2d(t_real i_height, t_real i_bathymetry){
    m_height = i_height;
    m_bathymetry = i_bathymetry;
}

//Getter
tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getHeight(t_real, t_real) const {
    return m_height;
}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getMomentumX(t_real , t_real ) const {
    return m_momentumX;
}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getMomentumY(t_real , t_real ) const {
    return m_momentumY;
}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getBathymetry(t_real i_x, t_real i_y) const {
    if (i_x >= -500 && i_x <= 500 && i_y >= -500 && i_y <= 500) {
        return m_bathymetry + getDisplacement(i_x, i_y);
    }
    return m_bathymetry;
}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getDisplacement(t_real i_x, t_real i_y) const {
    //d(x,y)= 5*f(x)*g(y)
    return 5*functionFx(i_x)*functionGy(i_y);
}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::functionFx(t_real i_x) const {
    //f(x) = sin((x/500 + 1)* M_PI)
    return sin(((i_x/500)+1)*M_PI);
}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::functionGy(t_real i_y) const {
    //g(y) = -(y/500)^2 + 1
    return -(pow((i_y/500), 2)) + 1;
}