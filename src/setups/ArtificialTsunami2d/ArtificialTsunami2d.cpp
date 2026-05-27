/**
 
 *
 * @section DESCRIPTION
 **/
#include "ArtificialTsunami2d.h"
#include <cmath>
#include <cstddef> 


tsunami_lab::setups::ArtificialTsunami2d::ArtificialTsunami2d(){

    

}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getBathymetry( t_real i_x,
                                                                            t_real i_y) const {


    //m_Bin is hardcoded to -100m                                                                 
    
    if(m_Bin < 0 ){    
        if( m_Bin < -m_delta){
            return m_Bin + displacement(i_x,i_y);  
        }else{
            return -m_delta + displacement(i_x,i_y);
        }
    }else{
       if( m_Bin > m_delta){
            return m_Bin    + displacement(i_x,i_y);
        }else{
            return m_delta + displacement(i_x,i_y);
        }
    }
}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getHeight( t_real, 
                                                                    t_real )const{
    //-100 m bathymetry and water level is at 0 -> 100m height
    return 100;                                                                                                                                                                                   
}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::displacement( t_real i_x,t_real i_y) const{

    if((i_x > -500) && (i_y > -500) && (i_x < 500) && (i_y < 500) ){
        return 5 * fFunction(i_x) * gFunction(i_y);
    }else{
        return 0;
    }
}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::fFunction(t_real i_x) const{
    return sin(((i_x/500)+1)* M_PI );
}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::gFunction(t_real i_y) const{
    return -((i_y/500)*(i_y/500))+1;
}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getMomentumY(  t_real ,
                                                                        t_real )const{
    return 0;
}



tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getMomentumX(  t_real,
                                                                        t_real )const{
    return 0;
}