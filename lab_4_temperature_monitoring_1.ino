String ssid = "Simulator Wifi";  
String password = "";
String host = "api.thingspeak.com";
const int httpPort = 80;

// API key for your single ThingSpeak channel
String apiKey = "67XR4D15YNW0MHLD";  // Channel 1

int pirPin = 8;  // PIR motion sensor output pin

void sendToThingSpeak(String apiKey, int temp, int motion) {
  // Send both fields in one request
  String uri = "/update?api_key=" + apiKey +
               "&field1=" + String(temp) +
               "&field2=" + String(motion);
  String httpPacket = "GET " + uri + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
  int length = httpPacket.length();

  Serial.print("AT+CIPSEND=");
  Serial.println(length);
  delay(10);

  Serial.print(httpPacket);
  delay(10);

  Serial.find("SEND OK\r\n");
}

int setupESP8266(void) {
  Serial.begin(115200);  
  Serial.println("AT");  
  delay(10);        
  if (!Serial.find("OK")) return 1;

  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  delay(10);        
  if (!Serial.find("OK")) return 2;
 
  Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
  delay(50);        
  if (!Serial.find("OK")) return 3;

  return 0;
}

void anydata(void) {
  int temp = map(analogRead(A0), 20, 358, -40, 125);  // TMP36
  int motion = digitalRead(pirPin);                   // PIR sensor (0 or 1)

  sendToThingSpeak(apiKey, temp, motion);
}

void setup() {
  pinMode(pirPin, INPUT);
  setupESP8266();
}

void loop() {
  anydata();
  delay(10000);  // send data every 10 seconds
}