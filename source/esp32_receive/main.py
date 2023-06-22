#ESP32-PC間通信用プログラム(PC側)
#socketライブラリをインポート
import socket
ip_address = '10.84.233.110' #サーバー（ESP32のIPアドレス）
port = 5000 #ポート番号
buffer_size = 4092 #一度に受け取るデータの大きさを指定
recieve_message = ""
recieve_status = True
#クライアント用インスタンスを生成
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# サーバーに接続を要求する（IPアドレスとポート番号を指定）
client.connect((ip_address, port))
'''
# サーバにデータを送信する
message = input("送信するメッセージを入力してください\n→")
message += "\n"#文末に改行コードを追加
client.sendall(bytes(message, encoding='ASCII'))#文字列をバイトに変換し送信（文字コードはASCIIを使用）
print("サーバーへデータ送信")
'''
#サーバーからの応答を受信
while True:
    data = client.recv(buffer_size) #サーバから送られてきたデータを読み取り（上限4092ビット）
    print("サーバからのメッセージ")
    print(data.decode())#受け取ったデータ（バイト形式）を読める形式にデコードして表示
#通信を終了
client.close()
print("ソケット通信を終了")