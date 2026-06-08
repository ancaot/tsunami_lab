
#include <catch2/catch.hpp>
#include <catch2/catch.hpp>
#include "../../constants.h"
#include <sstream>
#include <vector>
#ifndef __clang__
#define private public
#endif
#include "NetCdf.h"
#include <filesystem>
#include <cstdio>
#include <string>
#ifndef __clang__
#undef public
#endif

TEST_CASE("Test the NetCdf-writer", "[NetCdfWrite]")
{

    tsunami_lab::t_real l_h[28] = {0, 1, 2, 3, 4, 5, 0,
                                   1, 1, 2, 3, 4, 5, 4,
                                   6, 6, 7, 8, 9, 10, 10,
                                   7, 7, 8, 9, 10, 11, 11};

    tsunami_lab::t_real l_hu[28] = {12, 12, 13, 14, 15, 16, 16,
                                    17, 17, 18, 19, 20, 21, 21,
                                    22, 22, 23, 24, 25, 26, 26,
                                    27, 27, 28, 29, 30, 31, 31};

    tsunami_lab::t_real l_hv[28] = {32, 32, 33, 34, 35, 36, 36,
                                    37, 37, 38, 39, 40, 41, 41,
                                    42, 42, 43, 44, 45, 46, 46,
                                    47, 47, 48, 49, 50, 51, 51};

    tsunami_lab::t_real l_b[28] = {52, 52, 53, 54, 55, 56, 56,
                                   57, 57, 58, 59, 60, 61, 61,
                                   62, 62, 63, 64, 65, 66, 66,
                                   67, 67, 68, 69, 70, 71, 71};

    tsunami_lab::io::NetCdf *l_netCdf = new tsunami_lab::io::NetCdf(5, 2, 1, "testsFiles/test.nc");
    l_netCdf->fillConstants(5, 2, 7, 0.5, -50, -50, l_b, "testsFiles/test.nc");
    l_netCdf->updateFile(5, 2, 7, 0, 1, l_h, l_hu, l_hv, "testsFiles/test.nc");
    int l_err;
    int l_ncid;
    int l_dimXId, l_dimYId, l_TimeId;
    size_t l_nx, l_ny, l_nt;
    int l_vHId, l_vXId, l_vHuId, l_vYId, l_vTimeId, l_vHvId, l_vBId;

    REQUIRE(std::filesystem::exists("testsFiles/test.nc"));

    l_err = nc_open("testsFiles/test.nc", NC_NOWRITE, &l_ncid);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_dimid(l_ncid, "x", &l_dimXId);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_dimid(l_ncid, "y", &l_dimYId);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_dimid(l_ncid, "time", &l_TimeId);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_dimlen(l_ncid, l_dimXId, &l_nx);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_dimlen(l_ncid, l_dimYId, &l_ny);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

    l_err = nc_inq_dimlen(l_ncid, l_TimeId, &l_nt);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

    REQUIRE(l_nx == 5);
    REQUIRE(l_ny == 2);
    REQUIRE(l_nt == 1);

    l_err = nc_inq_varid(l_ncid, "x", &l_vXId);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_inq_varid(l_ncid, "y", &l_vYId);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_inq_varid(l_ncid, "time", &l_vTimeId);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_inq_varid(l_ncid, "h", &l_vHId);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_inq_varid(l_ncid, "hu", &l_vHuId);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_inq_varid(l_ncid, "hv", &l_vHvId);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_inq_varid(l_ncid, "b", &l_vBId);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

    std::vector<tsunami_lab::t_real> x(5);
    std::vector<tsunami_lab::t_real> y(2);
    std::vector<tsunami_lab::t_real> time(1);
    std::vector<tsunami_lab::t_real> height(10);
    std::vector<tsunami_lab::t_real> momentum_x(10);
    std::vector<tsunami_lab::t_real> momentum_y(10);
    std::vector<tsunami_lab::t_real> bathymetry(10);

    l_err = nc_get_var_float(l_ncid, l_vXId, &x[0]);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_get_var_float(l_ncid, l_vYId, &y[0]);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_get_var_float(l_ncid, l_vTimeId, &time[0]);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_get_var_float(l_ncid, l_vHId, &height[0]);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_get_var_float(l_ncid, l_vHuId, &momentum_x[0]);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_get_var_float(l_ncid, l_vHvId, &momentum_y[0]);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);
    l_err = nc_get_var_float(l_ncid, l_vBId, &bathymetry[0]);
    l_netCdf->checkNcErr(l_err, __FILE__, __LINE__);

    REQUIRE(x[0] == Approx(-49.75f));
    REQUIRE(x[1] == Approx(-49.25f));
    REQUIRE(x[2] == Approx(-48.75f));
    REQUIRE(x[3] == Approx(-48.25f));
    REQUIRE(x[4] == Approx(-47.75f));

    REQUIRE(y[0] == Approx(-49.75f));
    REQUIRE(y[1] == Approx(-49.25f));

    REQUIRE(time[0] == Approx(1));

    REQUIRE(momentum_x[0] == Approx(17));
    REQUIRE(momentum_x[1] == Approx(18));
    REQUIRE(momentum_x[2] == Approx(19));
    REQUIRE(momentum_x[3] == Approx(20));
    REQUIRE(momentum_x[4] == Approx(21));
    REQUIRE(momentum_x[5] == Approx(22));
    REQUIRE(momentum_x[6] == Approx(23));
    REQUIRE(momentum_x[7] == Approx(24));
    REQUIRE(momentum_x[8] == Approx(25));
    REQUIRE(momentum_x[9] == Approx(26));

    REQUIRE(momentum_y[0] == Approx(37));
    REQUIRE(momentum_y[1] == Approx(38));
    REQUIRE(momentum_y[2] == Approx(39));
    REQUIRE(momentum_y[3] == Approx(40));
    REQUIRE(momentum_y[4] == Approx(41));
    REQUIRE(momentum_y[5] == Approx(42));
    REQUIRE(momentum_y[6] == Approx(43));
    REQUIRE(momentum_y[7] == Approx(44));
    REQUIRE(momentum_y[8] == Approx(45));
    REQUIRE(momentum_y[9] == Approx(46));

    REQUIRE(height[0] == Approx(1));
    REQUIRE(height[1] == Approx(2));
    REQUIRE(height[2] == Approx(3));
    REQUIRE(height[3] == Approx(4));
    REQUIRE(height[4] == Approx(5));
    REQUIRE(height[5] == Approx(6));
    REQUIRE(height[6] == Approx(7));
    REQUIRE(height[7] == Approx(8));
    REQUIRE(height[8] == Approx(9));
    REQUIRE(height[9] == Approx(10));

    REQUIRE(bathymetry[0] == Approx(57));
    REQUIRE(bathymetry[1] == Approx(58));
    REQUIRE(bathymetry[2] == Approx(59));
    REQUIRE(bathymetry[3] == Approx(60));
    REQUIRE(bathymetry[4] == Approx(61));
    REQUIRE(bathymetry[5] == Approx(62));
    REQUIRE(bathymetry[6] == Approx(63));
    REQUIRE(bathymetry[7] == Approx(64));
    REQUIRE(bathymetry[8] == Approx(65));
    REQUIRE(bathymetry[9] == Approx(66));

    delete l_netCdf;
    std::string path = "testsFiles/test.nc";
    remove(path.c_str());
}

