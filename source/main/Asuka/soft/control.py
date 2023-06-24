#!/usr/bin/env python

"""
内容:
	あすかを指定座標へ向かわせるクラス群
情報:2023/06/23  K.Matsumoto
"""
import time
from pmw3901 import PMW3901, BG_CS_FRONT_BCM, BG_CS_BACK_BCM
import math
import serial
import log

OPTICAL_KEISUU = 0.188679245
OPTICAL_HANKEI = 155
ENSHU = 2 * OPTICAL_HANKEI * math.pi

# Pick the right class for the specified breakout
SensorClass = PMW3901 

class DriveTheCar:
    """ロボットの動きを制御するクラス"""
    def __init__(self):
        self.ser = serial.Serial('/dev/ttyACM0', 9600)
        self.fwd = 254
        self.bwd = 0
        self.rfwd = 200
        self.rbwd = 54

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
        self.ser.write(bytes([255, self.rfwd, self.rbwd, 255]))

    def turn_left(self):
        # print("左回転")
        # ser.write(bytes('l', 'utf-8'))
        self.ser.write(bytes([255, self.rbwd,  self.rfwd, 255]))

    def move_stop(self):
        # print("停止")
        # ser.write(bytes('s', 'utf-8'))
        self.ser.write(bytes([255, 127,  127, 255]))

class ControlTheCar:
    """ロボットの動きを決定するクラス"""
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
        self.flo.set_rotation(180)    # Rotation of sensor in degrees
                                    # choices=[0, 90, 180, 270]

        # 制御クラスをインスタンス化
        self.drive = DriveTheCar()

        # ログクラスをインスタンス化
        self.logs = log.Logs()
        self.logs.start()
    
    def goto(self, x, y):
        """指定座標へ移動する

        引数:
            x: x座標
            y: y座標
        """
        angle = self.__howManyTimesDoIHaveToTurn(x, y)
        print("曲がるよ!" + str(angle))
        self.logs.addPoint(self.ax, self.ay, self.aangle, self.tx, self.ty, 0, "回転:"+str(angle)+"度")
        self.__turn(angle)
        distance = self.__howManyMove(x, y)
        print("行くよ" + str(distance))
        self.logs.addPoint(self.ax, self.ay, self.aangle, self.tx, self.ty, 0, "直進:"+str(distance)+"mm")
        self.__move(distance)
        print("絶対角度" + str(self.aangle)+"絶対座標"+str(self.ax)+" "+str(self.ay))
        self.logs.addPoint(self.ax, self.ay, self.aangle, self.tx, self.ty, 0, "完了")
    
    def get180(self):
        """180度回転する(魔法の係数確認用)"""
        self.__turn(180)
    
    def get500(self):
        """500mm前進する(魔法の係数確認用)"""
        self.__move(500)
    
    def __turn(self, angle):
        """指定角度回転する"""
        # 回転開始前に初期化
        self.tx = 0
        if angle < 0:
            while (-(angle)) > self.__xToAngle(self.tx):
                self.drive.turn_left()
                x, y = self.__motion()
                self.__update_dx(x)
                self.logs.addAll(self.ax, self.ay, self.aangle, self.tx, self.ty, self.__xToAngle(self.tx))
            self.drive.move_stop()
            self.logs.addPoint(self.ax, self.ay, self.aangle, self.tx, self.ty, 0, "右旋回:"+str(self.__xToAngle(self.tx))+"度")
            print("左"+str(self.__xToAngle(self.tx)))
        elif angle > 0:
            while angle > self.__xToAngle(self.tx):
                self.drive.turn_right()
                x, y = self.__motion()
                self.__update_dx(x)
                self.logs.addAll(self.ax, self.ay, self.aangle, self.tx, self.ty, self.__xToAngle(self.tx))
            self.drive.move_stop()
            self.logs.addPoint(self.ax, self.ay, self.aangle, self.tx, self.ty, 0, "左旋回:"+str(self.__xToAngle(self.tx))+"度")
            print("右"+str(self.__xToAngle(self.tx)))
        else:
            pass
        self.tx = 0

    def __move(self, distance):
        """指定距離移動する"""
        self.ty = 0
        while distance > self.ty:
            self.drive.move_forward()
            x, y = self.__motion()
            self.__update_txty(y)
            self.logs.addAll(self.ax, self.ay, self.aangle, self.tx, self.ty, self.__xToAngle(self.tx))
        self.drive.move_stop()
        self.logs.addPoint(self.ax, self.ay, self.aangle, self.tx, self.ty, 0, "直進:"+str(self.ty)+"mm")
        self.ty = 0
    
    def __motion(self):
        """オプティカルフローセンサからロボットの動きを取得する"""
        try:
            mx, my = self.flo.get_motion()
            x = -1 * mx * OPTICAL_KEISUU
            y = my * OPTICAL_KEISUU
            self.tx += x
            self.ty += y
            time.sleep(0.01)
            return x, y
        except RuntimeError:
            return 0, 0

    def __update_txty(self, y):
        """ロボットの座標/角度を更新する"""
        self.ax += y * math.cos(math.radians(self.aangle))
        self.ay += y * math.sin(math.radians(self.aangle))
    
    def __update_dx(self, x):
        """#総円周と角度を計算する
        
        TODO:
            現在は動く都度計算しているが、まとめて計算するように変更する(ほうが良いかもしれない)
        """
        '''
        self.dx += x
        if self.dx > ENSHU:
            self.dx -= ENSHU
        elif self.dx < (-ENSHU):
            self.dx += ENSHU
        '''
        angle = self.__xToAngle(x) 
        self.aangle += angle if x < 0 else -angle
    
    def __howManyTimesDoIHaveToTurn(self, x, y):
        """回転する角度を計算する
        
        戻値:
            +/-(0~180))
        """
        dif_x = x - self.ax
        dif_y = y - self.ay
        dif_rotation = math.degrees(math.atan2(dif_y, dif_x)) - self.aangle
        
        if dif_rotation > 180:
            dif_rotation -= 360
        elif dif_rotation < -180:
            dif_rotation += 360
        return dif_rotation

    def __howManyMove(self, x, y):
        """移動する距離を計算する"""
        dif_x = x - self.ax
        dif_y = y - self.ay
        return math.sqrt(dif_x ** 2 + dif_y ** 2)

    def __xToAngle(self, x):
        """円周(X座標)から角度を計算する(戻り値は必ず+)"""
        angle = (abs(x) * 360) / (2 * OPTICAL_HANKEI * math.pi)
        return  angle


def main():
    try:
        time.sleep(10)
        control = ControlTheCar()
        control.goto(-500, 500)
        control.goto(500, 1000)
        control.goto(0, 0)
        control.logs.makeCSV('OpticalFlow')
        control.logs.makeTXT('OpticalFlow')
    except KeyboardInterrupt:
        pass
    print("終了")

if __name__ == "__main__":
    main()
