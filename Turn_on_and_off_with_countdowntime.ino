
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>

char auth[] = "***********************";
char ssid[] = "*********";
char pass[] = "*********";


SimpleTimer timer;

int CountdownRemainReset;
int CountdownRemain;
int CountdownTimer;

void setup() {
  pinMode(5,OUTPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Blynk.begin(auth, ssid, pass);
  while (Blynk.connect() == false) {}
  ArduinoOTA.setHostname("Countdowner"); // OPTIONAL
  ArduinoOTA.begin();
  CountdownTimer = timer.setInterval(1000, CountdownTimerFunction);
  timer.disable(CountdownTimer); // disable it on boot
}

void CountdownTimerFunction() {
  CountdownRemain--; // remove 1 every second
  CountdownShowFormatted(CountdownRemain);
  if (!CountdownRemain) { // check if CountdownRemain == 0/FALSE/LOW
    timer.disable(CountdownTimer); // if 0 stop timer
   Blynk.virtualWrite(1,digitalRead(5)*255);//moi them
    Blynk.virtualWrite(2, LOW); // reset START/STOP button status// moi sua 1thanh 2
    Blynk.virtualWrite(0, "TIMER COMPLETE");
    Blynk.virtualWrite(6, 255); // LED for timer completed
    Blynk.virtualWrite(5, 0); // Timer LED status light off
  digitalWrite(5, LOW);//moi them
  } else {
    Blynk.virtualWrite(6, 0); // LED for timer completed
        
  }
}

// Button Widget (Switch Type): Start/Pause Timer
BLYNK_WRITE(1) {
  if (param.asInt()) {
    if (CountdownRemain) { // check if there is a time set or not
      timer.enable(CountdownTimer);
           Blynk.virtualWrite(5, 255); // Timer LED status light on
            digitalWrite(5, HIGH);//moi them
    } else {
      Blynk.virtualWrite(1, LOW); // if CountdownRemain is set to 0, then dont start hte timer.   
      Blynk.virtualWrite(0, "COUNTDOWN TIME NOT SET"); // if CountdownRemain is set to 0, then tell the user
   digitalWrite(5, LOW);//moi them
    }
  } else {
    timer.disable(CountdownTimer);
    Blynk.virtualWrite(5, 0); // Timer LED status light off
      
  }
  //digitalWrite(5, LOW);//moi them1
}

// Button Widget (Momentary): Reset Timer
BLYNK_WRITE(2) {
  CountdownRemain = CountdownRemainReset; // reset to original start time
}

// Slider Widget (60-180): Set Timer (mins)
BLYNK_WRITE(3) {
  if (timer.isEnabled(CountdownTimer)) { // only update if timer not running
    Blynk.virtualWrite(3, param.asInt() ); // if running, refuse to let use change slider
  } else {
    CountdownRemainReset = param.asInt() * 60 + 1; // + 1 set the timer to 1:00:00 instead of 00:59:59
    CountdownRemain = param.asInt() * 60;
    CountdownShowFormatted(CountdownRemain);
  }
}

void CountdownShowFormatted(int seconds) {
  long days = 0;
  long hours = 0;
  long mins = 0;
  long secs = 0;
  String secs_o = ":";
  String mins_o = ":";
  String hours_o = ":";
  secs = seconds; // set the seconds remaining
  mins = secs / 60; //convert seconds to minutes
  hours = mins / 60; //convert minutes to hours
  days = hours / 24; //convert hours to days
  secs = secs - (mins * 60); //subtract the coverted seconds to minutes in order to display 59 secs max
  mins = mins - (hours * 60); //subtract the coverted minutes to hours in order to display 59 minutes max
  hours = hours - (days * 24); //subtract the coverted hours to days in order to display 23 hours max
  if (secs < 10) {
    secs_o = ":0";
  }
  if (mins < 10) {
    mins_o = ":0";
  }
  if (hours < 10) {
    hours_o = ":0";
  }
  Blynk.virtualWrite(0, days + hours_o + hours + mins_o + mins + secs_o + secs);

}
void loop() {
  
  Blynk.run();
 ArduinoOTA.handle();
  timer.run();
}
