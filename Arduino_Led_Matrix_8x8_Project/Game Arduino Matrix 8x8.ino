#include "LedControl.h"                  //  need the library for led matrix

#define MAX_NUMBER_OF_ROCKETS  20        /* ROCKETS */
#define ALL_ROCKETS_HAVE_BEEN_USED -1
#define MAX_NUMBER_OF_METEORS 64         /* METEORS */
#define ALL_MOTEORS_HAVE_BEEN_USED -1                    
#define SIZE_MESSAGE_START 15         /* size of string "START NEW GAME" */
#define SIZE_MESSAGE_GAME_OVER 10     /* size of string "GAME OVER" */
#define SIZE_MATRIX 8                 /* number of rows = 8, number of columns = 8 */
#define HEARTS 3                     /* number of lives for a game*/

int buzzerPin = 9,           /* BUZZER */
    rightFireButton = A0,    /* BUTTON */
    leftFireButton = A1,     /* BUTTON */
    leftButton = A2,         /* BUTTON */
    rightButton = A3,        /* BUTTON */
    ledPin = 13,             /* LED - the number of the LED pin */
    lives = HEARTS;          /* the player starts the game with 3 lives */

boolean GAME_OVER = true;    //state of game

LedControl lc = LedControl(12, 11, 10, 1);  /* LED MATRIX */
                                            /* DIN, CLK, LOAD, No. DRIVER
                                               pin 12 is connected to the MAX7219 pin 1
                                               pin 11 is connected to the CLK pin 13
                                               pin 10 is connected to LOAD pin 12
                                               1 as we are only using 1 MAX7219
                                               */

/******************** MESSAGES ************************/
int gameOverMessage[SIZE_MESSAGE_GAME_OVER][SIZE_MATRIX] = {
  {0x00, 0x3c, 0x40, 0x40, 0x48, 0x44, 0x38, 0x00}, // G
  {0x00, 0x38, 0x44, 0x7c, 0x44, 0x44, 0x44, 0x00}, // A
  {0x00, 0x44, 0x6c, 0x54, 0x44, 0x44, 0x44, 0x00}, // M
  {0x00, 0x7c, 0x40, 0x78, 0x40, 0x40, 0x7c, 0x00}, // E
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  (space)
  {0x00, 0x38, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00}, // O
  {0x00, 0x44, 0x44, 0x44, 0x44, 0x28, 0x10, 0x00}, // V
  {0x00, 0x7c, 0x40, 0x78, 0x40, 0x40, 0x7c, 0x00}, // E
  {0x00, 0x30, 0x48, 0x48, 0x70, 0x48, 0x44, 0x00}, // R
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  (space)
};

int startNewGameMessage[SIZE_MESSAGE_START][SIZE_MATRIX] = {
  {0x00, 0x3c, 0x40, 0x38, 0x04, 0x44, 0x38, 0x00}, // S
  {0x00, 0x7c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00}, // T
  {0x00, 0x38, 0x44, 0x7c, 0x44, 0x44, 0x44, 0x00}, // A
  {0x00, 0x30, 0x48, 0x48, 0x70, 0x48, 0x44, 0x00}, // R
  {0x00, 0x7c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00}, // T
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  (space)
  {0x00, 0x44, 0x64, 0x54, 0x4c, 0x44, 0x44, 0x00}, // N
  {0x00, 0x7c, 0x40, 0x78, 0x40, 0x40, 0x7c, 0x00}, // E
  {0x00, 0x00, 0x44, 0x54, 0x54, 0x54, 0x38, 0x00}, // W
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  (space)
  {0x00, 0x3c, 0x40, 0x40, 0x48, 0x44, 0x38, 0x00}, // G
  {0x00, 0x38, 0x44, 0x7c, 0x44, 0x44, 0x44, 0x00}, // A
  {0x00, 0x44, 0x6c, 0x54, 0x44, 0x44, 0x44, 0x00}, // M
  {0x00, 0x7c, 0x40, 0x78, 0x40, 0x40, 0x7c, 0x00}, // E
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  (space)
};


/******************** ROCKETS ************************/
struct Rocket {
  int coordX;
  int coordY;
  long unsigned int updateTime;

  boolean doesHitTarget(int, int);
};

Rocket ROCKET_NONE = {0, 0, 0};                     //initialize struct fields with 0
Rocket activeRockets[MAX_NUMBER_OF_ROCKETS] = {};   //initialize activeRockets array with 0

boolean Rocket::doesHitTarget(int targetCoordX, int targetCoordY) {
  return (coordX == targetCoordX && coordY == targetCoordY);
}

