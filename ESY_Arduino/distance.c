// Pin, an dem der Sensor angeschlossen ist
const int analogPin = A0;

// Funktion zur Umrechnung der gemessenen Spannung in den Abstand
float voltageToDistance(float voltage) {
  // Beispiel-Gleichung (ersetze diese mit deinen eigenen Kalibrierungswerten)
  float slope = -10.0; // Beispiel-Slope (Negativ, da die Spannung abnimmt, wenn der Abstand zunimmt)
  float intercept = 27.0; // Beispiel-Intercept

  // Berechne den Abstand mit der linearen Gleichung
  float distance = slope * voltage + intercept;

  return distance;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Lese die Spannung vom Sensor
  int sensorValue = analogRead(analogPin);

  // Konvertiere den ADC-Wert in eine Spannung (0-5V entspricht 0-1023 ADC-Werten)
  float voltage = sensorValue * (5.0 / 1023.0);

  // Konvertiere die Spannung in den Abstand
  float distance = voltageToDistance(voltage);

  // Gib den gemessenen Abstand aus
  Serial.print("Spannung: ");
  Serial.print(voltage);
  Serial.print(" V, Abstand: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(1000); // Warte 1 Sekunde vor der nächsten Messung
}
