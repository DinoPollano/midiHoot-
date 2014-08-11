
// Definition of interrupt names
#include < avr/io.h >
// ISR interrupt service routine
#include < avr/interrupt.h >
//Midi Library 
#include <MIDI.h>

#define  PATCH_PARAMETER_A 20
#define  PATCH_PARAMETER_B 21
#define  PATCH_PARAMETER_C 22
#define  PATCH_PARAMETER_D 23
#define  PATCH_PARAMETER_E 24
#define ledPin1  8
#define ledPin2  9 
#define LED_ON 0 
#define LED_OFF 3
#define chnl 1

bool midiAssignMode = false; 

byte storedValues[5] = {0,0,0,0,0}; 
byte pedalValues[5] = {127,127,127,127,127}; 
const int button1Pin = 2;// analog pin that button 1 is connect to, button 1 is toggle 

bool button1ChangeFlag = false;  
long lastDebounceTime1 = 0; 
bool button1Toggle = false; 
bool lastButton1Toggle = false; 

const int button2Pin = 3; // analog pin that button 2 is connect to, button 2 is momentary 
int buttonState2 = 0;         // variable for reading the pushbutton status
int lastButtonState2 = LOW;   // the previous reading from the input pin
bool button2ChangeFlag = false; 
long lastDebounceTime2 = 0; 



long debounceDelay1 = 100;
long debounceDelay2 = 50;



bool test = false;  
 
void setup()
{

  pinMode(button1Pin, INPUT);  // set the button1 as an input to read
  pinMode(button2Pin, INPUT);
  digitalWrite(button1Pin, HIGH);    // Enable pullup resistor
  digitalWrite(button2Pin, HIGH);    // Enable pullup resistor
  pinMode(ledPin1, OUTPUT);  
  pinMode(ledPin2,   OUTPUT);  
   digitalWrite(ledPin1, LOW);    
  digitalWrite(ledPin2, LOW);  
  
  sei();                    // Enable global interrupts
  EIMSK |= (1 << INT0);     // Enable external interrupt INT0
  EICRA |= (1 << ISC01);    // Trigger INT0 on falling edge
  EIMSK |= (1 << INT1);     // Enable external interrupt INT1
  EICRA |= (1 << ISC10);  // Trigger INT1 on any logical change 
 MIDI.setHandleControlChange(HandleControlChange);  
 MIDI.begin(1); // This sets to the MIDI baudrate (31250), sets the input to channel 1, enables thru


}
  /********INTERRUPT********/
ISR(INT0_vect)        
{  
    button1ChangeFlag = true; 
}

ISR(INT1_vect)
{
   button2ChangeFlag = true;    
}




/**********************************main*************************/
void loop()
{

   MIDI.read();

  if(midiAssignMode)
  {
    LED_FUNCTION(ledPin1, LED_ON);
    LED_FUNCTION(ledPin2, LED_ON);
  }
  
  if(midiAssignMode == true && button1ChangeFlag == true)
  {
    buttonState2 = HIGH; //off
   button1ChangeFlag = false;
    midiAssignMode = false;  
  }


  
  if(button1ChangeFlag != false)
  {      
      lastDebounceTime1 = millis(); 
      button1ChangeFlag = false;
  } 

   if(button2ChangeFlag != false)
  {         
     lastDebounceTime2 = millis();     
     button2ChangeFlag = false;
  }
  
  
                     /****DEBOUNCING***/
  if((lastDebounceTime1 != 0) && (millis()-lastDebounceTime1) > debounceDelay1)  
   {     
     if(lastButton1Toggle == true)
     {
       button1Toggle = false;
     }
     if(lastButton1Toggle == false)
     {
       button1Toggle = true;
     } 
      lastDebounceTime1 = 0;
      button1ChangeFlag = false;    
   }
   
   if((lastDebounceTime2 != 0) && (millis()-lastDebounceTime2) > debounceDelay2)
   {
      midiAssignMode = false;
     buttonState2 = digitalRead(button2Pin); 
     lastDebounceTime2 = 0;
     button2ChangeFlag = false; 
     
        if( button1Toggle == true && buttonState2 == true)
            {              
              midiAssignMode = true; 
              button1Toggle = false;
              buttonState2 = false;
            }
   }

                               /**OUTPUT**/
 if(midiAssignMode != true)
 {
    // if the button state has changed:
    if ( button1Toggle != lastButton1Toggle )   
    {
      if(  button1Toggle == true )
      {
         SENDCC(storedValues);
         LED_FUNCTION(ledPin1, LED_ON); //on
      }
     else if(button1Toggle == false)
      {
        SENDCC(pedalValues);      
        LED_FUNCTION(ledPin1, LED_OFF); //off
      }
       lastButton1Toggle = button1Toggle; 
    }
       if (buttonState2 != lastButtonState2  ) 
    {   
       if(buttonState2 == LOW) 
      {
       SENDCC(storedValues);
        LED_FUNCTION(ledPin2,LED_ON); // On          
      }
      else if(buttonState2 == HIGH ) 
      { 
         SENDCC(pedalValues);      
         LED_FUNCTION(ledPin2,LED_OFF); // Off 
      }
      lastButtonState2 = buttonState2;
    }   
 }    
    
}

