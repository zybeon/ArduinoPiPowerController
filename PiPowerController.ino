int PI_POWER = 4; // Default is Pin27? "dtoverlay=gpio-poweroff,gpiopin=2,active_low="y"
int PI_SHDWN = 3; //Default is GPIO3 on Rpi with "dtoverlay=gpio-shutdown"
int RELAY_PIN = 0;
int BUTTON_PIN = 2;
int LED_PIN = 1;
int AUTOPWR = 0; // 0 for auto off | 1 for auto on when plugged in
int MOM_TOGGLE = 0; // 0 for momentary | 1 for toggle
int RELAY_MODE = 0; // 0 for LOW on | 1 for HIGH on

int checkOffCount = 0;
int waitingOffTimeoutCount=0;
int powerstatus = 0; 
//Power Toggle Mode
// 0 Power off
// 1 Powering On (not used for now)
// 2 Powered on 
// 3 Powering off
// 4 Waiting for Safe Shutdown to finish
// 5 Hard Shutdown

int buttondowncount = 0;
int buttonupcount = 0;

void setup() {
  // put your setup code here, to run once:
    pinMode(LED_PIN, OUTPUT); //LED 
    pinMode(RELAY_PIN, OUTPUT); //Power Relay
    pinMode(BUTTON_PIN, INPUT);  //Button
    pinMode(PI_SHDWN, OUTPUT); //Shutdown trigger
    //pinMode(PI_POWER, INPUT); //Sense Pi Power
    
    if(AUTOPWR)//setting auto power status on power up.
      powerstatus = 1;
    else
      powerstatus = 0;
}

void loop() {
      //Read the state of the switch into a local variable:
      bool reading = digitalRead(BUTTON_PIN);

      if(buttondowncount>1000)
        buttondowncount = 0;
        
      if(buttonupcount>1000)
        buttonupcount = 0;

      if (!reading) {
        // Reset the debouncing timer
        buttonupcount = 0;
        buttondowncount = buttondowncount + 1;       
      } 

      if (reading) {
        buttonupcount = buttonupcount + 1;

        
        if(buttondowncount>=60 && buttonupcount >2)
        {//Long Pressed Button
          buttondowncount = 0;
          buttonupcount = 0;
          powerstatus = 5;
        }
        else if(buttondowncount>=3 && buttonupcount >2)
        {//Button Pressed
          buttondowncount = 0;
          buttonupcount = 0;
          if(powerstatus==0)
            powerstatus=2;
          else if(powerstatus==2)
            powerstatus=3;            
        }
      }
      
      if(powerstatus==1)
      {
          digitalWrite(LED_PIN, LOW);
          delay(300);
          digitalWrite(LED_PIN, HIGH);
          delay(200);
          digitalWrite(LED_PIN, LOW);
          delay(300);
          digitalWrite(LED_PIN, HIGH);
          delay(200);
      }
      else if(powerstatus==2)
      {
          digitalWrite(SHUTDOWN_PIN, LOW);
          digitalWrite(LED_PIN, HIGH);
          if(RELAY_MODE)
            digitalWrite(RELAY_PIN, HIGH); 
          else
            digitalWrite(RELAY_PIN, LOW);
      }
      else if(powerstatus==3)
      {
          //Start Shutdown Sequence
          // Call 
          digitalWrite(SHUTDOWN_PIN, HIGH);
          delay(200);
          digitalWrite(SHUTDOWN_PIN, LOW);
          powerstatus=4;
          checkOffCount = 0;
          waitingOffTimeoutCount = 0;
      }
      else if(powerstatus==4)
      {
          digitalWrite(LED_PIN, LOW);
          delay(300);
          digitalWrite(LED_PIN, HIGH);
          delay(200);

          //Check if it's off 5 times before shutting down                
          if(checkPiOff())
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
              digitalWrite(LED_PIN, LOW);
              if(RELAY_MODE)
                digitalWrite(RELAY_PIN, LOW); 
          	  else
            	digitalWrite(RELAY_PIN, HIGH); 
              powerstatus = 0;
              checkOffCount = 0;
              waitingOffTimeoutCount = 0; 
          }
          else if(waitingOffTimeoutCount>=40)
          {
            //Time out shutdown anyway
            digitalWrite(LED_PIN, LOW);
            if(RELAY_MODE)
              digitalWrite(RELAY_PIN, LOW); 
            else
              digitalWrite(RELAY_PIN, HIGH); 
            powerstatus = 0;
            checkOffCount = 0;
            waitingOffTimeoutCount = 0;                        
          }       
      }
      else if(powerstatus==5) //Hard shutdown
      {
          digitalWrite(LED_PIN, LOW);
          if(RELAY_MODE)
            digitalWrite(RELAY_PIN, LOW); 
          else
            digitalWrite(RELAY_PIN, HIGH);
          powerstatus = 0;
          checkOffCount = 0;
      }
      delay(50);
      
}
