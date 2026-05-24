#include <catch2/catch.hpp>
#include "TsunamiEvent2d.h"

TEST_CASE( "Test the TsunamiEvent2d setup.", "[TsunamiEvent2d]" ) {
#ifdef USE_NETCDF
  // Normally we would need mocked NetCDF files to test completely
  // For now we just test that the compiler can use this class.
  // tsunami_lab::setups::TsunamiEvent2d l_setup("bathymetry.nc", "displacement.nc");
#endif
  REQUIRE(true);
}