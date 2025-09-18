#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <SPI.h>
#include <WiFi.h>
#include <Wire.h>
#include <Firebase_ESP_Client.h>
#include <addons/RTDBHelper.h>

// WiFi Credentials
#define WIFI_SSID "SMART WASTE"
#define WIFI_PASSWORD

// Firebase Credentials
#define API_KEY
#define DATABASE_URL
#define USER_EMAIL "boluwatifelekeoduoye@gmail.com"
#define USER_PASSWORD

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Hardware pins
#define SS_PIN 5
#define RST_PIN 4

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid_scanner(SS_PIN, RST_PIN);
Servo myservo;

int no_button = 12;
int yes_button = 13;
int buzzer = 15;  // CHANGED from pin 4 to avoid conflict with RST_PIN
int led = 2;
int proximity_sensor_inside = 25;
int proximity_sensor_outside = 26;

int counter = 0;
int reward = 2500;
int total_reward;
String WImg_Url="https://";
// State Machine States
enum State {
  STATE_IDLE,
  STATE_CARD_SCANNED,
  STATE_FETCHING_USER,
  STATE_DISPLAY_USER,
  STATE_DROP_BOTTLE,
  STATE_UPDATE_FIREBASE
};
State currentState = STATE_IDLE;

// User data structure
struct UserData {
  String name;
  int balance;
  String img_url;
  bool exists;
};
UserData currentUser;
String currentUID = "";

// Non-blocking timers
unsigned long lastStateTime = 0;
unsigned long lastWiFiCheck = 0;
unsigned long lastIdleUpdate = 0;
const int wifiCheckInterval = 5000;
const int idleUpdateInterval = 1000;

void connectToWiFi();
void initFirebase();
void checkWiFiConnection();
UserData getUserData(String uid);
void updateUserData(String uid, int newBalance, int bottlesDropped);
void errorFeedback();
void beepBuzzer(int times);
void bottle_drop();
void changeState(State newState);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid_scanner.PCD_Init();
  lcd.init();
  lcd.backlight();

  myservo.attach(14);
  myservo.write(180);

  pinMode(no_button, INPUT_PULLUP);
  pinMode(yes_button, INPUT_PULLUP);

  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(proximity_sensor_outside, INPUT);
  pinMode(proximity_sensor_inside, INPUT);

  lcd.setCursor(0, 0);
  lcd.print("WASTE MANAGEMENT");
  lcd.setCursor(0, 1);
  lcd.print(" REWARD SYSTEM. ");
  delay(2000);

  connectToWiFi();
  initFirebase();

  lcd.clear();
  lcd.print("SYSTEM READY!   ");
  delay(1000);
  
  changeState(STATE_IDLE);
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Check WiFi periodically without blocking
  if (currentMillis - lastWiFiCheck >= wifiCheckInterval) {
    lastWiFiCheck = currentMillis;
    checkWiFiConnection();
  }

  // State Machine
  switch (currentState) {
    case STATE_IDLE:
      idleState();
      break;

    case STATE_FETCHING_USER:
      fetchingUserState();
      break;

    case STATE_DISPLAY_USER:
      displayUserState(currentMillis);
      break;

    case STATE_DROP_BOTTLE:
      bottle_drop();
      break;

    case STATE_UPDATE_FIREBASE:
      updateFirebaseState();
      break;
  }

  // Small delay to prevent watchdog timer issues
  delay(10);
}

