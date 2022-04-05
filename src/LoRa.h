#include <Arduino.h>

typedef struct ATCMD {   // Declare PERSON struct type
    char cmd[10];   // Declare member types
    char data[10];
} ATCMD;

QueueHandle_t cmd_queue;
SemaphoreHandle_t cmd_lock;

bool AT_cmd(ATCMD c)
{
    // SEND
    char buf[256] = {0};
    if(c.data == NULL) 
    sprintf(buf,"AT+%s",c.cmd);
    else 
    sprintf(buf,"AT+%s=%s", c.cmd, c.data); 

    xSemaphoreTake(cmd_lock, portMAX_DELAY);

    Serial2.write(buf);

    Serial.print("=");
    Serial.println(buf);
  
    // CONFIRM
    uint32_t nowtime = millis();
    while(millis() - nowtime < 300)
        if (Serial2.available() !=0) 
        {
            String str = Serial2.readString();
            Serial.println(str);
            xSemaphoreGive(cmd_lock);
            return str.indexOf("+OK") != -1;
        }
    Serial.println("Timeout");
    xSemaphoreGive(cmd_lock);
    return false;
}

void AT_cmd_async(ATCMD c)
{
    xQueueSend(cmd_queue, &c, portMAX_DELAY);
}

void AT_consume( void * pvParameters )
{
    ATCMD c;
    while(true)
        if ( xQueueReceive(cmd_queue, &c, portMAX_DELAY) == pdTRUE) {
            AT_cmd(c.cmd, c.data);
        }
}




void LoRa_init(int rxPin=16, int txPin=17)
{   
    cmd_lock = xSemaphoreCreateMutex();
    cmd_queue = xQueueCreate( 10, sizeof( ATCMD ) );
    xTaskCreate( AT_consume, "async_cmd", 2000, NULL, 0, NULL );

    Serial2.begin(115200, SERIAL_8N1, rxPin, txPin);

    AT_cmd("AutoLPM", "0");
    AT_cmd("LORAWAN", "0");
    AT_cmd("LoraSet", "868000000,18,5,2,1,8,1,0,0");
    AT_cmd("PrintMode", "0");
    // AT_cmd("ILOGLVL", "0");
    AT_cmd("RX", "0");

}

bool LoRa_send(String str)
{
    bool ret = AT_cmd("SendStr", str.c_str());    
    // AT_cmd("RX", "0");
    return ret;
}

bool LoRa_available() 
{
    return Serial2.available() > 0;
}

String LoRa_read()
{
    String receive_data = "";
    if (Serial2.available()) {
        receive_data = Serial2.readString();
        // AT_cmd("RX", "0");
    }
    return receive_data;
}
