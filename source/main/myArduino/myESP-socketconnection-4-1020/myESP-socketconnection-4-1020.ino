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
#define STRIDELENGTH 35//cm

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 mpu;

#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards
#define LED_PIN 5 // (Arduino is 13, Teensy is 11, Teensy++ is 6)

//#define BUTTON 15
#define PUSHBUTTON 19


bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
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

void setup() {
    pinMode(LED_PIN,OUTPUT);//LED
    //pinMode(BUTTON,INPUT);//B
    pinMode(PUSHBUTTON,INPUT);//PUSHB
    digitalWrite(5,HIGH);
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    Serial.begin(115200);
    delay(500);
    Serial.println();
    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // Calibration Time: generate offsets and calibrate our MPU6050
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.PrintActiveOffsets();
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
        Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
        Serial.println(F(")..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
    
    // configure LED for output
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
void loop() {
    float iDegreeRad = 0; 
    float iDisDegreeRad = 0; 
    //bool lightSwitch = 0;
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
            while(iTime-iAgoTime<1000){//1s毎に送信
            //10sループ
                // read a packet from FIFO
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

                    if(aaReal.z>1200){
                        walkFlag = 1; 
                        digitalWrite(5,HIGH);
                    }
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
