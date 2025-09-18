# 📒 Project Notes – Smart Waste Management System

## 📝 Overview
This project implements a **Smart Waste Management System** that rewards students for recycling bottles.  
It uses an **ESP32** with RFID authentication, proximity sensors, a servo-controlled lid, and Firebase cloud integration.

Students scan their card, drop bottles, and earn units (rewards) which are logged to the cloud.  
The system uses a **state machine** to handle different operational stages in a structured manner.

---

## ⚙️ Components & Hardware
- **ESP32 microcontroller**
- **MFRC522 RFID reader**
- **16x2 I2C LCD display**
- **ESP32Servo (servo motor)**
- **Proximity sensors** (inside & outside bin)
- **Yes/No push buttons**
- **Buzzer & LED indicator**
- **Firebase Realtime Database (cloud logging)**

---

## 🔑 Features
- RFID-based user authentication  
- LCD status messages and feedback  
- Servo motor for opening/closing bin lid  
- Bottle detection using dual proximity sensors  
- Reward calculation (per bottle = 2500 units)  
- Firebase cloud integration:
  - Fetch user details
  - Update balance
  - Log recycling sessions with timestamp  

---

## 🖥️ State Machine Flow
The project uses an **enum-based state machine**:

1. **`STATE_IDLE`** – Waiting for RFID card scan  
2. **`STATE_CARD_SCANNED`** – Card detected, UID captured  
3. **`STATE_FETCHING_USER`** – Verify user in Firebase  
4. **`STATE_DISPLAY_USER`** – Display user name on LCD  
5. **`STATE_DROP_BOTTLE`** – Monitor bottle drop & increment counter  
6. **`STATE_UPDATE_FIREBASE`** – Upload bottle count & new balance to Firebase  

---

## 📡 Firebase Database Structure
Users and logs are stored in Firebase like this:

```json
polyib_elect/Smart_Waste/
├── users/
│   ├── <UID1>/
│   │   ├── name: "John Doe"
│   │   ├── balance: 10000
│   │   └── Img_Url: "https://..."
│   └── <UID2>/
│       ├── name: "Jane Doe"
│       ├── balance: 7500
│       └── Img_Url: "https://..."
│
└── logs/
    ├── 1694523000/
    │   ├── DateTime: 1694523000
    │   ├── Name: "John Doe"
    │   ├── UID: "AB12CD34"
    │   ├── Bottles_Dropped: 3
    │   └── Img_Url: "https://..."
