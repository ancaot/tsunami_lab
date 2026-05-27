#include <catch2/catch.hpp>
#include "ArtificialTsunami2d.h"
#include "string"

TEST_CASE("Test the two-dimensional ArtificialTsunami2d setup.", "[ArtificialTsunami2d]")
{
  tsunami_lab::setups::ArtificialTsunami2d* l_ArtificialTsunami2d = new tsunami_lab::setups::ArtificialTsunami2d();

  tsunami_lab::t_real l_momentumX = l_ArtificialTsunami2d->getMomentumX(0,0);
  REQUIRE(l_momentumX == Approx(0));

  tsunami_lab::t_real l_momentumY = l_ArtificialTsunami2d->getMomentumY(0,0);
  REQUIRE(l_momentumY == Approx(0));

  tsunami_lab::t_real l_bathymetryValue = l_ArtificialTsunami2d->getBathymetry(0,0);
  REQUIRE(l_bathymetryValue == Approx(-99.84296f));
  l_bathymetryValue = l_ArtificialTsunami2d->getBathymetry(9,0);
  REQUIRE(l_bathymetryValue == Approx(-99.84296f));
  l_bathymetryValue = l_ArtificialTsunami2d->getBathymetry(0,4);
  REQUIRE(l_bathymetryValue == Approx(-100.15704f));
  l_bathymetryValue = l_ArtificialTsunami2d->getBathymetry(9,4);
  REQUIRE(l_bathymetryValue == Approx( -100.15704f));

  tsunami_lab::t_real l_heightValue = l_ArtificialTsunami2d->getHeight(2,1);
  l_heightValue = l_ArtificialTsunami2d->getHeight(0,0);
  REQUIRE(l_heightValue == 100.0f);
  l_heightValue = l_ArtificialTsunami2d->getHeight(0,4);
  REQUIRE(l_heightValue == 100.0f);
  l_heightValue = l_ArtificialTsunami2d->getHeight(9,0);
  REQUIRE(l_heightValue == 100.0f);
  l_heightValue = l_ArtificialTsunami2d->getHeight(9,4);
  REQUIRE(l_heightValue == 100.0f);
  delete l_ArtificialTsunami2d;
}