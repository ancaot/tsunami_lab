/**
 * @section DESCRIPTION
 * Two-dimensional dam break problem.
 **/

#ifndef TSUNAMI_LAB_SETUPS_DAM_BREAK_2D_H
#define TSUNAMI_LAB_SETUPS_DAM_BREAK_2D_H

#include "Setup.h"

namespace tsunami_lab {
  namespace setups {
    class DamBreak2d;
  }
}

class tsunami_lab::setups::DamBreak2d: public Setup{
    private:
        //variables
        //! height on the left side 
        t_real m_heightLeft = 0;
    
        //! height on the right side
        t_real m_heightRight = 0;

        // momentum on the left side in x-direction
        t_real m_momentumXLeft = 0;

        // momentum on the left side in x-direction
        t_real m_momentumXRight = 0;

        // momentum on the left side in y-direction
        t_real m_momentumYLeft = 0;

        // momentum on the left side in y-direction
        t_real m_momentumYRight = 0;

        //! location of the dam (x-coordinate)
        t_real m_locationXDam = 0;

        //! location of the dam (y-coordinate)
        t_real m_locationYDam = 0;

    public:
        //methods and constructor
        /**
      * Constructor.
      *
      * @param i_heightLeft water height on the left side of the dam.
      * @param i_heightRight water height on the right side of the dam.
      * @param i_momentumXLeft river momentum in x-direction on the left side of the dam.
      * @param i_momentumXRight river momentum in x-direction on the right side of the dam.
      * @param i_momentumYLeft river momentum in y-direction on the left side of the dam.
      * @param i_momentumYRight river momentum in y-direction on the right side of the dam.
      * @param i_locationXDam location (x-coordinate) of the dam.
      * @param i_locationYDam location (y-coordinate) of the dam.
      **/
      DamBreak2d( t_real i_heightLeft,
                t_real i_heightRight,
                t_real i_momentumXLeft,
                t_real i_momentumXRight,
                t_real i_momentumYLeft,
                t_real i_momentumYRight,
                t_real i_locationXDam,
                t_real i_locationYDam );

      /**
      * Gets the water height at a given point.
      *
      * @param i_x x-coordinate of the queried point.
      * @param i_y y-coordinate of the queried point.
      * @return height at the given point.
      **/
      t_real getHeight( t_real i_x,
                      t_real i_y ) const;

      /**
      * Gets the momentum in x-direction.
      *
      * @return momentum in x-direction.
      **/
      t_real getMomentumX( t_real,
                         t_real ) const;

      /**
      * Gets the momentum in y-direction.
      *
      * @return momentum in y-direction.
      **/
      t_real getMomentumY( t_real,
                         t_real ) const;
};

#endif