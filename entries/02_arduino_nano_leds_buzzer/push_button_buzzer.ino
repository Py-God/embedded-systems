// sound a buzzer when a push button is pressed

int value = 0; // return value from push_button 0 or 1
int push_button = 5; // the pin the button bridged 5v would reflect on
int buzzer = 4;


void setup() {
  // because of the natural nature of the microcontroller internal pullup resistor
  // the pin would be naturally low, which can mess up leds such that it is turning on without pressing a button
  // we are now inverting the code to turn on led with a low signal (research more)
  // this would mean the pin would be default high, we then need the button to bridge a low signal (GND) to activate
  // instead of a high 5v, that's why the button is now connected to gnd and not 5v now
  pinMode(push_button, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  value = digitalRead(push_button);

  if (value == LOW) {
    digitalWrite(buzzer, HIGH);
  } else {
    digitalWrite(buzzer, LOW);
  }
}
