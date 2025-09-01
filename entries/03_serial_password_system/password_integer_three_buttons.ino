int btn1 = 4;
int btn2 = 5;
int btn3 = 6;

int led = 7;
int buzzer = 8;

int digits_pressed = 0;
int attempts = 0;
int attempts_allowed = 3;
int password = 3;  // 21131

// if password is incorrect beep buzzer three times
// if password is incorrect and attempts have been reached, break out of program and keep beeping buzzer
// make your prints readable: change the button pin prints. change the admin message and the like

void setup() {
  Serial.begin(9600);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  if (attempts == 3) {
    Serial.println("Attempts exceeded.");
    digitalWrite(buzzer, HIGH);
  } else if (attempts < 3) {
    // increment password when a button is clicked. display clicked button, increment a digit pressed counter
    if (digitalRead(btn1) == LOW) {
      password *= 8;
      
      Serial.print("button ");
      Serial.print(1);
      Serial.println(" clicked!");
      
      digits_pressed++;
    } else if (digitalRead(btn2) == LOW) {
      password *= 7;
      
      Serial.print("button ");
      Serial.print(2);
      Serial.println(" clicked!");
      
      digits_pressed++;
    } else if (digitalRead(btn3) == LOW) {
      password++;

      Serial.print("button ");
      Serial.print(3);
      Serial.println(" clicked!");
      
      digits_pressed++;
    }

    // if wrong password is typed, display warning, reset password, and increment attempts
    if (digits_pressed == 5 && password != 10760) {
      Serial.println("Password incorrect.");

      digitalWrite(buzzer, HIGH);
      delay(100);
      digitalWrite(buzzer, LOW);
      delay(100);
      digitalWrite(buzzer, HIGH);
      delay(100);
      digitalWrite(buzzer, LOW);
      delay(100);
      digitalWrite(buzzer, HIGH);
      delay(100);
      digitalWrite(buzzer, LOW);
      delay(100);

      password = 3;
      attempts++;
      digits_pressed = 0;

      Serial.print("attempt: ");
      Serial.print(attempts);
      Serial.print("/");
      Serial.println(attempts_allowed);
    }

    // if password is correct, light up led, reset password and attempts. else led is low, and nothing is tampered with
    if (password == 10760) {
      digitalWrite(led, HIGH);
      Serial.println("Password correct!");
      delay(5000);
      digitalWrite(led, LOW);

      digits_pressed = 0;
      password = 3;
      attempts = 0;
    } else {
      digitalWrite(led, LOW);
    }

    delay(200);
  }
}
