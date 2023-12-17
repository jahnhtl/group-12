
int IN1 = 4;
int IN2 = 5;
int IN3 = 6;
int IN4 = 7;

const int startButtonPin = 2;  // Pin für den Start-Taster
const int stopButtonPin = 3;   // Pin für den Stop-Taster

volatile bool isRunning = false;  // Variable zur Verfolgung des Systemzustands

void setup() {
  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(stopButtonPin, INPUT_PULLUP);

	// Funktionen zu registrieren, die bei fallender Flanke 
  attachInterrupt(digitalPinToInterrupt(startButtonPin), startButtonInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(stopButtonPin), stopButtonInterrupt, FALLING);
}

void loop() {
  // Hauptprogrammcode hier
  if (isRunning) {
    // Code für den laufenden Zustand
    forward();
  } else {
    // Code für den gestoppten Zustand
    stop();
  }
}

void startButtonInterrupt() {
  // Funktion, die aufgerufen wird, wenn der Start-Taster gedrückt wird
  isRunning = true;
}

void forward(){
  analogWrite(9, 100);
  analogWrite(10, 100);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(1000);
}

void stop(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(1000);
}

void stopButtonInterrupt() {
  // Funktion, die aufgerufen wird, wenn der Stop-Taster gedrückt wird
  isRunning = false;
  Serial.println("Stop-Taster gedrückt");
}