#!/usr/bin/env python

"""
内容:
	PDRの受信データを処理しあすかに移動命令を出すクラス
情報:2023/06/23  K.Matsumoto
"""

#ESP32-PC間通信用プログラム(PC側)
#socketライブラリをインポート
import socket
import time
import log
import control
IP_ADDRESS = '10.84.233.108' #サーバー（ESP32のIPアドレス）
PORT = 5000 #ポート番号
BUFFER_SIZE = 4092 #一度に受け取るデータの大きさを指定
RECIEVE_MESSAGE = ""
RECIEVE_STATUS = True

class Walk:
    """PDRの受信データを処理し、あすかに移動命令を出すクラス"""
    def __init__(self):
        # 絶対座標
        self.tx = 0
        self.ty = 0

        #クライアント用インスタンスを生成
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # サーバーに接続を要求する（IPアドレスとポート番号を指定）
        self.client.connect((IP_ADDRESS, PORT))

        # あすかの制御クラスをインスタンス化
        self.asuka = control.ControlTheCar()

        #ログクラスをインスタンス化
        self.logs = log.Logs()
        self.logs.start()

    def receiveData(self, gotoFlag = True):
        """ESP32からデータを受け取り処理をする.
        
        引数：
            gotoFlag(boolen):デフォルトはTrue.あすかを動かさないときはFalse
        """
        datas = self.client.recv(BUFFER_SIZE) #サーバから送られてきたデータを読み取り（上限4092ビット）
        print("サーバからのメッセージ")
        receiveDatas = datas.decode()
        print(receiveDatas) #受け取ったデータ（バイト形式）を読める形式にデコードして表示
        # 溜まってるデータを分割
        for i in range(0, len(receiveDatas), 12):
            data = receiveDatas[i:i+12]
            if len(data) == 12 and data[0] == 'S' and data[11] == 'E' and data != 'S0000000000E':
                walk_X = int(data[1:6]) * 10 # cm → mm
                walk_y = int(data[6:11]) *10 # cm → mm
                self.tx += walk_X
                self.ty += walk_y
                print(self.tx, self.ty, walk_X, walk_y)
                # ログを取る
                self.logs.addAll(self.tx, self.ty, 0, walk_X, walk_y, 0)
                if gotoFlag:
                    # 移動司令を出す
                    self.asuka.goto(self.tx, self.ty)
            else:
                print("データが破損してます")
                pass 

def main():
    """メイン関数"""
    try:
        pdr = Walk()
        while True:
            pdr.receiveData(False)
    except KeyboardInterrupt:
        pdr.asuka.logs.makeCSV('OpticalFlow')
        pdr.asuka.logs.makeTXT('OpticalFlow')
        pdr.logs.makeCSV('PDR')
        pass
    finally:
        print("あすかの制御を終了します.")


if __name__ == "__main__":
    main()


#通信を終了
# client.close()
print("ソケット通信を終了")
