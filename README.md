# ArduinoPiPowerController

This is originally designed for use with a Digispark for ease of programming, size and the availability of what I had on hand.

Visit the below link to see functioning circuit online.
https://www.tinkercad.com/things/hANggwopnFI-rpi-power/

## Pinout
int LED_PIN = 1; //Default Digispark on-board LED
int RELAY_PIN = 2;
int BUTTON_PIN = 3;
int PI_POWER = 0; // Default is Pin27? "dtoverlay=gpio-poweroff,gpiopin=2,active_low="y"
int PI_SHDWN = 5; //Default is GPIO3 on Rpi with "dtoverlay=gpio-shutdown"

## Settings
**AUTOPWR 0/1** 
This determines the power state of the relay/mosfet when power is first applied. Only works with momentary switch.
0 for auto off
1 for auto on

**MOM_TOGGLE 0/1**
This determines the type of switch used.
0 for momentary push button
1 for toggle switch

**RELAY_MODE 0/1**
This controls the trigger for the relay.
 0 for active LOW | 1 for active HIGH
 
 ## Power modes
* 0 - Power off (turns relay off)
* 1 - Power on (turns relay on)
* 2 - Powering off (set PI_SHDWN high for 200ms then sets mode 3)
* 3 - Waiting for Pi to shutdown (waits for PI_PWR to go low then sets mode 0, times out after 2min and sets mode 0)
* 4 - Hard Shutdown (If in mode 1 and button is held down for ~10 seconds)
