
// Definition of interrupt names
#include < avr/io.h >
// ISR interrupt service routine
#include < avr/interrupt.h >
//Midi Library 
#include <MIDI.h>

#define C4 60 //input
#define C3 48 //output
#define A4 69 //input
#define A3 57 //output

#define PATCH_PARAMETER_A 20
#define PATCH_PARAMETER_B 21
#define PATCH_PARAMETER_C 22
#define PATCH_PARAMETER_D 23
#define PATCH_PARAMETER_E 24
#define ledPin1  8
#define ledPin2  9 
#define LED_ON 0 
#define LED_SLOW 1 
#define LED_FAST 2
#define LED_OFF 3

bool midiAssignMode = false; 


byte storedValues[5] = {0,0,0,0,0}; 
byte pedalValues[5] = {0,0,0,0,0}; 
const int button1Pin = 2;// analog pin that button 1 is connect to, button 1 is toggle 
const int button2Pin = 3; // analog pin that button 2 is connect to, button 2 is momentary 
/*const int ledPin1 =  8;  
int led1State = LOW;
const int ledPin2 =  9;  
int led2State = LOW;
*/

int buttonState2 = 0;         // variable for reading the pushbutton status
int lastButtonState2 = LOW;   // the previous reading from the input pin

bool button1ChangeFlag = false;  
long lastDebounceTime1 = 0; 
bool button1Toggle = false; 
bool lastButton1Toggle = false; 

bool button2ChangeFlag = false; 
long lastDebounceTime2 = 0; 

long debounceDelay1 = 100;
long debounceDelay2 = 50;


bool midiTrigger2 = false; 
bool lastMidiTrigger2 = false;
 
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
 MIDI.setHandleNoteOn(HandleNoteOn); 
 MIDI.setHandleNoteOff(HandleNoteOff);
 MIDI.setHandleControlChange(HandleControlChange);  
 MIDI.begin(1); // This sets to the MIDI baudrate (31250), sets the input to channel 1, enables thru
 // Serial.begin(9600); 

}
  /********INTERRUPT********/
ISR(INT0_vect)        
{  
    button1ChangeFlag = true; 
   // Serial.print("flag \n");
}

ISR(INT1_vect)
{
   
   button2ChangeFlag = true;    

}




/**********************************main*************************/
void loop()
{

 MIDI.read();

if(midiAssignMode == true)
{
  LED_FUNCTION(ledPin1,LED_SLOW);
  LED_FUNCTION(ledPin2,LED_SLOW);
}


if(midiAssignMode == true && button1ChangeFlag == true)
{
  buttonState2 = false;
 button1ChangeFlag = false;
  midiAssignMode = false;
  
}

if( button1Toggle == true && buttonState2 == true)
{
  
  midiAssignMode = true; 
  button1Toggle = false;
   buttonState2 = false;
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
     
     if (lastButton1Toggle == true)
     {
       button1Toggle = false;
      // Serial.print("debouncing stage - button off \n");
     }
     if (lastButton1Toggle == false)
     {
      button1Toggle = true;
       // Serial.print("debouncing stage - button on \n");
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

   }

                               /**OUTPUT**/
 if(midiAssignMode != true)
 {
    // if the button state has changed:
    if ( button1Toggle != lastButton1Toggle )   
    {
      

      if(  button1Toggle == true )
      {
        //led1State = HIGH;
                LED_FUNCTION(ledPin1, LED_ON); //on
        MIDI.sendNoteOn(C3, 127, 1); 
       // Serial.print(" output stage - button on \n"); 
      }
     else if(button1Toggle == false)
      {
      // led1State = LOW;
        LED_FUNCTION(ledPin1, LED_OFF); //off
        MIDI.sendNoteOff(C3, 0, 1); 
        //Serial.print(" Output stage - button off \n");
      }

    
       // digitalWrite(ledPin1,led1State); 
       lastButton1Toggle = button1Toggle;
      
    }
    
    
       if (buttonState2 != lastButtonState2 || midiTrigger2 != lastMidiTrigger2 ) 
    {   
       
  
       if(buttonState2 == LOW || midiTrigger2 == true) 
      {
      //led2State = HIGH;
        LED_FUNCTION(ledPin2,LED_ON); // On
        MIDI.sendNoteOn(A3, 127, 1);
        
                 
      }
      else if(buttonState2 == HIGH && midiTrigger2 == false) 
      {
     
        //led2State = LOW;
         MIDI.sendNoteOff(A3, 0, 1); 
         LED_FUNCTION(ledPin2,LED_OFF); // Off 
      }
      // digitalWrite(ledPin2,led2State); 
      lastButtonState2 = buttonState2;
      lastMidiTrigger2 = midiTrigger2; 
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
    
    case LED_SLOW:
    {
      digitalWrite(LED,HIGH);
      delay(500); 
      digitalWrite(LED,LOW);
      break;
 
    }
     case LED_FAST:
    {
      digitalWrite(LED,HIGH);
      delay(250); 
      digitalWrite(LED,LOW);
      delay(250);
       digitalWrite(LED,HIGH);
      delay(250); 
      digitalWrite(LED,LOW);
      delay(250);
     break;
    }
    
    default:
    {
      break; 
    }
  }
  
}

/*****************************MIDI***********************/

/******CC********/
void HandleControlChange(byte channel, byte number, byte value)
{
  if(midiAssignMode)
 {
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

void SENDCC(byte Values[5])
{
  for(int i = 0; i >= 4;)
 {
    switch(i)
    {
      case 0:
      {
        MIDI.sendControlChange(PATCH_PARAMETER_A,Values[0],1);
        break; 
      }
      case 1:
      {
        MIDI.sendControlChange(PATCH_PARAMETER_B,Values[1],1);
        break; 
      }
      case 2:
      {
        MIDI.sendControlChange(PATCH_PARAMETER_C,Values[2],1);
        break; 
      }
      case 3:
      {
        MIDI.sendControlChange(PATCH_PARAMETER_D,Values[3],1);
        break; 
      }
       case 4:
      {
        MIDI.sendControlChange(PATCH_PARAMETER_E,Values[4],1);
        break; 
      }
      default:
      {
        break; 
      }
    }
   
 } 
}


/*****CC****/
void HandleNoteOn(byte channel, byte pitch, byte velocity) // note on
{
      switch(pitch)
      {
        case C4 : //button1
        {
         
          if( velocity >= 1)
          {
            switch(lastButton1Toggle)
            {
              case true:
                {
                  button1Toggle = false; 
                   
                  break;
                }
              case false:
                {
                  button1Toggle = true;
                  break;
                }
              
              default:
                {
                  break;
                }
            }
          }
          break; 
        }
        
        case A4: //button2 
        {
           if( velocity >= 1)
          {
           midiTrigger2 = true; 
          }
        
          break;
        }
        
        default:
        {
          break; 
        }
      }
}

  void HandleNoteOff(byte channel, byte pitch, byte velocity)  // note off
  {
     switch(pitch)
        {
          
          case A4: //button2 
          {
           
             if( velocity == 0 )
            {
             midiTrigger2 = false;
            }
            break;
          }
          default:
          {
            break; 
          }
        }
  }

/*****************Midi************/ 


