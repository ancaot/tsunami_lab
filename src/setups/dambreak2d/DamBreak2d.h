/**
 * @author Ward Tammaa
 *
 * @section DESCRIPTION
 * circular dam break problem.
 **/
#ifndef TSUNAMI_LAB_SETUPS_DAM_BREAK_2D_H
#define TSUNAMI_LAB_SETUPS_DAM_BREAK_2D_H

#include "../Setup.h"

namespace tsunami_lab {
  namespace setups {
    class DamBreak2d;
  }
}

/**
 * circular dam break setup.
 **/
class tsunami_lab::setups::DamBreak2d: public Setup {

  public:

    /**
     * @brief Gets the water height at a given point.
     *
     * @param i_x x-coordinate of the queried point.
     * @param i_x y-coordinate of the queried point.
     * @return height at the given point.
     **/
    t_real getHeight( t_real i_x,
                      t_real i_y) const;

    /**
     * @brief Gets the momentum in x-direction.
     * @return momentum in x-direction.
     **/
    t_real getMomentumX( t_real,
                         t_real ) const;

    /**
     * @brief Gets the momentum in y-direction.
     * @return momentum in y-direction.
     **/
    t_real getMomentumY( t_real,
                         t_real ) const;

    /**
     * @brief Gets the bathymetry.
     * @return bathymetry.
     **/                
    t_real getBathymetry( t_real,
                          t_real ) const ;
};

#endif