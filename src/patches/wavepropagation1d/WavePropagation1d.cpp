/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional wave propagation patch.
 **/
#include "WavePropagation1d.h"
#include "../../solvers/Roe.h"
#include "../../solvers/fwave.h"



tsunami_lab::patches::WavePropagation1d::WavePropagation1d( t_idx i_nCells, bool i_choice,bool i_choiceBoundry ) {
  m_choiceBoundry = i_choiceBoundry;
  m_choice = i_choice;
  m_nCells = i_nCells;

  // allocate memory including a single ghost cell on each side
  for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
    m_h[l_st] = new t_real[  m_nCells + 2 ];
    m_hu[l_st] = new t_real[ m_nCells + 2 ];
  }
  m_b = new t_real[ m_nCells + 2 ];

  // init to zero
  for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
    for( t_idx l_ce = 0; l_ce < m_nCells+2; l_ce++ ) {
      m_h[l_st][l_ce] = 0;
      m_hu[l_st][l_ce] = 0;
      m_b[l_ce] = 0;
    }
  }
}
 //free memory
tsunami_lab::patches::WavePropagation1d::~WavePropagation1d() {
  for( unsigned short l_st = 0; l_st < 2; l_st++ ) {
    delete[] m_h[l_st];
    delete[] m_hu[l_st];
  }
  delete[] m_b;
}

void tsunami_lab::patches::WavePropagation1d::timeStep( t_real i_scaling) {
  // pointers to old and new data
  t_real * l_hOld  = m_h[m_step];
  t_real * l_huOld = m_hu[m_step];


  t_real * l_b  = m_b; 
  /**
  * explanation :"m_step = (m_step+1) % 2;""
  *  after every call the new data gets stored in the oldest values
  * [old,new] ->compute newer values ->[newer,new]
  * new data are now the old and newer becomes the new data
  */
  m_step = (m_step+1) % 2;
  t_real * l_hNew =  m_h[m_step];
  t_real * l_huNew = m_hu[m_step];


  // init new cell quantities
  for( t_idx l_ce = 1; l_ce < m_nCells+1; l_ce++ ) {
    l_hNew[l_ce]  = l_hOld[l_ce];
    l_huNew[l_ce] = l_huOld[l_ce];
  }

  // iterate over edges and update with Riemann solutions
  for( t_idx l_ed = 0; l_ed < m_nCells+1; l_ed++ ) {
    // determine left and right cell-id
    t_idx l_ceL = l_ed;
    t_idx l_ceR = l_ed+1;

    // compute net-updates
    t_real l_netUpdates[2][2];
    //std:: cout << l_bOld[l_ceR] << std::endl ;

    if(m_choice){
      solvers::Roe::netUpdates(l_hOld[l_ceL],
                              l_hOld[l_ceR],
                              l_huOld[l_ceL],
                              l_huOld[l_ceR],
                              l_netUpdates[0],
                              l_netUpdates[1]);
    }else{
      solvers::fwave::netUpdates( l_hOld[l_ceL],
                                  l_hOld[l_ceR],
                                  l_huOld[l_ceL],
                                  l_huOld[l_ceR],
                                  l_b[l_ceL],
                                  l_b[l_ceR],
                                  l_netUpdates[0],
                                  l_netUpdates[1]);
    }
    
    // update the cells' quantities
    l_hNew[l_ceL]  -= i_scaling * l_netUpdates[0][0];
    l_huNew[l_ceL] -= i_scaling * l_netUpdates[0][1];


    l_hNew[l_ceR]  -= i_scaling * l_netUpdates[1][0];
    l_huNew[l_ceR] -= i_scaling * l_netUpdates[1][1];
    
  }
  setGhostCollumn();
} 

void tsunami_lab::patches::WavePropagation1d::setGhostCollumn() {
  t_real * l_h = m_h[m_step];
  t_real * l_hu = m_hu[m_step];
  t_real * l_b = m_b;

  // set left boundary
  l_h[0] = l_h[1];
  l_b[0] = l_b[1];

    // set right boundary
  l_h[m_nCells+1] = l_h[m_nCells];
  l_b[m_nCells+1] = l_b[m_nCells];


  if(m_choiceBoundry){
    //reflecting boundary :same values except that the reflecting cell receives the particel velocity with opposite sign
    l_hu[m_nCells+ 1] = -(l_hu[m_nCells ]);
    l_hu[0] = -l_hu[1];
  }
  else
  {
    l_hu[m_nCells+1] = l_hu[m_nCells];
    l_hu[0] = l_hu[1];
  }
}