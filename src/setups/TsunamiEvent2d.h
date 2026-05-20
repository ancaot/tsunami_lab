/**
 * @section DESCRIPTION
 * Setup for the two-dimensional Tsunami simulation.
 *
*/

#ifndef TSUNAMI_LAB_SETUPS_TSUNAMI_EVENT_2D_H
#define TSUNAMI_LAB_SETUPS_TSUNAMI_EVENT_2D_H

#include "Setup.h"
#include <string>
#include <vector> 
#include "../io/Csv.h"
#include <filesystem>

namespace tsunami_lab {
    namespace setups {
        class TsunamiEvent2d;
    }
}

//add runtime options: simulation time and resolution

class tsunami_lab::setups::TsunamiEvent2d: public Setup {
    private:
        // delts = 20m
        t_real delta = 20;

        // height
        t_real m_height = 0;

        // bathymetry
        t_real m_bathymetry = 0;

        // bathymethry input of file
        t_real m_bathymetryIn = 0;

        // displacement
        t_real m_displacement = 0;

        // displacement input of file
        t_real m_displacementIn = 0;

        // file for bathymetry input
        std::string m_fileB;

        // file for displacement input
        std::string m_fileD;

        // resolution of simulation
        t_real m_resolution;

    public:
        /**
        * Constructor
        */
        TsunamiEvent2d(std::string i_fileBathymetry,
                        std::string i_fileDisplacement,
                        t_real i_resolution);

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
};

#endif