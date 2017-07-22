#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "your_firebase_host"				// insert your_firebase_host
#define FIREBASE_AUTH "your_firebase_auth"				// insert your_firebase_auth
#define WIFI_SSID "your_ssid"							// insert your_ssid
#define WIFI_PASSWORD "your_password"					// insert your_password

const byte button = 16;
WiFiClient client;
void setup() {
  pinMode(button, INPUT);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

int i = 0;
int buttonState = 0; 
void loop() {
  buttonState = digitalRead(button);
  if (buttonState == HIGH)
  {
    Serial.println(getTime());
    sendDataToFirebase();
    String temp = getTime();
    Firebase.pushString(temp, temp);
    if (Firebase.failed()) {
      Serial.print("set /data failed:");
      Serial.println(Firebase.error());
      return;
    }
    Serial.print("set /data to ");
    Serial.println(Firebase.getInt("data"));

    i++;
    delay(10000);
  }


}

String getTime() {
  WiFiClient client;
  while (!!!client.connect("google.com", 80)) {
    Serial.println("connection failed, retrying...");
  }

  client.print("HEAD / HTTP/1.1\r\n\r\n");

  while (!!!client.available()) {
    yield();
  }

  while (client.available()) {
    if (client.read() == '\n') {
      if (client.read() == 'D') {
        if (client.read() == 'a') {
          if (client.read() == 't') {
            if (client.read() == 'e') {
              if (client.read() == ':') {
                client.read();
                String theDate = client.readStringUntil('\r');
                client.stop();
                return theDate;
              }
            }
          }
        }
      }
    }
  }
}

void sendDataToFirebase() {
  String data = "{" ;
  data = data + "\"to\": \"your_id_android_refreshedToken\"," ;		// insert your_id_android_refreshedToken
  data = data + "\"notification\": {" ;
  data = data + "\"body\": \"detected\"," ;
  data = data + "\"sound\" : \"ON\" " ;
  data = data + "\"title\" : \"Notification\" " ;
  data = data + "} }" ;

  Serial.println("Send data...");
  if (client.connect("https://fcm.googleapis.com", 80)) {
    Serial.println("Connected to the server..");
    client.println("POST /fcm/send HTTP/1.1");
    client.println("Authorization: key= your_key");					// insert your_key Authorization
    client.println("Content-Type: application/json");
    client.println("Host: fcm.googleapis.com");
    client.print("Content-Length: ");
    client.println(data.length());
    client.print("\n");
    client.print(data);
  }
  Serial.println("Data sent...Reading response..");
  while (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  Serial.println("Finished!");
  client.flush();
  client.stop();
}
