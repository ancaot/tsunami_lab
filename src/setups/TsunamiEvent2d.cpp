/**
 * @section DESCRIPTION
 * Setup for the two-dimensional Tsunami simulation.
 *
*/
#include "TsunamiEvent2d.h"
#include <cmath>
#include <iostream>
#ifdef USE_NETCDF
#include "../io/NetCdf.h"
#endif

#include <fstream>
#include <vector>

void readBinary(std::string i_filePath,
                tsunami_lab::t_idx &o_nx,
                tsunami_lab::t_idx &o_ny,
                tsunami_lab::t_real **o_x,
                tsunami_lab::t_real **o_y,
                tsunami_lab::t_real **o_z) {
    
    // Replace .nc with .bin
    std::string binPath = i_filePath;
    if(binPath.size() > 3 && binPath.substr(binPath.size()-3) == ".nc") {
        binPath.replace(binPath.size()-3, 3, ".bin");
    }

    std::ifstream file(binPath, std::ios::binary);
    if(!file.is_open()) {
        std::cerr << "Failed to open binary file: " << binPath << std::endl;
        return;
    }

    // Read nx, ny (uint64_t usually 8 bytes, reading as size_t)
    uint64_t nx, ny;
    file.read(reinterpret_cast<char*>(&nx), sizeof(uint64_t));
    file.read(reinterpret_cast<char*>(&ny), sizeof(uint64_t));
    
    o_nx = nx;
    o_ny = ny;

    *o_x = new tsunami_lab::t_real[nx];
    *o_y = new tsunami_lab::t_real[ny];
    *o_z = new tsunami_lab::t_real[nx * ny];

    // the python script wrote float64
    std::vector<double> x_dbl(nx), y_dbl(ny), z_dbl(nx*ny);
    file.read(reinterpret_cast<char*>(x_dbl.data()), nx * sizeof(double));
    file.read(reinterpret_cast<char*>(y_dbl.data()), ny * sizeof(double));
    file.read(reinterpret_cast<char*>(z_dbl.data()), nx * ny * sizeof(double));

    for(size_t i = 0; i < nx; i++) (*o_x)[i] = x_dbl[i];
    for(size_t i = 0; i < ny; i++) (*o_y)[i] = y_dbl[i];
    for(size_t i = 0; i < nx*ny; i++) (*o_z)[i] = z_dbl[i];
}

//Constructor
tsunami_lab::setups::TsunamiEvent2d::TsunamiEvent2d(std::string i_fileBathymetry, 
                                                    std::string i_fileDisplacement) {
#ifdef USE_NETCDF
    tsunami_lab::io::NetCdf::read(i_fileBathymetry, m_nx_b, m_ny_b, &m_x_b, &m_y_b, &m_z_b);
    tsunami_lab::io::NetCdf::read(i_fileDisplacement, m_nx_d, m_ny_d, &m_x_d, &m_y_d, &m_z_d);
#else
    readBinary(i_fileBathymetry, m_nx_b, m_ny_b, &m_x_b, &m_y_b, &m_z_b);
    readBinary(i_fileDisplacement, m_nx_d, m_ny_d, &m_x_d, &m_y_d, &m_z_d);
#endif
}

//Destructor
tsunami_lab::setups::TsunamiEvent2d::~TsunamiEvent2d() {
    if(m_x_b) delete[] m_x_b;
    if(m_y_b) delete[] m_y_b;
    if(m_z_b) delete[] m_z_b;

    if(m_x_d) delete[] m_x_d;
    if(m_y_d) delete[] m_y_d;
    if(m_z_d) delete[] m_z_d;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getNearestNeighbour(t_real i_x, t_real i_y, t_idx i_nx, t_idx i_ny,
                                                                             t_real* i_x_arr, t_real* i_y_arr, t_real* i_z_arr) const {
    if(i_nx == 0 || i_ny == 0 || i_x_arr == nullptr || i_y_arr == nullptr || i_z_arr == nullptr) return 0;
    
    // Find closest x index
    t_idx l_closest_x = 0;
    t_real l_min_dist_x = std::abs(i_x_arr[0] - i_x);
    for(t_idx i = 1; i < i_nx; i++){
        t_real l_dist = std::abs(i_x_arr[i] - i_x);
        if(l_dist < l_min_dist_x){
            l_min_dist_x = l_dist;
            l_closest_x = i;
        }else{
            // coordinates are sorted, so we can break early
            break; 
        }
    }

    // Find closest y index
    t_idx l_closest_y = 0;
    t_real l_min_dist_y = std::abs(i_y_arr[0] - i_y);
    for(t_idx i = 1; i < i_ny; i++){
        t_real l_dist = std::abs(i_y_arr[i] - i_y);
        if(l_dist < l_min_dist_y){
            l_min_dist_y = l_dist;
            l_closest_y = i;
        }else{
            break;
        }
    }

    // 2D Array is mapped to 1D Array
    return i_z_arr[l_closest_y * i_nx + l_closest_x];
}

//Getter
tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getHeight(t_real i_x, 
                                                                t_real i_y) const {
    /**
    * h = max(b_in,delta)   if b_in <0
    *     0                 else
    */

    t_real m_bathymetryIn = getNearestNeighbour(i_x, i_y, m_nx_b, m_ny_b, m_x_b, m_y_b, m_z_b);

    if(m_bathymetryIn < 0){
        return std::max(-m_bathymetryIn, delta);
    }else{
        return 0;
    }
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getMomentumX(t_real, t_real) const {
    //hu = 0
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getMomentumY(t_real, t_real) const {
    //hv = 0
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getBathymetry(t_real i_x, 
                                                                    t_real i_y) const {
    /** 
    * b = min(b_in, -delta)+d   if b_in < 0
    *     max(b_in, delta)+d    else
    */

    t_real m_bathymetryIn = getNearestNeighbour(i_x, i_y, m_nx_b, m_ny_b, m_x_b, m_y_b, m_z_b);
    t_real m_displacement = getNearestNeighbour(i_x, i_y, m_nx_d, m_ny_d, m_x_d, m_y_d, m_z_d);

    if(m_bathymetryIn < 0){
        return std::min(m_bathymetryIn, -delta) + m_displacement;
    }else{
        return std::max(m_bathymetryIn, delta) + m_displacement;
    }
}
