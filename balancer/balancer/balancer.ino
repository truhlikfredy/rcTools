#include <SPI.h>
#include <Wire.h>
#include <PpmPwm.h>             //https://github.com/truhlikfredy/rcTools/tree/master/PpmPwm
#include <Adafruit_GFX.h>       //https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_SSD1306.h>   //https://github.com/adafruit/Adafruit_SSD1306
#include <Adafruit_Sensor.h>    //https://github.com/adafruit/Adafruit_Sensor
#include <Adafruit_ADXL345_U.h> //https://github.com/adafruit/Adafruit_ADXL345

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


//OLED
#define OLED_RESET     4
#define GRAPH_START    56
#define GRAPH_LEN      (SSD1306_LCDWIDTH - GRAPH_START)
#define GRAPH_ENTRIES  32
#define SPLASH_DELAY   2000

Adafruit_SSD1306 display(OLED_RESET);

struct History {
  unsigned int  graph[GRAPH_ENTRIES];
  unsigned char index;
} history;


//ACCEL
#define ACL_LOOP       250
boolean                  aclEnabled   = true;
Adafruit_ADXL345_Unified accel        = Adafruit_ADXL345_Unified(12345);
sensors_event_t          event; 


struct Acel {
  unsigned int  x = 0;
  unsigned int  y = 0;
  unsigned int  z = 0;
  unsigned int avg = 0;
  unsigned int maxAvg =0;
  unsigned long peak;
  unsigned long sum=0;
  unsigned char loop =1;
  sensors_event_t  event_old; 
} acel;



//PPM pwm
PpmPwm pwm(4,9,10,11,12);  //4 ports D9, D10, D11, D12


//Encoder
#define ENCODER_CLK    2      // Used for generating interrupts using CLK signal, it needs to stay 2 because isr0 is binded
#define ENCODER_DT     3      // Used for reading DT signal
#define ENCODER_BTN    6      // Used for reading the push button
struct Encoder {
  volatile boolean      updated;
  volatile unsigned int pos_min;
  volatile unsigned int pos;
  volatile unsigned int pos_max;
} encoder;


//Menu
typedef enum {ROOT, 
              MANUAL, 
              AUTO_SET, 
              AUTO_SET_START, 
              AUTO_SET_END, 
              AUTO_SET_PROFILE, 
              AUTO_RUN,
              AUTO_FINISHED,
              ESC_CALIBRATE, 
              INVALID
              } menuModeType;
              
//menuModeType menuMode    = ROOT;
menuModeType menuMode    = MANUAL;
menuModeType menuModeOld = INVALID;


void EncoderIsr()  {                    // Interrupt service routine is executed when a HIGH to LOW transition is detected on CLK
  boolean up;

  encoder.updated = true;

  if (digitalRead(ENCODER_CLK)) {
    up = digitalRead(ENCODER_DT);
  }
  else {
    up = !digitalRead(ENCODER_DT);
  }

  if (up) {
    if (encoder.pos < encoder.pos_max) {
      encoder.pos++;
    }
  } 
  else {
    if (encoder.pos > encoder.pos_min) {
      encoder.pos--; 
    }
  }
}


void setupEncoder() {
  pinMode(     ENCODER_CLK, INPUT );
  pinMode(     ENCODER_DT , INPUT );  
  pinMode(     ENCODER_BTN, INPUT );
  digitalWrite(ENCODER_BTN, HIGH  );
  
  attachInterrupt(0, EncoderIsr, FALLING);   // interrupt 0 is always connected to pin 2 on Arduino UNO  

  encoder.pos_min = 0;
  encoder.pos_max = 100;  
}


void setupPwm() {
  pwm.initializePwmOutput();
}


void setupLcd() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Welcome!"            );
  display.println("Engine Balancer v1.0");
  display.println("by: Anton Krug 2016" );
  display.display();
  delay(SPLASH_DELAY);  
}


void setupAccel() {
  if (!accel.begin()) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("No ADXL456 detected!");
    display.println("Program halted.");
    display.println("Press button to");
    display.println("continue without it.");
    display.display();
    while(aclEnabled) {
      if ( digitalRead(ENCODER_BTN) == 0 )  {
        aclEnabled = false;
      }
    }
  }
  accel.setRange(ADXL345_RANGE_4_G);
  delay(200);
  accel.getEvent(&acel.event_old);  
}


