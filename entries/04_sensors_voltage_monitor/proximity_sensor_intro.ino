int proximity_sensor = 3;

void setup() {
  Serial.begin(9600);

  pinMode(proximity_sensor, INPUT);
}

void loop() {
  Serial.println(digitalRead(proximity_sensor));
}
