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
const int sensorPinLeft = A4;   // Sensor-Pin am Arduino GP2Y0A02YK0F
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
  if (isRunning) {
    sensorValueFront = analogRead(sensorPinFront); // Lies den analogen Wert vom Sensor
    sensorValueLeft = analogRead(sensorPinLeft); // Lies den analogen Wert vom Sensor
    sensorValueRight = analogRead(sensorPinRight); // Lies den analogen Wert vom Sensor

    int leftSpeed = map(sensorValueLeft, 0, 1023, 0, 255) - 255;
    int rightSpeed = map(sensorValueRight, 0, 1023, 0, 255) - 255;

    Serial.print("Left: ");
    Serial.print(sensorValueLeft);
    Serial.print(" | Front:");
    Serial.print(sensorValueFront);
    Serial.print(" | Right:");
    Serial.print(sensorValueRight);
    Serial.println();

    if (sensorValueFront > 15) {
      forward(leftSpeed, rightSpeed);
    } else {
      stop();
    }
  } else {
    stop();
  }
}

void startButtonInterrupt() {
  isRunning = true;
}

void stopButtonInterrupt() {
  isRunning = false;
}

void forward(int leftSpeed, int rightSpeed) {
  analogWrite(motorLeftEnablePin, abs(leftSpeed));
  analogWrite(motorRightEnablePin, abs(rightSpeed));
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stop() {
  analogWrite(motorLeftEnablePin, 0);
  analogWrite(motorRightEnablePin, 0);
}
