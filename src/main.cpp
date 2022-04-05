
#include <M5Stack.h>
#include "LoRa.h"


void setup() {
  M5.begin(); 
  M5.Power.begin();
  
  LoRa_init();
  
  M5.Lcd.setTextColor(YELLOW);  
  M5.Lcd.setTextSize(2); 
  M5.Lcd.setCursor(65, 10); 
  M5.Lcd.println("LoRa TEST"); 
  
}


void loop() 
{
  M5.update();
  if (M5.BtnA.wasPressed()) 
  {
    M5.Lcd.setTextColor(RED);
    M5.Lcd.print("- ");
    
    // Send P
    if ( LoRa_send("P") ) M5.Lcd.println("P");
    else M5.Lcd.println("Error");
  } 

  if (LoRa_available())
  {
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.print("= ");

    // Recv
    String msg = LoRa_read();
    M5.Lcd.println(msg);
  }
  
}
