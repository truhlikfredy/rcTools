int sensorPin = A0;    
int sensorValue =0;
unsigned int i = 0, match =0, min = 0;

void setup() {
  Serial.begin(9600);
  digitalWrite(A0, HIGH); 
}

int directions[] = {1001 , 210, 650, 311, 233, 65535, 368, 190, 483, 270  };

void loop() {
  sensorValue = analogRead(sensorPin);
  sensorValue = sensorValue + analogRead(sensorPin);
  sensorValue = sensorValue + analogRead(sensorPin);
  sensorValue = sensorValue + analogRead(sensorPin);
  sensorValue = sensorValue / 4;

  min = 65535;

  for (i=0; i<10; i++) {
    int diff = abs(sensorValue - directions[i]);
    if ( diff < min ) {
      match = i;
      min = diff;
    }
  }
  Serial.print(match);
  Serial.print(" ");
  Serial.println(sensorValue);
  
  delay(20);
}
