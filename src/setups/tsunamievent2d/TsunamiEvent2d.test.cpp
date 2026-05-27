#include <catch2/catch.hpp>
#include "TsunamiEvent2d.h"
#include "string"

TEST_CASE("Test the two-dimensional tsunamiEvent setup.", "[TsunamiEvent2d]")
{
  tsunami_lab::setups::TsunamiEvent2d* l_tsunamiEvent2d = new tsunami_lab::setups::TsunamiEvent2d( "data/artificialtsunami_bathymetry_1000.nc" ,"data/artificialtsunami_displ_1000.nc");

  tsunami_lab::t_real l_momentumX = l_tsunamiEvent2d->getMomentumX(0,0);
  REQUIRE(l_momentumX == Approx(0));

  tsunami_lab::t_real l_momentumY = l_tsunamiEvent2d->getMomentumY(0,0);
  REQUIRE(l_momentumY == Approx(0));

  tsunami_lab::t_real l_bathymetryValue = l_tsunamiEvent2d->getBathymetry(0,0);
  REQUIRE(l_bathymetryValue == Approx(-99.84296f));
  REQUIRE(l_bathymetryValue == Approx(-99.84296f));
  l_bathymetryValue = l_tsunamiEvent2d->getBathymetry(0,4);
  REQUIRE(l_bathymetryValue == Approx( -99.84296f));
  l_bathymetryValue = l_tsunamiEvent2d->getBathymetry(9,4);
  REQUIRE(l_bathymetryValue == Approx(-100.15704f));

  tsunami_lab::t_real l_heightValue = l_tsunamiEvent2d->getHeight(2,1);
  l_heightValue = l_tsunamiEvent2d->getHeight(0,0);
  REQUIRE(l_heightValue == 100.0f);
  l_heightValue = l_tsunamiEvent2d->getHeight(0,4);
  REQUIRE(l_heightValue == 100.0f);
  l_heightValue = l_tsunamiEvent2d->getHeight(9,0);
  REQUIRE(l_heightValue == 100.0f);
  l_heightValue = l_tsunamiEvent2d->getHeight(9,4);
  REQUIRE(l_heightValue == 100.0f);

  delete l_tsunamiEvent2d;
}