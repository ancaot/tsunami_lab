/**
 * Simple implementation of Stations manager.
 **/
#include "Stations.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <cmath>


tsunami_lab::io::Stations::Stations( t_real i_outputIntervalSeconds ) {
  if( i_outputIntervalSeconds <= 0 ) i_outputIntervalSeconds = 1.0;
  m_outputInterval = i_outputIntervalSeconds;
  m_nextOutputTime = 0.0;
}

void tsunami_lab::io::Stations::setOutputInterval( t_real i_outputIntervalSeconds ) {
  if( i_outputIntervalSeconds <= 0 ) return;
  m_outputInterval = i_outputIntervalSeconds;
}

void tsunami_lab::io::Stations::setDomainStart( t_real i_x, t_real i_y ) {
  m_domainStartX = i_x;
  m_domainStartY = i_y;
}

bool tsunami_lab::io::Stations::loadFromCsv( std::string const & i_path ) {
  m_stations.clear();

  std::ifstream l_file( i_path.c_str() );
  if( !l_file.is_open() ) return false;

  std::string l_line;
  while( std::getline( l_file, l_line ) ) {
    if( l_line.empty() ) continue;
    // skip comments starting with '#'
    if( l_line[0] == '#' ) continue;

    std::stringstream l_ss( l_line );
    std::string l_name;
    std::string l_xs;
    std::string l_ys;
    if( !std::getline( l_ss, l_name, ',' ) ) continue;
    if( !std::getline( l_ss, l_xs, ',' ) ) continue;
    if( !std::getline( l_ss, l_ys, ',' ) ) continue;

    try {
      t_real l_x = static_cast< t_real >( std::stod( l_xs ) );
      t_real l_y = static_cast< t_real >( std::stod( l_ys ) );

      Station l_s;
      l_s.name = l_name;
      l_s.x = l_x;
      l_s.y = l_y;
      m_stations.push_back( std::move( l_s ) );
    }
    catch(...) {
      continue;
    }
  }

  return !m_stations.empty();
}

bool tsunami_lab::io::Stations::loadFromXml( std::string const & i_path ) {
  m_stations.clear();

  #ifdef USE_PUGIXML
  pugi::xml_document l_doc;
  pugi::xml_parse_result l_result = l_doc.load_file( i_path.c_str() );
  if( !l_result ) return false;

  pugi::xml_node l_root = l_doc.child( "stations" );
  if( !l_root ) return false;

  pugi::xml_attribute l_dtAttr = l_root.attribute( "output_interval" );
  if( l_dtAttr ) {
    t_real l_dt = static_cast< t_real >( l_dtAttr.as_float( m_outputInterval ) );
    setOutputInterval( l_dt );
  }

  for( pugi::xml_node l_station = l_root.child( "station" ); l_station; l_station = l_station.next_sibling( "station" ) ) {
    pugi::xml_attribute l_name = l_station.attribute( "name" );
    pugi::xml_attribute l_x = l_station.attribute( "x" );
    pugi::xml_attribute l_y = l_station.attribute( "y" );

    if( !l_name || !l_x || !l_y ) continue;

    try {
      Station l_s;
      l_s.name = l_name.as_string();
      l_s.x = static_cast< t_real >( l_x.as_float() );
      l_s.y = static_cast< t_real >( l_y.as_float() );
      m_stations.push_back( std::move( l_s ) );
    }
    catch(...) {}
  }

  #else
  std::ifstream l_file( i_path.c_str() );
  if( !l_file.is_open() ) return false;

  std::string l_line;
  while( std::getline( l_file, l_line ) ) {
    if( l_line.empty() ) continue;
    if( l_line.find( "output_interval" ) != std::string::npos ) {
      std::size_t l_pos = l_line.find( "output_interval" );
      std::size_t l_q1 = l_line.find( '"', l_pos );
      if( l_q1 != std::string::npos ) {
        std::size_t l_q2 = l_line.find( '"', l_q1 + 1 );
        if( l_q2 != std::string::npos ) {
          try {
            setOutputInterval( static_cast< t_real >( std::stod( l_line.substr( l_q1 + 1, l_q2 - l_q1 - 1 ) ) ) );
          }
          catch(...) {}
        }
      }
    }
    if( l_line.find( "<station" ) != std::string::npos ) {
      auto l_get = [&]( std::string const & i_key ) -> std::string {
        std::size_t l_pos = l_line.find( i_key );
        if( l_pos == std::string::npos ) return "";
        std::size_t l_q1 = l_line.find( '"', l_pos );
        if( l_q1 == std::string::npos ) return "";
        std::size_t l_q2 = l_line.find( '"', l_q1 + 1 );
        if( l_q2 == std::string::npos ) return "";
        return l_line.substr( l_q1 + 1, l_q2 - l_q1 - 1 );
      };

      std::string l_name = l_get( "name" );
      std::string l_x = l_get( "x" );
      std::string l_y = l_get( "y" );
      if( l_name.empty() || l_x.empty() || l_y.empty() ) continue;

      try {
        Station l_s;
        l_s.name = l_name;
        l_s.x = static_cast< t_real >( std::stod( l_x ) );
        l_s.y = static_cast< t_real >( std::stod( l_y ) );
        m_stations.push_back( std::move( l_s ) );
      }
      catch(...) {}
    }
  }
  #endif

  return !m_stations.empty();
}

