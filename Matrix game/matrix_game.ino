#include <EEPROM.h>
#include <LiquidCrystal.h>

#include "LedControl.h"

#define BLANK         (char) 254
#define UP_ARROW      byte(0)
#define DOWN_ARROW    byte(1)
#define BACK_ARROW    byte(2)
#define RESTART_ARROW byte(3)

#define MAIN_MENU       0
#define START_MENU      1
#define SETTINGS_MENU   2
#define HIGHSCORE_MENU  3
#define START_MENU_END  4
#define LOADING_SCREEN  5
#define DISPLAY_SCORE   6
#define ABOUT_GAME_MENU 7

#define BACK_SELECTED 0
#define RESTART_SELECTED 1

#define UPPER_MAIN_MENU 0
#define LOWER_MAIN_MENU 1
#define THIRD_MAIN_MENU 2

#define ITEM_START 0
#define ITEM_SETTINGS 2
#define ITEM_HIGHSCORE 1

#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8
#define NUM_OF_OBSTACLES 4
#define MAX_ON_SCREEN_OBSTACLES 3

LedControl lc = LedControl(12, 11, 10, 1);
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

struct Obstacle {
  int xPos;
  int yPos;
  int width;
  int height;
};
Obstacle * obstacleList;

byte restartArrow[] = {
  B00000,
  B01111,
  B01001,
  B11101,
  B01001,
  B00001,
  B00111,
  B00000
};

byte downArrow[] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B10101,
  B01110,
  B00100
};

byte upArrow[] = {
  B00100,
  B01110,
  B10101,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

byte backArrow[] = {
  B00010,
  B00100,
  B01000,
  B11111,
  B01000,
  B00100,
  B00010,
  B00000
};

int matrix[8][8] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1,
};

int hardObstacles[NUM_OF_OBSTACLES][3] = {
  6, 2, 2,
  5, 2, 1,
  5, 3, 1,
  6, 1, 3
};

int easyObstacles[NUM_OF_OBSTACLES][3] = {
  6, 1, 1,
  6, 2, 1,
  6, 1, 2,
  5, 1, 1
};

int gameOverState = 2;

// joystick pins
const int pinX = A0;
const int pinY = A1;
const int pinSW = 8;
int buttonState = 0; 
const int buttonPin = 13;

 // joystick values
int xValue = 0;
int yValue = 0;
int xPressed = 0;
int yPressed = 0;
int currentSWValue = 0;
int previousSWValue = 0;

long previousMillis = 0;
long previousMillisLoading = 0;
long previousMillisSettings = 0;
long previousMillisFinishGame = 0;

// for matrix
long spawnObstacleMillis = 0;
long previousMillisJumping = 0;
long previousMillisGravity = 0;
long previousMillisObstacleMove = 0;

int index = 0;
int startTime = 0;
int longPress = 0;
int firstPress = 0;
int buttonPressed = 0;
int obstacleSpeed = 10;
int buttonCurrentValue = 0;
int buttonPreviousValue = 0;

// matrix variables
int currentX = 1;
int currentY = 0;
int additionalLedX = 0;
int additionalLedY = -1;

// LCD pins
const int RS = 7;
const int enable = 6;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

// start menu variables
byte fall = 0;
byte blink = 0;
int highScore = 0;
int currentLevel = 1;
int currentScore = 0;
int currentLives = 3;
long delay005Sec = 50;
long delay05Sec = 500;
long delay1Sec = 1000;
long delay3Sec = 3000;
long delay10Sec = 10000;
long delay30Sec = 30000;
int startingLevelValue = 1;

int menuState = LOADING_SCREEN;
int mainMenuState = UPPER_MAIN_MENU; 
int endMenuSelection = BACK_SELECTED;
int mainMenuSelectedValue = ITEM_START;

int getHigh()
{
   int score = 0;
   for (int i = 0; i <= 20; i++)
    score+=EEPROM.read(i);
   return score;
}

