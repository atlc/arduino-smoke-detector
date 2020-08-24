// Current schematic:
//https://www.circuito.io/app?components=97,512,11021,398782,985157

#include <MQ2.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SENSOR_PIN A0
int LPG, CO, Smoke;
String sensor_output;
MQ2 mq2(SENSOR_PIN);

#define OLED_WIDTH 128
#define OLED_HEIGHT 32
Adafruit_SSD1306 screen(OLED_WIDTH, OLED_HEIGHT, &Wire);

#define SILENCE_PIN 2
int is_silenced = 0;

#define CO_THRESHHOLD 10
#define SMOKE_THRESHHOLD 1000
#define LPG_THRESHHOLD 2000

#define PIEZO_PIN 8
// Rough approximation of the average frequency of the first few notes at the beginning or end of an interval in the "Alternating Wail", also known as the Chicago Siren
int frequencies[] = { 510, 610, 720, 1020, 900, 850, 1000, 900, 750, 700, 830, 780, 650, 600, 720, 670, 560, 510, 620, 570, 480, 450 };

void setup(){
  Serial.begin(9600);
  mq2.begin();
  pinMode(SILENCE_PIN, INPUT);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!screen.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // I2C Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  screen.setTextColor(WHITE);

  // Warmup time should be ~15 minutes (900s) in production, but is 5 seconds for debug purposes
  timeout(3, "Srch Ghsts");
}

void loop(){

//  LPG = mq2.readLPG();
//  CO = mq2.readCO(); 
//  Smoke = mq2.readSmoke();

  LPG = analogRead(0);
  CO = analogRead(1);
  Smoke = analogRead(3);

//  alert(LPG, CO, Smoke);
  check_for_snooze();

  sensor_output = String("[ Sensor ] [ Trace ]\nEctoplasm: " + String(CO) + " ppm\nLIDAR:     " + String(Smoke) + " ppm\nEVP:       " + String(LPG) + " ppm");
  
  screen.clearDisplay();
  screen.setCursor(0,0);
  screen.println(sensor_output);
  screen.display();
  delay(150);
}

void alert(int LPG, int CO, int SMOKE) {
  if (LPG >= LPG_THRESHHOLD || CO >= CO_THRESHHOLD || SMOKE >= SMOKE_THRESHHOLD) {
//    Serial.println("BEEEP BEEEP BEEEP BEEEEEEEPPPP");
    for (int i=0; i < sizeof(frequencies); i++) {
      tone(PIEZO_PIN, frequencies[i], 350);
      delay(500);
      check_for_snooze();
      noTone(PIEZO_PIN);
    }
  }
}

void check_for_snooze(){
  is_silenced = digitalRead(SILENCE_PIN);

  if (is_silenced == 1) {
    timeout(180, "Asleep for");
  }
  screen.setTextSize(1);
}

void timeout(int duration, String message) {
  for (int i=duration;i>0;i--) {
      delay(1000);
      screen.clearDisplay();
      screen.setCursor(0,0);
      screen.setTextSize(2);
      screen.println(String(message + "\n   T-" + String(i)));
      screen.display();
    }
}
