//comment out to disable serial output
//#define serialSpeed  9600

#define SWITCH_A     9         //mode A button
#define SWITCH_B     8         //programer switch button
#define OUTPUT_FC    7         //OSD will communicate with FC
#define OUTPUT_FTDI  6         //OSD will communicate with FTDI usb converter
#define sensorPinA   A0        //dpad Left
#define sensorPinB   A1        //dpad Right

#include <PpmPwm.h>            //https://github.com/truhlikfredy/rcTools/tree/master/PpmPwm
#include <DPad1.h>             //https://github.com/truhlikfredy/rcTools/tree/master/1pinDpad/DPad1
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>      //https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_SSD1306.h>  //https://github.com/adafruit/Adafruit_SSD1306
#include <Debounce.h>          //https://github.com/truhlikfredy/rcTools/tree/master/Debounce a modified http://playground.arduino.cc/Code/Debounce


#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);


// Because I messed up and I run out of 10k resitors I used two 22k in paraler and then
// I connected them to wrong buttons, so I had to give second dPad different values
//                               0     1    2    3    4    5      6    7    8    9
unsigned int  directionsA[] = {1010 , 365, 480, 414, 608, 65535, 765, 258, 312, 282 };
unsigned int  directionsB[] = {1010 , 419, 488, 366, 757, 65535, 601, 276, 306, 256 };


DPad1 left (sensorPinA, &directionsA[0]);
DPad1 right(sensorPinB, &directionsB[0]);
PpmPwm pwm(4, 10, 11, 12, 13);
Debounce debA = Debounce( 20 , SWITCH_A ); 
Debounce debB = Debounce( 20 , SWITCH_B ); 


void setup() {
  //setup serial output
  #if defined(serialSpeed)
  Serial.begin(serialSpeed);
  #endif

  //setup push button debouncers
  pinMode(     SWITCH_A,    INPUT );
  digitalWrite(SWITCH_A,    HIGH  );
  
  pinMode(     SWITCH_B,    INPUT );
  digitalWrite(SWITCH_B,    HIGH  );

  //setup outputs
  pinMode(     OUTPUT_FC,   OUTPUT);
  digitalWrite(OUTPUT_FC,   LOW   );

  pinMode(     OUTPUT_FTDI, OUTPUT);
  digitalWrite(OUTPUT_FTDI, LOW   );

  //setup pwm outputs
  pwm.updatePortMs(0,500);   
  pwm.updatePortMs(1,500);   
  pwm.updatePortMs(2,500);   
  pwm.updatePortMs(3,500);  
  pwm.initializePwmOutput();

  //setup dpads internal pull-ups 
  // Set sensor pins to pull ups if you are not using external pull-ups
  //  digitalWrite(sensorPinA, HIGH); 
  //  digitalWrite(sensorPinB, HIGH); 

  //setup LCD
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x78 (for the 128x32)
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("RC controller");
  display.println("Emulator v1.0");
  display.println("by Anton Krug");
  display.println("2016");  
  display.display();
  delay(1000);
}


