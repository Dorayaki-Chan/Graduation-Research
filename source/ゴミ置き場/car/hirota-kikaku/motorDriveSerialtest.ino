/*
内容:
    cytron 10Amp DC motor driver のテストプログラム
情報:2023/5/16 N.Hirota

アドレス:

なんぞや:シリアル受信含む、pcとarduinoの結合テスト
4バイトまとめて受けれたらLED点灯
初版

TODO:

*/
#define PWMR 3
#define DIRR 4
#define PWML 5
#define DIRL 6

void setup() {
        Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
        pinMode(DIRR,OUTPUT);
        pinMode(DIRL,OUTPUT);
        pinMode(PWMR,OUTPUT);
        pinMode(PWML,OUTPUT);
        pinMode(13,OUTPUT);
        digitalWrite(DIRR,LOW);
        digitalWrite(DIRL,LOW);
}

void loop() {
        int motorSig[2]={0};//送られたデータをそのまま受け取る
        int incomingByteNum = 0;//何バイト目か
        int lastRecvTime=0;
        // send data only when you receive data:
        while(1){
                digitalWrite(13,LOW);
                if (Serial.available() > 0) {
                        int incomingByte = Serial.read();
                        if(incomingByteNum==0){//1byte目
                                if(incomingByte == 255){//正しい先頭文字ですか
                                        incomingByteNum++;
                                }
                        }else if(incomingByteNum==1){//2byte目
                                if(incomingByte != 255){//正しい2byte目ですか
                                        motorSig[0]=incomingByte;
                                        incomingByteNum++;
                                }
                        }else if(incomingByteNum==2){//3byte目
                                if(incomingByte != 255){//正しい3byte目ですか
                                        motorSig[1]=incomingByte;
                                        incomingByteNum++;
                                }else{
                                        incomingByteNum=0;//1byte目に遷移
                                }
                        }else if(incomingByteNum==3){//4byte目、正しい末尾文字ですか
                                if(incomingByte == 255){//正しい末尾文字ですのでモーターを回します
                                        digitalWrite(13,HIGH);//LEDつける
                                        lastRecvTime=millis();
                                        if(motorSig[0]>127)
                                                digitalWrite(DIRR,LOW);
                                        else{
                                                digitalWrite(DIRR,HIGH);
                                        }
                                        analogWrite(PWMR,abs(motorSig[0]-127)*2);
                                        //Serial.println(abs(motorSig[0]-127)*2);
                                        if(motorSig[1]>127)
                                                digitalWrite(DIRL,LOW);
                                        else{
                                                digitalWrite(DIRL,HIGH);
                                        }
                                        analogWrite(PWML,abs(motorSig[1]-127)*2);
                                        //Serial.println(abs(motorSig[1]-127)*2);
                                        //delay(100);
                                }
                                incomingByteNum=0;//1byte目に遷移
                        }
                }else{
                        int nowTime=millis();
                        if(nowTime-lastRecvTime>3000){//前回の受信から3sたったら停止
                                analogWrite(PWMR,0);
                                analogWrite(PWML,0);
                        }
                }
        }
}