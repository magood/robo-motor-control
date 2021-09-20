#include <Wire.h>

byte commanded_speed = 0;
byte commanded_direction = 1; //1 for forward, 0 for reverse
unsigned long last_command_millis = 0; // millis() value the last time a command was recieved from the main.
unsigned long command_timeout = 8000; //how long between commands before we assume the main is dead?

const byte M_PWM_PIN = M_PWM_PIN;
const byte M_DIR_PIN = 9;

// the setup function runs once when you press reset or power the board
void setup() {
  //Speed/direction pin setup
  pinMode(M_PWM_PIN, OUTPUT);
  analogWrite(M_PWM_PIN, 0);
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
    Serial.println("Rollover.");
    last_command_millis = now;
  } else {
    if (now - last_command_millis >= command_timeout) {
      //Assume main is dead - go safe
      Serial.println("Main is dead? Going safe.");
      commanded_speed=0;
      commanded_direction=1;
    }
  }

  //Write out PWM and high/low to motor:
  digitalWrite(M_DIR_PIN, commanded_direction);
  analogWrite(M_PWM_PIN, commanded_speed);
  delay(1000);
  Serial.print("last command: ");
  Serial.print(last_command_millis);
  Serial.print(", ");
  Serial.print(millis() - last_command_millis);
  Serial.println(" ago.");
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  Serial.print("Wire event(");
  Serial.print(howMany);
  Serial.print(". ");
  if (howMany >= 2) {
    last_command_millis = millis();
    commanded_direction = Wire.read(); //first byte is direction
    commanded_speed = Wire.read(); //second byte is speed
    Serial.print("Speed commanded: ");
    Serial.println(commanded_speed);
  }
  //flush:
  while(Wire.available()){
    Wire.read();
  }
}
