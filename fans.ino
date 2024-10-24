#define IR_RECEIVE_PIN 7  // ir
#define SERVO_PIN 8        // servo

bool shake = false;  // shake開關
unsigned long previousShakeMillis = 0;  // shake在一定時間內不可重複call function
const int debounceInterval = 500;  // 防止按鍵抖動

unsigned long previousServoMillis = 0;  // 上次servo作動時間
int servoPos = 0;  // 當前servo角度
bool movingRight = true;  // servo的移動方向

bool turn_on_off = false;
unsigned long previousTurnOnMillis = 0;
int motor = 6;
int motorSpeed = 200;

#include <IRremote.hpp>
#include <Servo.h>

Servo myservo; 

struct IRCode {
  uint16_t address;
  uint8_t command;
  uint8_t repeats;
};

IRCode irCodes[4];  

void setup() {
  Serial.begin(9600);
  myservo.attach(SERVO_PIN);  
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); 
  pinMode(motor, OUTPUT);  
  // 紅外線編碼
  irCodes[0] = {48, 136, 1};
  irCodes[1] = {48, 134, 1};
  irCodes[2] = {48, 135, 1};
  irCodes[3] = {48, 133, 1};
}
void loop() {

  if (IrReceiver.decode()) {
    uint16_t address = IrReceiver.decodedIRData.address;
    uint8_t command = IrReceiver.decodedIRData.command;

    // check struct中是否存在一樣的編碼
    int index = findIRCodeIndex(address, command);
    if (index != -1) {
      executeAction(index);  //由index決定動作0開關,1 增加,2減少, 3擺頭
    } else {
      Serial.println("No matching command found.");
    }

    IrReceiver.resume();
  }

  // 馬達控制：只有當開關為 ON 時才會啟動，並每秒檢查一次狀態
  if (turn_on_off) {
    analogWrite(motor, motorSpeed);  // 更新馬達速度
  }else{
    analogWrite(motor, 0);
  }

  // 如果按下shake按鍵且0.5秒內沒有重複執行避免function卡住出不來
  if (shake && millis() - previousServoMillis >= 500) {
    previousServoMillis = millis();
    actionShake();
  }
}
void actionShake() {
  if (movingRight) {
    servoPos += 5;
    if (servoPos >= 180) {
      movingRight = false;  // 最大角度180後向左
    }
  } else {
    servoPos -= 5;
    if (servoPos <= 0) {
      movingRight = true;  
    }
  }

  myservo.write(servoPos); 
}

// 遍歷找個struct找出相同
int findIRCodeIndex(uint16_t address, uint8_t command) {
  for (int i = 0; i < 4; i++) {
    if (irCodes[i].address == address && irCodes[i].command == command) {
      return i;
    }
  }
  return -1; 
}

// 依照index做出對應動作
void executeAction(int index) {
  unsigned long currentMillis = millis();
  switch (index) {
    
    case 0:  // 開關
      Serial.println("Action: Turn on/off.");
      
      // 只有在按鈕未被按下時才執行狀態切換
      if (currentMillis - previousTurnOnMillis >= 700) {
        previousTurnOnMillis = currentMillis;
        turn_on_off = !turn_on_off;  // 切換開關狀態
        
        Serial.println(turn_on_off ? "Fan ON" : "Fan OFF");
      }
      

      break;

    case 1:
      Serial.println("Action: Increase speed.");
      if (motorSpeed < 151) motorSpeed += 50;  // 增加速度
      Serial.print("Motor Speed: ");
      Serial.println(motorSpeed);
      break;

    case 2:
      Serial.println("Action: Decrease speed.");
      if (motorSpeed > 50) motorSpeed -= 50;  // 減少速度
      Serial.print("Motor Speed: ");
      Serial.println(motorSpeed);
      break;

    case 3:
      Serial.println("Action: Toggle shake.");
    
      if (currentMillis - previousShakeMillis >= debounceInterval) {
        previousShakeMillis = currentMillis;  // 更新上次按下按鈕的時間防止按鈕抖動重複執行
        shake = !shake;  // 按下按鈕後交換bool值
        Serial.println(shake ? "Shake ON" : "Shake OFF");
      }
      break;

    default:
      Serial.println("Unknown action.");
      break;
  }
}

