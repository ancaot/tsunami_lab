/**
 * Simple 2D wave propagation patch implemented as independent 1D
 * sweeps in x-direction for each row. This is intended as a minimal
 * 2D surrogate for comparison along a central transect with the
 * existing 1D solver.
 **/
#ifndef TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_2D
#define TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_2D

#include "WavePropagation.h"

namespace tsunami_lab {
  namespace patches {
    class WavePropagation2d;
  }
}

class tsunami_lab::patches::WavePropagation2d: public WavePropagation {
  private:
    unsigned short m_step = 0;
    t_idx m_nx = 0;
    t_idx m_ny = 0;

    // storage includes two time layers; data layout per layer:
    // row-major with a single ghost cell at left and right for each row
    // internal stride = m_nx + 2
    t_real * m_h[2] = { nullptr, nullptr };
    t_real * m_hu[2] = { nullptr, nullptr };
    t_real * m_hv[2] = { nullptr, nullptr };
    t_real * m_b = nullptr;

    // compact buffers without ghost cells (for CSV output)
    t_real * m_compactH = nullptr;
    t_real * m_compactHu = nullptr;
    t_real * m_compactHv = nullptr;

  public:
    WavePropagation2d( t_idx i_nx, t_idx i_ny );
    ~WavePropagation2d();

    void timeStep( t_real i_scaling );
    void setGhostOutflow();

    // returns stride for compact arrays (number of columns)
    t_idx getStride();

    t_real const * getHeight();
    t_real const * getMomentumX();
    t_real const * getMomentumY();

    void setHeight( t_idx  i_ix,
                    t_idx  i_iy,
                    t_real i_h );

    void setMomentumX( t_idx  i_ix,
                       t_idx  i_iy,
                       t_real i_hu );

    void setMomentumY( t_idx  i_ix,
                       t_idx  i_iy,
                       t_real i_hv );
};

#endif
