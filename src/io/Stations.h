/**
 * @author
 *
 * @section DESCRIPTION
 * Summarizes a collection of user-defined stations. Each station has a name
 * and a location (x,y). All stations share a common output interval in
 * seconds. The output for each station is written to a separate ASCII-CSV
 * file with two columns: time, height.
 *
 * This implementation supports a lightweight CSV-based station
 * configuration file with lines of the form: name,x,y.
 * XML loading uses pugixml when USE_PUGIXML is enabled at compile time.
 **/
#ifndef TSUNAMI_LAB_IO_STATIONS
#define TSUNAMI_LAB_IO_STATIONS

#include "../constants.h"
#include "../patches/WavePropagation.h"

#ifdef USE_PUGIXML
#include <pugixml.hpp>
#endif

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <cctype>

namespace tsunami_lab {
  namespace io {
    class Stations;
  }
}

class tsunami_lab::io::Stations {
  public:
    struct Station {
      std::string name;
      t_real x = 0;
      t_real y = 0;
      std::unique_ptr<std::ofstream> stream;
      bool fileOpened = false;
    };

    /**
     * Construct stations manager with given output interval in seconds.
     **/
    Stations( t_real i_outputIntervalSeconds = 1.0 );

    /**
     * Sets the domain start (origin) in meters. Stations coordinates are
     * interpreted relative to this origin. Default is (0,0).
     **/
    void setDomainStart( t_real i_x, t_real i_y );

    /**
     * Load station definitions from a CSV file with lines `name,x,y`.
     * Returns true on success.
     **/
    bool loadFromCsv( std::string const & i_path );

    /**
     * Load station definitions from a simple XML file.
     * Expected schema:
     * <stations output_interval="10">
     *   <station name="s0" x="100" y="0" />
     * </stations>
     * Returns true on success.
     **/
    bool loadFromXml( std::string const & i_path );

    /**
     * Loads stations from CSV or XML depending on file extension.
     **/
    bool loadFromFile( std::string const & i_path );

    /**
     * Override output interval in seconds.
     **/
    void setOutputInterval( t_real i_outputIntervalSeconds );

    /**
     * Open per-station output files. Optional prefix will be prepended
     * to generated file names: prefix_name.csv
     **/
    void openFiles( std::string const & i_prefix = "station" );

    /**
     * Sample the given solver at current simulation time. If the next
     * output time has been reached, write the station values (time,height)
     * into the respective CSV files.
     **/
    void sampleAndMaybeWrite( tsunami_lab::patches::WavePropagation * i_waveProp,
                              t_real i_simTime,
                              t_real i_dxy,
                              t_idx  i_nx,
                              t_idx  i_ny );

  private:
    std::vector< Station > m_stations;
    t_real m_outputInterval = 1.0;
    t_real m_nextOutputTime = 0.0;

    static std::string sanitizeName( std::string const & i_name );
    Station const * findNearestStationCell( Station const & i_station,
                         t_real i_dxy,
                         t_idx  i_nx,
                         t_idx  i_ny,
                         t_idx & o_ix,
                         t_idx & o_iy ) const;

    // domain origin
    t_real m_domainStartX = 0.0;
    t_real m_domainStartY = 0.0;
};

#endif
