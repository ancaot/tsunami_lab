#include "TsunamiEvent2d.h"
#include "../../io/NetCdf/NetCdf.h"
#include <cmath>
#include <cstddef> 


tsunami_lab::setups::TsunamiEvent2d::TsunamiEvent2d(const char*  i_bathFile, const char*    i_disFile)
{
tsunami_lab::io::NetCdf::read(i_bathFile,"z",m_nx_bat,m_ny_bat,&m_bathymetry_values,&m_bathymetry_x_values,&m_bathymetry_y_values);
    for(t_idx i = 0; i < m_nx_bat*m_ny_bat; ++i){
        if(std::isnan(m_bathymetry_values[i])){
            m_bathymetry_values[i] = 0;
        }
    }
    tsunami_lab::io::NetCdf::read(i_disFile,"z",m_nx_dis,m_ny_dis,&m_displacement_values,&m_displacement_x_values,&m_displacement_y_values);
    for(t_idx i = 0; i < m_nx_dis*m_ny_dis; ++i){
        if(std::isnan(m_displacement_values[i])){
            m_displacement_values[i] = 0;
        }
    }
    
}

tsunami_lab::setups::TsunamiEvent2d::~TsunamiEvent2d(){

    delete[] m_bathymetry_values;
    delete[] m_bathymetry_x_values;
    delete[] m_bathymetry_y_values;
    delete[] m_displacement_values;
    delete[] m_displacement_x_values;
    delete[] m_displacement_y_values;
}


tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getBathymetry( t_real i_x,
                                                                        t_real i_y) const {

    
    t_real l_batNetCdf = getBathymetryNetCdf(i_x, i_y);

    if(l_batNetCdf < 0 )
    {
        if(l_batNetCdf < -m_delta)
        {
            return l_batNetCdf + displacement(i_x, i_y);
        }
        else
        {
            return  displacement(i_x, i_y) - m_delta;
        }
    }
    else
    {
        if(l_batNetCdf < m_delta)
        {
            return  displacement(i_x, i_y) + m_delta;
        }
        else
        {
            return l_batNetCdf + displacement(i_x, i_y);
        }
    }
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::displacement( t_real i_x,t_real i_y) const {

    if (i_x < m_displacement_x_values[0] || i_x > m_displacement_x_values[m_nx_dis - 1] ||
        i_y < m_displacement_y_values[0] || i_y > m_displacement_y_values[m_ny_dis - 1])
    {
        return 0;
    }
    //nachdem man schon eigentlich den index raushat wollen wir ihn nochmal berechnen????? 
    t_idx l_x = findClosestIndex(m_displacement_x_values,m_nx_dis, i_x);
    t_idx l_y = findClosestIndex(m_displacement_y_values,m_ny_dis, i_y);

    return m_displacement_values[l_y * m_nx_dis + l_x];
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getHeight( t_real i_x,
                                                                    t_real i_y)const{

    t_real l_bin = getBathymetryNetCdf(i_x, i_y);

    if (l_bin < 0) {
        return (-l_bin > m_delta) ? -l_bin : m_delta;
    }
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getBathymetryNetCdf(t_real i_x, t_real i_y) const {
    //check whether the position is within our domain
    if (i_x < m_bathymetry_x_values[0] || i_x > m_bathymetry_x_values[m_nx_bat - 1] ||
        i_y < m_bathymetry_y_values[0] || i_y > m_bathymetry_y_values[m_ny_bat - 1])
    {
        return 0;
    }

    t_idx l_x = findClosestIndex(m_bathymetry_x_values,m_nx_bat, i_x);
    t_idx l_y = findClosestIndex(m_bathymetry_y_values,m_ny_bat,i_y);
    
    return m_bathymetry_values[l_y * m_nx_bat + l_x];
}

tsunami_lab::t_idx tsunami_lab::setups::TsunamiEvent2d::findClosestIndex(const t_real* arr,size_t size, t_real value) const {

   // if(arr == nullptr) return -1;

    if (size == 0) {
        return 0; 
    }

    t_idx closestIndex = 0;
    for (t_idx index = 0; index < size; ++index) {
        if (arr[index] > value) {
            if (value - arr[index - 1] > arr[index] - value) {
                closestIndex = index;
            } else {
                closestIndex = index - 1;
            }
            break;
        }
    }

    return closestIndex;
}


tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getMomentumY(  t_real,
                                                                        t_real)const{
    return 0;                                                                                                                                                                      
}



tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getMomentumX(  t_real ,
                                                                        t_real)const{
    return 0;                                                                                                                       
}   