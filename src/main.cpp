#include "patches/wavepropagation1d/WavePropagation1d.h"
#include "setups/tsunamievent2d/TsunamiEvent2d.h"
#include "patches/wavepropagation2d/WavePropagation2d.h"
#include "setups/ArtificialTsunami2d/ArtificialTsunami2d.h"
#include "setups/dambreak/DamBreak1d.h"
#include "setups/dambreak2d/DamBreak2d.h"
#include "setups/rarerare/RareRare.h"
#include "setups/shockshock/ShockShock.h"
#include "setups/subcriticalflow/SubcriticalFlow.h"
#include "setups/supercriticalflow/SupercriticalFlow.h"
#include "setups/tsunamievent1d/TsunamiEvent1d.h"
#include "io/Csv/Csv.h"
#include "io/NetCdf/NetCdf.h"
#include "io/Stations/Station.h"
#include "io/JsReader/Configuration.h"
#include "setups/CheckPoint/CheckPoint.h"
#include <filesystem>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include <thread>
#include <ctime>

#ifdef _WIN32
#include <conio.h>
#endif




void updateProgressBar(double current, double total, int width) {
  const double progress = std::min(std::max((current / total) * 100.0, 0.0), 100.0);
  const int numHashes = static_cast<int>((progress / 100.0) * width);

  std::cout << "\r\033[1;95mProgress: ["
            << std::string(numHashes, '#') << std::string(width - numHashes, ' ')
            << "] " << static_cast<int>(progress) << "%";

  std::cout.flush();
}


void ensureOutputDirectories() {
  if (!std::filesystem::exists("outputs")) {
    std::filesystem::create_directory("outputs");
  }
  if (!std::filesystem::exists("stations")) {
    std::filesystem::create_directory("stations");
  }
}

std::string generateNewName( std::string name,  std::string filePath) {
    std::string newName = name;
    std::string fullPath = filePath + newName;

    int counter = 1;
    while (std::ifstream(fullPath)) {
        std::ostringstream oss;
        oss << name.substr(0, name.find_last_of(".")) << "_" << counter << name.substr(name.find_last_of("."));
        newName = oss.str();
        fullPath = filePath + newName;
        counter++;
    }

    return newName;
}

namespace cli {
  const char* const reset = "\033[0m";
  const char* const soft = "\033[38;5;245m";
  const char* const pink = "\033[38;5;213m";
  const char* const cyan = "\033[38;5;45m";
  const char* const violet = "\033[38;5;141m";
  const char* const red = "\033[38;5;203m";
  const char* const green = "\033[38;5;120m";

  std::string trim(std::string i_text) {
    const auto l_begin = i_text.find_first_not_of(" \t\r\n");
    if (l_begin == std::string::npos) return "";
    const auto l_end = i_text.find_last_not_of(" \t\r\n");
    return i_text.substr(l_begin, l_end - l_begin + 1);
  }

  std::string lower(std::string i_text) {
    std::transform(i_text.begin(), i_text.end(), i_text.begin(), [](unsigned char c) {
      return static_cast<char>(std::tolower(c));
    });
    return i_text;
  }

  void printLogo() {
    std::cout << pink
              << "  _______ _____ _    _ _   _          __  __ _____ \n"
              << " |__   __/ ____| |  | | \\ | |   /\\   |  \\/  |_   _|\n"
              << "    | | | (___ | |  | |  \\| |  /  \\  | \\  / | | |  \n"
              << "    | |  \\___ \\| |  | | . ` | / /\\ \\ | |\\/| | | |  \n"
              << "    | |  ____) | |__| | |\\  |/ ____ \\| |  | |_| |_ \n"
              << "    |_| |_____/ \\____/|_| \\_/_/    \\_\\_|  |_|_____|\n"
              << reset;
    std::cout << cyan << "        wave simulation console" << soft
              << "  |  config-aware interactive launch\n\n" << reset;
  }

  void section(const std::string& i_title) {
    std::cout << "\n" << violet << "== " << i_title << " ==" << reset << std::endl;
  }

  enum class Key {
    up,
    down,
    enter,
    other
  };

  Key readKey() {
#ifdef _WIN32
    const int l_key = _getch();
    if (l_key == 13) return Key::enter;
    if (l_key == 224 || l_key == 0) {
      const int l_arrow = _getch();
      if (l_arrow == 72) return Key::up;
      if (l_arrow == 80) return Key::down;
    }
    return Key::other;
#else
    const int l_key = std::cin.get();
    if (l_key == '\n') return Key::enter;
    if (l_key == 27 && std::cin.get() == '[') {
      const int l_arrow = std::cin.get();
      if (l_arrow == 'A') return Key::up;
      if (l_arrow == 'B') return Key::down;
    }
    return Key::other;
#endif
  }