void saveHigh(int score)
{
   int a = score % 20;
   int b = (score - a) / 20;
   if (score < 5100) {
    for (int i = 0; i <= 19; i++)
     EEPROM.write(i, b);
 
    EEPROM.write(20, a);
   }
}

void drawMatrix() {
  for (int i = 0; i < MATRIX_HEIGHT; i++) {
    for (int j = 0; j < MATRIX_WIDTH; j++) {
       lc.setLed(0, i, j, matrix[i][j]);
    }
  }
}

void printObstacles() {
  for (int i = 0; i < MAX_ON_SCREEN_OBSTACLES; i++) {
    char buffer[256];
    sprintf(buffer, "%d:\nx:\t%d\ny:\t%d\nwidth:\t%d\nheight:\t%d\n", i + 1, 
    obstacleList[i].xPos, obstacleList[i].yPos,
    obstacleList[i].width, obstacleList[i].height);
    Serial.print(buffer);
  }
  Serial.println("\n");
}

void spawnObstacle(int yPos, int width, int height) {
  int spawnIndex = -1;
  for (int i = 0; i < MAX_ON_SCREEN_OBSTACLES; i++){
    if (obstacleList[i].xPos == 20)
      spawnIndex = i;
  }
  
  if (spawnIndex > -1) {
    obstacleList[spawnIndex].yPos = yPos;
    obstacleList[spawnIndex].xPos = 10;
    obstacleList[spawnIndex].width = width;
    obstacleList[spawnIndex].height = height;
  }
}

void spawnEasyObstacle() {
    index = random(NUM_OF_OBSTACLES);
    spawnObstacle(easyObstacles[index][0], easyObstacles[index][1], easyObstacles[index][2]);
}

void spawnHardObstacle() {
    index = random(NUM_OF_OBSTACLES);
    spawnObstacle(hardObstacles[index][0], hardObstacles[index][1], hardObstacles[index][2]);
}

void drawObstacles() {
  
  for (int i = 0; i < MAX_ON_SCREEN_OBSTACLES; i++){
    // obstacle is on screen
    if (obstacleList[i].xPos < 20) {

      // all obstacle is on screen
      if (obstacleList[i].xPos + obstacleList[i].width - 1 < MATRIX_WIDTH) {
      
        for (int j = 0; j < obstacleList[i].width; j++)
          for (int k = 0; k < obstacleList[i].height; k++) 
            matrix[obstacleList[i].yPos - k][obstacleList[i].xPos + j] = 1;

          if (obstacleList[i].xPos + obstacleList[i].width < MATRIX_WIDTH)
            for (int j = 0; j < obstacleList[i].height; j++)
              matrix[obstacleList[i].yPos - j][obstacleList[i].xPos + obstacleList[i].width] = 0;
            
         if (obstacleList[i].xPos > 0)
          obstacleList[i].xPos--;
         else
          obstacleList[i].width--;
         
          if ((obstacleList[i].xPos < currentX && 
               obstacleList[i].xPos + obstacleList[i].width >= currentX) && (
              (obstacleList[i].yPos >= currentY && 
               obstacleList[i].yPos - obstacleList[i].height <= currentY) ||
              (obstacleList[i].yPos >= currentY + additionalLedY && 
               obstacleList[i].yPos - obstacleList[i].height <= currentY + additionalLedY))) {

            if (!(--currentLives)){
              gameOverState = 1;
              Serial.println("GAme over");
            } else {
              matrix[6][0] = 0; matrix[5][0] = 0; matrix[6][1] = 0;
              matrix[6][2] = 0; matrix[6][3] = 0; matrix[5][1] = 0;
              matrix[5][2] = 0; matrix[4][1] = 0; matrix[5][3] = 0;
              obstacleList[i].xPos = 20;
            }
          }
          
          
       if (obstacleList[i].xPos + obstacleList[i].width < 0){
          obstacleList[i].xPos = 20;
          currentScore++;
          for (int idx = 0; idx < NUM_OF_OBSTACLES; idx++)
            if (obstacleList[i].yPos == hardObstacles[idx][0] &&
                obstacleList[i].height == hardObstacles[idx][2]) {
               currentScore++;
              
            }
          if (highScore < currentScore) {
            saveHigh(currentScore);
            highScore = currentScore;
          }
       }
       
       } else {
          obstacleList[i].xPos--;
        }
      
    }
  }
}

