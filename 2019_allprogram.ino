#include <Adafruit_NeoPixel.h>
//サーボモーターを制御するためのライブラリをインクルード
#include <SoftwareSerial.h>
#include<Servo.h>
#include <Wire.h>
#include <MP3Player_KT403A.h>
#include "Adafruit_MPR121.h"
#include <FlexiTimer2.h>
#include<EEPROM.h>
#include <time.h>
#ifdef __AVR__
#include <avr/power.h>
#endif


/************************************************************  HVC-P2(カメラ) 設定  *********************************************************************/
SoftwareSerial mySerial(62, 63); // RX, TX

#define Synchronous_Code        (byte)0xFE

/* Command number */
#define GET_VERSION             (byte)0x00
#define SET_CAMERA_ANGLE        (byte)0x01
#define GET_CAMERA_ANGLE        (byte)0x02
#define EXECUTE                 (byte)0x04
#define SET_THRESHOLD           (byte)0x05
#define GET_THRESHOLD           (byte)0x06
#define SET_SIZE_RANGE          (byte)0x07
#define GET_SIZE_RANGE          (byte)0x08
#define SET_DETECTION_ANGLE     (byte)0x09
#define GET_DETECTION_ANGLE     (byte)0x0A
#define SET_BAUDRATE            (byte)0x0E
#define REGISTRATION            (byte)0x10
#define DELETE_DATA             (byte)0x11
#define DELETE_USER             (byte)0x12
#define DELETE_ALL              (byte)0x13
#define GET_PERSON_DATA         (byte)0x15
#define SAVE_ALBUM              (byte)0x20
#define LOAD_ALBUM              (byte)0x21
#define WRITE_ALBUM             (byte)0x22

/* Execution flag */
#define BODY_DETECTION          0x00000001
#define HAND_DETECTION          0x00000002
#define FACE_DETECTION          0x00000004
#define FACE_DIRECTION          0x00000008
#define AGE_ESTIMATION          0x00000010
#define GENDER_ESTIMATION       0x00000020
#define GAZE_ESTIMATION         0x00000040
#define BLINK_ESTIMATION        0x00000080
#define EXPRESSION_ESTIMATION   0x00000100
#define FACE_RECOGNITION        0x00000200

/* Image info of Execute command */
#define IMAGE_NONE              0x00000000
#define IMAGE_QVGA              0x00000001
#define IMAGE_QVGA_HALF         0x00000002

int tmp;
static const int TIME_OUT = 3000;
static const int MAX_DATA_LENGTH = 512;
uint8_t          data[MAX_DATA_LENGTH] = {0};
char             buf[128];

//int execFlag = BODY_DETECTION | HAND_DETECTION | FACE_DETECTION | FACE_DIRECTION | AGE_ESTIMATION | GENDER_ESTIMATION | GAZE_ESTIMATION | BLINK_ESTIMATION | EXPRESSION_ESTIMATION | FACE_RECOGNITION;
//int imageno = IMAGE_NONE;
int execFlag =  BODY_DETECTION | HAND_DETECTION | FACE_DETECTION | FACE_DIRECTION | AGE_ESTIMATION | GENDER_ESTIMATION | GAZE_ESTIMATION | BLINK_ESTIMATION | EXPRESSION_ESTIMATION | FACE_RECOGNITION;
int imageno = IMAGE_NONE;


/* 各種評価値 */
int HAPPINESS = 0;
int posX = 0;
int GENDER;
int AGE;
int SIZE;
int FACE_COUNT = 0;
int happy = 0;
int un_happy = 0;
int A_happy = 0;
int A_un_happy = 0;
int A_kao;
int A_times;
int B_happy = 0;
int B_un_happy = 0;
int B_kao;
int B_times;
int C_happy = 0;
int C_un_happy = 0;
int C_kao;
int C_times;
int D_happy = 0;
int D_un_happy = 0;
int D_kao;
int D_times;
int times = 0;
int times_happy = 0;
int times_un_happy = 0;
int C1;
int C2;
int C3;
int Conclusion;
int data_aaa = 0;



// Read byte
static inline bool HVC_ReadByte(volatile uint8_t* readBuff, const int byteCount, unsigned long time_start) {
  volatile int count = 0;

  while (count < byteCount) {
    if (mySerial.available() > 0) {
      readBuff[count] = mySerial.read();
      count++;
    }

    //Timeout
    unsigned long n = millis();

    if (n - time_start > TIME_OUT) {
      return false;
    }
  }

  return true;
}

static inline void HVC_ClearSerial() {
  while (true) {
    if (mySerial.available() > 0) {
      volatile byte r = mySerial.read();
    }
    else {
      break;
    }
  }
}

/************************************************************  HVC-P2  GetVersion  *********************************************************************/

void HVC_GetVersion() {
  int ret = false;
  unsigned long time_start = millis();
  uint8_t response[4] = {0}, model[13] = {0}, major_ver = 0, minor_ver = 0, release_ver = 0, revision[4] = {0};
  uint32_t revision_data = 0;

  // Get Model and Version
  Serial.println("HVC_GetVersion()");
  mySerial.write(Synchronous_Code);  //FE
  mySerial.write(GET_VERSION);  //00
  mySerial.write((byte)0x00);
  mySerial.write((byte)0x00);
  mySerial.flush();  //送信完了まで待機

  do {
    ret = true;
    while (ret) {
      uint8_t r1 = 0x00, r2 = 0xFF;
      ret = HVC_ReadByte(&r1, 1, time_start);
      Serial.print(ret);

      if ((r1 == 0xFE) && ret) {
        ret = HVC_ReadByte(&r2, 1, time_start);
      }
      else {
        continue;
      }

      if ((r1 == 0xFE) && (r2 == 0x00) && ret) {

        break;
      }
    }
    if (ret == false) {
      break;
    }

    // Check Synchronous code and Response code
    ret = HVC_ReadByte(response, 4, time_start);
    if (ret == false) {
      break;
    }

    // Check data length
    if (response[0] != 0x13) {
      ret = false;
      break;
    }
    if (response[1] != 0x00) {
      ret = false;
      break;
    }
    if (response[2] != 0x00)
    {
      ret = false;
      break;
    }
    if (response[3] != 0x00) {
      ret = false;
      break;
    }

    // Get Model
    ret = HVC_ReadByte(model, 12, time_start);
    if (ret == false) {
      break;
    }

    // Get Major version
    ret = HVC_ReadByte(&major_ver, 1, time_start);
    if (ret == false) {
      break;
    }

    // Get Minor version
    ret = HVC_ReadByte(&minor_ver, 1, time_start);
    if (ret == false) {
      break;
    }

    // Get Release version
    ret = HVC_ReadByte(&release_ver, 1, time_start);
    if (ret == false) {
      break;
    }

    // Get Revision number
    ret = HVC_ReadByte(revision, 4, time_start);
    if (ret == false) {
      break;
    }

    revision_data = (revision[3] << 24) | (revision[2] << 16) | (revision[1] << 8) | revision[0];
  } while (0);

  if (ret == false) {
    Serial.print("ERROR: response : ng\n");

    //HVC_ClearSerial();
    return ret;
  }

  sprintf(buf, "Model:%s, ", model);
  Serial.print(buf);

  sprintf(buf, "Major:%d, ", major_ver);
  Serial.print(buf);

  sprintf(buf, "Minor:%d, ", minor_ver);
  Serial.print(buf);

  sprintf(buf, "Release:%d, ", release_ver);
  Serial.print(buf);

  sprintf(buf, "Revision:%d\n", revision_data);
  Serial.print(buf);
}

// Execute
bool Execute(int execflag, int imageno) {
  int ret = false, index = 0;
  uint8_t sendData[32], response[6] = {0};
  uint8_t nHumanCount = 0, nHandCount = 0, nFaceCount = 0;
  uint32_t data_length = 0;
  unsigned long time_start = millis();

  /* Send Execute command signal */
  sendData[0] = (uint8_t)(execflag & 0xff);
  sendData[1] = (uint8_t)((execflag >> 8) & 0xff);
  sendData[2] = (uint8_t)(imageno & 0xff);

  HVC_ClearSerial();
  mySerial.write(Synchronous_Code);
  mySerial.write(EXECUTE);
  mySerial.write((byte)0x03);
  mySerial.write((byte)0x00);
  mySerial.write((byte)sendData[0]);
  mySerial.write((byte)sendData[1]);
  mySerial.write((byte)sendData[2]);
  mySerial.flush();

  do {
    // Check Synchronous code and Response code
    ret = true;
    while (ret) {
      uint8_t r1 = 0x00, r2 = 0xFF;
      ret = HVC_ReadByte(&r1, 1, time_start);

      if ((r1 == 0xFE) && ret) {
        ret = HVC_ReadByte(&r2, 1, time_start);
      }
      else {
        continue;
      }

      if ((r1 == 0xFE) && (r2 == 0x00) && ret) {
        break;
      }
    }
    if (ret == false) {
      break;
    }

    uint8_t response[4] = {0};
    ret = HVC_ReadByte(response, 4, time_start);
    if (ret == false) {
      break;
    }

    data_length = (response[3] << 24) | (response[2] << 16) | (response[1] << 8) | response[0];
    if (data_length == 0) {
      ret = false;
      break;
    }
    if (data_length >= MAX_DATA_LENGTH) {
      ret = false;
      break;
    }

    // Get data
    ret = HVC_ReadByte(data, data_length, time_start);
    if (ret == false) {
      break;
    }

  } while (0);

  if (ret == false) {
    Serial.print("ERROR: response : ng!\n");
    HVC_ClearSerial();
    return ret;
  }

  // header
  nHumanCount = data[index + 0];
  nHandCount  = data[index + 1];
  nFaceCount  = data[index + 2];
  FACE_COUNT = nFaceCount;
  index += 4;

  if (nHumanCount < 0 || nHumanCount > 35) {
    Serial.print("nHumanCount : Error");
    return ret;
  }

  if (nHandCount < 0 || nHandCount > 35) {
    Serial.print("nHandCount : Error");
    return ret;
  }

  if (nFaceCount < 0 || nFaceCount > 35) {
    Serial.print("nFaceCount : Error");
    return ret;
  }

  //顔の位置判定に利用
  sprintf(buf, "Face result count:%d\n", nFaceCount); 
  Serial.print(buf);
  for (int i = 0; i < nFaceCount; i++) {
    if (0 != (execflag & FACE_DETECTION)) {
      int16_t x = 0, y = 0, size = 0, conf = 0;
      x    = (int16_t)(((uint16_t)data[index + 1] << 8) | ((uint16_t)data[index + 0]));
      y    = (int16_t)(((uint16_t)data[index + 3] << 8) | ((uint16_t)data[index + 2]));
      size = (int16_t)(((uint16_t)data[index + 5] << 8) | ((uint16_t)data[index + 4]));
      conf = (int16_t)(((uint16_t)data[index + 7] << 8) | ((uint16_t)data[index + 6]));
      index += 8;
      sprintf(buf, "    Index:%d\t\tX:%d Y:%d Size:%d Confidence:%d\n", i, x, y, size, conf);
      Serial.print(buf);
      posX = x;   //書き足し部分
      SIZE = size;

    }

    //表情認識に利用
    if (0 != (execflag & EXPRESSION_ESTIMATION)) {
      int16_t neutral = 0, happiness = 0, surprise = 0, anger = 0, sadness = 0, degree = 0;
      neutral   = (int16_t)data[index + 0];
      happiness = (int16_t)data[index + 1];
      surprise  = (int16_t)data[index + 2];
      anger     = (int16_t)data[index + 3];
      sadness   = (int16_t)data[index + 4];
      degree    = (int16_t)data[index + 5];

      index += 6;
      sprintf(buf, "    Expression\t\tScore:%d, %d, %d, %d, %d Degree:%d\n", neutral, happiness, surprise, anger, sadness, degree);
      Serial.print(buf);
      HAPPINESS = happiness;   //書き足し部分
    }

    //性別判定に利用
    if (0 != (execflag & GENDER_ESTIMATION)){
      int16_t gender = 0, conf = 0;
      gender = (int16_t)data[index+0];
      conf   = (int16_t)(((uint16_t)data[index+2] << 8) | ((uint16_t)data[index+1]));
      index += 3;
      sprintf(buf, "    Gender\t\tGender:%d Confidence=%d\n", gender, conf);
      Serial.print(buf);
      GENDER = gender;  //書き足し部分
    }

    if (0 != (execflag & AGE_ESTIMATION)){
      int16_t age = 0, conf = 0;
      age  = (int16_t)data[index+0];
      conf = (int16_t)(((uint16_t)data[index+2] << 8) | ((uint16_t)data[index+1]));
      index += 3;
      sprintf(buf, "    Age\t\t\tAge:%d Confidence=%d\n", age, conf);
      Serial.print(buf);
      AGE = age;  //書き足し部分
    }
    return true;

  }
}

