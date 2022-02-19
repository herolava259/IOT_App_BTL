#include<ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include <DHT.h>


#define DHTTYPE DHT11
/*FPT Telecom-4540 2,4g
  12345678*/
#define WIFI_SSID "Galaxyj7pro"
#define WIFI_PASSWORD "oskr0675"
#define API_KEY "AIzaSyCr-DZn-PpQFXc1d2B0v8hPgbn6ip4Vjyc"
#define FIREBASE_PROJECT_ID "smart-home-335009"
#define USER_EMAIL "chopperman259@gmail.com"
#define USER_PASSWORD "12345678"

FirebaseData fbdoRead;
FirebaseData fbdoWrite;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long dataMillis = 0;
int count = 0;
const int DHTPin = 5;
//duong dan 
String const documentPathHumi = "device/LbYGhsxZ7MLZ5xAeJw0c";
String const documentPathTemp = "device/aRhJRdpt7KPE52Q4SpE3";
String const documentPathLed = "device/knpERCccRGTgyyBRna96";

//trang thai
String const mask = "status";
DHT dht(DHTPin,DHTTYPE);


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  dht.begin();
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while(WiFi.status() != WL_CONNECTED)
  {
    
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n",FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.token_status_callback = tokenStatusCallback;

  fbdoRead.setBSSLBufferSize(1024,4096);
  fbdoWrite.setBSSLBufferSize(1024,4096);
  Firebase.begin(&config,&auth);

  Firebase.reconnectWiFi(true);
}

void loop() {
  if(Firebase.ready() && (millis() - dataMillis > 2000 || dataMillis == 0))
  {
    /* Doc trang thai bat tat den */
    dataMillis = millis();

    Serial.print("Read switch led...");
    Serial.println();
    if (Firebase.Firestore.getDocument(&fbdoRead, FIREBASE_PROJECT_ID, "", documentPathLed.c_str(), mask.c_str()))
            Serial.printf("ok\n%s\n\n", fbdoRead.payload().c_str());

            FirebaseJson json(fbdoRead.payload().c_str());
            FirebaseJsonData result;
            json.get(result, "fields/status/stringValue");

            if(result.type == "string"){
              Serial.println();
              Serial.println(result.to<String>().c_str());
              if(String(result.to<String>().c_str()) == "on"){
                digitalWrite(D2,HIGH); 
                Serial.print("ON LAMP");
                Serial.println();
              }else{
                digitalWrite(D2,LOW); 
                Serial.print("OFF LAMP");
                Serial.println();
              }
            }
        else
            Serial.println(fbdoRead.errorReason());

    /*Dua thong tin cam bien */
    Serial.print("Commit a document");
    std::vector<struct fb_esp_firestore_document_write_t> writes;

    struct fb_esp_firestore_document_write_t update_write_humi;
    struct fb_esp_firestore_document_write_t update_write_temp;

    
    
    update_write_humi.type = fb_esp_firestore_document_write_type_update;
    update_write_temp.type = fb_esp_firestore_document_write_type_update;
    

    FirebaseJson contentHumi;
    FirebaseJson contentTemp;
    

    double h = dht.readHumidity();
    double t = dht.readTemperature();
    double f = dht.readTemperature(true);
    

    if (isnan(h) || isnan(t) || isnan(f))
    {
      Serial.println('Khong the doc du lieu tu cam bien DHT');
      return;
    }
    
    
    contentHumi.set("fields/humidity/doubleValue", h);

    update_write_humi.update_document_content = contentHumi.raw();
    update_write_humi.update_document_path = documentPathHumi.c_str();
    update_write_humi.update_masks = "humidity";
    update_write_humi.current_document.exists = "true";
    update_write_humi.current_document.update_time = "2022-05-02T15:01:23Z";
    writes.push_back(update_write_humi);
    count++;

    contentTemp.set("fields/temperature/doubleValue", t);
    
    update_write_temp.update_document_content = contentTemp.raw();
    update_write_temp.update_document_path = documentPathTemp.c_str();
    update_write_temp.update_masks = "temperature";
    update_write_temp.current_document.exists = "true";
    update_write_temp.current_document.update_time = "2022-05-02T15:01:23Z";
    writes.push_back(update_write_temp);
    Serial.print("\nUpdate a document...");

    if(Firebase.Firestore.commitDocument(&fbdo, FIREBASE_PROJECT_ID,"",writes,"" ))
    {
      Serial.printf("ok\n%s\n\n", fbdoWrite.payload().c_str());
    }else{
      Serial.println(fbdoWrite.errorReason());
    }
    
    
  }

}