boolean isRocketNone(struct Rocket rocket) {
  return ( rocket.coordX == ROCKET_NONE.coordX &&
           rocket.coordY == ROCKET_NONE.coordY &&
           rocket.updateTime == ROCKET_NONE.updateTime
         );
}

int findRocketID() {
  for (int i = 0; i < MAX_NUMBER_OF_ROCKETS; i++) {
    if (isRocketNone(activeRockets[i])) {
      return i;
    }
  }
  return ALL_ROCKETS_HAVE_BEEN_USED;
}

void deleteRocket(int rocketID) {
  activeRockets[rocketID] = ROCKET_NONE;
}

void drawRockets() {
  Rocket rocket;

  for (int rocketID = 0; rocketID < MAX_NUMBER_OF_ROCKETS; rocketID++) {
    rocket = activeRockets[rocketID];

    if (!isRocketNone(rocket)) {  // if (!isRocketNone(rocket))
      lc.setLed(0, rocket.coordX, rocket.coordY, true);

      if (rocket.coordY < SIZE_MATRIX) {
        activeRockets[rocketID].coordY += 1;
        activeRockets[rocketID].updateTime = millis();
      } else {
        deleteRocket(rocketID);
      }
    }
  }
}



/******************** SPACESHIP ************************/
class Spaceship {
  private:
    long unsigned int lastMovementTime;
    long unsigned int lastGunReloadingTime;

  public:
    int coordX;
    int movementSpeed;
    int gunReloadingSpeed;

    int score = 0;

    boolean canMoveLeft();
    boolean canMoveRight();
    boolean canShoot();

    boolean isTouchingCoords(int, int);

    void moveLeft();
    void moveRight();
    void shootLeft();
    void shootRight();

    ~Spaceship(){    //constructor
      lastMovementTime = 0;
      lastGunReloadingTime = 0;
    }
};

boolean Spaceship::canMoveLeft() {
  return (coordX > 1 && (millis() - lastMovementTime) > movementSpeed);
}

boolean Spaceship::canMoveRight() {
  return (coordX < (SIZE_MATRIX - 2) && (millis() - lastMovementTime) > movementSpeed);
}

boolean Spaceship::canShoot() { //if current time - the last time the gun was loaded/ the last time we fired > the speed of reloading a gun, you are allowed to shoot again
  return (millis() - lastGunReloadingTime) > gunReloadingSpeed;
}

void Spaceship::shootLeft() {
  int rocketID = findRocketID();

  if (rocketID != ALL_ROCKETS_HAVE_BEEN_USED) { // if we still have rockets we can shoot with
    activeRockets[rocketID].coordX = coordX - 1;
    activeRockets[rocketID].coordY = 2;
    activeRockets[rocketID].updateTime = millis();
  }

  lastGunReloadingTime = millis();  //update time, last shoot/fire
}

void Spaceship::shootRight() {
  int rocketID = findRocketID();

  if (rocketID != ALL_ROCKETS_HAVE_BEEN_USED) {  // if we still have rockets we can shoot with
    activeRockets[rocketID].coordX = coordX + 1;
    activeRockets[rocketID].coordY = 2;
    activeRockets[rocketID].updateTime = millis();
  }
  
  lastGunReloadingTime = millis();  //update time, last shoot/fire
}

bool Spaceship::isTouchingCoords(int placeCoordX, int placeCoordY) {
  bool isTouchingRightSide = (placeCoordX == coordX + 1 && (placeCoordY == 1 || placeCoordY == 2));
  bool isTouchingLeftSide = (placeCoordX == coordX - 1 && (placeCoordY == 1 || placeCoordY == 2));
  bool isTouchingFrontSide = (placeCoordX == coordX && placeCoordY == 1);

  return isTouchingRightSide || isTouchingLeftSide  || isTouchingFrontSide;
}

void Spaceship::moveLeft() {
  if (coordX > 1) {
    coordX -= 1;
  }
  lastMovementTime = millis();
}

void Spaceship::moveRight() {
  if (coordX < SIZE_MATRIX) {
    coordX += 1;
  }
  lastMovementTime = millis();
}

Spaceship spaceship;


/********************** METEOR ******************/
class Meteor {
  private:
    long unsigned int lastMovementTime = 0;
  public:
    int coordX;
    int coordY = SIZE_MATRIX - 1;  // Always starts from the top part of the screen

    int movementSpeed = 3000;

    boolean canMove();
    void moveForward();
};

Meteor liveMeteors[MAX_NUMBER_OF_METEORS] = {};           //initialize liveMeteors array with 0
boolean occupiedMeteorPlace[MAX_NUMBER_OF_METEORS] = {};  //initialize occupiedMeteorPlace array with 0
long lastCreatedMeteorTime = millis();
int createMeteorSpeed = 1000;

