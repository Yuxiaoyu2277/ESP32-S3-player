/*

  Unicode.ino

  Demonstrates how to connect U8g2 fonts to TFT_eSPI library.

  U8g2_for_TFT_eSPI:
    - Use U8g2 fonts with TFT_eSPI
    - Supports UTF-8 in print statement
    - 90, 180 and 270 degree text direction
  
  List of all U8g2 fonts:    https://github.com/olikraus/u8g2/wiki/fntlistall

  TFT_eSPI library:          https://github.com/Bodmer/TFT_eSPI
  U8g2_for_TFT_eSPI library: https://github.com/Bodmer/U8g2_for_TFT_eSPI

*/
#include <SPI.h>
#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI(135, 240);
U8g2_for_TFT_eSPI u8f;       // U8g2 font instance

void setup() {
  tft.begin();
  u8f.begin(tft);                     // connect u8g2 procedures to TFT_eSPI
}

void loop() {  
  tft.fillScreen(TFT_BLACK);          // clear the graphics buffer  
  u8f.setForegroundColor(TFT_WHITE);  // apply color
  u8f.setFont(all_font);     // extended font
  u8f.setFontDirection(1);     // extended font
  u8f.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8f.setCursor(100,0);                // start writing at this position
  u8f.print("大橙子到此一游QWQ");                 // UTF-8 string: "<" 550 448 664 ">"

  while(1) delay(2000);
} 

