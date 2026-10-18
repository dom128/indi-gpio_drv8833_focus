/*
  INDI Driver for gpioDRV8833

  Copyright (C) 2016 Piotr Dlugosz

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <memory>
#include <indicom.h>
//#include <pigpiod_if2.h>

#include "GPIO_DRV8833_Focuser.h"

#include "config.h"

#include "Stepper.h"

//#define PARK_PARK 0
//#define PARK_UNPARK 1

// We declare an auto pointer to gpioDRV8833.
static std::unique_ptr<GPIO_DRV8833_Focuser> gpioDRV8833Focuser(new GPIO_DRV8833_Focuser());

void ISPoll(void *p);

void ISGetProperties(const char *dev)
{
    gpioDRV8833Focuser->ISGetProperties(dev);
}

void ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int num)
{
    gpioDRV8833Focuser->ISNewSwitch(dev, name, states, names, num);
}

void ISNewText(	const char *dev, const char *name, char *texts[], char *names[], int num)
{
    gpioDRV8833Focuser->ISNewText(dev, name, texts, names, num);
}

void ISNewNumber(const char *dev, const char *name, double values[], char *names[], int num)
{
    gpioDRV8833Focuser->ISNewNumber(dev, name, values, names, num);
}

void ISNewBLOB (const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[], char *names[], int n)
{
    INDI_UNUSED(dev);
    INDI_UNUSED(name);
    INDI_UNUSED(sizes);
    INDI_UNUSED(blobsizes);
    INDI_UNUSED(blobs);
    INDI_UNUSED(formats);
    INDI_UNUSED(names);
    INDI_UNUSED(n);
}

void ISSnoopDevice (XMLEle *root)
{
    gpioDRV8833Focuser->ISSnoopDevice(root);
}

/****************************************************************
**
**
*****************************************************************/

GPIO_DRV8833_Focuser::GPIO_DRV8833_Focuser()
{
    //FI::SetCapability(FOCUSER_CAN_ABS_MOVE | FOCUSER_CAN_REL_MOVE | FOCUSER_CAN_ABORT | FOCUSER_CAN_SYNC);
    FI::SetCapability(FOCUSER_CAN_ABORT |FOCUSER_CAN_REVERSE|FOCUSER_HAS_VARIABLE_SPEED);
    //isMoving = false;
    setSupportedConnections(CONNECTION_NONE);
    setVersion(GPIO_DRV8833_VERSION_MAJOR, GPIO_DRV8833_VERSION_MINOR);

}

bool GPIO_DRV8833_Focuser::initProperties()
{

    INDI::Focuser::initProperties();
    
    gpioN[0].value = 16;
    gpioN[0].min=0;
    gpioN[0].max=40;
    gpioN[0].step=1;

    gpioN[1].value = 20;
    gpioN[1].min=0;
    gpioN[1].max=40;
    gpioN[1].step=1;

    gpioN[2].value = 23;
    gpioN[2].min=0;
    gpioN[2].max=40;
    gpioN[2].step=1;

    gpioN[3].value = 24;
    gpioN[3].min=0;
    gpioN[3].max=40;
    gpioN[3].step=1;

    IUFillNumber(&gpioN[0],"GPIOA01PIN","INA1 Pin Number","%2.0f",0,40,1,16);
    IUFillNumber(&gpioN[1],"GPIOA02PIN","INA2 Pin Number","%2.0f",0,40,1,20);

    IUFillNumber(&gpioN[2],"GPIOB01PIN","INB1 Pin Number","%2.0f",0,40,1,23);
    IUFillNumber(&gpioN[3],"GPIOB02PIN","INB2 Pin Number","%2.0f",0,40,1,24);
	
	
    IUFillNumberVector(&gpioNP, gpioN,4,getDeviceName(),"GPIO_SETTINGS","GPIO Pins Settings", COMMUNICATION_TAB, IP_RW,20, IPS_IDLE);

    FocusSpeedN[0].min = 1;
    FocusSpeedN[0].max = 40;
    FocusSpeedN[0].value = 30;

    IUUpdateMinMax(&FocusSpeedNP);
    
	
	
	stepsPerRevolution = 200;  // change this to fit the number of steps per revolution

	// for your motor	
	// initialize the Stepper library on pins ??? through ???:
	
	myStepper = new Stepper(stepsPerRevolution, gpioN[0].value, gpioN[1].value, gpioN[2].value, gpioN[3].value);

    return true;
}

bool GPIO_DRV8833_Focuser::updateProperties()
{
    INDI::Focuser::updateProperties();

    if (isConnected())
    {
        defineNumber(&gpioNP);
	delete myStepper;
	myStepper = new Stepper(stepsPerRevolution, gpioN[0].value, gpioN[1].value, gpioN[2].value, gpioN[3].value);
    }
    else
    {
        deleteProperty(gpioNP.name);
    }
    return true;
}



bool GPIO_DRV8833_Focuser::saveConfigItems(FILE *fp)
{
    INDI::Focuser::saveConfigItems(fp);

    IUSaveConfigNumber(fp,&gpioNP);
    return true;
}

