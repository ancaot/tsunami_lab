/**
 * @author 
 *
 * @section DESCRIPTION
 * One-dimensional hydraulic jump setup over a hump.
 **/
#ifndef TSUNAMI_LAB_SETUPS_HYDRAULIC_JUMP_1D_H
#define TSUNAMI_LAB_SETUPS_HYDRAULIC_JUMP_1D_H

#include "Setup.h"

namespace tsunami_lab {
  namespace setups {
    /**
     * Base class for the hydraulic jump setups.
     **/
    class HydraulicJump1d: public Setup {
      protected:
        //! location of the hump center
        t_real m_humpCenter = 10;

        //! half-width of the hump support interval
        t_real m_humpHalfWidth = 2;

        //! bathymetry in the flat part outside the hump
        t_real m_bathymetryOutside = 0;

        //! coefficient of the quadratic hump profile
        t_real m_bathymetryCurvature = 0.05;

        /**
         * Gets the bathymetry at a given x-coordinate.
         *
         * @param i_x x-coordinate of the queried point.
         * @return bathymetry at the given point.
         **/
        t_real getBathymetry( t_real i_x ) const;

      public:
        /**
         * Gets the water height at a given point.
         *
         * @param i_x x-coordinate of the queried point.
         * @param i_y y-coordinate of the queried point.
         * @return water height at the given point.
         **/
        t_real getHeight( t_real i_x,
                          t_real i_y ) const;

        /**
         * Gets the momentum in x-direction.
         *
         * @param i_x x-coordinate of the queried point.
         * @param i_y y-coordinate of the queried point.
         * @return momentum in x-direction.
         **/
        t_real getMomentumX( t_real i_x,
                             t_real i_y ) const;

        /**
         * Gets the momentum in y-direction.
         *
         * @param i_x x-coordinate of the queried point.
         * @param i_y y-coordinate of the queried point.
         * @return momentum in y-direction.
         **/
        t_real getMomentumY( t_real i_x,
                             t_real i_y ) const;
    };

    /**
     * Subcritical hydraulic jump setup.
     **/
    class HydraulicJumpSubcritical1d: public HydraulicJump1d {
      private:
        //! discharge hu in the whole domain
        t_real m_momentumX = 0;

      public:
        /**
         * Constructor.
         **/
        HydraulicJumpSubcritical1d();

        /**
         * Gets the water height at a given point.
         **/
        t_real getHeight( t_real i_x,
                          t_real i_y ) const;

        /**
         * Gets the momentum in x-direction.
         **/
        t_real getMomentumX( t_real i_x,
                             t_real i_y ) const;

        /**
         * Gets the momentum in y-direction.
         **/
        t_real getMomentumY( t_real i_x,
                             t_real i_y ) const;
    };

    /**
     * Supercritical hydraulic jump setup.
     **/
    class HydraulicJumpSupercritical1d: public HydraulicJump1d {
      private:
        //! discharge hu in the whole domain
        t_real m_momentumX = 0;

      public:
        /**
         * Constructor.
         **/
        HydraulicJumpSupercritical1d();

        /**
         * Gets the water height at a given point.
         **/
        t_real getHeight( t_real i_x,
                          t_real i_y ) const;

        /**
         * Gets the momentum in x-direction.
         **/
        t_real getMomentumX( t_real i_x,
                             t_real i_y ) const;

        /**
         * Gets the momentum in y-direction.
         **/
        t_real getMomentumY( t_real i_x,
                             t_real i_y ) const;
    };
  }
}

#endif