#include "NMEA.h"
#include <Arduino.h>

NMEA gps;

void setup() {
  Serial.begin(115200);

  HardwareSerial *gps_port;
  gps_port = &Serial2;
  gps.begin(gps_port, 230400);
  delay(500);
}

void loop() {
  gps.update();
  //gps.feedthrough();

  /* // RMC prints
  Serial.print("UTC: "); Serial.println(gps.rmc->utc);
  Serial.print("LAT: "); Serial.println(gps.rmc->lat_deg);
  Serial.print("LON: "); Serial.println(gps.rmc->lon_deg);
  Serial.print("DAY: "); Serial.println(gps.rmc->day);
  Serial.print("MONTH: "); Serial.println(gps.rmc->month);
  Serial.print("YEAR: "); Serial.println(gps.rmc->year);
  
  // GLL prints
  Serial.print("UTC: "); Serial.println(gps.gll->utc);
  Serial.print("LAT: "); Serial.println(gps.gll->lat_deg);
  Serial.print("LON: "); Serial.println(gps.gll->lon_deg);
  Serial.print("HOUR: "); Serial.println(gps.gll->hour);
  Serial.print("MINUTE: "); Serial.println(gps.gll->minute);
  Serial.print("SECOND: "); Serial.println(gps.gll->second);
  
  // GGA prints
  Serial.print("UTC: "); Serial.println(gps.gga->utc);
  Serial.print("LAT: "); Serial.println(gps.gga->lat_deg);
  Serial.print("LON: "); Serial.println(gps.gga->lon_deg);
  Serial.print("HOUR: "); Serial.println(gps.gga->hour);
  Serial.print("MINUTE: "); Serial.println(gps.gga->minute);
  Serial.print("SECOND: "); Serial.println(gps.gga->second);
  Serial.print("QUALITY: "); Serial.println(gps.gga->fix_quality);
  */
  // VTG prints
  Serial.print("TRUE TRACK: "); Serial.println(gps.vtg->true_track_deg);
  Serial.print("MAGNETIC TRACK: "); Serial.println(gps.vtg->magnetic_track_deg);
  Serial.print("Knots: "); Serial.println(gps.vtg->sog_knots,4);
  Serial.print("KPH: "); Serial.println(gps.vtg->sog_kph,4);

  delay(500);
}

