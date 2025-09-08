int fire_sensor = 2;

void setup() {
  Serial.begin(9600);

  pinMode(fire_sensor, INPUT);

}

void loop() {
  Serial.println(digitalRead(fire_sensor));

}
