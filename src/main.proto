//Press ButtonA to send "Hello"

#include <M5Stack.h>
#include "TFTTerminal.h"

TFT_eSprite Disbuff = TFT_eSprite(&M5.Lcd);
TFT_eSprite TerminalBuff = TFT_eSprite(&M5.Lcd);
TFTTerminal terminal(&TerminalBuff);

bool ReceiveAT(uint32_t timeout)
{
  uint32_t nowtime = millis();
  while(millis() - nowtime < timeout){
    if (Serial2.available() !=0) {
      String str = Serial2.readString();
      // if (str.indexOf("+OK") != -1 || str.indexOf("+ERROR") != -1) {
      //   terminal.println(str);
      //   //Serial.println(str);
      //   return true;
      // }else {
      //   terminal.println("ERROR");
      //   //Serial.println("Syntax Error");
      //   break;
      // }
      if (str.indexOf("+OK") != -1) terminal.println("OK");
      else terminal.println("ERROR");
      return true;
    }
  }
  terminal.println("TIMEOUT");
  //Serial.println("Timeout");
  return false;
}

void ATCommand(char cmd[],char date[], uint32_t timeout = 300)
{
  char buf[256] = {0};
  if(date == NULL)
  {
    sprintf(buf,"AT+%s",cmd);
  }
  else 
  {
    sprintf(buf,"AT+%s=%s",cmd,date); 
  }
  Serial2.write(buf);
  terminal.print(buf);
  terminal.print(" : ");
  ReceiveAT(timeout);
}

void setup() 
{
  M5.begin();
  M5.Power.begin();
  
  Serial2.begin(115200, SERIAL_8N1, 16, 17);

  TerminalBuff.createSprite(250,220);
  TerminalBuff.fillRect(0,0,250,220,BLACK);
  TerminalBuff.setTextColor(GREEN);
  TerminalBuff.pushSprite(0,0);
  terminal.setFontsize(2);
  terminal.setGeometry(0,0,250,220);
  ATCommand("AutoLPM", "0");
  ATCommand("LORAWAN", "0");
  ATCommand("LoraSet", "868000000,18,5,2,1,8,1,0,0");
  ATCommand("PrintMode", "0");
  ATCommand("RX", "0");
}

unsigned long lastKick = 0;
bool toggleWhite = false;

void loop() 
{
  M5.update();
  if(M5.BtnA.wasPressed()) {
    ATCommand("SendStr", "Hello");    
    ATCommand("RX", "0");
  }

  if(M5.BtnC.wasPressed()) {
    lastKick = 1;
  }
  
 if(Serial2.available() > 0) {
   String receive_data = Serial2.readString();
   //  Serial.println(receive_data);
   //terminal.println(receive_data);
   ATCommand("RX", "0");
   if (receive_data.indexOf("KICK") != -1) 
   {
      if (toggleWhite) M5.Lcd.fillScreen(WHITE);
      else M5.Lcd.fillScreen(BLACK);
      toggleWhite = !toggleWhite;
      // terminal.println("KICK-in");
   }
   else terminal.println(receive_data);
 } 

  if (lastKick > 0 && millis()-lastKick>10) {
    lastKick = millis();
    ATCommand("SendStr", "KICK");
    ATCommand("RX", "0");
    if (toggleWhite) M5.Lcd.fillScreen(WHITE);
    else M5.Lcd.fillScreen(BLACK);
    toggleWhite = !toggleWhite;
    // terminal.println("KICK-out");
  } 

}