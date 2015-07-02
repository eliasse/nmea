#ifndef _NMEA_
#define _NMEA_

#include <stdint.h>
#include <Arduino.h>
#include "conversion.h"

namespace RCM_ELEMENTS { // Defines the field locations in an NMEA sentence
    const uint8_t UTC = 2;
    const uint8_t STATUS = 3;
    const uint8_t LAT = 4;
    const uint8_t LAT_DIR = 5;
    const uint8_t LON = 6;
    const uint8_t LON_DIR = 7;
    const uint8_t SOG = 8;
    const uint8_t TC = 9;
    const uint8_t DATE = 10;
    const uint8_t MAG_VAR = 11;
    const uint8_t MAG_VAR_DIR = 12;
};

namespace GLL_ELEMENTS {
  const uint8_t LAT = 2;
  const uint8_t LAT_DIR = 3;
  const uint8_t LON = 4;
  const uint8_t LON_DIR = 5;
  const uint8_t UTC = 6;
  const uint8_t VALIDITY = 7;
};

namespace GGA_ELEMENTS {
  const uint8_t UTC = 2;
  const uint8_t LAT = 3;
  const uint8_t LAT_DIR = 4;
  const uint8_t LON = 5;
  const uint8_t LON_DIR = 6;  
  const uint8_t FIX_QUALITY = 7;
  const uint8_t NUMBER_OF_SATELLITES = 8;
  const uint8_t HDOP = 9;
  const uint8_t ALTITUDE = 10;
  const uint8_t ALT_REF = 11;
  const uint8_t HEIGHT_OF_GEOID = 12;
  const uint8_t HEIGHT_REF = 13;
  const uint8_t TIME_SINCE_LAST_DGPS = 14;
  const uint8_t DGPS_STATION_ID_NUMBER = 15;
};

namespace VTG_ELEMENTS {
  const uint8_t TRACK_TRUE = 2;
  const uint8_t TRACK_MAGNETIC = 4;
  const uint8_t SOG_KNOTS = 6;
  const uint8_t SOG_KPH = 8;
};
 
class NMEA {
 public:

  struct RMC {  // Decoded RMC Contents //
    // UTC Field
    int utc;
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    // Status field
    char status;
    // Latitude field
    float lat_, lat_min_, lat_deg_; // Help variables
    float lat_deg, lat_rad;
    // Latitude direction 
    char lat_dir; 
    // Longitude field
    float lon_, lon_min_, lon_deg_; // Help variables	
    float lon_deg, lon_rad; 
    // Longitude direction
    char lon_dir;   
    // Speed Over Ground field 
    float sog_knots;
    float sog_ms;
    // True Course field
    float tc;
    // Date field
    int date;
    uint8_t day;
    uint8_t month;
    uint8_t year;
    // Magnetic Variation field
    float mag_var;
    // Direction of Magnetic Variation
    char mag_var_dir;
  } *rmc = 0;

  struct GLL {
    // Latitude
    float lat_, lat_min_, lat_deg_; // Help vars
    float lat_deg, lat_rad; 
    char lat_dir;
    // Longitude
    float lon_, lon_min_, lon_deg_; // Help vars
    float lon_deg, lon_rad;
    char lon_dir;
    // Fix Time
    int utc;
    uint8_t hour, minute, second;
    // Fix validity
    bool valid;
  } *gll = 0;

  struct GGA {
    // Fix time
    int utc;
    uint8_t hour, minute, second;
    // Latitude
    float lat_,lat_min_, lat_deg_; // Help vars
    float lat_deg, lat_rad;
    char lat_dir;
    // Longitude
    float lon_, lon_min_, lon_deg_; // Help vars
    float lon_deg, lon_rad;
    char lon_dir;
    // Fix Quality
    /* 0 = Invalid, 1 = GPS fix (SPS), 2 = DGPS fix, 3 = PPS fix,
       4 = Real Time Kinematic, 5 = Float RTK, 6 = Estimates (dead reckoning),
       7 = Manual Inpute Mode, 8 = Simulation Mode */
    uint8_t fix_quality;
    // Number of satellites in view
    uint8_t n_sats;
    // Horizontal Dilution of Precision (HDOP)
    float hdop;
    // Altitude and reference level (Always above mean sea level?) 
    float alt;
    char alt_ref; 
    // Height of geoid (mean sea level) above WGS84 ellipsoid
    float hei;
    char hei_ref;
    // Time in seconds since last DGPS update
    int time_since_dgps;
    // DGPS station id number
    uint16_t dgps_station_id;
  } *gga = 0;

  struct VTG {
    // Track in degrees relative to true north
    float true_track_deg, true_track_rad;
    // Track in degrees relative to magnetic north
    float magnetic_track_deg, magnetic_track_rad;
    // Speed in knots
    float sog_knots;
    // Speed in kilometers per hour
    float sog_kph; 
  } *vtg = 0;

  NMEA(); // Empty constructor
  void begin(HardwareSerial *SerialPort, int baud); // Initialize serial comm
  void feedthrough(void); // Just read incoming data and print without decoding
  void update(void);      // Read and decode incoming sentences 
  
 private:
  HardwareSerial* port;
  // Variables accessed by the update() function
  char buf;          // Single byte buffer
  char nmea[100];    // Buffer for current sentence
  char crc[3];       // CRC extracted from end of NMEA sentence
  int CRC;           // Calculated by checksum()
  int CRCin;         // crc[3] converted into int
  byte bytes_read;   // Number of bytes in the current NMEA sentence

  // Calculates and return a checksum from the received sentence 
  int checksum(char * s);

  // Functions that interprets respective NMEA sentences
  void decode_RMC(char * tok, uint8_t n_fields, bool is_empty[]);
  void decode_GLL(char * tok, uint8_t n_fields, bool is_empty[]);
  void decode_GGA(char * tok, uint8_t n_fields, bool is_empty[]);
  void decode_VTG(char * tok, uint8_t n_fields, bool is_empty[]);

  struct RMC_STRINGS {
    char * utc_c;
    char * status_c;
    char * lat_c;
    char * lat_dir_c;
    char * lon_c;
    char * lon_dir_c;
    char * tc_c;
    char * sog_c;
    char * date_c;
    char * mag_var_c;
    char * mag_var_dir_c;
  } *rmc_str = 0; // Initialize pointer to null (memory not yet allocated)

  struct GLL_STRINGS {
    char * lat;
    char * lat_dir;
    char * lon;
    char * lon_dir;
    char * utc;
    char * validity;
  } *gll_str = 0; // Initialize pointer to null (memory not yet allocated)
  
  struct GGA_STRINGS {
    char * utc;
    char * lat;
    char * lat_dir;
    char * lon;
    char * lon_dir;
    char * fix_quality;
    char * n_sats;
    char * hdop;
    char * alt;
    char * alt_ref;
    char * hei;
    char * hei_ref;
    char * time_since_dgps;
    char * dgps_station_id;
  } *gga_str = 0; 

  struct VTG_STRINGS {
    char * true_track;
    char * magnetic_track;
    char * sog_knots;
    char * sog_kph; 
  } *vtg_str = 0;
};



#endif
