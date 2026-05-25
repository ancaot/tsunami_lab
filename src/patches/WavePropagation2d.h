/**
 * @section DESCRIPTION
 * Two-dimensional wave propagation patch.
 * Simple 2D wave propagation patch implemented as independent 1D
 * sweeps in x-direction for each row. This is intended as a minimal
 * 2D surrogate for comparison along a central transect with the
 * existing 1D solver.
 **/
#ifndef TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_2D
#define TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_2D

#include "../constants.h"
#include "WavePropagation.h"

namespace tsunami_lab {
  namespace patches {
    class WavePropagation2d;
  }
}

class tsunami_lab::patches::WavePropagation2d: public WavePropagation{
    private:
        //! current step which indicates the active values in the arrays below
        unsigned short m_step = 0;

        //! number of cells discretizing the computational domain
        t_idx m_nx = 0;
        t_idx m_ny = 0;

        //! water heights for the current and next time step for all cells
        t_real * m_h[2] = { nullptr, nullptr };

        //! x momenta for the current and next time step for all cells
        t_real * m_hu[2] = { nullptr, nullptr };

        //! y momenta for the current and next time step for all cells
        t_real * m_hv[2] = { nullptr, nullptr };

        //! bathymetric data for all cells
        t_real * m_b; 
        
        // compact buffers without ghost cells (for CSV output)
        t_real * m_compactH = nullptr;
        t_real * m_compactHu = nullptr;
        t_real * m_compactHv = nullptr;
        t_real * m_compactB = nullptr;
    
    public:
        /**
        * CONSTRUCTOR
        *
        * @param i_nx number of cells in x-direction
        * @param i_nx number of cells in y-direction
        */
        WavePropagation2d( t_idx i_nx, t_idx i_ny);

        /**
        * DECONSTRUCTOR
        *
        */
        ~WavePropagation2d();

        /**
         * Performs a time step.
         *
         * @param i_scaling scaling of the time step (dt / dxy).
         **/
        void timeStep( t_real i_scaling );

        /**
         * Sets the values of the ghost cells according to outflow boundary conditions.
         **/
        void setGhostOutflow();

        /**
         * Gets the stride in y-direction. x-direction is stride-1.
         *
         * @return stride in y-direction.
         **/
        t_idx getStride();

        /**
         * Gets cells' water heights.
         *
         * @return water heights.
         */
        t_real const * getHeight();

        /**
         * Gets the cells' momenta in x-direction.
         *
         * @return momenta in x-direction.
         **/
        t_real const * getMomentumX();

        /**
         * Gets the cells' momenta in y-direction.
         *
         * @return momenta in y-direction.
         **/
        t_real const * getMomentumY();

        /**
         * Gets the cells' bathymetry.
         *
         * @return bathymetry.
         **/
        t_real const * getBathymetry();

        /**
         * Sets the height of the cell to the given value.
         *
         * @param i_ix id of the cell in x-direction.
         * @param i_iy id of the cell in y-direction.
         * @param i_h water height.
         **/
        void setHeight( t_idx  i_ix,
                    t_idx  i_iy,
                    t_real i_h );

        /**
         * Sets the momentum in x-direction to the given value.
         *
         * @param i_ix id of the cell in x-direction.
         * @param i_iy id of the cell in y-direction.
         * @param i_hu momentum in x-direction.
         **/
        void setMomentumX( t_idx  i_ix,
                       t_idx  i_iy,
                       t_real i_hu );

        /**
         * Sets the momentum in y-direction to the given value.
         *
         * @param i_ix id of the cell in x-direction.
         * @param i_iy id of the cell in y-direction.
         * @param i_hv momentum in y-direction.
         **/
        void setMomentumY( t_idx  i_ix,
                       t_idx  i_iy,
                       t_real i_hv );

        /**
         * Sets the bathymetry to the given value.
         * @param i_ix id of the cell in x-direction.
         * @param i_iy id of the cell in y-direction.
         * @param i_b  bathymetry.
         **/
        void setBathymetry( t_idx i_ix,
                            t_idx i_iy,
                            t_real i_b);
};

#endif