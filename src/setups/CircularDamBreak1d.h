/**
 * @section DESCRIPTION
 * Zirkulärer Dammbruch als symmetrisches Problem für 1D.
 * Innerer Bereich höher als äußerer.
 * Gut zum Vergleichen von 1D und 2D Solver an gemeinsamen Stationen.
 **/
#ifndef TSUNAMI_LAB_SETUPS_CIRCULAR_DAM_BREAK_1D_H
#define TSUNAMI_LAB_SETUPS_CIRCULAR_DAM_BREAK_1D_H

#include "Setup.h"

namespace tsunami_lab {
  namespace setups {
    class CircularDamBreak1d;
  }
}

class tsunami_lab::setups::CircularDamBreak1d: public Setup {
  private:
    t_real m_heightInner = 0;
    t_real m_heightOuter = 0;
    t_real m_radius = 0;
    t_real m_center = 0;

  public:
    CircularDamBreak1d( t_real i_heightInner,
                        t_real i_heightOuter,
                        t_real i_radius,
                        t_real i_center );

    t_real getHeight( t_real i_x,
                      t_real ) const;

    t_real getMomentumX( t_real,
                         t_real ) const;

    t_real getMomentumY( t_real,
                         t_real ) const;
};

#endif
