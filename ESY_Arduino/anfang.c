
int IN1 = 4;
int IN2 = 5;
int IN3 = 6;
int IN4 = 7;

const int sensorPinFront = A5; // Sensor-Pin am Arduino GP2Y0A02YK0F
const int sensorPinRight = A3; // Sensor-Pin am Arduino GP2Y0A21YK0F
const int sensorPinLeft = A4; // Sensor-Pin am Arduino GP2Y0A21YK0F
int sensorValueFront; // Variable zur Aufnahme des Sensorwerts
int sensorValueRight; // Variable zur Aufnahme des Sensorwerts
int sensorValueLeft; // Variable zur Aufnahme des Sensorwerts

const int startButtonPin = 2;  // Pin für den Start-Taster
const int stopButtonPin = 3;   // Pin für den Stop-Taster

volatile bool isRunning = false;  // Variable zur Verfolgung des Systemzustands

void setup() {
  Serial.begin(9600);
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(stopButtonPin, INPUT_PULLUP);

  // Funktionen zu registrieren, die bei fallender Flanke 
  attachInterrupt(digitalPinToInterrupt(startButtonPin), startButtonInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(stopButtonPin), stopButtonInterrupt, FALLING);
}

void loop() {
  sensorValueFront = analogRead(sensorPinFront); // Lies den analogen Wert vom Sensor
  sensorValueLeft = analogRead(sensorPinLeft); // Lies den analogen Wert vom Sensor
  sensorValueRight = analogRead(sensorPinRight); // Lies den analogen Wert vom Sensor

  // Konvertiere den ADC-Wert in eine Spannung (0-5V entspricht 0-1023 ADC-Werten)
  float voltageFront = sensorValueFront * (5.0 / 1023.0);
  Serial.println(voltageFront);

  // Konvertiere die Spannung in den Abstand
  float distanceFront = voltageToDistanceFront(voltageFront);
  
  Serial.print(sensorValueFront);
  Serial.print("  ");
  Serial.println(distanceFront);
  delay(1000);
  

  if (isRunning) {
    if(distanceFront > 15){
      forward();
    }
    else{
      stop();
    }
  }
  else{
    stop();
  }
}

float voltageToDistanceFront(float voltageFront) {
  float distanceFront = -49.87*pow(voltageFront,6) + 636*pow(voltageFront,5)-3345.16*pow(voltageFront,4)+9252.75*pow(voltageFront,3)-14122.49*pow(voltageFront,2)+11185.39*voltageFront-3521.13;
  //float distanceFront = -19.98*voltageFront + 64.41;
  return distanceFront;
}

void startButtonInterrupt() {
  isRunning = true;
}

void stopButtonInterrupt() {
  isRunning = false;
}

void forward(){
  analogWrite(9, 150);
  analogWrite(10, 150);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void back(){
  analogWrite(9, 150);
  analogWrite(10, 150);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void stop(){
  analogWrite(9, 0);
  analogWrite(10, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void turnLeft() {
  analogWrite(9, 150);
  analogWrite(10, 0); // Reduzieren Sie das linke Rad, um nach rechts zu drehen
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnRight() {
  analogWrite(9, 0);
  analogWrite(10, 150); 
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void twistLeft(){
  analogWrite(9, 150);
  analogWrite(10, 150);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}