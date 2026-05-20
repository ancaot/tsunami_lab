/**
 * @section DESCRIPTION
 * Setup for the two-dimensional artificial Tsunami simulation.
 *
*/

#ifndef TSUNAMI_LAB_SETUPS_ARTIFICIAL_TSUNAMI_2D_H
#define TSUNAMI_LAB_SETUPS_ARTIFICIAL_TSUNAMI_2D_H

#include "Setup.h"
#include <string>
#include <vector> 

namespace tsunami_lab {
    namespace setups {
        class ArtificialTsunami2d;
    }
}

class tsunami_lab::setups::ArtificialTsunami2d: public Setup {
    private:
        //height
        t_real m_height = 10;

        // momentum in x-direction
        t_real m_momentumX = 0;

        // momentum in x-direction
        t_real m_momentumY = 0;

        // bathymetry
        t_real m_bathymetry = -100;


    public:
        /**
        * Constructor
        */
        ArtificialTsunami2d(t_real i_height,
                            t_real i_bathymetry);

                /**
        * Gets the water height at a given point.
        *
        * @param i_x x-coordinate of the queried point.
        * @param i_y y-coordinate of the queried point.
        * @return water height at the given point.
        **/
        t_real getHeight(t_real i_x,
                        t_real i_y) const;

        /**
        * Gets the momentum in x-direction.
        *
        * @param i_x x-coordinate of the queried point.
        * @param i_y y-coordinate of the queried point.
        * @return momentum in x-direction.
        **/
        t_real getMomentumX(t_real i_x,
                            t_real i_y) const;

        /**
        * Gets the momentum in y-direction.
        *
        * @param i_x x-coordinate of the queried point.
        * @param i_y y-coordinate of the queried point.
        * @return momentum in y-direction.
        **/
        t_real getMomentumY(t_real i_x,
                            t_real i_y) const;

        /**
        * Gets the bathymetry.
        *
        * @param i_x x-coordinate of the queried point.
        * @param i_y y-coordinate of the queried point.
        * @return bathymetry.
        **/
        t_real getBathymetry(t_real i_x,
                            t_real i_y) const;

        /**
        * Computes function d(x,y) given in task description
        *
        * @param i_x x-coordinate of the queried point.
        * @param i_y y-coordinate of the queried point.
        * @return d(x,y) = 5*f(x)*g(y)
        */
        t_real getDisplacement(t_real i_x,
                            t_real i_y) const;

        /**
        * Computes the function f(x) given in task description
        *
        * @param i_x x-coordinate of the queried point.
        * @return f(x) = sin((x/500 + 1) * PI)
        */
        t_real functionFx(t_real i_x) const;

        /**
        * Computes the function g(y) given in task description
        *
        * @param i_y y-coordinate of the queried point.
        * @return g(y) = -(y/500)^2 + 1
        */
        t_real functionGy(t_real i_y) const;
};


#endif