  json loadConfig(const std::string& i_path) {
    std::ifstream l_file(i_path);
    if (!l_file.is_open()) {
      throw std::runtime_error("Unable to open " + i_path);
    }
    json l_data;
    l_file >> l_data;
    return l_data;
  }

  bool hasAny(const json& i_data, const std::vector<std::string>& i_names) {
    for (const std::string& l_name : i_names) {
      if (i_data.contains(l_name)) return true;
    }
    return false;
  }

  template< typename T >
  T getOr(const json& i_data, const std::vector<std::string>& i_names, T i_default) {
    for (const std::string& l_name : i_names) {
      if (i_data.contains(l_name)) {
        return i_data.at(l_name).get<T>();
      }
    }
    return i_default;
  }

  std::string promptString(const std::string& i_label, const std::string& i_default) {
    std::cout << cyan << "? " << reset << i_label << " "
              << soft << "[" << i_default << "]" << reset << ": ";
    std::string l_input;
    std::getline(std::cin, l_input);
    l_input = trim(l_input);
    return l_input.empty() ? i_default : l_input;
  }

  tsunami_lab::t_real promptReal(const std::string& i_label, tsunami_lab::t_real i_default) {
    while (true) {
      const std::string l_answer = promptString(i_label, std::to_string(i_default));
      std::istringstream l_stream(l_answer);
      tsunami_lab::t_real l_value = 0;
      if (l_stream >> l_value) return l_value;
      std::cout << red << "  Please enter a number." << reset << std::endl;
    }
  }

  tsunami_lab::t_idx promptIndex(const std::string& i_label, tsunami_lab::t_idx i_default) {
    while (true) {
      const std::string l_answer = promptString(i_label, std::to_string(i_default));
      std::istringstream l_stream(l_answer);
      tsunami_lab::t_idx l_value = 0;
      if (l_stream >> l_value && l_value > 0) return l_value;
      std::cout << red << "  Please enter a positive integer." << reset << std::endl;
    }
  }

  bool promptBool(const std::string& i_label, bool i_default) {
    const std::string l_default = i_default ? "yes" : "no";
    while (true) {
      std::string l_answer = lower(promptString(i_label + " (yes/no)", l_default));
      if (l_answer == "yes" || l_answer == "y" || l_answer == "true" || l_answer == "1") return true;
      if (l_answer == "no" || l_answer == "n" || l_answer == "false" || l_answer == "0") return false;
      std::cout << red << "  Please answer yes or no." << reset << std::endl;
    }
  }

  std::string promptChoice(const std::string& i_label,
                           const std::vector<std::string>& i_options,
                           const std::string& i_default) {
    if (i_options.empty()) return i_default;

    std::size_t l_selected = 0;
    for (std::size_t l_i = 0; l_i < i_options.size(); ++l_i) {
      if (lower(i_options[l_i]) == lower(i_default)) {
        l_selected = l_i;
        break;
      }
    }

    std::cout << cyan << "? " << reset << i_label << " "
              << soft << "[" << i_default << "]" << reset
              << soft << "  use up/down, enter to confirm" << reset << std::endl;

    auto l_render = [&]() {
      for (std::size_t l_i = 0; l_i < i_options.size(); ++l_i) {
        std::cout << "\033[2K";
        if (l_i == l_selected) {
          std::cout << pink << "  > " << i_options[l_i] << reset << std::endl;
        } else {
          std::cout << soft << "    " << i_options[l_i] << reset << std::endl;
        }
      }
    };

    l_render();
    while (true) {
      const Key l_key = readKey();
      if (l_key == Key::enter) {
        std::cout << "\033[" << i_options.size() << "A";
        for (std::size_t l_i = 0; l_i < i_options.size(); ++l_i) {
          std::cout << "\033[2K";
          if (l_i == l_selected) {
            std::cout << green << "  > " << i_options[l_i] << reset << std::endl;
          } else {
            std::cout << soft << "    " << i_options[l_i] << reset << std::endl;
          }
        }
        return i_options[l_selected];
      }
      if (l_key == Key::up) {
        l_selected = (l_selected == 0) ? i_options.size() - 1 : l_selected - 1;
      } else if (l_key == Key::down) {
        l_selected = (l_selected + 1) % i_options.size();
      } else {
        continue;
      }
      std::cout << "\033[" << i_options.size() << "A";
      l_render();
    }
  }

  std::vector<std::string> findNetCdfFiles(const std::string& i_root) {
    std::vector<std::string> l_files;
    if (!std::filesystem::exists(i_root)) return l_files;
    for (const auto& l_entry : std::filesystem::recursive_directory_iterator(i_root)) {
      if (!l_entry.is_regular_file()) continue;
      if (lower(l_entry.path().extension().string()) == ".nc") {
        l_files.push_back(l_entry.path().generic_string());
      }
    }
    std::sort(l_files.begin(), l_files.end());
    return l_files;
  }

