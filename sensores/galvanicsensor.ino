int GSR = A0;
int sensorValue;

void setup() {
  Serial.begin(9600);
  delay(5000);
}

void loop() {
  sensorValue = analogRead(GSR);
  float conductiveVoltage = sensorValue * (5.0 / 1023.0);
  
  Serial.print("SensorValue=");
  Serial.print(sensorValue); 
  Serial.println();
  delay(5000);
}
