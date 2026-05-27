/**
 * @author Ward Tammaa 
 *
 * @section DESCRIPTION
 * The Shock Shock problem.
 **/
#include "TsunamiEvent1d.h"
#include "../../io/Csv/Csv.h"
#include <cmath>
#include <cstddef> 


tsunami_lab::setups::TsunamiEvent1d::TsunamiEvent1d(){

    const std::string filename = "data/data_end.csv";
    std::size_t columnIndex = 3;
    m_bathymetry_values = tsunami_lab::io::Csv::read(filename,columnIndex); 
    
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getBathymetry( t_real i_x,
                                                                        t_real) const {
    t_real l_bin = getBathymetryCsv(i_x);
    if(l_bin < 0 ){
        if( l_bin < -m_delta){
            return l_bin    + displacement(i_x);
        }else{
            return -m_delta + displacement(i_x);
        }
    }else{
       if( l_bin > m_delta){
            return l_bin    + displacement(i_x);
        }else{
            return m_delta + displacement(i_x);
        }
    } 
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getHeight( t_real i_x,
                                                                    t_real      )const{
    t_real l_bin = getBathymetryCsv(i_x);                           
    if(l_bin < 0 ){
        if( -l_bin < m_delta){
            return m_delta;
        }else{
            return -l_bin;
        }
    }else{
        return 0;
    }                                                                                                                                                                                     
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::displacement( t_real i_x) const{

    if(i_x > 175000 && i_x < 250000){
        return 10* sin(((i_x-175000)/(37500))* M_PI + M_PI);
    }else{
        return 0;
    }
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getBathymetryCsv(t_real i_x) const{
    //i_x gets divided by 250 because every cell is in 250m steps
    std::size_t l_index = i_x /250; 
    return m_bathymetry_values[l_index];
}


tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getMomentumY(  t_real,
                                                                        t_real)const{
    return 0;                                                                                                                                                                      
}



tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getMomentumX(  t_real ,
                                                                        t_real)const{
    return 0;                                                                                                                       
}