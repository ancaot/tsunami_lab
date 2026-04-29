/**
 * @section DESCRIPTION
 * Setup for the Tsunami simulation.
 *
*/

#ifndef TSUNAMI_LAB_SETUPS_TSUNAMI_EVENT_1D_H
#define TSUNAMI_LAB_SETUPS_TSUNAMI_EVENT_1D_H

#include "Setup.h"
#include <string>
#include <vector> 
#include "../io/Csv.h"
#include <filesystem>

namespace tsunami_lab {
    namespace setups {
        class TsunamiEvent1d;
    }
}

/*
* 1d Tsunami Event Setup
*/

class tsunami_lab::setups::TsunamiEvent1d: public Setup {
    private:
        //height
        t_real m_height = 0;
        //momentum
        t_real m_momentum = 0;

        //file with bathymetry data
        std::string m_file = "data/01_dem_02.csv";

        t_real m_delta = 20;

        std::vector<t_real> bathymetryData = tsunami_lab::io::Csv::read(m_file);

    public:
        /**
        * Constructor
        *
        * @param i_height height of cell
        * @param i_momentrum momentum of cell
        */
        TsunamiEvent1d(t_real i_height,
                        t_real i_momentum);

        /**
        * Extracts the bathymetry from file for a given point.
        *
        * @param i_x x-coordinate of the queried point.
        * @return extracted bathymetry for the given point.
        **/
        t_real extractBathymetry(t_real i_x) const;

        /**
        * Get the bathymetry for a given point.
        *
        * @param i_x x-coordinate of the queried point.
        * @param i_y y-coordinate of the queried point.
        * @return bathymetry at the given point.
        **/                        
        t_real getBathymetry(t_real i_x,
                            t_real ) const;

        /**
        * Gets the vertical displacement at a given point.
        *
        * @param i_x x-coordinate of the queried point.
        * @return vertical displacement at the given point.
        **/
        t_real getDisplacement(t_real i_x) const;

        /**
        * Gets the water height at a given point.
        *
        * @param i_x x-coordinate of the queried point.
        * @param i_y y-coordinate of the queried point.
        * @return water height at the given point.
        **/
        t_real getHeight(t_real i_x,
                        t_real ) const;

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

};

#endif