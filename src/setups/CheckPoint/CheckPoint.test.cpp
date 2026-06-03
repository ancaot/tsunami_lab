#include <catch2/catch.hpp>
#include "CheckPoint.h"

TEST_CASE("Test CheckPoint setup values", "[CheckPointSetup]") {
  auto * l_h = new tsunami_lab::t_real[4]{1, 2, 3, 4};
  auto * l_hu = new tsunami_lab::t_real[4]{5, 6, 7, 8};
  auto * l_hv = new tsunami_lab::t_real[4]{9, 10, 11, 12};
  auto * l_b = new tsunami_lab::t_real[4]{13, 14, 15, 16};

  tsunami_lab::setups::CheckPoint l_setup(20, 20, 2, 2, 0, 0, l_h, l_hu, l_hv, l_b);

  REQUIRE(l_setup.getHeight(5, 5) == Approx(1));
  REQUIRE(l_setup.getMomentumX(15, 5) == Approx(6));
  REQUIRE(l_setup.getMomentumY(5, 15) == Approx(11));
  REQUIRE(l_setup.getBathymetry(15, 15) == Approx(16));
}