void idleState() {
  unsigned long currentMillis = millis();
  
  // Update idle message without spamming LCD
  if (currentMillis - lastIdleUpdate >= idleUpdateInterval) {
    lastIdleUpdate = currentMillis;
    lcd.setCursor(0, 0);
    lcd.print("  PLEASE SCAN   ");
    lcd.setCursor(0, 1);
    lcd.print("   YOUR CARD.   ");
  }
  
  // Check for RFID card quickly
  if (rfid_scanner.PICC_IsNewCardPresent() && rfid_scanner.PICC_ReadCardSerial()) {
    Serial.println("SCANNED----");
    
    // Read card UID
    currentUID = "";
    for (byte i = 0; i < rfid_scanner.uid.size; i++) {
      currentUID += String(rfid_scanner.uid.uidByte[i] < 0x10 ? "0" : "");
      currentUID += String(rfid_scanner.uid.uidByte[i], HEX);
    }
    currentUID.toUpperCase();

    digitalWrite(buzzer, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("  CARD SCANNED! ");
    lcd.setCursor(0, 1);
    lcd.print(currentUID);
        lcd.print("          ");
    delay(200);
    digitalWrite(buzzer, LOW);
    delay(1000);
    changeState(STATE_FETCHING_USER);
  }
}

void fetchingUserState() {
  lcd.clear();
  lcd.print(" CHECKING USER  ");
  lcd.setCursor(0, 1);
  lcd.print("  IN DATABASE...");
  
  currentUser = getUserData(currentUID);
  
  if (!currentUser.exists) {
    lcd.clear();
    lcd.print("    USER NOT    ");
    lcd.setCursor(0, 1);
    lcd.print("     FOUND      ");
    beepBuzzer(3);
    delay(2000);
    changeState(STATE_IDLE);
  } else {
    changeState(STATE_DISPLAY_USER);
  }
}

void displayUserState(unsigned long currentMillis) {
  lcd.setCursor(0, 0);
  // lcd.clear();
  lcd.print("WELCOME:        ");
  lcd.setCursor(0, 1);
  String displayName = currentUser.name;
  if (displayName.length() > 16) {
    displayName = displayName.substring(0, 16);
  }
  lcd.print(displayName);
      lcd.print("                  ");
  
  if (currentMillis - lastStateTime >= 2000) {
    changeState(STATE_DROP_BOTTLE);
  }
}

void updateFirebaseState() {
  lcd.clear();
  lcd.print(" UPDATING CLOUD ");
  lcd.setCursor(0, 1);
  lcd.print("  PLEASE WAIT...");
  
  total_reward = reward * counter;
  int newBalance = currentUser.balance + total_reward;
  updateUserData(currentUID, newBalance, counter);
  
  lcd.clear();
  lcd.print("   DATA LOGGED  ");
  lcd.setCursor(0, 1);
  lcd.print(" SUCCESSFULLY!  ");
  delay(1000);

  lcd.clear();
  lcd.print(" BOTTLE COUNT:  ");
  lcd.setCursor(0, 1);
  lcd.print("      ");
  lcd.print(counter);
  lcd.print("        ");
  delay(1000);

  lcd.clear();
  lcd.print("  TOTAL REWARD: ");
  lcd.setCursor(0, 1);
  lcd.print("   ");
  lcd.print(total_reward);
  lcd.print(" UNITS ");
  delay(1000);

  lcd.clear();
  lcd.print("  NEW BALANCE:  ");
  lcd.setCursor(0, 1);
  lcd.print("   ");
  lcd.print(newBalance);
  lcd.print(" UNITS ");
  delay(2000);

  counter = 0;
  changeState(STATE_IDLE);
}

void changeState(State newState) {
  currentState = newState;
  lastStateTime = millis();
}

void bottle_drop() {
  lcd.setCursor(0, 0);
  lcd.print("  PLEASE DROP   ");
  lcd.setCursor(0, 1);
  lcd.print("  YOUR BOTTLE.  ");

  if (digitalRead(proximity_sensor_inside) == HIGH && digitalRead(proximity_sensor_outside) == LOW) {
    lcd.setCursor(0, 0);
    lcd.print("BOTTLE DROPPED! ");
    lcd.setCursor(0, 1);
    lcd.print("                ");

    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);

    myservo.write(0);
    delay(1500);
    myservo.write(180);

    counter++;

    lcd.setCursor(0, 0);
    lcd.print("     BOTTLE     ");
    lcd.setCursor(0, 1);
    lcd.print("   RECORDED!    ");
    delay(1000);

    // Check if user wants to add another bottle
    lcd.setCursor(0, 0);
    lcd.print("ANOTHER BOTTLE? ");
    lcd.setCursor(0, 1);
    lcd.print("YES           NO");
delay(1000);
    unsigned long startTime = millis();
    while (millis() - startTime < 10000) { // 10-second timeout
      if (digitalRead(no_button) == LOW) {
        changeState(STATE_UPDATE_FIREBASE);
        return;
      }
      if (digitalRead(yes_button) == LOW) {
        // Reset to drop another bottle
        return; // Stay in DROP_BOTTLE state
      }
      delay(50);
    }
    
    // Timeout, proceed to update
    changeState(STATE_UPDATE_FIREBASE);
  }
}