/************************************************************  各種定義  *********************************************************************/

// RGBLEDに出力するピン番号
//#define RGBLED_OUTPIN    15

// Arduinoにぶら下がっているRGBLEDの個数
#define NUMRGBLED        20

int touch_sensing_number = 9;
int timer = 0;

//サーボモータ(Servoオブジェクトの宣言)
Servo myservo1;
Servo myservo2;
Servo myservo3;
Servo myservo4;
Servo myservo5;

Adafruit_MPR121 MPR121;

Adafruit_MPR121 cap = Adafruit_MPR121();
// You can have up to 4 on one i2c bus but one is enough for testing!


//DCモータ
#define PIN_IN1  22
#define PIN_IN2  23
#define PIN_VREF 2 // PWM

#define PIN_IN3  24
#define PIN_IN4  25
#define PIN_VREF2 3 // PWM


//LED　ピン番号
#define LED1 42  //左腕
#define LED2 43
#define LED3 44

#define LED4 48 //設定なし

#define LED5 49 //尻尾
#define LED6 50

#define LED7 10  //身体・真ん中
#define LED8 11
#define LED9 12

#define LED10 16  //頭
#define LED11 17

#define LED12 45  //右腕
#define LED13 46
#define LED14 47

#define LED15 51  //身体・上
#define LED16 52
#define LED17 53

#define LED18 13  //身体・下
#define LED19 14
#define LED20 15

#define NUM_LEDS 20
#define BRIGHTNESS 50 //LED明るさ


//MP3　ピン番号
SoftwareSerial mp3(19, 18);

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;
unsigned long duration;

// RGBLEDのライブラリを生成する(色指定はRGBの並びで行う、LEDの速度は800KHzとする)
//Adafruit_NeoPixel RGBLED = Adafruit_NeoPixel(NUMRGBLED, RGBLED_OUTPIN, NEO_RGB + NEO_KHZ800);

