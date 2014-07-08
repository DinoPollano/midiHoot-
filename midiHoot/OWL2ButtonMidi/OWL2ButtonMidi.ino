
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


const int button1Pin = 2;// analog pin that button 1 is connect to, button 1 is toggle 
const int button2Pin = 3; // analog pin that button 2 is connect to, button 2 is momentary 
const int ledPin1 =  8;  
int led1State = LOW;
const int ledPin2 =  9;  
int led2State = LOW;


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
  pinMode(ledPin1,   OUTPUT);  // set the button1 as an input to read
  pinMode(ledPin2,   OUTPUT);  // set the button1 as an input to read
  
  sei();                    // Enable global interrupts
  EIMSK |= (1 << INT0);     // Enable external interrupt INT0
  EICRA |= (1 << ISC01);    // Trigger INT0 on falling edge
  EIMSK |= (1 << INT1);     // Enable external interrupt INT1
  EICRA |= (1 << ISC10);  // Trigger INT1 on any logical change 
 MIDI.setHandleNoteOn(HandleNoteOn); 
 MIDI.setHandleNoteOff(HandleNoteOff); 
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
/*****************************MIDI IN***********************/
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

/*****************Midi in************/ 

/**********************************main*************************/
void loop()
{

 MIDI.read();

  
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
     buttonState2 = digitalRead(button2Pin); 
     lastDebounceTime2 = 0;
     button2ChangeFlag = false; 

   }

                               /**OUTPUT**/

    // if the button state has changed:
    if ( button1Toggle != lastButton1Toggle )   
    {
      

      if(  button1Toggle == true )
      {
        led1State = HIGH;
        MIDI.sendNoteOn(C3, 127, 1); 
       // Serial.print(" output stage - button on \n"); 
      }
     else if(button1Toggle == false)
      {
       led1State = LOW;
        MIDI.sendNoteOff(C3, 0, 1); 
        //Serial.print(" Output stage - button off \n");
      }

    
        digitalWrite(ledPin1,led1State); 
       lastButton1Toggle = button1Toggle;
      
    }
    
    
       if (buttonState2 != lastButtonState2 || midiTrigger2 != lastMidiTrigger2 ) 
    {   
       
  
       if(buttonState2 == LOW || midiTrigger2 == true) 
      {
      led2State = HIGH;
       
        MIDI.sendNoteOn(A3, 127, 1);
        
                 
      }
      else if(buttonState2 == HIGH && midiTrigger2 == false) 
      {
     
        led2State = LOW;
         MIDI.sendNoteOff(A3, 0, 1); 
      }
       digitalWrite(ledPin2,led2State); 
      lastButtonState2 = buttonState2;
      lastMidiTrigger2 = midiTrigger2; 
    }               
}

/************main********/ 
