#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLqOapkNm2"
#define BLYNK_DEVICE_NAME "uvois try"

#include "EmonLib.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "time.h"
 
EnergyMonitor emon;
#define vCalibration 36.50
#define currCalibration 0.03
BlynkTimer timer;

#define relayPin 13

char auth[] = "l4C4s0n3ZdTW1ZIINi5ZQKu9c0FjI75i";
//char ssid[] = "Galaxy Note10b3ec";
//char pass[] = "ecfr7874";
char ssid[] = "iPhone";
char pass[] = "12344321";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 28800;
const int   daylightOffset_sec = 0;
int x=0;

float kWh = 0;
unsigned long lastmillis = millis();
 
void myTimerEvent() {
    emon.calcVI(20, 2000);
    Serial.print("Vrms: ");
    Serial.print(emon.Vrms, 2);
    Serial.print("V");
    Blynk.virtualWrite(V0, emon.Vrms);
    Serial.print("\tIrms: ");
    Serial.print(emon.Irms, 4);
    Serial.print("A");
    Blynk.virtualWrite(V1, emon.Irms);
    Serial.print("\tPower: ");
    Serial.print(emon.apparentPower, 4);
    Serial.print("W");
    Blynk.virtualWrite(V2, emon.apparentPower);
    Serial.print("\tWh: ");
    kWh = kWh + emon.apparentPower*(millis()-lastmillis)/3600000.0;
    Serial.print(kWh, 4);
    Serial.println("Wh");
    lastmillis = millis();
    Blynk.virtualWrite(V3, kWh);
}
 
void setup() {
  Serial.begin(9600);
  pinMode(relayPin,OUTPUT);
  emon.voltage(35, vCalibration, 1.7); // Voltage: input pin, calibration, phase_shift
  emon.current(34, currCalibration); // Current: input pin, calibration.
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(5000L, myTimerEvent);
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

}
 
void loop() {
  Blynk.run();
  timer.run();
  printLocalTime();
  if (emon.Irms > 5.00 ){   
    Blynk.logEvent("overcurrent","Abnormal current detected");
  }
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  char timeMinute[3];
  strftime(timeMinute,3, "%I", &timeinfo);
  char timeSecond[3];
  strftime(timeSecond,3, "%S", &timeinfo);
  if (strncmp(timeHour,"20",2)==0 && x<1){
    Blynk.logEvent("timer","Your system is OFF");
    //Serial.println("Your system is OFF");
    x+=1;
  }
  if (strncmp(timeHour,"20",2)==0 && emon.Vrms > 0){
    Blynk.logEvent("voltage","Your system is still ON. Are you the one using it?");
  }
}
