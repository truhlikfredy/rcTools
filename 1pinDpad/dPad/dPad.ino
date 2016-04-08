#define sensorPin   A0
#define serialSpeed 9600

unsigned int  directionsA[] = {999 , 206, 646, 308, 230, 65535, 364, 187, 481, 266 };
unsigned char diffA         = 11;

unsigned int  val           = 0;
unsigned char i             = 0;


void setup() {
  Serial.begin(serialSpeed);
  digitalWrite(A0, HIGH); 
}


unsigned char findMatchA(unsigned int input) {
  //find if the input is in interval X and X+diff for each entry in the lookup table
  for (i=0; i<10; i++) {
    if (directionsA[i]<=input && (directionsA[i]+diffA)>=input) {
      return(i);
    }    
  }
  return(0);
}


void loop() { 
  Serial.println( findMatchA( analogRead(sensorPin) ) );
  delay(20);
}


