const int pinLedR = 9;
const int pinLedG = 10;
const int pinLedB = 11;

const int pinPotR = A0;
const int pinPotG = A1;
const int pinPotB = A2;

unsigned int valuePotR = 0;
unsigned int valuePotG = 0;
unsigned int valuePotB = 0;

unsigned int ledValueR = 0;
unsigned int ledValueG = 0;
unsigned int ledValueB = 0;

void setup() {
  pinMode(pinPotR, INPUT);
  pinMode(pinPotG, INPUT);
  pinMode(pinPotB, INPUT);
  
  pinMode(pinLedR, OUTPUT);
  pinMode(pinLedG, OUTPUT);
  pinMode(pinLedB, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  valuePotR =  analogRead(pinPotR);
  valuePotG =  analogRead(pinPotG);
  valuePotB =  analogRead(pinPotB);
  
  ledValueR = map(valuePotR, 0, 1023, 0, 255);
  ledValueG = map(valuePotG, 0, 1023, 0, 255);
  ledValueB = map(valuePotB, 0, 1023, 0, 255);
  
  analogWrite(pinLedR, ledValueR);
  analogWrite(pinLedG, ledValueG);
  analogWrite(pinLedB, ledValueB);
  
  Serial.println(valuePotR);
}