boolean Meteor::canMove() {
  return (millis() - lastMovementTime) > movementSpeed;
}

void Meteor::moveForward() {
  int newCoordX = coordX + random(1);  // Move +1 forward or do not move at all
  int newCoordY = coordY + random(2) - 1;  // Move left, right or do not move at all

  boolean isNewPositionOccupied = false;
  Meteor meteor;

  for (int meteorID = 0; meteorID < MAX_NUMBER_OF_METEORS; meteorID++) {
    if (occupiedMeteorPlace[meteorID]) {
      meteor = liveMeteors[meteorID];

      if (meteor.coordX == newCoordX && meteor.coordY == newCoordY) {
        isNewPositionOccupied = true;
        break;
      }
    }
  }

  if (!isNewPositionOccupied) {
    coordX = newCoordX;
    coordY = newCoordY;
    lastMovementTime = millis();
  }
}

void createMeteor() {
  if ((millis() - lastCreatedMeteorTime) < createMeteorSpeed) {
    return;
  }

  Meteor meteor;
  meteor.coordX = random(SIZE_MATRIX);  //get a random number between 0 and 8

  for (int meteorID = 0; meteorID < MAX_NUMBER_OF_METEORS; meteorID++) {
    if (!occupiedMeteorPlace[meteorID]) {
      occupiedMeteorPlace[meteorID] = true;
      liveMeteors[meteorID] = meteor;
      break;
    }
  }
}

void drawMeteors() {
  for (int meteorID = 0; meteorID < MAX_NUMBER_OF_METEORS; meteorID++) {
    Meteor meteor = liveMeteors[meteorID];

    if (occupiedMeteorPlace[meteorID]) {
      lc.setLed(0, meteor.coordX, meteor.coordY, true);

      if (meteor.canMove()) {
        liveMeteors[meteorID].moveForward();

        if (meteor.coordX < 0 || meteor.coordX > SIZE_MATRIX - 1 || meteor.coordY < 0) {
          occupiedMeteorPlace[meteorID] = false;
        }
      }
    }
  }
}

void checkIfMeteorTouchedSpaceship() {
  for (int meteorID = 0; meteorID < MAX_NUMBER_OF_METEORS; meteorID++) {
    Meteor meteor = liveMeteors[meteorID];

    if (occupiedMeteorPlace[meteorID] && spaceship.isTouchingCoords(meteor.coordX, meteor.coordY)) {
      lives--;
      blinkScreen(4);
      delay(200);
      
      if(lives){     //if you still have lives, resetGame
         resetGame();
      }
      else{  
          boolean PLAY_SOUND = true;   /* BUZZER -> send tune if game is over */
          if (PLAY_SOUND == true) {
             makeBeep(1, 1, 220);  //frequency = 220
             makeBeep(5, 1, 440); //frequency = 440
             makeBeep(1, 1, 660); //frequency = 660
          }
          PLAY_SOUND = false;
          GAME_OVER = true;  //if there are no more lives, send tune and print "game over"
      }
    }
  }
}



/******************** GAME ******************************/
byte currentGrid[SIZE_MATRIX] = {};
byte meteorGrid[SIZE_MATRIX] = {};
byte EMPTY_GRID[SIZE_MATRIX] = {B00000000,   //blank matrix
                                B00000000,
                                B00000000,
                                B00000000,
                                B00000000,
                                B00000000,
                                B00000000,
                                B00000000,
                               };
int gameSpeed = 500;

void drawGrid(byte *grid) {
  for (int row = 0; row < SIZE_MATRIX; row++) {
    lc.setColumn(0, row, grid[row]);
  }
}

void drawSpaceship(int coord) {
  int shift = coord - 1;
  currentGrid[1] = B10100000 >> shift;
  currentGrid[0] = B11100000 >> shift;
}

boolean isButtonPressed(int buttonID) {
  int buttonState = digitalRead(buttonID);  // read the state of the pushbutton value
  if (buttonState == HIGH) {  // check if the pushbutton is pressed. If it is, the buttonState is HIGH
    digitalWrite(ledPin, HIGH);   // turn LED on
    return 1;
  }
  digitalWrite(ledPin, LOW);  // turn LED off
  return 0;
}

void blinkScreen(int nTimes) {
  for (int i = 0; i < nTimes; i++) {
    for (int row = 0; row < 8; row++) {
      lc.setColumn(0, row, B11111111);
    }
    delay(50);

    for (int row = 0; row < 8; row++) {
      lc.setColumn(0, row, B00000000);
    }
    delay(50);
  }
}

