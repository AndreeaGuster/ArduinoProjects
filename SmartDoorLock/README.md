# Smart Door Lock Arduino Uno #
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
