#ifndef _NMEA_
#define _NMEA_

#include <stdint.h>
#include <Arduino.h>
#include "conversion.h"

namespace RCM_ELEMENTS { // Defines the field placements locations in an NMEA sentence
    const uint8_t UTC = 2;
    const uint8_t STATUS = 3;
    const uint8_t LAT = 4;
    const uint8_t LAT_DIR = 5;
    const uint8_t LON = 6;
    const uint8_t LON_DIR = 7;
    const uint8_t SOG = 8;
    const uint8_t COG = 9;
    const uint8_t DATE = 10;
    const uint8_t MAG_VAR = 11;
    const uint8_t MAG_VAR_DIR = 12;
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
    // Course Over Ground field
    float cog;
    // Date field
    int date;
    uint8_t day;
    uint8_t month;
    uint8_t year;
    // Magnetic Variation field
    float mag_var;
    // Direction of Magnetic Variation
    char mag_var_dir;
  } *rmc;

  NMEA(); // Empty constructor
  void begin(HardwareSerial *SerialPort, int baud); 
  void feedthrough(void);
  void update(void);
  
 private:
  HardwareSerial* port;
  char buf;
  char nmea[100];
  char crc[3];
  int CRC;
  int CRCin;
  byte bytes_read;

  void decode_RMC(char * tok, uint8_t n_fields, bool is_empty[]);
  int checksum(char * s);

  struct RMC_STRINGS {
    char * utc_c;
    char * status_c;
    char * lat_c;
    char * lat_dir_c;
    char * lon_c;
    char * lon_dir_c;
    char * cog_c;
    char * sog_c;
    char * date_c;
    char * mag_var_c;
    char * mag_var_dir_c;
  } *rmc_str;
};



#endif
