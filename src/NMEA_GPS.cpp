#include "NMEA_GPS.h"

NMEA_GPS::NMEA_GPS (){ // Constructor
}

int NMEA_GPS::checksum(char * s) {
    int c = 0;
 
    while(*s)
        c ^= *s++;
 
    return c;
}

void NMEA_GPS::begin(HardwareSerial *SerialPort, int baud) {
  port = SerialPort;
  port->begin(baud);

  while(!port) {;}

  Serial.println("NMEA communication initialized");
  Serial.flush();
}

void NMEA_GPS::feedthrough(void) {
  char buffer[100];
  byte n_bytes;
  n_bytes = port->readBytesUntil('*',buffer,100);
  buffer[n_bytes] = '\0';
  Serial.println(buffer);
}

void NMEA_GPS::update(void) {

  while (port->available()){

    do { // Find a starting character
      if (!port->available()) return;
      buf = port->read();
      // if ((buf == '\n') || (buf == '\cr')) return;
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

      strcpy(nmea_copy, nmea);

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

      // Use appropriate method to decode NMEA sentence
      
      if (strcmp(tok,"GNRMC") == 0)
	{
	  if (rmc == 0) { // Allocate memory if not already done
	    rmc_str = new(RMC_STRINGS);
	    rmc     = new(RMC);
	    rmc->utc = 0;
	  }
	  decode_RMC(tok, n_fields, is_empty);
	}
      
      else if (strcmp(tok,"GNGLL") == 0)
	{
	  if (gll == 0) {
	    gll_str = new(GLL_STRINGS);
	    gll     = new(GLL);
	    gll->utc = 0;
	  }
	  decode_GLL(tok, n_fields, is_empty);
	}      

      else if (strcmp(tok, "GNGGA") == 0)
	{
	  if (gga == 0) {
	    gga_str = new (GGA_STRINGS);
	    gga     = new (GGA);
	    gga->utc = 0;
	    gga->fix_quality = 0;
	  }
	  decode_GGA(tok, n_fields, is_empty);
	}

      else if (strcmp(tok, "GNVTG") == 0)
	{
	  if (vtg == 0) {
	    vtg_str = new (VTG_STRINGS);
	    vtg     = new (VTG);
	  }
	  decode_VTG(tok, n_fields, is_empty);
	}
    }
    
  }

}

void NMEA_GPS::decode_VTG(char * tok, uint8_t n_fields, bool is_empty[])
{
  strcpy(vtg->vtg, nmea_copy);

  for (uint8_t element = 2; element <= n_fields; element++)
    {
      //if (is_empty[element] == true) { continue; }
      
      switch (element)
	{
	default:
	  // Required as some elements are skipped
	  char * dump;
	  dump = strtok(NULL,","); 
	  break;
	case VTG_ELEMENTS::TRACK_TRUE:
	  if (is_empty[element] == true) {
	    vtg->true_track_deg = NAN;
	    vtg->true_track_rad = NAN;
	    continue;
	  }
	  vtg_str->true_track = strtok(NULL,",");
	  vtg->true_track_deg = atof(vtg_str->true_track);
	  vtg->true_track_rad = toRad(vtg->true_track_deg);
	  break;
	case VTG_ELEMENTS::TRACK_MAGNETIC:
	  if (is_empty[element] == true) {
	    vtg->magnetic_track_deg = NAN;
	    vtg->magnetic_track_rad = NAN;
	    continue;
	  }
	  vtg_str->magnetic_track = strtok(NULL,",");
	  vtg->magnetic_track_deg = atof(vtg_str->magnetic_track);
	  vtg->magnetic_track_rad = toRad(vtg->magnetic_track_deg);
	  break;
	case VTG_ELEMENTS::SOG_KNOTS:
	  vtg_str->sog_knots = strtok(NULL,",");
	  vtg->sog_knots = atof(vtg_str->sog_knots);
	  break;
	case VTG_ELEMENTS::SOG_KPH:
	  vtg_str->sog_kph = strtok(NULL,",");
	  vtg->sog_kph = atof(vtg_str->sog_kph);
	  break;
	}
    }
}

