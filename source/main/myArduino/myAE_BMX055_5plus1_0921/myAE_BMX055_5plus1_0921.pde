import processing.serial.*; // シリアルライブラリをインポート

Serial myPort;  //シリアルポートのインスタンス
int available_serialport = 3; // シリアル検索プログラムで調べたシリアルポートの番号に設定数値を変更してお
String arduinoPort = Serial.list()[available_serialport ]; //シリアルポートの設定
float []data = new float [3]; //シリアルのデータを格納する配列を宣言

void setup() {
  lights();
  size(300, 300, P3D); // キャンバスサイズ
  myPort = new Serial(this, arduinoPort, 115200); // シリアルポートの設定
}

void draw() {
  background(230); //背景グレー
  translate(width / 2, height / 2, 0); // 原点を図形の中心に
  rotateX(radians(-data[1])); //ここではProcessingのX軸としてPITCH軸の値を渡している.センサの初期方向にあわせお好みで調整
  rotateZ(radians(-data[0])); //ここではProcessingのZ軸としてROLL軸の値を渡している.センサの初期方向にあわせお好みで調整

  int size = 10;//図形のサイズ倍率。キャンバスを大きくする時に変更可
  box(20 * size, 1 * size, 15 * size); //GY-521基盤のような直方体を描く
  translate(0, -4 * size, 0);
}

void serialEvent(Serial p) { //シリアルを監視
  String inString = myPort.readStringUntil('\n'); //データがあったら改行のところまで読み込む
  if (inString != null) { //シリアルの文字列データが何か入っていれば
    inString = trim(inString); //シリアル文字列の前後の空白を削除
    data = float(split(inString, ',')); //ピリオドで分割して配列に格納
    println(data);//受信した配列データをprocessingのコンソールに出力
  }
}
