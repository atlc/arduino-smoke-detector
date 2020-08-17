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

  // Warmup time should be ~10 minutes (600s) in production, but is 5 seconds for debug purposes
  timeout(5, "Warming up");
}

void loop(){

  LPG = mq2.readLPG();
  CO = mq2.readCO(); 
  Smoke = mq2.readSmoke();

  alert(LPG, CO, Smoke);
  check_for_snooze();

  sensor_output = String("[ Gas ]  [ PPM ]\nCO:      " + String(CO) + " ppm\nSmoke:   " + String(Smoke) + " ppm\nLPG:     " + String(LPG) + " ppm");
  
  screen.clearDisplay();
  screen.setCursor(0,0);
  screen.println(sensor_output);
  screen.display();
  delay(150);
}

void alert(int LPG, int CO, int SMOKE) {
  if (LPG >= LPG_THRESHHOLD || CO >= CO_THRESHHOLD || SMOKE >= SMOKE_THRESHHOLD) {
    Serial.println("BEEEP BEEEP BEEEP BEEEEEEEPPPP");
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

// Alternate Wail (Chicago Siren) frequency map
// Plotting each interval as a range, where each range is an array of a start time & frequency pair, and a stop time & frequency pair.
//    [(ms_0, hZ_0), (ms_1, hZ_1)]
/*  
 *   {
 *    [(000, 510), (438, 610)],
 *    [(438, 720), (1091, 1020)],
 *    [(1091, 900), (1738, 850)],
 *    [(1738, 1000), (2390, 900)],
 *    [(2390, 750), (3040, 700)],
 *    [(3040, 830), (3690, 780)],
 *    [(3690, 650), (4342, 600)],
 *    [(4342, 720), (5000, 670)],
 *    [(5000, 560), (5640, 510)],
 *    [(5640, 620), (6287, 570)],
 *    [(6287, 480), (6939, 450)],
 *   }
 * 
 */
