unsigned int sensorPin = A0;    
unsigned int val =0;
unsigned int i = 0;

void setup() {
  Serial.begin(9600);
  //digitalWrite(sensorPin, HIGH); 
  Serial.println("Started.");
}


void loop() {
  unsigned int min=65535, max=0;
  Serial.print("Press button, new measurement will be begin in: ");
  for (i=5;i>0;i--) {
    Serial.print(i);
    Serial.print(" ");
    delay(1000);
  }
  Serial.println(" Taking values for about 7 seconds... ");
  
  for (i=0;i<65535;i++) {
    val = analogRead(sensorPin);  
    if (val<min) min=val;
    if (val>max) max=val;
  }

  Serial.print("Min: ");
  Serial.print(min);
  Serial.print(" Max: ");
  Serial.print(max);
  Serial.print(" Diff: ");
  Serial.print(max-min);
  Serial.println();  
}