Adafruit_NeoPixel RGBLED1 = Adafruit_NeoPixel(NUM_LEDS, LED1, NEO_GRBW + NEO_KHZ800);  //adafruit_NeoPixel(LEDの数, pin番号, A value indicating the type of NeoPixels that are connected. In most cases you can leave this off and pass just two arguments;)
Adafruit_NeoPixel RGBLED2 = Adafruit_NeoPixel(NUM_LEDS, LED2, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED3 = Adafruit_NeoPixel(NUM_LEDS, LED3, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED4 = Adafruit_NeoPixel(NUM_LEDS, LED4, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED5 = Adafruit_NeoPixel(NUM_LEDS, LED5, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED6 = Adafruit_NeoPixel(NUM_LEDS, LED6, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED7 = Adafruit_NeoPixel(NUM_LEDS, LED7, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED8 = Adafruit_NeoPixel(NUM_LEDS, LED8, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED9 = Adafruit_NeoPixel(NUM_LEDS, LED9, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED10 = Adafruit_NeoPixel(NUM_LEDS, LED10, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED11 = Adafruit_NeoPixel(NUM_LEDS, LED11, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED12 = Adafruit_NeoPixel(NUM_LEDS, LED12, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED13 = Adafruit_NeoPixel(NUM_LEDS, LED13, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED14 = Adafruit_NeoPixel(NUM_LEDS, LED14, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED15 = Adafruit_NeoPixel(NUM_LEDS, LED15, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED16 = Adafruit_NeoPixel(NUM_LEDS, LED16, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED17 = Adafruit_NeoPixel(NUM_LEDS, LED17, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED18 = Adafruit_NeoPixel(NUM_LEDS, LED18, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED19 = Adafruit_NeoPixel(NUM_LEDS, LED19, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel RGBLED20 = Adafruit_NeoPixel(NUM_LEDS, LED20, NEO_GRBW + NEO_KHZ800);

/*************************************************************ピン出力設定（起動時に実行）***************************************************************/
void init_PFC() {
  pinMode( 27, OUTPUT );
  pinMode( 28, OUTPUT );
  pinMode( 29, OUTPUT );
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
}

byte neopix_gamma[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};

/*
  byte neopix_gamma2[] = {
  10,  10,  10,  10,  20,  20,  20,  300,  300,  300,  400,  400,  400,  500,  500,  500,
  600,  600,  600,  700,  700,  700,  800,  800,  800,  900,  900,  900,  1000,  1000,  1000,  1100,
  1100,  1100,  1200,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2
  };
*/

int kk = 1; //1:緑　2:赤　3:青 4:黄（黄緑）5:水色　6:ピンク　7:紫　8:オレンジ 9:黄緑
int uu = 1;  //1:全身 2:体（正面全部） 3:頭　4:左腕　5:尻尾 6:右腕 7:尻尾＋左腕

int hana[16][3] = {{0, 0, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 150, 110},
  {255, 0, 255}, {0, 255, 255}, {150, 150, 255}, {255, 255, 0}, {255, 100, 0},
  {250, 250, 250}, {0, 200, 0}, {0, 200, 200}, {0, 0, 200}, {0, 0, 200}
};
/*
  int haru[16][3] = {{0, 0, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 150, 110},
  {255, 0, 255}, {0, 255, 255}, {150, 150, 255}, {255, 255, 0}, {255, 100, 0},
  {250, 250, 250}, {0, 200, 0}, {0, 200, 200}, {0, 0, 200}, {0, 0, 200}
  };
  int haya[16][3] = {{0, 0, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 150, 110},
  {255, 0, 255}, {0, 255, 255}, {150, 150, 255}, {255, 255, 0}, {255, 100, 0},
  {250, 250, 250}, {0, 200, 0}, {0, 200, 200}, {0, 0, 200}, {0, 0, 200}
  };
*/

/*****************************************************************************************************************************LEDの初期設定（起動時に実行）************************************************************************************************************************************************/
void setled() {
  delay(100) ;
  RGBLED1.begin();
  RGBLED1.setBrightness(BRIGHTNESS);  //明るさ調整
  RGBLED1.show();                     //光らせる
  delay(50) ;
  RGBLED2.begin();
  RGBLED2.setBrightness(BRIGHTNESS);
  RGBLED2.show();
  delay(50) ;
  RGBLED3.begin();
  RGBLED3.setBrightness(BRIGHTNESS);
  RGBLED3.show();
  delay(50) ;
  RGBLED6.begin();
  RGBLED6.setBrightness(BRIGHTNESS);
  RGBLED6.show();
  delay(50) ;
  RGBLED9.begin();
  RGBLED9.setBrightness(BRIGHTNESS);
  RGBLED9.show();
  delay(50) ;
  RGBLED8.begin();
  RGBLED8.setBrightness(BRIGHTNESS);
  RGBLED8.show();
  delay(50) ;
  RGBLED7.begin();
  RGBLED7.setBrightness(BRIGHTNESS);
  RGBLED7.show();
  delay(50) ;
  RGBLED4.begin();
  RGBLED4.setBrightness(BRIGHTNESS);
  RGBLED4.show();
  delay(50) ;
  RGBLED5.begin();
  RGBLED5.setBrightness(BRIGHTNESS);
  RGBLED5.show();
  delay(50) ;
  RGBLED10.begin();
  RGBLED10.setBrightness(BRIGHTNESS);
  RGBLED10.show();
  delay(50) ;
  RGBLED11.begin();
  RGBLED11.setBrightness(BRIGHTNESS);
  RGBLED11.show();
  delay(50) ;
  RGBLED12.begin();
  RGBLED12.setBrightness(BRIGHTNESS);
  RGBLED12.show();
  delay(50) ;
  RGBLED13.begin();
  RGBLED13.setBrightness(BRIGHTNESS);
  RGBLED13.show();
  delay(50) ;
  RGBLED14.begin();
  RGBLED14.setBrightness(BRIGHTNESS);
  RGBLED14.show();
  delay(50) ;
  RGBLED15.begin();
  RGBLED15.setBrightness(BRIGHTNESS);
  RGBLED15.show();
  delay(50) ;
  RGBLED16.begin();
  RGBLED16.setBrightness(BRIGHTNESS);
  RGBLED16.show();
  delay(50) ;
  RGBLED17.begin();
  RGBLED17.setBrightness(BRIGHTNESS);
  RGBLED17.show();
  delay(50) ;
  RGBLED18.begin();
  RGBLED18.setBrightness(BRIGHTNESS);
  RGBLED18.show();
  delay(50) ;
  RGBLED19.begin();
  RGBLED19.setBrightness(BRIGHTNESS);
  RGBLED19.show();
  delay(50) ;
  RGBLED20.begin();
  RGBLED20.setBrightness(BRIGHTNESS);
  RGBLED20.show();
  delay(500) ;
}

/*****************************************************************************************************************************静電容量センサの初期設定***************************************************************************************************************/
int mode_select_frag = 0;

void init_mpr() {
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test");

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");

  MPR121.setThresholds(12, 6);
}

/*****************************************************************************************************************************LEDの発光パターン（組み合わせ)***************************************************************************************************************/
void unit() {

  switch (uu) {
    case 0:
      break;

    case 1://全身（カラフル）
      RGBLED1.setPixelColor( 0, RGBLED1.Color( hana[kk][0] , hana[kk][1], hana[kk][2]));                  //setPixelColor(The pixel to set, 0 to 9.,24-bit RGB color value, 0xRRGGBB.)
      RGBLED2.setPixelColor( 0, RGBLED2.Color( hana[kk + 1][0] , hana[kk + 1][1], hana[kk + 1][2]) );
      RGBLED3.setPixelColor( 0, RGBLED3.Color( hana[kk + 2][0] , hana[kk + 2][1], hana[kk + 2][2]) );
      RGBLED4.setPixelColor( 0, RGBLED4.Color( hana[kk + 3][0] , hana[kk + 3][1], hana[kk + 3][2]) );
      RGBLED5.setPixelColor( 0, RGBLED5.Color( hana[kk + 4][0] , hana[kk + 4][1], hana[kk + 4][2]) );
      RGBLED6.setPixelColor( 0, RGBLED6.Color( hana[kk + 5][0] , hana[kk + 5][1], hana[kk + 5][2]) );
      RGBLED7.setPixelColor( 0, RGBLED7.Color( hana[kk + 6][0] , hana[kk + 6][1], hana[kk + 6][2]) );
      RGBLED8.setPixelColor( 0, RGBLED8.Color( hana[kk + 7][0] , hana[kk + 7][1], hana[kk + 7][2]) );
      RGBLED9.setPixelColor( 0, RGBLED9.Color( hana[kk + 8][0] , hana[kk + 8][1], hana[kk + 8][2]) );
      RGBLED10.setPixelColor( 0, RGBLED10.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED11.setPixelColor( 0, RGBLED11.Color(  hana[kk + 1][0] , hana[kk + 1][1], hana[kk + 1][2]) );
      RGBLED12.setPixelColor( 0, RGBLED12.Color(  hana[kk + 2][0] , hana[kk + 2][1], hana[kk + 2][2])  );
      RGBLED13.setPixelColor( 0, RGBLED13.Color( hana[kk + 3][0] , hana[kk + 3][1], hana[kk + 3][2]) );
      RGBLED14.setPixelColor( 0, RGBLED14.Color( hana[kk + 4][0] , hana[kk + 4][1], hana[kk + 4][2]));
      RGBLED15.setPixelColor( 0, RGBLED15.Color(  hana[kk + 5][0] , hana[kk + 5][1], hana[kk + 5][2]) );
      RGBLED16.setPixelColor( 0, RGBLED16.Color( hana[kk + 6][0] , hana[kk + 6][1], hana[kk + 6][2]) );
      RGBLED17.setPixelColor( 0, RGBLED17.Color( hana[kk + 7][0] , hana[kk + 7][1], hana[kk + 7][2]));
      RGBLED18.setPixelColor( 0, RGBLED18.Color(hana[kk + 8][0] , hana[kk + 8][1], hana[kk + 8][2])  );
      RGBLED19.setPixelColor( 0, RGBLED19.Color( hana[kk + 1][0] , hana[kk + 1][1], hana[kk + 1][2]) );
      RGBLED20.setPixelColor( 0, RGBLED20.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      LED_START();
      //  delay(1000) ;
      break;

    case 2://全身（一色）
      RGBLED1.setPixelColor( 0, RGBLED1.Color( hana[kk][0] , hana[kk][1], hana[kk][2]));
      RGBLED2.setPixelColor( 0, RGBLED2.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED3.setPixelColor( 0, RGBLED3.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED4.setPixelColor( 0, RGBLED4.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED5.setPixelColor( 0, RGBLED5.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED6.setPixelColor( 0, RGBLED6.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED7.setPixelColor( 0, RGBLED7.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED8.setPixelColor( 0, RGBLED8.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED9.setPixelColor( 0, RGBLED9.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED10.setPixelColor( 0, RGBLED10.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED11.setPixelColor( 0, RGBLED11.Color(  hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED12.setPixelColor( 0, RGBLED12.Color(  hana[kk][0] , hana[kk][1], hana[kk][2])  );
      RGBLED13.setPixelColor( 0, RGBLED13.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED14.setPixelColor( 0, RGBLED14.Color( hana[kk][0] , hana[kk][1], hana[kk][2]));
      RGBLED15.setPixelColor( 0, RGBLED15.Color(  hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED16.setPixelColor( 0, RGBLED16.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED17.setPixelColor( 0, RGBLED17.Color( hana[kk][0] , hana[kk][1], hana[kk][2]));
      RGBLED18.setPixelColor( 0, RGBLED18.Color(hana[kk][0] , hana[kk][1], hana[kk][2])  );
      RGBLED19.setPixelColor( 0, RGBLED19.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED20.setPixelColor( 0, RGBLED20.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      LED_START();
      //  delay(1000) ;
      break;

    case 3://身体正面
      RGBLED7.setPixelColor( 0, RGBLED7.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED8.setPixelColor( 0, RGBLED8.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED9.setPixelColor( 0, RGBLED9.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED15.setPixelColor( 0, RGBLED15.Color(  hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED16.setPixelColor( 0, RGBLED16.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED17.setPixelColor( 0, RGBLED17.Color( hana[kk][0] , hana[kk][1], hana[kk][2]));
      RGBLED18.setPixelColor( 0, RGBLED18.Color(hana[kk][0] , hana[kk][1], hana[kk][2])  );
      RGBLED19.setPixelColor( 0, RGBLED19.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED20.setPixelColor( 0, RGBLED20.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      LED_START();
      break;

    case 4://頭
      RGBLED10.setPixelColor( 0, RGBLED10.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED11.setPixelColor( 0, RGBLED11.Color(  hana[kk][0] , hana[kk][1], hana[kk][2]) );
      LED_START();
      break;

    case 5://尻尾
      RGBLED5.setPixelColor( 0, RGBLED5.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED6.setPixelColor( 0, RGBLED6.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      LED_START();
      break;

    case 6://両腕
      RGBLED1.setPixelColor( 0, RGBLED1.Color( hana[kk][0] , hana[kk][1], hana[kk][2]));
      RGBLED2.setPixelColor( 0, RGBLED2.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED3.setPixelColor( 0, RGBLED3.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED12.setPixelColor( 0, RGBLED12.Color(  hana[kk][0] , hana[kk][1], hana[kk][2])  );
      RGBLED13.setPixelColor( 0, RGBLED13.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED14.setPixelColor( 0, RGBLED14.Color( hana[kk][0] , hana[kk][1], hana[kk][2]));
      LED_START();
      break;

    case 7://右腕
      RGBLED12.setPixelColor( 0, RGBLED12.Color(hana[kk][0] , hana[kk][1], hana[kk][2])  );
      RGBLED13.setPixelColor( 0, RGBLED13.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED14.setPixelColor( 0, RGBLED14.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      LED_START();
      break;

    case 8://左腕
      RGBLED1.setPixelColor( 0, RGBLED1.Color( hana[kk][0] , hana[kk][1], hana[kk][2]));
      RGBLED2.setPixelColor( 0, RGBLED2.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      RGBLED3.setPixelColor( 0, RGBLED3.Color( hana[kk][0] , hana[kk][1], hana[kk][2]) );
      LED_START();
      break;

  }
  //delay(1000);
  duration = pulseIn(38, HIGH);
  // Serial.println(duration);
}

/*****************************************************************************************************************************LEDを点灯する関数***************************************************************************************************************/
void LED_START() {
  RGBLED1.show();
  RGBLED2.show();
  RGBLED3.show();
  RGBLED4.show();
  RGBLED5.show();
  RGBLED6.show();
  RGBLED7.show();
  RGBLED8.show();
  RGBLED9.show();
  RGBLED10.show();
  RGBLED11.show();
  RGBLED12.show();
  RGBLED13.show();
  RGBLED14.show();
  RGBLED15.show();
  RGBLED16.show();
  RGBLED17.show();
  RGBLED18.show();
  RGBLED19.show();
  RGBLED20.show();
}

/*****************************************************************************************************************************LEDを消灯する関数***************************************************************************************************************/
void LED_STOP() {
  RGBLED1.setPixelColor( 0, RGBLED1.Color( hana[0][0] , hana[0][1], hana[0][2]));
  RGBLED2.setPixelColor( 0, RGBLED2.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED3.setPixelColor( 0, RGBLED3.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED4.setPixelColor( 0, RGBLED4.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED5.setPixelColor( 0, RGBLED5.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED6.setPixelColor( 0, RGBLED6.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED7.setPixelColor( 0, RGBLED7.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED8.setPixelColor( 0, RGBLED8.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED9.setPixelColor( 0, RGBLED9.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED10.setPixelColor( 0, RGBLED10.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED11.setPixelColor( 0, RGBLED11.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED12.setPixelColor( 0, RGBLED12.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED13.setPixelColor( 0, RGBLED13.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED14.setPixelColor( 0, RGBLED14.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED15.setPixelColor( 0, RGBLED15.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED16.setPixelColor( 0, RGBLED16.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED17.setPixelColor( 0, RGBLED17.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED18.setPixelColor( 0, RGBLED18.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED19.setPixelColor( 0, RGBLED19.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  RGBLED20.setPixelColor( 0, RGBLED20.Color( hana[0][0] , hana[0][1], hana[0][2]) );
  LED_START();
}

/*****************************************************************************************************************************DCモータを動かす関数   (モーターの回転速度(0~255)※255はduty100%のためNG)***************************************************************************************************************/
/*停止*/
void teisi() {
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, LOW);
  delay(5000);
}

/*ブレーキ*/
void brake() {
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, HIGH);
  delay(2000);
}

/*前進*/
void mae() {
  //
  analogWrite(PIN_VREF, 120);
  analogWrite(PIN_VREF2, 120);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);
  delay(5000);
}

/*後退*/
void usiro() {
  analogWrite(PIN_VREF, 120);
  analogWrite(PIN_VREF2, 120);
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);
  delay(5000);
}

/*右前に一周*/
void migimae_kaiten() {
  analogWrite(PIN_VREF, 120);
  analogWrite(PIN_VREF2, 120);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);
  delay(9200);
}

/*左前に一周*/
void hidarimae_kaiten() {
  analogWrite(PIN_VREF, 120);
  analogWrite(PIN_VREF2, 120);
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);
  delay(9200);
}

/*右前カーブ*/
void migi_senkai() {
  analogWrite(PIN_VREF, 180);
  analogWrite(PIN_VREF2, 60);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);
  delay(5000);
}

/*左前カーブ*/
void hidari_senkai() {
  analogWrite(PIN_VREF, 60);
  analogWrite(PIN_VREF2, 180);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);
  delay(5000);
}

/*8の字走行*/
void hati() {
  analogWrite(PIN_VREF, 60);
  analogWrite(PIN_VREF2, 180);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);
  delay(2500);
  teisi();
  analogWrite(PIN_VREF, 180);
  analogWrite(PIN_VREF2, 60);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);
  delay(2500);
}

/*****************************************************************************************************************************サーボモータ設定***************************************************************************************************************/
int pos, pos2;
int move_speed = 10;

/*****************************************************************************************************************************サーボモータ右腕*************************************************************************************************************/
void servo_righthand() {
  myservo1.attach(8); //8番ピンをサーボモーターの出力ピンとして設定*/

  for (pos = 90; pos <= 150 ; pos += 1) {
    myservo1.write(pos);
    delay(move_speed);
  }
  for (pos = 150; pos >= 30 ; pos -= 1) {
    myservo1.write(pos);
    delay(move_speed);
  }
  for (pos = 30; pos <= 90; pos += 1) {
    myservo1.write(pos);
    delay(move_speed);
  }

  myservo1.detach();
}

/*****************************************************************************************************************************サーボモータ左腕***************************************************************************************************************/
void servo_lefthand() {
  myservo2.attach(7); //7番ピンをサーボモーターの出力ピンとして設定*/

  for (pos = 90; pos >= 30 ; pos -= 1) {
    myservo2.write(pos);
    delay(move_speed);
  }
  for (pos = 30; pos <= 150 ; pos += 1) {
    myservo2.write(pos);
    delay(move_speed);
  }
  for (pos = 150; pos >= 90; pos -= 1) {
    myservo2.write(pos);
    delay(move_speed);
  }

  myservo2.detach();
}

/*****************************************************************************************************************************サーボモータ両手同時動作*************************************************************************************************************/
void servo_bothhand() {
  myservo1.attach(8); //8番ピンをサーボモーターの出力ピンとして設定*/
  myservo2.attach(7); //7番ピンをサーボモーターの出力ピンとして設定*/

  pos2 = 90;
  for (pos = 90; pos <= 150 ; pos += 1) {
    myservo1.write(pos);
    pos2 = pos2 - 1;
    myservo2.write(pos2);
    delay(move_speed);
  }
  pos2 = 30;
  for (pos = 150; pos >= 30 ; pos -= 1) {
    myservo1.write(pos);
    pos2 = pos2 + 1;
    myservo2.write(pos2);
    delay(move_speed);
  }
  pos2 = 150;
  for (pos = 30; pos <= 90; pos += 1) {
    myservo1.write(pos);
    pos2 = pos2 - 1;
    myservo2.write(pos2);
    delay(move_speed);
  }

  myservo1.detach();
  myservo2.detach();
}

/*****************************************************************************************************************************サーボモータ両手交互動作***************************************************************************************************************/
void servo_mutualhand() {
  myservo1.attach(8); //8番ピンをサーボモーターの出力ピンとして設定*/
  myservo2.attach(7); //7番ピンをサーボモーターの出力ピンとして設定*/

  for (pos = 90; pos <= 150 ; pos += 1) {
    myservo1.write(pos);
    myservo2.write(pos);
    delay(move_speed);
  }
  for (pos = 150; pos >= 30 ; pos -= 1) {
    myservo1.write(pos);
    myservo2.write(pos);
    delay(move_speed);
  }
  for (pos = 30; pos <= 90; pos += 1) {
    myservo1.write(pos);
    myservo2.write(pos);
    delay(move_speed);
  }

  myservo1.detach();
  myservo2.detach();
}

/*****************************************************************************************************************************サーボモータ尻尾***************************************************************************************************************/
void servo_tale() {
  myservo5.attach(4); //4番ピンをサーボモーターの出力ピンとして設定*/

  for (pos = 90; pos >= 30 ; pos -= 1) {
    myservo5.write(pos);
    delay(move_speed);
  }
  for (pos = 30; pos <= 150 ; pos += 1) {
    myservo5.write(pos);
    delay(move_speed);
  }
  for (pos = 150; pos >= 90; pos -= 1) {
    myservo5.write(pos);
    delay(move_speed);
  }

  myservo5.detach();
}

/*****************************************************************************************************************************サーボモータ首***************************************************************************************************************/
void servo_neck() {
  myservo3.attach(6); //6番ピンをサーボモーターの出力ピンとして設定

  for (pos = 60; pos >= 0 ; pos -= 1) {
    myservo3.write(pos);
    delay(move_speed);
  }
  for (pos = 0; pos <= 120 ; pos += 1) {
    myservo3.write(pos);
    delay(move_speed);
  }
  for (pos = 120; pos >= 60; pos -= 1) {
    myservo3.write(pos);
    delay(move_speed);
  }

  myservo3.detach();
}

/*****************************************************************************************************************************サーボモータ頭***************************************************************************************************************/
void servo_head() {
  myservo4.attach(5); //5番ピンをサーボモーターの出力ピンとして設定

  for (pos = 45; pos >= 15 ; pos -= 1) {
    myservo4.write(pos);
    delay(move_speed);
  }
  for (pos = 15; pos <= 75 ; pos += 1) {
    myservo4.write(pos);
    delay(move_speed);
  }
  for (pos = 75; pos >= 45; pos -= 1) {
    myservo4.write(pos);
    delay(move_speed);
  }

  myservo4.detach();
}

/*****************************************************************************************************************************サーボモータの位置の初期化***************************************************************************************************************/
void servo_init_posi() {
  myservo1.attach(8); //8番ピンをサーボモーターの出力ピンとして設定*/
  myservo2.attach(7); //7番ピンをサーボモーターの出力ピンとして設定*/
  myservo5.attach(4); //4番ピンをサーボモーターの出力ピンとして設定*/
  myservo3.attach(6); //6番ピンをサーボモーターの出力ピンとして設定
  myservo4.attach(5); //5番ピンをサーボモーターの出力ピンとして設定

  myservo1.write(90);
  myservo2.write(90);
  myservo5.write(90);
  myservo3.write(60);
  myservo4.write(45);
  delay(500);

  myservo1.detach();
  myservo2.detach();
  myservo5.detach();
  myservo3.detach();
  myservo4.detach();
}

/*****************************************************************************************************************************温度を検知する関数***************************************************************************************************************/
int ans , temp , tv ; //温度計測用の変数

void Temp_sensing() {
  //マルチプレクサ出力切り換え
  digitalWrite( 27, LOW );
  digitalWrite( 28, LOW );
  digitalWrite( 29, LOW );
  delay(10);
  ans = analogRead(0) ; //アナログ０番ピンからセンサ値を読込む
  tv = map(ans, 0, 1023, 0, 5000) ; //センサ値を電圧に変換する
  temp = map(tv, 300, 1600, -30, 100) ; //電圧から温度に変換する(LM61は-30度から100度まで計測)
  Serial.print("\n温度=");
  Serial.print(temp); //温度を表示
  Serial.print("度\n");
}

/********************************************************************************************************************タッチを検知する関数(静電容量センサ)***********************************************************************************************/
int count_t = 0;
int count_r = 0;

void Touch_sensing() {
  Serial.println("チェック④");
  // Get the currently touched pads
  currtouched = cap.touched();

  //検知するタッチセンサの番号(3～9）
  for (touch_sensing_number = 3; touch_sensing_number < 10; touch_sensing_number++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(touch_sensing_number)) && !(lasttouched & _BV(touch_sensing_number)) ) {   //https://qiita.com/autumn-position/items/ac016c58190f77f66a13
      Serial.print(touch_sensing_number); Serial.println(" touched");
      Serial.println("LED点灯");
      //unit();
      mode_select_frag = 1;
      Serial.println(touch_sensing_number);
      break;
    }

    // if it *was* touched and now *isnt*, alert!
    else if (!(currtouched & _BV(touch_sensing_number)) && (lasttouched & _BV(touch_sensing_number)) ) {
      Serial.print(touch_sensing_number); Serial.println(" released");
      // LED_STOP();
    }
  }

  // reset our state
  lasttouched = currtouched;

  // comment out this line for detailed data from the sensor!
  return;

  // put a delay so it isn't overwhelming
  // delay(100);
}

/*******************************************感情表現*******************************************/
int nn;

/*基本状態*/
void feel_calm() {
  kk = 1;  //緑
  uu = 2;
  unit();
  delay(5000);
  LED_STOP();
  kk = 0;
  uu = 0;
}

/*楽しい*/
void feel_happy_1() {
  kk = 8;
  uu = 2;
  unit();
  mp3.listen();
  SpecifyfolderPlay(8, 13);

  analogWrite(PIN_VREF, 120);
  analogWrite(PIN_VREF2, 120);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);

  myservo1.attach(8); //8番ピンをサーボモーターの出力ピンとして設定*/
  myservo2.attach(7); //7番ピンをサーボモーターの出力ピンとして設定*/
  myservo5.attach(4); //4番ピンをサーボモーターの出力ピンとして設定*/

  for (nn = 0; nn < 3; nn++) {
    for (pos = 90; pos <= 150 ; pos += 1) {
      myservo1.write(pos);
      myservo2.write(pos);
      myservo5.write(pos);
      delay(move_speed);
    }
    for (pos = 150; pos >= 30 ; pos -= 1) {
      myservo1.write(pos);
      myservo2.write(pos);
      myservo5.write(pos);
      delay(move_speed);
    }
    for (pos = 30; pos <= 90; pos += 1) {
      myservo1.write(pos);
      myservo2.write(pos);
      myservo5.write(pos);
      delay(move_speed);
    }
  }

  myservo1.detach();
  myservo2.detach();
  myservo5.detach();

  teisi();
  LED_STOP();
  servo_init_posi();
}

void feel_happy_2() {
  kk = 8;
  uu = 2;
  unit();

  mp3.listen();
  SpecifyfolderPlay(8, 21);

  analogWrite(PIN_VREF, 120);
  analogWrite(PIN_VREF2, 120);
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);

  myservo1.attach(8); //8番ピンをサーボモーターの出力ピンとして設定*/
  myservo2.attach(7); //7番ピンをサーボモーターの出力ピンとして設定*/
  myservo5.attach(4); //6番ピンをサーボモーターの出力ピンとして設定*/

  for (nn = 0; nn < 2; nn++) {
    for (pos = 90; pos <= 150 ; pos += 1) {
      myservo1.write(pos);
      myservo2.write(pos);
      myservo5.write(pos);
      delay(move_speed);
    }
    for (pos = 150; pos >= 30 ; pos -= 1) {
      myservo1.write(pos);
      myservo2.write(pos);
      myservo5.write(pos);
      delay(move_speed);
    }
    for (pos = 30; pos <= 90; pos += 1) {
      myservo1.write(pos);
      myservo2.write(pos);
      myservo5.write(pos);
      delay(move_speed);
    }
  }

  myservo1.detach();
  myservo2.detach();
  myservo5.detach();

  teisi();
  LED_STOP();
  servo_init_posi();
}

void feel_happy_3() {
  kk = 2;
  uu = 2;
  unit();

  mp3.listen();
  SpecifyfolderPlay(8, 24);

  analogWrite(PIN_VREF, 120);
  analogWrite(PIN_VREF2, 120);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);

  myservo1.attach(8); //8番ピンをサーボモーターの出力ピンとして設定*/
  myservo2.attach(7); //7番ピンをサーボモーターの出力ピンとして設定*/
  myservo3.attach(6); //6番ピンをサーボモーターの出力ピンとして設定

  for (nn = 0; nn < 2; nn++) {
    pos2 = 60;
    for (pos = 90; pos <= 150 ; pos += 1) {
      myservo1.write(pos);
      myservo2.write(pos);
      pos2 = pos2 - (1);
      myservo3.write(pos2);
      delay(move_speed);
    }
    pos2 = 0;
    for (pos = 150; pos >= 30 ; pos -= 1) {
      myservo1.write(pos);
      myservo2.write(pos);
      pos2 = pos2 + (1);
      myservo3.write(pos2);
      delay(move_speed);
    }
    pos2 = 120;
    for (pos = 30; pos <= 90; pos += 1) {
      myservo1.write(pos);
      myservo2.write(pos);
      pos2 = pos2 - (1);
      myservo3.write(pos2);
      delay(move_speed);
    }
  }

  myservo1.detach();
  myservo2.detach();
  myservo3.detach();

  teisi();
  LED_STOP();
  servo_init_posi();
}

/*悲しみ*/
void feel_sad_1() {
  kk = 3;
  uu = 2;
  unit();

  mp3.listen();
  SpecifyfolderPlay(8, 15);

  analogWrite(PIN_VREF, 120);
  analogWrite(PIN_VREF2, 120);
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);

  myservo3.attach(6); //6番ピンをサーボモーターの出力ピンとして設定

  for (nn = 0; nn < 2; nn++) {
    for (pos = 60; pos >= 0 ; pos -= 1) {
      myservo3.write(pos);
      delay(move_speed);
    }
    for (pos = 0; pos <= 120 ; pos += 1) {
      myservo3.write(pos);
      delay(move_speed);
    }
    for (pos = 120; pos >= 0; pos -= 1) {
      myservo3.write(pos);
      delay(move_speed);
    }
    for (pos = 0; pos <= 60 ; pos += 1) {
      myservo3.write(pos);
      delay(move_speed);
    }
  }

  myservo3.detach();

  teisi();
  LED_STOP();
  servo_init_posi();
}

void feel_sad_2() {
  kk = 3;
  uu = 2;
  unit();

  mp3.listen();
  SpecifyfolderPlay(8, 8);

  analogWrite(PIN_VREF, 120);
  analogWrite(PIN_VREF2, 120);
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);

  myservo3.attach(6); //6番ピンをサーボモーターの出力ピンとして設定


  for (nn = 0; nn < 2; nn++) {
    for (pos = 60; pos >= 0 ; pos -= 1) {
      myservo3.write(pos);
      delay(move_speed);
    }
    for (pos = 0; pos <= 120 ; pos += 1) {
      myservo3.write(pos);
      delay(move_speed);
    }
    for (pos = 120; pos >= 0; pos -= 1) {
      myservo3.write(pos);
      delay(move_speed);
    }
    for (pos = 0; pos <= 60 ; pos += 1) {
      myservo3.write(pos);
      delay(move_speed);
    }
  }

  myservo3.detach();

  teisi();
  LED_STOP();
  servo_init_posi();
}

void feel_sad_3() {
  kk = 5;
  uu = 2;
  unit();

  mp3.listen();
  SpecifyfolderPlay(8, 9);

  analogWrite(PIN_VREF, 120);
  analogWrite(PIN_VREF2, 120);
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);

  myservo1.attach(8); //8番ピンをサーボモーターの出力ピンとして設定*/
  myservo2.attach(7); //7番ピンをサーボモーターの出力ピンとして設定*/
  myservo3.attach(6); //6番ピンをサーボモーターの出力ピンとして設定

  myservo1.write(150);
  myservo2.write(30);


  for (nn = 0; nn < 3; nn++) {
    for (pos = 60; pos >= 0 ; pos -= 1) {
      myservo3.write(pos);
      delay(move_speed);
    }
    for (pos = 0; pos <= 120 ; pos += 1) {
      myservo3.write(pos);
      delay(move_speed);
    }
    for (pos = 120; pos >= 60; pos -= 1) {
      myservo3.write(pos);
      delay(move_speed);
    }
  }
  myservo3.detach();

  teisi();
  LED_STOP();
  servo_init_posi();
}

/**********************************************************************************************************search(人探し)**********************************************************************************************************/
void search() 
{
  int right_or_left = 0;
  int flag = 0;

  FACE_COUNT = 0;
  
  while (flag == 0)
  {
    while(FACE_COUNT == 0)
    {
      LED_STOP();
      right_or_left = random(1, 3); //左右どっちに回るかランダム
     
      if (right_or_left == 1)   //左回り
      {
        analogWrite(PIN_VREF, 200);
        analogWrite(PIN_VREF2, 200);
        digitalWrite(PIN_IN1, HIGH);
        digitalWrite(PIN_IN2, LOW);
        digitalWrite(PIN_IN3, LOW);
        digitalWrite(PIN_IN4, HIGH);
        delay(1000);
        teisi();
        mySerial.listen();
        Execute(execFlag, imageno);
      }

      else  //右回り
      {
        analogWrite(PIN_VREF, 200);
        analogWrite(PIN_VREF2, 200);
        digitalWrite(PIN_IN1, LOW);
        digitalWrite(PIN_IN2, HIGH);
        digitalWrite(PIN_IN3, HIGH);
        digitalWrite(PIN_IN4, LOW);
        delay(1000);
        teisi();
        mySerial.listen();
        Execute(execFlag, imageno);
      }
    }
    
    //前進
    analogWrite(PIN_VREF, 200);
    analogWrite(PIN_VREF2, 200);
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, HIGH);
    digitalWrite(PIN_IN3, LOW);
    digitalWrite(PIN_IN4, HIGH);
    delay(2000);
    teisi();

    uu = 2; //光らせ方
    kk = 5; //光らせる色
    unit();
    FACE_COUNT = 0;
    mySerial.listen();
    Execute(execFlag, imageno);

    if((0 < posX) && (posX < 700))
    {
      analogWrite(PIN_VREF, 200);
      analogWrite(PIN_VREF2, 200);
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);
      digitalWrite(PIN_IN3, LOW);
      digitalWrite(PIN_IN4, HIGH);
      delay(500);
      teisi();
    }

    else if((900 < posX) && (posX < 1599))
    {
      analogWrite(PIN_VREF, 200);
      analogWrite(PIN_VREF2, 200);
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH);
      digitalWrite(PIN_IN3, HIGH);
      digitalWrite(PIN_IN4, LOW);
      delay(1000);
      teisi();
    }

    /*FACE_COUNT = 0;
    mySerial.listen();
    Execute(execFlag, imageno);*/
    
    if(SIZE > 300)
    {
      flag = 1;
    }
  }
}

/**********************************************************************************************************遊びの定義**********************************************************************************************************/
int kaisuu;

/********************************************************************************************************遊びA　タッチ遊び********************************************************************************************************/
void gameA_start() {
  //DCモータとLED起動
  //頭：赤　身体：緑　右腕：紫　尻尾：青　左腕：黄

  //左腕
  RGBLED1.setPixelColor( 0, RGBLED1.Color( hana[8][0] , hana[8][1], hana[8][2]) );
  RGBLED2.setPixelColor( 0, RGBLED2.Color( hana[8][0] , hana[8][1], hana[8][2]) );
  RGBLED3.setPixelColor( 0, RGBLED3.Color( hana[8][0] , hana[8][1], hana[8][2]) );

  //指定なし
  RGBLED4.setPixelColor( 0, RGBLED4.Color( hana[4][0] , hana[4][1], hana[4][2]) );

  //尻尾
  RGBLED5.setPixelColor( 0, RGBLED5.Color( hana[3][0] , hana[3][1], hana[3][2]) );
  RGBLED6.setPixelColor( 0, RGBLED6.Color( hana[3][0] , hana[3][1], hana[3][2]) );

  //身体・中
  RGBLED7.setPixelColor( 0, RGBLED7.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED8.setPixelColor( 0, RGBLED8.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED9.setPixelColor( 0, RGBLED9.Color( hana[1][0] , hana[1][1], hana[1][2]) );

  //頭
  RGBLED10.setPixelColor( 0, RGBLED10.Color( hana[2][0] , hana[2][1], hana[2][2]) );
  RGBLED11.setPixelColor( 0, RGBLED11.Color( hana[2][0] , hana[2][1], hana[2][2]) );

  //右腕
  RGBLED12.setPixelColor( 0, RGBLED12.Color( hana[7][0] , hana[7][1], hana[7][2]) );
  RGBLED13.setPixelColor( 0, RGBLED13.Color( hana[7][0] , hana[7][1], hana[7][2]) );
  RGBLED14.setPixelColor( 0, RGBLED14.Color( hana[7][0] , hana[7][1], hana[7][2]) );

  //身体・上下
  RGBLED15.setPixelColor( 0, RGBLED15.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED16.setPixelColor( 0, RGBLED16.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED17.setPixelColor( 0, RGBLED17.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED18.setPixelColor( 0, RGBLED18.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED19.setPixelColor( 0, RGBLED19.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED20.setPixelColor( 0, RGBLED20.Color( hana[1][0] , hana[1][1], hana[1][2]) );

  LED_START();

  analogWrite(PIN_VREF, 60);
  analogWrite(PIN_VREF2, 180);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);

}


void gameA() {
  int tp;
  int kaisuu_true = 0;
  int kaisuu_false = 0;
  int gameA_count;
  int touch_game_score = 0;
  int nA = 0;
  int nnA = 0;
  int touch_place;
  int touch_place_sound;
  int touch_place_place;

  mySerial.listen();
  HVC_GetVersion();

  for (gameA_count = 0; gameA_count < 3; gameA_count ++) {

    touch_place = random( 6 );
    touch_place_sound = touch_place + 42;
    touch_place_place = touch_place + 4;
    mode_select_frag = 0;
    gameA_start();

    mp3.listen();
    SpecifyfolderPlay(8, 47); //mp3再生「色を触ってね」
    delay(2000);

    while ( nA < 3 ) {
      mp3.listen();
      SpecifyfolderPlay(8, touch_place_sound); //mp3再生（フォルダ，番号）

      while ( nnA < 250 ) {
        Touch_sensing();
        if (mode_select_frag == 1) {
          nA = 3;
          break;
        }
        nnA++;
      }

      nnA = 0;
      nA++;
    }
    nA = 0;

    if (mode_select_frag == 1) {
      //感圧センサでモード分岐
      if (touch_sensing_number == touch_place_place) {
        mp3.listen();
        SpecifyfolderPlay(8, 49);//「◎」
        delay(3000);
        mp3.listen();
        SpecifyfolderPlay(8, 48);//「正解！」
        delay(1500);
        kaisuu_true++;
        feel_happy_2();  //喜ぶ2
        touch_sensing_number = 0;
      }

      else {
        mp3.listen();
        SpecifyfolderPlay(8, 50);//「×」
        delay(1500);
        kaisuu_false++;
        feel_sad_2();//悲しむ2
        touch_sensing_number = 0;
      }
    }
    else {
      //接触しなかったら
      kaisuu_false++;
      feel_sad_3();//悲しむ3
      touch_sensing_number = 0;
    }
  }
  if ( kaisuu_true > 2 ) {
    feel_happy_1();
  }
  else if (kaisuu_false > 2 ) {
    feel_sad_1();
  }
  else {
    delay(50);
  }
}

/********************************************************************************************************遊びB　音階ドレミ********************************************************************************************************/

void gameB() {

  int gameB_count;
  int gameB_count_2;
  int touch_count = 0;
  int tp;
  int nB = 0;
  int nnB = 0;
  int array_B[10][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {34, 5}, {36, 7}, {35, 6}, {31, 8}, {32, 2}, {33, 3}};

  mySerial.listen();
  HVC_GetVersion();

  for (gameB_count = 0; gameB_count < 3; gameB_count ++) {
    mode_select_frag = 0;
    kk = 1;
    uu = 1;
    unit();
    analogWrite(PIN_VREF, 60);
    analogWrite(PIN_VREF2, 180);
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, HIGH);
    digitalWrite(PIN_IN3, LOW);
    digitalWrite(PIN_IN4, HIGH);

    while ( nB < 3 ) {
      mp3.listen();
      SpecifyfolderPlay(8, 40); //mp3再生（フォルダ，番号）//「好きなところを触ってね」
      delay(1000);

      while ( nnB < 250 ) {
        Touch_sensing();
        if (mode_select_frag == 1) {
          nB = 3;
          break;
        }
        nnB++;
      }

      nnB = 0;
      nB++;
    }
    nB = 0;

    if (mode_select_frag == 1) {
      mp3.listen();
      SpecifyfolderPlay(8, array_B[touch_sensing_number][0]); //押された箇所によってドレミファソラシドのどれかがなる
      kk = array_B[touch_sensing_number][1];//色
      uu = 2;
      unit();
      touch_count++;
      delay(1000);
      LED_STOP();
      touch_sensing_number = 0;
      delay(500);
    }
  }

  if ( touch_count == 0 ) {
    feel_sad_1();
  }
  else if ( touch_count == 1 ) {
    feel_sad_2();
  }
  else if ( touch_count == 2) {
    feel_happy_2();
  }
  else if ( touch_count == 3) {
    feel_happy_1();
  }
}

/********************************************************************************************************遊びC  音に合わせて踊る（5曲中1曲ランダムに流れる）********************************************************************************************************/

void gameC() {
  int music_servo;
  int nC;
  int music_list = random( 6 );
  mySerial.listen();
  HVC_GetVersion();

  int array_C[5][4] = {{27, 3, 8, 8}, {26, 2, 7, 7}, {29, 4, 5, 6}, {30, 2, 7, 8}, {28, 2, 3, 8}};

  mp3.listen();
  SpecifyfolderPlay(8, array_C[music_list][0]); //mp3再生（フォルダ，番号）

  RGBLED1.setPixelColor( 0, RGBLED1.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );
  RGBLED2.setPixelColor( 0, RGBLED2.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );
  RGBLED3.setPixelColor( 0, RGBLED3.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );

  RGBLED4.setPixelColor( 0, RGBLED4.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );

  RGBLED5.setPixelColor( 0, RGBLED5.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );
  RGBLED6.setPixelColor( 0, RGBLED6.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );

  RGBLED7.setPixelColor( 0, RGBLED7.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );
  RGBLED8.setPixelColor( 0, RGBLED8.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );
  RGBLED9.setPixelColor( 0, RGBLED9.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );

  RGBLED10.setPixelColor( 0, RGBLED10.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );
  RGBLED11.setPixelColor( 0, RGBLED11.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );

  RGBLED12.setPixelColor( 0, RGBLED12.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );
  RGBLED13.setPixelColor( 0, RGBLED13.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );
  RGBLED14.setPixelColor( 0, RGBLED14.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );

  RGBLED15.setPixelColor( 0, RGBLED15.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );
  RGBLED16.setPixelColor( 0, RGBLED16.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );
  RGBLED17.setPixelColor( 0, RGBLED17.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );
  RGBLED18.setPixelColor( 0, RGBLED18.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );
  RGBLED19.setPixelColor( 0, RGBLED19.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );
  RGBLED20.setPixelColor( 0, RGBLED20.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );
  LED_START();

  analogWrite(PIN_VREF, 180);
  analogWrite(PIN_VREF2, 60);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);

  myservo1.attach(8); //8番ピンをサーボモーターの出力ピンとして設定
  myservo2.attach(7); //7番ピンをサーボモーターの出力ピンとして設定
  myservo3.attach(6); //4番ピンをサーボモーターの出力ピンとして設定
  myservo5.attach(4); //4番ピンをサーボモーターの出力ピンとして設定

  for ( music_servo = 0; music_servo < 8; music_servo++ ) {
    pos2 = 60;
    for (pos = 90; pos <= 150 ; pos += 1) {
      myservo1.write(pos);
      myservo2.write(pos);
      myservo5.write(pos);
      pos2 = pos2 - (1);
      myservo3.write(pos2);
      delay(move_speed);
    }

    pos2 = 0;
    LED_STOP();
    RGBLED1.setPixelColor( 0, RGBLED1.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );
    RGBLED2.setPixelColor( 0, RGBLED2.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );
    RGBLED3.setPixelColor( 0, RGBLED3.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );

    RGBLED4.setPixelColor( 0, RGBLED4.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );

    RGBLED5.setPixelColor( 0, RGBLED5.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );
    RGBLED6.setPixelColor( 0, RGBLED6.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );

    RGBLED7.setPixelColor( 0, RGBLED7.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );
    RGBLED8.setPixelColor( 0, RGBLED8.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );
    RGBLED9.setPixelColor( 0, RGBLED9.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );

    RGBLED10.setPixelColor( 0, RGBLED10.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );
    RGBLED11.setPixelColor( 0, RGBLED11.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );

    RGBLED12.setPixelColor( 0, RGBLED12.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );
    RGBLED13.setPixelColor( 0, RGBLED13.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );
    RGBLED14.setPixelColor( 0, RGBLED14.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );

    RGBLED15.setPixelColor( 0, RGBLED15.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );
    RGBLED16.setPixelColor( 0, RGBLED16.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );
    RGBLED17.setPixelColor( 0, RGBLED17.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );
    RGBLED18.setPixelColor( 0, RGBLED18.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );
    RGBLED19.setPixelColor( 0, RGBLED19.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );
    RGBLED20.setPixelColor( 0, RGBLED20.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );
    LED_START();

    for (pos = 150; pos >= 30 ; pos -= 1) {
      myservo1.write(pos);
      myservo2.write(pos);
      myservo5.write(pos);
      pos2 = pos2 + (1);
      myservo3.write(pos2);
      delay(move_speed);
    }

    pos2 = 120;
    LED_STOP();
    RGBLED1.setPixelColor( 0, RGBLED1.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );
    RGBLED2.setPixelColor( 0, RGBLED2.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );
    RGBLED3.setPixelColor( 0, RGBLED3.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );

    RGBLED4.setPixelColor( 0, RGBLED4.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );

    RGBLED5.setPixelColor( 0, RGBLED5.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );
    RGBLED6.setPixelColor( 0, RGBLED6.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );

    RGBLED7.setPixelColor( 0, RGBLED7.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );
    RGBLED8.setPixelColor( 0, RGBLED8.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );
    RGBLED9.setPixelColor( 0, RGBLED9.Color( hana[array_C[music_list][1]][0] , hana[array_C[music_list][1]][1], hana[array_C[music_list][1]][2]) );

    RGBLED10.setPixelColor( 0, RGBLED10.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );
    RGBLED11.setPixelColor( 0, RGBLED11.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );

    RGBLED12.setPixelColor( 0, RGBLED12.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );
    RGBLED13.setPixelColor( 0, RGBLED13.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );
    RGBLED14.setPixelColor( 0, RGBLED14.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );

    RGBLED15.setPixelColor( 0, RGBLED15.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );
    RGBLED16.setPixelColor( 0, RGBLED16.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );
    RGBLED17.setPixelColor( 0, RGBLED17.Color( hana[array_C[music_list][2]][0] , hana[array_C[music_list][2]][1], hana[array_C[music_list][2]][2]) );
    RGBLED18.setPixelColor( 0, RGBLED18.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );
    RGBLED19.setPixelColor( 0, RGBLED19.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );
    RGBLED20.setPixelColor( 0, RGBLED20.Color( hana[array_C[music_list][3]][0] , hana[array_C[music_list][3]][1], hana[array_C[music_list][3]][2]) );
    LED_START();

    for (pos = 30; pos <= 90; pos += 1) {
      myservo1.write(pos);
      myservo2.write(pos);
      myservo5.write(pos);
      pos2 = pos2 - (1);
      myservo3.write(pos2);
      delay(move_speed);

    }
  }

  myservo1.detach();
  myservo2.detach();
  myservo5.detach();
  myservo3.detach();

  LED_STOP();
  teisi();
}

/********************************************************************************************************遊びD (riddle, なぞなぞ)********************************************************************************************************/
void gameD_start() {
  //DCモータとLED起動
  //頭：赤　身体：緑　右腕：紫　尻尾：青　左腕：黄

  //左腕
  RGBLED1.setPixelColor( 0, RGBLED1.Color( hana[8][0] , hana[8][1], hana[8][2]) );
  RGBLED2.setPixelColor( 0, RGBLED2.Color( hana[8][0] , hana[8][1], hana[8][2]) );
  RGBLED3.setPixelColor( 0, RGBLED3.Color( hana[8][0] , hana[8][1], hana[8][2]) );

  //指定なし
  RGBLED4.setPixelColor( 0, RGBLED4.Color( hana[4][0] , hana[4][1], hana[4][2]) );

  //尻尾
  RGBLED5.setPixelColor( 0, RGBLED5.Color( hana[3][0] , hana[3][1], hana[3][2]) );
  RGBLED6.setPixelColor( 0, RGBLED6.Color( hana[3][0] , hana[3][1], hana[3][2]) );

  //身体・中
  RGBLED7.setPixelColor( 0, RGBLED7.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED8.setPixelColor( 0, RGBLED8.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED9.setPixelColor( 0, RGBLED9.Color( hana[1][0] , hana[1][1], hana[1][2]) );

  //頭
  RGBLED10.setPixelColor( 0, RGBLED10.Color( hana[2][0] , hana[2][1], hana[2][2]) );
  RGBLED11.setPixelColor( 0, RGBLED11.Color( hana[2][0] , hana[2][1], hana[2][2]) );

  //右腕
  RGBLED12.setPixelColor( 0, RGBLED12.Color( hana[7][0] , hana[7][1], hana[7][2]) );
  RGBLED13.setPixelColor( 0, RGBLED13.Color( hana[7][0] , hana[7][1], hana[7][2]) );
  RGBLED14.setPixelColor( 0, RGBLED14.Color( hana[7][0] , hana[7][1], hana[7][2]) );

  //身体・上下
  RGBLED15.setPixelColor( 0, RGBLED15.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED16.setPixelColor( 0, RGBLED16.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED17.setPixelColor( 0, RGBLED17.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED18.setPixelColor( 0, RGBLED18.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED19.setPixelColor( 0, RGBLED19.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED20.setPixelColor( 0, RGBLED20.Color( hana[1][0] , hana[1][1], hana[1][2]) );

  LED_START();

  analogWrite(PIN_VREF, 60);
  analogWrite(PIN_VREF2, 180);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);

}

void gameD() {
  int tp;
  int kaisuu_true = 0;
  int kaisuu_false = 0;
  int gameD_count;
  int touch_game_score = 0;
  int nD = 0;
  int nnD = 0;
  int riddle_num;

  mySerial.listen();
  HVC_GetVersion();

  for (gameD_count = 0; gameD_count < 3; gameD_count ++) {

    riddle_num = random( 9 );
    gameA_start();

    mp3.listen();
    SpecifyfolderPlay(8, 47); //mp3再生「色を触ってね」
    delay(2000);

    while ( nD < 3 ) {
      mp3.listen();
      SpecifyfolderPlay(8, touch_place_sound); //mp3再生（フォルダ，番号）

      while ( nnD < 250 ) {
        Touch_sensing();
        if (mode_select_frag == 1) {
          nD = 3;
          break;
        }
        nnD++;
      }

      nnD = 0;
      nD++;
    }
    nD = 0;
  }
}

/*****************************************************************************************************************************遊びE (gokko, ごっこ遊び)***************************************************************************************************************/
void gameE_start() {
  //DCモータとLED起動
  //頭：赤　身体：緑　右腕：紫　尻尾：青　左腕：黄

  //左腕
  RGBLED1.setPixelColor( 0, RGBLED1.Color( hana[8][0] , hana[8][1], hana[8][2]) );
  RGBLED2.setPixelColor( 0, RGBLED2.Color( hana[8][0] , hana[8][1], hana[8][2]) );
  RGBLED3.setPixelColor( 0, RGBLED3.Color( hana[8][0] , hana[8][1], hana[8][2]) );

  //指定なし
  RGBLED4.setPixelColor( 0, RGBLED4.Color( hana[4][0] , hana[4][1], hana[4][2]) );

  //尻尾
  RGBLED5.setPixelColor( 0, RGBLED5.Color( hana[3][0] , hana[3][1], hana[3][2]) );
  RGBLED6.setPixelColor( 0, RGBLED6.Color( hana[3][0] , hana[3][1], hana[3][2]) );

  //身体・中
  RGBLED7.setPixelColor( 0, RGBLED7.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED8.setPixelColor( 0, RGBLED8.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED9.setPixelColor( 0, RGBLED9.Color( hana[1][0] , hana[1][1], hana[1][2]) );

  //頭
  RGBLED10.setPixelColor( 0, RGBLED10.Color( hana[2][0] , hana[2][1], hana[2][2]) );
  RGBLED11.setPixelColor( 0, RGBLED11.Color( hana[2][0] , hana[2][1], hana[2][2]) );

  //右腕
  RGBLED12.setPixelColor( 0, RGBLED12.Color( hana[7][0] , hana[7][1], hana[7][2]) );
  RGBLED13.setPixelColor( 0, RGBLED13.Color( hana[7][0] , hana[7][1], hana[7][2]) );
  RGBLED14.setPixelColor( 0, RGBLED14.Color( hana[7][0] , hana[7][1], hana[7][2]) );

  //身体・上下
  RGBLED15.setPixelColor( 0, RGBLED15.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED16.setPixelColor( 0, RGBLED16.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED17.setPixelColor( 0, RGBLED17.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED18.setPixelColor( 0, RGBLED18.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED19.setPixelColor( 0, RGBLED19.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED20.setPixelColor( 0, RGBLED20.Color( hana[1][0] , hana[1][1], hana[1][2]) );

  LED_START();

  analogWrite(PIN_VREF, 60);
  analogWrite(PIN_VREF2, 180);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);

}

void gameE() {
  int tp;
  int kaisuu_true = 0;
  int kaisuu_false = 0;
  int gameE_count;
  int touch_game_score = 0;
  int nE = 0;
  int nnE = 0;
  int riddle_num;

  mySerial.listen();
  HVC_GetVersion();

  for (gameE_count = 0; gameE_count < 3; gameE_count ++) {

    riddle_num = random( 9 );
    gameE_start();

    mp3.listen();
    SpecifyfolderPlay(8, 47); //mp3再生「色を触ってね」
    delay(2000);

    while ( nE < 3 ) {
      mp3.listen();
      SpecifyfolderPlay(8, touch_place_sound); //mp3再生（フォルダ，番号）

      while ( nnE < 250 ) {
        Touch_sensing();
        if (mode_select_frag == 1) {
          nE = 3;
          break;
        }
        nnE++;
      }

      nnE = 0;
      nE++;
    }
    nE = 0;
  }
}

/*****************************************************************************************************************************遊びF (yomikikase, 読み聞かせ)***************************************************************************************************************/
void gameF_start() {
  //DCモータとLED起動
  //頭：赤　身体：緑　右腕：紫　尻尾：青　左腕：黄

  //左腕
  RGBLED1.setPixelColor( 0, RGBLED1.Color( hana[8][0] , hana[8][1], hana[8][2]) );
  RGBLED2.setPixelColor( 0, RGBLED2.Color( hana[8][0] , hana[8][1], hana[8][2]) );
  RGBLED3.setPixelColor( 0, RGBLED3.Color( hana[8][0] , hana[8][1], hana[8][2]) );

  //指定なし
  RGBLED4.setPixelColor( 0, RGBLED4.Color( hana[4][0] , hana[4][1], hana[4][2]) );

  //尻尾
  RGBLED5.setPixelColor( 0, RGBLED5.Color( hana[3][0] , hana[3][1], hana[3][2]) );
  RGBLED6.setPixelColor( 0, RGBLED6.Color( hana[3][0] , hana[3][1], hana[3][2]) );

  //身体・中
  RGBLED7.setPixelColor( 0, RGBLED7.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED8.setPixelColor( 0, RGBLED8.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED9.setPixelColor( 0, RGBLED9.Color( hana[1][0] , hana[1][1], hana[1][2]) );

  //頭
  RGBLED10.setPixelColor( 0, RGBLED10.Color( hana[2][0] , hana[2][1], hana[2][2]) );
  RGBLED11.setPixelColor( 0, RGBLED11.Color( hana[2][0] , hana[2][1], hana[2][2]) );

  //右腕
  RGBLED12.setPixelColor( 0, RGBLED12.Color( hana[7][0] , hana[7][1], hana[7][2]) );
  RGBLED13.setPixelColor( 0, RGBLED13.Color( hana[7][0] , hana[7][1], hana[7][2]) );
  RGBLED14.setPixelColor( 0, RGBLED14.Color( hana[7][0] , hana[7][1], hana[7][2]) );

  //身体・上下
  RGBLED15.setPixelColor( 0, RGBLED15.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED16.setPixelColor( 0, RGBLED16.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED17.setPixelColor( 0, RGBLED17.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED18.setPixelColor( 0, RGBLED18.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED19.setPixelColor( 0, RGBLED19.Color( hana[1][0] , hana[1][1], hana[1][2]) );
  RGBLED20.setPixelColor( 0, RGBLED20.Color( hana[1][0] , hana[1][1], hana[1][2]) );

  LED_START();

  analogWrite(PIN_VREF, 60);
  analogWrite(PIN_VREF2, 180);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);

}

void gameF() {
  int tp;
  int kaisuu_true = 0;
  int kaisuu_false = 0;
  int gameF_count;
  int touch_game_score = 0;
  int nF = 0;
  int nnF = 0;
  int riddle_num;

  mySerial.listen();
  HVC_GetVersion();

  for (gameF_count = 0; gameF_count < 3; gameF_count ++) {

    riddle_num = random( 9 );
    gameF_start();

    mp3.listen();
    SpecifyfolderPlay(8, 47); //mp3再生「色を触ってね」
    delay(2000);

    while ( nF < 3 ) {
      mp3.listen();
      SpecifyfolderPlay(8, touch_place_sound); //mp3再生（フォルダ，番号）

      while ( nnF < 250 ) {
        Touch_sensing();
        if (mode_select_frag == 1) {
          nF = 3;
          break;
        }
        nnF++;
      }

      nnF = 0;
      nF++;
    }
    nF = 0;
  }
}

/*****************************************************************************************************************************起動時に実行される関数***************************************************************************************************************/
void setup() {

  mp3.begin( 9600 );
  mySerial.begin( 9600 );
  Serial.begin( 9600 );
  randomSeed(analogRead(0));
  init_PFC(); //ピン出力設定

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
  delay(100);
  SelectPlayerDevice(0x02);       // SDカードを指定
  SetVolume(0x04);                 //スピーカーの音量を設定(本番は0x10)
  init_mpr(); //静電容量センサ初期化

  setled(); //LED初期化

  randomSeed(analogRead(0));

  /*前回のデータを出力*/
  delay(10000);
  for (times = 0; times < 1000; times++) {
    data_aaa = EEPROM.read(times);
    Serial.print ( times );
    Serial.print ( "：" );
    Serial.print ( data_aaa );
    Serial.print ( "\n" );
  }
  delay(5000);

  /*EEPROM初期化*/
  for (times = 0; times < 1000; times++) {

    EEPROM.write(times, 255);

  }
  times = 0;
  search();
  Serial.write("set_up") //raspberry pi にset up が終わったことを通知
}

/*****************************************************************************************************************************メイン関数***************************************************************************************************************/
void loop()
{

  double Score_A, Score_B, Score_C, Score_D, Score_ABC;
  double prob_A, prob_B, prob_C, prob_AB ;

  /*   実行確率1：1：1でA，B，Cの文字を表示する　*/
  /*  A,B,Cのケースに対し，三段階で評価  */
  /*  1,0.6,0.2でweightをつける  */
  int n;
  int a = 0, b = 0, c = 0;

  for ( n = 0; n < 6; n++ ) {

    int h = random( 3 );

    if (h == 0) {
      servo_init_posi();
      gameA();  //遊びAを行う．
      times++;
      A_times++;
      Score_A = (A_happy * 100) / (A_happy + A_un_happy);

      /*  Conclusion 一桁目*/
      if ( Score_A >= 0 && Score_A <= 50) {
        C1 = 0;
      }
      else {
        C1 = 1;
      }

      /*  Conclusion 二桁目*/
      A_kao = A_un_happy + A_happy;

      if ( A_kao >= 0 && A_kao < 9) {
        C2  = A_kao;
      }
      else {
        C2 = 9;
      }

      /*  Conclusion 三桁目は 1  */
      C3 = 1;
      Conclusion = C1 * 100 + C2 * 10 + C3;

      EEPROM.write( times , Conclusion );

      A_happy = 0;
      A_un_happy = 0;
      A_kao = 0;

      break;
    }

    else if (h == 1) {
      servo_init_posi();
      gameB();  //遊びBを行う．
      times++;
      B_times++;
      Score_B = (B_happy * 100) / (B_happy + B_un_happy);

      /*  Conclusion 一桁目*/
      if ( Score_B >= 0 && Score_B <= 50) {
        C1 = 0;
      }
      else {
        C1 = 1;
      }

      /*  Conclusion 二桁目*/
      B_kao = B_un_happy + B_happy;

      if ( B_kao >= 0 && B_kao < 9) {
        C2  = B_kao;
      }
      else {
        C2 = 9;
      }

      /*  Conclusion 三桁目は 1  */
      C3 = 2;
      Conclusion = C1 * 100 + C2 * 10 + C3;

      EEPROM.write( times , Conclusion );

      B_happy = 0;
      B_un_happy = 0;
      B_kao = 0;

      break;
    }

    else if (h == 2) {
      servo_init_posi();
      gameC();  //遊びCを行う．
      times++;
      C_times++;
      Score_C = (C_happy * 100) / (C_happy + C_un_happy);

      /*  Conclusion 一桁目*/
      if ( Score_C >= 0 && Score_C <= 50) {
        C1 = 0;
      }
      else {
        C1 = 1;
      }

      /*  Conclusion 二桁目*/
      C_kao = C_un_happy + C_happy;

      if ( C_kao >= 0 && C_kao < 9) {
        C2  = C_kao;
      }
      else {
        C2 = 9;
      }

      /*  Conclusion 三桁目は 1  */
      C3 = 3;
      Conclusion = C1 * 100 + C2 * 10 + C3;

      EEPROM.write( times , Conclusion );

      C_happy = 0;
      C_un_happy = 0;
      C_kao = 0;


      break;
    }

    /*else if (h == 3) {
      servo_init_posi();
      gameD();  //遊びDを行う．
      times++;
      D_times++;
      Score_D = (D_happy * 100) / (D_happy + D_un_happy);

      //  Conclusion 一桁目
      if ( Score_D >= 0 && Score_D <= 50) {
        C1 = 0;
      }
      else {
        C1 = 1;
      }

      //  Conclusion 二桁目
      D_kao = D_un_happy + D_happy;

      if ( D_kao >= 0 && D_kao < 9) {
        C2  = D_kao;
      }
      else {
        C2 = 9;
      }

      //  Conclusion 三桁目は 1  
      C3 = 3;
      Conclusion = C1 * 100 + C2 * 10 + C3;

      EEPROM.write( times , Conclusion );

      D_happy = 0;
      D_un_happy = 0;
      D_kao = 0;


      break;
    }
*/
    /***********************************************************************************************************************************************************************************************************************************************/

    Score_ABC = Score_A + Score_B + Score_C;

    prob_A = Score_A * 7 / Score_ABC + 1;  //実行確率０にはならない計算
    prob_B = Score_B * 7 / Score_ABC + 1;
    prob_C = Score_C * 7 / Score_ABC + 1;
    Score_A = 0;
    Score_B = 0;
    Score_C = 0;

    /***********************************************************************************************************************************************************************************************************************************************/

    int m;
    for ( m = 0; m < 10; m++ ) { //m(max)の値*10=全体で行う総実験数（遊びの回数）としてm(max)の値を変更．
      //実装する際は，乱数3に戻ってしまわないように，大きい値にしといたほうが良い．
      int l;

      for ( l = 0; l < 10; l++ ) {

        int i = random( 1 , 11 );

        if ( 0 < i && i <= prob_A ) {
          servo_init_posi();
          gameA();  //遊びAを行う．
          times++;
          A_times++;
          Score_A = (A_happy * 100) / (A_happy + A_un_happy);

          /*  Conclusion 一桁目*/
          if ( Score_A >= 0 && Score_A <= 50) {
            C1 = 0;
          }
          else if ( Score_A > 50 && Score_A <= 100) {
            C1 = 1;
          }

          /*  Conclusion 二桁目*/
          A_kao = A_un_happy + A_happy;

          if ( A_kao >= 0 && A_kao < 9) {
            C2  = A_kao;
          }
          else {
            C2 = 9;
          }

          /*  Conclusion 三桁目は 1  */
          C3 = 1;
          Conclusion = C1 * 100 + C2 * 10 + C3;

          EEPROM.write( times , Conclusion );

          A_happy = 0;
          A_un_happy = 0;
          A_kao = 0;

          break;
        }

        else if ( prob_A < i && i <= (prob_A + prob_B) ) {
          servo_init_posi();
          gameB();  //遊びBを行う．
          times++;
          B_times++;
          Score_B = (B_happy * 100) / (B_happy + B_un_happy);

          /*  Conclusion 一桁目*/
          if ( Score_B >= 0 && Score_B <= 50) {
            C1 = 0;
          }
          else if ( Score_B > 50 && Score_B <= 100) {
            C1 = 1;
          }

          /*  Conclusion 二桁目*/
          B_kao = B_un_happy + B_happy;

          if ( B_kao >= 0 && B_kao < 9) {
            C2  = B_kao;
          }
          else {
            C2 = 9;
          }

          /*  Conclusion 三桁目は 1  */
          C3 = 2;
          Conclusion = C1 * 100 + C2 * 10 + C3;

          EEPROM.write( times , Conclusion );

          B_happy = 0;
          B_un_happy = 0;
          B_kao = 0;

          break;
        }

        else if ( prob_AB < i && i <= 10) {
          servo_init_posi();
          gameC();  //遊びCを行う．
          times++;
          C_times++;
          Score_C = (C_happy * 100) / (C_happy + C_un_happy);

          /*  Conclusion 一桁目*/
          if ( Score_C >= 0 && Score_C <= 50) {
            C1 = 0;
          }
          else if ( Score_C > 50 && Score_C <= 100) {
            C1 = 1;
          }

          /*  Conclusion 二桁目*/
          C_kao = C_un_happy + C_happy;

          if ( C_kao >= 0 && C_kao < 9) {
            C2  = C_kao;
          }
          else {
            C2 = 9;
          }

          /*  Conclusion 三桁目は 1  */
          C3 = 3;
          Conclusion = C1 * 100 + C2 * 10 + C3;

          EEPROM.write( times , Conclusion );

          C_happy = 0;
          C_un_happy = 0;
          C_kao = 0;

          break;
        }


        Score_ABC = Score_A + Score_B + Score_C;

        prob_A = Score_A * 10 / Score_ABC;
        prob_B = Score_B * 10 / Score_ABC;
        prob_C = Score_C * 10 / Score_ABC;
        Score_A = 0;
        Score_B = 0;
        Score_C = 0;

      }
    }
  }
}
