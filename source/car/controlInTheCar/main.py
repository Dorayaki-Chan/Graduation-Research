#!/usr/bin/env python
import time
import argparse
from pmw3901 import PMW3901, BG_CS_FRONT_BCM, BG_CS_BACK_BCM
import os
import math
import serial

OPTICAL_KEISUU = 0.188679245
OPTICAL_HANKEI = 60
ENSHU = 2 * OPTICAL_HANKEI * math.pi


# Pick the right class for the specified breakout
SensorClass = PMW3901 

flo = SensorClass(spi_port=0, spi_cs_gpio=BG_CS_FRONT_BCM)
flo.set_rotation(0)     # Rotation of sensor in degrees
                        # choices=[0, 90, 180, 270]

# ロボットの操作を行うクラス
class DriveTheCar:
    def __init__(self):
        self.ser = serial.Serial('/dev/ttyACM0', 9600)
        self.fwd = 0
        self.bwd = 254

    def move_forward(self):
        print("前進")
        # ser.write(bytes('f', 'utf-8'))
        self.ser.write(bytes([255, self.fwd,  self.fwd, 255]))

    def move_backward(self):
        print("後進")
        # ser.write(bytes('b', 'utf-8'))
        self.ser.write(bytes([255, self.bwd, self.bwd, 255]))

    def turn_right(self):
        print("右回転")
        # ser.write(bytes('r', 'utf-8'))
        self.ser.write(bytes([255, self.fwd, self.bwd, 255]))

    def turn_left(self):
        print("左回転")
        # ser.write(bytes('l', 'utf-8'))
        self.ser.write(bytes([255, self.bwd,  self.fwd, 255]))

    def move_stop(self):
        print("停止")
        # ser.write(bytes('s', 'utf-8'))
        self.ser.write(bytes([255, 127,  127, 255]))

# ロボット自身の動きを制御するクラス
class ControlTheCar:
    def __init__(self):
        # ロボットの座標/角度を初期化
        self.tx = 0
        self.ty = 0
        self.angle = 90
        
        # 動作中のみ保持する座標/角度を初期化
        self.x = 0
        self.y = 0

        # 永続的に円周を保存するための変数
        self.dx = 0

        self.speed = 0

        # 制御クラスをインスタンス化
        self.drive = DriveTheCar()

        # 経過時間を計測するために開始時刻を保存
        self.start = time.time()
    
    def __turn(self, angle):
        self.x = 0
        if angle < 0:
            while (-(angle)) > self.__xToAngle(self.x):
                x, y = self.__motion()
                self.drive.turn_right()
                self.__update_dx(x)
            self.drive.move_stop()
        elif angle > 0:
            while angle > self.__xToAngle(self.x):
                x, y = self.__motion()
                self.__motion()
                self.drive.turn_left()
                self.__update_dx(x)
            self.drive.move_stop()
        else:
            pass

    def __move(self, distance):
        self.y = 0
        while distance > self.y * OPTICAL_KEISUU:
            x, y = self.__motion()
            self.drive.move_forward()
            self.__update_txty(y)
        self.drive.move_stop()
    
    # 経過時間を秒で返す
    def __get_elapsed_time(self):
        return time.time() - self.start
        
    def __motion(self):
        try:
            x, y = flo.get_motion()
            self.x += x
            self.y += y 
            time.sleep(0.01)
            return x, y
        except RuntimeError:
            pass

    # ロボットの座標/角度を更新する
    def __update_txty(self, y):
        self.tx += y * math.cos(math.radians(self.__xToAngle(self.dx)))
        self.ty += y * math.sin(math.radians(self.__xToAngle(self.dx)))
    
    def __update_dx(self, x):
        self.dx += x
        dx = self.dx * OPTICAL_KEISUU
        if dx > ENSHU:
            self.dx -= ENSHU / OPTICAL_KEISUU
        elif dx < (-ENSHU):
            self.dx += ENSHU / OPTICAL_KEISUU
    
    # 何度回れば目標座標に向くかを計算する(+/-(0~180))
    def __howManyTimesDoIHaveToTurn(self, x, y):
        dif_x = x - self.tx
        dif_y = y - self.ty

        dif_rotation = math.degrees(math.atan2(dif_y, dif_x)) - self.__xToAngle(self.dx)
        
        if dif_rotation > 180:
            dif_rotation -= 360
        elif dif_rotation < -180:
            dif_rotation += 360
        return dif_rotation

    def __howManyMove(self, x, y):
        dif_x = x - self.tx
        dif_y = y - self.ty

        return math.sqrt(dif_x ** 2 + dif_y ** 2)
    
    def goto(self, x, y):
        angle = self.__howManyTimesDoIHaveToTurn(x, y)
        self.__turn(angle)
        distance = self.__howManyMove(x, y)
        self.__move(distance)


    # 円周(X座標)から角度を計算する
    def __xToAngle(self, x):
        angle = (abs(x) * OPTICAL_KEISUU * 360) / (2 * math.pi * OPTICAL_HANKEI)
        return  angle


try:
    control = ControlTheCar()
    control.goto(500, 500)

except KeyboardInterrupt:
    pass

print("終了")
