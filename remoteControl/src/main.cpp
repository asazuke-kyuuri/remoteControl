#include <Arduino.h>
#include <ESP32Servo.h> 
#include <PS4Controller.h>
#include "PS4Controller.h"
/* ここにサーボモータのライブラリをインクルードする */
//
//

// PS4コントローラーのMACアドレス
// 使いたいPS4コントローラのMACアドレスをあらかじめ調べておく
#define PS4_ADDR "0C:B8:15:C5:1C:C4" 

//サーボモータにつなぐピンのお名前
#define leftPin 33
#define rightPin 22

//サーボモータのお名前
Servo servoLeft;
Servo servoRight;

bool turnCounter=true; //曲がって直進するか　void loop()の中で使用

void ReceiveControllerInput();

float l_x = 0.0; // 左スティックのX軸
float l_y = 0.0; // 左スティックのY軸
float r_x = 0.0; // 右スティックのX軸
float r_y = 0.0; // 右スティックのY軸

// ボタンの状態を保存する配列(単押し, 長押し判定用)
bool prev_bttn_state[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

void setup() {
  Serial.begin(115200);
  PS4.begin(PS4_ADDR);
  Serial.println("> PS4: Started");
  Serial.println("> PS4: Press PS button to connect");
  // PS4コントローラーが接続されるまで待つ
  while (!PS4.isConnected()) {
    Serial.println("> PS4: Connecting...");
    delay(1000);
  }
  // 接続されたら，LEDを白色にして振動させる
  Serial.println("> PS4: Connected");
  PS4.setLed(255, 255, 255);
  PS4.setRumble(200, 200);
  PS4.sendToController(); // PS4に送信(これでLEDが白色＆振動する)
  delay(1000); // 1秒間待つ
  PS4.setRumble(0, 0);
  PS4.sendToController(); // PS4に送信(これで振動が止まる)

  /* ここにサーボモータの初期化コードを書く */
  ESP32PWM::allocateTimer(0); // ledcチャンネル 0,1,8,9用タイマ(C Plusではノイズ音が鳴るため非推奨)
  ESP32PWM::allocateTimer(1); // ledcチャンネル 2,3,10,11用タイマ割当許可
  servoLeft.setPeriodHertz(50); 
  servoLeft.attach(leftPin, 500, 2500);
  servoRight.setPeriodHertz(50); 
  servoRight.attach(rightPin, 500, 2500);
  
}
 
void loop() {
  bool moveSerect=false;//停止かそれ以外か
  ReceiveControllerInput(); // PS4コントローラーの入力を受け取る
  Serial.printf("> PS4: LStickX: %f, LStickY: %f, RStickX: %f, RStickY: %f\n", l_x, l_y, r_x, r_y);
  /* ここにスティックの値に応じたサーボモータの制御コードを書く */
  if(r_x!=0||r_y!=0){
    turnCounter=true;
  }
  if(l_x!=0||l_y!=0){
    moveSerect=true;
  }
  if(moveSerect){
    if(l_x==0&&l_y==1){
      //直進
       servoLeft.write(0);
       servoRight.write(0);
    }
    else if(l_x==0&&l_y==-1){
      //後進
      servoLeft.write(180);
      servoRight.write(180);
    }
    else if(l_x==1&&l_y==0){
      if(turnCounter){
        //機体を90度右に回転
        servoLeft.write(0);
        servoRight.write(90);
        turnCounter=false;
        delay(30); //適切な値を探す
      }
      //直進
      servoLeft.write(0);
      servoRight.write(0);
    }
    else if(l_x==-1&&l_y==0){
      if(turnCounter){
        //機体を90度左に回転
        servoLeft.write(90);
        servoRight.write(0);
        turnCounter=false;
        delay(30); //適切な値を探す
      }
      //直進
      servoLeft.write(0);
      servoRight.write(0);
    }
  }
  else{
    //停止
    servoLeft.write(90);
    servoRight.write(90);
  }
  delay(10); // delayの秒数[ms]は適宜変更する. 10ms = 0.01s.
}

// PS4コントローラーの入力を受け取る
void ReceiveControllerInput() {
  if (PS4.LatestPacket() && PS4.isConnected()) {
    // スティックの値を取得して，値の範囲を-1.0 ~ 1.0に正規化する
    l_x = PS4.LStickX() / 127.0; // -127 ~ 127 -> -1.0 ~ 1.0
    l_y = PS4.LStickY() / 127.0; // -127 ~ 127 -> -1.0 ~ 1.0
    r_x = PS4.RStickX() / 127.0; // -127 ~ 127 -> -1.0 ~ 1.0
    r_y = PS4.RStickY() / 127.0; // -127 ~ 127 -> -1.0 ~ 1.0

    // ボタン押したときの挙動をifの中に書く
    if (PS4.Up() && !prev_bttn_state[0]) {
      Serial.println("> PS4: Up button pressed");
    } 
    if (PS4.Down() && !prev_bttn_state[1]) {
      Serial.println("> PS4: Down button pressed");
    }
    if (PS4.Left() && !prev_bttn_state[2]) {
      Serial.println("> PS4: Left button pressed");
    }
    if (PS4.Right() && !prev_bttn_state[3]) {
      Serial.println("> PS4: Right button pressed");
    }
    if (PS4.Triangle() && !prev_bttn_state[4]) {
      Serial.println("> PS4: Triangle button pressed");
    }
    if (PS4.Circle() && !prev_bttn_state[5]) {
      Serial.println("> PS4: Circle button pressed");
    }
    if (PS4.Cross() && !prev_bttn_state[6]) {
      Serial.println("> PS4: Cross button pressed");
    }
    if (PS4.Square() && !prev_bttn_state[7]) {
      Serial.println("> PS4: Square button pressed");
    }
    if (PS4.L1() && !prev_bttn_state[8]) {
      Serial.println("> PS4: L1 button pressed");
    }
    if (PS4.R1() && !prev_bttn_state[9]) {
      Serial.println("> PS4: R1 button pressed");
    }
    if (PS4.L2() && !prev_bttn_state[10]) {
      Serial.println("> PS4: L2 button pressed");
    }
    if (PS4.R2() && !prev_bttn_state[11]) {
      Serial.println("> PS4: R2 button pressed");
    }
    if (PS4.Share() && !prev_bttn_state[12]) {
      Serial.println("> PS4: Share button pressed");
    }
    if (PS4.Options() && !prev_bttn_state[13]) {
      Serial.println("> PS4: Options button pressed");
    }
    if (PS4.L3() && !prev_bttn_state[14]) {
      Serial.println("> PS4: L3 button pressed");
    }
    if (PS4.R3() && !prev_bttn_state[15]) {
      Serial.println("> PS4: R3 button pressed");
    }

    // デフォルトだと，押した瞬間だけ判定される．
    // 長押し判定するなら，任意のボタンの部分をコメントアウトする↓
    /* 
      UPボタンを長押しできるようにしたいとき 
      例: // prev_bttn_state[0] = PS4.Up(); 
    */
    prev_bttn_state[0] = PS4.Up();
    prev_bttn_state[1] = PS4.Down();
    prev_bttn_state[2] = PS4.Left();
    prev_bttn_state[3] = PS4.Right();
    prev_bttn_state[4] = PS4.Triangle();
    prev_bttn_state[5] = PS4.Circle();
    prev_bttn_state[6] = PS4.Cross();
    prev_bttn_state[7] = PS4.Square();
    prev_bttn_state[8] = PS4.L1();
    prev_bttn_state[9] = PS4.R1();
    prev_bttn_state[10] = PS4.L2();
    prev_bttn_state[11] = PS4.R2();
    prev_bttn_state[12] = PS4.Share();
    prev_bttn_state[13] = PS4.Options();
    prev_bttn_state[14] = PS4.L3();
    prev_bttn_state[15] = PS4.R3();
  }
}