void checkIfSpaceshipHitMeteor() {
  Rocket rocket;
  Meteor meteor;

  for (int rocketID = 0; rocketID < MAX_NUMBER_OF_ROCKETS; rocketID++) {
    rocket = activeRockets[rocketID];

    if (!isRocketNone(rocket)) {
      for (int meteorID = 0; meteorID < MAX_NUMBER_OF_METEORS; meteorID++) {
        meteor = liveMeteors[meteorID];

        if (occupiedMeteorPlace[meteorID] && rocket.doesHitTarget(meteor.coordX, meteor.coordY)) {
          occupiedMeteorPlace[meteorID] = false;
        }
      } // meteor loop end
    }
  }  // rocket loop end
}

void resetGame() {
  boolean PLAY_SOUND = true;  /* BUZZER */
  if (PLAY_SOUND == true){
    makeBeep(5, 1, 440);  //frequency = 440 
    makeBeep(1, 1, 660); //frequency = 660 
  }
  PLAY_SOUND = false;
  
  // Set up default position for the spaceship
  spaceship.coordX = 4;  //middle point of spacehip symbol is located on position 4 on the LED MATRIX
  spaceship.movementSpeed = 50;  // The lower - the faster.
  spaceship.gunReloadingSpeed = 200;  // The lower - the faster.

  for (int meteorID = 0; meteorID < MAX_NUMBER_OF_METEORS; meteorID++) {    // Clear all meteors
    occupiedMeteorPlace[meteorID] = false;
  }

  for (int rocketID = 0; rocketID < MAX_NUMBER_OF_ROCKETS; rocketID++) { // Clear all spaceship rockets
    deleteRocket(rocketID);
  }
}



/**************************BUZZER****************************/
void makeBeep(int intervalSeconds, int howlongSeconds, int freq) {
  delay(intervalSeconds * 1000);
  tone(buzzerPin, freq, howlongSeconds * 1000);
}



/***************************SETUP****************************/
void setup() {
  Serial.begin(9600);
  /* LED MATRIX */
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, 2); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);    // clear screen
  /* BUZZER */
  pinMode(buzzerPin, OUTPUT);  //specify pin to behave as an output
  /* LED */
  pinMode(ledPin, OUTPUT);
  /* BUTTONS */
  pinMode(leftButton, INPUT);
  pinMode(leftFireButton, INPUT);
  pinMode(rightFireButton, INPUT);
  pinMode(rightButton, INPUT); 
  
  resetGame();
}

void loop() {
 if(!GAME_OVER){
  if (isButtonPressed(leftButton) && spaceship.canMoveLeft()) {
    spaceship.moveLeft();
  }
  if (isButtonPressed(rightButton) && spaceship.canMoveRight()) {
    spaceship.moveRight();
  }
  if (isButtonPressed(leftFireButton) && spaceship.canShoot()) {
    spaceship.shootLeft();
  }
  if (isButtonPressed(rightFireButton) && spaceship.canShoot()) {
    spaceship.shootRight();
  }
     
  if (random(10) < 3){
    createMeteor();
  }

  memcpy(currentGrid, EMPTY_GRID, SIZE_MATRIX);  /* I used memcpy because: If I use drawGrid(EMPTY_GRID) function,
                                                  * and then  drawGrid(currentGrid) function, we get
                                                  * a turn on - turn off light effect, and we don't want that*/
  drawSpaceship(spaceship.coordX);
  drawGrid(currentGrid);
  drawRockets();
  drawMeteors();

  checkIfMeteorTouchedSpaceship();
  checkIfSpaceshipHitMeteor();
 }
 else{   
    if(!lives){ //if game is over (there are no more lives)
     for (int i = 0; i < SIZE_MESSAGE_GAME_OVER; i++) {
        for (int row = 0; row < SIZE_MATRIX; row++) {
           lc.setColumn(0, row, gameOverMessage[i][SIZE_MATRIX - 1 - row]);
        }
        //if you press leftButton and rightButton, the game starts again
        if (isButtonPressed(leftButton) && isButtonPressed(rightButton)){
          GAME_OVER = false;
          lives = HEARTS;
          blinkScreen(4);
          break;   
        }
        delay(400);
     }  
    }
    else{  //if there are 3 lives, it means the game just started, so we don't print game over message
      for (int i = 0; i < SIZE_MESSAGE_START; i++) {
         for (int row = 0; row < SIZE_MATRIX; row++) {
            lc.setColumn(0, row, startNewGameMessage[i][SIZE_MATRIX - 1 - row]);
         }
         //if you press leftButton and rightButton in the same time, the game starts 
         if (isButtonPressed(leftButton) && isButtonPressed(rightButton)){
            GAME_OVER = false;
            lives = HEARTS + 1;
            blinkScreen(4);
            break;
        } 
        delay(400);
      }
    }    
  }
 }
