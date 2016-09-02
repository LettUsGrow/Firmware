/*
LED Light Intensity Control
 The analogWrite() function uses PWM, so if
 you want to change the pin you're using, be
 sure to use another PWM capable pin. On most
 Arduino, the PWM pins are identified with 
 a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11.
 */

int led1 = 5; // the PWM pin the LED is attached to
int led2 = 6;
int led3 = 9;
int led4 = 10;
int fogger = 0; // set fogger output pin no. (not PWM)
int ambientlight = A0; //set light sensor input pin


// SET DEFAULT VARIABLES
int defaultbrightness = 100;   // Default LED desired brightness %age
int fadeamount = 1;    // LED intensity change steps      %age
int ledon ;          //set default LED on time       
int ledoff;         //set default LED off time
int fogon = ledon;         //set fogger on time
int fogoff = ledoff;       //set fogger off time
int foghumidity = 100;    // set default desired fog humidity %age
int startbrightness = 0;
int targetbrightness;
int ledbrightness;

// the setup routine runs once when you press reset:
void setup() {
  // declare led pins to be an output:
  pinMode(led1, OUTPUT);
  //pinMode(led2, OUTPUT);
  //pinMode(led3, OUTPUT);
  //pinMode(led4, OUTPUT);
  //pinMode(fogger, OUTPUT);
  //declare input pins
  pinMode(A0, INPUT);
  // set variables
  targetbrightness = defaultbrightness;
  ledbrightness = startbrightness;
  analogWrite(led1, ledbrightness);

  //setup serial output for Pot (testing only)
  Serial.begin(9600);
    
}

// the loop routine runs over and over again forever:
void loop() {
 // set local clock
 // should the lights be on?
 //if(hour() >= hour(ledon)){


 
  int ambientbrightness = analogRead(A0)/10.23;  //read ambient light sensor %age
 Serial.print(ambientbrightness);
 Serial.print("    ");
 Serial.print(ledbrightness);
   Serial.print("    ");
 Serial.println(ledbrightness*2.56);
 
  if(ambientbrightness < targetbrightness){    //if the ambient brightness is lower than the target brightness
   int ledtargetbrightness = targetbrightness-ambientbrightness;   //calculate the additional LED brighness to top upto perfect
   
   if (ledbrightness < ledtargetbrightness){ //if the current led brightness is not correct the change it
    ledbrightness = ledbrightness + fadeamount;  //increase or decrease the brightness by the step amount
    analogWrite(led1, ledbrightness*2.56-2.56); //output the new value
   }
   if (ledbrightness > ledtargetbrightness){ //if the current led brightness is not correct the change it
    ledbrightness = ledbrightness - fadeamount;  //increase or decrease the brightness by the step amount
    analogWrite(led1, ledbrightness*2.56-2.56); //output the new value
   }
   delay(25);
  }
 //}
}

void ServerSet(){  //ping the server to set variables
  // capture desired brightness from the server
  
  // set local clock from system clock
 
  //set light on time
  
  //set light off time
  
  //set fogger on time
  
  //set fogger off time
  
  //set fogger humidty from server
}

void ServerData(){  //send data to the server

}