void drawUpperMainMenu() {
  // title
  lcd.setCursor(3, 0);
  lcd.print("Game Menu!");

  // items
  lcd.setCursor(0, 1);
  lcd.print("Start");
  if (mainMenuSelectedValue == ITEM_START)
    lcd.print(" <");
    
  // scroll arrow
  lcd.setCursor(15, 1);
  
  lcd.write(DOWN_ARROW);

  if (yValue > 600 && !yPressed) {
    yPressed = 1;
    lcd.clear();
    mainMenuState = LOWER_MAIN_MENU;
    mainMenuSelectedValue = ITEM_HIGHSCORE;
  } else {
    yPressed = 0;
  }
}

void drawLowerMainMenu() {
  // items
  lcd.setCursor(0, 0);
  lcd.print("Highscore");
  if (mainMenuSelectedValue == ITEM_HIGHSCORE) {
    lcd.print(" <");
  }
    
  lcd.setCursor(0, 1);
  lcd.print("Settings");
  if (mainMenuSelectedValue == ITEM_SETTINGS) {
    lcd.print(" <");
  }
 
  // scroll arrow
  lcd.setCursor(15, 0);
  lcd.write(UP_ARROW);
  lcd.setCursor(15, 1);
  lcd.write(DOWN_ARROW);

  if (yValue < 400 && !yPressed) {
    yPressed = 1;
    
    if (mainMenuSelectedValue == ITEM_HIGHSCORE) {
      lcd.clear();
      mainMenuState = UPPER_MAIN_MENU;
      mainMenuSelectedValue = ITEM_START;
    } else if (mainMenuSelectedValue = ITEM_SETTINGS) {
      lcd.setCursor(9, 1);
      lcd.print((char)254);
      mainMenuSelectedValue = ITEM_HIGHSCORE;
    }
   }
   else if (yValue > 600 && !yPressed) {
      yPressed = 1;
      if (mainMenuSelectedValue == ITEM_SETTINGS) {
        mainMenuState = THIRD_MAIN_MENU;
        lcd.clear();
      } else {
        lcd.setCursor(10, 0);
        lcd.print(BLANK);
        mainMenuSelectedValue = ITEM_SETTINGS;
      }
    } else if (yValue >= 400 && yValue <= 600) {
      yPressed = 0;
    }
}

void drawThirdMainMenu() {
  // items
  lcd.setCursor(0, 0);
  lcd.print("About <");
   
  // scroll arrow
  lcd.setCursor(15, 0);
  lcd.write(UP_ARROW);

  if (yValue < 400 && !yPressed) {
    yPressed = 1;
    mainMenuState = LOWER_MAIN_MENU;
    mainMenuSelectedValue = ITEM_SETTINGS;
  } else if (yValue >= 400 && yValue <= 600) {
    yPressed = 0;
  }

  if (currentSWValue == 1 && previousSWValue != currentSWValue) {
    lcd.clear();
    menuState = ABOUT_GAME_MENU;
  }
  previousSWValue = currentSWValue;

}

