#include "Station.h"

void tsunami_lab::io::Station::write(tsunami_lab::t_idx              i_x,
                                     tsunami_lab::t_idx              i_y,
                                     tsunami_lab::t_real             i_time_in_seconds,
                                     tsunami_lab::t_real             i_water_height,
                                     tsunami_lab::t_real             i_water_hu,
                                     tsunami_lab::t_real             i_water_hv,
                                     std::string                     i_csv_path,
                                     std::string i_dim){

    std::ofstream io_stream(i_csv_path,std::ios::app);
  
    if (!io_stream.is_open()) {
        std::cerr << "Error opening file: " << i_csv_path << std::endl;
        return ; 
    }
  
    std::uintmax_t fileSize = std::filesystem::file_size(i_csv_path);
    if(fileSize == 0){
        
        io_stream << "x,y,water_height,water_hu";
        if(i_dim == "2d"){
            io_stream <<",water_hv"; 
        }
        io_stream <<",time_in_seconds";
    }
    io_stream << "\n";
    io_stream << i_x << "," << i_y << "," << i_water_height << "," << i_water_hu; 
    if(i_dim == "2d"){
        io_stream << "," << i_water_hv;
    }
    io_stream << ","<< i_time_in_seconds;
    io_stream << std::flush;
}
