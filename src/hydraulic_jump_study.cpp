/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Standalone runner for the hydraulic jump study.
 **/

#include "patches/WavePropagation1d.h"
#include "setups/HydraulicJump1d.h"
#include "io/Csv.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <string>

namespace {
  constexpr tsunami_lab::t_real g_gravity = 9.81f;

  struct ScenarioResult {
    tsunami_lab::t_real maxFroude = 0;
    tsunami_lab::t_real maxFroudePosition = 0;
    tsunami_lab::t_real jumpPosition = 0;
    tsunami_lab::t_real expectedMomentum = 0;
    tsunami_lab::t_real meanVelocityFinal = 0;
    tsunami_lab::t_real stdDevVelocityFinal = 0;
    tsunami_lab::t_real minVelocityFinal = 0;
    tsunami_lab::t_real maxVelocityFinal = 0;
  };

  ScenarioResult evaluateInitialState( tsunami_lab::setups::Setup const & i_setup,
                                       tsunami_lab::t_idx                   i_nx,
                                       tsunami_lab::t_real                 i_dxy ) {
    ScenarioResult l_result;
    l_result.maxFroude = std::numeric_limits< tsunami_lab::t_real >::lowest();
    l_result.maxFroudePosition = 0;

    for( tsunami_lab::t_idx l_cx = 0; l_cx < i_nx; ++l_cx ) {
      tsunami_lab::t_real l_x = (l_cx + 0.5f) * i_dxy;
      tsunami_lab::t_real l_h = i_setup.getHeight( l_x, 0 );
      tsunami_lab::t_real l_hu = i_setup.getMomentumX( l_x, 0 );

      if( l_h <= 0 ) {
        continue;
      }

      tsunami_lab::t_real l_u = l_hu / l_h;
      tsunami_lab::t_real l_froude = std::abs( l_u ) / std::sqrt( g_gravity * l_h );
      if( l_froude > l_result.maxFroude ) {
        l_result.maxFroude = l_froude;
        l_result.maxFroudePosition = l_x;
      }
    }

    return l_result;
  }

  tsunami_lab::t_real estimateJumpPosition( tsunami_lab::t_real const * i_height,
                                            tsunami_lab::t_idx           i_nx,
                                            tsunami_lab::t_real           i_dxy ) {
    if( i_nx < 2 ) {
      return 0;
    }

    tsunami_lab::t_idx l_bestCell = 0;
    tsunami_lab::t_real l_bestJump = std::numeric_limits< tsunami_lab::t_real >::lowest();

    for( tsunami_lab::t_idx l_cx = 0; l_cx + 1 < i_nx; ++l_cx ) {
      tsunami_lab::t_real l_jump = std::abs( i_height[l_cx + 1] - i_height[l_cx] );
      if( l_jump > l_bestJump ) {
        l_bestJump = l_jump;
        l_bestCell = l_cx;
      }
    }

    return (l_bestCell + 0.5f) * i_dxy;
  }

  void writeSnapshot( std::string const & i_prefix,
                      tsunami_lab::patches::WavePropagation1d const & i_waveProp,
                      tsunami_lab::t_real i_dxy,
                      tsunami_lab::t_idx i_nx,
                      tsunami_lab::t_idx i_snapshotId ) {
    std::string l_path = i_prefix + "_" + std::to_string( i_snapshotId ) + ".csv";
    std::ofstream l_file( l_path.c_str() );
    tsunami_lab::io::Csv::write( i_dxy,
                                 i_nx,
                                 1,
                                 1,
                                 i_waveProp.getHeight(),
                                 i_waveProp.getMomentumX(),
                                 nullptr,
                                 l_file );
  }

  struct MomentumAnalysis {
    tsunami_lab::t_real meanMomentum = 0;
    tsunami_lab::t_real minMomentum = std::numeric_limits< tsunami_lab::t_real >::max();
    tsunami_lab::t_real maxMomentum = std::numeric_limits< tsunami_lab::t_real >::lowest();
    tsunami_lab::t_real stdDevMomentum = 0;
    tsunami_lab::t_real expectedMomentum = 0;
  };

