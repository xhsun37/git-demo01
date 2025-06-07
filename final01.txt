#include "BluetoothSerial.h"

BluetoothSerial serialBT;

// 馬達控制腳位定義
#define IN1 14  // 左馬達 IN1
#define IN2 27  // 左馬達 IN2
#define IN3 26  // 右馬達 IN3
#define IN4 25  // 右馬達 IN4

void setup() {
  Serial.begin(115200);
  serialBT.begin("zh1h3ng");
  delay(500); // 等待藍牙初始化

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotors(); // 啟動時先停車
}

void loop() {
  // 檢查藍牙連線
  if (!serialBT.hasClient()) {
    Serial.println("等待藍牙連接...");
    stopMotors();
    delay(1000);
    return;
  }

  if (serialBT.available()) {
    int cmd = serialBT.read(); // 讀取指令
    Serial.print("收到指令：");
    Serial.println((char)cmd);

    switch (cmd) {
      case 'A': // 左轉
        Serial.println("左轉");
        turnLeft(200); // PWM 速度 200
        break;
      case 'B': // 右轉
        Serial.println("右轉");
        turnRight(200);
        break;
      case 'C': // 前進
        Serial.println("前進");
        goForward(200);
        break;
      case 'D': // 後退
        Serial.println("後退");
        goBackward(200);
        break;
      case 'E': // 停止
        Serial.println("停止");
        stopMotors();
        break;
      default:
        Serial.println("無效指令");
        stopMotors();
        break;
    }

    // 清理緩衝區
    while (serialBT.available()) {
      serialBT.read();
    }
    delay(50); // 縮短延遲，提高響應
  }
}

// 馬達動作函式定義
void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void goForward(int speed) {
  analogWrite(IN1, speed);
  digitalWrite(IN2, LOW);
  analogWrite(IN3, speed);
  digitalWrite(IN4, LOW);
}

void goBackward(int speed) {
  digitalWrite(IN1, LOW);
  analogWrite(IN2, speed);
  digitalWrite(IN3, LOW);
  analogWrite(IN4, speed);
}

void turnLeft(int speed) {
  digitalWrite(IN1, LOW);
  analogWrite(IN2, speed);
  analogWrite(IN3, speed);
  digitalWrite(IN4, LOW);
}

void turnRight(int speed) {
  analogWrite(IN1, speed);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  analogWrite(IN4, speed);
}