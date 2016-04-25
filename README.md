# rcTools
Small projects handle some aspects of radio controlled devices and make development with them more pleasant.

# 1pinDpad
Arduino library which allows 4 way dpad to be attached to 1 single pin (supports multiple buttons pressed at the same time). Includes debouncing as well. Meant for RC controller emulator where 2 inputs are dedicated for 2 dpads and 4 outputs are driving PWM for each axis. In total they are 6 pins, meant exactly for digispark at mini board.

![4 way Dpad]
(https://raw.githubusercontent.com/truhlikfredy/rcTools/master/screenShots/dpad.jpg)


# PpmPwm
Arduino library to generate PWM signals on mutliple pins with same time offsets and delays like it would be generated from PPM signal.

![pwm signal]
(https://raw.githubusercontent.com/truhlikfredy/rcTools/master/screenShots/ppm.gif)

# Balancer
Automates couple tasks, drives engines with consistent profile and speed ranges and measures vibrations. Graphs them on the LCD as well.

![balancer GUI]
(https://raw.githubusercontent.com/truhlikfredy/rcTools/master/screenShots/balancer1.jpg)