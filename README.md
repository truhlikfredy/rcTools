# rcTools
Small projects handle some aspects of radio controlled devices and make development with them more pleasant.

# 1pinDpad
Arduino library which allows 4 way dpad to be attached to 1 single pin (supports multiple buttons pressed at the same time). Includes debouncing as well. Meant for RC controller emulator where 2 inputs are dedicated for 2 dpads and 4 outputs are driving PWM for each axis. In total they are 6 pins, meant exactly for digispark at mini board.

![4 way Dpad](/screenShots/dpad.jpg)


![schematic](r/1pinDpad/schematic1.png)

![diagram](/1pinDpad/dpad1.png)

I'm not sure how exatly this product is made but probably with right values this library work with it. Probably fastes way to make this kind of dpad is to solder it quickly by hand on small board.

https://www.virtuabotix.com/product/virtuabotix-1-pin-dpad-1-0-versalino-control-arduino-circuits/



# rcControllerEmulator
![emulator](/screenShots/emulator1.jpg)

![emulator2](/screenShots/emulator2.jpg)

Using PpmPwm and 1pinDpad libraries I managed to create RC controller emulator, this emulator can be connected directly into the flight controller. This allowed me build a simple non-flyable drone. This is a very handy tool to test software with no risks. If this would be flyable drone you would have these following risks.

*  Danger from propellers (wasting time with removing them)
*  My drone is not meant to idle disarmed for hours (some components require some air circulation as cooling)
*  Requires elaborate setup, drone itself needs a power source, video receiver needs another power source, LCD display needs yet another power source, RC transmitter needs guess what, a power source. And either into hassle and maintain many LiPo batteries. So after experiments you need to go and discharge the residual charge, when you are using it you need be careful not to discharge them too much. If some features require multiple days you need to store LiPo safely because of the explosion hazards. Or you will make extension leads to power all components from single power source, but then you need to modify you RC transmitter to accept AUX power.
*  If you want to fly next day and you want to work on the firmware day before you need to spend extra time backing up eeprom settings and last stable firmware flash file.
*  You need a lot of desk space to fit laptop, drone, video reciever, power extension/harness and RC transmitter together on a desk.

Because non-flyable drone was never meant to fly, some parts which are meant to transfer data wirelessly now can be connected directly (less components and less power sources needed). It's compact on a desk and to store away, it's safe to leave it powered for prolonged periods, even over the night (no risk of LiPos explosions and some components have extra heatsinks which would be too heavy for flyable drone). You can experiment with firmware without worry that it could affect next day flying. No hassle with propellers. It is fast to setup, requires only 1 power jack and 1 ISP connector to flash the device.


# PpmPwm
Arduino library to generate PWM signals on mutliple pins with same time offsets and delays like it would be generated from PPM signal.

![pwm signal](/screenShots/ppm.gif)

# Balancer
Automates couple tasks, drives engines with consistent profile and speed ranges and measures vibrations. Graphs them on the LCD as well.

![balancer GUI](/screenShots/balancer1.jpg)

Prototype PCB:

![balancer PCB](/screenShots/balancer2.jpg)
