/**
 * @author Ward Tammaa 
 *
 * @section DESCRIPTION
 * supercriticalflow.
 **/
#ifndef TSUNAMI_LAB_SETUPS_SUPERCRITICAL_FLOW_H
#define TSUNAMI_LAB_SETUPS_SUPERCRITICAL_FLOW_H

#include "../Setup.h"

namespace tsunami_lab {
  namespace setups {
    class SupercriticalFlow;
  }
}

/**
 * SupercriticalFlow setup.
 **/
class tsunami_lab::setups::SupercriticalFlow: public Setup {

  public:

    /**
     * @brief Gets the water height at a given point.
     * @param i_x x-coordinate of the queried point.
     * @return Height at the given point.
     **/
    t_real getHeight( t_real i_x,
                      t_real      ) const;

    /**
     * @brief Gets the momentum in x-direction.
     * @return Momentum in x-direction.
     **/
    t_real getMomentumX( t_real ,
                         t_real ) const;

    /**
     * @brief Gets the momentum in y-direction.
     * @return Momentum in y-direction.
     **/
    t_real getMomentumY( t_real,
                         t_real ) const;

    /**
     * @brief Gets the bathymetry at a given point.
     * @param i_x x-coordinate of the queried point.
     * @return Bathymetry.
     **/               
    t_real getBathymetry( t_real i_x,
                          t_real ) const ;
};

#endif