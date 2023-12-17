const int sensorPinFront = A5; // Sensor-Pin am Arduino GP2Y0A02YK0F
const int sensorPinRight = A3; // Sensor-Pin am Arduino GP2Y0A21YK0F
const int sensorPinLeft = A4; // Sensor-Pin am Arduino GP2Y0A21YK0F
int sensorValueFront; // Variable zur Aufnahme des Sensorwerts
int sensorValueRight; // Variable zur Aufnahme des Sensorwerts
int sensorValueLeft; // Variable zur Aufnahme des Sensorwerts

int numReadings = 50; // Anzahl der Messungen für den Durchschnitt
int sensorSumFront = 0; // Summe der Sensorwerte für den Durchschnitt
int sensorSumRight = 0; // Summe der Sensorwerte für den Durchschnitt
int sensorSumLeft = 0; // Summe der Sensorwerte für den Durchschnitt

void setup() {
  Serial.begin(9600); // Starte die serielle Kommunikation
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

    //delay(1); // Warte kurz zwischen den Messungen
  }

  // Berechne den Durchschnitt
  float avgSensorFront = float(sensorSumFront) / numReadings;
  float avgSensorLeft = float(sensorSumLeft) / numReadings;
  float avgSensorRight = float(sensorSumRight) / numReadings;

  // Gib den Durchschnitt aus
  Serial.print("Average Left: ");
  Serial.print(avgSensorLeft);
  Serial.print("  Average Front: ");
  Serial.print(avgSensorFront);
  Serial.print("  Average Right: ");
  Serial.println(avgSensorRight);
  //Vergleich mit einem gelesenden Wert
  Serial.print("Left: ");
  Serial.print(sensorValueLeft);
  Serial.print("  Front: ");
  Serial.print(sensorValueFront);
  Serial.print("  Right: ");
  Serial.println(sensorValueRight);

  delay(3000); // Warte 1 Sekunde
}
