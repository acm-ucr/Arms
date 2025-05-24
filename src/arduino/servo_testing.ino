#include <Servo.h>

Servo servo1;  // create servo object to control a servo
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;

void setup() {
  Serial.begin(9600);
  pinMode(3, INPUT);
  pinMode(6, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(5, INPUT);
  servo1.attach(3);  // attaches the servo on pin 9 to the servo object
  servo2.attach(6);
  servo3.attach(9);
  servo4.attach(10);
  servo5.attach(11);
  servo6.attach(5);
}

void loop() {
    // input angle through serial monitor
    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      int angle = input.toInt();
      servo1.write(angle);
      servo2.write(angle);
      servo3.write(angle);
      // servo4.write(input);
      // servo5.write(input);
      // servo6.write(input);

    }

// running all 6
 int angle1 = 0;
 int angle2 = 180;
//   servo1.write(angle1);
//   servo2.write(angle1);
//   servo3.write(angle1);
  servo4.write(angle1);
  servo5.write(angle1);
  servo6.write(angle1);
  delay(1000);
//   servo1.write(angle2);
//   servo2.write(angle2);
//   servo3.write(angle2);
  servo4.write(angle2);
  servo5.write(angle2);
  servo6.write(angle2);
  delay(1000);
}
