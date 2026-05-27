/**
 * @author Ward Tammaa 
 *
 * @section DESCRIPTION
 *
 **/
#ifndef TSUNAMI_LAB_SETUPS_TSUNAMIEVENT1D_H
#define TSUNAMI_LAB_SETUPS_TSUNAMIEVENT1D_H

#include "../Setup.h"
#include <fstream>
#include <sstream>
#include <vector>


namespace tsunami_lab {
  namespace setups {
    class TsunamiEvent1d;
  }
}

/**
 * TsunamiEvent1d setup.
 **/
class tsunami_lab::setups::TsunamiEvent1d: public Setup {

  private:
    /**
     * @param m_delta avoids running into numerical issues due to missing support for wetting and drying in our solver.
     */
    t_real m_delta = 20; 

    /**
    * @param m_bathymetry_values contains all bathymetry values 
    */
    std::vector<t_real> m_bathymetry_values;
    
    /**
    * @brief the method adds the vertical displacement, typically caused by a subduction-zone earthquake.
    * @param i_x is the distance from the Fukushima Daini Nuclear Power Plant.
    */
    t_real displacement( t_real i_x) const;

  public:
  
    /**
     * @brief The constructor for TsunamiEvent1D.
     * @param i_delta to avoid running into numerical issues (small value)
     **/

    TsunamiEvent1d();

    /**
     * @brief Gets the water height at a given point.
     *
     * @param i_x is the distance from the Fukushima Daini Nuclear Power Plant
     * @return Height at the given point.
     **/
    t_real getHeight( t_real i_x,
                      t_real      ) const;

    /**
     * @brief Gets the momentum in x-direction.
     *
     * @return Momentum in x-direction.
     **/
    t_real getMomentumX( t_real ,
                         t_real ) const;

    /**
     * @brief Gets the momentum in y-direction.
     * @return Momentum in y-direction.
     **/
    t_real getMomentumY( t_real,
                         t_real ) const;
    
    /**
    * @brief Computes the bathymetry as explained in <a href="https://scalable.uni-jena.de/opt/tsunami/chapters/assignment_3.html#equation-eq-tsunami-event-1d">3.4.1</a>.
    * @param i_x is the distance from the Fukushima Daini Nuclear Power Plant.
    * @return Bathymetry.
    */
    t_real getBathymetry( t_real i_x,
                          t_real ) const ;

    /** 
    * @brief divide the distance i_x by 250 to determine the index for the bathymetry in the csv file.
    * @param i_x is the distance from the Fukushima Daini Nuclear Power Plant.
    * @return Bathymetry in the csv file (not the value that we use).
    */
    t_real getBathymetryCsv(t_real i_x) const;
};

#endif