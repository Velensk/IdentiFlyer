/**
 *  ___ ___  _  _   _     ___  __  ___    ___ ___  ___
 * | __/ _ \| \| | /_\   ( _ )/  \( _ )  / __| _ \/ __|
 * | _| (_) | .` |/ _ \  / _ \ () / _ \ | (_ |  _/\__ \
 * |_| \___/|_|\_/_/ \_\ \___/\__/\___/  \___|_|  |___/
 *
 * This example is meant to work with the Adafruit
 * FONA 808 or 3G Shield or Breakout
 *
 * Copyright: 2015 Adafruit
 * Author: Todd Treece
 * Licence: MIT
 *
 */
#include "Adafruit_FONA.h"

/////////////////////////////////////////////////////////////////////////// HELPER

void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}

int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }
 
    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';
 
    reverse(str, i);
    str[i] = '\0';
    return i;
}

//This is a second int to string method as the first one is built to work with the float to str and
//doesn't provide the correct output format.
void tostring(char str[], int num)
{
    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}
 
// Converts a floating point number to string.
void ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;
 
    // Extract floating part
    float fpart = n - (float)ipart;
 
    // convert integer part to string
    int i = intToStr(ipart, res, 0);
 
    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot
 
        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);
 
        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////GLOBAL

// standard pins for the shield, adjust as necessary
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

#define SERIAL_NUM 7
#define COORD_DIGITS 4

// We default to using software serial. If you want to use hardware serial
// (because softserial isnt supported) comment out the following three lines 
// and uncomment the HardwareSerial line
#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

// Hardware serial is also possible!
//  HardwareSerial *fonaSerial = &Serial1;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

// Have a FONA 3G? use this object type instead
//Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

///////////////////////////////////////////////////////////////////////////// SETUP

void setup() {

  while (! Serial);

  Serial.begin(115200);
  Serial.println(F("Adafruit FONA 808 & 3G GPS demo"));
  Serial.println(F("Initializing FONA... (May take a few seconds)"));

  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while(1);
  }
  Serial.println(F("FONA is OK"));
  // Try to enable GPRS
  

  Serial.println(F("Enabling GPS..."));
  fona.enableGPS(true);
}

//////////////////////////////////////////////////////////////////////////  LOOP

void loop() {
  delay(2000);

  float latitude, longitude, speed_kph, heading, speed_mph, altitude;

  // if you ask for an altitude reading, getGPS will return false if there isn't a 3D fix
  boolean gps_success = fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude);

  if (gps_success) {

    Serial.print("GPS lat:");
    Serial.println(latitude, 6);
    Serial.print("GPS long:");
    Serial.println(longitude, 6);
    Serial.print("GPS speed KPH:");
    Serial.println(speed_kph);
    Serial.print("GPS speed MPH:");
    speed_mph = speed_kph * 0.621371192;
    Serial.println(speed_mph);
    Serial.print("GPS heading:");
    Serial.println(heading);
    Serial.print("GPS altitude:");
    Serial.println(altitude);

  } else {
    Serial.println("Waiting for FONA GPS 3D fix...");
  }

  // Fona 3G doesnt have GPRSlocation :/
  if ((fona.type() == FONA3G_A) || (fona.type() == FONA3G_E))
    return;
  // Check for network, then GPRS 
  Serial.println(F("Checking for Cell network..."));
  if (fona.getNetworkStatus() == 1) {
    // network & GPRS? Great! Print out the GSM location to compare
    boolean gsmloc_success = fona.getGSMLoc(&latitude, &longitude);

    if (gsmloc_success) {
      Serial.print("GSMLoc lat:");
      Serial.println(latitude, 6);
      Serial.print("GSMLoc long:");
      Serial.println(longitude, 6);
      Serial.print("NetworkStatus:");
      Serial.println(fona.getNetworkStatus());
      uint16_t statuscode;
        int16_t length;
        char url[80];
        char data[80];
        char longtitudeStr[9];
        char latitudeStr[9];
        char SerialNumberStr[25];

        bool lat_sign=false;
        bool long_sign=false;
        //The following code exists because the method that converts floats to strings doesn't like negatives.
        if (latitude<0)
          {
            latitude *= -1;
            lat_sign =true;
          }
          if (longitude<0)
          {
            longitude *= -1;
            long_sign =true;
          }

        Serial.println("Co-ords");
        Serial.println(latitude);
        Serial.println(longitude);

        ftoa(longitude, longtitudeStr, COORD_DIGITS);
        ftoa(latitude, latitudeStr, COORD_DIGITS);

        Serial.println("Pre-signcheck");
        Serial.println(longtitudeStr);
        Serial.println(latitudeStr);
        char* temp = "-";
        if (long_sign)
          {
            strcat(temp, longtitudeStr);
            Serial.println("Temp,string");
            Serial.println(temp);
            Serial.println(longtitudeStr);
            strcpy(longtitudeStr, temp);
          }
          temp = "-";
          if (lat_sign)
          {
            strcat(temp, latitudeStr);
            strcpy(longtitudeStr, temp);
          }
        delay(2);
        Serial.println("Post signcheck");
        Serial.println(longtitudeStr);
        Serial.println(latitudeStr);
        tostring(SerialNumberStr, SERIAL_NUM);
        Serial.println(SerialNumberStr);
        
        FONAFlashStringPtr contentType = FONAFlashStringPtr();

        strcpy(url, "notifly.pompeii.dog/location/create/");
        strcpy(data, "{\"longitude\": \"");
        
        strcat(url, SerialNumberStr);
        strcat(data, longtitudeStr);
        strcat(data, "\", \"latitude\": \"");
        strcat(data, latitudeStr);
        strcat(data, "\"}");
   
        Serial.println(url);
        Serial.println(data);

        Serial.println(F("****"));
        if (!fona.HTTP_POST_start("notifly.pompeii.dog/location/create/15", F("application/json"), (uint8_t *) data, strlen(data), &statuscode, (uint16_t *)&length)) {
          Serial.println("Failed!");

        }
        while (length > 0) {
          while (fona.available()) {
                                   char c = fona.read();
                                   Serial.write(c);
                                   length--;
                                   }
                           }
          fona.HTTP_POST_end();
    }else {
      Serial.println("GSM location failed...");
      Serial.println(F("Disabling GPRS"));
      fona.enableGPRS(false);
      Serial.println(F("Enabling GPRS"));
      if (!fona.enableGPRS(true)) {
        Serial.println(F("Failed to turn GPRS on"));  
      }
    }
  }
}

