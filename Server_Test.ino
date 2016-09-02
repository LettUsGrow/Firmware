*
 * Test server for Lettus Grow.
 * 
 * Basic idea is that the Node server sends simple commands to this
 * server as trailing slashes after this servers address (crudely).
 * Only works if the Node server is on the same Wifi network.
 * Currently only works with an Arduino Uno Wifi.
 *
 * Created 30th June 2016
 * By Louis JA
 * Modified Last Time 4th July by Reinaldo Melo
 */


#include <Wire.h>
#include <ArduinoWiFi.h>

void setup() {
  Serial.begin(9600);
  Wifi.begin();                                     // Start Wifi server
}
void loop() {
  while (Wifi.available()) {
    process(Wifi);
  }
}

void process(WifiData client) {
  String command = client.readStringUntil('/');    // Read the command
  
  String response = "HTTP/1.1 200 OK\n";           // HTTP header
  response += "Content-Type: application/json\n";
  response += "Accept: application/json\n";
  response += "Connection: close\n\n";             // Don't keep the connection open after sending this data

  if(command == "updateLight") {                         // Can't switch on String's in Arduino's Java
    response += lights(client);
  } else {
    response += invalid();
  }

  response += DELIMITER;                           // Signals the end of the HTTP request
  client.print(response);                          // Send to client
}

// Update the value of a particular light
String updateLight() {
  if (client.read() != '/') {                      // If the next character is not a '/' we have a malformed URL
    return invalid();
  }
  
  int light = client.parseInt();                   // Which light are we changing
  
  if (client.read() != '/') { 
    return invalid(); 
  }
  
  int value = client.parseInt();
  
  /* INSERT CODE HERE */
  return "";
}

String invalid() {
  return "{ \"error\": 0 }\n";
}https://codeshare.io/6arE5
