#include "ublox.h"
#include <Arduino.h>

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
  char nav_rec_warn; // Navigation receiver warning
  char lat[]; // 
  char lon[];
  char sog[]; // speed over ground knots
  char cog[]; // course [deg]
  char date_of_fix[]; // ddmmyy
  char mag_var[];     // magnetic variation
  char mag_var_dir;   // direction of magnetic variation
  char checksum[2];  
};

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
}

void loop() {
  while (Serial2.available()){

    do { // Find a starting character
      buf = Serial2.read();
    } while (buf != '$');

    char nmea[100];

    // Read until the end of the NMEA sentence
    byte bytes_read = Serial2.readBytesUntil('*',nmea,100);
    nmea[bytes_read]='\0';
    
    // Get CRC checksum bytes
    char crc[3];
    Serial2.readBytes(crc,2); crc[2] = '\0';

    // Convert the CRC ASCII hex string to int
    int CRCin = (int)strtol(crc,NULL,16); 
    
    // Calculate a new CRC from the NMEA sentence
    int CRC;
    CRC = checksum(nmea);

    // Decode the message only if the checksum matches
    if (CRC == CRCin) {

      char * tok;
      tok = strtok(nmea, ",");

      if (strcmp(tok,"GNRMC") == 0){
	Serial.print("RMC FOUND");
      }
      /*
      while (tok != NULL)
	{
	  Serial.println(tok);
	  tok = strtok(NULL,",");
	}
      */
      
      /*      uint8_t csr = 0; // A "cursor" to keep track of the current location
      // Get the message Talker ID
      char m_talkerID[3];
      m_talkerID[0] = nmea[csr++]; // csr is incremented after the access
      m_talkerID[1] = nmea[csr++];
      m_talkerID[2] = '\0';
      // Get the message Type
      char m_type[4];
      m_type[0] = nmea[csr++];
      m_type[1] = nmea[csr++];
      m_type[2] = nmea[csr++];
      m_type[3] = '\0';
      
      //      Serial.println(m_talkerID);
      //Serial.println(m_type);

      if (strcmp(m_type, "GSV") == 0) {
	 
      }
      else if (strcmp(m_type, "GSA") == 0) {
	 
      }
      else if (strcmp(m_type, "GLL") == 0) {
	 
      }
      else if (strcmp(m_type, "GGA") == 0) {
	 
      }
      else if (strcmp(m_type, "RMC") == 0) {
	/*csr++;
	char utc_time[7];
	utc_time[6] = '\0';
	uint8_t u = 0;

	do {
	  utc_time[u++] = nmea[csr++];
	} while(nmea[csr] != ',');

	Serial.print("UTC: ");
	Serial.println(utc_time);
	}


      else if (strcmp(m_type, "VTG") == 0) {
	 
      }
      */
      //Serial.print("CRCi: "); Serial.println(CRCin,HEX);
      //Serial.print("CRCc: "); Serial.println(CRC,HEX);
      //Serial.print("Bytes: "); Serial.println(bytes_read);    
      //Serial.print(nmea);
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