void setup()   {                
  setupEncoder();
  setupPwm();
  setupLcd();
  setupAccel();
}


void measure() {  
  if (aclEnabled) {
    
    // Get sensor data
    accel.getEvent(&event);

    unsigned int diffX = round(abs(event.acceleration.x - acel.event_old.acceleration.x)*100);
    unsigned int diffY = round(abs(event.acceleration.y - acel.event_old.acceleration.y)*100);
    unsigned int diffZ = round(abs(event.acceleration.z - acel.event_old.acceleration.z)*100);

    acel.x=max(acel.x, diffX);
    acel.y=max(acel.y, diffY);
    acel.z=max(acel.z, diffZ);

    acel.sum += diffX + diffY + diffZ;

    acel.peak = max(acel.peak, acel.x + acel.y + acel.z);
  }

  if (acel.loop%ACL_LOOP == 0) { 
    acel.avg                     = acel.sum/ACL_LOOP;
    acel.maxAvg                  = max(acel.maxAvg, acel.avg);
    history.graph[history.index] = acel.avg;
    history.index                = (history.index+1) % GRAPH_ENTRIES;
  } 
}

void measurePost() {
  if ( acel.loop%ACL_LOOP == 0 ) {
    acel.x          = 0;
    acel.y          = 0;
    acel.z          = 0;
    acel.sum        = 0;
    acel.loop       = 0;
    acel.event_old  = event;  
  }
  
  if ( digitalRead(ENCODER_BTN) == 0 ) {
    acel.peak     = 0;
    acel.maxAvg   = 0;
    history.index = 0;
    for (unsigned char i=0;i<GRAPH_ENTRIES;i++) {
      history.graph[i]=0;
    }
  }
  
  acel.loop++;  
}


void displayGraph(unsigned int rpm, bool percentage) {
  encoder.updated = false;    
  
  display.clearDisplay();   
  display.setCursor(0,0);
  
  display.print("RPM ");
  display.print(rpm, DEC);
  display.println((percentage)?"%":"");
  
  display.print("Avg ");
  display.println(acel.avg,    DEC);
  
  display.print("MA  ");
  display.println(acel.maxAvg, DEC);
  
  display.print("Peak");
  display.println(acel.peak,   DEC);
  
  for (unsigned char i=0;i<GRAPH_ENTRIES;i++) {
    const unsigned long entry = history.graph[(i+history.index)%GRAPH_ENTRIES];
    const unsigned int  len   = (entry * GRAPH_LEN) / acel.maxAvg;
    display.drawLine(GRAPH_START,     i, 
                     GRAPH_START+len, i, 
                     WHITE);
  }
  
  display.display();
}


void menuChangeEncoderSettings() {
  if (menuMode!=menuModeOld) {
    switch (menuMode) {
      
      case ROOT:
        encoder.pos     = 0;
        encoder.pos_min = 0;
        encoder.pos_max = 2;
        break;
       
    }    
  }
  menuModeOld=menuMode;  
}


void menuRoot() {
  display.clearDisplay();
  display.setCursor(0,0);  
  switch (encoder.pos) {
    
    case 0:
      display.println("Manual run");
      break;
      
    case 1:
      display.println("Automated run");
      break;

    case 2:
      display.println("Calibrate ESC");
      break;
      
      
  }
  display.display();  
}


void menuManual() {
  measure();
  if (encoder.updated || acel.loop%ACL_LOOP==0)  { 
    pwm.updatePortPercentage(0,encoder.pos);
    //    displayGraph(encoder.pos, true);
    displayGraph(pwm.getPortMsWithOffset(0), false);
  }  
  measurePost();  
}


void menuDisplay() {
    switch (menuMode) {
      
      case ROOT:
        menuRoot();
        break;

      case MANUAL:
        menuManual();
        break;
    }    
}

void loop() {
  menuChangeEncoderSettings();
  menuDisplay();
  
}


