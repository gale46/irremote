/*
 * SendAndReceive.cpp
 *
 * Demonstrates sending IR codes and receiving it simultaneously
 *
 *  This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 ************************************************************************************
 * MIT License
 *
 * Copyright (c) 2021-2023 Armin Joachimsmeyer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************************
 */



//#define EXCLUDE_UNIVERSAL_PROTOCOLS // Saves up to 1000 bytes program memory.
//#define EXCLUDE_EXOTIC_PROTOCOLS // saves around 650 bytes program memory if all other protocols are active
//#define NO_LED_FEEDBACK_CODE      // saves 92 bytes program memory
//#define RECORD_GAP_MICROS 12000   // Default is 8000. Activate it for some LG air conditioner protocols
//#define SEND_PWM_BY_TIMER         // Disable carrier PWM generation in software and use (restricted) hardware PWM.
//#define USE_NO_SEND_PWM           // Use no carrier PWM, just simulate an active low receiver signal. Overrides SEND_PWM_BY_TIMER definition

//#define DEBUG // Activate this for lots of lovely debug output from the decoders.
#include <Arduino.h>
#include <EEPROM.h>


// select only NEC and the universal decoder for pulse distance protocols
#define DECODE_NEC             // Includes Apple and Onkyo
#define DECODE_DISTANCE_WIDTH  // In case NEC is not received correctly. Universal decoder for pulse distance width protocols

#include "PinDefinitionsAndMore.h"  // Define macros for input and output pin etc.
#include <IRremote.hpp>
#define IR_RECEIVE_PIN 7
#define LED_REC 2
#define LED_SEN 8
#define numMaxIRCodes 5//儲存數量
#define DELAY_AFTER_SEND 2000
#define DELAY_AFTER_LOOP 5000

// int EEPROM_ADDRESS = 0
int EEPROM_BASE_ADDRESS = 10;
unsigned long lastReceiveTime = 0; 
unsigned long receiveTimeout = 5000;
bool receiveMode = false;
int numIRCodes;//計算儲存成功個數
int count = 1;//計算loop次數
//儲存型態
struct IRCode {
    uint16_t address;
    uint8_t command;
    uint8_t repeats;
};
IRCode irCodes[10];
void setup() {
  numIRCodes = EEPROM.read(0);//第0個存放初始化資料個數
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  IrSender.begin();  // Start with IR_SEND_PIN -which is defined in PinDefinitionsAndMore.h- as send pin and enable feedback LED at default feedback LED pin
  if (ENABLE_LED_FEEDBACK) {//紅外線接收成功亮燈
    pinMode(LED_REC, OUTPUT);
  }

}





void clear(){
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

/*
 * Send NEC IR protocol
 */
void send_ir_data(int i) {
    IRCode irCodes;
    int address = EEPROM_BASE_ADDRESS + i * sizeof(irCodes)*10;
    EEPROM.get(address , irCodes);
    digitalWrite(LED_SEN, HIGH);
    delay(100);
    digitalWrite(LED_SEN, LOW);
    Serial.print(F("Sending: 0x"));
    Serial.print(irCodes.address, HEX);
    Serial.print(irCodes.command, HEX);
    Serial.println(irCodes.repeats, HEX);
    IrSender.sendNEC(irCodes.address, irCodes.command, irCodes.repeats++);

    Serial.print("EEPROM address to store: ");
    Serial.println(address);
    Serial.print("EEPROM address of size: ");
    Serial.println(sizeof(irCodes));

    Serial.flush(); 
  

}

void receive_ir_data() {
  Serial.println(IrReceiver.decode());
  if (IrReceiver.decode()) {
    lastReceiveTime = millis();
    digitalWrite(LED_REC, HIGH);
    delay(100);
    digitalWrite(LED_REC, LOW);
    bool turn = true; 
    //檢查是否有重複
    for(int i = 0;i <= numIRCodes;i++){
      if(irCodes[i].command == IrReceiver.decodedIRData.command){
        Serial.print(F("decoded command EXIST: "));
        Serial.println(irCodes[i].command, HEX);
        turn = false;//將turn設為false避免重複儲存
        break;
      }
    }
    if(numIRCodes <= numMaxIRCodes && turn == true){
      irCodes[numIRCodes].address = IrReceiver.decodedIRData.address;
      irCodes[numIRCodes].command = IrReceiver.decodedIRData.command;
      irCodes[numIRCodes].repeats = 1;
      Serial.print(F("decoded address"));
      Serial.print(numIRCodes);
      Serial.print(": ");
      Serial.print(irCodes[numIRCodes].address, HEX);
      Serial.print(F(" decoded command"));
      Serial.print(numIRCodes);
      Serial.print(": ");
      Serial.print(irCodes[numIRCodes].command, HEX);
      Serial.print(F(" decoded repeats"));
      Serial.print(numIRCodes);
      Serial.print(": ");
      Serial.println(irCodes[numIRCodes].repeats, HEX);
      numIRCodes++;
      int address = EEPROM_BASE_ADDRESS + numIRCodes * sizeof(irCodes);
      EEPROM.put(address, irCodes);//存到eeprom指定位置
      EEPROM.write(0, numIRCodes);//0存放ircode的numIRCode個數
      Serial.print("EEPROM address to store: ");
      Serial.println(address);
      Serial.print("EEPROM address of size: ");
      Serial.println(sizeof(irCodes));
      Serial.println(numIRCodes);
      // EEPROM.commit();
      send_ir_data(numIRCodes);
    }
    IrReceiver.resume();// Clear the decode buffer
    
  }
  if(millis() - lastReceiveTime >=  receiveTimeout){
      receiveMode = false;
  }
}

void loop() {
  while(!Serial.available());
  int input = Serial.readString().toInt();
  // Serial.println("connect");
  Serial.print("input: ");
  Serial.println(input);
  switch(input){
      case 0:
        receiveMode = true;
        lastReceiveTime = millis();
        while(receiveMode == true){
          receive_ir_data();
          Serial.println("receving");
          delay(500);
        }
        Serial.println("complete");
        break;
      case 1:
          send_ir_data(1);
          // IrReceiver.restartAfterSend();
          Serial.println("complete");
        break;
      case 2:
          send_ir_data(2);
          // IrReceiver.restartAfterSend();
          Serial.println("complete");
        break;
      case 3:
          send_ir_data(3);
          // IrReceiver.restartAfterSend();
          Serial.println("complete");
        break;
      case 4:
        send_ir_data(4);
        // IrReceiver.restartAfterSend();
        Serial.println("complete");
      break;
      case 5:
        clear();
        Serial.println("complete");
      break;
      default:
        break;
    }
  // Serial.print("loop: ");
  // Serial.println(count);
  // count++;
  Serial.flush();
}
