/**
 *
 * @section DESCRIPTION

 **/
#ifndef TSUNAMI_LAB_SETUPS_ARTIFICIALTSUNAMI2D_H
#define TSUNAMI_LAB_SETUPS_ARTIFICIALTSUNAMI2D_H

#include "../Setup.h"
#include <fstream>
#include <sstream>
#include <vector>


namespace tsunami_lab {
  namespace setups {
    class ArtificialTsunami2d;
  }
}

/**
 * ArtificialTsunami2d setup.
 **/
class tsunami_lab::setups::ArtificialTsunami2d: public Setup {

  private:
    /**
     * @param m_delta avoids running into numerical issues due to missing support for wetting and drying in our solver.
     */
    t_real m_delta = 20;
    /**
     * @param m_Bin The swimming pool has a constant bathymetry of -100m before the “earthquake” 
     */
    t_real m_Bin = -100;
    
    /**
    * @brief computes the artificial displacement in the swimming pool.
    * @param i_x position in the swimming pool.
    * @param i_y position in the swimming pool.
    */
    t_real displacement( t_real i_x ,t_real i_y) const;

    /**
    * @brief return -((i_y/500)*(i_y/500))+1;
    * @param i_x position in the swimming pool.
    */
    t_real gFunction(t_real i_y) const;

    /**
    * @brief return sin(((i_x/500)+1)* M_PI );
    * @param i_x position in the swimming pool.
    */
    t_real fFunction(t_real i_x) const;

  public:
  
    /**
     * @brief The constructor for ArtificialTsunami2d.
     * @param i_delta to avoid running into numerical issues (small value)
     **/

    ArtificialTsunami2d();

    /**
     * @brief is an arificial simulation so no inputs needed
     * @return height is hardcoded to 100;
     **/
    t_real getHeight( t_real ,
                      t_real ) const;

    /**
     * @brief Gets the momentum in x-direction.
     * @return Momentum in x-direction.
     **/
    t_real getMomentumX( t_real,
                         t_real) const;

    /**
     * @brief Gets the momentum in y-direction.
     * @return Momentum in y-direction.
     **/
    t_real getMomentumY( t_real,
                         t_real ) const;
    
    /**
    * @brief Computes the bathymetry explained in <a href="https://scalable.uni-jena.de/opt/tsunami/chapters/assignment_5.html#equation-eq-artificial">5.2.1</a>.
    * @param i_x position in the swimming pool.
    * @param i_x position in the swimming pool.
    * @return artificial computed Bathymetry.
    */
    t_real getBathymetry( t_real i_x,
                          t_real i_y) const ;
};

#endif