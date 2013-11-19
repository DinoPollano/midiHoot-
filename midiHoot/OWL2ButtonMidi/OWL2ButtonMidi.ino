
// Definition of interrupt names
#include < avr/io.h >
// ISR interrupt service routine
#include < avr/interrupt.h >

const int button1Pin = 2;// analog pin that button 1 is connect to 
const int button2Pin = 3; // analog pin that button 2 is connect to 
const int analogOutPin = 8 ;  // analog pin that Led 1 is connected to

     
int buttonState1 = 0;         // variable for reading the pushbutton status
int buttonState2 = 0;         // variable for reading the pushbutton status
int lastButtonState1 = LOW;   // the previous reading from the input pin
int lastButtonState2 = LOW;   // the previous reading from the input pin

volatile int reading1 = LOW;
volatile int reading2 = LOW;



void setup() 
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  
  
  pinMode(button1Pin, INPUT);  // set the button1 as an input to read
  digitalWrite(button1Pin, HIGH);    // Enable pullup resistor
  pinMode(button2Pin, INPUT);
  pinMode(analogOutPin,   OUTPUT);  // set the button1 as an input to read
  sei();                    // Enable global interrupts
  EIMSK |= (1 << INT0);     // Enable external interrupt INT0
  
  EICRA |= (1 << ISC01);    // Trigger INT0 on falling edge

}

void loop() 
{
  
  

//reading1 = digitalRead(button1Pin);
reading2 = digitalRead(button2Pin);

//buttonState1 = reading1;
buttonState2 = reading2; 
 

    // if the button state has changed:
   
    if (buttonState1 != lastButtonState1) 
    {
      if(buttonState1 == HIGH)
      {
        Serial.print("Button 1 Pressed \n");   
        digitalWrite(analogOutPin, HIGH);
      }
      else if(buttonState1 == LOW)
      {
         Serial.print("Button 1 released \n"); 
         digitalWrite(analogOutPin, LOW);
      }
      lastButtonState1 = buttonState1;
    }
    
    
       if (buttonState2 != lastButtonState2) 
    {
    
       if(buttonState2 == HIGH)
      {
        Serial.print("Button 2 Pressed \n");   
        digitalWrite(analogOutPin, HIGH);
      }
      else if(buttonState2 == LOW)
      {
       Serial.print("Button 2 released \n"); 
       digitalWrite(analogOutPin, LOW);
      }
      lastButtonState2 = buttonState2;
    }
 
                     
}

ISR(INT0_vect)
{
  Serial.print("interrupt 1 works");
}
