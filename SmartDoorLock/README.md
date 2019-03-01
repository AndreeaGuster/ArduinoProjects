# Smart Door Lock Arduino Uno #
## Description ##
It's a smart securization based project. Using the RFID tag you can open the door lock or not, depending on the configuration of the card. When access is denied the red LED lights up, and warning sound is released. If you have access, the lock opens and the green LED lights up. 
On both Serial Monitor and LCD a message will be printed to inform you if you have access or not.
Also, on Serial Monitor you can see what was the time when your card was scanned.
## Video ##
Link: https://youtu.be/jBvWtaaC1QA
## Components ##
* Arduino Uno
* breadboard
* passive buzzer
* 2 LEDs (green and red)
* DC 12V Cabinet Door Lock Electric Lock Assembly Solenoid 
* 5V Relay Module
* RFID-RC522 Reader
* 9V battery
* jumper wires

## Wairing ##
### Relay Module ###
* GND (ground) -> GND pin
* IN (low/high output) -> ~3 pin
* VCC -> 5V pin 

### RFID-RC522 ###
* 3.3V -> 3.3V pin
* RST -> ~9 pin
* GND -> GND pin
* IRQ 
* MOSI -> ~11 pin
* MISO -> ~12 pin
* SCK -> 13 pin
* SDA -> ~10 pin

### BUZZER ###
* (+) -> 2 pin
* GND

### RED LED ###
* 4 pin
* GND

### GREEN LED ###
* ~5 pin
* GND

### POTENTIOMETER ###
* 5V
* V0 from LCD
* GND

### LCD ###
* K -> GND pin
* A -> 5V pin
* D7 -> A0 pin
* D6 -> A1 pin
* D5 -> A2 pin
* D4 -> A3 pin
* E -> A4 pin
* RW -> GND
* RS -> A5
* V0 -> middle pin from potentiometer
* VDD -> 5V
* VSS -> GND
Note: Don't use 0 or 1 pins, because they will interfere with Serial Monitor.