void drawStartMenu() {

  if (previousMillis + delay30Sec < millis()) {
    previousMillis = millis();
    currentLevel++;
    obstacleSpeed += 10; 
  }

    lcd.setCursor(0, 0);
    lcd.write(BACK_ARROW);
    lcd.print("End");
  
    lcd.setCursor(8, 0);
    lcd.print("Lives: ");
    lcd.print(currentLives);
  
    lcd.setCursor(0, 1);
    lcd.print("LvL: ");
    if (currentLevel > 9)
      lcd.setCursor(4, 1);
    lcd.print(currentLevel);
  
    lcd.setCursor(8, 1);
    lcd.print("Score: ");
    if (currentScore > 9)
      lcd.setCursor(14, 1);
    lcd.print(currentScore);

    // ActualGame
    if (gameOverState == 1) {
    for (int i = 0; i < MATRIX_HEIGHT; i++)
      for (int j = 0; j < MATRIX_WIDTH; j++)
       lc.setLed(0, i, j, true);
      
      lcd.clear();
      menuState = START_MENU_END;
    } else if(gameOverState == 2) { 
    
      if (previousMillisObstacleMove + obstacleSpeed < millis()) {
        previousMillisObstacleMove = millis();
        drawObstacles();
      }
  
  
      buttonCurrentValue = digitalRead(buttonPin);
      if (firstPress == 1 && buttonCurrentValue == 0)
        firstPress = 0;

      if (!firstPress) {
          if (buttonCurrentValue == 1 && buttonPreviousValue == 0) {
            buttonPressed = 1;
            buttonPreviousValue = 1;
          } else if (buttonCurrentValue == 0 && buttonPreviousValue == 1) {
            buttonPressed = 0;
            buttonPreviousValue = 0;
           
              
            if(spawnObstacleMillis + 1000 < millis()) {
              spawnObstacleMillis = millis();
              if (longPress)
                spawnHardObstacle();
              else
                spawnEasyObstacle();
              startTime = 0;
              longPress = 0;
            }
            
          }
          if (buttonPressed == 0){
            startTime = millis();
          } else {
            if (startTime && startTime + 300 < millis()) {
              longPress = 1;
              startTime = 0;
            } 
          }
      }
      lc.setLed(0, currentY, currentX, true);
      lc.setLed(0, currentY + additionalLedY, currentX + additionalLedX, true);
      
      drawMatrix();
      additionalLedX = 0;
      additionalLedY = -1;
      if (yValue < 400 && currentY > 0 && !fall) {
        if (matrix[currentY - 1][currentX] != 1) {
          
          if (previousMillisJumping + delay005Sec < millis()) {
            previousMillisJumping = millis();
            
            if (currentY > 1) {
              if (currentY == 6) currentY--;
              currentY--;
              lc.setLed(0, currentY, currentX, false);
            } 
          }
          if (currentY == 1) fall = true;
        } 
        
      } else if (yValue >= 400 && yValue <= 600) {
        yPressed = 0;
        
        fall = true;
        if (matrix[currentY + 1][currentX])
          fall = false;
          
      } else if (yValue > 600) {
        additionalLedX = -1;
        additionalLedY = 0;
      }
  
      if (fall == true) {
        if (previousMillisGravity + delay005Sec + 100 < millis()) {
          previousMillisGravity = millis();
          if (!matrix[currentY + 1][currentX]) currentY++;
        }
      }
    }
  
      // End of actual game

   if (currentSWValue == 1 && previousSWValue != currentSWValue) {
    currentX = 1;
    currentLives = 3;
    currentScore = 0;
    gameOverState = 2;
    
    for (int i = 0; i < MATRIX_WIDTH; i++) {
      matrix[6][i] = 0;
      matrix[5][i] = 0;
      matrix[4][i] = 0;
    }

    for (size_t i = 0; i < MAX_ON_SCREEN_OBSTACLES; i++){
      obstacleList[i].xPos = 20;
      obstacleList[i].yPos = 6;
      obstacleList[i].width = 2;
      obstacleList[i].height = 2;
    }
    lcd.clear();
    gameOverState = 1;
  }
  previousSWValue = currentSWValue;
}


void drawStartMenuEnd() {
  lcd.setCursor(0, 0);
  lcd.print("Congratulations!");

  lcd.setCursor(3, 1);
  lcd.print("Continue...");

  if (currentSWValue == 1 && previousSWValue != currentSWValue) {
    lcd.clear();
    menuState = DISPLAY_SCORE;
  }
  previousSWValue = currentSWValue;
}

