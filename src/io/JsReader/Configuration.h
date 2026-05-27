#ifndef TSUNAMI_LAB_IO_CONFIGURATION
#define TSUNAMI_LAB_IO_CONFIGURATION

#include "../../constants.h"
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <vector>

using json = nlohmann::json;

namespace tsunami_lab {
  namespace io {
    class Configuration;
  }
}

class tsunami_lab::io::Configuration {

    public:
        /**
         * @param i_configVariable value of the argument you are looking for in the json file.
         * @return String value
         */
        static std::string readFromConfigString(std::string i_configVariable);
        /**
         * @param i_configVariable value of the argument you are looking for in the json file.
         * @return t_real value
         */
        static tsunami_lab::t_real readFromConfigReal(std::string i_configVariable);
        /**
         * @param i_configVariable value of the argument you are looking for in the json file.
         * @return boolean value
         */
        static bool readFromConfigBoolean(std::string i_configVariable);
        /**
         * @param i_configVariable value of the argument you are looking for in the json file.
         * @return t_idx value
         */
        static t_idx readFromConfigIndex(std::string i_configVariable);
         /**
         * @brief checks whether there are arguments that are missing for the simulation.
         * @param i_keys a vector that contains string values.
         * @return vector<std::string> of the missing keys (should be empty if everything should work). 
         */
        static std::vector<std::string> checkMissingKeys(std::vector<std::string> i_keys);
        /**
         * @param stations an empty vector which is filled with stations from the json file.
         */
        static void readStationsFromJson(std::vector<tsunami_lab::Station> & stations);


        static void readStationsFromString(const std::string& json_str, std::vector<tsunami_lab::Station>& stations);
        /**
         * @return frequency value tells us after how many secons the next time step is.
         */
        static tsunami_lab::t_real getFrequencyFromJson();
};
#endif