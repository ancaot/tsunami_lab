/**
 * @section DESCRIPTION
 * CUDA-backed two-dimensional F-Wave propagation patch.
 *
 * This class implements the existing WavePropagation interface so that the
 * main application can select a CUDA backend without changing the surrounding
 * setup, output, checkpoint and station code. The numerical CUDA kernels are
 * intentionally limited to the F-Wave solver.
 **/
#ifndef TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_2D_CUDA
#define TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_2D_CUDA

#include "../WavePropagation.h"

#include <cstddef>

namespace tsunami_lab {
  namespace patches {
    class WavePropagation2dCuda;
  }
}

class tsunami_lab::patches::WavePropagation2dCuda: public WavePropagation {
  private:
    t_idx m_xCells = 0;
    t_idx m_yCells = 0;
    t_idx m_cells = 0;
    t_idx m_maxEdges = 0;
    unsigned short m_step = 0;
    bool m_reflecting = false;
    bool m_hostDirty = true;
    bool m_deviceDirty = false;

    t_real * m_hostB = nullptr;
    t_real * m_hostH[2] = { nullptr, nullptr };
    t_real * m_hostHu[2] = { nullptr, nullptr };
    t_real * m_hostHv[2] = { nullptr, nullptr };

    t_real * m_deviceB = nullptr;
    t_real * m_deviceH[2] = { nullptr, nullptr };
    t_real * m_deviceHu[2] = { nullptr, nullptr };
    t_real * m_deviceHv[2] = { nullptr, nullptr };
    t_real * m_edges[4] = { nullptr, nullptr, nullptr, nullptr };

    t_idx getIndex(t_idx i_ix, t_idx i_iy) const {
      return (m_xCells + 2) * i_iy + i_ix;
    }

    void uploadIfNeeded();
    void downloadIfNeeded();
    void setGhostCellsOnHost();
    void setGhostCellsOnDevice();

  public:
    WavePropagation2dCuda(t_idx i_xCells,
                          t_idx i_yCells,
                          bool  i_useRoe,
                          bool  i_reflectingBoundary);

    ~WavePropagation2dCuda() override;

    void timeStep(t_real i_scaling) override;

    void setGhostCollumn() override;

    t_idx getStride() override {
      return m_xCells + 2;
    }

    t_idx getGhostcellX() override {
      return 1;
    }

    t_idx getGhostcellY() override {
      return 1;
    }

    t_real const * getHeight() override;
    t_real const * getBathymetry() override;
    t_real const * getMomentumX() override;
    t_real const * getMomentumY() override;

    void setHeight(t_idx i_ix, t_idx i_iy, t_real i_h) override;
    void setBathymetry(t_idx i_ix, t_idx i_iy, t_real i_b) override;
    void setMomentumX(t_idx i_ix, t_idx i_iy, t_real i_hu) override;
    void setMomentumY(t_idx i_ix, t_idx i_iy, t_real i_hv) override;
};

#endif
