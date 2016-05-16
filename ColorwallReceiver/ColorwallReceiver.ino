/********************************************
LED drive unit for couch LEDs
********************************************/

#include <Adafruit_NeoPixel.h>
#define PIN 4

int numPix = 144;
uint32_t warmWhite;
uint32_t fullColor;
uint32_t data = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPix, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial1.begin(9600);
  strip.begin();
  strip.show();

  warmWhite = strip.Color ( 255, 110, 10 );
  fullColor = warmWhite;
  data = fullColor;

  strip.setBrightness(0);
  fullString();
  for (int i = 0; i < 256; i++){
    strip.setBrightness(i);
    strip.show();
    delay(5);
  }
}

void loop() {

  while (Serial1.available() > 0){
    data = Serial1.read();
    if (data != fullColor){
      fullColor = data;
      fullString();
    }
  }

  if (fullColor != warmWhite){
    fullColor = warmWhite;
    lineString(20);
  }

}


void fullString() {
  for (int p = 0; p < numPix; p++) {
      strip.setPixelColor(p, fullColor);
    }
    strip.show();
}

void lineString(int del) {
  for (int p = 0; p < numPix; p++) {
      strip.setPixelColor(p, fullColor);
      strip.show();
      delay(del);
  }
}

