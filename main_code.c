// Pins für die Motorsteuerung
const int IN1 = 4;
const int IN2 = 5;
const int IN3 = 6;
const int IN4 = 7;

// Enable-Pins für die Motoren
const int motorLeftEnablePin = 9;
const int motorRightEnablePin = 10;

// Pins für die Infrarotsensoren
const int sensorPinRight = A3;  // Sensor-Pin am Arduino GP2Y0A02YK0F
const int sensorPinLeft = A2;   // Sensor-Pin am Arduino GP2Y0A02YK0F
const int sensorPinFront = A5;  // Sensor-Pin am Arduino GP2Y0A21YK0F

// Start- und Stop-Taster
const int startButtonPin = 2;
const int stopButtonPin = 3;

// Geschwindigkeit der Motoren (0-255)
const int motorSpeed = 150; // Experimentiere mit diesem Wert

// Sensorwerte deklarieren
int sensorValueFront;
int sensorValueLeft;
int sensorValueRight;

bool isRunning = false;

void setup() {
  // Initialisierung der seriellen Kommunikation
  Serial.begin(9600);

  // Initialisierung der Pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(motorLeftEnablePin, OUTPUT);
  pinMode(motorRightEnablePin, OUTPUT);

  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(stopButtonPin, INPUT_PULLUP);

  // Funktionen zu registrieren, die bei fallender Flanke 
  attachInterrupt(digitalPinToInterrupt(startButtonPin), startButtonInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(stopButtonPin), stopButtonInterrupt, FALLING);
}

void loop() {
  // Lese die analogen Werte von den Infrarotsensoren
  sensorValueFront = analogRead(sensorPinFront);
  sensorValueLeft = analogRead(sensorPinLeft);
  sensorValueRight = analogRead(sensorPinRight);

  // Konvertiere die Spannung in den Abstand
  float distanceFront = voltageToDistanceFront(sensorValueFront);
  float distanceLeft = voltageToDistanceLeft(sensorValueLeft);
  float distanceRight = voltageToDistanceRight(sensorValueRight);

  // Ausgabe der Sensorwerte auf der seriellen Schnittstelle
  Serial.print("Links: ");
  Serial.print(distanceLeft);
  Serial.print(" cm | Vorne: ");
  Serial.print(distanceFront);
  Serial.print(" cm | Rechts: ");
  Serial.print(distanceRight);
  Serial.println(" cm  ");
    
  if (isRunning) {
      float korrektur = calculateCorrection(distanceLeft, distanceRight);
      adjustMotorSpeeds(korrektur);
      
  } else {
    stop();
  }
}

float calculateCorrection(float linksAbstand, float rechtsAbstand) {
  // Berechne den Korrekturwert basierend auf dem Unterschied zwischen links und rechts
  float zielAbstand = 20.0;  // Passe diesen Wert entsprechend deiner Anforderungen an
  float differenz = linksAbstand - rechtsAbstand;

  // Verwende einen proportionalen Regler, um die Korrektur zu berechnen
  float korrektur = differenz / zielAbstand;

  // Beschränke die Korrektur auf einen vernünftigen Bereich
  korrektur = constrain(korrektur, -1.0, 1.0);
  Serial.println(korrektur);

  return korrektur;
}

void adjustMotorSpeeds(float korrektur) {
  // Passe die Motorgeschwindigkeiten basierend auf dem Korrekturwert an
  int minSpeed = 90; // Minimale Geschwindigkeit
  int maxSpeedChange = 100; // Begrenzung der Geschwindigkeitsänderung

  int linkeMotorGeschwindigkeit = constrain(motorSpeed + korrektur * maxSpeedChange, minSpeed, 255);
  int rechteMotorGeschwindigkeit = constrain(motorSpeed - korrektur * maxSpeedChange, minSpeed, 255);
  Serial.print(linkeMotorGeschwindigkeit);
  Serial.print(" | ");
  Serial.println(rechteMotorGeschwindigkeit);
  delay(1000);

  // Aktualisiere die Motorgeschwindigkeiten
  forward(linkeMotorGeschwindigkeit, rechteMotorGeschwindigkeit);
}

void startButtonInterrupt() {
  isRunning = true;
}

void stopButtonInterrupt() {
  isRunning = false;
}

float voltageToDistanceLeft(float voltageLeft) {
  float distanceLeft = 101.929921 + (-0.944760) * voltageLeft + 0.00419381697807914757 * pow(voltageLeft, 2) 
                  - 0.00000993658241604138 * pow(voltageLeft, 3) + 0.000000011952303996626338293728 * pow(voltageLeft, 4) 
                  - 0.000000000005721892010782867073 * pow(voltageLeft, 5);
  return distanceLeft;
}

float voltageToDistanceFront(float voltageFront) {
  float distanceFront = 326.520326 - 3.495438 * voltageFront + 0.01745371161904565871 * pow(voltageFront, 2)
    - 0.00004547541709635030 * pow(voltageFront, 3)
    + 0.000000059394041692704409989898 * pow(voltageFront, 4)
    - 0.000000000030849183618367244279 * pow(voltageFront, 5);
  return distanceFront;
}

float voltageToDistanceRight(float voltageRight) {
  float distanceRight = 118.473550 + (-1.150951) * voltageRight + 0.00521565514927053390 * pow(voltageRight, 2)
                  - 0.00001241542739366339 * pow(voltageRight, 3) + 0.000000014899084765225949588948 * pow(voltageRight, 4)
                  - 0.000000000007097354980691055009 * pow(voltageRight, 5);
  return distanceRight;
}

void forward(int leftSpeed, int rightSpeed) {
  analogWrite(motorLeftEnablePin, leftSpeed);
  analogWrite(motorRightEnablePin, rightSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  
  digitalWrite(IN4, HIGH);  
}

void stop() {
  analogWrite(motorLeftEnablePin, 0);
  analogWrite(motorRightEnablePin, 0);
}