void displayScore() {
   lcd.setCursor(3, 0);
   lcd.print("Score: ");
    if (currentScore > 9)
      lcd.setCursor(10, 0);
   lcd.print(currentScore);
  
   if (previousMillisSettings + delay05Sec < millis()) {
    previousMillisSettings = millis();
    blink = !blink;
   } 

  lcd.setCursor(1, 1);
  if (endMenuSelection == BACK_SELECTED) {
    if (blink)
      for (int i = 0; i < 5; i++)
       lcd.print(BLANK);
    else {
       lcd.write(BACK_ARROW);
       lcd.print("Back");
    }
  } else {
    lcd.write(BACK_ARROW);
    lcd.print("Back");
  }

  lcd.setCursor(8, 1);
  if (endMenuSelection == RESTART_SELECTED) {
    if (blink)
      for (int i = 0; i < 8; i++)
       lcd.print(BLANK);
    else {
       lcd.write(RESTART_ARROW);
       lcd.print("Restart");
    }
  } else {
    lcd.write(RESTART_ARROW);
    lcd.print("Restart");
  }

  if (currentSWValue == 1 && previousSWValue != currentSWValue) {
    currentX = 1;
    currentLives = 3;
    currentScore = 0;
    gameOverState = 2;
    
    for (int i = 0; i < MATRIX_WIDTH; i++) {
      matrix[6][i] = 0;
      matrix[5][i] = 0;
      matrix[4][i] = 0;
    }

    for (size_t i = 0; i < MAX_ON_SCREEN_OBSTACLES; i++){
      obstacleList[i].xPos = 20;
      obstacleList[i].yPos = 6;
      obstacleList[i].width = 2;
      obstacleList[i].height = 2;
    }
    lcd.clear();
    
    if (endMenuSelection == BACK_SELECTED)
      menuState = MAIN_MENU;
    else 
      menuState = START_MENU;
  }
  previousSWValue = currentSWValue;

  if (xValue < 400 && !xPressed) {
    xPressed = 1;
    endMenuSelection = BACK_SELECTED;
  }
  else if (xValue > 600 && !xPressed) {
    xPressed = 1;
    endMenuSelection = RESTART_SELECTED;
  } 
  else if (xValue >= 400 && xValue <= 600) {
    xPressed = 0;
  }

}

void drawHighscoreMenu() {
  lcd.setCursor(0, 0);
  lcd.write(BACK_ARROW);
  lcd.print("Back");

  lcd.setCursor(2, 1);
  lcd.print("Highscore: ");
  if (highScore > 99)
    lcd.setCursor(12, 1);
  else if (highScore > 9)
    lcd.setCursor(13, 1);
  lcd.print(highScore);

  if (currentSWValue == 1 && previousSWValue != currentSWValue) {
    lcd.clear();
    menuState = MAIN_MENU;
  }
  previousSWValue = currentSWValue;
}

void drawSettingsMenu() {
  lcd.setCursor(0, 0);
  lcd.write(BACK_ARROW);
  lcd.print("Back");

  lcd.setCursor(1, 1);
  lcd.print("Start level: ");
  
  if (startingLevelValue > 9)
    lcd.setCursor(13, 1);

  if (previousMillisSettings + delay05Sec < millis()) {
    previousMillisSettings = millis();
    blink = !blink;
  } 

  if (blink) {
   if (startingLevelValue > 9)
    lcd.print(BLANK);
   lcd.print(BLANK);
  } else {
   lcd.print(startingLevelValue);
  }
 
  if (currentSWValue == 1 && previousSWValue != currentSWValue) {
    lcd.clear();
    menuState = MAIN_MENU;
  }
  previousSWValue = currentSWValue;

  if (yValue > 600 && !yPressed) {
    yPressed = 1;
    if (--startingLevelValue < 0)
      startingLevelValue = 50;
   
  } else if (yValue < 400 && !yPressed) {
    yPressed = 1;
    if (++startingLevelValue > 50)
     startingLevelValue = 0;
  
  } else if (yValue >= 400 && yValue <= 600) {
    yPressed = 0;
  }
}

