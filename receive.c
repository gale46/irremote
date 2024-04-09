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
  if (irrecv.decode(&results)) {
    // 显示接收到的红外信号码
    Serial.print("Received IR code: 0x");
    Serial.println(results.value, HEX);
    Serial.println(results.decode_type);
    Serial.println(results.bits);
    digitalWrite(2, HIGH);
    delay(100);
    digitalWrite(2, LOW);
    delay(500);  // 等待一段时间，以便完全处理当前信号
    irrecv.resume();  // 继续接收下一个红外信号
  }
}
