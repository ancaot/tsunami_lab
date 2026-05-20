/**
 * @section DESCRIPTION
 * Setup for the two-dimensional Tsunami simulation.
 *
*/
#include "TsunamiEvent2d.h"
#ifdef USE_NETCDF
#include "../io/NetCdf.h"

//Constructor
tsunami_lab::setups::TsunamiEvent2d::TsunamiEvent2d(t_real i_nx,
                                                    t_real i_ny,
                                                    std::string i_fileBathymetry, 
                                                    std::string i_fileDisplacement){
    m_fileB = i_fileBathymetry;
    m_fileD = i_fileDisplacement;
    m_nx = i_nx;
    m_ny = i_ny;
}

//Getter
tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getHeight(t_real i_x, 
                                                                t_real i_y) const {
    /**
    * h = max(b_in,delta)   if b_in <0
    *     0                 else
    */

    tsunami_lab::io::NetCdf::read(m_fileB, m_nx, m_ny, i_x, i_y, m_bathymetry);

    if(m_bathymetryIn < 0){
        return std::max(-m_bathymetryIn, delta);
    }else{
        return 0;
    }
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getMomentumX(t_real i_x, t_real i_y) const {
    //hu = 0
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getMomentumY(t_real i_x, t_real i_y) const {
    //hv = 0
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getBathymetry(t_real i_x, 
                                                                    t_real i_y) const {
    /** 
    * b = min(b_in, -delta)+d   if b_in < 0
    *     max(b_in, delta)+d    else
    */

    tsunami_lab::io::NetCdf::read(m_fileB, m_nx, m_ny, i_x, i_y, m_bathymetry);
    tsunami_lab::io::NetCdf::read(m_fileD, m_nx, m_ny, i_x, i_y, m_displacement);

    if(m_bathymetryIn < 0){
        return std::min(m_bathymetryIn, -delta) + m_displacement;
    }else{
        return std::max(m_bathymetryIn, delta) + m_displacement;
    }
}

#endif