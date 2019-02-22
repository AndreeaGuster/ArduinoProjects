//RFID Solenoid Lock

#include <SPI.h>  // for relay
#include <MFRC522.h>  // for RFID
#include <LiquidCrystal.h> // for LCD
#include <TimeLib.h> // for current date and time

// RFID
#define SS_PIN 10
#define RST_PIN 9
// LED
#define LED_G 5 //define green LED pin
#define LED_R 4 //define red LED pin
// RELAY
#define RELAY 3 //relay pin
#define ACCESS_DELAY 2000
#define DENIED_DELAY 1000
// BUZZER
#define BUZZER 2 //buzzer pin
// LCD
#define LCD_DELAY 500
#define COLUMNS_LCD 16
#define ROWS_LCD 2
#define FIRST_ROW_LCD 0
#define FIRST_COL_LCD 0
#define SECOND_ROW_LCD 1
#define V0_PIN 6 // PWN 
// DATE AND TIME
#define HR 11
#define MN 0
#define SC 0
#define DY 23
#define MT 2
#define YR 19


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
LiquidCrystal lcd(A5, A4, A3, A2, A1, A0);
 
void setup() {
  Serial.begin(9600);                    // Initiate a serial communication
  SPI.begin();                           // Initiate  SPI bus
  lcd.begin(COLUMNS_LCD, ROWS_LCD);      // Initiate  LCD bus
  mfrc522.PCD_Init();                    // Initiate MFRC522
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(V0_PIN, OUTPUT); // PWN 
  noTone(BUZZER);
  setTime(HR, MN, SC, DY, MT, YR);
  digitalWrite(RELAY, HIGH); // modif
  analogWrite(V0_PIN, 90); // PWN 
  delay(LCD_DELAY);

  Serial.println("Put your card to the reader...");
  Serial.println();

}

void digitalClockDisplay(){
  Serial.print("Time: ");
  Serial.print(hour());
  Serial.print(":");  
  Serial.print(minute());
  Serial.print(":"); 
  Serial.print(second());
  Serial.print("  Date: ");  
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year());
  Serial.println();                 
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Put your card to the reader"); 
  lcd.setCursor(FIRST_COL_LCD, SECOND_ROW_LCD);
  lcd.print("the reader...");
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  //Show UID on serial monitor
  Serial.print("UID tag :");
  
  String content = "";
  byte letter;
  
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
 
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
    
  if (content.substring(1) == "35 F9 EC BB") { //change here the UID of the card/cards that you want to give access
    String codeCard = "35 F9 EC BB";
    
    Serial.println("Authorized access");
    digitalClockDisplay();
    Serial.println();

    lcd.clear();
    lcd.setCursor(FIRST_COL_LCD, FIRST_ROW_LCD);
    lcd.print("Access");
    lcd.setCursor(FIRST_COL_LCD, SECOND_ROW_LCD);
    lcd.print("To ID: ");
    lcd.print(codeCard);

    delay(LCD_DELAY * 2);
    digitalWrite(RELAY, LOW);
    digitalWrite(LED_G, HIGH);
    
    delay(ACCESS_DELAY);  // wait 2 seconds
    digitalWrite(RELAY, HIGH);//modif
    digitalWrite(LED_G, LOW);

    delay(LCD_DELAY);
    lcd.clear();
  }
 
  else {
    String code = "60 96 D3 1A"; 
    Serial.println("Access denied");
    digitalClockDisplay();
    Serial.println();

    lcd.clear();
    lcd.setCursor(FIRST_COL_LCD, FIRST_ROW_LCD);
    lcd.print("Access denied");
    lcd.setCursor(FIRST_COL_LCD, SECOND_ROW_LCD);
    lcd.print("To ID: ");
    lcd.print(code);
    
    digitalWrite(LED_R, HIGH);
    tone(BUZZER, 300);
    delay(DENIED_DELAY);
    digitalWrite(LED_R, LOW);
    noTone(BUZZER);

    delay(LCD_DELAY * 2);
    lcd.clear();
  }
}
