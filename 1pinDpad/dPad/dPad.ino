#define sensorPinA         A0
#define serialSpeed        9600
#define debounceIterations 100

unsigned int  directionsA[] = {999 , 206, 646, 308, 230, 65535, 364, 187, 481, 266 };
unsigned char diffA         = 11;

unsigned int  val           = 0;
unsigned char i             = 0;


void setup() {
  Serial.begin(serialSpeed     );
  digitalWrite(sensorPinA, HIGH); 
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


unsigned char getStateA(unsigned int input) {
  static unsigned char previousReturnState = 255;
  static unsigned char previousState       = 255;
  static unsigned int  iterations          = 0;

  unsigned char state = findMatchA(input);
  if (state != previousState) {
    iterations = 0;
  } else if ( state==previousState && iterations<65535 ) {
    iterations ++;
  }
  previousState = state;

  if (iterations > debounceIterations) {
    previousReturnState = state;
  }
  return previousReturnState;
}


void loop() { 
  static unsigned char stateA    = 0;
  static unsigned char stateAold = 0;

  stateA=getStateA(analogRead(sensorPinA));
  
  if (stateA!=stateAold) {
    Serial.println(stateA);
  }
  stateAold=stateA;
}