  MomentumAnalysis analyzeMomentum( tsunami_lab::t_real const * i_height,
                                    tsunami_lab::t_real const * i_hu,
                                    tsunami_lab::t_idx i_nx,
                                    tsunami_lab::t_real i_expectedHu ) {
    MomentumAnalysis l_result;
    l_result.expectedMomentum = i_expectedHu;

    if( i_nx < 1 ) {
      return l_result;
    }

    tsunami_lab::t_real l_sum = 0;
    tsunami_lab::t_real l_count = 0;

    for( tsunami_lab::t_idx l_cx = 0; l_cx < i_nx; ++l_cx ) {
      if( i_height[l_cx] > 0 ) {
        tsunami_lab::t_real l_u = i_hu[l_cx] / i_height[l_cx];
        l_sum += l_u;
        l_count += 1;

        l_result.minMomentum = std::min( l_result.minMomentum, l_u );
        l_result.maxMomentum = std::max( l_result.maxMomentum, l_u );
      }
    }

    if( l_count > 0 ) {
      l_result.meanMomentum = l_sum / l_count;

      tsunami_lab::t_real l_variance = 0;
      for( tsunami_lab::t_idx l_cx = 0; l_cx < i_nx; ++l_cx ) {
        if( i_height[l_cx] > 0 ) {
          tsunami_lab::t_real l_u = i_hu[l_cx] / i_height[l_cx];
          tsunami_lab::t_real l_diff = l_u - l_result.meanMomentum;
          l_variance += l_diff * l_diff;
        }
      }
      l_result.stdDevMomentum = std::sqrt( l_variance / l_count );
    }

    return l_result;
  }

  ScenarioResult runScenario( std::string const & i_name,
                              tsunami_lab::setups::Setup const & i_setup,
                              tsunami_lab::t_idx i_nx,
                              tsunami_lab::t_real i_endTime,
                              tsunami_lab::t_real i_dxy,
                              tsunami_lab::t_idx i_outputEvery,
                              tsunami_lab::t_real i_expectedMomentum ) {
    tsunami_lab::patches::WavePropagation1d l_waveProp( i_nx );

    tsunami_lab::t_real l_hMax = std::numeric_limits< tsunami_lab::t_real >::lowest();
    for( tsunami_lab::t_idx l_cx = 0; l_cx < i_nx; ++l_cx ) {
      tsunami_lab::t_real l_x = (l_cx + 0.5f) * i_dxy;
      tsunami_lab::t_real l_h = i_setup.getHeight( l_x, 0 );
      tsunami_lab::t_real l_hu = i_setup.getMomentumX( l_x, 0 );
      tsunami_lab::t_real l_hv = i_setup.getMomentumY( l_x, 0 );

      l_hMax = std::max( l_hMax, l_h );
      l_waveProp.setHeight( l_cx, 0, l_h );
      l_waveProp.setMomentumX( l_cx, 0, l_hu );
      l_waveProp.setMomentumY( l_cx, 0, l_hv );
    }

    ScenarioResult l_initial = evaluateInitialState( i_setup, i_nx, i_dxy );
    std::cout << i_name << " initial max Froude: " << l_initial.maxFroude
              << " at x = " << l_initial.maxFroudePosition << std::endl;

    tsunami_lab::t_real l_speedMax = std::sqrt( g_gravity * l_hMax );
    tsunami_lab::t_real l_dt = 0.5f * i_dxy / l_speedMax;
    tsunami_lab::t_real l_scaling = l_dt / i_dxy;

    tsunami_lab::t_real l_simTime = 0;
    tsunami_lab::t_idx l_timeStep = 0;
    tsunami_lab::t_idx l_snapshot = 0;

    while( l_simTime < i_endTime ) {
      if( l_timeStep % i_outputEvery == 0 ) {
        writeSnapshot( i_name, l_waveProp, i_dxy, i_nx, l_snapshot );
        ++l_snapshot;
      }

      l_waveProp.setGhostOutflow();
      l_waveProp.timeStep( l_scaling );

      l_simTime += l_dt;
      ++l_timeStep;
    }

    l_initial.jumpPosition = estimateJumpPosition( l_waveProp.getHeight(), i_nx, i_dxy );
    std::cout << i_name << " estimated jump position: " << l_initial.jumpPosition << std::endl;

    // Analyze final momentum: theoretical expectation is constant momentum
    MomentumAnalysis l_momentumAnalysis = analyzeMomentum( l_waveProp.getHeight(),
                                                           l_waveProp.getMomentumX(),
                                                           i_nx,
                                                           i_expectedMomentum );
    l_initial.expectedMomentum = l_momentumAnalysis.expectedMomentum;
    l_initial.meanVelocityFinal = l_momentumAnalysis.meanMomentum;
    l_initial.stdDevVelocityFinal = l_momentumAnalysis.stdDevMomentum;
    l_initial.minVelocityFinal = l_momentumAnalysis.minMomentum;
    l_initial.maxVelocityFinal = l_momentumAnalysis.maxMomentum;

    std::cout << i_name << " final velocity analysis:" << std::endl;
    std::cout << "  expected u: " << l_momentumAnalysis.expectedMomentum << std::endl;
    std::cout << "  mean u: " << l_momentumAnalysis.meanMomentum << std::endl;
    std::cout << "  std dev u: " << l_momentumAnalysis.stdDevMomentum << std::endl;
    std::cout << "  min u: " << l_momentumAnalysis.minMomentum << std::endl;
    std::cout << "  max u: " << l_momentumAnalysis.maxMomentum << std::endl;

    return l_initial;
  }
}

