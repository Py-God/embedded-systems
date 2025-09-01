#include <LiquidCrystal_I2C.h>

int btn1 = 4;
int btn2 = 5;
int btn3 = 6;

int led = 7;
int buzzer = 8;

int digits_pressed = 0;
int attempts = 0;
int attempts_allowed = 3;
int password = 3;  // 21131

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  lcd.init();

  lcd.backlight();
  
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("PASSWORD BASED");
  lcd.setCursor(0, 1);
  lcd.print("SECURITY SYSTEM.");
  delay(4000);
  lcd.clear();
}

void loop() {
  if (attempts == 3) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ATTEMPTS");
    lcd.setCursor(0, 1);
    lcd.print("EXCEEDED");
    
    digitalWrite(buzzer, HIGH);
  } else if (attempts < 3) {
    // increment password when a button is clicked. display clicked button, increment a digit pressed counter
    if (digitalRead(btn1) == LOW) {
      password *= 8;
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("BTN1 CLICKED");
      
      digits_pressed++;
    } else if (digitalRead(btn2) == LOW) {
      password *= 7;
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("BTN2 CLICKED");
      
      digits_pressed++;
    } else if (digitalRead(btn3) == LOW) {
      password++;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("BTN3 CLICKED");
      
      digits_pressed++;
    }

    // if wrong password is typed, display warning, reset password, and increment attempts
    if (digits_pressed == 5 && password != 10760) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PASSWORD");
      lcd.setCursor(0, 1);
      lcd.print("INCORRECT!");
      delay(2000);
      lcd.clear();

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

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ATTEMPTS:");
      lcd.setCursor(0, 1);
      lcd.print(attempts);
      lcd.setCursor(1, 1);
      lcd.print("/");
      lcd.setCursor(2, 1);
      lcd.print(attempts_allowed);

      delay(2000);
      lcd.clear();
    }

    // if password is correct, light up led, reset password and attempts. else led is low, and nothing is tampered with
    if (password == 10760) {
      digitalWrite(led, HIGH);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PASSWORD");
      lcd.setCursor(0, 1);
      lcd.print("CORRECT!");

      delay(4000);
      lcd.clear();

      digits_pressed = 0;
      password = 3;
      attempts = 0;
    } else {
      digitalWrite(led, LOW);
    }

    delay(200);
    lcd.clear();
  }
}
