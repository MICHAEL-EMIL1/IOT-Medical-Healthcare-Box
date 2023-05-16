#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

float dis , duration;

const char *ssid = "Redmi 9";
const char *password = "mernaalaa123";

const char* host = "maker.ifttt.com";
const char* apiKey = "p0gFxtfnYkHQP_nCdtTJgNRoJOGlbclJmXH-V2K48kX";

WiFiClient client;
HTTPClient http;
WiFiUDP ntpUdp;
NTPClient timeClient (ntpUdp , "pool.ntp.org" , 7200);

void setup() {

  Serial.begin(115200);
  Serial.printf("connecting to%s.." , ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("connected ");
  
  timeClient.begin();
  
  pinMode(D1, OUTPUT); //trig
  pinMode(D0, INPUT);  //echo
  pinMode(D2, OUTPUT); //BUZZER
}

void loop() {
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  Serial.println(formattedTime);
  delay(1000);

  Serial.print("connecting to ");
  Serial.println(host);
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  if (WiFi.status() == WL_CONNECTED) {
    if (formattedTime >= "20:00:00" && formattedTime <= "20:00:10" ) {
      String url = "/trigger/send_email/with/key/";
      url += apiKey;
      Serial.print("Requesting URL: ");
      Serial.println(url);
      String IFTTT_POST_DATA = String("{\"value1\":") + "\"" + "Take" + "\"" + "," +
                               String("\"value2\":")  + "\"" + "Your" + "\"" + "," +
                               String("\"value3\":")  + "\"" + "Medicine!" + "\"" + "}" ;
      String IFTTT_POST_DATA_SIZE = String(IFTTT_POST_DATA.length());
      client.print(String("POST ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n" +
                   "Content-Type: application/json\r\n" + "Content-Length: " + IFTTT_POST_DATA_SIZE + "\r\n\r\n" + IFTTT_POST_DATA + "\r\n");

    }
  }
  //another mail to his son if his father miss to take the pill by ultrasonic
  //to turn off buzzer when he take the pill and open the box
  digitalWrite(D1, LOW);
  delayMicroseconds(5);
  digitalWrite(D1, HIGH);
  delayMicroseconds(5);
  digitalWrite(D1, LOW);
  duration = pulseIn(D0, HIGH);
  dis  = duration / 29 / 2;
  Serial.println(dis);
  if (dis < 45 && formattedTime >= "20:02:10" && formattedTime <= "20:04:10") {
    Serial.println("closed");
    digitalWrite(D2, HIGH);
    if (WiFi.status() == WL_CONNECTED) {
      if (formattedTime >= "20:03:30" && formattedTime <= "20:03:40" ) {
        String url = "/trigger/medicine/with/key/";
        url += apiKey;
        Serial.print("Requesting URL: ");
        Serial.println(url);
        String IFTTT_POST_DATA = String("{\"value1\":") + "\"" + "Ask " + "\"" + "," +
                                 String("\"value2\":")  + "\"" + "About " + "\"" + "," +
                                 String("\"value3\":")  + "\"" + " Medicine!" + "\"" + "}" ;
        String IFTTT_POST_DATA_SIZE = String(IFTTT_POST_DATA.length());
        client.print(String("POST ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n" +
                     "Content-Type: application/json\r\n" + "Content-Length: " + IFTTT_POST_DATA_SIZE + "\r\n\r\n" + IFTTT_POST_DATA + "\r\n");

      }
    }
  }
  else if (dis > 45) {
    Serial.println("opened");
    digitalWrite(D2, LOW);
  }
}