  std::string promptNetCdfFile(const std::string& i_label,
                               const std::string& i_default,
                               const std::vector<std::string>& i_files) {
    if (i_files.empty()) {
      std::cout << soft << "No NetCDF files found. Keeping configured path." << reset << std::endl;
      return i_default;
    }
    std::size_t l_selected = 0;
    for (std::size_t l_i = 0; l_i < i_files.size(); ++l_i) {
      if (i_files[l_i] == i_default) {
        l_selected = l_i;
        break;
      }
    }

    std::cout << cyan << "? " << reset << i_label << " "
              << soft << "[" << i_default << "]" << reset
              << soft << "  use up/down, enter to confirm" << reset << std::endl;

    auto l_render = [&]() {
      for (std::size_t l_i = 0; l_i < i_files.size(); ++l_i) {
        std::cout << "\033[2K";
        if (l_i == l_selected) {
          std::cout << pink << "  > " << i_files[l_i] << reset << std::endl;
        } else {
          std::cout << soft << "    " << i_files[l_i] << reset << std::endl;
        }
      }
    };

    l_render();
    while (true) {
      const Key l_key = readKey();
      if (l_key == Key::enter) {
        std::cout << "\033[" << i_files.size() << "A";
        for (std::size_t l_i = 0; l_i < i_files.size(); ++l_i) {
          std::cout << "\033[2K";
          if (l_i == l_selected) {
            std::cout << green << "  > " << i_files[l_i] << reset << std::endl;
          } else {
            std::cout << soft << "    " << i_files[l_i] << reset << std::endl;
          }
        }
        return i_files[l_selected];
      }
      if (l_key == Key::up) {
        l_selected = (l_selected == 0) ? i_files.size() - 1 : l_selected - 1;
      } else if (l_key == Key::down) {
        l_selected = (l_selected + 1) % i_files.size();
      } else {
        continue;
      }
      std::cout << "\033[" << i_files.size() << "A";
      l_render();
    }
  }
}

//getting duration time in min, secs...
void printDuration(std::chrono::nanoseconds duration){
  auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
  duration -= hours;

  auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
  duration -= minutes;

  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
  duration -= seconds;

  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
  duration -= milliseconds;

  auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);
  duration -= microseconds;

  auto nanoseconds = duration;

  std::cout << cli::soft << "  Duration: " << cli::reset 
        << hours.count() << " hours, " 
        << minutes.count() << " minutes, "
        << seconds.count() << " seconds, "
        << milliseconds.count() << " milliseconds, "
        << microseconds.count() << " microseconds, "
        << nanoseconds.count() << " nanoseconds"
        << std::endl; 
}