void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lcd.clear();
  lcd.print("WIFI: CONNECTING");
  Serial.println("WiFi: Connecting");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    lcd.setCursor(0, 1);
    lcd.print("ATTEMPT " + String(attempts + 1));
    Serial.println("WiFi: Attempt " + String(attempts + 1));
    delay(500);
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    lcd.clear();
    lcd.print("WIFI FAILED!");
    Serial.println("WiFi: Connection failed!");
    errorFeedback();
    ESP.restart();
  }

  lcd.clear();
  lcd.print("WIFI CONNECTED!");
  Serial.println("WiFi: Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
}

void initFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  lcd.clear();
  lcd.print("FIREBASE: INIT");
  Serial.println("Firebase: Initializing");
  delay(500);

  if (Firebase.ready()) {
    lcd.clear();
    lcd.print("FIREBASE: READY");
    Serial.println("Firebase: Ready");
    delay(1000);
  } else {
    lcd.clear();
    lcd.print("FIREBASE: ERROR");
    Serial.println("Firebase: Connection failed");
    errorFeedback();
    delay(2000);
  }
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    lcd.clear();
    lcd.print("WIFI DISCONNECTED");
    lcd.setCursor(0, 1);
    lcd.print("RECONNECTING...");
    Serial.println("WiFi disconnected. Reconnecting...");
    connectToWiFi();
  }
}

UserData getUserData(String uid) {
  UserData user;
  user.exists = false;

  checkWiFiConnection();

  String path = "polyib_elect/Smart_Waste/users/" + uid;

  if (Firebase.ready() && Firebase.RTDB.getJSON(&fbdo, path)) {
    if (fbdo.dataType() == "json") {
      FirebaseJson *json = fbdo.jsonObjectPtr();
      FirebaseJsonData jsonData;

      if (json->get(jsonData, "name")) {
        user.name = jsonData.stringValue;
        user.exists = true;
      }

      if (json->get(jsonData, "balance")) {
        user.balance = jsonData.intValue;
      }

      // if (json->get(jsonData, "Img_Url")) {
      //   user.img_url = jsonData.stringValue;
      // }
    }
  }

  return user;
}

void updateUserData(String uid, int newBalance, int bottlesDropped) {
  checkWiFiConnection();

  String patha = "polyib_elect/Smart_Waste/";

  if (Firebase.ready() && Firebase.RTDB.getJSON(&fbdo, patha)) {
    if (fbdo.dataType() == "json") {
      FirebaseJson *json = fbdo.jsonObjectPtr();
      FirebaseJsonData jsonData;

      if (json->get(jsonData, "Img_Url")) {
        WImg_Url = jsonData.stringValue;
      }
    }
    }

  String path = "polyib_elect/Smart_Waste/users/" + uid;

  if (Firebase.ready()) {
    Firebase.RTDB.setInt(&fbdo, path + "/balance", newBalance);
  }

  String logPath = "polyib_elect/Smart_Waste/logs/" + String(millis());
  if (Firebase.ready()) {
    FirebaseJson json;
    json.set("DateTime", Firebase.getCurrentTime());

    UserData user = getUserData(uid);
    json.set("Name", user.name);
    json.set("UID", uid);
    json.set("Bottles_Dropped", bottlesDropped);
    json.set("Img_Url", WImg_Url);

    Firebase.RTDB.setJSON(&fbdo, logPath, &json);
  }
}

void errorFeedback() {
  digitalWrite(buzzer, HIGH);
  delay(300);
  digitalWrite(buzzer, LOW);
  delay(300);
}

void beepBuzzer(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);
    delay(200);
  }
}