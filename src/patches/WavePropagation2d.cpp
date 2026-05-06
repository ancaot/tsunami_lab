/**
 * @section DESCRIPTION
 * Two-dimensional wave propagation patch.
 **/

 #include "WavePropagation2d.h"
 #include "../solvers/FWave.h"

 tsunami_lab::patches::WavePropagation2d::WavePropagation2d(t_idx i_nCells){
    m_nCells = i_nCells;

    // allocate memory including a single ghost cell on each side
    for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
        m_h[l_st] = new t_real[  m_nCells + 2 ];
        m_hu[l_st] = new t_real[ m_nCells + 2 ];
        m_hv[l_st] = new t_real[ m_nCells + 2 ];
    }
    m_b = new t_real[  m_nCells + 2 ];

    // init to zero
    for( t_idx l_ce = 0; l_ce < m_nCells; l_ce++ ) {
        for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
          m_h[l_st][l_ce] = 0;
          m_hu[l_st][l_ce] = 0;
          m_hv[l_st][l_ce] = 0;
        }
        m_b[l_ce] = 0;
    }
}

tsunami_lab::patches::WavePropagation2d::~WavePropagation2d() {
    for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
        delete[] m_h[l_st];
        delete[] m_hu[l_st];
        delete [] m_hv[l_st];
    }
    delete[] m_b;
}

void tsunami_lab::patches::WavePropagation2d::timeStep(t_real i_scaling){
    // pointers to old and new data
    t_real * l_hOld = m_h[m_step];
    t_real * l_huOld = m_hu[m_step];
    t_real * l_hvOld = m_hv[m_step];

    m_step = (m_step+1) % 2;
    t_real * l_hNew =  m_h[m_step];
    t_real * l_huNew = m_hu[m_step];
    t_real * l_hvNew = m_hv[m_step];

    // init new cell quantities
    for( t_idx l_ce = 1; l_ce < m_nCells+1; l_ce++ ) {
        l_hNew[l_ce] = l_hOld[l_ce];
        l_huNew[l_ce] = l_huOld[l_ce];
        l_hvNew[l_ce] = l_hvOld[l_ce];
    }

    // iterate over edges and update with Riemann solutions
    for( t_idx l_ed = 0; l_ed < m_nCells+1; l_ed++ ) {
        // determine left and right cell-id
        t_idx l_ceL = l_ed;
        t_idx l_ceR = l_ed+1;

        // compute net-updates f-wave
        //provides hu and hu²+1/2gh²
        t_real l_netUpdatesA[2][2];
        solvers::FWave::netUpdates( l_hOld[l_ceL],
                              l_hOld[l_ceR],
                              l_huOld[l_ceL],
                              l_huOld[l_ceR],
                              m_b[l_ceL],
                              m_b[l_ceR],
                              l_netUpdatesA[0],
                              l_netUpdatesA[1] );

        //provides hv and hv²+1/2gh²
        t_real l_netUpdatesB[2][2];
        solvers::FWave::netUpdates( l_hOld[l_ceL],
                              l_hOld[l_ceR],
                              l_hvOld[l_ceL],
                              l_hvOld[l_ceR],
                              m_b[l_ceL],
                              m_b[l_ceR],
                              l_netUpdatesB[0],
                              l_netUpdatesB[1] );

        // compute huv
        t_real l_huvL = (l_huOld[l_ceL] * l_hvOld[l_ceL])/l_hOld[l_ceL];
        t_real l_huvR = (l_huOld[l_ceR] * l_hvOld[l_ceR])/l_hOld[l_ceR];

        // update the cells' quantities
        /* | h  |   |     hu        |   |      hv       |
        *  | hu | + | hu² + 1/2gh²  | + |     huv       | for A+dQ, A-dQ, B+dQ, B-dQ
        *  | hv |   |     huv       |   | hv² + 1/2gh²  |
        */

        l_hNew[l_ceL]  -= i_scaling * l_netUpdatesA[0][0] - i_scaling * l_netUpdatesB[0][0];
        l_huNew[l_ceL] -= i_scaling * l_netUpdatesA[0][1] - i_scaling * l_huvL;
        l_hvNew[l_ceL] -= i_scaling * l_huvL - i_scaling * l_netUpdatesB[0][1];

        l_hNew[l_ceR]  -= i_scaling * l_netUpdatesA[1][0] - i_scaling * l_netUpdatesB[1][0];
        l_huNew[l_ceR] -= i_scaling * l_netUpdatesA[1][1] - i_scaling * l_huvR;
        l_hvNew[l_ceR] -= i_scaling * l_huvR - i_scaling * l_netUpdatesB[1][1];

    }
}

void tsunami_lab::patches::WavePropagation2d::setGhostOutflow() {
  t_real * l_h = m_h[m_step];
  t_real * l_hu = m_hu[m_step];
  t_real * l_hv = m_hv[m_step];

  // set left boundary
  l_h[0] = l_h[1];
  l_hu[0] = l_hu[1];
  l_hv[0] = l_hv[1];

  // set right boundary
  l_h[m_nCells+1] = l_h[m_nCells];
  l_hu[m_nCells+1] = l_hu[m_nCells];
  l_hv[m_nCells+1] = l_hv[m_nCells];
}