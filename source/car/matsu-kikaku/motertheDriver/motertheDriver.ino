/*
内容:ガキの玩具のArudinoプログラム
情報:2023/06/16 K.Matsumoto

アドレス:

なんぞや:シリアル受信含む、pcとarduinoの結合
1byte:255
2byte:左 0-127-254

TODO:

*/
const int MOTER_RIGHT_EN = 9;
const int MOTER_RIGHT_FWD = 8;
const int MOTER_RIGHT_BWD = 11;

const int MOTER_LEFT_EN = 10;
const int MOTER_LEFT_FWD = 12;
const int MOTER_LEFT_BWD = 13;

void setup() {
        // モーター右
        pinMode(MOTER_RIGHT_EN, OUTPUT); 
        pinMode(MOTER_RIGHT_FWD, OUTPUT);
        pinMode(MOTER_RIGHT_BWD, OUTPUT);

        // モーター左
        pinMode(MOTER_LEFT_EN, OUTPUT);
        pinMode(MOTER_LEFT_FWD, OUTPUT);
        pinMode(MOTER_LEFT_BWD, OUTPUT);

        Serial.begin(9600);
}

void loop() {
        int motorSig[2]={0};//送られたデータをそのまま受け取る
        int incomingByteNum = 0;//何バイト目か
        int lastRecvTime=0;
        // send data only when you receive data:
        while(1){
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
                                        lastRecvTime=millis();
                                        // 左
                                        if(motorSig[0]>127)
                                                backword('l');
                                        else{
                                                forword('l');
                                        }
                                        analogWrite(MOTER_RIGHT_EN, abs(motorSig[0]-127)*2);
                                        // 右
                                        if(motorSig[1]>127)
                                                backword('r');
                                        else{
                                                forword('r');
                                        }
                                        analogWrite(MOTER_LEFT_EN,abs(motorSig[1]-127)*2);
                                        //Serial.println(abs(motorSig[1]-127)*2);
                                        //delay(100);
                                }
                                incomingByteNum=0;//1byte目に遷移
                        }
                }else{
                        int nowTime=millis();
                        if(nowTime-lastRecvTime>3000){//前回の受信から3sたったら停止
                                mstop();
                        }
                }
        }
}

void forword(char rf){
        if(rf == 'r'){
                digitalWrite(MOTER_RIGHT_FWD, HIGH);
                digitalWrite(MOTER_RIGHT_BWD, LOW);
        }
        else{
                digitalWrite(MOTER_LEFT_FWD, HIGH);
                digitalWrite(MOTER_LEFT_BWD, LOW);
        }
}

void backword(char rf){
        if(rf == 'r'){
                digitalWrite(MOTER_RIGHT_FWD, LOW);
                digitalWrite(MOTER_RIGHT_BWD, HIGH);
        }
        else{
                digitalWrite(MOTER_LEFT_FWD, LOW);
                digitalWrite(MOTER_LEFT_BWD, HIGH);
        }
}

void mstop(){
        digitalWrite(MOTER_RIGHT_FWD, LOW);
        digitalWrite(MOTER_RIGHT_BWD, LOW);
        analogWrite(MOTER_RIGHT_EN, 255);

        digitalWrite(MOTER_LEFT_FWD, LOW);
        digitalWrite(MOTER_LEFT_BWD, LOW);
        analogWrite(MOTER_LEFT_EN, 255);
}