bool tsunami_lab::io::Stations::loadFromFile( std::string const & i_path ) {
  std::string l_lower = i_path;
  std::transform( l_lower.begin(), l_lower.end(), l_lower.begin(), []( unsigned char c ){ return static_cast<char>( std::tolower(c) ); } );

  if( l_lower.size() >= 4 && l_lower.substr( l_lower.size() - 4 ) == ".xml" ) return loadFromXml( i_path );
  return loadFromCsv( i_path );
}

void tsunami_lab::io::Stations::openFiles( std::string const & i_prefix ) {
  for( auto & l_s : m_stations ) {
    std::string l_name = sanitizeName( l_s.name );
    std::string l_folder = i_prefix + "/" + l_name;
    std::error_code l_ec;
    std::filesystem::create_directories( l_folder, l_ec );
    std::string l_path = l_folder + "/" + l_name + ".csv";
    l_s.stream.reset( new std::ofstream( l_path.c_str(), std::ios::out ) );
    if( l_s.stream && l_s.stream->is_open() ) {
      *l_s.stream << "time,height\n" << std::flush;
      l_s.fileOpened = true;
    }
    else {
      l_s.fileOpened = false;
    }
  }
}

void tsunami_lab::io::Stations::sampleAndMaybeWrite( tsunami_lab::patches::WavePropagation * i_waveProp,
                                                    t_real i_simTime,
                                                    t_real i_dxy,
                                                    t_idx  i_nx,
                                                    t_idx  i_ny ) {
  if( m_stations.empty() ) return;

  // if simulation time has reached or passed next output time, write and
  // advance next output time (support multiple intervals if dt large).
  if( i_simTime + 1e-12 < m_nextOutputTime ) return;

  while( i_simTime + 1e-12 >= m_nextOutputTime ) {
    // write one sample at m_nextOutputTime
    for( auto const & l_s : m_stations ) {
      if( !l_s.fileOpened || !l_s.stream ) continue;

      // find nearest cell index
      t_idx l_ix = 0;
      t_idx l_iy = 0;
      Station const * l_found = findNearestStationCell( l_s, i_dxy, i_nx, i_ny, l_ix, l_iy );
      if( l_found == nullptr ) continue;

      // select effective stride: if solver stride equals logical nx, use it,
      // otherwise assume getHeight() returns contiguous interior of size i_nx
      t_idx l_solverStride = i_waveProp->getStride();
      t_idx l_effectiveStride = ( l_solverStride == i_nx ? l_solverStride : i_nx );
      t_idx l_id = l_iy * l_effectiveStride + l_ix;

      t_real const * l_h = i_waveProp->getHeight();
      if( l_h == nullptr ) continue;

      // guard against out-of-range access
      // assume height array length >= i_nx * i_ny
      t_idx l_maxIndex = static_cast<t_idx>( i_nx * i_ny );
      if( l_id >= l_maxIndex ) continue;

      t_real l_val = l_h[l_id];
      *l_s.stream << std::fixed << std::setprecision(6) << m_nextOutputTime << "," << l_val << "\n";
      l_s.stream->flush();
    }

    m_nextOutputTime += m_outputInterval;
  }
}

std::string tsunami_lab::io::Stations::sanitizeName( std::string const & i_name ) {
  std::string l_out;
  for( char c : i_name ) {
    if( std::isalnum( static_cast<unsigned char>(c) ) ) l_out.push_back( c );
    else if( std::isspace( static_cast<unsigned char>(c) ) ) l_out.push_back( '_' );
    else l_out.push_back( '_' );
  }
  if( l_out.empty() ) l_out = "station";
  return l_out;
}

tsunami_lab::io::Stations::Station const * tsunami_lab::io::Stations::findNearestStationCell( Station const & i_station,
                                                                                             t_real i_dxy,
                                                                                             t_idx  i_nx,
                                                                                             t_idx  i_ny,
                                                                                             t_idx & o_ix,
                                                                                             t_idx & o_iy ) const {
  if( i_dxy <= 0 ) return nullptr;

  // compute coordinates relative to domain start
  t_real l_relX = i_station.x - m_domainStartX;
  t_real l_relY = i_station.y - m_domainStartY;

  // round to nearest cell center (cells centered at (i+0.5)*dxy)
  long l_ix_long = static_cast<long>( std::floor( l_relX / i_dxy + 0.5 ) );
  long l_iy_long = static_cast<long>( std::floor( l_relY / i_dxy + 0.5 ) );

  if( l_ix_long < 0 || l_iy_long < 0 ) return nullptr;
  if( static_cast<t_idx>(l_ix_long) >= i_nx || static_cast<t_idx>(l_iy_long) >= i_ny ) return nullptr;

  o_ix = static_cast<t_idx>( l_ix_long );
  o_iy = static_cast<t_idx>( l_iy_long );
  return &i_station;
}
