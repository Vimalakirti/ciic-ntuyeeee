#include <EEPROM.h>
#include <OneWire.h>
#include "GravityTDS.h"

#define TdsSensorPin A1
GravityTDS gravityTds;

int DS18S20_Pin = 2; //DS18S20 Signal pin on digital 2
//Temperature chip i/o
OneWire ds(DS18S20_Pin);  // on digital pin 2

float temperature = 25, tdsValue = 0;

void setup()
{
    Serial.begin(9600);
    gravityTds.setPin(TdsSensorPin);
    gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();  //initialization
}

void loop()
{
    Serial.println("START");
    temperature = getTemp();  //add your temperature sensor and read it
    gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
    gravityTds.update();  //sample and calculate 
    tdsValue = gravityTds.getTdsValue();  // then get the value
    Serial.println(temperature,0);
    //Serial.println("(Celsius)");
    Serial.println(tdsValue,0);
    //Serial.println("(ppm)");
    Serial.println("END");
    delay(6000);
}


float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius
  
  byte data[12];
  byte addr[8];
  
  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      Serial.println("no more sensors on chain, reset search!");
      ds.reset_search();
      return -1000;
  }
  
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }
  
  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end
  
  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad
  
    
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
    
  ds.reset_search();
    
  byte MSB = data[1];
  byte LSB = data[0];
  
  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
    
  return TemperatureSum;
    
}
