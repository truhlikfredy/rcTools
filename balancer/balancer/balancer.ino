#include <TimerOne.h>           //https://github.com/PaulStoffregen/TimerOne
#include <SPI.h>
#include <Wire.h>
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

Adafruit_SSD1306 display(OLED_RESET);


//ACCEL
#define ACL_LOOP       250
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
sensors_event_t          e_old; 


//PPM
#define PWM_PORT       9
volatile unsigned int  ppm = 1000;
volatile unsigned char ppmPart = 0;


//Encoder
#define ENCODER_CLK    2      // Used for generating interrupts using CLK signal, it needs to stay 2 because isr0 is binded
#define ENCODER_DT     3      // Used for reading DT signal
#define ENCODER_BTN    6      // Used for reading the push button
volatile boolean       TurnDetected;
volatile long          virtualPosition=0;

struct encoder {
  volatile boolean      updated;
  volatile unsigned int pos_min;
  volatile unsigned int pos;
  volatile unsigned int pos_max;
};


void EncoderIsr()  {                    // Interrupt service routine is executed when a HIGH to LOW transition is detected on CLK
  boolean up;

  TurnDetected = true;

  if (digitalRead(ENCODER_CLK)) {
    up = digitalRead(ENCODER_DT);
  }
  else {
    up = !digitalRead(ENCODER_DT);
  }

  if (up) {
    if (virtualPosition<100) {
      virtualPosition++;
    }
  } 
  else {
    if (virtualPosition>0) {
      virtualPosition--; 
    }
  }
}


void PwmIsr(void) {
 static unsigned int lastValue = 0;
 if (ppmPart==0) {
   digitalWrite(9,HIGH);
   Timer1.setPeriod(ppm);
   lastValue = ppm;
   ppmPart++;
 } 
 else if (ppmPart==1) {
   Timer1.setPeriod(20000-lastValue);
   digitalWrite(9,LOW);
   ppmPart--;
 }
}

void setup()   {                
  pinMode(     PWM_PORT   , OUTPUT);
  pinMode(     ENCODER_CLK, INPUT );
  pinMode(     ENCODER_DT , INPUT );  
  pinMode(     ENCODER_BTN, INPUT );
  digitalWrite(ENCODER_BTN, HIGH  );

  attachInterrupt(0, EncoderIsr, FALLING);   // interrupt 0 is always connected to pin 2 on Arduino UNO
  
  Timer1.initialize(ppm);
  Timer1.attachInterrupt(PwmIsr);
   
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Welcome!"            );
  display.println("Engine Balancer v1.0");
  display.println("by: Anton Krug 2016" );
  display.display();
  delay(2000);

  if (!accel.begin()) {
    display.clearDisplay();
    display.setCursor(0,8);
    display.println("No ADXL456 detected!");
    display.println("Program halted.");
    display.display();
    while(1);
  }
  accel.setRange(ADXL345_RANGE_4_G);
  delay(200);
  accel.getEvent(&e_old);
}


void loop() {

  static unsigned char loop =1;
  static unsigned int  maxX,maxY,maxZ,avg,maxAvg;
  static unsigned long maxTotal,sum=0;
  static unsigned int  graph[GRAPH_ENTRIES];
  static unsigned char graphIndex;
 

  /* Get a new sensor event */ 
  sensors_event_t e; 
  accel.getEvent(&e);

  unsigned int diffX = round(abs(e.acceleration.x - e_old.acceleration.x)*100);
  unsigned int diffY = round(abs(e.acceleration.y - e_old.acceleration.y)*100);
  unsigned int diffZ = round(abs(e.acceleration.z - e_old.acceleration.z)*100);

  maxX=max(maxX,diffX);
  maxY=max(maxY,diffY);
  maxZ=max(maxZ,diffZ);

  sum+=diffX+diffY+diffZ;

  maxTotal=max(maxTotal,maxX+maxY+maxZ);

  if (loop%ACL_LOOP==0) { 
    avg=sum/ACL_LOOP;
    maxAvg=max(maxAvg,avg);
    graph[graphIndex]=avg;
    graphIndex=(graphIndex+1)%GRAPH_ENTRIES;
  }


 if (TurnDetected || loop%ACL_LOOP==0)  { 
   ppm = map(virtualPosition, 0, 100, 1000, 2000);


   TurnDetected = false;    
   display.clearDisplay();   
   display.setCursor(0,0);
   
   display.print("RPM ");
   display.println(ppm,DEC);
   display.println("%");

   display.print("Avg ");
   display.println(avg,DEC);
   
   display.print("MA  ");
   display.println(maxAvg,DEC);

   display.print("Peak");
   display.println(maxTotal,DEC);
   
   for (unsigned char i=0;i<GRAPH_ENTRIES;i++) {
    const unsigned long entry = graph[(i+graphIndex)%GRAPH_ENTRIES];
    const unsigned int  len   = (entry * GRAPH_LEN) / maxAvg;
    display.drawLine(GRAPH_START,     i, 
                     GRAPH_START+len, i, 
                     WHITE);
   }

   display.display();
 }

 if (loop%ACL_LOOP==0) {
   maxX=0;
   maxY=0;
   maxZ=0;
   sum=0;
   loop=0;
   e_old=e;  
 }

 if (digitalRead(ENCODER_BTN)==0) {
   maxTotal=0;
   maxAvg=0;
   graphIndex=0;
   for (unsigned char i=0;i<GRAPH_ENTRIES;i++) graph[i]=0;
 }
 
 loop++;
  
}


