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


struct RMC {
char fix_time[];
char nav_rec_warn;
char lat[];
char lon[];
char sog[];
char cog[];
char date_of_fix[];
char mag_var[];
char mag_var_dir;
char checksum[2];
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

char * tok;
tok = strtok(nmea, ",");

if (strcmp(tok,"GNRMC") == 0){

char * utc_c = strtok(NULL,",");
char * status_c = strtok(NULL,",");
char * lat_c = strtok(NULL,",");
char * lat_dir_c = strtok(NULL,",");
char * lon_c = strtok(NULL,",");
char * lon_dir_c = strtok(NULL,",");
char * sog_c = strtok(NULL,",");
char * cog_c = strtok(NULL,",");
char * date_c = strtok(NULL,",");
char * mag_var_c = strtok(NULL,",");


int utc = atoi(utc_c);



char lat_deg_c[3];
strncpy(lat_deg_c,lat_c,2);
lat_deg_c[2] = '\0';
float lat_deg = atof(lat_deg_c);



char lat_min_c[9];
strcpy(lat_min_c, lat_c+=2);

float lat_min = atof(lat_min_c);



lat_deg = lat_deg + (lat_min/60.f);

float lat = toRad(lat_deg);



Serial.println(lon_c);
char lon_deg_c[4];
strncpy(lon_deg_c,lon_c,3);
lon_deg_c[3] = '\0';
float lon_deg = atof(lon_deg_c);


char lon_min_c[9];
strcpy(lon_min_c, lon_c+=3);
float lon_min = atof(lon_min_c);


lon_deg = lon_deg + (lon_min / 60.f);
Serial.println(lon_deg,6);
float lon = toRad(lon_deg);
Serial.println(lon,6);
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