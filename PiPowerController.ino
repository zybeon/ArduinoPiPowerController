int PI_POWER = 3; // Default is Pin27? "dtoverlay=gpio-poweroff,gpiopin=2,active_low="y"
int PI_SHDWN = 4; //Default is GPIO3 on Rpi with "dtoverlay=gpio-shutdown"
int RELAY_PIN = 2;
int BUTTON_PIN = 0;
int LED_PIN = 1;

bool AUTOPWR = 0; // 0 for auto off | 1 for auto on when plugged in
bool MOM_TOGGLE = 1; // 1 for momentary | 0 for toggle
bool RELAY_MODE = 0; // 0 for LOW on | 1 for HIGH on

int checkOffCount = 0;
int waitingOffTimeoutCount=0;
int powerstatus; 
//Power Modes
// 0 Power off
// 1 Power on 
// 2 Powering off
// 3 Waiting for Pi to shutdown
// 4 Hard Shutdown

int buttondowncount = 0;
int buttonupcount = 0;

void setup(){
	// put your setup code here, to run once:
	pinMode(LED_PIN, OUTPUT); //LED 
	pinMode(RELAY_PIN, OUTPUT); //Power Relay
	pinMode(BUTTON_PIN, INPUT);  //Button
	pinMode(PI_SHDWN, OUTPUT); //Shutdown trigger
	pinMode(PI_POWER, INPUT); //Sense Pi Power
	
  if(AUTOPWR)//Setting auto power status on power up.
		powerstatus = 1;
	else
		powerstatus = 0;
}

void loop() {

  //Read the state of the button/swtich into a local variable:
	bool reading = !digitalRead(BUTTON_PIN);
	
	if(MOM_TOGGLE) //Mommentary button check
	{
		
		if(buttondowncount>1000)
			buttondowncount = 0;
		if(buttonupcount>1000)
			buttonupcount = 0;
		
		if (reading) {
			// Reset the debouncing timer
			buttonupcount = 0;
			buttondowncount = buttondowncount + 1;
		}
		if (!reading)
		{
			buttonupcount = buttonupcount + 1;
			if(buttondowncount>=60 && buttonupcount >2)
			{//Long Pressed Button
				buttondowncount = 0;
				buttonupcount = 0;
				powerstatus = 4;
			}
			else if(buttondowncount>=3 && buttonupcount >2)
			{//Button Pressed
				buttondowncount = 0;
				buttonupcount = 0;
				if(powerstatus==0)
					powerstatus=1;
				else if(powerstatus==1)
					powerstatus=2;
			}
		}
	}
  else
  {
    if(powerstatus==0){
      if(reading)
        checkOffCount = checkOffCount + 1;
      else
        checkOffCount=0;
      if(checkOffCount>=5) {
        powerstatus = 1;
        checkOffCount = 0;
      }
    }
    else if(powerstatus==1){
      if(reading)
        checkOffCount = checkOffCount + 1;
      else
      checkOffCount=0;
      if(checkOffCount>=5){
        powerstatus = 2;
        checkOffCount = 0;
      }
    }
  }
	if(powerstatus==0) //Power off
	{
		digitalWrite(LED_PIN, LOW);
		if(RELAY_MODE)
			digitalWrite(RELAY_PIN, LOW);
		else
			digitalWrite(RELAY_PIN, HIGH);
	}
	else if(powerstatus==1) //Power on
	{
		digitalWrite(PI_SHDWN, LOW);
		digitalWrite(LED_PIN, HIGH);
		if(RELAY_MODE)
			digitalWrite(RELAY_PIN, HIGH);
		else
			digitalWrite(RELAY_PIN, LOW); 
	}
  else if(powerstatus==2) //Powering off
  {
    digitalWrite(PI_SHDWN, HIGH);
    delay(200);
    digitalWrite(PI_SHDWN, LOW);
    powerstatus=3;
    checkOffCount = 0;
    waitingOffTimeoutCount = 0;
  }
  else if(powerstatus==3) //Waiting for Pi to shutdown
  {
    digitalWrite(LED_PIN, LOW);
    delay(300);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    
    //Check if it's off 5 times before shutting down                
    //if(checkPiOff())
    if(!digitalRead(PI_POWER))
      checkOffCount = checkOffCount + 1;
    else
    {
      checkOffCount=0;
      waitingOffTimeoutCount = waitingOffTimeoutCount + 1;
    }
    if(checkOffCount>=5)
    {
      //It's really off but wait 2 seconds
      delay(2000);
      powerstatus = 0;
      checkOffCount = 0;
      waitingOffTimeoutCount = 0; 
    }
    else if(waitingOffTimeoutCount>=40)
    {
      //Time out shutdown anyway
      powerstatus = 0;
      checkOffCount = 0;
      waitingOffTimeoutCount = 0;                        
    }
  }
  else if(powerstatus==4) //Hard shutdown
  {
    powerstatus = 0;
		checkOffCount = 0;
  }
	delay(50);
}
