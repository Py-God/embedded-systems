int gas_sensor = A0;
int gas_sensor_value;
int percentage_gas_sensor_value;
int buzzer = 3;

void setup() {
  pinMode(gas_sensor, INPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  gas_sensor_value = analogRead(gas_sensor);
  if (gas_sensor_value < 240) {
    gas_sensor_value = 0;
  }
  percentage_gas_sensor_value = map(gas_sensor_value, 0, 1023, 0, 100);

  Serial.print(percentage_gas_sensor_value);
  Serial.println("%");

  if (percentage_gas_sensor_value > 50) {
    digitalWrite(buzzer, HIGH);
  } else {
    digitalWrite(buzzer, LOW);
  }
}
