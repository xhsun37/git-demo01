/*
   -- Bluetooth Classic Control for RC Car (ESP32) --
   Receives commands from Android app or Serial Bluetooth Terminal via BluetoothSerial.
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
   Pinout: IN1: GPIO 14, IN2: GPIO 27, IN3: GPIO 26, IN4: GPIO 25, ENA: GPIO 12, ENB: GPIO 13
*/

#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

// 馬達控制腳位定義
#define IN1 14  // 左馬達 IN1
#define IN2 27  // 左馬達 IN2
#define IN3 26  // 右馬達 IN3
#define IN4 25  // 右馬達 IN4
#define ENA 12  // 左馬達使能 (PWM)
#define ENB 13  // 右馬達使能 (PWM)

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
  SerialBT.setTimeout(5); // 縮短逾時，加快響應
  Serial.println("Bluetooth initialized. Waiting for connection... MAC: " + SerialBT.getBtAddressString());

  // 設定馬達腳位
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

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
      SerialBT.flush(); // 清除緩衝區
    }
  }

  // 若未連線，等待
  if (!isConnected) {
    delay(100); // 縮短等待時間
    return;
  }

  // 處理藍牙數據
  if (SerialBT.available()) {
    char c = SerialBT.read();
    Serial.print("Received command: ");
    Serial.println(c);

    // 每次指令前重置馬達
    Stop();

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
    // 清除剩餘緩衝區數據
    while (SerialBT.available()) {
      SerialBT.read();
    }
    SerialBT.flush(); // 確保緩衝區清空
    delay(5); // 縮短延遲，加快響應
  }
}

// 馬達控制函式
void Forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speed - 15); // 左馬達稍慢
  analogWrite(ENB, speed);
  Serial.print("Forward: IN1=H, IN2=L, IN3=H, IN4=L, ENA="); Serial.print(speed - 15);
  Serial.print(", ENB="); Serial.println(speed);
}

void Back() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speed - 15); // 左馬達稍慢
  analogWrite(ENB, speed);
  Serial.print("Backward: IN1=L, IN2=H, IN3=L, IN4=H, ENA="); Serial.print(speed - 15);
  Serial.print(", ENB="); Serial.println(speed);
}

void Left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  Serial.print("Left: IN1=L, IN2=H, IN3=H, IN4=L, ENA="); Serial.print(speed);
  Serial.print(", ENB="); Serial.println(speed);
}

void Right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speed - 20); // 左馬達稍慢
  analogWrite(ENB, speed);
  Serial.print("Right: IN1=H, IN2=L, IN3=L, IN4=H, ENA="); Serial.print(speed - 20);
  Serial.print(", ENB="); Serial.println(speed);
}

void Stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  Serial.println("Motors: IN1=L, IN2=L, IN3=L, IN4=L, ENA=0, ENB=0");
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