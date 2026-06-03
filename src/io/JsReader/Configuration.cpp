#include "Configuration.h"
#include <utility>

namespace {
    std::string resolveConfigKey(const json& i_data, const std::string& i_key) {
        if (i_data.contains(i_key)) return i_key;

        const std::vector<std::pair<std::string, std::string>> l_aliases = {
            {"solver", "numerical_solver"},
            {"setup", "scenario"},
            {"wavepropagation", "wave_model"},
            {"dimension_x", "domain_size_x"},
            {"dimension_y", "domain_size_y"},
            {"nx", "cells_x"},
            {"ny", "cells_y"},
            {"k", "coarse_factor"},
            {"domain_start_x", "origin_x"},
            {"domain_start_y", "origin_y"},
            {"endtime", "simulation_end_time"},
            {"writer", "output_format"},
            {"bathfile", "bathymetry_file"},
            {"disfile", "displacement_file"},
            {"outputfilename", "output_name"},
            {"reflecting_boundary", "reflective_boundary"},
            {"hu", "initial_momentum_x"},
            {"hv", "initial_momentum_y"},
            {"hl", "left_height"},
            {"hr", "right_height"},
            {"location", "dam_location"}
        };

        for (const auto& l_alias : l_aliases) {
            if (i_key == l_alias.first && i_data.contains(l_alias.second)) return l_alias.second;
            if (i_key == l_alias.second && i_data.contains(l_alias.first)) return l_alias.first;
        }

        return i_key;
    }
}

std::string tsunami_lab::io::Configuration::readFromConfigString(std::string i_configVariable){
    
    std::ifstream f("configs/config.json");
    json data = json::parse(f); 
    return data[resolveConfigKey(data, i_configVariable)];
}

tsunami_lab::t_real tsunami_lab::io::Configuration::readFromConfigReal(std::string i_configVariable){
    
    std::ifstream f("configs/config.json");
    json data = json::parse(f); 
    return data[resolveConfigKey(data, i_configVariable)];
}

tsunami_lab::t_idx tsunami_lab::io::Configuration::readFromConfigIndex(std::string i_configVariable){
    
    std::ifstream f("configs/config.json");
    json data = json::parse(f); 
    return data[resolveConfigKey(data, i_configVariable)];
}

bool tsunami_lab::io::Configuration::readFromConfigBoolean(std::string  i_configVariable){
    try {
        std::ifstream f("configs/config.json");
        json data = json::parse(f); 
        return data[resolveConfigKey(data, i_configVariable)];
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << '\n';
        return false;
    }
}

std::vector<std::string> tsunami_lab::io::Configuration::checkMissingKeys(std::vector<std::string> i_keys) {
    std::vector<std::string> missingKeys;
    std::string filePath = "configs/config.json";
    // Read JSON data from the file
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return missingKeys;
    }

    json jsonData;
    try {
        fileStream >> jsonData;
    } catch (const json::parse_error& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        fileStream.close();
        return missingKeys;
    }

    fileStream.close();

    // Check for missing keys
    for (const auto& key : i_keys) {
        if (!jsonData.contains(resolveConfigKey(jsonData, key))) {
            missingKeys.push_back(key);
        }
    }

    return missingKeys;
}

void tsunami_lab::io::Configuration::readStationsFromJson(std::vector<tsunami_lab::Station> & stations) {
    std::string filename = "configs/stations.json";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    json json_data;
    file >> json_data;
    file.close();

    stations.clear(); 

    for (const auto& station_data : json_data["stations"]) {
        tsunami_lab::Station station;
        station.i_name = station_data["i_name"];
        station.i_x = station_data["i_x"];
        station.i_y = station_data["i_y"];
        stations.push_back(station);
    }
}


void tsunami_lab::io::Configuration::readStationsFromString(const std::string& json_str, std::vector<tsunami_lab::Station>& stations) {
    try {
        json json_data = json::parse(json_str);

        stations.clear();

        for (const auto& station_data : json_data["stations"]) {
            tsunami_lab::Station station;
            station.i_name = station_data["i_name"];
            station.i_x = station_data["i_x"];
            station.i_y = station_data["i_y"];
            stations.push_back(station);
        }
    } catch (const json::parse_error& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
}

tsunami_lab::t_real tsunami_lab::io::Configuration::getFrequencyFromJson(){
    std::string filename = "configs/stations.json";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 0;
    }
    json json_data;
    file >> json_data;
    file.close();
    return json_data["frequency"];
}
