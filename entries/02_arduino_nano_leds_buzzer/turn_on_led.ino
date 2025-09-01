int led_green = 7;  // initialize what pin you want to use, and what you would use it for
int led_yellow = 6;
int led_red = 5;


void setup() {
  pinMode(led_green, OUTPUT);  // configure led variable as a pin, and make it an output
  pinMode(led_yellow, OUTPUT);
  pinMode(led_red, OUTPUT);
}

void loop() {
  digitalWrite(led_red, HIGH);  // led 5 is a digital pin, so we send an high 5v signal there
  delay(100);              // it is measured in mS

  digitalWrite(led_yellow, HIGH);
  digitalWrite(led_red, LOW);
  delay(100);

  digitalWrite(led_green, HIGH);
  digitalWrite(led_yellow, LOW);
  delay(100);

  digitalWrite(led_green, LOW);
}