int main( int i_argc,
          char *i_argv[] ) {
  tsunami_lab::t_idx l_nx = 200;
  tsunami_lab::t_real l_endTime = 200.0f;
  tsunami_lab::t_idx l_outputEvery = 25;

  if( i_argc > 1 ) {
    l_nx = static_cast< tsunami_lab::t_idx >( std::atoi( i_argv[1] ) );
  }
  if( i_argc > 2 ) {
    l_endTime = static_cast< tsunami_lab::t_real >( std::atof( i_argv[2] ) );
  }
  if( i_argc > 3 ) {
    l_outputEvery = static_cast< tsunami_lab::t_idx >( std::atoi( i_argv[3] ) );
  }

  if( l_nx < 1 || l_outputEvery < 1 ) {
    std::cerr << "invalid arguments" << std::endl;
    return EXIT_FAILURE;
  }

  tsunami_lab::t_real l_dxy = 25.0f / static_cast< tsunami_lab::t_real >( l_nx );

  tsunami_lab::setups::HydraulicJumpSubcritical1d l_subcritical;
  tsunami_lab::setups::HydraulicJumpSupercritical1d l_supercritical;

  ScenarioResult l_sub = runScenario( "hydraulic_jump_subcritical",
                                      l_subcritical,
                                      l_nx,
                                      l_endTime,
                                      l_dxy,
                                      l_outputEvery,
                                      4.42f );

  ScenarioResult l_sup = runScenario( "hydraulic_jump_supercritical",
                                      l_supercritical,
                                      l_nx,
                                      l_endTime,
                                      l_dxy,
                                      l_outputEvery,
                                      0.18f );

  std::ofstream l_summary( "hydraulic_jump_summary.csv" );
  l_summary << "scenario,max_froude,max_froude_x,jump_x,expected_u,final_mean_u,final_std_u,final_min_u,final_max_u\n";
  l_summary << "subcritical," << l_sub.maxFroude << "," << l_sub.maxFroudePosition << "," 
            << l_sub.jumpPosition << "," << l_sub.expectedMomentum << "," 
            << l_sub.meanVelocityFinal << "," << l_sub.stdDevVelocityFinal << "," 
            << l_sub.minVelocityFinal << "," << l_sub.maxVelocityFinal << "\n";
  l_summary << "supercritical," << l_sup.maxFroude << "," << l_sup.maxFroudePosition << "," 
            << l_sup.jumpPosition << "," << l_sup.expectedMomentum << "," 
            << l_sup.meanVelocityFinal << "," << l_sup.stdDevVelocityFinal << "," 
            << l_sup.minVelocityFinal << "," << l_sup.maxVelocityFinal << "\n";

  std::cout << "summary written to hydraulic_jump_summary.csv" << std::endl;
  return EXIT_SUCCESS;
}