int main() {
  tsunami_lab::t_idx l_nx = 1;
  tsunami_lab::t_idx l_ny = 1;
  tsunami_lab::t_real l_dxy = 0;

  cli::printLogo();

  const std::string filename = "tsunami_lab/configs/config.json";
  json l_config;
  try {
    l_config = cli::loadConfig(filename);
  } catch (const std::exception& l_error) {
    std::cout << cli::red << "Cannot start: " << l_error.what() << cli::reset << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<std::pair<std::string, std::vector<std::string>>> l_requiredKeys = {
    {"numerical solver", {"numerical_solver", "solver"}},
    {"scenario", {"scenario", "setup"}},
    {"wave model", {"wave_model", "wavepropagation"}},
    {"domain size x", {"domain_size_x", "dimension_x"}},
    {"domain size y", {"domain_size_y", "dimension_y"}},
    {"cells x", {"cells_x", "nx"}},
    {"cells y", {"cells_y", "ny"}},
    {"origin x", {"origin_x", "domain_start_x"}},
    {"origin y", {"origin_y", "domain_start_y"}},
    {"end time", {"simulation_end_time", "endtime"}},
    {"output format", {"output_format", "writer"}},
    {"bathymetry file", {"bathymetry_file", "bathfile"}},
    {"displacement file", {"displacement_file", "disfile"}},
    {"output name", {"output_name", "outputfilename"}},
    {"boundary mode", {"reflective_boundary", "reflecting_boundary"}}
  };

  std::vector<std::string> l_missingKeys;
  for (const auto& l_group : l_requiredKeys) {
    if (!cli::hasAny(l_config, l_group.second)) {
      l_missingKeys.push_back(l_group.first);
    }
  }
  if (!l_missingKeys.empty()) {
    std::cout << cli::red << "Config is missing these values:" << cli::reset << std::endl;
    for (const auto& l_key : l_missingKeys) std::cout << "  - " << l_key << std::endl;
    return EXIT_FAILURE;
  }

  ensureOutputDirectories();

  //Declaration---------------------------------------------------------------------------START


  tsunami_lab::setups::Setup *l_setup = nullptr;
  std::string l_temp_setup,l_temp_solver,l_temp_waveprop,l_temp_bathFile,l_temp_disFile,l_temp_writer;
  tsunami_lab::t_real l_domain_start_x = -1,l_domain_start_y = -1,l_temp_dimension_x = -1,l_temp_dimension_y = -1,l_frequency = -1,l_temp_endtime = -1;
  tsunami_lab::t_idx  l_timeStep = 0;
  tsunami_lab::t_real l_simTime = 0,l_dt = 0,l_last_simTime_time = 0;
  tsunami_lab::t_idx l_time_step_index = 0;
  bool reflecting_boundary;
  
  std::vector<tsunami_lab::Station> l_stations;

  l_nx = cli::getOr<tsunami_lab::t_idx>(l_config, {"cells_x", "nx"}, 2700);
  l_ny = cli::getOr<tsunami_lab::t_idx>(l_config, {"cells_y", "ny"}, 1500);
  l_temp_setup = cli::getOr<std::string>(l_config, {"scenario", "setup"}, "tsunamievent2d");
  l_temp_solver = cli::getOr<std::string>(l_config, {"numerical_solver", "solver"}, "fwave");
  l_temp_waveprop = cli::getOr<std::string>(l_config, {"wave_model", "wavepropagation"}, "2d");
  l_domain_start_x = cli::getOr<tsunami_lab::t_real>(l_config, {"origin_x", "domain_start_x"}, -200000);
  l_domain_start_y = cli::getOr<tsunami_lab::t_real>(l_config, {"origin_y", "domain_start_y"}, -750000);
  l_temp_dimension_x = cli::getOr<tsunami_lab::t_real>(l_config, {"domain_size_x", "dimension_x"}, 2700000);
  l_temp_dimension_y = cli::getOr<tsunami_lab::t_real>(l_config, {"domain_size_y", "dimension_y"}, 1500000);
  l_temp_endtime = cli::getOr<tsunami_lab::t_real>(l_config, {"simulation_end_time", "endtime"}, 36000);
  l_temp_writer = cli::getOr<std::string>(l_config, {"output_format", "writer"}, "csv");
  tsunami_lab::t_idx l_k = cli::getOr<tsunami_lab::t_idx>(l_config, {"coarse_factor", "k"}, 1);
  l_temp_bathFile = cli::getOr<std::string>(l_config, {"bathymetry_file", "bathfile"}, "");
  l_temp_disFile = cli::getOr<std::string>(l_config, {"displacement_file", "disfile"}, "");
  std::string l_temp_outputfilename = cli::getOr<std::string>(l_config, {"output_name", "outputfilename"}, "tsunami_output.csv");
  reflecting_boundary = cli::getOr<bool>(l_config, {"reflective_boundary", "reflecting_boundary"}, false);

  tsunami_lab::t_real l_temp_hr = cli::getOr<tsunami_lab::t_real>(l_config, {"right_height", "hr"}, 55);
  tsunami_lab::t_real l_temp_hl = cli::getOr<tsunami_lab::t_real>(l_config, {"left_height", "hl"}, 25);
  tsunami_lab::t_real l_temp_hu = cli::getOr<tsunami_lab::t_real>(l_config, {"initial_momentum_x", "hu"}, 0);
  tsunami_lab::t_real l_temp_location = cli::getOr<tsunami_lab::t_real>(l_config, {"dam_location", "location"}, 0);

  l_frequency = tsunami_lab::io::Configuration::getFrequencyFromJson();

  tsunami_lab::t_real * l_cp_h = nullptr;
  tsunami_lab::t_real * l_cp_hu = nullptr;
  tsunami_lab::t_real * l_cp_hv = nullptr;
  tsunami_lab::t_real * l_cp_b = nullptr;
  bool l_useCheckpoint = false;
  tsunami_lab::t_real l_simTimeLastCP = 0;

  if (std::filesystem::exists("outputs/checkpoints/checkpoint.nc")) {
    try {
      l_useCheckpoint = true;
      tsunami_lab::io::NetCdf::readCheckPoint("outputs/checkpoints/checkpoint.nc",
                                              &l_temp_solver,
                                              &l_temp_setup,
                                              &l_temp_waveprop,
                                              &l_temp_dimension_x,
                                              &l_temp_dimension_y,
                                              &l_nx,
                                              &l_ny,
                                              &l_domain_start_x,
                                              &l_domain_start_y,
                                              &l_temp_endtime,
                                              &l_temp_writer,
                                              &l_temp_bathFile,
                                              &l_temp_disFile,
                                              &l_temp_outputfilename,
                                              &reflecting_boundary,
                                              &l_simTimeLastCP,
                                              &l_cp_hu,
                                              &l_cp_hv,
                                              &l_cp_h,
                                              &l_temp_location,
                                              &l_cp_b);
      l_simTime = l_simTimeLastCP;
      std::cout << cli::green << "Resuming from checkpoint 'outputs/checkpoints/checkpoint.nc' at time: " << l_simTime << cli::reset << std::endl;
    }
    catch (const std::exception& l_error) {
      std::cout << cli::red << "Ignoring broken checkpoint: " << l_error.what() << cli::reset << std::endl;
      std::filesystem::remove("outputs/checkpoints/checkpoint.nc");
      l_useCheckpoint = false;
    }
  }

  if (!l_useCheckpoint) {
    cli::section("Launch settings");
    l_temp_solver = cli::promptChoice("Numerical solver", {"fwave", "roe"}, cli::lower(l_temp_solver));
    l_temp_waveprop = cli::promptChoice("Wave model", {"2d", "1d"}, cli::lower(l_temp_waveprop));
    if (l_temp_waveprop == "2d") {
      const std::vector<std::string> l_2dSetups = {"tsunamievent2d", "dambreak2d", "artificialtsunami2D"};
      if (std::find(l_2dSetups.begin(), l_2dSetups.end(), l_temp_setup) == l_2dSetups.end()) {
        l_temp_setup = "tsunamievent2d";
      }
      l_temp_setup = cli::promptChoice("Scenario", l_2dSetups, l_temp_setup);
    }
    else {
      const std::vector<std::string> l_1dSetups = {"dambreak1d", "tsunamievent1d", "shockshock", "rarerare", "subcriticalflow", "supercriticalflow"};
      if (std::find(l_1dSetups.begin(), l_1dSetups.end(), l_temp_setup) == l_1dSetups.end()) {
        l_temp_setup = "dambreak1d";
      }
      l_temp_setup = cli::promptChoice("Scenario", l_1dSetups, l_temp_setup);
    }
    l_temp_writer = cli::promptChoice("Output format", {"csv", "netcdf"}, cli::lower(l_temp_writer));

    cli::section("Domain");
    l_temp_dimension_x = cli::promptReal("Domain size x [m]", l_temp_dimension_x);
    l_temp_dimension_y = cli::promptReal("Domain size y [m]", l_temp_dimension_y);
    l_nx = cli::promptIndex("Cells x", l_nx);
    l_ny = cli::promptIndex("Cells y", l_ny);
    l_k = cli::promptIndex("Coarsening factor k", l_k);
    l_domain_start_x = cli::promptReal("Origin x [m]", l_domain_start_x);
    l_domain_start_y = cli::promptReal("Origin y [m]", l_domain_start_y);
    l_temp_endtime = cli::promptReal("Simulation end time [s]", l_temp_endtime);
    reflecting_boundary = cli::promptBool("Reflective boundary", reflecting_boundary);

    cli::section("Input data");
    const std::vector<std::string> l_ncFiles = cli::findNetCdfFiles("data/nc/data_in");
    if (l_temp_setup == "tsunamievent2d") {
      l_temp_bathFile = cli::promptNetCdfFile("Bathymetry NetCDF", l_temp_bathFile, l_ncFiles);
      l_temp_disFile = cli::promptNetCdfFile("Displacement NetCDF", l_temp_disFile, l_ncFiles);
    }
    l_temp_outputfilename = cli::promptString("Output base name", l_temp_outputfilename);
  }
  if (!l_useCheckpoint) {
    l_temp_outputfilename = generateNewName(l_temp_outputfilename,"outputs/");
  }
  if (l_temp_writer == "netcdf" && std::filesystem::path(l_temp_outputfilename).extension().empty()) {
    l_temp_outputfilename += ".nc";
  }
  std::string l_temp_outputfile =  "outputs/" + l_temp_outputfilename;

  const char * l_bathFile = l_temp_bathFile.c_str();
  const char * l_disFile = l_temp_disFile.c_str();
  const char * l_outputFile = l_temp_outputfile.c_str();
  

  tsunami_lab::io::Configuration::readStationsFromJson(l_stations);
  l_dxy = l_temp_dimension_x / l_nx;
  

  //Declaration---------------------------------------------------------------------------END
  //Reading the Solver from the Json file-------------------------------------------------START
   
  bool l_solver;
  if(l_temp_solver == "roe") {
    l_solver = true;
  }else{
    l_solver = false;
  }
  //Reading the Solver from the Json file-------------------------------------------------END
  //Determine which setup and which wavepropagation to use--------------------------------START
  tsunami_lab::patches::WavePropagation *l_waveProp = nullptr;
  if (l_useCheckpoint) {
    l_setup = new tsunami_lab::setups::CheckPoint(l_temp_dimension_x,
                                                  l_temp_dimension_y,
                                                  l_nx,
                                                  l_ny,
                                                  l_domain_start_x,
                                                  l_domain_start_y,
                                                  l_cp_h,
                                                  l_cp_hu,
                                                  l_cp_hv,
                                                  l_cp_b);
    l_waveProp = (l_temp_waveprop == "2d")
        ? static_cast<tsunami_lab::patches::WavePropagation*>(new tsunami_lab::patches::WavePropagation2d(l_nx, l_ny, l_solver, reflecting_boundary))
        : static_cast<tsunami_lab::patches::WavePropagation*>(new tsunami_lab::patches::WavePropagation1d(l_nx, l_solver, reflecting_boundary));
  }
  else if(l_temp_waveprop == "2d"){
    l_waveProp = new tsunami_lab::patches::WavePropagation2d( l_nx, l_ny, l_solver, reflecting_boundary);
    if(l_temp_setup == "artificialtsunami2D")
    {
      l_setup = new tsunami_lab::setups::ArtificialTsunami2d();
    }
    else if(l_temp_setup == "tsunamievent2d")
    {
      l_setup = new tsunami_lab::setups::TsunamiEvent2d(l_bathFile ,l_disFile);
    }
    else
    {
      l_setup = new tsunami_lab::setups::DamBreak2d();
    }
   
  }else if(l_temp_waveprop == "1d")
  {
      l_waveProp = new tsunami_lab::patches::WavePropagation1d( l_nx , l_solver, reflecting_boundary);
      if(l_temp_setup == "tsunamievent1d"){
        l_setup = new tsunami_lab::setups::TsunamiEvent1d();
      }else if(l_temp_setup == "dambreak1d"){
        l_setup = new tsunami_lab::setups::DamBreak1d(l_temp_hl ,l_temp_hr,l_temp_location); 
      }else if(l_temp_setup == "supercriticalflow"){
        l_setup = new tsunami_lab::setups::SupercriticalFlow();
      }
      else if(l_temp_setup == "subcriticalflow"){
        l_setup = new tsunami_lab::setups::SubcriticalFlow();
      }
      else if(l_temp_setup == "shockshock" || l_temp_setup =="rarerare" ){
        if(l_temp_setup == "shockshock" ){
          l_setup = new tsunami_lab::setups::ShockShock(l_temp_hl ,l_temp_hu,l_temp_location);  
        }else{
          l_setup = new tsunami_lab::setups::RareRare(l_temp_hl ,l_temp_hu,l_temp_location);  
        }
      }else if(l_temp_setup == "dambreak1d"){
        
        l_setup = new tsunami_lab::setups::DamBreak1d(l_temp_hl ,l_temp_hr,l_temp_location); 

      }else if(l_temp_setup == "dambreak2d"){

        l_setup = new tsunami_lab::setups::DamBreak2d();
      }
    l_ny = 1;
  }
    //Determine which setup and which wavepropagation to use--------------------------------END
  cli::section("Runtime matrix");
  std::cout << cli::soft << "  solver       " << cli::reset << l_temp_solver << std::endl;
  std::cout << cli::soft << "  scenario     " << cli::reset << l_temp_setup << std::endl;
  std::cout << cli::soft << "  model        " << cli::reset << l_temp_waveprop << std::endl;
  std::cout << cli::soft << "  output       " << cli::reset << l_temp_outputfile << std::endl;
  std::cout << cli::soft << "  cells        " << cli::reset << l_nx << " x " << l_ny << std::endl;
  std::cout << cli::soft << "  cell width   " << cli::reset << l_dxy << " m" << std::endl;

  //timer for whole computation including initialising the setup
  std::cout << cli::soft << "  starting the timer  " << cli::reset << std::endl;
  auto start = std::chrono::high_resolution_clock::now();

  // maximum observed height in the setup
  tsunami_lab::t_real l_hMax = std::numeric_limits< tsunami_lab::t_real >::lowest();
  // set up solver


  for( tsunami_lab::t_idx l_cy = 0; l_cy < l_ny; l_cy++ ){
    for( tsunami_lab::t_idx l_cx = 0; l_cx < l_nx; l_cx++ ){
      tsunami_lab::t_real l_y = l_cy * l_dxy + l_domain_start_y;
      tsunami_lab::t_real l_x = l_cx * l_dxy + l_domain_start_x;

      // get initial values of the setup
      tsunami_lab::t_real l_h = l_setup->getHeight( l_x,
                                                    l_y );
      l_hMax = std::max( l_h, l_hMax );

      tsunami_lab::t_real l_hu = l_setup->getMomentumX( l_x,
                                                        l_y );
      tsunami_lab::t_real l_hv = l_setup->getMomentumY( l_x,
                                                        l_y );
      tsunami_lab::t_real l_bv = l_setup->getBathymetry(l_x,
                                                        l_y );

      // set initial values in wave propagation solver
      l_waveProp->setHeight( l_cx,
                            l_cy,
                            l_h );
      l_waveProp->setMomentumX( l_cx,
                                l_cy,
                                l_hu );
      l_waveProp->setMomentumY( l_cx,
                                l_cy,
                                l_hv );
      l_waveProp->setBathymetry( l_cx,
                                l_cy,
                                l_bv);
    }
  }
  // derive maximum wave speed in setup; the momentum is ignored
  tsunami_lab::t_real l_speedMax = std::sqrt( 9.81 * l_hMax );
  l_dt = 0.50 * l_dxy / l_speedMax;
  if (l_useCheckpoint && l_dt > 0) {
    l_timeStep = static_cast<tsunami_lab::t_idx>(std::ceil(l_simTime / l_dt));
    l_time_step_index = static_cast<tsunami_lab::t_idx>(std::ceil(l_timeStep / 25.0));
  }
  
  // derive scaling for a time step
  tsunami_lab::t_real l_scaling = l_dt/l_dxy;
  
  std::cout << cli::soft << "  time step    " << cli::reset << l_dt << " seconds" << std::endl;

  tsunami_lab::t_real amount_time_steps = ceil(l_temp_endtime/l_dt);
  std::cout << cli::soft << "  steps        " << cli::reset << amount_time_steps << std::endl;

  // set up time and print control
  std::cout << "\n" << cli::green << "Starting simulation loop" << cli::reset << "\n" << std::endl;
  

  // Checking if the "y" of each Station is set 0, else delete it from the vector.
  if(l_temp_waveprop == "1d" && l_stations.size() != 0){
    l_stations.erase(
    std::remove_if(l_stations.begin(), l_stations.end(), [&](const auto& station) {
        if (station.i_y != 0) {
            std::cout << cli::red << "[skip] " << station.i_name << " needs y = 0 for 1d." << cli::reset << std::endl;
            return true;
        }
        return false; 
    }),
    l_stations.end());
  }
  //stations removing out of boundary-------------------------------------------------------------start
  //removing out of boundary stations
  std::string l_stations_string;
  if(l_temp_waveprop == "2d"){
    l_stations.erase(
    std::remove_if(l_stations.begin(), l_stations.end(), [&](const auto& station) {
    if (station.i_x < l_domain_start_x || station.i_x >= l_temp_dimension_x + l_domain_start_x ||
        station.i_y < l_domain_start_y || station.i_y >= l_temp_dimension_y + l_domain_start_y) {
        std::cout << cli::red << "[skip] " << station.i_name << " is out of boundary." << cli::reset << std::endl;
        std::string l_foldername = "stations/"+station.i_name;
        l_stations_string= l_stations_string + l_foldername +"/"+ station.i_name+".csv"+"$$"; 
        return true; // Remove the station
    }
    std::cout << cli::green << "[ok] " << station.i_name << " is in boundary." << cli::reset << std::endl;
    return false; // Keep the station
    }),
    l_stations.end());
  
  }else{
    l_stations.erase(
    std::remove_if(l_stations.begin(), l_stations.end(), [&](const auto& station) {
    if (station.i_x < l_domain_start_x || station.i_x >= l_temp_dimension_x + l_domain_start_x) {
        std::cout << cli::red << "[skip] " << station.i_name << " is out of boundary." << cli::reset << std::endl;
        return true; // Remove the station
    }
    std::cout << cli::green << "[ok] " << station.i_name << " is in boundary." << cli::reset << std::endl;
    return false; // Keep the station
    }),
    l_stations.end());
  }

  //stations ---------------------------------------------------------------------------------end
  tsunami_lab::t_real  l_current_frequency_time = l_frequency+l_last_simTime_time;

  //create the netCdf file reader/writer
  tsunami_lab::io::NetCdf* l_netCdf = nullptr ;

  if(l_temp_writer == "netcdf"){
    l_netCdf = new tsunami_lab::io::NetCdf(l_nx,l_ny,l_k,l_outputFile);
    l_netCdf->fillConstants(l_nx,
                            l_ny,
                            l_waveProp->getStride(),
                            l_dxy,
                            l_domain_start_x,
                            l_domain_start_y,
                            l_waveProp->getBathymetry(),
                            l_outputFile);
  }

  // timer for timestep loop
  auto cell_loop_start = std::chrono::high_resolution_clock::now();
  while( l_simTime < l_temp_endtime ){
    if( l_timeStep % 25 == 0 ) {

      if(l_temp_writer == "csv"){
        std::string l_path = "outputs/solution_" + std::to_string(l_time_step_index) + ".csv";
        std::ofstream l_file;
        l_file.open( l_path );
        tsunami_lab::io::Csv::write(l_dxy,
                                    l_nx,
                                    l_ny,
                                    l_waveProp->getStride(),
                                    l_domain_start_x,
                                    l_domain_start_y,
                                    l_temp_waveprop,
                                    l_waveProp->getHeight(),
                                    l_waveProp->getMomentumX(),
                                    l_waveProp->getMomentumY(),
                                    l_waveProp->getBathymetry(),
                                    l_file);

        l_file.close();
      }else{
        l_netCdf->updateFile( l_nx,
                              l_ny,
                              l_waveProp->getStride(),
                              l_time_step_index,
                              l_simTime,
                              l_waveProp->getHeight(),
                              l_waveProp->getMomentumX(),
                              l_waveProp->getMomentumY(),
                              l_outputFile);
      }

      tsunami_lab::io::NetCdf* cp_netCdf = (l_netCdf != nullptr) ? l_netCdf : new tsunami_lab::io::NetCdf(l_nx, l_ny, l_k, "outputs/temp.nc");
      cp_netCdf->createCheckPoint(l_temp_solver, l_temp_setup, l_temp_waveprop, l_temp_dimension_x, l_temp_dimension_y, l_nx, l_ny, l_domain_start_x, l_domain_start_y, l_temp_endtime, l_temp_writer, l_temp_bathFile, l_temp_disFile, l_temp_outputfilename, reflecting_boundary, l_simTime, l_waveProp->getMomentumX(), l_waveProp->getMomentumY(), l_waveProp->getHeight(), l_temp_location, l_waveProp->getBathymetry(), l_waveProp->getStride(), l_dxy, "checkpoint.nc");
      if (l_netCdf == nullptr) {
        delete cp_netCdf;
        std::filesystem::remove("outputs/temp.nc");
      }

      l_time_step_index++;
    }
    
    //STATIONS_---------------------------------------------START 
    if(l_current_frequency_time <= l_simTime){
      for (const auto& station : l_stations) {
        std::string l_foldername = "stations/"+station.i_name;
        if (!std::filesystem::exists(l_foldername)){
          std::filesystem::create_directory(l_foldername);
        }
        //compute cell ID
        tsunami_lab::t_idx l_ix = ((station.i_x - l_domain_start_x ) / l_dxy )+ l_waveProp->getGhostcellX();
        tsunami_lab::t_idx l_iy = ((station.i_y - l_domain_start_y ) / l_dxy )+ l_waveProp->getGhostcellY();
        if(l_temp_waveprop == "1d"){
          l_iy = 0; 
        }
        tsunami_lab::t_idx l_id = l_iy * l_waveProp->getStride() + l_ix;
        const tsunami_lab::t_real* l_water_height =  l_waveProp->getHeight();
        const tsunami_lab::t_real* l_water_hu =  l_waveProp->getMomentumX();
        const tsunami_lab::t_real* l_water_hv =  l_waveProp->getMomentumY();
        std::string l_station_path = l_foldername +"/"+ station.i_name+".csv";
        if(l_temp_waveprop == "2d"){
        tsunami_lab::io::Station::write(l_ix,
                                        l_iy,
                                        l_simTime,
                                        l_water_height[l_id],
                                        l_water_hu[l_id],
                                        l_water_hv[l_id],
                                        l_station_path,
                                        l_temp_waveprop);
        }else{
          tsunami_lab::io::Station::write(l_ix,
                                        l_iy,
                                        l_simTime,
                                        l_water_height[l_id],
                                        l_water_hu[l_id],
                                        -1,
                                        l_station_path,
                                        l_temp_waveprop);
        }
      }
      l_last_simTime_time = l_simTime;
      l_current_frequency_time = l_current_frequency_time + l_frequency;
    }
    //STATIONS----------------------------------------------END

    l_waveProp->timeStep( l_scaling);
    l_timeStep++;
    l_simTime += l_dt;
    updateProgressBar(l_simTime, l_temp_endtime,50);
  }

  //get duration of both timers
  auto cell_loop_end = std::chrono::high_resolution_clock::now();
  auto end = std::chrono::high_resolution_clock::now();

  auto cell_loop_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(cell_loop_end - cell_loop_start);
  std::cout << "\n\n" << cli::green << "Duration of time step loop: " << cli::reset << std::endl;
  printDuration(cell_loop_duration);

  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  std::cout << "\n\n" << cli::green << "Duration of programm: " << cli::reset << std::endl;
  printDuration(duration);
  
  std::cout << "\n\n" << cli::green << "Simulation complete. Outputs are in 'outputs'." << cli::reset << std::endl;

  // free memory
  std::cout << cli::soft << "Releasing memory..." << cli::reset << std::endl;
  delete l_setup;
  delete l_waveProp;
  delete l_netCdf;
  std::cout << cli::pink << "Goodbye from TSUNAMI." << cli::reset << std::endl;
  return EXIT_SUCCESS;
}
