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

int targetbrightness = 80;     // target light intensity as %age
int lightpgain = 1;             // proportional gain for light control code CURRENTLY UNUSED

int targethumidity = 50;        // target humidty as %age
int humiditylow = 25;

int warningwaterlevel = 50;     // water level %age to display warning
int stopwaterlevel = 25;        // water level %age to terminate fogger

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
float redbalance = 0.6;
int red;
int blue;

// water level warnings

String warning;


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
  int ambientbrightness = 100 - (analogRead(lightsensorpin) / 10.23);   // reads the input from the light sensor and normalises to %age of the max 1023 output
  int waterlevel = analogRead(watersensorpin) / 10.23;                  // reads the water level sensor and converts to %age
  int humidity = analogRead(humiditysensorpin) / 10.23;                 // reads humidity sensor as a %age

  if ( waterlevel <= 50 && waterlevel > 25 ) {
    warning = "Water low";
  }
  else if ( waterlevel <= 25 ) {
    warning = "No Water";
  }
  else {
    warning = "Water ok";
  }

 
  if ( (hour() >= foggeronhour) && (hour() < foggeroffhour) && (waterlevel >= 25)){  // checks if the fogger should be on
    if (humidity < humiditylow){
      fog = 1; //placeholder for now
      digitalWrite(foggerpin, HIGH);                              // signal to turn on fogger relay
    }
    else if( humidity >= targethumidity) {
      fog = 0;
      digitalWrite(foggerpin, LOW);
    }
  }
  
  if ((hour() >= lightonhour) && (hour() < lightoffhour)){     // checks the lights should be on
    if( ambientbrightness < targetbrightness ){                // check the light needs topping up
      ledbrightness = 0.25* (targetbrightness - ambientbrightness -1) ;  //-1 ensures the LED doesnt light up at zero output

      red = redbalance*ledbrightness*2.55;
      blue = (1-redbalance)*(ledbrightness*2.55);


      
      analogWrite(ledredpin, red);        // output to red led
      analogWrite(ledbluepin, blue);   //output to blue led
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
  Serial.print("  Ambient: ");
  Serial.print(ambientbrightness);
  Serial.print("   LED: ");
  Serial.print("Red: ");
  Serial.print(red);
  Serial.print("  Blue: ");
  Serial.print(blue);
  Serial.print("   All: ");
  Serial.print(ledbrightness);
  Serial.print("   Water level: ");
  Serial.print(waterlevel);
  Serial.print("   Humidity: ");
  Serial.print(humidity);
  Serial.print("   Fogger: ");
  Serial.print(fog);
  Serial.print("   ");
  Serial.println(warning);
}
