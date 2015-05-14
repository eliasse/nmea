#include "NMEA.h"

NMEA::NMEA(){
  rmc_str = new(RMC_STRINGS);
  rmc = new(RMC);
}

int NMEA::checksum(char * s) {
    int c = 0;
 
    while(*s)
        c ^= *s++;
 
    return c;
}

void NMEA::begin(HardwareSerial *SerialPort, int baud) {
  port = SerialPort;
  port->begin(baud);

  while(!port) {;}

  Serial.println("NMEA communication initialized");
  Serial.flush();
}

void NMEA::feedthrough(void) {
  char buffer[100];
  byte n_bytes;
  n_bytes = port->readBytesUntil('*',buffer,100);
  buffer[n_bytes] = '\0';
  Serial.println(buffer);
}

void NMEA::update(void) {

  while (port->available()){

    do { // Find a starting character
      buf = port->read();
    } while (buf != '$');

    // Read until the end of the NMEA sentence
    bytes_read = port->readBytesUntil('*',nmea,100);
    nmea[bytes_read]='\0';

    // Get CRC checksum bytes
    port->readBytes(crc,2); crc[2] = '\0';

    // Convert the CRC ASCII hex string to int
    CRCin = (int)strtol(crc,NULL,16); 
    
    // Calculate a new CRC from the NMEA sentence
    CRC = checksum(nmea);

    // Decode the message only if the checksum matches
    if (CRC == CRCin) {

      Serial.println(nmea);

      // Count comma's 
      // Count double-occurrances and determine empty spots
      uint8_t i = 0;
      uint8_t n_fields = 0; // Number of commas found (thus the number of elements)
      uint8_t dc = 0;      // Number of double-commas (= number of empty elements)
      bool is_empty[20];
      
      while (nmea[i] != '\0'){
	if (nmea[i] == ',') {
	  n_fields++;
	  is_empty[n_fields] = false; // Just to make sure the bool is initialized

	  if (nmea[i-1] == ',') { 
	    dc++; 
	    is_empty[n_fields] = true;
	  }
	}
	i++;
      }
      
      char * tok;
      tok = strtok(nmea, ",");
      
      if (strcmp(tok,"GNRMC") == 0){
	decode_RMC(tok, n_fields, is_empty);
      }
    }
    
  }
}

void NMEA::decode_RMC(char * tok, uint8_t n_fields, bool is_empty[])
{
  // First element to extract is 2 (UTC)
  for (uint8_t element = 2; element <= n_fields; element++) {
    
    if (is_empty[element] == true) { continue; }
	  
    switch (element) 
      {
      default:
	break;
	      
      case RCM_ELEMENTS::UTC:
	rmc_str->utc_c = strtok(NULL,",");
	Serial.println(rmc_str->utc_c);
	rmc->utc = atoi(rmc_str->utc_c);
	Serial.println(rmc->utc);
	rmc->second = rmc->utc % 100;
	rmc->minute = (uint8_t)((rmc->utc % 10000 - rmc->second)/100);
	rmc->hour = (uint8_t)((rmc->utc - rmc->minute - rmc->second)/10000);
	break;
	      
      case RCM_ELEMENTS::STATUS:
	rmc_str->status_c = strtok(NULL,",");
	rmc->status = *rmc_str->status_c;
	break;
	      
      case RCM_ELEMENTS::LAT:
	rmc_str->lat_c = strtok(NULL,",");  // ddmm.mmm
	// Latitude degrees into float
	rmc->lat_ = atof(rmc_str->lat_c);
	rmc->lat_min_ = fmod(rmc->lat_, 100);
	rmc->lat_deg_ = rmc->lat_ - rmc->lat_min_;
	rmc->lat_deg = (rmc->lat_deg_ / 100.f) + (rmc->lat_min_ / 60.f);
	rmc->lat_rad = toRad(rmc->lat_deg);  
	break;

      case RCM_ELEMENTS::LAT_DIR:
	rmc_str->lat_dir_c = strtok(NULL,","); // N/S (north/south)
	rmc->lat_dir = *rmc_str->lat_dir_c;
	break;

      case RCM_ELEMENTS::LON:
	rmc_str->lon_c = strtok(NULL,",");  // dddmm.mmm
	rmc->lon_ = atof(rmc_str->lon_c);
	rmc->lon_min_ = fmod(rmc->lon_, 100);
	rmc->lon_deg_ = rmc->lon_ - rmc->lon_min_;
	rmc->lon_deg = (rmc->lon_deg_ / 100.f) + (rmc->lon_min_ / 60.f);
	rmc->lon_rad = toRad(rmc->lon_deg);
	break;

      case RCM_ELEMENTS::LON_DIR:
	rmc_str->lon_dir_c = strtok(NULL,","); // E/W (east/west)
	rmc->lon_dir = *rmc_str->lon_dir_c;
	break;

      case RCM_ELEMENTS::SOG: // Speed Over Ground
	rmc_str->sog_c = strtok(NULL,","); // knots
	rmc->sog_knots = atof(rmc_str->sog_c);
	rmc->sog_ms = rmc->sog_knots*0.514444f;
	break;

      case RCM_ELEMENTS::COG: // Course Over Ground
	rmc_str->cog_c = strtok(NULL,","); // degrees
	rmc->cog = atof(rmc_str->cog_c);
	break;

      case RCM_ELEMENTS::DATE:
	rmc_str->date_c = strtok(NULL,","); // ddmmyy
	rmc->date = atoi(rmc_str->date_c);
	rmc->year = rmc->date % 100;
	rmc->month = (uint8_t)((rmc->date % 10000 - rmc->year)/100);
	rmc->day = (uint8_t)((rmc->date - rmc->month - rmc->year)/10000);
	break;

      case RCM_ELEMENTS::MAG_VAR:
	rmc_str->mag_var_c = strtok(NULL,","); // Magnetic variation
	rmc->mag_var = atof(rmc_str->mag_var_c);
	break;

      case RCM_ELEMENTS::MAG_VAR_DIR:
	rmc_str->mag_var_dir_c = strtok(NULL,",");
	rmc->mag_var_dir = *rmc_str->mag_var_dir_c;
	break;
      }
  }
}
