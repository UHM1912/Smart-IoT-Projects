// Using hardware Serial for ESP8266 communication just like previous lab

String ssid = "Simulator Wifi";
String password = "";  // Empty for open network
String host = "api.thingspeak.com";
const int httpPort = 80;

String apiKey = "U1P21V9YTJ3HO7FG";  // Your ThingSpeak key

const int pingPin = 9;   // PING sensor single pin
const int ledPin = 7;    // LED pin

void sendToThingSpeak(String apiKey, int distance, int binFull) {
  String uri = "/update?api_key=" + apiKey +
               "&field1=" + String(distance) +
               "&field2=" + String(binFull);
 
  String httpPacket = "GET " + uri + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";

  Serial.print("AT+CIPSEND=");
  Serial.println(httpPacket.length());
  delay(100);
 
  if (Serial.find(">")) {
    Serial.print(httpPacket);
    delay(500);
    Serial.find("SEND OK");
  }
}

int setupESP8266() {
  Serial.begin(115200);  // Hardware Serial at 115200 baud
  delay(2000);

  Serial.println("AT");
  delay(2000);
  if (!Serial.find("OK")) return 1;

  Serial.print("AT+CWJAP=\"");
  Serial.print(ssid);
  Serial.print("\",\"");
  Serial.print(password);
  Serial.println("\"");
  delay(6000);
 
  if (!Serial.find("OK")) return 2;

  Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
  delay(2000);
  if (!Serial.find("OK")) return 3;

  return 0;
}

long getDistance() {
  long duration, distance;
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  pinMode(pingPin, INPUT);
 
  duration = pulseIn(pingPin, HIGH, 30000);  // 30ms timeout
  if(duration == 0) return -1;
  distance = duration * 0.034 / 2;
  return distance;
}

void anydata() {
  long distance = getDistance();
  int binFull = (distance >= 0 && distance < 50) ? 1 : 0;

  digitalWrite(ledPin, binFull);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Bin Full: ");
  Serial.println(binFull);

  sendToThingSpeak(apiKey, distance, binFull);
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Start Serial for ESP8266 communication and debugging
  Serial.begin(115200);

  int result = setupESP8266();
  if(result != 0) {
    Serial.print("ESP8266 setup failed, error code: ");
    Serial.println(result);
  } else {
    Serial.println("ESP8266 connected");
  }
}

void loop() {
  anydata();
  delay(15000);  // ThingSpeak rate limit
}
