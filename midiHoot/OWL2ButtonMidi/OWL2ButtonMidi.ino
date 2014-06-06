
// Definition of interrupt names
#include < avr/io.h >
// ISR interrupt service routine
#include < avr/interrupt.h >
//Midi Library 
#include <MIDI.h>

const int button1Pin = 2;// analog pin that button 1 is connect to, button 1 is toggle 
const int button2Pin = 3; // analog pin that button 2 is connect to, button 2 is momentary 
const int ledPin1 =  8;  
int led1State = LOW;
const int ledPin2 =  9;  
int led2State = LOW;

int buttonState1 = 0;         // variable for reading the pushbutton status
int buttonState2 = 0;         // variable for reading the pushbutton status
int lastButtonState1 = LOW;   // the previous reading from the input pin
int lastButtonState2 = LOW;   // the previous reading from the input pin

bool button1ChangeFlag = false;  
bool currentStateFlag1 = false;
long lastDebounceTime1 = 0; 

bool button2ChangeFlag = false; 
bool midiButton2 = false;
bool lastMidiButton2State = false; 
long lastDebounceTime2 = 0; 

long debounceDelay1 = 100;
long debounceDelay2 = 50;


 
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
 MIDI.begin(1); // This sets to the MIDI baudrate (31250), sets the input to channel 1, enables thru

}

ISR(INT0_vect)
{  
    button1ChangeFlag = true; 

}

ISR(INT1_vect)
{
   
   button2ChangeFlag = true;    

}

void HandleNoteOn(byte channel, byte pitch, byte velocity)
{
      switch(pitch)
      {
        case 52: //button1
        {
         
          if( velocity > 1)
          {
         button1ChangeFlag  = true; 
          }
          break; 
        }
        case 42: //button2 
        {
           if( velocity > 1)
          {
           button2ChangeFlag = true; 
           midiButton2 = true; 
          }
           if( velocity == 0 )
          {
           button2ChangeFlag = true; 
           midiButton2 = false; 
          }
          break;
        }
        default:
        {
          break; 
        }
      }
}


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
  
  if((lastDebounceTime1 != 0) && (millis()-lastDebounceTime1) > debounceDelay1)
   {
     
     if (lastButtonState1 == HIGH)
     {
       buttonState1 = LOW;
     }
       if (lastButtonState1 == LOW)
     {
       buttonState1 = HIGH;
     }
      lastDebounceTime1 = 0;
      button1ChangeFlag = false; 
      currentStateFlag1 = false; 
   }
   
   
   if((lastDebounceTime2 != 0) && (millis()-lastDebounceTime2) > debounceDelay2)
   {
     buttonState2 = digitalRead(button2Pin); 
     lastDebounceTime2 = 0;
     button2ChangeFlag = false; 

   }

    // if the button state has changed:
    if (buttonState1 != lastButtonState1) 
    {
    
    
      if(buttonState1 == HIGH)
      {
        led1State = HIGH;
        MIDI.sendNoteOn(32, 127, 1); 
      }
      else if(buttonState1)
      {
       led1State = LOW;
        MIDI.sendNoteOff(52, 0, 1); 
      }
      lastButtonState1 = buttonState1;
      digitalWrite(ledPin1,led1State); 
      
    }
    
    
       if (buttonState2 != lastButtonState2  ) 
    {   
       
  
       if(buttonState2 == LOW || midiButton2 != lastMidiButton2State) 
      {
      led2State = HIGH;
       
         MIDI.sendNoteOn(62, 127, 1);
        
                 
      }
      else if(buttonState2 == HIGH || midiButton2 != lastMidiButton2State ) 
      {
     
    led2State = LOW;
       MIDI.sendNoteOff(62, 0, 1); 
      }
      
      lastButtonState2 = buttonState2;
      lastMidiButton2State = midiButton2; 
    }                
}


