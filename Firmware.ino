#include <SimpleDHT.h>

SimpleDHT11 DHT11;

#include <TimeLib.h>

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial      // Allows BLYNK serial print so no physical device connection is required.
#include <SimpleTimer.h>        // required for <1 second function calls
SimpleTimer timer;

#include <WidgetRTC.h>

#include <TimeAlarms.h>         // required for function calls at specific times, i.e. midnight firmware updates

// HTTP Server

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WiFi.h>

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

BLYNK_WRITE(V1) {
  targetbrightness = param.asInt();
}
BLYNK_WRITE(V2) {
  redbalance = param.asFloat() / 100;
}
BLYNK_WRITE(V3) {
  targethumidity = param.asInt();
}
BLYNK_READ(V4) {
  Blynk.virtualWrite(V4, waterlevel);
}
BLYNK_WRITE(V6) {
  TimeInputParam t(param);
  if (t.hasStartTime()) {
    onhour = t.getStartHour();
    onminute = t.getStartMinute();
  }
  if (t.hasStopTime()) {
    offhour = t.getStopHour();
    offminute = t.getStopMinute();
  }
}

WidgetRTC rtc;
BLYNK_ATTACH_WIDGET(rtc, V5);


char blynkauth[] = "6343b1dd1db2495abb1e457217ec0aad";

const char ssid[] = "Triangle";
const char pass[] = "CliftonLife6";

WiFiClient client;

unsigned long myChannelNumber = 162238;
const char * myWriteAPIKey = "XXBMA2W43TTEQEHP";

void dataupload(){
  Serial.println("Uploading to ThingSpeak");
  if (WiFi.status() != WL_CONNECTED) {
    ThingSpeak.setField(1, ambientbrightness);
    ThingSpeak.setField(2, waterlevel);
    ThingSpeak.setField(3, humidity);
    ThingSpeak.setField(4, temperature);
    ThingSpeak.setField(5, targetbrightness);
    ThingSpeak.setField(6, redbalance);
    ThingSpeak.setField(7, targethumidity);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    Serial.println("Upload Complete");
  }
  else{
    Serial.println("No WiFi Connection - Upload failed");
  }
}

void reconnect(){
  Serial.println("Checking connections");
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected - Reconnecting");
    WiFi.begin(ssid, pass);
  }
  else {
    Serial.println("WiFi still connected");
  }
  if (Blynk.connect() == false) {
    Serial.println("Blynk Disconnected - Reconnecting");
    Blynk.config(blynkauth);
    delay(500);
  }
  else {
    Serial.println("Blynk still connected");
  }
}

void blynksync(){
  Blynk.syncAll();
}

void serialdebug(){
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

void firmwareupdate(){
  Serial.println("Inititating software update");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Connected - Begin update");
    ESPhttpUpdate.update("https://github.com/LettUsGrow/Firmware/raw/master/sketch_oct12a.ino.d1_mini.bin"); // not dependant on 
    /* ********************** This only works if the update webpage sends bk some information!
    t_httpUpdate_return ret = ESPhttpUpdate.update("https://github.com/LettUsGrow/Firmware/raw/master/sketch_oct12a.ino.d1_mini.bin");
    switch(ret) {
        case HTTP_UPDATE_FAILED:
            //Serial.println("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            Serial.println("HTTP_UPDATE_FAILED");
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("HTTP_UPDATE_NO_UPDATES");
            break;

        case HTTP_UPDATE_OK:
            Serial.println("HTTP_UPDATE_OK");
            Serial.println("Rebooting board");
            ESP.restart();
            break;
    }
    */
    delay(1000);
    ESP.restart();
  }
  else {
    reconnect();
    firmwareupdate();
  }
}

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

  Serial.println("Set default time");

  setTime(7, 59, 30, 1, 1, 2015);

  Serial.println("Set wifi");

  WiFi.mode(WIFI_STA);
  int cnt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (cnt++ >= 10) {
      WiFi.beginSmartConfig();
      while (1) {
        delay(1000);
        if (WiFi.smartConfigDone()) {
          Serial.println();
          Serial.println("SmartConfig: Success");
          break;
        }
        Serial.print("|");
      }
    }

  Serial.println("Start Thing Speak");

  ThingSpeak.begin(client);

  Serial.println("Start Blynk");

  //Blynk.config(blynkauth);
  //delay(50);
  //if (Blynk.connect() == false) {
  //  Serial.println("Blynk failed to connect");
  //}
  
  rtc.begin();

  Blynk.syncAll();

  Serial.println("Setup timers");

//Timers and alarms
  timer.setInterval(60000,dataupload);       //  upload to Thing Speak every minute
  //timer.setInterval(300000, reconnect);      //  Check wifi and blynk connections and reconnect if failed every five minutes
  timer.setInterval(500, blynksync);      //  Sync with the Blynk app every 500ms
  timer.setInterval(1000, serialdebug);      //  Show the time and other variables every second

  //Alarm.alarmRepeat(0, 0, 0, firmwareupdate);
  
  Serial.println("Setup Complete");
}

void loop() {       //  Alarm delay, Blync, Sensor sampling,
  BlynkProvisioning.run();
  
  Alarm.delay(0);
  Blynk.run();
  timer.run();

// ******************   Sensor sampling
  digitalWrite(lightsensorswitch, HIGH);
  delay(10);
  //ambientbrightness = analogRead(lightsensorpin)/10.23;
  ambientbrightness = 0;
  delay(10);
  digitalWrite(lightsensorswitch, LOW);
  
  //Serial.println("read hum temp");
  DHT11.read(humiditysensorpin, &temperature, &humidity, NULL);

  digitalWrite(foggerpin, LOW);
  delay(10);
  digitalWrite(lightsensorswitch,LOW);
  digitalWrite(watersensorswitch,HIGH);
  delay(10);
  waterlevel = analogRead(watersensorpin)/10.23;
  delay(10);
  digitalWrite(watersensorswitch,LOW);

//Water level change blynk colours - (SLOWS CLOCK SPEED MASSIVELY!)
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

// ******************   Fogger logic
  if ((hour() >= onhour) && (hour() < offhour) && (minute() >= onminute) && (minute() < offminute) && (waterlevel >= 25)) { // checks if the fogger should be on
    if ((second()>= 0) && (second()<= 60*targethumidity/100)){
      digitalWrite(foggerpin, HIGH);  
    }
    else{
      digitalWrite(foggerpin, LOW);
    }
  }

// ******************   Lighting Logic
  if ((hour() >= onhour) && (hour() < offhour) && (minute() >= onminute) && (minute() < offminute)) {    // checks the lights should be on
    if ( ambientbrightness < targetbrightness ) {              // check the light needs topping up
      ledbrightness = (targetbrightness - ambientbrightness - 1) ; //-1 ensures the LED doesnt light up at zero output

      red = redbalance * ledbrightness * 2.55;
      blue = (1 - redbalance) * (ledbrightness * 2.55);

      analogWrite(ledredpin, red);        // output to red led
      analogWrite(ledbluepin, blue);   //output to blue led
    }
    else {
      ledbrightness = 0;
      analogWrite(ledredpin, ledbrightness);
      analogWrite(ledbluepin, ledbrightness);
    }
  }
  else {
    analogWrite(ledredpin, 0);
    analogWrite(ledbluepin, 0);
  }
}

