
#include <M5Stack.h>
#include "LoRa.h"


void setup() {
  M5.begin(); 
  M5.Power.begin();
  
  
  M5.Lcd.setTextColor(YELLOW);  
  M5.Lcd.setTextSize(2); 
  M5.Lcd.setCursor(0, 10); 
  M5.Lcd.println("Audio5\n"); 
  
  M5.Lcd.print("LoRa init ... "); 
  LoRa_init();
  M5.Lcd.println("OK\n"); 
}


void loop() 
{
  M5.update();
  if (M5.BtnA.wasPressed()) 
  {
    M5.Lcd.setTextColor(RED);
    M5.Lcd.print("- ");
    
    // Send P
    uint32_t time = LoRa_send("P");
    if ( time > 0 ) {
      M5.Lcd.printf("P (%d)\n", time);
      //M5.Speaker.tone(440, 200);
    }
    else M5.Lcd.println("Error");
  } 

  if (M5.BtnB.wasPressed()) 
  {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 10); 
  } 

  if (LoRa_available())
  {
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.print("= ");

    // Recv
    String msg = LoRa_read();
    M5.Lcd.println(msg);
    M5.Speaker.tone(440, 200);
  }
  
}
