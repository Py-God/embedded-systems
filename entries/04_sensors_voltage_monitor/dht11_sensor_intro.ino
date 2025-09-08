#include <DHT11.h>

DHT11 dht11(2);

int temperature;
int humidity;

void setup() {
  Serial.begin(9600);
}

void loop() {
  temperature = dht11.readTemperature();
  humidity = dht11.readHumidity();
  Serial.print(temperature);
  Serial.print(" °C");
  Serial.print(" and ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(1000);
}
