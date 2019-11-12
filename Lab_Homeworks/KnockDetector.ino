int period = 200;
int buttonValue = 0;
int sensorReading = 0;   

const int piezoPin = 8;
const int buttonPin = 7;
const int threshold = 900;
const int knockSensor = A0;   

unsigned int counter = 0;
unsigned long time_now = 0;

bool detectKnockState = true;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(piezoPin, OUTPUT);
  pinMode(knockSensor, INPUT);
  Serial.begin(9600);      
}

void loop() {

  if(millis() > time_now + period) {
       time_now = millis();
  
    if (detectKnockState) {     
       sensorReading = analogRead(knockSensor);
       Serial.println(sensorReading);
       
       if (sensorReading >= threshold) {
        detectKnockState = !detectKnockState;
       }
     
    } else {
      buttonValue = digitalRead(buttonPin);
      Serial.println(buttonValue);
    
      if (buttonValue) {
       tone(piezoPin, HIGH);
      } else {
       noTone(piezoPin);
       detectKnockState = !detectKnockState;
      }
    
    }
  } 
}
