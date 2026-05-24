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
        // delta = 20m
        t_real delta = 20;

        // bathymetry data
        t_idx m_nx_b = 0;
        t_idx m_ny_b = 0;
        t_real* m_x_b = nullptr;
        t_real* m_y_b = nullptr;
        t_real* m_z_b = nullptr;

        // displacement data
        t_idx m_nx_d = 0;
        t_idx m_ny_d = 0;
        t_real* m_x_d = nullptr;
        t_real* m_y_d = nullptr;
        t_real* m_z_d = nullptr;

        /**
         * @brief Nearest neighbour interpolation
         * @param i_x x-coordinate
         * @param i_y y-coordinate
         * @param i_nx number of x cells in dataset
         * @param i_ny number of y cells in dataset
         * @param i_x_arr array of x coordinates in dataset
         * @param i_y_arr array of y coordinates in dataset
         * @param i_z_arr array of z values in dataset
         * @return interpolated z value at (i_x, i_y)
         */
        t_real getNearestNeighbour(t_real i_x, t_real i_y, t_idx i_nx, t_idx i_ny,
                                   t_real* i_x_arr, t_real* i_y_arr, t_real* i_z_arr) const;

    public:
        /**
        * Constructor
        *
        * @param i_nx number of x Cells.
        * @param i_ny number of y Cells.
        * @param i_fileBathymetry name of file with bathymetry data
        * @param i_fileDisplacement name of file with displacement data
        */
        TsunamiEvent2d( std::string i_fileBathymetry,
                        std::string i_fileDisplacement );

        /**
        * Destructor
        */
        ~TsunamiEvent2d();

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