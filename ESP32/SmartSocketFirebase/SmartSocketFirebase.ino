
/*
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2020 mobizt
 * 
 * This example is for FirebaseESP32 Arduino library v 3.7.3 or later
 *
*/

//This example shows how to set stream event callback functions.
//Two events are available from Firebase's Real Time database HTTP stream connection, dataAvailable and streamTimeout.

// Example reworked by Hrvoje Dumancic for Faculty of Organisation and Information in Varazdin, Croatia

#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "Dumancic"
#define WIFI_PASSWORD "25081963"
#define FIREBASE_HOST "project1-565c4.firebaseio.com"
#define FIREBASE_AUTH "E3QgKJXva6aBU3podQdJpyyon1GpxFlOviXM544b"
#define SWITCH_PIN 22

//Define Firebase data object
FirebaseData firebaseData1;

unsigned long sendDataPrevMillis = 0;

String streamPathString = "/1";
String eventPathString = "/OnOff";
String switchControlKeyString = "on";
String eventPathSwitch = eventPathString + "/" + switchControlKeyString;

uint16_t count = 0;

void printResult(StreamData &data);

void streamCallback(StreamData data)
{
  if (data.dataType() == "boolean") {
      controlBooleanData(data);// BRANCH BOOLEAN
  } else if (data.dataType() == "json") {
      controlJsonData(data);
      // BRANCH JSON
  } else {
    Serial.print("Unsupported data format: ");
    Serial.println(data.dataType());
  }
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    Serial.println();
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();
  }
}

void setup()
{

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  pinMode(SWITCH_PIN, OUTPUT);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  if (!Firebase.beginStream(firebaseData1, streamPathString))
  {
    Serial.println("------------------------------------");
    Serial.println("Can't begin stream connection...");
    Serial.println("REASON: " + firebaseData1.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  Firebase.setStreamCallback(firebaseData1, streamCallback, streamTimeoutCallback);
}

void loop()
{
}

void printOutUnknownKey(String key) {
    Serial.print("Key: ");
    Serial.println(key);  
}

void controlBooleanData(StreamData &data) {
  if (data.streamPath() == streamPathString) {
    if (data.dataPath() == eventPathSwitch) {
      bool boolValue = data.boolData() == 1 ? true : false;
      controlSwitch(boolValue);
    }
  }
}

void controlJsonData(StreamData &data) {
  if (data.streamPath() == streamPathString) {
    if (data.dataPath() == eventPathString) {
      decodeSwitchControl(data);
    }
  } else {
    Serial.println("Unknown command");
  }
}

void decodeSwitchControl(StreamData &data) {  
  FirebaseJson *json = data.jsonObjectPtr();
  size_t len = json->iteratorBegin();
  
  String key, value = "";
  int type = 0;
  for (size_t i = 0; i < len; i++)
  {
    json->iteratorGet(i, type, key, value);
    
    if (key == switchControlKeyString) {
        bool boolValue = checkBooleanFirebaseValue(value);
        controlSwitch(boolValue);
    } else {
        printOutUnknownKey(key);
    }
  }
  json->iteratorEnd();
}

void controlSwitch(bool value) {
  if (value) {
      digitalWrite(SWITCH_PIN, HIGH);
  } else {
      digitalWrite(SWITCH_PIN, LOW);
  }
}

boolean checkBooleanFirebaseValue(String value) {
    if (value == "true") {
      return true;
    }
    return false;
}
