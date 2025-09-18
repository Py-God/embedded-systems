#include <Servo.h>

Servo myservo;


void setup() {
  myservo.attach(180);

  myservo.write(90);
  delay(15);
}

void loop() {
  
}
