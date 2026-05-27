#include <catch2/catch.hpp>
#include "../../constants.h"
#include <sstream>
#define private public
#include "Station.h"
#include "string"
#include <sstream>
#include <iostream>
#include <fstream>

#undef public


TEST_CASE( "Test the Station IO", "[Station]" ) {

    tsunami_lab::t_idx l_x = 5;
    tsunami_lab::t_idx l_y = 0;
    tsunami_lab::t_idx l_time = 0;
    tsunami_lab::t_real l_water_height = 7.56;
    tsunami_lab::t_real l_water_hu = 7.56;
    tsunami_lab::t_real l_water_hv = 7.56;
    if (std::filesystem::exists("tests")) std::filesystem::remove_all("tests");
    std::filesystem::create_directory("tests");
    std::string l_csv_path = "tests/test_station_io.csv";

    tsunami_lab::io::Station::write(l_x,
                                    l_y,
                                    l_time,
                                    l_water_height,
                                    l_water_hu,
                                    l_water_hv,
                                    l_csv_path,
                                    "2d");
    std::ifstream inputFile(l_csv_path);
    std::stringstream csvStream;
    std::string line;
    while (std::getline(inputFile, line)) {
        csvStream << line << '\n';
    }
    std::string l_ref0 = R"V0G0N(x,y,water_height,water_hu,water_hv,time_in_seconds
5,0,7.56,7.56,7.56,0
)V0G0N";
    REQUIRE( csvStream.str().size() == l_ref0.size() );
    REQUIRE( csvStream.str() == l_ref0 );

    inputFile.close();
    if (std::filesystem::exists("tests")) std::filesystem::remove_all("tests");
}
