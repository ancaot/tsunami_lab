/**
 * @author Ward Tammaa, Mohamad khaled Minawe
 * @section DESCRIPTION
 * tow-dimensional wave propagation patch.
 **/
#ifndef TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_2D
#define TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_2D

#include "../WavePropagation.h"

namespace tsunami_lab {
  namespace patches {
    class WavePropagation2d;
  }
}

class tsunami_lab::patches::WavePropagation2d: public WavePropagation {
  private:
    //! current step which indicates the active values in the arrays below
    unsigned short m_step = 0;

    //! boolean to which solver will be used
    bool m_choice = true;
    //if true then we'll use reflecting boundary conditions at cell m_nCells+1 (last cell)
    bool m_choiceBoundry = false;

    //! number of cells discretizing the computational domain
    t_idx m_xCells = 0;
    t_idx m_yCells = 0;

    t_real * m_b = nullptr;
    t_real * m_h[2] = { nullptr, nullptr };
    t_real * m_hu[2] = { nullptr, nullptr };
    t_real * m_hv[2] = { nullptr, nullptr };

    /**
     * @param i_ix id of the cell in x-direction.
     * @param i_ix id of the cell in x-direction.
     * @return Gets the Index of the cell.
     */
    t_idx getIndex(tsunami_lab::t_idx  i_ix,tsunami_lab::t_idx  i_iy){
      return (m_xCells+2) * i_iy +i_ix;
    }
  public:
    /**
     * @brief Constructs the 1d wave propagation solver.
     * @param i_choice which solver to choice from (true means Roe and false means our Fwave).
     * @param i_nCells number of cells.
     **/
    WavePropagation2d( t_idx i_xCells, t_idx i_yCells, bool i_choice, bool i_choiceBoundary );

    /**
     * @brief Destructor which frees all allocated memory.
     **/
    ~WavePropagation2d();
    
    /**
     * @brief Performs a time step.
     *
     * @param i_scaling scaling of the time step (dt / dx).
     **/
    void timeStep( t_real i_scaling_x);
   
 

    /**
     * @brief Sets all ghost cells.
     */
    void setGhostCollumn();

    /**
     * @return amount of cells in one column/row.
     **/
    t_idx getStride(){
      return m_xCells+2;
    }
    /**
     * @brief leftest column contains ghostcells so we skip it
     */
    t_idx getGhostcellX(){
      return 1;
    } 
    /**
     * @brief bottom row contains ghostcells so we skip it
     */
    t_idx getGhostcellY(){
      return 1;
    }

    /**
     * @brief Gets the cells water heights.
     * @return Water heights.
     */
    t_real const * getHeight(){
      return m_h[m_step];
    }

    /**
     * @brief Gets cells' Bathymetry.
     * @return Bathymetry.
     */
   t_real const * getBathymetry(){
      return m_b;
    }

    /**
     * @brief Gets the cell's momenta in x-direction.
     * @return momenta in x-direction.
     **/
    t_real const * getMomentumX(){
      return m_hu[m_step];
    }

     /**
     * @brief Gets the cell's momenta in y-direction.
     * @return momenta in y-direction.
     **/
    t_real const * getMomentumY(){
      return m_hv[m_step];
    }

    /**
     * @brief Sets the height of the cell to the given value.
     * @param i_ix id of the cell in x-direction.
     * @param i_ix id of the cell in x-direction.
     * @param i_h water height.
     **/
    void setHeight( t_idx  i_ix,
                    t_idx  i_iy,
                    t_real i_h ) {
      m_h[m_step][getIndex(i_ix+1,i_iy+1)] = i_h;
    }


    /**
     * @brief Sets the Bathymetry of the cell to the given value.
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in y-direction.
     * @param i_b Bathymetry.
     **/
    void setBathymetry(t_idx  i_ix,
                       t_idx  i_iy,
                       t_real i_b){
      m_b[getIndex(i_ix+1,i_iy+1)] = i_b;
    }

    /**
     * @brief Sets the momentum in x-direction to the given value.
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in y-direction.
     * @param i_hu momentum in x-direction.
     **/
    void setMomentumX( t_idx  i_ix,
                       t_idx  i_iy,
                       t_real i_hu ) {
      m_hu[m_step][getIndex(i_ix+1,i_iy+1)] = i_hu;
    }

   /**
     * @brief Sets the momentum in y-direction to the given value.
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in y-direction.
     * @param i_hu momentum in y-direction.
     **/
    void setMomentumY( t_idx  i_ix,
                       t_idx  i_iy,
                       t_real i_hv) {
      m_hv[m_step][getIndex(i_ix+1,i_iy+1)] = i_hv;
    }
    

    

};

#endif
