#ifndef TSUNAMI_LAB_SETUPS_CHECKPOINT_H
#define TSUNAMI_LAB_SETUPS_CHECKPOINT_H

#include "../Setup.h"

namespace tsunami_lab {
  namespace setups {
    class CheckPoint;
  }
}

class tsunami_lab::setups::CheckPoint: public Setup {
  private:
    t_idx m_nx, m_ny;
    t_real m_domainStartX, m_domainStartY;
    t_real m_domainSizeX, m_domainSizeY;
    t_real *m_h;
    t_real *m_hu;
    t_real *m_hv;
    t_real *m_b;
    t_real m_dxy;

  public:
    CheckPoint(t_real i_domainSizeX,
               t_real i_domainSizeY,
               t_idx i_nx,
               t_idx i_ny,
               t_real i_domainStartX,
               t_real i_domainStartY,
               t_real* i_h,
               t_real* i_hu,
               t_real* i_hv,
               t_real* i_b);

    ~CheckPoint();

    t_real getHeight(t_real i_x, t_real i_y) const override;
    t_real getMomentumX(t_real i_x, t_real i_y) const override;
    t_real getMomentumY(t_real i_x, t_real i_y) const override;
    t_real getBathymetry(t_real i_x, t_real i_y) const override;
};

#endif
