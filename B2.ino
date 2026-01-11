#include <Arduino_FreeRTOS.h>
#include <semphr.h>

//==========
// khai bao pin
//==========
//LCD pin
const int RS =12, EN = 11;
const int D4 =5, D5=4, D6= 3, D7= 2;
const int TEMP = A0; //LM35 pin
const int LED = 8; // led red pin

SemaphoreHandle_t Flag; // khai bao co RTOS
volatile float Temp;
//==========
// LCD_handle
//==========
// ham tao xung Enable
void pulseEnable(){
  digitalWrite(EN,LOW);
  delayMicroseconds(1);
  digitalWrite(EN,HIGH);
  delayMicroseconds(1);
  digitalWrite(EN,LOW);
  delayMicroseconds(100);
}
// ham gui 4bit MSB=D7 LSB=D4
void Sen4bit(byte data){
  digitalWrite(D7, (data >>3)&0b00001);
  digitalWrite(D6, (data >>2)&0b00001);
  digitalWrite(D5, (data >>1)&0b00001);
  digitalWrite(D4, data &0b00001);
  pulseEnable();
}
// ham gui 8bit, gui 4bit MSB truoc, mode chon gui lenh hay du lieu
void SendData(byte data, bool mode){
  digitalWrite(RS,mode); // LOW lenh, high du lieu
  Sen4bit(data>>4);
  Sen4bit(data & 0b00001111);
}
//ham gui lenh chan Resis Select
void LcdCMD(byte cmd){
  SendData(cmd,LOW);
}
//ham gui 1 ky tu
void lcdWriteChar (byte data){
  SendData(data,HIGH);
}
//ham gui chuoi ky tu
void lcdPrint(const char* str){
  while(*str)
    lcdWriteChar(*str ++);
}
//==========
//task 1
void TaskLM35(void *pvParameters){
  // pin setup
  pinMode(TEMP, INPUT);
  float CurrentTemp;
  //vong lap
  while(1){
    if(xSemaphoreTake(Flag, portMAX_DELAY) == pdTRUE){
      Temp = (5.0*analogRead(TEMP)*100.0)/1024.0;
      xSemaphoreGive(Flag);
    } 
    vTaskDelay(pdMS_TO_TICKS(250));
  }
}
//task 2
void TasKLCD(void *pvParameters){
  // pin setup
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(RS, OUTPUT);
  pinMode(EN, OUTPUT);
  float receivedTemp;
  char PrintTemp[10];
  // khoi tao LCD che do 4bit
  vTaskDelay(pdMS_TO_TICKS(20)); //doi LCD on dinh nguon
  Sen4bit(0x02); // gui lenh khoi tao o che do 4bit
  // cau hinh interface
  LcdCMD(0x28); // cau hinh 2 dong, 4bit
  LcdCMD(0x0C); // bat mang hinh, tat con tro
  LcdCMD(0x06); // tu dong tang con tro
  LcdCMD(0x01); // xoa mang hinh
  vTaskDelay(pdMS_TO_TICKS(1));
  //vong lap
  while(1){
    float receivedTemp;
    if(xSemaphoreTake(Flag, portMAX_DELAY) == pdTRUE) {
      receivedTemp = Temp;
      xSemaphoreGive(Flag);
    }
      dtostrf(receivedTemp,6,2,PrintTemp);
      if(receivedTemp > 100){
        LcdCMD(0x80); //bat dau ghi o dong 1 cot 0
        lcdPrint("Systerm Info");
        LcdCMD(0xC0);
        lcdPrint("   Warning   ");
      }
      else{
        LcdCMD(0x80); //bat dau ghi o dong 1 cot 0
        lcdPrint("Systerm Info");
        LcdCMD(0xC0);
        lcdPrint("Temp:");
        lcdPrint(PrintTemp);
      }
    
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
//task 3
void TaskLED(void *pvParameters){
  // pin setup
  pinMode(LED, OUTPUT);
  float receivedWarn;
  //vong lap
  while(1){
    if(xSemaphoreTake(Flag, portMAX_DELAY) == pdTRUE) {
      receivedWarn = Temp;
      xSemaphoreGive(Flag);
    }
      if(receivedWarn > 100){
        digitalWrite(LED,HIGH);
        vTaskDelay(pdMS_TO_TICKS(1000));
        digitalWrite(LED,LOW);
        vTaskDelay(pdMS_TO_TICKS(1000));
      }
      else{
        vTaskDelay(pdMS_TO_TICKS(50));
      }
    
  }
}
void setup() {
  Flag = xSemaphoreCreateMutex();
  xTaskCreate(TaskLM35,"LM35",128,NULL,2,NULL);
  xTaskCreate(TasKLCD,"LCD",256,NULL,1,NULL);
  xTaskCreate(TaskLED,"LED",128,NULL,1,NULL);
  vTaskStartScheduler();
}
 
void loop() {
  //hello world
}