void drawAboutMenu() {
  lcd.setCursor(0, 0);
  lcd.write(BACK_ARROW);
  lcd.print("Back");

  lcd.setCursor(0, 1);
  lcd.print("Creator: Andrei");

  if (currentSWValue == 1 && previousSWValue != currentSWValue) {
    lcd.clear();
    menuState = MAIN_MENU;
  }
  previousSWValue = currentSWValue;
}

void drawLoadingScreen() {
  lcd.setCursor(2, 0);
  lcd.print("Jumping LEDs");
  lcd.setCursor(4, 1);
  lcd.print("Loading...");

  if (previousMillisLoading == 0)
    previousMillisLoading = millis();
   
  if (previousMillisLoading + delay3Sec < millis()){
    menuState = MAIN_MENU;
    lcd.clear();
  }
}

void setup() {

  obstacleList = new Obstacle[MAX_ON_SCREEN_OBSTACLES];
    
  for (size_t i = 0; i < MAX_ON_SCREEN_OBSTACLES; i++){
    obstacleList[i].xPos = 20;
    obstacleList[i].yPos = 6;
    obstacleList[i].width = 2;
    obstacleList[i].height = 2;
  }
  
  lc.shutdown(0, false);
  lc.setIntensity(0, 2);
  lc.clearDisplay(0);

  pinMode(buttonPin, INPUT);
  pinMode(pinSW, INPUT_PULLUP);
  
  //lcd.home();
  lcd.begin(16, 2);
  lcd.createChar(UP_ARROW, upArrow);
  lcd.home();
  lcd.createChar(DOWN_ARROW, downArrow);
  lcd.home();
  lcd.createChar(BACK_ARROW, backArrow);
  lcd.home();
  lcd.createChar(RESTART_ARROW, restartArrow);
  
  Serial.begin(9600);
//  for (int i = 0; i < 21; i++)
//    EEPROM.write(i, 0);

   highScore = getHigh();
}

void loop() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  currentSWValue = !digitalRead(pinSW);
  
  if (menuState == LOADING_SCREEN) {
    drawLoadingScreen();
  
  } else if (menuState == MAIN_MENU) {
    if (mainMenuState == UPPER_MAIN_MENU) {
      drawUpperMainMenu();
    } else if (mainMenuState == LOWER_MAIN_MENU) {
      drawLowerMainMenu();
    } else if (mainMenuState == THIRD_MAIN_MENU) {
      drawThirdMainMenu();
    }
  
    if (mainMenuSelectedValue == ITEM_START && currentSWValue == 1 && previousSWValue != currentSWValue) {
      menuState = START_MENU;
      firstPress = 1;
      previousMillis = millis();
      previousMillisFinishGame = millis();
      currentLevel = startingLevelValue;
      obstacleSpeed = 100;
      lcd.clear();
    } else if (mainMenuSelectedValue == ITEM_HIGHSCORE && currentSWValue == 1 && previousSWValue != currentSWValue) {
      menuState = HIGHSCORE_MENU;
      previousMillisSettings = millis();
      lcd.clear();
    } else if (mainMenuSelectedValue == ITEM_SETTINGS && currentSWValue == 1 && previousSWValue != currentSWValue) {
      menuState = SETTINGS_MENU;
      previousMillisSettings = millis();
      lcd.clear();
    }
    previousSWValue = currentSWValue;
    
   } else if (menuState == START_MENU) {
    drawStartMenu();
   } else if (menuState == HIGHSCORE_MENU) {
    drawHighscoreMenu();
   } else if (menuState == SETTINGS_MENU) {
    drawSettingsMenu();
   } else if (menuState == START_MENU_END) {
    drawStartMenuEnd();
   } else if (menuState == DISPLAY_SCORE) {
    displayScore();
   } else if (menuState == ABOUT_GAME_MENU) {
    drawAboutMenu();
   }
}