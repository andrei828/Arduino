#define LOCKED_DIGIT 1
#define BROWSE_DIGITS 0

// joystick pins
const int pinX = A0; 
const int pinY = A1; 
const int pinSW = 0;

// 4 digit 7 segment display pins
const int pinA = 12;
const int pinB = 8;
const int pinC = 5;
const int pinD = 3;
const int pinE = 2;
const int pinF = 11;
const int pinG = 6;
const int pinDP = 4;
const int pinD1 = 7;
const int pinD2 = 9;
const int pinD3 = 10;
const int pinD4 = 13;

// state variables
int currentDigit = 0;
int state = BROWSE_DIGITS;

// imutable variables
const int segSize = 8;
const int noOfDigits = 10;
const int noOfDisplays = 4;

// mutable values
int blink = 0;
int xValue = 0;
int yValue = 0;
int dpState = LOW;
int swState = LOW;
int lastSwState = LOW;
int joyStickToggleX = 0;
int joyStickToggleY = 0;
unsigned long lastIncreasing = 0;
unsigned long delayBlinkTime = 300;

// segments array, similar to before
int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};
// digits array, to switch between them easily
int digits[noOfDisplays] = {
  pinD1, pinD2, pinD3, pinD4
};

// values of the four digits
unsigned int digitArray[noOfDisplays] = { 0, 0, 0, 0 };

// digits for 7 segement display
byte digitMatrix[noOfDigits + 1][segSize - 1] = {
  // a  b  c  d  e  f  g
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}, // 9
  {0, 0, 0, 0, 0, 0, 0}  // NULL
};

void displayNumber(byte digit, byte decimalPoint) {
  for (int i = 0; i < segSize - 1; i++)
    digitalWrite(segments[i], digitMatrix[digit][i]);

  // write the decimalPoint to DP pin
  digitalWrite(segments[segSize - 1], decimalPoint);
}

// activate the display no. received as param
void showDigit(int num) {
  for (int i = 0; i < noOfDisplays; i++)
    digitalWrite(digits[i], HIGH);
  
  digitalWrite(digits[num], LOW);
}


void setup() {
  pinMode(pinSW, INPUT_PULLUP);

  for (int i = 0; i < segSize - 1; i++)
    pinMode(segments[i], OUTPUT);

  for (int i = 0; i < noOfDisplays; i++)
    pinMode(digits[i], OUTPUT);

  Serial.begin(9600);
}

void displayArray() {
  for (int i = 0; i < noOfDisplays; i++) {
    showDigit(i);
    if (i == currentDigit)
      displayNumber(digitArray[i], HIGH);
    else
      displayNumber(digitArray[i], LOW);
    delay(5);
  }
}

void loop() {

  // input values
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  swState = digitalRead(pinSW);

  // change push button state
  if (swState != lastSwState) {
    if (swState == LOW) {
      dpState = !dpState;
    }
  }
  lastSwState = swState;

  
  // update output here
  if (state == BROWSE_DIGITS) {
    
    if (yValue < 400 && joyStickToggleY == 0) {
      joyStickToggleY = 1;
      
      if (++currentDigit > 3) 
        currentDigit = 0;
      
    } else if (yValue > 600 && joyStickToggleY == 0) {
      joyStickToggleY = 1;

      if (--currentDigit < 0)
        currentDigit = 3;
    }
    else if (yValue >= 400 && yValue <= 600) {
      joyStickToggleY = 0;
    }

    // switch state
    if (dpState == HIGH) {
      dpState = !dpState;
      state = LOCKED_DIGIT;
    }

    displayArray();
    
  } else if (state == LOCKED_DIGIT) {
    
    if (xValue < 400 && joyStickToggleX == 0) {
      joyStickToggleX = 1;

      if (++digitArray[currentDigit] > 9)
        digitArray[currentDigit] = 0;
        
    } else if (xValue > 600 && joyStickToggleX == 0) {
      joyStickToggleX = 1;
      
      if (--digitArray[currentDigit] < 0)
        digitArray[currentDigit] = 9;
    }
    else if (xValue >= 400 && xValue <= 600) {
      joyStickToggleX = 0;
    }

    // switch state
    if (dpState == HIGH) {
      blink = 0;
      dpState = !dpState;
      state = BROWSE_DIGITS;
    }

    if (blink == 0) {
      displayArray();
    }  else {
      for (int i = 0; i < noOfDisplays; i++) {
        showDigit(i);
        if (i == currentDigit)
          displayNumber(10, HIGH);
        else
          displayNumber(digitArray[i], LOW);
        delay(5);
      }

    }

    if (millis() - lastIncreasing >= delayBlinkTime) {
      blink = !blink;
      lastIncreasing = millis();
    }

  }


}
