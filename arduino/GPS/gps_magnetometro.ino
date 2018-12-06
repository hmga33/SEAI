/*##########---PINOS---#########
-------GPS:
TX - 5 (arduino)
RX - 4 (arduino)
VIN - 5V
GND - GND
-------MGNETOMETRO:
SDA - SDA (arduino)
SCL - SCL (arduino)
VDD - 5V
GND - GND
-------ACELEROMETRO:
SDA - A4 (arduino)
SCL - A5 (arduino)
INT - 2 (arduino)
VCC - 5V
GND - GND
###################################*/

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include "Wire.h"
#include "SparkFunIMU.h"
#include "SparkFunLSM303C.h"
#include "LSM303CTypes.h"

float speed=0;
unsigned long start, finished, t;

SoftwareSerial mySerial(5, 4);
Adafruit_GPS GPS(&mySerial);
#define GPSECHO  false
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy
LSM303C myIMU;

void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit GPS library basic test!");
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  useInterrupt(true);
  delay(1000);
  mySerial.println(PMTK_Q_RELEASE);
  if (myIMU.begin(
    MODE_I2C,
    MAG_DO_40_Hz,
    MAG_FS_16_Ga,
    MAG_BDU_ENABLE,
    MAG_OMXY_HIGH_PERFORMANCE,
    MAG_OMZ_HIGH_PERFORMANCE,
    MAG_MD_CONTINUOUS,
    ACC_FS_2g,
    ACC_BDU_ENABLE,
    ACC_X_ENABLE|ACC_Y_ENABLE|ACC_Z_ENABLE,
    ACC_ODR_100_Hz
  ) != IMU_SUCCESS)
  {
    Serial.println("Failed setup.");
    while(1);
  }
}

SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

uint32_t timer = millis();

void loop()
{
  float mag_x,mag_y,mag_z, acc_x, acc_y, acc_z;
  
  acc_x = myIMU.readAccelX();
  if ( !isnan(acc_x) )
  {
    //Serial.print("\nAccelerometer:\n X = ");
    //Serial.println(acc_x-5, 4);
    acc_y = myIMU.readAccelY();
    //Serial.print(" Y = ");
    //Serial.println(acc_y-64, 4);
    acc_z = myIMU.readAccelZ();
    //Serial.print(" Z = ");
    //Serial.println(acc_z, 4);

  finished=millis();
  t=finished-start;
  //Serial.print(t,4);
  //Serial.println(" milliseconds elapsed");
  if(t>0 /*&& (abs(acc_x)>15 || abs(acc_y)>15)*/){   //para filtrar "ruidos" menores que 15
    speed=speed+9.8*(sqrt(((acc_x-5)*(acc_x-5)*((acc_y-64)*(acc_y-64))))*2*0.001)*t/1000;     //2 -> sensibilidade é 2g
    //Serial.print("--------------------------------Speed (m/s): ");
    //Serial.println(speed,4);
    start=millis();
  } 
  }

  mag_x = myIMU.readMagX();
  if ( !isnan(mag_x) )
  { 
    //Serial.print("\nMagnetometer:\n X = ");
    //Serial.println(mag_x, 4);
    mag_y = myIMU.readMagY();
    //Serial.print(" Y = ");
    //Serial.println(mag_y, 4);
    mag_z = myIMU.readMagZ();
    //Serial.print(" Z = ");
    //Serial.println(mag_z, 4);
    float Pi = 3.14159;
    float heading = (atan2(mag_y,mag_x) * 180) / Pi;
    if (heading < 0)
    {
      heading = 360 + heading;
    }
    Serial.print("---------Compass Heading: ");
    Serial.println(heading);
  }
  
  //value = myIMU.readTempC();
  //if ( !isnan(value) )
  //{
    //Serial.print("\nThermometer:\n");
    //Serial.print(" Degrees C = ");
    //Serial.println(value, 4);
    //Serial.print(" Degrees F = ");
    //Serial.println(myIMU.readTempF(), 4);
  //}

  if (! usingInterrupt) {
    char c = GPS.read();
    if (GPSECHO)
      if (c) Serial.print(c);
  }
  if (GPS.newNMEAreceived()) { 
    if (!GPS.parse(GPS.lastNMEA()))
      return;  
  }
  if (timer > millis())  timer = millis();
  if (millis() - timer > 2000) { 
    timer = millis(); // reset the timer
    Serial.print("\nTime: ");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality); 
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", "); 
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Location (in degrees, works with Google Maps): ");
      Serial.print(GPS.latitudeDegrees, 4);
      Serial.print(", "); 
      Serial.println(GPS.longitudeDegrees, 4);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
  }
  
  //delay(1000);
}
