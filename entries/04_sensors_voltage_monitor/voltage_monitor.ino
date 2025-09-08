int sensor = A0;
int bat = 0;
float out_voltage = 0.00;
float in_voltage = 0.00;
int r1 = 1000;
int r2 = 2000;


void setup() {
  // put your setup code here, to run once:
pinMode(sensor,INPUT);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
bat = analogRead(sensor);
out_voltage = (bat/1023.0) * 4.79;
in_voltage = ((r1 + r2)/r1) * out_voltage;
Serial.println(in_voltage);
delay(500);
}
