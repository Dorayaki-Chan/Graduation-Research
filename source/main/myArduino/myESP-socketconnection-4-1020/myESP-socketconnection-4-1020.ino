/*
内容:
	myESP-socketconnnection-2+myMPU6050の合体
情報:2022/10/5  N.Hirota

アドレス:
なんぞや:ソケット通信サーバー側
注意事項:https://lang-ship.com/blog/work/esp32-tcp-ip-socket/
http://den3.net/activity_diary/2021/03/30/3407/

URGPDR3と対
TODO:
espに開始信号と停止信号もつけないと
あと先頭文字と末尾文字
正面待機のとき点滅に,接続のときもなんかほしい

*/
#include <WiFi.h>
#include <WiFiClient.h>

//#define SERIALPRINT
#define LAB//どこで使うか、wifi HOMEorLAB
#define STRIDELENGTH 92//cm

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 mpu;

#define INTERRUPT_PIN 2  // Arduino Uno とほとんどのボードでピン 2 を使用
#define LED_PIN 5 // (Arduino は 13, Teensy は 11, Teensy++ は 6)

//#define BUTTON 15
#define PUSHBUTTON 19


bool blinkState = false;

// MPU 制御/ステータス変数
bool dmpReady = false;  // DMP 初期化が成功した場合は true に設定
uint8_t mpuIntStatus;   // MPU からの実際の割り込みステータス バイトを保持する
uint8_t devStatus;      // 各デバイス操作後にステータスを返します (0 = 成功、!0 = エラー)
uint16_t packetSize;    // 予想される DMP パケット サイズ (デフォルトは 42 バイト)
uint16_t fifoCount;     // 現在 FIFO にある全バイト数
uint8_t fifoBuffer[64]; // FIFO 格納バッファ

// 向き/モーション変数
Quaternion q;           // [w, x, y, z]         四元数コンテナ
VectorInt16 aa;         // [x, y, z]            加速度センサー測定
VectorInt16 aaReal;     // [x, y, z]            加速度センサー測定
VectorInt16 aaWorld;    // [x, y, z]            ワールド フレーム加速度センサー測定
VectorFloat gravity;    // [x, y, z]            重力ベクトル
float euler[3];         // [psi, theta, phi]    オイラー角コンテナ
float ypr[3];           // [yaw, pitch, roll]   ヨー/ピッチ/ロール コンテナと重力ベクトル

volatile bool mpuInterrupt = false;     // MPU 割り込みピンがハイになったかどうかを示します
void dmpDataReady() {
    mpuInterrupt = true;
}

#ifdef HOME
const char *ssid="Buffalo-G-2070";         
const char *password="7t5s4uu4rakh4";
#endif

#ifdef LAB
const char *ssid="LCSWLANG";       
const char *password="arailabair";   
#endif

#ifdef SMARTPHONE
const char *ssid="Japan_Free_Wi-Fi";       
const char *password="tokyo2020";   
#endif

WiFiServer server(5000);

int strideLength = STRIDELENGTH;

