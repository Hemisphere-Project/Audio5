#include <Arduino.h>

typedef struct ATCMD {   
    char cmd[10]; 
    char data[128];
} ATCMD;

QueueHandle_t cmd_queue;
SemaphoreHandle_t cmd_lock;

bool AT_execute(ATCMD c)
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

void AT_consume( void * pvParameters )
{
    ATCMD c;
    while(true)
        if ( xQueueReceive(cmd_queue, &c, portMAX_DELAY) == pdTRUE) {
            AT_execute(c);
            delay(1);
        }
}

bool AT_cmd(ATCMD c, bool blocking=false)
{
    if (blocking) return AT_execute(c);
    else xQueueSend(cmd_queue, &c, portMAX_DELAY);
    return true;
}


void LoRa_init(int rxPin=16, int txPin=17)
{   
    cmd_lock = xSemaphoreCreateMutex();
    cmd_queue = xQueueCreate( 10, sizeof( ATCMD ) );
    xTaskCreate( AT_consume, "async_cmd", 2000, NULL, 0, NULL );

    Serial2.begin(115200, SERIAL_8N1, rxPin, txPin);

    AT_cmd( (ATCMD) {"LoraSet",     "868000000,20,12,0,1,8,1,0,0"}, true );
    AT_cmd( (ATCMD) {"AutoLPM",     "0"}, true );
    AT_cmd( (ATCMD) {"LORAWAN",     "0"}, true );
    AT_cmd( (ATCMD) {"PrintMode",   "0"}, true );
    AT_cmd( (ATCMD) {"RX",   "0"}, true );
}

unsigned long LoRa_send(String str)
{
    unsigned long start = millis();
    ATCMD c = {"SendStr", ""};
    strncpy(c.data, str.c_str(), sizeof(c.data) - 1);
    bool ret = AT_cmd( c, true );    
    AT_cmd( (ATCMD) {"RX",   "0"} );
    if (ret) return millis() - start;
    else return 0;
}

bool LoRa_available() 
{
    if( xSemaphoreTake( cmd_lock, 0 ) == pdTRUE )
    {
        bool ret = Serial2.available() > 0;
        xSemaphoreGive(cmd_lock);
        return ret;
    }
    return false;
}

String LoRa_read()
{
    String receive_data = "";
    
    xSemaphoreTake(cmd_lock, portMAX_DELAY);
    if (Serial2.available()) {
        receive_data = Serial2.readString();
        AT_cmd( (ATCMD) {"RX",   "0"} );
    }
    xSemaphoreGive(cmd_lock);
    return receive_data;
}
