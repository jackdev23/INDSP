/*******************************************************************************
Copyright 2018 Luca Facchinetti, IW2NDH  iw2ndh@gmail.com
 All trademarks referred to in source code and documentation 
 are copyright their respective owners.
 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
 
    This program is distributed WITHOUT ANY WARRANTY.
    If you offer a hardware kit using this software, 
    show your appreciation by sending the author
    a complimentary kit or a donation to a cats refuge ;-)
*****************************************************************************/

#include <Wire.h>
#include "INDSP_I2C.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    9  // you can also connect this to the Arduino reset
// in which case, set this #define pin to -1!
#define TFT_DC     8

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// For 1.54" TFT with ST7789
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
//#define TFT_SCLK 13   // set these to be whatever pins you like!
//#define TFT_MOSI 11   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


//INDSP example. 
//Connect: ARDUINO  INDSP
//            A4 = Rb6
//            A5 = Rb5  
//            5V = Vin
//           GND = GND
//Connect: 4 x 2k2 Potentiometers to A0, A1, A2, A3 (+5Volt and GND)
//Moving the Potentiometers will change the gain of the associated band

#define MASTER_GAIN 9     /* This must be an integer in the range 0 (default) to 12 (max volume) */

unsigned char  GAINS[8] =  {9, 9, 9, 9, 9, 9, 9, 9};

int oldVal[4];
int sensorValue[4];
unsigned char valdB[4];

INDSP_I2C dsp;

void setup() {
  Serial.begin(9600);
  Serial.print("Hello! INDSP test");
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setTextWrap(true); // Allow text to run off right edge
  tft.fillScreen(ST77XX_BLACK);

  dsp.init();
  dsp.setBandGain(DSP_EQU_31_BAND, 9);
  dsp.setBandGain(DSP_EQU_62_BAND, 9);
  dsp.setBandGain(DSP_EQU_125_BAND, 9);
  dsp.setBandGain(DSP_EQU_250_BAND, 9);
  dsp.setBandGain(DSP_EQU_500_BAND, 9);
  dsp.setBandGain(DSP_EQU_1000_BAND, 9);
  dsp.setBandGain(DSP_EQU_2000_BAND, 9);
  dsp.setBandGain(DSP_EQU_4000_BAND, 9);

  dsp.setMute(DSP_FALSE);
  dsp.setNoiseReduction(25);
  dsp.noiseApply(DSP_FALSE);
  dsp.setMasterGain(MASTER_GAIN);
  dsp.EQUapply(DSP_TRUE);
  dsp.setAutoNotch(DSP_FALSE);
  dsp.selectFilter(DSP_FIL_450_950);
  dsp.filterApply(DSP_FALSE);

  tft.setRotation(3);
  tft.setCursor(5, 25);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.println("INDSP Test");
  tft.setCursor(10, 45);
  tft.println("by IW2NDH");
  tft.setCursor(10, 75);
  tft.println("NOISE");
}
void testON() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(5, 25);
  tft.println("INDSP Test");
  tft.setCursor(10, 50);
  tft.println("by IW2NDH");
  tft.setCursor(10, 75);
  tft.println("NOISE  ON");
}
void testOFF() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(5, 5);
  tft.println("INDSP Test");
  tft.setCursor(10, 30);
  tft.println("by IW2NDH");
  tft.setCursor(10, 75);
  tft.println("NOISE  OFF");
}
void writeVal() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(5, 5);
  tft.println("INDSP Test");
  tft.setCursor(10, 30);
  tft.println("by IW2NDH");
  tft.setCursor(90, 55);
  for (int i = 0; i < 4; i++) {
    tft.setCursor(75, 55 + 15 * i);
    tft.print("dB ");
    tft.println(-(oldVal[i] - 9));
  }
  tft.setCursor(0, 55);
  tft.println("250");
  tft.println("500");
  tft.println("1000");
  tft.println("2000");
}

void loop() {
  sensorValue[0] = analogRead(A0);
  sensorValue[1] = analogRead(A1);
  sensorValue[2] = analogRead(A2);
  sensorValue[3] = analogRead(A3);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):

  valdB[0] = (unsigned char) (sensorValue[0] / 56);
  valdB[1] = (unsigned char) (sensorValue[1] / 56);
  valdB[2] = (unsigned char) (sensorValue[2] / 56);
  valdB[3] = (unsigned char) (sensorValue[3] / 56);
  if (oldVal[0] != valdB[0]) {
    dsp.setBandGain(DSP_EQU_250_BAND, valdB[0]);
    oldVal[0] = valdB[0];
    writeVal();
  }
  if (oldVal[1] != valdB[1]) {
    dsp.setBandGain(DSP_EQU_500_BAND, valdB[1]);
    oldVal[1] = valdB[1];
    writeVal();
  }
  if (oldVal[2] != valdB[2]) {
    dsp.setBandGain(DSP_EQU_1000_BAND, valdB[2]);
    oldVal[2] = valdB[2];
    writeVal();
  }
  if (oldVal[3] != valdB[3]) {
    dsp.setBandGain(DSP_EQU_2000_BAND, valdB[3]);
    oldVal[3] = valdB[3];
    writeVal();
  }
}
