#ESP32-PC間通信用プログラム(PC側)
#socketライブラリをインポート
import socket
import time
IP_ADDRESS = '10.84.233.110' #サーバー（ESP32のIPアドレス）
PORT = 5000 #ポート番号
BUFFER_SIZE = 4092 #一度に受け取るデータの大きさを指定
RECIEVE_MESSAGE = ""
RECIEVE_STATUS = True

'''
# サーバにデータを送信する
message = input("送信するメッセージを入力してください\n→")
message += "\n"#文末に改行コードを追加
client.sendall(bytes(message, encoding='ASCII'))#文字列をバイトに変換し送信（文字コードはASCIIを使用）
print("サーバーへデータ送信")
'''


class Walk:
    def __init__(self):
        # 絶対座標
        self.tx = 0
        self.ty = 0

        #クライアント用インスタンスを生成
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # サーバーに接続を要求する（IPアドレスとポート番号を指定）
        self.client.connect((IP_ADDRESS, PORT))
        pass

    def moveRobot(self):
        while True:
            self.receiveData()
    
    #サーバーからの応答を受信
    def receiveData(self):
        datas = self.client.recv(BUFFER_SIZE) #サーバから送られてきたデータを読み取り（上限4092ビット）
        print("サーバからのメッセージ")
        receiveDatas = datas.decode()
        print(receiveDatas)#受け取ったデータ（バイト形式）を読める形式にデコードして表示
        for i in range(0, len(receiveDatas)-12, 12):
            data = receiveDatas[i:i+12]
            if len(data) == 12 and data[0] == 'S' and data[11] == 'E':
                walk_X = int(data[1:6])
                walk_y = int(data[6:11])
                self.tx += walk_X
                self.ty += walk_y
            else:
                print("データが破損してます")
                pass 

'''
# テスト
'''
def test():
    walk = Walk()
    while True:
        walk.receiveData()
        time.sleep(5)


if __name__ == "__main__":
    test()


#通信を終了
# client.close()
print("ソケット通信を終了")