void NMEA_GPS::decode_GGA(char * tok, uint8_t n_fields, bool is_empty[])
{
  strcpy(gga->gga, nmea_copy);

  for (uint8_t element = 2; element <= n_fields; element++)
    {
      if (is_empty[element] == true) { continue; }

      switch (element)
	{
	default:
	  break;
	case GGA_ELEMENTS::UTC:
	  float tmp_utc;
	  gga_str->utc = strtok(NULL,",");
	  gga->utc     = atoi(gga_str->utc);
	  gga->second  = gga->utc % 100;
	  gga->minute  = (uint8_t)((gga->utc % 10000 - gga->second)/100);
	  gga->hour    = (uint8_t)((gga->utc - gga->minute - gga->second)/10000);
	  break;
	case GGA_ELEMENTS::LAT:
	  gga_str->lat  = strtok(NULL,",");
	  gga->lat_     = atof(gga_str->lat);
	  gga->lat_min_ = fmod(gga->lat_,100);
	  gga->lat_deg_ = gga->lat_ - gga->lat_min_;
	  gga->lat_deg  = (gga->lat_deg_ / 100.f) + (gga->lat_min_ / 60.f);
	  gga->lat_rad  = toRad(gga->lat_deg);
	  break;
	case GGA_ELEMENTS::LAT_DIR:
	  gga_str->lat_dir = strtok(NULL,",");
	  gga->lat_dir     = *gga_str->lat_dir;
	  break;
	case GGA_ELEMENTS::LON:
	  gga_str->lon  = strtok(NULL,",");
	  gga->lon_     = atof(gga_str->lon);
	  gga->lon_min_ = fmod(gga->lon_,100);
	  gga->lon_deg_ = gga->lon_ - gga->lon_min_;
	  gga->lon_deg  = (gga->lon_deg_ / 100.f) + (gga->lon_min_ / 60.f);
	  gga->lon_rad  = toRad(gga->lon_deg);
	  break;
	case GGA_ELEMENTS::LON_DIR:
	  gga_str->lon_dir = strtok(NULL,",");
	  gga->lon_dir     = *gga_str->lon_dir;
	  break;
	case GGA_ELEMENTS::FIX_QUALITY:
	  gga_str->fix_quality = strtok(NULL,",");
	  if (*gga_str->fix_quality == '0') {
	    gga->fix_quality = 0; }
	  else {
	    gga->fix_quality = atoi(gga_str->fix_quality); }
	  break;
	case GGA_ELEMENTS::NUMBER_OF_SATELLITES:
	  gga_str->n_sats = strtok(NULL,",");
	  gga->n_sats = atoi(gga_str->n_sats);
	  break;
	case GGA_ELEMENTS::HDOP:
	  gga_str->hdop = strtok(NULL,",");
	  gga->hdop = atof(gga_str->hdop);
	  break;
	case GGA_ELEMENTS::ALTITUDE:
	  gga_str->alt = strtok(NULL,",");
	  gga->alt = atof(gga_str->alt);
	  break;
	case GGA_ELEMENTS::ALT_REF:
	  gga_str->alt_ref = strtok(NULL,",");
	  gga->alt_ref = *gga_str->alt_ref;
	  break;
	case GGA_ELEMENTS::HEIGHT_OF_GEOID:
	  gga_str->hei = strtok(NULL,",");
	  gga->hei = atof(gga_str->hei);
	  break;
	case GGA_ELEMENTS::HEIGHT_REF:
	  gga_str->hei_ref = strtok(NULL,",");
	  gga->hei_ref = *gga_str->hei_ref;
	  break;
	case GGA_ELEMENTS::TIME_SINCE_LAST_DGPS:
	  gga_str->time_since_dgps = strtok(NULL,",");
	  gga->time_since_dgps = atoi(gga_str->time_since_dgps);
	  break;
	case GGA_ELEMENTS::DGPS_STATION_ID_NUMBER:
	  gga_str->dgps_station_id = strtok(NULL,",");
	  gga->dgps_station_id = atoi(gga_str->dgps_station_id);
	  break;
	}
    }
}

void NMEA_GPS::decode_GLL(char * tok, uint8_t n_fields, bool is_empty[])
{
  strcpy(gll->gll, nmea_copy);

  for (uint8_t element = 2; element <= n_fields; element++)
    {
      if (is_empty[element] == true) { continue; }

      switch (element)
	{
	default:
	  break;
	case GLL_ELEMENTS::LAT:
	  gll_str->lat  = strtok(NULL,",");
	  gll->lat_     = atof(gll_str->lat);
	  gll->lat_min_ = fmod(gll->lat_,100);
	  gll->lat_deg_ = gll->lat_ - gll->lat_min_;
	  gll->lat_deg  = (gll->lat_deg_ / 100.f) + (gll->lat_min_ / 60.f);
	  gll->lat_rad  = toRad(gll->lat_deg);
	  break;
	case GLL_ELEMENTS::LAT_DIR:
	  gll_str->lat_dir = strtok(NULL,",");
	  gll->lat_dir     = *gll_str->lat_dir;
	  break;
	case GLL_ELEMENTS::LON:
	  gll_str->lon  = strtok(NULL,",");
	  gll->lon_     = atof(gll_str->lon);
	  gll->lon_min_ = fmod(gll->lon_,100);
	  gll->lon_deg_ = gll->lon_ - gll->lon_min_;
	  gll->lon_deg  = (gll->lon_deg_ / 100.f) + (gll->lon_min_ / 60.f);
	  gll->lon_rad  = toRad(gll->lon_deg);
	  break;
	case GLL_ELEMENTS::LON_DIR:
	  gll_str->lon_dir = strtok(NULL,",");
	  gll->lon_dir     = *gll_str->lon_dir;
	  break;
	case GLL_ELEMENTS::UTC:
	  gll_str->utc = strtok(NULL,",");
	  gll->utc     = atoi(gll_str->utc);
	  gll->second  = gll->utc % 100;
	  gll->minute  = (uint8_t)((gll->utc % 10000 - gll->second)/100);
	  gll->hour    = (uint8_t)((gll->utc - gll->minute - gll->second)/10000);
	  break;
	case GLL_ELEMENTS::VALIDITY:
	  gll_str->validity = strtok(NULL,",");
	  gll->validity        = *gll_str->validity;
	  break;
	}
    }
}

