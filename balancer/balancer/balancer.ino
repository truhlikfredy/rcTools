#include <TimerOne.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>



//OLED
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define PIN_BTN 6
//#define GRAPH_START   64
#define GRAPH_START   56
#define GRAPH_LEN     (SSD1306_LCDWIDTH - GRAPH_START)
#define GRAPH_ENTRIES 32

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


//ACCEL
#define ACL_LOOP      250
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
sensors_event_t e_old; 


//PPM
#define PPM_PORT=9;


//Encoder
#define PinCLK=2;                   // Used for generating interrupts using CLK signal
#define PinDT=3;                    // Used for reading DT signal
#define PinSW=4;                    // Used for the push button switch
volatile boolean TurnDetected;
volatile long virtualPosition=0;
volatile unsigned int ppm = 1000;
volatile unsigned char ppmPart = 0;




void encoderIsr ()  {                    // Interrupt service routine is executed when a HIGH to LOW transition is detected on CLK
 boolean up;
 if (digitalRead(PinCLK))
   up = digitalRead(PinDT);
 else
   up = !digitalRead(PinDT);
 TurnDetected = true;

   if (up) {
    if (virtualPosition<100) {
      virtualPosition++;
    }
   } else {
    if (virtualPosition>0) {
      virtualPosition--; 
    }
  }
}


void ppmIsr(void) {
  if (ppmPart==0) {
    digitalWrite(9,HIGH);
    Timer1.setPeriod(ppm);
    ppmPart++;
  } else if (ppmPart==1) {
    Timer1.setPeriod(20000-ppm);
    digitalWrite(9,LOW);
    ppmPart--;
  }
}

void setup()   {                
  pinMode(PinCLK,INPUT);
  pinMode(PinDT,INPUT);  
  pinMode(PinSW,INPUT);
  pinMode(PIN_BTN,INPUT);
  digitalWrite(PIN_BTN, HIGH);
  attachInterrupt (0,isr,FALLING);   // interrupt 0 is always connected to pin 2 on Arduino UNO
  pinMode(9,OUTPUT);
  
  Timer1.initialize(ppm);
  Timer1.attachInterrupt(updatePPM);
   
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

   g_input[0] = map(virtualPosition, 0, 100, 1000, 2000);
   g_input[1] = map(virtualPosition, 0, 100, 1000, 2000);
   g_input[2] = map(virtualPosition, 0, 100, 1000, 2000);
   g_input[3] = map(virtualPosition, 0, 100, 1000, 2000);
//   g_PPMOut.update();
 
   TurnDetected = false;    
   display.clearDisplay();   
   display.setCursor(0,0);
   
   display.print("RPM ");
   display.println(g_input[0],DEC);
//   display.println("%");

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

 if (digitalRead(PIN_BTN)==0) {
   maxTotal=0;
   maxAvg=0;
   graphIndex=0;
   for (unsigned char i=0;i<GRAPH_ENTRIES;i++) graph[i]=0;
 }
 
 loop++;
  
//  display.println("%");

//  display.print("ACC:");
//  display.println(250,DEC);
  
//  display.println("Hello, world!");
//  display.println("Hello, world!");
//  display.display();
//  delay(4000);
//  display.clearDisplay();

  // draw the first ~12 characters in the font
//  testdrawchar();
//  display.display();
//  delay(2000);
//  display.clearDisplay();

  // draw scrolling text
//  testscrolltext();
//  delay(2000);
//  display.clearDisplay();  
}