TEST_CASE("Test the NetCdf-reader ", "[NetCdfreader]")
{

    tsunami_lab::t_real *l_data;
    tsunami_lab::t_idx l_nx;
    tsunami_lab::t_idx l_ny;
    tsunami_lab::io::NetCdf::read("testsFiles/testFileForTheReader.nc", "data", l_nx, l_ny, &l_data, nullptr, nullptr);
    for (tsunami_lab::t_idx i = 0; i < 71; i++)
    {
        REQUIRE(i == l_data[i]);
    }
    REQUIRE(6 == l_nx);
    REQUIRE(12 == l_ny);
    delete[] l_data;
}

TEST_CASE("Test the NetCdf-checkpoint-writer", "[NetCdfCheckpoint]"){

    tsunami_lab::t_real l_h[16] = {0, 0, 0, 0, 
                                   0, 1, 2, 0, 
                                   0, 6, 7, 0, 
                                   0, 0, 0, 0, };

    tsunami_lab::t_real l_hu[16] = {0, 0, 0, 0, 
                                    0, 17, 18, 0, 
                                    0, 22, 23, 0, 
                                    0, 0, 0, 0, };

    tsunami_lab::t_real l_hv[16] = {0, 0, 0, 0, 
                                    0, 37, 38, 0, 
                                    0, 42, 43, 0, 
                                    0, 0, 0, 0, };

    tsunami_lab::t_real l_b[16] = {0, 0, 0, 0, 
                                   0, 57, 58, 0, 
                                   0, 62, 63, 0, 
                                   0, 0, 0, 0, };

    tsunami_lab::t_real l_h_read_result[4] = {1, 2, 6, 7};
    tsunami_lab::t_real l_hu_read_result[4] = {17, 18, 22, 23};
    tsunami_lab::t_real l_hv_read_result[4] = {37, 38, 42, 43};
    tsunami_lab::t_real l_b_read_result[4] = {57, 58, 62, 63};

    tsunami_lab::io::NetCdf *l_netCdf = new tsunami_lab::io::NetCdf(2, 2, 1, "testsFiles/testCheckPoint.nc");
    REQUIRE(std::filesystem::exists("testsFiles/testCheckPoint.nc"));

    std::string l_path = "outputs";
    std::string l_cpPath = l_path + "/checkpoints";

    if(!std::filesystem::exists(l_path)){
        std::filesystem::create_directory(l_path);
    }
    if(!std::filesystem::exists(l_cpPath)){
        std::filesystem::create_directory(l_cpPath);
    }

    l_netCdf->createCheckPoint("fwave", "tsunamievent2d", "2d", 50, 50, 2, 2, -25, -25, 360, 
                                "csv", "bathfile.nc", "dispfile.nc", "output.csv", false, 60, 
                                l_hu, l_hv, l_h,  0, l_b, 4, 250, "testCheckPoint.nc");
    REQUIRE(std::filesystem::exists("outputs/checkpoints/testCheckPoint.nc"));


    // reading file
    std::string o_solver;
    std::string o_scenario;
    std::string o_waveModel;
    tsunami_lab::t_real o_domainSizeX;
    tsunami_lab::t_real o_domainSizeY;
    tsunami_lab::t_idx o_nx;
    tsunami_lab::t_idx o_ny;
    tsunami_lab::t_real o_domainStartX;
    tsunami_lab::t_real o_domainStartY;
    tsunami_lab::t_real o_simEndTime;
    std::string o_outputFormat;
    std::string o_bathFile;
    std::string o_dispFile;
    std::string o_outputName;
    bool o_reflectiveBoundary;
    tsunami_lab::t_real o_simTimeLastCP;
    tsunami_lab::t_real *l_hua;
    tsunami_lab::t_real *l_hva;
    tsunami_lab::t_real *l_ha;
    tsunami_lab::t_real o_damLocation;
    tsunami_lab::t_real *l_ba;

    tsunami_lab::io::NetCdf::readCheckPoint("outputs/checkpoints/testCheckPoint.nc",
                                            &o_solver,
                                            &o_scenario,
                                            &o_waveModel,
                                            &o_domainSizeX,
                                            &o_domainSizeY,
                                            &o_nx,
                                            &o_ny,
                                            &o_domainStartX,
                                            &o_domainStartY,
                                            &o_simEndTime,
                                            &o_outputFormat,
                                            &o_bathFile,
                                            &o_dispFile,
                                            &o_outputName,
                                            &o_reflectiveBoundary,
                                            &o_simTimeLastCP,
                                            &l_hua,
                                            &l_hva,
                                            &l_ha,
                                            &o_damLocation,
                                            &l_ba);

    //checking
    REQUIRE(o_solver == "fwave");
    REQUIRE(o_scenario == "tsunamievent2d");
    REQUIRE(o_waveModel == "2d");
    REQUIRE(o_domainSizeX == 50);
    REQUIRE(o_domainSizeY == 50);
    REQUIRE(o_nx == 2);
    REQUIRE(o_ny == 2);
    REQUIRE(o_domainStartX == -25);
    REQUIRE(o_domainStartY == -25);
    REQUIRE(o_simEndTime == 360);

    REQUIRE(o_outputFormat == "csv");
    REQUIRE(o_bathFile ==  "bathfile.nc");
    REQUIRE(o_dispFile ==  "dispfile.nc");
    REQUIRE(o_outputName == "output.csv");
    REQUIRE(o_reflectiveBoundary == false);
    REQUIRE(o_simTimeLastCP == 60);

    REQUIRE(o_damLocation == 0);

    for (tsunami_lab::t_idx l_i = 0; l_i < 4; l_i++){
        REQUIRE(l_hua[l_i] == l_hu_read_result[l_i]);
        REQUIRE(l_hva[l_i] == l_hv_read_result[l_i]);
        REQUIRE(l_ha[l_i] == l_h_read_result[l_i]);
        REQUIRE(l_ba[l_i] == l_b_read_result[l_i]);
    }

    delete[] l_hua;
    delete[] l_hva;
    delete[] l_ha;
    delete[] l_ba;

    std::string path = "outputs/checkpoints/testCheckPoint.nc";
    remove(path.c_str());
    path = "testFiles/testCheckPoint.nc";
    remove(path.c_str());

}
