#include <LiquidCrystal.h>

#define UP_ARROW   byte(0)
#define DOWN_ARROW byte(1)
#define BACK_ARROW byte(2)
#define BLANK      (char) 254

#define MAIN_MENU 0
#define START_MENU 1
#define SETTINGS_MENU 2
#define HIGHSCORE_MENU 3
#define START_MENU_END 4

#define UPPER_MAIN_MENU 0
#define LOWER_MAIN_MENU 1

#define ITEM_START 0
#define ITEM_HIGHSCORE 1
#define ITEM_SETTINGS 2

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

// joystick pins
const int pinX = A0;
const int pinY = A1;
const int pinSW = 8;

 // joystick values
int xValue = 0;
int yValue = 0;
int xPressed = 0;
int yPressed = 0;
int currentSWValue = 0;
int previousSWValue = 0;

long previousMillis = 0;
long previousMillisSettings = 0;
long previousMillisFinishGame = 0;

// LCD pins
const int RS = 12;
const int enable = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

// start menu variables
byte blink = 0;
long delay3Sec = 3000;
long delay05Sec = 500;
long delay10Sec = 10000;
int startingLevelValue = 1;
int currentLevel = 1;
int currentScore = 0;
int highScore = 0;

// highscore menu variables
int highscore = 0;

LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

int mainMenuSelectedValue = ITEM_START;
int menuState = MAIN_MENU;
int mainMenuState = UPPER_MAIN_MENU; 

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
}

void drawStartMenu() {

  if (previousMillis + delay3Sec < millis()) {
    previousMillis = millis();
    currentLevel++;
    currentScore = currentLevel * 3;
    if (highScore < currentScore)
     highScore = currentScore;
  }

  if (previousMillisFinishGame + delay10Sec < millis()) {
    previousMillisFinishGame = millis();
    lcd.clear();
    menuState = START_MENU_END;
    
  } else {
    lcd.setCursor(0, 0);
    lcd.write(BACK_ARROW);
    lcd.print("Back");
  
    lcd.setCursor(8, 0);
    lcd.print("Lives: 3");
  
    lcd.setCursor(0, 1);
    lcd.print("LvL: ");
    if (currentLevel > 9)
      lcd.setCursor(4, 1);
    lcd.print(currentLevel);
  
    lcd.setCursor(8, 1);
    lcd.print("Score: ");
    if (currentLevel * 3 > 9)
      lcd.setCursor(14, 1);
    lcd.print(currentLevel * 3);

    if (currentSWValue == 1 && previousSWValue != currentSWValue) {
     lcd.clear();
     menuState = MAIN_MENU;
    }

    previousSWValue = currentSWValue;
  } 
}

void drawStartMenuEnd() {
  lcd.setCursor(0, 0);
  lcd.print("Congratulations!");

  lcd.setCursor(5, 1);
  lcd.write(BACK_ARROW);
  lcd.print("Back");

  if (currentSWValue == 1 && previousSWValue != currentSWValue) {
    lcd.clear();
    menuState = MAIN_MENU;
  }
  previousSWValue = currentSWValue;
}

void drawHighscoreMenu() {
  lcd.setCursor(0, 0);
  lcd.write(BACK_ARROW);
  lcd.print("Back");

  lcd.setCursor(2, 1);
  lcd.print("Highscore: ");
  if (highScore > 9)
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

void setup() {

pinMode(pinSW, INPUT_PULLUP);

//lcd.home();
lcd.begin(16, 2);
lcd.createChar(UP_ARROW, upArrow);
lcd.home();
lcd.createChar(DOWN_ARROW, downArrow);
lcd.home();
lcd.createChar(BACK_ARROW, backArrow);

Serial.begin(9600);
}

void loop() {

xValue = analogRead(pinX);
yValue = analogRead(pinY);
currentSWValue = !digitalRead(pinSW);

if (menuState == MAIN_MENU) {
  if (mainMenuState == UPPER_MAIN_MENU) {
    drawUpperMainMenu();

    if (yValue > 600 && !yPressed) {
      yPressed = 1;
      lcd.clear();
      mainMenuState = LOWER_MAIN_MENU;
      mainMenuSelectedValue = ITEM_HIGHSCORE;
    } else {
      yPressed = 0;
    }
    
  } else {
    drawLowerMainMenu();

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
      
      lcd.setCursor(10, 0);
      lcd.print(BLANK);
      mainMenuSelectedValue = ITEM_SETTINGS;
    } else if (yValue >= 400 && yValue <= 600) {
      yPressed = 0;
    }
  }

  if(mainMenuSelectedValue == ITEM_START && currentSWValue == 1 && previousSWValue != currentSWValue) {
    menuState = START_MENU;
    previousMillis = millis();
    previousMillisFinishGame = millis();
    currentLevel = startingLevelValue;
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
 }
}