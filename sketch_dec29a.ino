#include<ESP8266WiFi.h>
#include "FirebaseESP8266.h"
//#include<random.h>
#include<ArduinoJson.h>
#define WIFI_SSID "LE SON"
#define WIFI_PASSWORD "66709993"


#define FIREBASE_HOST "upbeat-repeater-324007-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "qL0spQz6r97N65enKtbKWGQ3gS06e3lTFZ7HsMqi"

FirebaseData firebaseData;
//WIFiClient client;
String path = "/";
FirebaseJson json;
int glob = 0;
void setup() 
{
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  if(!Firebase.beginStream(firebaseData, path)){
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();    
  }

  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:
  glob = (glob+9999)%23;
//  Serial.println("UPDATE:",glob);
  delay(250);
  Firebase.setInt(firebaseData, path + "/data" ,glob);
}
