/********************************************
Control unit for couch LEDs
Pins assigned for Teensy 3.x board

pin 0 = RX on XBee
pin 1 = TX on XBee
pin 2 = NeoPixel data (single pixel)
pin 14 = button, red
pin 15 = button, green
pin 16 = button, blue
pin 17 = button, white
pin 18 = knob, red (potentiometer)
pin 19 = knob, green
pin 20 = knob, blue
********************************************/

#include <Adafruit_NeoPixel.h>
#define PIN 2

uint32_t ledOn = 10000; // time that local led stays on after last activity
uint16_t highPot = 1023; // lower from 1023 until max scaled result is 255
uint8_t whiteAdd = 125; // amount to boost color if combined with white button

uint8_t button[] = {14, 15, 16, 17};
uint8_t pot[] = {18, 19, 20};
uint16_t lastPot[3];
uint8_t bPixel[4];
bool lastButton[] = {LOW, LOW, LOW, LOW};
bool currentButton[] = {LOW, LOW, LOW, LOW};

uint32_t currentMillis = 0;
uint32_t prevMillis = 0;
bool trigger = false;
uint32_t warmWhite;
uint32_t fullColor;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  currentMillis = millis();
  prevMillis = currentMillis;

// unused pins on Teensy set to OUTPUT to save power
  for (int i=3; i < 14; i++) {
    pinMode(i, OUTPUT);
  }
  for (int i=21; i < 24; i++) {
    pinMode(i, OUTPUT);
  }

  for (int p = 0; p < 3; p++){
    lastPot[p] = analogRead(pot[p]);
  }
  warmWhite = strip.Color(255, 110, 10);
  fullColor = warmWhite;

  Serial1.begin(9600);
  Serial.begin(9600); //for testing
  strip.begin();
  strip.show();
}

void loop() {
  currentMillis = millis();

// check knobs, set color if any changed
  for (int p = 0; p < 3; p++){
    if (analogRead(pot[p]) != lastPot[p]) {
      int potZero = map(analogRead(pot[0]), 0, highPot, 0, 255);
      int potOne = map(analogRead(pot[1]), 0, highPot, 0, 255);
      int potTwo = map(analogRead(pot[2]), 0, highPot, 0, 255);
      potZero = constrain(potZero, 0, 255);
      potOne = constrain(potOne, 0, 255);
      potTwo = constrain(potTwo, 0, 255);
      fullColor = strip.Color(potZero, potOne, potTwo);
      setPx();
      lastPot[p] = analogRead(pot[p]);
    }
  }

// check buttons
  for (int b = 0; b < 4; b++){
    currentButton[b] = debounce(lastButton[b], button[b]);
    if (lastButton[b] == LOW && currentButton[b] == HIGH) {
      trigger = true;
    }
    lastButton[b] = currentButton[b];
  }

// set color if any buttons were pressed
  if (trigger == true){
    bool bColor = false;
    for (int b = 0; b < 3; b++){
      if (digitalRead(button[b]) == HIGH){
        bPixel[b] = 255;
        bColor = true;
      }
      else {
        bPixel[b] = 0;
      }
    }

    if (digitalRead(button[3]) == HIGH && bColor == true){
      for (int b = 0; b < 3; b++){
        bPixel[b] = constrain((bPixel[b] + whiteAdd), 0, 255);
      }
    }
    else if (digitalRead(button[3]) == HIGH && bColor == false){
      for (int b = 0; b < 3; b++){
        bPixel[b] = 255;
      }
    }

    fullColor = strip.Color(bPixel[0], bPixel[1], bPixel[2]);
    setPx;
    trigger = false;
  }

// fade out local LED if inactive
  if (currentMillis - prevMillis > ledOn) {
    for (int b=255; b>0; b--){
      strip.setBrightness(b);
      strip.show();
      delay(4);
    }
    strip.setPixelColor(0, 0, 0, 0);
    strip.setBrightness(255);
    strip.show();
  }
}


boolean debounce(boolean last, byte btn){
  boolean current = digitalRead(btn);
  if (last != current)
  {
    delay(5);
    current = digitalRead(btn);
  }
  return current;
}

void setPx(){
  strip.setPixelColor(0, fullColor);
  strip.show();
  Serial1.print(fullColor);
  Serial.println(fullColor); //testing
  prevMillis = currentMillis;
}

