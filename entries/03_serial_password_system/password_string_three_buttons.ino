int btn1 = 4;
int btn2 = 5;
int btn3 = 6;

int led = 7;
int buzzer = 8;

String password = "11321";
String input_value = "";


void setup() {
  Serial.begin(9600);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  if (digitalRead(btn1) == LOW) {
    Serial.println("Button 1 clicked!");
    input_value.concat("1");
  } else if (digitalRead(btn2) == LOW) {
    Serial.println("Button 2 clicked!");
    input_value.concat(("2"));
  } else if (digitalRead(btn3) == LOW) {
    Serial.println("Button 3 clicked!");
    input_value.concat("3");
  }

  if (input_value == password) {
    Serial.println("Password correct!");
    digitalWrite(led, HIGH);
    delay(200);
    digitalWrite(led, LOW);

    input_value = "";
  }

  delay(300);
}
