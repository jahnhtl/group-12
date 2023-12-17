const int sensorPin = A0; // Sensor-Pin am Arduino
int sensorValue; // Variable zur Aufnahme des Sensorwerts

void setup() {
  Serial.begin(9600); // Starte die serielle Kommunikation
}

void loop() {
  sensorValue = analogRead(sensorPin); // Lies den analogen Wert vom Sensor

  Serial.print("Sensorwert: ");
  Serial.println(sensorValue);

  delay(1000); // Warte 1 Sekunde
}