#include <IRremote.h>

const int IR_RECEIVE_PIN = 7;  // 红外接收器连接到 Arduino 的 7 号脚
// #define IR_SEND_PIN  11

IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;
IRsend irsend;
void setup() {
  // decode_type_t {
  //   UNKNOWN = 0, PULSE_WIDTH, PULSE_DISTANCE, APPLE,
  //   DENON, JVC, LG, LG2,
  //   NEC, NEC2, ONKYO, PANASONIC,
  //   KASEIKYO, KASEIKYO_DENON, KASEIKYO_SHARP, KASEIKYO_JVC,
  //   KASEIKYO_MITSUBISHI, RC5, RC6, SAMSUNG,
  //   SAMSUNGLG, SAMSUNG48, SHARP, SONY,
  //   BANG_OLUFSEN, BOSEWAVE, LEGO_PF, MAGIQUEST,
  //   WHYNTER, FAST
  // }
  Serial.begin(9600); 
  irrecv.enableIRIn();  // 启用红外接收器
  pinMode(2, OUTPUT);
  
}

void loop() {
  for(int i = 0;i<3;i++){
    irsend.sendSony(0xCF311EE, 32);
    delay(1000);
    irsend.sendNEC(0xCF3619E, 32);  
    delay(2000);
  }
}
