/*
   -- Bluetooth Classic Control for RC Car (ESP32) --
   Receives commands from Android app via BluetoothSerial.
   Commands:
     - '1': Forward
     - '2': Right
     - '3': Backward
     - '4': Left
     - '5': Toggle speed (low/high)
     - '6': Stop
     - 'S': Stop
     - 'R': Reset to low speed
   Hardware: ESP32-WROOM-32D, L298N, 2 DC motors, 7.4V battery, USB power
   Pinout: IN1: GPIO 14, IN2: GPIO 27, IN3: GPIO 26, IN4: GPIO 25
*/

#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

// 馬達控制腳位定義
#define IN1 14 // 左馬達 IN1
#define IN2 27 // 左馬達 IN2
#define IN3 26 // 右馬達 IN3
#define IN4 25 // 右馬達 IN4

// 速度模式
int speed = 180; // 初始低速
int control_spd = 1; // 速度模式計數器
char val = '0'; // 當前指令

void setup() {
  Serial.begin(115200); // 初始化序列埠
  delay(1000); // 確保序列埠穩定
  Serial.println("Starting ESP32...");

  // 初始化藍牙
  SerialBT.begin("zh1h3ng"); // 設備名稱
  Serial.println("Bluetooth initialized. Waiting for connection... MAC: " + SerialBT.getBtAddressString());

  // 設定馬達腳位
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // 初始化馬達停止
  Stop();
}

void loop() {
  // 檢查藍牙連線狀態
  static bool wasConnected = false;
  bool isConnected = SerialBT.hasClient();
  if (isConnected != wasConnected) {
    wasConnected = isConnected;
    Serial.print("Bluetooth connection: ");
    Serial.println(isConnected ? "Connected" : "Disconnected");
    if (!isConnected) {
      Stop();
    }
  }

  // 若未連線，等待
  if (!isConnected) {
    delay(500);
    return;
  }

  // 處理藍牙數據
  while (SerialBT.available()) {
    char c = SerialBT.read();
    Serial.print("Received command: ");
    Serial.println(c);

    if (c == 'S' || c == '6') { // 停止指令
      Stop();
    } else {
      val = c;
      switch (val) {
        case '1': // 前進
          Forward();
          break;
        case '2': // 右轉
          Right();
          break;
        case '3': // 後退
          Back();
          break;
        case '4': // 左轉
          Left();
          break;
        case '5': // 切換速度
          Change_Speed_And_Print();
          Stop();
          break;
        case '6': // 停止
          Stop();
          break;
        case 'R': // 重置模式
          Resetmode();
          Stop();
          break;
        default:
          Serial.println("Invalid command");
          Stop();
          break;
      }
    }
    delay(10); // 短延遲，確保穩定
  }
}

// 馬達控制函式
void Forward() {
  analogWrite(IN1, speed - 15); // 左馬達稍慢
  digitalWrite(IN2, LOW);
  analogWrite(IN3, speed);
  digitalWrite(IN4, LOW);
  Serial.print("Forward: IN1="); Serial.print(speed - 15);
  Serial.print(", IN2=0, IN3="); Serial.print(speed);
  Serial.println(", IN4=0");
}

void Back() {
  digitalWrite(IN1, LOW);
  analogWrite(IN2, speed - 15);
  digitalWrite(IN3, LOW);
  analogWrite(IN4, speed);
  Serial.print("Backward: IN1=0, IN2="); Serial.print(speed - 15);
  Serial.print(", IN3=0, IN4="); Serial.println(speed);
}

void Left() {
  digitalWrite(IN1, LOW);
  analogWrite(IN2, speed);
  analogWrite(IN3, speed);
  digitalWrite(IN4, LOW);
  Serial.print("Left: IN1=0, IN2="); Serial.print(speed);
  Serial.print(", IN3="); Serial.print(speed);
  Serial.println(", IN4=0");
}

void Right() {
  analogWrite(IN1, speed - 20); // 左馬達稍慢
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  analogWrite(IN4, speed);
  Serial.print("Right: IN1="); Serial.print(speed - 20);
  Serial.print(", IN2=0, IN3=0, IN4="); Serial.println(speed);
}

void Stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Motors: IN1=0, IN2=0, IN3=0, IN4=0");
}

void Change_Speed_And_Print() {
  control_spd++;
  if ((control_spd % 2) == 1) {
    speed = 180;
    Serial.println("Low speed mode");
    SerialBT.println("Low speed mode");
  } else {
    speed = 250;
    Serial.println("High speed mode");
    SerialBT.println("High speed mode");
  }
}

void Resetmode() {
  control_spd = 1;
  speed = 150;
  Serial.println("Reset to low speed (150)");
  SerialBT.println("Reset to low speed (150)");
}