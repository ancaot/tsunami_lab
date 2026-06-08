#include <catch2/catch.hpp>
#include "../../constants.h"
#include <string>
#ifndef __clang__
#define private public
#endif
#include "Configuration.h"
#ifndef __clang__
#undef public
#endif

TEST_CASE( "Test the Json Reader", "[JsonReader]" ) {

    std::string l_variable = "solver";
    std::string l_outputString       = tsunami_lab::io::Configuration::readFromConfigString(l_variable); 
    REQUIRE(l_outputString == "FWAVE");

    l_variable = "cell_amount";
    tsunami_lab::t_real l_outputReal = tsunami_lab::io::Configuration::readFromConfigReal(l_variable); 
    REQUIRE(l_outputReal == 500.0);

    l_variable = "";
    bool l_outputBoolean = tsunami_lab::io::Configuration::readFromConfigReal(l_variable); 
    REQUIRE(l_outputBoolean == false);
}