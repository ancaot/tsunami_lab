#include "fwave.h"
#include <cmath>

void tsunami_lab::solvers::fwave::eigenvalues( t_real   i_hL,
                                t_real   i_hR,
                                t_real   i_uL,
                                t_real   i_uR,
                                t_real & o_waveSpeedL,
                                t_real & o_waveSpeedR ){

  
  t_real l_hSqrtL = std::sqrt( i_hL );
  t_real l_hSqrtR = std::sqrt( i_hR );

  //determine the height : h_Roe 
  t_real l_hRoe = t_real(0.5) * ( i_hL + i_hR );

  //determine the particle velocity : u_Roe
  t_real l_uRoe = l_hSqrtL * i_uL + l_hSqrtR * i_uR;
  l_uRoe /= l_hSqrtL + l_hSqrtR;

  //compute the wave speeds
  t_real l_ghSqrtRoe = m_gSqrt * std::sqrt( l_hRoe );

  o_waveSpeedL = l_uRoe - l_ghSqrtRoe;
  o_waveSpeedR = l_uRoe + l_ghSqrtRoe;
}

void tsunami_lab::solvers::fwave::flux( t_real i_hL,
                                        t_real i_hR,
                                        t_real i_huL,
                                        t_real i_huR,
                                        t_real o_fdelta[2]){
    /*
     ___(1.3.3)___
     compute f(q_left):
     compute hu² = (hu)² / h
    */
    t_real pow2HuL= i_huL*i_huL/i_hL;
    t_real gPowHL = m_g * i_hL*i_hL;
    t_real totalL = pow2HuL + 0.5 * gPowHL;
    t_real fql[2] = {i_huL, totalL};

    //compute f(q_right)
    t_real pow2HuR= i_huR*i_huR/i_hR;
    t_real gPowHR = m_g * i_hR*i_hR;
    t_real totalR = pow2HuR + 0.5 * gPowHR;
    t_real fqr[2] = {i_huR, totalR};
    //compute Δf
    o_fdelta[0] = fqr[0] - fql[0];
    o_fdelta[1] = fqr[1] - fql[1];
}
 

void tsunami_lab::solvers::fwave::decompose(t_real i_alphas[2],
                                            t_real i_eigens[2],
                                            t_real o_minus_A_deltaQ[2], 
                                            t_real o_plus_A_deltaQ[2]){
 
    //Negative speed of wave propagation                                    
    if( i_eigens[0] < 0){
        o_minus_A_deltaQ[0] +=  i_alphas[0];
        o_minus_A_deltaQ[1] += (i_alphas[0] * i_eigens[0]);
    }else
    {
        o_plus_A_deltaQ[0] +=   i_alphas[0];
        o_plus_A_deltaQ[1] +=  (i_alphas[0] * i_eigens[0]);
    }
    //Positive speed of wave propagation
    if(i_eigens[1] < 0){
        o_minus_A_deltaQ[0] += o_minus_A_deltaQ[0] + (i_alphas[1]);
        o_minus_A_deltaQ[1] += o_minus_A_deltaQ[1] + (i_alphas[1] * i_eigens[1]);
        
    }else{
        o_plus_A_deltaQ[0] += o_plus_A_deltaQ[0] + (i_alphas[1]);
        o_plus_A_deltaQ[1] += o_plus_A_deltaQ[1] + (i_alphas[1] * i_eigens[1]) ;
    }
 }

    
void tsunami_lab::solvers::fwave::inverseMatrix(t_real i_eigen1,
                                                t_real i_eigen2,
                                                t_real o_inverse[4]){
    //Compute the determinant of a 2x2 matrix 
    t_real det = 1 / (i_eigen2 - i_eigen1);

    //Compute the inverses of specific 2x2 matrices. (first row contains only ones)
    o_inverse[0] = i_eigen2 * det;
    o_inverse[1] = -1 * det;
    o_inverse[2] = -i_eigen1 * det;
    o_inverse[3] = 1 * det;
    
}

void tsunami_lab::solvers::fwave::eigencoefficientAlpha(t_real i_inverse[4],
                                                        t_real i_delta_f[2],
                                                        t_real i_b,
                                                        t_real o_eigencoefficients[2]){

    i_delta_f[1] = i_delta_f[1] - i_b;
    //m x n ° n x p = 
    o_eigencoefficients[0] = (i_inverse[0] * i_delta_f[0]) + (i_inverse[1] * i_delta_f[1]);
    o_eigencoefficients[1] = (i_inverse[2] * i_delta_f[0]) + (i_inverse[3] * i_delta_f[1]);

}

void tsunami_lab::solvers::fwave::netUpdates(t_real   i_hL,
                                             t_real   i_hR,
                                             t_real   i_huL,
                                             t_real   i_huR,
                                             t_real   i_bL,
                                             t_real   i_bR,
                                             t_real   o_minus_A_deltaQ[2],
                                             t_real   o_plus_A_deltaQ[2]){

    //two dry cells next to each other you cant divide by zero
        o_minus_A_deltaQ[1] = 0;
        o_minus_A_deltaQ[0] = 0;
        o_plus_A_deltaQ[1] = 0;
        o_plus_A_deltaQ[0] = 0;
        t_real temp[2] = {};
    if(i_hL <= 0){

        if(i_hR <= 0){
            return;
        }
        i_hL = i_hR;
        i_huL = -i_huR;
        i_bL = i_bR;
        o_minus_A_deltaQ = temp;
    }
    //left cell is a dry cell
    else if(i_hR <= 0){
        i_hR = i_hL;
        i_huR = -i_huL;
        i_bR = i_bL;
        o_plus_A_deltaQ = temp;
    }





    t_real l_uL = i_huL / i_hL;
    t_real l_uR = i_huR / i_hR;
      
    t_real l_sL = 0;
    t_real l_sR = 0;

    eigenvalues(i_hL,i_hR,l_uL,l_uR,l_sL,l_sR);

    t_real l_inverse[4];

    inverseMatrix(l_sL, l_sR, l_inverse);

    t_real l_fdelta[2] = {0,0};
    flux(i_hL,i_hR,i_huL,i_huR,l_fdelta);
    
    t_real l_bathymetry = (-m_g) * (i_bR-i_bL) *((i_hL+i_hR)/2);

    t_real l_eigencoefficients[2];
    eigencoefficientAlpha(l_inverse,l_fdelta,l_bathymetry,l_eigencoefficients);

   
    t_real l_eigens[2] = {l_sL,l_sR};
    decompose(l_eigencoefficients,l_eigens,o_minus_A_deltaQ,o_plus_A_deltaQ);

}