#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
long circle = 0;
int circleTarget = 1000;

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
const int sensorPinFront = A1;  // Sensor-Pin am Arduino GP2Y0A21YK0F

// Start- und Stop-Taster
const int startButtonPin = 2;
const int stopButtonPin = 3;

// Geschwindigkeit der Motoren (0-255)
int motorSpeed = 150; // Initialer Wert, wird später dynamisch angepasst

// Sensorwerte deklarieren
int sensorValueFront;
int sensorValueLeft;
int sensorValueRight;

bool isRunning = false;

// Arithmetischen Mittelwert der Sensoren
int numReadings = 31; // Anzahl der Messungen für den Durchschnitt 0-255 aufgrund des Datentypen int
long sensorSumFront = 0; // Summe der Sensorwerte für den Durchschnitt
long sensorSumRight = 0; // Summe der Sensorwerte für den Durchschnitt
long sensorSumLeft = 0; // Summe der Sensorwerte für den Durchschnitt

void setup() {
  // Initialisierung der seriellen Kommunikation
  Serial.begin(115200);

  //LCD
  lcd.init();
  lcd.backlight();

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
  // Setze die Summen zurück
  sensorSumFront = 0;
  sensorSumRight = 0;
  sensorSumLeft = 0;

  // Führe die Messungen durch
  for (int i = 0; i < numReadings; i++) {
    sensorValueFront = analogRead(sensorPinFront);
    sensorValueLeft = analogRead(sensorPinLeft);
    sensorValueRight = analogRead(sensorPinRight);

    // Addiere die Werte zur Summe
    sensorSumFront += sensorValueFront;
    sensorSumLeft += sensorValueLeft;
    sensorSumRight += sensorValueRight;
  }

  // Berechne den Durchschnitt
  short avgSensorFront = sensorSumFront / numReadings;
  short avgSensorLeft = sensorSumLeft / numReadings;
  short avgSensorRight = sensorSumRight / numReadings;

  //Konvertiere die Spannung in den Abstand
  int distanceFront = constrain(voltageToDistanceFront(avgSensorFront), 0, 130); // 0 - 130cm Reichweite vom großen Sensor
  int distanceLeft = constrain(voltageToDistanceLeft(avgSensorLeft), 0, 70);     // 0 - 70cm Reichweite vom kleinen Sensor
  int distanceRight = constrain(voltageToDistanceRight(avgSensorRight), 0, 70);  // 0 - 70cm Reichweite vom kleinen Sensor

  // Einstellung des Motor-Speed basierend auf dem Frontsensor
  motorSpeed = map(distanceFront, 0, 130, 100, 255); // Ändern Sie den Wertebereich entsprechend Ihrer Anforderungen

  // Ausgabe der Sensorwerte auf der seriellen Schnittstelle
  Serial.print("Links: ");
  Serial.print(distanceLeft);
  Serial.print(" cm | Vorne: ");
  Serial.print(distanceFront);
  Serial.print(" cm | Rechts: ");
  Serial.print(distanceRight);
  Serial.println(" cm  ");

  if (millis() - circle >= circleTarget) {
    circle = millis();
    lcd.clear();
    
    lcd.setCursor(0, 0); 
    lcd.print("L ");
    lcd.print(distanceLeft);
    lcd.print("cm");
    
    lcd.setCursor(10, 0);
    lcd.print("R ");
    lcd.print(distanceRight);
    lcd.print("cm");  
    
    lcd.setCursor(5, 1);
    lcd.print("F ");
    lcd.print(distanceFront);
    lcd.print("cm");
  }
  
  if (isRunning) {
    // Überprüfen, ob eine Kurve erkannt wurde
    if (distanceLeft > 30 && distanceRight > 30) {
      // Wenn eine Kurve erkannt wurde, die Geschwindigkeiten der Motoren anpassen, um die Kurve zu fahren
      adjustForCurve(distanceLeft, distanceRight);
    } else {
      // Ansonsten normales Fahrverhalten beibehalten
      float korrektur = calculateCorrection(sensorSumLeft, sensorSumRight);
      adjustMotorSpeeds(korrektur);
    }
  } else {
    stop();
  }
}

void adjustForCurve(int distanceLeft, int distanceRight) {
  // Berechne die Differenz zwischen den Abständen
  int difference = distanceLeft - distanceRight;
  float intensityFactor = 9;

  // Berechne die angepassten Geschwindigkeiten
  int leftSpeed = motorSpeed;
  int rightSpeed = motorSpeed;
  if (difference > 0) {
    // Das Hindernis ist näher am linken Sensor, lenke nach rechts
    rightSpeed = motorSpeed - difference * intensityFactor; // Je größer die Differenz, desto mehr lenken nach rechts
  } else if (difference < 0) {
    // Das Hindernis ist näher am rechten Sensor, lenke nach links
    leftSpeed = motorSpeed + difference * intensityFactor; // Je größer die Differenz, desto mehr lenken nach links
  }

  // Stellen Sie sicher, dass die Geschwindigkeiten innerhalb des gültigen Bereichs liegen
  leftSpeed = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);

  // Motorgeschwindigkeiten anpassen
  forward(leftSpeed, rightSpeed);
}


float calculateCorrection(float sensorSumRight, float sensorSumLeft) {
  // Berechne den Korrekturwert basierend auf dem Unterschied zwischen links und rechts
  float zielAbstand = 50.0;  // Passe diesen Wert entsprechend deiner Anforderungen an
  float differenz = sensorSumLeft - sensorSumRight;

  // Einen proportionalen Regler, um die Korrektur zu berechnen
  float korrektur = differenz / zielAbstand;

  // Beschränkung der Korrektur auf einen vernünftigen Bereich
  korrektur = constrain(korrektur, -1.0, 1.0);
  return korrektur;
}

void adjustMotorSpeeds(float korrektur) {
  // Passe die Motorgeschwindigkeiten basierend auf dem Korrekturwert an
  int minSpeed = 100; // Minimale Geschwindigkeit
  int maxSpeedChange = 150; // Begrenzung der Geschwindigkeitsänderung

  int linkeMotorGeschwindigkeit = constrain(motorSpeed + korrektur * maxSpeedChange, minSpeed, 255);
  int rechteMotorGeschwindigkeit = constrain(motorSpeed - korrektur * maxSpeedChange, minSpeed, 255);

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


