#include <HardwareSerial.h>

#include "sys/time.h"

#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEBeacon.h"
#include "esp_sleep.h"

//BLE
#define GPIO_DEEP_SLEEP_DURATION     10  // sleep x seconds and then wake up
RTC_DATA_ATTR static time_t last;        // remember last boot in RTC Memory
RTC_DATA_ATTR static uint32_t bootcount; // remember number of boots in RTC Memory

#ifdef __cplusplus
extern "C" {
#endif

uint8_t temprature_sens_read();
//uint8_t g_phyFuns;

#ifdef __cplusplus
}
#endif

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
BLEAdvertising *pAdvertising;
struct timeval now;

#define BEACON_UUID           "74d6945f-42e8-403b-855e-32a37e86a7d5" // UUID 1 128-Bit (may use linux tool uuidgen or random numbers via https://www.uuidgenerator.net/)
//74d6945f-42e8-403b-855e-32a37e86a7d5 D5A
//3f6a05ab-753b-423e-9d09-c91be50f209e
//CB10023F-A318-3394-4199-A8730C7C1AEC EC1A
//BLE1

int getSensorPin = A0;
int vinSensorPin = 13;
int ButtonPin = 34;
int ButtonVal;
int val;
int LED = 2;
int flag = 0; //whether into button mode

void setBeacon() {

  BLEBeacon oBeacon = BLEBeacon();
  oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
  //HardwareSerial::println(BLEUUID(BEACON_UUID));
  oBeacon.setProximityUUID(BLEUUID(BEACON_UUID));
  oBeacon.setMajor(2);
  oBeacon.setMinor(2);
  //oBeacon.setMajor((bootcount & 0xFFFF0000) >> 16);
  //oBeacon.setMinor(bootcount&0xFFFF);
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
  
  oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04
  
  std::string strServiceData = "";
  
  strServiceData += (char)26;     // Len
  strServiceData += (char)0xFF;   // Type
  strServiceData += oBeacon.getData(); 
  oAdvertisementData.addData(strServiceData);
  
  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);

}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);
  pinMode(getSensorPin, INPUT);
  pinMode(vinSensorPin, OUTPUT);
  pinMode(ButtonPin,INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(vinSensorPin, HIGH);

}
void loop() {
  
  val = analogRead(getSensorPin);
  if(val > 2000 || flag==1) {
    flag = 1;
    ButtonVal = digitalRead(ButtonPin);
    if(ButtonVal==1) {
      flag = 0;
    }
    Serial2.println("[{\"id\":\"test01\",\"value\":[\"1.0\"]}]");
    Serial.print("ButtonVal is ");
    Serial.println(ButtonVal);
    //Serial2.println("[{\"id\":\"Region201\",\"value\":[\"1.0\"]}]");
    //Serial2.println("{path: {\"deviceId\":24790262077},data: [{ \"id\":\"sensor01\",\"value\":[\"1.0\"] }],headers: {\"CK\":\"PKST1CTZ41PWF7CGW1\"}}");
    digitalWrite(LED, HIGH);
    delay(1000);  //延遲 1秒
    //BLE
    gettimeofday(&now, NULL);

    Serial.printf("start ESP32 %d\n",bootcount++);
  
    Serial.printf("deep sleep (%lds since last reset, %lds since last boot)\n",now.tv_sec,now.tv_sec-last);
  
    last = now.tv_sec;
    
    // Create the BLE Device
    BLEDevice::init("ESP3232");
  
    // Create the BLE Server
     BLEServer *pServer = BLEDevice::createServer(); // <-- no longer required to instantiate BLEServer, less flash and ram usage
  
    pAdvertising = BLEDevice::getAdvertising();
    BLEDevice::startAdvertising();
    
    setBeacon();
    
    // Start advertising
    pAdvertising->start();
    Serial.println("Advertizing started...");
    pAdvertising->stop();
    Serial.printf("enter deep sleep\n");
    //esp_deep_sleep(1000000LL * GPIO_DEEP_SLEEP_DURATION);
   // Serial.printf("in deep sleep\n");
    //BLE
  }else{
   Serial2.println("[{\"id\":\"test01\",\"value\":[\"0.0\"]}]");
   //Serial2.println("[{\"id\":\"Region201\",\"value\":[\"0.0\"]}]");
   //Serial2.println("{path: {\"deviceId\":24790262077},data: [{ \"id\":\"sensor01\",\"value\":[\"0.0\"] }],headers: {\"CK\":\"PKST1CTZ41PWF7CGW1\"}}");
    digitalWrite(LED, LOW);
    delay(1000);  //延遲1秒
  }
  if(Serial2.read()=='Y') {
   // led_blink();
   // led_blink();
    Serial.println(val);  //左方板向PC傳送字串
  }
  Serial.println(val);

}


void led_blink() {
  digitalWrite(LED, HIGH);
  delay(20);
  digitalWrite(LED, LOW);
  delay(20);
  }
