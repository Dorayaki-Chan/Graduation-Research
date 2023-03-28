/*
内容:
	madgwickフィルタによる角度算出withMPU
情報:2021/09/14 N.Hirota

アドレス:
なんぞや:
https://qiita.com/Ninagawa_Izumi/items/9520bad3c78ee40194fc
注意事項:地磁気なしの6軸
TODO:

*/
//================================================================//
//  AE-BMX055             Arduino UNO                             //
//    VCC                    +5V                                  //
//    GND                    GND                                  //
//    SDA                    A4(SDA)                              //
//    SCL                    A5(SCL)                              //
//                                                                //
//   (JP4,JP5,JP6はショートした状態)                                //
//   http://akizukidenshi.com/catalog/g/gK-13010/                 //
//================================================================//

#include "MPU6050.h"
MPU6050 accelgyro;
#include <MadgwickAHRS.h>
Madgwick MadgwickFilter;

int16_t ax, ay, az;//加速度　int16_tは2バイトの符号付き整数
int16_t gx, gy, gz;//角速度　同上
float ROLL, PITCH, YAW;

void setup()
{
  Wire.begin();
  Serial.begin(115200);
  accelgyro.initialize();//I2Cデバイスの初期化
  delay(300);
  MadgwickFilter.begin(500);//フィルタのサンプリングを100Hzで
}

void loop()
{
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  MadgwickFilter.updateIMU(gx / 131.0, gy / 131.0, gz / 131.0, ax / 16384.0, ay / 16384.0, az / 16384.0);
  ROLL = MadgwickFilter.getRoll();
  PITCH = MadgwickFilter.getPitch();
  YAW  = MadgwickFilter.getYaw();
  Serial.print(ROLL); Serial.print(",");
  Serial.print(PITCH); Serial.print(",");
  Serial.print(YAW);
  Serial.print("\n");
  delay(10);
}