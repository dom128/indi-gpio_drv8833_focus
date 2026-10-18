/*
 * Stepper.cpp - Stepper library for Raspberry Pi - Version 1.0
 *
 * Modified for raspberry pi (1.0) by Abe Wieland
 * 
 * See Stepper.h for additional instructions.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
*/

#include "Stepper.h"
#include "bcm2835.h"
#include <iostream>

/*
 * two-wire constructor.
 * Sets which wires should control the motor.
 */
Stepper::Stepper(int number_of_steps, int motor_pin_1, int motor_pin_2)
{
	// Initialize bcm2835. If not root, fails.
	if (!bcm2835_init())
	{
		std::cerr << "Failed to initialize: Stepper.cpp must be run as root." << std::endl;
	}

  this->step_number = 0;    // which step the motor is on
  this->direction = 0;      // motor direction
  this->number_of_steps = number_of_steps; // total number of steps for this motor

  // Arduino pins for the motor control connection:
  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;

  // setup the pins on the microcontroller:
	bcm2835_gpio_fsel(this->motor_pin_1, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(this->motor_pin_2, BCM2835_GPIO_FSEL_OUTP);

  // When there are only 2 pins, set the others to 0:
  this->motor_pin_3 = 0;
  this->motor_pin_4 = 0;
  this->motor_pin_5 = 0;

  // pin_count is used by the stepMotor() method:
  this->pin_count = 2;
}


/*
 *   constructor for four-pin version
 *   Sets which wires should control the motor.
 */
Stepper::Stepper(int number_of_steps, int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4)
{
	// Initialize bcm2835. If not root, fails.
	if (!bcm2835_init())
	{
		std::cerr << "Failed to initialize: Stepper.cpp must be run as root." << std::endl;
	}
  
  this->step_number = 0;    // which step the motor is on
  this->direction = 0;      // motor direction
  this->number_of_steps = number_of_steps; // total number of steps for this motor

  // Arduino pins for the motor control connection:
  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;
  this->motor_pin_3 = motor_pin_3;
  this->motor_pin_4 = motor_pin_4;

  // setup the pins on the microcontroller:
	bcm2835_gpio_fsel(this->motor_pin_1, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(this->motor_pin_2, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(this->motor_pin_3, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(this->motor_pin_4, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(this->motor_pin_5, BCM2835_GPIO_FSEL_OUTP);

  // When there are 4 pins, set the others to 0:
  this->motor_pin_5 = 0;

  // pin_count is used by the stepMotor() method:
  this->pin_count = 4;
}

/*
 *   constructor for five phase motor with five wires
 *   Sets which wires should control the motor.
 */
Stepper::Stepper(int number_of_steps, int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4, int motor_pin_5)
{
	// Initialize bcm2835. If not root, fails.
	if (!bcm2835_init())
	{
		std::cerr << "Failed to initialize: Stepper.cpp must be run as root." << std::endl;
	}
  
  this->step_number = 0;    // which step the motor is on
  this->direction = 0;      // motor direction
  this->number_of_steps = number_of_steps; // total number of steps for this motor

  // Arduino pins for the motor control connection:
  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;
  this->motor_pin_3 = motor_pin_3;
  this->motor_pin_4 = motor_pin_4;
  this->motor_pin_5 = motor_pin_5;

  // setup the pins on the microcontroller:
	bcm2835_gpio_fsel(this->motor_pin_1, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(this->motor_pin_2, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(this->motor_pin_3, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(this->motor_pin_4, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(this->motor_pin_5, BCM2835_GPIO_FSEL_OUTP);

  // pin_count is used by the stepMotor() method:
  this->pin_count = 5;
}

/*
 * Sets the speed in revs per minute
 */
void Stepper::setSpeed(long whatSpeed)
{
  this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / whatSpeed;
}

/*
 * Moves the motor steps_to_move steps.  If the number is negative,
 * the motor moves in the reverse direction.
 */

void Stepper::step(int steps_to_move)
{
  int steps_left; // how many steps to take
  if (steps_to_move > 0) // determine direction based on whether steps_to_mode is + or -:
  {
    this->direction = 1;
    steps_left = steps_to_move;
  }
  if (steps_to_move < 0)
  {
    this->direction = 0;
    steps_left = -steps_to_move;
  }

  // decrement the number of steps, moving one step each time:
  while (steps_left > 0) ////
  {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    // move only if the appropriate delay has passed:
    if (std::chrono::duration_cast<std::chrono::microseconds>(now - this->last_step_time).count() >= this->step_delay)
    {
      // get the timeStamp of when you stepped:
      this->last_step_time = now;
      // increment or decrement the step number,
      // depending on direction:
      if (this->direction == 1)
      {
        this->step_number++;
        if (this->step_number == this->number_of_steps) {
          this->step_number = 0;
        }
      }
      else
      {
        if (this->step_number == 0) {
          this->step_number = this->number_of_steps;
        }
        this->step_number--;
      }
      // decrement the steps left:
      steps_left--;
      // step the motor to step number 0, 1, ..., {3 or 10}
      if (this->pin_count == 5)
        stepMotor(this->step_number % 10);
      else
        stepMotor(this->step_number % 4);
    }
  }
}

/*
 * Moves the motor forward or backwards.
 */
void Stepper::stepMotor(int thisStep)
{
  if (this->pin_count == 2) {
    switch (thisStep) {
      case 0:  // 01
        bcm2835_gpio_write(motor_pin_1, LOW);
        bcm2835_gpio_write(motor_pin_2, HIGH);
      break;
      case 1:  // 11
        bcm2835_gpio_write(motor_pin_1, HIGH);
        bcm2835_gpio_write(motor_pin_2, HIGH);
      break;
      case 2:  // 10
        bcm2835_gpio_write(motor_pin_1, HIGH);
        bcm2835_gpio_write(motor_pin_2, LOW);
      break;
      case 3:  // 00
        bcm2835_gpio_write(motor_pin_1, LOW);
        bcm2835_gpio_write(motor_pin_2, LOW);
      break;
    }
  }
  if (this->pin_count == 4) {
    switch (thisStep) {
      case 0:  // 1010
        bcm2835_gpio_write(motor_pin_1, HIGH);
        bcm2835_gpio_write(motor_pin_2, LOW);
        bcm2835_gpio_write(motor_pin_3, HIGH);
        bcm2835_gpio_write(motor_pin_4, LOW);
      break;
      case 1:  // 0110
        bcm2835_gpio_write(motor_pin_1, LOW);
        bcm2835_gpio_write(motor_pin_2, HIGH);
        bcm2835_gpio_write(motor_pin_3, HIGH);
        bcm2835_gpio_write(motor_pin_4, LOW);
      break;
      case 2:  //0101
        bcm2835_gpio_write(motor_pin_1, LOW);
        bcm2835_gpio_write(motor_pin_2, HIGH);
        bcm2835_gpio_write(motor_pin_3, LOW);
        bcm2835_gpio_write(motor_pin_4, HIGH);
      break;
      case 3:  //1001
        bcm2835_gpio_write(motor_pin_1, HIGH);
        bcm2835_gpio_write(motor_pin_2, LOW);
        bcm2835_gpio_write(motor_pin_3, LOW);
        bcm2835_gpio_write(motor_pin_4, HIGH);
      break;
    }
  }

  if (this->pin_count == 5) {
    switch (thisStep) {
      case 0:  // 01101
        bcm2835_gpio_write(motor_pin_1, LOW);
        bcm2835_gpio_write(motor_pin_2, HIGH);
        bcm2835_gpio_write(motor_pin_3, HIGH);
        bcm2835_gpio_write(motor_pin_4, LOW);
        bcm2835_gpio_write(motor_pin_5, HIGH);
        break;
      case 1:  // 01001
        bcm2835_gpio_write(motor_pin_1, LOW);
        bcm2835_gpio_write(motor_pin_2, HIGH);
        bcm2835_gpio_write(motor_pin_3, LOW);
        bcm2835_gpio_write(motor_pin_4, LOW);
        bcm2835_gpio_write(motor_pin_5, HIGH);
        break;
      case 2:  // 01011
        bcm2835_gpio_write(motor_pin_1, LOW);
        bcm2835_gpio_write(motor_pin_2, HIGH);
        bcm2835_gpio_write(motor_pin_3, LOW);
        bcm2835_gpio_write(motor_pin_4, HIGH);
        bcm2835_gpio_write(motor_pin_5, HIGH);
        break;
      case 3:  // 01010
        bcm2835_gpio_write(motor_pin_1, LOW);
        bcm2835_gpio_write(motor_pin_2, HIGH);
        bcm2835_gpio_write(motor_pin_3, LOW);
        bcm2835_gpio_write(motor_pin_4, HIGH);
        bcm2835_gpio_write(motor_pin_5, LOW);
        break;
      case 4:  // 11010
        bcm2835_gpio_write(motor_pin_1, HIGH);
        bcm2835_gpio_write(motor_pin_2, HIGH);
        bcm2835_gpio_write(motor_pin_3, LOW);
        bcm2835_gpio_write(motor_pin_4, HIGH);
        bcm2835_gpio_write(motor_pin_5, LOW);
        break;
      case 5:  // 10010
        bcm2835_gpio_write(motor_pin_1, HIGH);
        bcm2835_gpio_write(motor_pin_2, LOW);
        bcm2835_gpio_write(motor_pin_3, LOW);
        bcm2835_gpio_write(motor_pin_4, HIGH);
        bcm2835_gpio_write(motor_pin_5, LOW);
        break;
      case 6:  // 10110
        bcm2835_gpio_write(motor_pin_1, HIGH);
        bcm2835_gpio_write(motor_pin_2, LOW);
        bcm2835_gpio_write(motor_pin_3, HIGH);
        bcm2835_gpio_write(motor_pin_4, HIGH);
        bcm2835_gpio_write(motor_pin_5, LOW);
        break;
      case 7:  // 10100
        bcm2835_gpio_write(motor_pin_1, HIGH);
        bcm2835_gpio_write(motor_pin_2, LOW);
        bcm2835_gpio_write(motor_pin_3, HIGH);
        bcm2835_gpio_write(motor_pin_4, LOW);
        bcm2835_gpio_write(motor_pin_5, LOW);
        break;
      case 8:  // 10101
        bcm2835_gpio_write(motor_pin_1, HIGH);
        bcm2835_gpio_write(motor_pin_2, LOW);
        bcm2835_gpio_write(motor_pin_3, HIGH);
        bcm2835_gpio_write(motor_pin_4, LOW);
        bcm2835_gpio_write(motor_pin_5, HIGH);
        break;
      case 9:  // 00101
        bcm2835_gpio_write(motor_pin_1, LOW);
        bcm2835_gpio_write(motor_pin_2, LOW);
        bcm2835_gpio_write(motor_pin_3, HIGH);
        bcm2835_gpio_write(motor_pin_4, LOW);
        bcm2835_gpio_write(motor_pin_5, HIGH);
        break;
    }
  }
}

/*
  version() returns the version of the library:
*/
int Stepper::version(void)
{
  return 1;
}

//release motor
void Stepper::release(void)
{
	if (this->pin_count == 4) {
        	bcm2835_gpio_write(motor_pin_1, LOW);
	        bcm2835_gpio_write(motor_pin_2, LOW);
	}
	if (this->pin_count == 4) {
        	bcm2835_gpio_write(motor_pin_1, LOW);
	        bcm2835_gpio_write(motor_pin_2, LOW);
        	bcm2835_gpio_write(motor_pin_3, LOW);
	        bcm2835_gpio_write(motor_pin_4, LOW);
	}
	if (this->pin_count == 5) {
        	bcm2835_gpio_write(motor_pin_1, LOW);
	        bcm2835_gpio_write(motor_pin_2, LOW);
        	bcm2835_gpio_write(motor_pin_3, LOW);
	        bcm2835_gpio_write(motor_pin_4, LOW);
        	bcm2835_gpio_write(motor_pin_5, LOW);
	}
}
