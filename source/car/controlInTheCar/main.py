#!/usr/bin/env python
import time
import argparse
from pmw3901 import PMW3901, BG_CS_FRONT_BCM, BG_CS_BACK_BCM
import os
import math
import serial

OPTICAL_KEISUU = 0.188679245
OPTICAL_HANKEI = 50
ENSHU = 2 * OPTICAL_HANKEI * math.pi


# Pick the right class for the specified breakout
SensorClass = PMW3901 



# ロボットの操作を行うクラス
class DriveTheCar:
    def __init__(self):
        self.ser = serial.Serial('/dev/ttyACM0', 9600)
        self.fwd = 254 #220
        self.bwd = 0 #34

    def move_forward(self):
        # print("前進")
        # ser.write(bytes('f', 'utf-8'))
        self.ser.write(bytes([255, self.fwd,  self.fwd, 255]))

    def move_backward(self):
        # print("後進")
        # ser.write(bytes('b', 'utf-8'))
        self.ser.write(bytes([255, self.bwd, self.bwd, 255]))

    def turn_right(self):
        # print("右回転")
        # ser.write(bytes('r', 'utf-8'))
        self.ser.write(bytes([255, self.fwd, self.bwd, 255]))

    def turn_left(self):
        # print("左回転")
        # ser.write(bytes('l', 'utf-8'))
        self.ser.write(bytes([255, self.bwd,  self.fwd, 255]))

    def move_stop(self):
        # print("停止")
        # ser.write(bytes('s', 'utf-8'))
        self.ser.write(bytes([255, 127,  127, 255]))

# ロボット自身の動きを制御するクラス
class ControlTheCar:
    def __init__(self):
        # ロボットの座標/角度を初期化
        # 単位 : mm
        self.ax = 0
        self.ay = 0
        # self.dx = 0
        self.aangle = 90
        
        # 動作中のみ保持する座標/角度を初期化
        # 単位 : mm
        self.tx = 0
        self.ty = 0

        self.speed = 0

        # センサーをセットアップ
        self.flo = SensorClass(spi_port=0, spi_cs_gpio=BG_CS_FRONT_BCM)
        self.flo.set_rotation(0)    # Rotation of sensor in degrees
                                    # choices=[0, 90, 180, 270]

        # 制御クラスをインスタンス化
        self.drive = DriveTheCar()

        # 経過時間を計測するために開始時刻を保存
        self.start = time.time()
    
    def goto(self, x, y):
        angle = self.__howManyTimesDoIHaveToTurn(x, y)
        print("曲がるよ!" + str(angle))
        self.__turn(angle)
        distance = self.__howManyMove(x, y)
        print("行くよ" + str(distance))
        self.__move(distance)
        self.__chosei(x, y)
    
    # ロボットの回転制御
    def __turn(self, angle):
        # 回転開始前に初期化
        self.tx = 0
        if angle < 0:
            while (-(angle)) > self.__xToAngle(self.tx):
                self.drive.turn_left()
                x, y = self.__motion()
                self.__update_dx(x)
            self.drive.move_stop()
            print("左"+str(self.tx))
        elif angle > 0:
            while angle > self.__xToAngle(self.tx):
                self.drive.turn_right()
                x, y = self.__motion()
                self.__update_dx(x)
            self.drive.move_stop()
            print("右"+str(self.tx))
        else:
            pass
        print("絶対角度" + str(self.aangle)+"絶対座標"+str(self.ax)+" "+str(self.ay))

    def __move(self, distance):
        self.ty = 0
        while distance > self.ty:
            self.drive.move_forward()
            x, y = self.__motion()
            self.__update_txty(y)
        self.drive.move_stop()
    
    # 座標で止まる
    def __chosei(self, x, y):
        self.ty = 0
        if x > 0:
            while x <= self.ax:
                self.drive.move_forward()
                x, y = self.__motion()
                self.__update_txty(y)
        else:
            while x >= self.ax:
                self.drive.move_forward()
                x, y = self.__motion()
                self.__update_txty(y)
        self.drive.move_stop()
        if y > 0:
            while y <= self.ay:
                self.drive.move_forward()
                x, y = self.__motion()
                self.__update_txty(y)
        else:
            while y >= self.ay:
                self.drive.move_forward()
                x, y = self.__motion()
                self.__update_txty(y)
        self.drive.move_stop()
    
    # 経過時間を秒で返す
    def __get_elapsed_time(self):
        return time.time() - self.start
    
    # オプティカルフローセンサの値取得
    def __motion(self):
        try:
            mx, my = self.flo.get_motion()
            x = mx * OPTICAL_KEISUU
            y = my * OPTICAL_KEISUU
            self.tx += x
            self.ty += y
            time.sleep(0.01)
            return x, y
        except RuntimeError:
            return 0, 0

    # ロボットの座標/角度を更新する
    def __update_txty(self, y):
        self.ax += y * math.cos(math.radians(self.aangle))
        self.ay += y * math.sin(math.radians(self.aangle))
    
    # 総円周と角度を計算
    def __update_dx(self, x):
        '''
        self.dx += x
        if self.dx > ENSHU:
            self.dx -= ENSHU
        elif self.dx < (-ENSHU):
            self.dx += ENSHU
        '''
        angle = self.__xToAngle(x) 
        self.aangle += angle if x < 0 else -angle
    
    # 何度回れば目標座標に向くかを計算する(+/-(0~180))
    def __howManyTimesDoIHaveToTurn(self, x, y):
        dif_x = x - self.ax
        dif_y = y - self.ay
        dif_rotation = math.degrees(math.atan2(dif_y, dif_x)) - self.aangle
        
        if dif_rotation > 180:
            dif_rotation -= 360
        elif dif_rotation < -180:
            dif_rotation += 360
        return dif_rotation

    # 進む距離を計算する
    def __howManyMove(self, x, y):
        dif_x = x - self.tx
        dif_y = y - self.ty
        return math.sqrt(dif_x ** 2 + dif_y ** 2)

    # 円周(X座標)から角度を計算する(戻り値は必ず+)
    def __xToAngle(self, x):
        angle = (abs(x) * 360) / (2 * OPTICAL_HANKEI * math.pi)
        return  angle


#shiken = DriveTheCar()
#for i in range(3):
#    shiken.move_forward()
#    time.sleep(1)

try:
    control = ControlTheCar()
    control.goto(-500, 500)
    control.goto(500, 1000)
    control.goto(0, 0)
except KeyboardInterrupt:
    pass

print("終了")