// -q:setuo()は何をしているの？
// -a:MPU6050の初期化
void setup() {
    pinMode(LED_PIN,OUTPUT);//LED
    //pinMode(BUTTON,INPUT);//B
    pinMode(PUSHBUTTON,INPUT);//PUSHB
    digitalWrite(5,HIGH);

    // -q:何をしているの？
    // -a:シリアル通信の初期化
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C クロック。コンパイルに問題がある場合は、この行にコメントしてください
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    Serial.begin(115200);
    delay(500);
    Serial.println();
    // デバイスの初期化
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);

    // 接続を確認する
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // DMP をロードして構成する
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // 独自のジャイロ オフセットをここに入力し、最小感度にスケーリングします
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 私のテストチップの1688工場出荷時のデフォルト

    // 動作することを確認してください (動作している場合は 0 を返します)
    if (devStatus == 0) {
        // キャリブレーション時間: オフセットを生成し、MPU6050 をキャリブレーションします。
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.PrintActiveOffsets();
        // 準備が整ったので、DMP をオンにします。
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // Arduino 割り込み検出を有効にする
        Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
        Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
        Serial.println(F(")..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // DMP Ready フラグを設定して、メインの loop() 関数がそれを使用してもよいことを認識できるようにします。
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // 後で比較するために、予想される DMP パケット サイズを取得する
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // エラー！
        // 1 = 初期メモリロードに失敗しました
        // 2 = DMP 構成の更新に失敗しました
        // (壊れそうな場合、通常コードは 1 になります)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
    
    // 出力用の LED を構成する
    pinMode(LED_PIN, OUTPUT);
    WiFi.begin(ssid, password);
    Serial.print("WiFi.begin()");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.println("WiFi Connected.");
    Serial.printf("IP Address  : ");
    Serial.println(WiFi.localIP());

    server.begin();
    Serial.println("server.begin()");
    
    digitalWrite(5,LOW);
    delay(500);
}
bool syomenSet=0;

// -q:loop()は何をしているの？
// -a:MPU6050からデータを取得し、クライアントからの接続を待つ
void loop() {
    float iDegreeRad = 0; 
    float iDisDegreeRad = 0; 
    //bool lightSwitch = 0;
    /* 正面をセットする */
    /* ボタン押したときのあれ */
    if(syomenSet==0){
        Serial.println("syomen set up");
        while(digitalRead(PUSHBUTTON)==0){
            digitalWrite(5,HIGH);
            delay(50);
            digitalWrite(5,LOW);
            delay(50);
        }
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        iDegreeRad = ypr[0]; 
        iDisDegreeRad = iDegreeRad; 
        syomenSet = 1;
    }
    digitalWrite(5,LOW);

    WiFiClient client = server.available();

    if (client) {//||digitalRead(BUTTON)==0
        Serial.println("New Client.");
        bool walkFlag = 0;
        while (client.connected()) {
            int size = client.available();
            if (size) {
                Serial.println();
                Serial.print("size:");
                Serial.println(size);
                for (int i = 0; i < size; i++) {
                    char c = client.read();
                    Serial.print(c);
                }
                Serial.println("");
            }

            //Serial.println("New Client."); 
            //char sendDataChar[16];
            //snprintf(sendDataChar,16,"S0000000000E");//Sで始まり,Eで終わる　エラー回避
            //client.write(sendDataChar);  
            //while (client.connected()) {//ボタンが1ならwifionでよし ||digitalRead(BUTTON)==0
            // 下準備部
            if (!dmpReady) return;
            int iAgoTime = millis();
            int iTime = iAgoTime;
            int  xWalk = 0;
            int  yWalk = 0;
            int8_t walkCount=0;//歩数が保存されてる 64

            //1s毎に送信       
            while(iTime-iAgoTime<1000){
                //10sループ
                // FIFO からパケットを読み取る
                if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
                    mpu.dmpGetQuaternion(&q, fifoBuffer);
                    mpu.dmpGetAccel(&aa, fifoBuffer);
                    mpu.dmpGetGravity(&gravity, &q);
                    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
                    //Serial.print("  aRealz:");
                    //Serial.println(aaReal.z);//必要
                    mpu.dmpGetQuaternion(&q, fifoBuffer);//test
                    mpu.dmpGetGravity(&gravity, &q);
                    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
                    iDegreeRad = ypr[0];
                    //Serial.print("ypr: ");
                    //Serial.println(iDegreeRad * 180/M_PI);
                    
                    /* 歩き始め検知*/
                    if(aaReal.z>1200){
                        walkFlag = 1; 
                        digitalWrite(5,HIGH);
                    }
                    /*　着地を検知　*/
                    if(walkFlag==1&&aaReal.z<300){
                        //一歩分、
                        walkCount++;
                        digitalWrite(5,LOW);
                        walkFlag = 0;
                        //Serial.print("walkCount= ");
                        //Serial.print(walkCount);

                        //角度取得
                        mpu.dmpGetQuaternion(&q, fifoBuffer);
                        mpu.dmpGetGravity(&gravity, &q);
                        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
                        iDegreeRad = ypr[0];
                        Serial.print(" ypr: ");
                        Serial.print(iDegreeRad * 180/M_PI);
                        
                        Serial.print(" DisDegree: ");
                        Serial.print((iDegreeRad-iDisDegreeRad)* 180/M_PI);
                        xWalk += strideLength * sin(iDegreeRad-iDisDegreeRad);
                        yWalk += strideLength * cos(iDegreeRad-iDisDegreeRad);
                        Serial.print(" Xwalk: ");
                        Serial.print(strideLength * sin(iDegreeRad-iDisDegreeRad));
                        Serial.print(" Ywalk: ");
                        Serial.println(strideLength * cos(iDegreeRad-iDisDegreeRad));
                    }
                }
                iTime = millis();
            }
            
            //Serial.print("walkCount= ");
            //Serial.print(walkCount);
            
            //送信部
            char sendDataChar[16];
            snprintf(sendDataChar,16,"S%05d%05dE",xWalk,yWalk);//Sで始まり,Eで終わる　エラー回避
            client.write(sendDataChar);

            //#ifdef SERIALPRINT
            Serial.print("send data = ");
            Serial.println(sendDataChar);
            //#endif
        }
        client.stop();
        Serial.println();
        Serial.println("client.stop()");
    }

    delay(1);
}
