#define serialSpeed        115200

#define sensorPinA         A0
#define sensorPinB         A1

unsigned int  directionsA[] = {999 , 206, 646, 308, 230, 65535, 364, 187, 481, 266 };

class DPad {
  
private:

  unsigned char port;
  unsigned int  *directionsPtr;
  unsigned char diff;
  unsigned char globalState;
  unsigned char globalStateOld;
  unsigned int  debounce;

  unsigned char previousState;
  unsigned int  iterations;
  

  unsigned char findMatch(unsigned int input) {
    //find if the input is in interval X and X+diff for each entry in the lookup table
    for (unsigned char i=0; i<10; i++) {
      if (*(directionsPtr+i)<=input && (*(directionsPtr+i)+diff)>=input) {
        return(i);
      }    
    }
    return(0);
  }


public:


  DPad(unsigned char port, unsigned int *directionsPtr, unsigned char diff=11, unsigned int debounce=150) {
    this->port           = port;
    this->diff           = diff;
    this->directionsPtr  = directionsPtr;
    this->debounce       = debounce;
    
    this->globalState    = 255;
    this->globalStateOld = 255;
    this->previousState  = 255;
    this->iterations     = 0;
  }


  void aquireState() {
    globalStateOld = globalState;
  
    unsigned char state = findMatch(analogRead(port));
    if (state != previousState) {
      iterations = 0;
    } else if ( state==previousState && iterations<65535 ) {
      iterations ++;
    }
    previousState = state;
  
    if (iterations > debounce) {
      globalState = state;
    }
  }


  boolean isStateChanged() {
    return globalState != globalStateOld;
  }


  unsigned char getState() {
    return globalState;
  }

};


//DPad left (sensorPinA, &directionsA[0], diffA, debounceIterations);
//DPad right(sensorPinB, &directionsA[0], diffA, debounceIterations);
DPad left (sensorPinA, &directionsA[0]);
DPad right(sensorPinB, &directionsA[0]);


void setup() {
  Serial.begin(serialSpeed     );
  digitalWrite(sensorPinA, HIGH); 
  digitalWrite(sensorPinB, HIGH); 
}


void loop() { 
  left .aquireState();
  right.aquireState();
  
  if (left.isStateChanged() || right.isStateChanged()) {
    Serial.print(left.getState());
    Serial.print(" ");
    Serial.print(right.getState());
    Serial.println();
    Serial.flush();
  }
}


