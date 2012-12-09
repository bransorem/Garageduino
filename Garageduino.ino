#include <WiFi.h>

char ssid[] = "SSID";          //  your network SSID (name) 
char pass[] = "password";    // your network password
int PORT = 8080;             // any port you choose
WiFiServer server(PORT);
int status = WL_IDLE_STATUS;     // the Wifi radio's status

byte IP = 9;
byte LED = 8;
byte SENSOR = 4;
byte SWITCH = 2;

void setup() {
  // initialize serial:
  Serial.begin(115200);
  Serial.println("Starting Garageduino");
  
  pinMode(LED, OUTPUT);
  pinMode(IP, OUTPUT);
  pinMode(SENSOR, INPUT); //set sensor pin as input for door sensor
  pinMode(SWITCH, OUTPUT); 
  digitalWrite(SWITCH, HIGH);
  
  byte MAC[6];
  WiFi.macAddress(MAC);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    while(true);
  }
  else {
    server.begin();
    Serial.print("IP:");
    IPAddress myAddress = WiFi.localIP();
    Serial.println(myAddress);
    digitalWrite(IP, HIGH);
  }
  
  
}

void loop() {
  // listen for incoming clients
  
  WiFiClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    while (client.connected()) {
      if (client.available()) {
        String get = getcmd(client);
        
        Serial.println(get);
        send_response(client, get);
        break;
      }
    }
    // give the web browser time to receive the data
    delay(1);
      // close the connection:
      client.stop();
      Serial.println("client disonnected");
  }
}


String getcmd(WiFiClient client) {
  String command = "";
  String response = "";
  String temp = "";
  String headers[20];  // up to 20 HTTP headers
  int count = 0;
  char in;
  while(client.available()) { // read full string
    in = client.read();
    if (in == '\n') {
      if (command == "\r") break;
      temp = checkcmd(command);
      if (temp != "") response = temp;
      headers[count] = command;
      count++;
      command = "";
    }
    else {
      command += in;
    }
  }

  // Way to parse and return individual headers stored in 'headers' array
  String header = "";
  for (int i = 0; i < count; i++) {
    header += headers[i];
    Serial.println(headers[i]);
  }
  
  return response;
}

void send_response(WiFiClient client, String response) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connnection: close");
  client.println();
  while (response.length() > 25){
    String first_bit = response.substring(0, 25);
    response.replace(first_bit, "");
    client.println(first_bit);
  }
  if (response.length() > 0){
    client.println(response);
  }
}


String checkcmd(String cmd){
  String response = "";
  
  String header1 = "CUSTOM";
  
  // header with request to turn on led
  if (cmd.startsWith(header1)){
    String tmp = cmd.substring(header1.length() + 1, cmd.length());
    
    if (tmp.compareTo("toggle")){
      digitalWrite(SWITCH, LOW);
      delay(300);
      digitalWrite(SWITCH, HIGH);
    }
    if (tmp.compareTo("status")){
      int a;
      
      if (digitalRead(SENSOR) == HIGH) {
        a = 1;  //DOOR CLOSED
      }
      else {
        a = 0;  //DOOR OPEN
      }
      
      //int a = digitalRead(SENSOR);
      response = String(a);
    }
  }
  
  return response;
}
