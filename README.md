# indi-gpio_drv8833_focus
A simple GPIO driver using a DRV8833 Stepper / DC breakout to drive a custom telescope focuser.
The project is based on https://github.com/sywong2000/indi-gpio_pwm_focus

## Build and install

To build the INDI driver from source, ensure the necessary build dependencies are installed. For a Raspberry Pi running Debian or Ubuntu Linux one may use:

```bash
sudo apt update
sudo apt install indi-full libindi-dev # INDI development requirements
sudo apt install libpigpiod-if-dev # GPIO development library for Raspberry Pi 
sudo apt install build-essential cmake # build tools
```

Then clone the driver repository (for example in your `~/src/` folder):

```bash
git clone https://github.com/dom128/indi-gpio_drv8833_focus.git
```

Next, the INDI driver can be built from source:

```bash
cd indi-gpio_drv8833_focus/
cmake .
make
```

Install the driver either by using `sudo make install`, or by moving the freshly built executable and the driver's .xml file to their respective installation targets:

```bash
sudo mv indi_gpio_drv8833_focus /usr/local/bin/
sudo mv indi_gpio_drv8833_focus.xml /usr/share/indi/
```

The driver may be tested by running:

```bash
indiserver -v indi_gpio_drv8833_focus
```

### Tested on ...
- Raspberry Pi 4 Model B Rev 1.5 (armhf) running Raspbian GNU/Linux 10 (buster)
- cmake version 3.16.3
- libindi-dev version 1.9.7+deb10u1
- libpigpiod-if-dev version 1.79-1+rpt1