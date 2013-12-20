
// Definition of interrupt names
#include < avr/io.h >
// ISR interrupt service routine
#include < avr/interrupt.h >
//Midi Library 
#include <MIDI.h>

const int button1Pin = 2;// analog pin that button 1 is connect to
const int button2Pin = 3; // analog pin that button 2 is connect to
const int analogOutPin = 8 ;  // analog pin that Led 1 is connected to

int buttonState1 = 0;         // variable for reading the pushbutton status
int buttonState2 = 0;         // variable for reading the pushbutton status
int lastButtonState1 = LOW;   // the previous reading from the input pin
int lastButtonState2 = LOW;   // the previous reading from the input pin
bool button1ChangeFlag = false;  
bool currentStateFlag1 = false;
long lastDebounceTime1 = 0; 
bool button2ChangeFlag = false;  
bool currentStateFlag2 = false;
long lastDebounceTime2 = 0; 
long debounceDelay1 = 200;
long debounceDelay2 = 50;



void setup()
{
 MIDI.begin(); // This sets to the MIDI baudrate (31250), sets the input to channel 1, enables thru
  pinMode(button1Pin, INPUT);  // set the button1 as an input to read
  pinMode(button2Pin, INPUT);
  digitalWrite(button1Pin, HIGH);    // Enable pullup resistor
  digitalWrite(button2Pin, HIGH);    // Enable pullup resistor
  pinMode(analogOutPin,   OUTPUT);  // set the button1 as an input to read
  sei();                    // Enable global interrupts
  EIMSK |= (1 << INT0);     // Enable external interrupt INT0
  EICRA |= (1 << ISC01);    // Trigger INT0 on falling edge
  EIMSK |= (1 << INT1);     // Enable external interrupt INT1
  EICRA |= (1 << ISC10);    // Trigger INT1 on any logical change
 
}

ISR(INT0_vect)
{  
    button1ChangeFlag = true; 
}

ISR(INT1_vect)
{
   button2ChangeFlag = true;    
}


void loop()
{

  if(button1ChangeFlag != currentStateFlag1)
  {
      lastDebounceTime1 = millis(); 
      currentStateFlag1 = button1ChangeFlag;
  }

   if(button2ChangeFlag != currentStateFlag2)
  {
      lastDebounceTime2 = millis(); 
      currentStateFlag2 = button2ChangeFlag;
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
     currentStateFlag2 = false; 
   }

    // if the button state has changed:
    if (buttonState1 != lastButtonState1) 
    {
      if(buttonState1 == HIGH)
      {
        Serial.print("Button 1 ON \n");   
        digitalWrite(analogOutPin, HIGH);
        MIDI.send(NoteOn,60,100,01); 
      }
      else if(buttonState1 == LOW)
      {
         Serial.print("Button 1 OFF \n"); 
         digitalWrite(analogOutPin, LOW);
          
      }
      lastButtonState1 = buttonState1;
    }
    
    
       if (buttonState2 != lastButtonState2) 
    { byte C4  = 60; 
        byte velocity = 100;
        byte channel = 01;
    
       if(buttonState2 == LOW)
      {
       
        Serial.print("Button 2 Pressed \n");   
        digitalWrite(analogOutPin, HIGH);
        MIDI.send(NoteOn, C4, velocity, channel); 
      }
      else if(buttonState2 == HIGH)
      {
       Serial.print("Button 2 released \n"); 
       digitalWrite(analogOutPin, LOW);
       MIDI.send(NoteOff, C4, velocity, channel); 
      }
      lastButtonState2 = buttonState2;
    }                
}


