#include <Arduino.h>
#include "NMEA.h"
#include <Arduino.h>


void setup();

void loop();
#line 4 "gps.ino"
NMEA gps;

void setup() {
Serial.begin(9600);

HardwareSerial *gps_port;
gps_port = &Serial2;
gps.begin(gps_port, 9600);
delay(500);
}

void loop() {
gps.update();


Serial.print("UTC: "); Serial.println(gps.rmc->utc);
Serial.print("LAT: "); Serial.println(gps.rmc->lat_deg);
Serial.print("LON: "); Serial.println(gps.rmc->lon_deg);
Serial.print("DAY: "); Serial.println(gps.rmc->day);
Serial.print("MONTH: "); Serial.println(gps.rmc->month);
Serial.print("YEAR: "); Serial.println(gps.rmc->year);
delay(500);
}