bool GPIO_DRV8833_Focuser::ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n)
{
    if(strcmp(dev, getDeviceName()) == 0)
    {
        if (strcmp(gpioNP.name, name)==0)
        {
            IUUpdateNumber(&gpioNP, values, names, n);
            IDSetNumber(&gpioNP,nullptr);
            gpioNP.s = IPS_OK;
            LOG_INFO("GPIO pins settings updated.");
        }
    }

    return INDI::Focuser::ISNewNumber(dev, name, values, names, n);
}
bool GPIO_DRV8833_Focuser::SetFocuserSpeed(int speed)
{
    //LOGF_DEBUG("Setting Focuser Speed on Pin# %d with speed=%d", (int)gpioN[0].value, speed);
    LOGF_DEBUG("Setting Focuser Speed with speed=%d", speed);

    //set_mode(pi_gpio,gpioN[0].value, PI_OUTPUT);
    //set_PWM_range(pi_gpio,gpioN[0].value, FocusSpeedN[0].max);
    //set_PWM_dutycycle(pi_gpio,gpioN[0].value, speed);
    return true;
}

bool GPIO_DRV8833_Focuser::ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n)
{
    return INDI::Focuser::ISNewSwitch(dev, name, states, names, n);
}

bool GPIO_DRV8833_Focuser::AbortFocuser()
{
    LOG_INFO("Stopping...");
    //LOGF_DEBUG("Writing LOW to Enable Pin #", gpioN[3].value);
    //set_mode(pi_gpio,gpioN[3].value,PI_OUTPUT);
    //gpio_write(pi_gpio,gpioN[3].value,PI_LOW);

	myStepper->release();

    //isMoving = false;
    return true;
}





IPState GPIO_DRV8833_Focuser::MoveFocuser(FocusDirection dir, int speed, uint16_t duration)
{
	/*
    if (isMoving)
    {
        LOG_DEBUG("Focuser Already Moving... returning BUSY signal");
        return IPS_BUSY;
    }

	*/
    LOG_INFO("Moving Focuser.... ");
    //unsigned int a01, a02, b01, b02;//
	
	myStepper->setSpeed(speed);//////////////////////////////////

	int steps = int(( speed * duration * stepsPerRevolution)/( 60 * 1000 ));

    if (!(dir==FOCUS_INWARD) != !isReversed)
    {
        //a01 = isReversed?PI_HIGH:PI_LOW;
        //a02 = isReversed?PI_LOW:PI_HIGH;
	
	// step 1/100 of a revolution:
	myStepper->step(steps);///////////////////////////////////////

	
    }
    else
    {
        //a01 = isReversed?PI_LOW:PI_HIGH;
        //a02 = isReversed?PI_HIGH:PI_LOW;
	myStepper->step(-steps);/////////////////////////////

    }

	

	/*
    LOGF_DEBUG("Write on Speed pin # %f with speed %d",gpioN[0].value, speed);
    set_mode(pi_gpio,gpioN[0].value,PI_OUTPUT);
    set_PWM_dutycycle(pi_gpio,gpioN[0].value,speed);
	*/

	/*
    LOGF_DEBUG("Write on A01 pin # %f with %d",gpioN[0].value, a01);
    set_mode(pi_gpio,gpioN[0].value,PI_OUTPUT);
    gpio_write(pi_gpio,gpioN[0].value,a01);

    LOGF_DEBUG("Write on A02 pin # %f with %d",gpioN[1].value, a02);
    set_mode(pi_gpio,gpioN[1].value,PI_OUTPUT);
    gpio_write(pi_gpio,gpioN[1].value,a02);

    LOGF_DEBUG("Write on B01 pin # %f with %d",gpioN[2].value, b01);
    set_mode(pi_gpio,gpioN[2].value,PI_OUTPUT);
    gpio_write(pi_gpio,gpioN[2].value,b01);

    LOGF_DEBUG("Write on B02 pin # %f with %d",gpioN[3].value, b02);
    set_mode(pi_gpio,gpioN[3].value,PI_OUTPUT);
    gpio_write(pi_gpio,gpioN[3].value,b02);
	*/


    //isMoving = true;
	
    //usleep(duration*1000);
    AbortFocuser();
    LOG_INFO("Done... ");
    return IPS_OK;
	
}




const char * GPIO_DRV8833_Focuser::getDefaultName()
{
    return (char *)"DRV8833 Focuser via GPIO pins";
}


bool GPIO_DRV8833_Focuser::Connect()
{
	/*
    pi_gpio = pigpio_start(NULL,NULL);
    if (pi_gpio>=0)
    {
        LOG_INFO("Connected successfully!");
    }
    else
    {
        LOG_INFO("Unable to Connect to the GPIO when callin pigpio_start(). Check if pigpiod daemon is running.");
        return false;
    }
	*/

	/*
    // if the 3.3v pin settings is not 0 then switch on the circuit
    if (gpioN[4].value >0)
    {
        set_mode(pi_gpio,gpioN[4].value,PI_OUTPUT);
        gpio_write(pi_gpio,gpioN[4].value,PI_HIGH);
    }
	*/

    return true;
}


bool GPIO_DRV8833_Focuser::Disconnect()
{
    AbortFocuser();

    // if the 3.3v pin settings is not 0 then switch on the circuit

	/*

    if (gpioN[4].value >0)
    {
        set_mode(pi_gpio,gpioN[4].value,PI_OUTPUT);
        gpio_write(pi_gpio,gpioN[4].value,PI_LOW);
    }

    pigpio_stop(pi_gpio);
	*/
    LOG_INFO("Device Disconnected. Bye!");
    return true;
}


bool GPIO_DRV8833_Focuser::ReverseFocuser(bool enabled)
{
    isReversed = enabled;
    return true;

}
