#include <Arduino.h>
#include "ublox.h"
#include <Arduino.h>


void setup();

void loop();

int checksum(char *s);
#line 4 "gps.ino"
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
Serial.print("RMC FOUND");
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