#include <catch2/catch.hpp>
#include "NetCdf.h"

TEST_CASE( "Test the NetCdf IO.", "[NetCdf]" ) {
  // It is hard to test NetCDF without creating a file, but we can verify the API or missing file behavior.
  tsunami_lab::io::NetCdf l_netCdf("non_existent_file.nc");
  REQUIRE(!l_netCdf.isOpen());
}