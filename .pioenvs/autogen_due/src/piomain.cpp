#include <Arduino.h>
#include "ublox.h"
#include <Arduino.h>


void setup();

void loop();

int checksum(char *s);

inline float toRad(float x);

inline float toDeg(float x);
#line 4 "gps.ino"
const float pi = 3.141592653589793;

char separator = ',';
char start_char = '$';
char stop_char = '*';
char msg_name[6];


char buf;
char long_buf[100];

namespace NMEA
{
char RMC[] = "GPRMC";
}

enum M_TYPES {
RMC,
GSV,
GLL,
GSA,
GGA,
VTG
};

int utc;
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

namespace RCM_ELEMENTS {
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

void setup() {
Serial.begin(9600);
Serial2.begin(9600);
}

void loop() {
while (Serial2.available()){

do {
buf = Serial2.read();
} while (buf != '$');

char nmea[100];


byte bytes_read = Serial2.readBytesUntil('*',nmea,100);
nmea[bytes_read]='\0';


char crc[3];
Serial2.readBytes(crc,2); crc[2] = '\0';


int CRCin = (int)strtol(crc,NULL,16);


int CRC;
CRC = checksum(nmea);


if (CRC == CRCin) {

Serial.println(nmea);



uint8_t i = 0;
uint8_t n_comma = 0;
uint8_t dc = 0;
bool is_empty[20];

while (nmea[i] != '\0'){
if (nmea[i] == ',') {
n_comma++;
is_empty[n_comma] = false;

if (nmea[i-1] == ',') {
dc++;
is_empty[n_comma] = true;
}
}
i++;
}





char * tok;
tok = strtok(nmea, ",");

if (strcmp(tok,"GNRMC") == 0){


for (uint8_t element = 2; element <= n_comma; element++) {
if (is_empty[element] == true) { continue; }

switch (element)
{
default:
break;

case RCM_ELEMENTS::UTC:
utc_c = strtok(NULL,",");
utc = atoi(utc_c);
uint8_t second;
uint8_t minute;
uint8_t hour;

second = utc % 100;
minute = (uint8_t)((utc % 10000 - second)/100);
hour = (uint8_t)((utc - minute  - second)/10000);
break;

case RCM_ELEMENTS::STATUS:
status_c = strtok(NULL,",");
break;

case RCM_ELEMENTS::LAT:
lat_c = strtok(NULL,",");

char lat_deg_c[3];
strncpy(lat_deg_c,lat_c,2);
lat_deg_c[2] = '\0';
float lat_deg;
lat_deg = atof(lat_deg_c);


char lat_min_c[9];
strcpy(lat_min_c, lat_c+=2);
float lat_min;
lat_min = atof(lat_min_c);


lat_deg = lat_deg + (lat_min/60.f);
float lat;
lat = toRad(lat_deg);
Serial.print("LAT: "); Serial.println(lat,6);
break;

case RCM_ELEMENTS::LAT_DIR:
lat_dir_c = strtok(NULL,",");
break;

case RCM_ELEMENTS::LON:
lon_c = strtok(NULL,",");

char lon_deg_c[4];
strncpy(lon_deg_c,lon_c,3);
lon_deg_c[3] = '\0';
float lon_deg;
lon_deg = atof(lon_deg_c);


char lon_min_c[9];
strcpy(lon_min_c, lon_c+=3);
float lon_min;
lon_min = atof(lon_min_c);


lon_deg = lon_deg + (lon_min / 60.f);
float lon;
lon = toRad(lon_deg);
break;

case RCM_ELEMENTS::LON_DIR:
lon_dir_c = strtok(NULL,",");
break;

case RCM_ELEMENTS::SOG:
sog_c = strtok(NULL,",");
float sog_knots;
sog_knots = atof(sog_c);
float sog_ms;
sog_ms = sog_knots*0.514444f;
break;

case RCM_ELEMENTS::COG:
cog_c = strtok(NULL,",");
float cog;
cog = atof(cog_c);
break;

case RCM_ELEMENTS::DATE:
date_c = strtok(NULL,",");
int date;
uint8_t day;
uint8_t month;
uint8_t year;

date = atoi(date_c);
year = date % 100;
month = (uint8_t)((date % 10000 - year)/100);
day = (uint8_t)((date - month - year)/10000);
break;

case RCM_ELEMENTS::MAG_VAR:
mag_var_c = strtok(NULL,",");
float mag_var;
mag_var = atof(mag_var_c);
break;

case RCM_ELEMENTS::MAG_VAR_DIR:
mag_var_dir_c = strtok(NULL,",");
break;
}
}
}
}

}

delay(500);
}

int checksum(char *s) {
int c = 0;

while(*s)
c ^= *s++;

return c;
}

inline float toRad(float x){
return (x * (pi/180.f));
}

inline float toDeg(float x){
return (x * (180.f/pi));
}