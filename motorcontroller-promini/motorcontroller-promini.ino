// Simple motor controller firmware
// Responding to I2C messages from some Main
// And including some safety features like a deadman switch.
// Could be expanded with target absolute speeds, PID handling, etc.
// Well suited to a pro-mini board.
// MG 2021

#include <Wire.h>

byte commanded_speed = 0;
byte commanded_direction = 1; //1 for forward, 0 for reverse
unsigned long last_command_millis = 0; // millis() value the last time a command was recieved from the main.
unsigned long command_timeout = 8000; //how long between commands before we assume the main is dead?

const byte M_PWM_PIN = 8;
const byte M_DIR_PIN = 9;
const bool DEBUG_SERIAL_OUT = true;
const unsigned int LOOP_DELAY = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  //Speed/direction/LED indicator pin setup
  pinMode(M_PWM_PIN, OUTPUT);
  analogWrite(M_PWM_PIN, 0);
  pinMode(LED_BUILTIN, OUTPUT);
  analogWrite(LED_BUILTIN, 0);
  pinMode(M_DIR_PIN, OUTPUT);
  digitalWrite(M_DIR_PIN, HIGH);

  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
}

// the loop function runs over and over again forever
void loop() {
  unsigned long now = millis();
  if (now < last_command_millis) {
    //rollover
    if (DEBUG_SERIAL_OUT) {
      Serial.println("Rollover.");
    }
    last_command_millis = now;
  } else {
    if (now - last_command_millis >= command_timeout) {
      //Assume main is dead - go safe
      commanded_speed=0;
      commanded_direction=1;
      if (DEBUG_SERIAL_OUT) {
        Serial.println("Main is dead? Going safe.");
      }
    }
  }

  //Write out PWM and high/low to motor:
  digitalWrite(M_DIR_PIN, commanded_direction);
  analogWrite(M_PWM_PIN, commanded_speed);
  //Visual indication of throttle on the internal LED.
  analogWrite(LED_BUILTIN, commanded_speed);
  delay(LOOP_DELAY);
  if (DEBUG_SERIAL_OUT) {
    Serial.print("last command: ");
    Serial.print(last_command_millis);
    Serial.print(", ");
    Serial.print(millis() - last_command_millis);
    Serial.println(" ago.");
  }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  if (DEBUG_SERIAL_OUT) {
    Serial.print("Wire event(");
    Serial.print(howMany);
    Serial.print(". ");
  }
  if (howMany >= 2) {
    last_command_millis = millis();
    commanded_direction = Wire.read(); //first byte is direction
    commanded_speed = Wire.read(); //second byte is speed
    if (DEBUG_SERIAL_OUT) {
      Serial.print("Speed commanded: ");
      Serial.println(commanded_speed);
    }
  }
  //flush:
  while(Wire.available()){
    Wire.read();
  }
}
