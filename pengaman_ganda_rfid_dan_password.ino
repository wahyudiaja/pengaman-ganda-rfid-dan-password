// Not:
// Lengkapi librari yang di perlukan pada tanda #include <librarinya.h>
// Silahkan sesuaikan kembali dengan IO yang digunakan pada arduino kalian jika menggunakan selain arduino mega
// Silahkan sesuaikan uncomment jika menggunakan servo atau selenoid door lock
// Jangan lupa di SUBSCRIBE LOgic Solution :)


#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include "pitches.h"
//#include <Servo.h>
#include <SPI.h>
#include <Wire.h>

//int i=0;

// Create instances
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 mfrc522(9, 8); // MFRC522 mfrc522(SS_PIN, RST_PIN)
  //Servo sg90;
  // Initialize Pins for led's, servo and buzzer
  // Blue LED is connected to 5V
constexpr uint8_t greenLed = 2;
constexpr uint8_t redLed = 3;
constexpr uint8_t doorLock = 6;
  //constexpr uint8_t servoPin = 4;
constexpr uint8_t buzzerPin = 5;

//++++++
int masukkan_password_melody[] = {NOTE_G4,0,NOTE_A4,0, NOTE_B4,0,NOTE_A4,0,NOTE_B4,0, NOTE_C5,0};
int masukkan_password_noteDurations[] = {8,8,8,8,8,4,8,8,8,8,8,4};
//++++++

char initial_password[4] = {'1', '2', '3', '4'};  // Variable to store initial password
String tagUID1 = "60 34 8F 1E";  // String to store UID of tag. Change it with your tag's UID
String tagUID2 = "E9 1C BE 5A"; //gantungan kunci
String tagUID3 = "04 52 4C 5A 10 2A 70";//ktp
//kalau kurang tambahkan di bawahnya
char password[4];   // Variable to store users password
boolean RFIDMode = true; // boolean to change modes
char key_pressed = 0; // Variable to store incoming keys
uint8_t i = 0;  // Variable used for counter

// defining how many rows and columns our keypad have
const byte rows = 4;
const byte columns = 4;

//+++++
//const int red = 11;//pin yang digunakan
//++++

// Keypad pin map
char hexaKeys[rows][columns] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
// Initializing pins for keypad
byte row_pins[rows] = {23, 25, 27, 29};
byte column_pins[columns] = {31, 33, 35, 37};

// Create instance for keypad
Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);

void setup() {
  // Arduino Pin configuration
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(doorLock, OUTPUT);
  digitalWrite(doorLock, HIGH);
  delay(500);
  digitalWrite(doorLock,LOW);
  delay(500);
  //+++++
  //+++++
  //sg90.attach(servoPin);  //Declare pin 8 for servo
  //sg90.write(0); // Set initial position at 90 degrees
  
  lcd.begin();   // LCD screen
  lcd.backlight();
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  lcd.clear(); // Clear LCD screen
}

void loop() {

  // System will first look for mode
  if (RFIDMode == true) {  
    
    int i; 
    lcd.setCursor(0, 0);
    lcd.print("-Door Lock- >LS<");
    lcd.setCursor(0, 1);
    lcd.print("-Scan Your Tag- ");

    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }
    //Reading from the card
    String tag = "";
    for (byte j = 0; j < mfrc522.uid.size; j++)
    {
      tag.concat(String(mfrc522.uid.uidByte[j] < 0x10 ? " 0" : " "));
      tag.concat(String(mfrc522.uid.uidByte[j], HEX));
    }
    tag.toUpperCase();
    //Checking the card
    if (tag.substring(1) == tagUID1, tagUID2, tagUID3)
    
    {
      // If UID of tag is matched.
      lcd.clear();
      lcd.print("Tag Matched");
      digitalWrite(greenLed, HIGH);
      delay(1000);
      digitalWrite(greenLed, LOW);
      lcd.clear();
      lcd.print("Enter Password:");
      lcd.setCursor(0, 1);
      RFIDMode = false; // Make RFID mode false
     
      for (int i = 0; i < 6; i++){
      int masukkan_password_noteDuration = 500/masukkan_password_noteDurations[i];
      tone(buzzerPin, masukkan_password_melody[i],masukkan_password_noteDuration);
      int masukkan_password_pauseBetweenNotes = masukkan_password_noteDuration * 1.30;
      delay(masukkan_password_pauseBetweenNotes);
      noTone(buzzerPin);
      }
      
    }
    else
    {
      // If UID of tag is not matched.
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wrong Tag Shown");
      lcd.setCursor(0, 1);
      lcd.print("Access Denied");
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(redLed, HIGH);
      delay(3000);
      digitalWrite(buzzerPin, LOW);
      digitalWrite(redLed, LOW);
      lcd.clear();
    }
  }
  // If RFID mode is false, it will look for keys from keypad
  if (RFIDMode == false) {
    key_pressed = keypad_key.getKey(); // Storing keys
    if (key_pressed)
    {
      password[i++] = key_pressed; // Storing in password variable
      lcd.print("*");
    }
    if (i == 4) // If 4 keys are completed
    {
      delay(200);
      if (!(strncmp(password, initial_password, 4))) // If password is matched
      {
        lcd.clear();
        lcd.print("Pass Accepted");
        digitalWrite(doorLock, HIGH);
        //sg90.write(90); // Door Opened
        lcd.setCursor (0,1);
        lcd.print(F("AutoLock after "));
        for(int i=5; i>0; i--){
        lcd.setCursor (15,1); lcd.print(i);
        delay (1000);
        }
        digitalWrite(greenLed, HIGH);
        delay(3000);
        digitalWrite(greenLed, LOW);
        digitalWrite(doorLock, LOW);
        //sg90.write(0); // Door Closed
        lcd.clear();
        i = 0;
        RFIDMode = true; // Make RFID mode true
      }
      else    // If password is not matched
      {
        lcd.clear();
        lcd.print("Wrong Password");
        digitalWrite(buzzerPin, HIGH);
        digitalWrite(redLed, HIGH);
        delay(3000);
        digitalWrite(buzzerPin, LOW);
        digitalWrite(redLed, LOW);
        lcd.clear();
        i = 0;
        RFIDMode = true;  // Make RFID mode true
      }
    }
  }
}
