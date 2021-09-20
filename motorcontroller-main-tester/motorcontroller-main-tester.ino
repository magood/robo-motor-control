// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
  pinMode(LED_BUILTIN, OUTPUT);
}

byte x = 0;

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(1);        // dir
  Wire.write(x);              // speed
  Wire.endTransmission();    // stop transmitting

  x++;
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(5800);
}