void NMEA_GPS::decode_RMC(char * tok, uint8_t n_fields, bool is_empty[])
{
  strcpy(rmc->rmc, nmea_copy);

  // First element to extract is 2 (UTC)
  for (uint8_t element = 2; element <= n_fields; element++) {
    
    if (is_empty[element] == true) { continue; }
	  
    switch (element) 
      {
      default:
	break;
	      
      case RMC_ELEMENTS::UTC:
	rmc_str->utc_c = strtok(NULL,",");
	rmc->utc       = atoi(rmc_str->utc_c);
	rmc->second    = rmc->utc % 100;
	rmc->minute    = (uint8_t)((rmc->utc % 10000 - rmc->second)/100);
	rmc->hour      = (uint8_t)((rmc->utc - rmc->minute - rmc->second)/10000);
	break;
	      
      case RMC_ELEMENTS::STATUS:
	rmc_str->status_c = strtok(NULL,",");
	rmc->status       = *rmc_str->status_c;
	break;
	      
      case RMC_ELEMENTS::LAT:
	rmc_str->lat_c = strtok(NULL,",");  // ddmm.mmm
	// Latitude degrees into float
	rmc->lat_      = atof(rmc_str->lat_c);
	rmc->lat_min_  = fmod(rmc->lat_, 100);
	rmc->lat_deg_  = rmc->lat_ - rmc->lat_min_;
	rmc->lat_deg   = (rmc->lat_deg_ / 100.f) + (rmc->lat_min_ / 60.f);
	rmc->lat_rad   = toRad(rmc->lat_deg);  
	break;

      case RMC_ELEMENTS::LAT_DIR:
	rmc_str->lat_dir_c = strtok(NULL,","); // N/S (north/south)
	rmc->lat_dir       = *rmc_str->lat_dir_c;
	break;

      case RMC_ELEMENTS::LON:
	rmc_str->lon_c = strtok(NULL,",");  // dddmm.mmm
	rmc->lon_      = atof(rmc_str->lon_c);
	rmc->lon_min_  = fmod(rmc->lon_, 100);
	rmc->lon_deg_  = rmc->lon_ - rmc->lon_min_;
	rmc->lon_deg   = (rmc->lon_deg_ / 100.f) + (rmc->lon_min_ / 60.f);
	rmc->lon_rad   = toRad(rmc->lon_deg);
	break;

      case RMC_ELEMENTS::LON_DIR:
	rmc_str->lon_dir_c = strtok(NULL,","); // E/W (east/west)
	rmc->lon_dir       = *rmc_str->lon_dir_c;
	break;

      case RMC_ELEMENTS::SOG: // Speed Over Ground
	rmc_str->sog_c = strtok(NULL,","); // knots
	rmc->sog_knots = atof(rmc_str->sog_c);
	rmc->sog_ms    = rmc->sog_knots*0.514444f;
	break;

      case RMC_ELEMENTS::TC: // True Course
	rmc_str->tc_c = strtok(NULL,","); // degrees
	rmc->tc       = atof(rmc_str->tc_c);
	break;

      case RMC_ELEMENTS::DATE:
	rmc_str->date_c = strtok(NULL,","); // ddmmyy
	rmc->date       = atoi(rmc_str->date_c);
	rmc->year       = rmc->date % 100;
	rmc->month      = (uint8_t)((rmc->date % 10000 - rmc->year)/100);
	rmc->day        = (uint8_t)((rmc->date - rmc->month - rmc->year)/10000);
	break;

      case RMC_ELEMENTS::MAG_VAR:
	rmc_str->mag_var_c = strtok(NULL,","); // Magnetic variation
	rmc->mag_var       = atof(rmc_str->mag_var_c);
	break;

      case RMC_ELEMENTS::MAG_VAR_DIR:
	rmc_str->mag_var_dir_c = strtok(NULL,",");
	rmc->mag_var_dir       = *rmc_str->mag_var_dir_c;
	break;
      }
  }
}
