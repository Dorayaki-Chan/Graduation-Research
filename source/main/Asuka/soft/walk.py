#!/usr/bin/env python

"""
内容:
	PDRの受信データを処理するクラス
情報:2023/06/23  K.Matsumoto
"""

#ESP32-PC間通信用プログラム(PC側)
#socketライブラリをインポート
import socket
import time
import log
IP_ADDRESS = '10.84.233.110' #サーバー（ESP32のIPアドレス）
PORT = 5000 #ポート番号
BUFFER_SIZE = 4092 #一度に受け取るデータの大きさを指定
RECIEVE_MESSAGE = ""
RECIEVE_STATUS = True

class Walk:
    """PDRの受信データを処理するクラス"""
    def __init__(self):
        # 絶対座標
        self.tx = 0
        self.ty = 0

        #クライアント用インスタンスを生成
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # サーバーに接続を要求する（IPアドレスとポート番号を指定）
        self.client.connect((IP_ADDRESS, PORT))

        #ログクラスをインスタンス化
        self.logs = log.Logs()
        self.logs.start()

    def receiveData(self):
        """ESP32からデータを受け取り処理をする."""
        datas = self.client.recv(BUFFER_SIZE) #サーバから送られてきたデータを読み取り（上限4092ビット）
        print("サーバからのメッセージ")
        receiveDatas = datas.decode()
        print(receiveDatas) #受け取ったデータ（バイト形式）を読める形式にデコードして表示
        # 溜まってるデータを分割
        for i in range(0, len(receiveDatas)-12, 12):
            data = receiveDatas[i:i+12]
            if len(data) == 12 and data[0] == 'S' and data[11] == 'E':
                walk_X = int(data[1:6])
                walk_y = int(data[6:11])
                self.tx += walk_X
                self.ty += walk_y
                # ログを取る
                self.logs.addAll(self.tx, self.ty, 0, walk_X, walk_y, 0)
            else:
                print("データが破損してます")
                pass 

def test():
    """テスト用関数"""
    walk = Walk()
    while True:
        walk.receiveData()
        time.sleep(5)


if __name__ == "__main__":
    test()


#通信を終了
# client.close()
print("ソケット通信を終了")