/************main********/ 



void LED_FUNCTION(int LED, int mode)
{
  
  switch(mode)
  {
    case LED_ON:
    {
      
      if( !digitalRead(LED) )
      {
        digitalWrite(LED, HIGH);
      }
      else if( digitalRead(LED))
      {
        digitalWrite(LED, LOW); 
      }
      break; 
    }
    
    case LED_OFF:
    {
      
      if( !digitalRead(LED) )
      {
        digitalWrite(LED, LOW);
      }
      else if( digitalRead(LED))
      {
        digitalWrite(LED, LOW); 
      }
      break; 
    }
    
    default:
    {
      break; 
    }
  }
  
}

/*****************************MIDI***********************/

/***********CC**********/
void HandleControlChange(byte channel, byte number, byte value)
{
  if(midiAssignMode)
 {
   LED_FUNCTION(ledPin1,LED_OFF);
   
   switch(number)
   {
     case PATCH_PARAMETER_A:
     {
        storedValues[0] = value;
       break; 
     }
     case PATCH_PARAMETER_B:
     {
        storedValues[1] = value;
       break; 
     }
     case PATCH_PARAMETER_C:
     {
        storedValues[2] = value;
       break; 
     }
     case PATCH_PARAMETER_D:
     {
        storedValues[3] = value;
       break; 
     }
     case PATCH_PARAMETER_E:
     {
        storedValues[4] = value;
       break; 
     }  
     default:
     {
       break; 
     }
   }
    delay(10); 
    LED_FUNCTION(ledPin1,LED_ON);
  
 }
 else
 {
   
    switch(number)
   {
     case PATCH_PARAMETER_A:
     {
        pedalValues[0] = value;
       break; 
     }
     case PATCH_PARAMETER_B:
     {
        pedalValues[1] = value;
       break; 
     }
     case PATCH_PARAMETER_C:
     {
        pedalValues[2] = value;
       break; 
     }
     case PATCH_PARAMETER_D:
     {
        pedalValues[3] = value;
       break; 
     }
     case PATCH_PARAMETER_E:
     {
        pedalValues[4] = value;
       break; 
     }  
     default:
     {
       break; 
     }
   }
 }
 
 
  
}

void SENDCC(byte Values[])
{
        MIDI.sendControlChange(PATCH_PARAMETER_A,Values[0],chnl);
        MIDI.sendControlChange(PATCH_PARAMETER_B,Values[1],chnl);
        MIDI.sendControlChange(PATCH_PARAMETER_C,Values[2],chnl);
        MIDI.sendControlChange(PATCH_PARAMETER_D,Values[3],chnl);
        MIDI.sendControlChange(PATCH_PARAMETER_E,Values[4],chnl);
}


/*****CC****/

/*****************Midi************/ 


