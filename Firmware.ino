/*
LED Light Intensity Control
 The analogWrite() function uses PWM, so if
 you want to change the pin you're using, be
 sure to use another PWM capable pin. On most
 Arduino, the PWM pins are identified with 
 a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11.
 */
#include <SimpleDHT.h>

SimpleDHT11 DHT11;

#include <TimeLib.h>

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <WidgetRTC.h>

#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


  //  Set global variables
int ledredpin = D1;              //set red led pin
int ledbluepin = D2;             // set blue led pin
int lightsensorpin = A0;        //set light sensor pin

int foggerpin = D4;              //set fogger output pin
int humiditysensorpin = D8;     // set humidty sensor pin

int watersensorpin = A0;        // set water sensor in
int lightsensorswitch = D5;
int watersensorswitch = D6;

// Set defalt operating values

int targetbrightness = 100;     // target light intensity as %age

int targethumidity = 50;        // target humidty as %age
int humiditylow = 25;

int warningwaterlevel = 50;     // water level %age to display warning
int stopwaterlevel = 25;        // water level %age to terminate fogger

// create operating values
int ambientbrightness = 13;
int waterlevel = 47;
byte humidity = 15;
byte temperature = 15;


// set light start and end hour
int onhour = 8;
int onminute = 0;
int offhour = 20;
int offminute = 0;

//set starting light brightness
int ledbrightness = 0;
float redbalance = 0.5;
float red;
float blue;

BLYNK_WRITE(V1){
  targetbrightness = param.asInt();
}
BLYNK_WRITE(V2){
  redbalance = param.asFloat()/100;
}
BLYNK_WRITE(V3){
  targethumidity = param.asInt();
}
BLYNK_READ(V4){
   Blynk.virtualWrite(V4, waterlevel);
}
BLYNK_WRITE(V6){
  TimeInputParam t(param);
  if (t.hasStartTime()){
    onhour = t.getStartHour();
    onminute = t.getStartMinute();
  }
  if (t.hasStopTime()){
    offhour = t.getStopHour();
    offminute = t.getStopMinute();
  }
}

WidgetRTC rtc;
BLYNK_ATTACH_WIDGET(rtc,V5);


char auth[] = "15721cd4ce454bc99a15c7073f5e9ac9";

const char ssid[] = "LetUsGrow";
const char pass[] = "welovesalad";

WiFiClient client;

unsigned long myChannelNumber = 162238;
const char * myWriteAPIKey = "XXBMA2W43TTEQEHP";



void setup() {
  Serial.begin(115200);
  Serial.println("Set pins");
  
  //set output pins
  pinMode(ledredpin, OUTPUT);
  pinMode(ledbluepin, OUTPUT);
  pinMode(foggerpin, OUTPUT);
  pinMode(lightsensorswitch, OUTPUT);
  pinMode(watersensorswitch, OUTPUT);

  //set input pins
  pinMode(lightsensorpin, INPUT);
  pinMode(humiditysensorpin, INPUT);
  pinMode(watersensorpin, INPUT);

// Set-up OTA code updates

Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
 // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
 // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
 // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Set time");
  
  setTime(7,59,30,1,1,2015);

  Serial.println("Set wifi");
  WiFi.begin(ssid,pass);

  Serial.println("Start Thing Speak");
  
  ThingSpeak.begin(client);

  Serial.println("Start Blynk");
  
  Blynk.begin(auth, ssid, pass);

  rtc.begin();

  Blynk.syncAll();

  Serial.println("Setup Complete");
}

void loop() {
  //Read sensors
  //digitalWrite(lightsensorswitch, HIGH);
  //delay(10);
  //ambientbrightness = analogRead(lightsensorpin)/10.23;
  ambientbrightness = 0;
  //delay(10);
  //digitalWrite(lightsensorswitch, LOW);

  //delay(10);

  //digitalWrite(watersensorswitch, HIGH);
  //delay(10);
  //waterlevel = analogRead(watersensorpin) / 10.23;                  // reads the water level sensor and converts to %age
  //delay(10);
  //digitalWrite(watersensorswitch, LOW);
  
//Serial.println("read hum temp");
  DHT11.read(humiditysensorpin, &temperature, &humidity, NULL);

  //run blynk
  Blynk.run();
  
  //IF wifi, thingspeak, blynk not connected, restart each of them

  if (second() == 0){
    WiFi.begin(ssid,pass);
    if(Blynk.connect() == false){
      Serial.print("Blynk Disconnected - Reconnecting");
      Blynk.begin(auth, ssid, pass);
      delay(500);
    }
    
    Serial.println("upload data");
    Blynk.syncAll();
    
    ThingSpeak.setField(1, ambientbrightness);
    ThingSpeak.setField(2, waterlevel);
    ThingSpeak.setField(3, humidity);
    ThingSpeak.setField(4, temperature);
    ThingSpeak.setField(5, targetbrightness);
    ThingSpeak.setField(6, redbalance);
    ThingSpeak.setField(7, targethumidity);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);    
    delay(1000);
  }

//Water level change blynk colours SLOWS CLOCK SPEED MASSIVELY!
/*Serial.println("run water logic");
  if ( waterlevel <= warningwaterlevel && waterlevel > stopwaterlevel ) { //running a bit low
    Blynk.setProperty(V4, "color", "#ED9D00");
  }
  else if ( waterlevel <= stopwaterlevel ) {
    Blynk.notify(String("Please refill the water"));
    Blynk.setProperty(V4, "color", "#D3435C");
  }
  else {
    Blynk.setProperty(V4, "color", "#04C0F8");
  }\
*/
//Serial.println("run fog logic");
  
  if ((hour() >= onhour) && (hour() < offhour) && (minute() >= onminute) && (minute() < offminute) && (waterlevel >= 25)){  // checks if the fogger should be on
    if (humidity < humiditylow){
      digitalWrite(foggerpin, HIGH);                              // signal to turn on fogger relay
    }
    else if( humidity >= targethumidity) {
      digitalWrite(foggerpin, LOW);
    }
  }
//Serial.println("run light logic");
    if ((hour() >= onhour) && (hour() < offhour) && (minute() >= onminute) && (minute() < offminute)){     // checks the lights should be on
    if( ambientbrightness < targetbrightness ){                // check the light needs topping up
      ledbrightness = (targetbrightness - ambientbrightness -1) ;  //-1 ensures the LED doesnt light up at zero output

      red = redbalance*ledbrightness*2.55;
      blue = (1-redbalance)*(ledbrightness*2.55);
     
      analogWrite(ledredpin, red);        // output to red led
      analogWrite(ledbluepin, blue);   //output to blue led
      //digitalWrite(ledredpin, HIGH);
      //digitalWrite(ledbluepin, HIGH);
    
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

  Serial.print(year());
  Serial.print("  ");
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.print(second());
  Serial.print("  ");
  Serial.print(ambientbrightness);
  Serial.print("  ");
  Serial.print(red);
  Serial.print("  ");
  Serial.print(blue);
  Serial.print("  ");
  Serial.print(redbalance);
  Serial.print("  temp: ");
  Serial.print(temperature);
  Serial.print("  hum: ");
  Serial.print(targethumidity);
  Serial.print("  wat: ");
  Serial.print(waterlevel);
  Serial.print("  Start: ");
  Serial.print(onhour);
  Serial.print(":");
  Serial.print(onminute);
  Serial.print("  Stop: ");
  Serial.print(offhour);
  Serial.print(":");
  Serial.println(offminute);
  
}
