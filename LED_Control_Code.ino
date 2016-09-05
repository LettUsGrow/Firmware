/*
LED Light Intensity Control
 The analogWrite() function uses PWM, so if
 you want to change the pin you're using, be
 sure to use another PWM capable pin. On most
 Arduino, the PWM pins are identified with 
 a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11.
 */

// include libraries

#include<Time.h>
#include <TimeAlarms.h> // seems to be needed to use setTime command

//Set pin variables

int ledredpin = 5;              //set red led pin
int ledbluepin = 6;             // set blue led pin
int lightsensorpin = A0;        //set light sensor pin

int foggerpin = 4;              //set fogger output pin
int humiditysensorpin = A1;     // set humidty sensor pin

int watersensorpin = A2;        // set water sensor pin

// Set defalt operating values

int targetbrightness = 100;     // target light intensity as %age
int lightpgain = 1;             // proportional gain for light control code CURRENTLY UNUSED


int targethumidity = 50;        // target humidty as %age CURRENTLY UNUSED

//LED and fog starting values
int ledon = 0;
int fog = 0;

// set fogger start and end hour
int foggeronhour = 8;
int foggeroffhour = 20;

// set light start and end hour
int lightonhour = 8;
int lightoffhour = 20;

//set starting light brightness
int ledbrightness = 0;
int redbalance = 0.5;


void setup() {

  //set output pins
  pinMode(ledredpin, OUTPUT);
  pinMode(ledbluepin, OUTPUT);
  pinMode(foggerpin, OUTPUT);

  //set input pins
  pinMode(lightsensorpin, INPUT);
  pinMode(humiditysensorpin, INPUT);
  pinMode(watersensorpin, INPUT);

  //set local clock
  setTime(7, 59, 50, 1, 1, 2016);

  // start serial output
  Serial.begin(9600);

  // set variables
  ledbrightness = 0;

}

void loop() {
  int ambientbrightness = analogRead(lightsensorpin) / 10.23;  // reads the input from the light sensor and normalises to %age of the max 1023 output

  if ( (hour() >= foggeronhour) && (hour() < foggeroffhour)) {  // checks if the fogger should be on
    fog = 1; //placeholder for now
    digitalWrite(foggerpin, HIGH);                              // signal to turn on fogger relay

  }
  else {
    fog = 0;
    digitalWrite(foggerpin, LOW);                               // signal to turn off fogger relay
  }
  
  if ((hour() >= lightonhour) && (hour() < lightoffhour)){     // checks the lights should be on
    if( ambientbrightness < targetbrightness ){                // check the light needs topping up
      ledbrightness = targetbrightness - ambientbrightness -1;  //-1 ensures the LED doesnt light up at zero output
      
      analogWrite(ledredpin, redbalance*(ledbrightness*2.55));        // output to red led
      analogWrite(ledbluepin, (1-redbalance)*(ledbrightness*2.55));   //output to blue led
    }
    else{
      ledbrightness = 0;
      analogWrite(ledredpin, ledbrightness);
      analogWrite(ledbluepin, ledbrightness);
    }
  }
  else{
    analogWrite(ledredpin, 0);
    analogWrite(ledbluepin, 0);
  }
  

  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.print(second());
  Serial.print("  Ambient Brightness: ");
  Serial.print(ambientbrightness);
  Serial.print("   LED: ");
  Serial.println(ledbrightness*2.55);


}
