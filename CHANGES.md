indi-gpio_drv8833_focus - Source origins and changes

Project adaptation by dom128, 2022-02-24.

INDI focuser driver and build configuration

GPIO_DRV8833_Focuser.cpp and GPIO_DRV8833_Focuser.h were adapted from the
GPIO PWM focuser driver:
https://github.com/sywong2000/indi-gpio_pwm_focus

The adaptation renames the driver for DRV8833 stepper control, updates the
GPIO configuration and integrates a four-pin Stepper instance. Movement
requests set the motor speed and convert duration into a signed step count.
The abort operation calls the added motor-release method. The header adds
the Stepper include and members for the motor instance and steps per revolution.

CMakeLists.txt, config.h.cmake and indi_gpio_drv8833_focus.xml.cmake were
adapted from the same driver project for the new driver names, version
settings and build. The executable includes Stepper.cpp and bcm2835.c.

Stepper control

Stepper.cpp and Stepper.h were adapted from RPI-Stepper, a Raspberry Pi port
of the Arduino Stepper library:
https://github.com/abewieland/RPI-Stepper
https://github.com/arduino-libraries/Stepper

The project adds Stepper::release() in Stepper.cpp to set motor-control GPIO
outputs LOW, and its public declaration in Stepper.h. Other changes to the
imported implementation are minor comments and whitespace. Raspberry Pi GPIO
access, timing and stepping sequences were already present in RPI-Stepper.

Bundled GPIO library

bcm2835.c and bcm2835.h are unchanged copies from bcm2835 version 1.71:
https://www.airspayce.com/mikem/bcm2835/bcm2835-1.71.tar.gz

CMake helpers

cmake_modules/CMakeCommon.cmake, FindINDI.cmake and UnityBuild.cmake match
unchanged copies in the upstream GPIO PWM driver repository linked above.
CMakeCommon.cmake also matches the helper distributed with INDI v1.8.5:
https://github.com/indilib/indi

Publication preparation, 2026-10-18

Added component attributions and licence texts in LICENSE, and dated
modification notices to the four adapted focuser and Stepper source files.
Original source notices are retained. These notices document the historical
2022 modifications; they do not change the motor-control implementation.

Licensing

The four adapted focuser and Stepper source files, including the project's
modifications, remain under LGPL-2.1-or-later. The combined driver containing
bcm2835 is distributed under GPL-3.0-only. The imported CMake helpers retain
their component licences. See LICENSE for full terms and copyright notices.