void loop() { 
  static unsigned int chThr   = 0, 
                      chYaw   = 0, 
                      chPitch = 0, 
                      chRoll  = 0;
                      
  static boolean      btnA    = false,
                      btnB    = false,
                      osdFc   = true;

  //read dpad states
  left .aquireState();
  right.aquireState();

  btnA = debA.update();
  btnB = debB.update();

  //when any state changed update values and graphics for both
  if (left.isStateChanged() || right.isStateChanged() || btnA || btnB) {

    //Print out states to serial line (it's not needed but it doesn't hurt)
    #if defined(serialSpeed)
    Serial.print(left.getState());
    Serial.print(" ");
    Serial.print(right.getState());
    Serial.println();
    #endif

    //switch state between OSD comunicating with FC and FTDI chip
    if ( btnA && debA.read()==0 ) {
      osdFc = !osdFc;

      //turn off both states for moment and wait for the relays / transistors to finish transition
      digitalWrite(OUTPUT_FC,   LOW );
      digitalWrite(OUTPUT_FTDI, LOW );
      delay(100);

      //set the outpus depending on the state
      if (osdFc) {
        digitalWrite(OUTPUT_FC,   HIGH);
        digitalWrite(OUTPUT_FTDI, LOW );
      }
      else {
        digitalWrite(OUTPUT_FC,   LOW );
        digitalWrite(OUTPUT_FTDI, HIGH);        
      }
    }

    //transfer the left dPad state into PPM type PWM values
    switch (left.getState()) {
      case 0: chThr=1500;   chYaw=1500;  break;
      case 1: chThr=1000;   chYaw=1000;  break;      
      case 2: chThr=1000;   chYaw=1500;  break;      
      case 3: chThr=1000;   chYaw=2000;  break;      
      case 4: chThr=1500;   chYaw=1000;  break;      
      case 5: chThr=1500;   chYaw=1500;  break;      
      case 6: chThr=1500;   chYaw=2000;  break;      
      case 7: chThr=2000;   chYaw=1000;  break;      
      case 8: chThr=2000;   chYaw=1500;  break;      
      case 9: chThr=2000;   chYaw=2000;  break;      
    }

    //transfer the right dPad state into PPM type PWM values
    switch (right.getState()) {
      case 0: chPitch=1500; chRoll=1500; break;
      case 1: chPitch=1000; chRoll=1000; break;      
      case 2: chPitch=1000; chRoll=1500; break;      
      case 3: chPitch=1000; chRoll=2000; break;      
      case 4: chPitch=1500; chRoll=1000; break;      
      case 5: chPitch=1500; chRoll=1500; break;      
      case 6: chPitch=1500; chRoll=2000; break;      
      case 7: chPitch=2000; chRoll=1000; break;      
      case 8: chPitch=2000; chRoll=1500; break;      
      case 9: chPitch=2000; chRoll=2000; break;      
    }

    //update the pwm outputs
    pwm.updatePortMsWithOffset(0, chThr  );
    pwm.updatePortMsWithOffset(1, chRoll );
    pwm.updatePortMsWithOffset(2, chPitch);
    pwm.updatePortMsWithOffset(3, chYaw  );

    //update OLED display
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Left:    ");  display.println(left.getState()    );
    display.print("Right:   ");  display.println(right.getState()   );
    display.print("chThr:   ");  display.println(chThr              );
    display.print("chYaw:   ");  display.println(chYaw              );
    display.print("chPitch: ");  display.println(chPitch            );
    display.print("chRoll:  ");  display.println(chRoll             );
    display.print("BtnA:    ");  display.print(  debA.read()        );
    display.print(" BtnB:   ");  display.println(debB.read()        );
    display.print("OSD com: ");  display.println((osdFc?"FC":"FTDI"));

    //draw the PPM lines for each chanell
    display.drawLine(80, 2*8+4, map(chThr,  1000, 2000, 85, 128), 2*8+4, WHITE);
    display.drawLine(80, 3*8+4, map(chYaw,  1000, 2000, 85, 128), 3*8+4, WHITE);
    display.drawLine(80, 4*8+4, map(chPitch,1000, 2000, 85, 128), 4*8+4, WHITE);
    display.drawLine(80, 5*8+4, map(chRoll, 1000, 2000, 85, 128), 5*8+4, WHITE);

    //try to visualy represent dPad directions
    display.drawLine(80+12, 8, 80+12+ map(chYaw,  1000, 2000, -10, +10) , 8 + map(chThr,   1000, 2000, +6, -6), WHITE);
    display.drawLine(80+36, 8, 80+36+ map(chRoll, 1000, 2000, -10, +10) , 8 + map(chPitch, 1000, 2000, +6, -6), WHITE);
        
    display.display();
  }
}


