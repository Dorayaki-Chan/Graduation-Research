/*
内容:ガキの玩具のArudinoプログラム(お家用)
情報:2023/06/16 K.Matsumoto

アドレス:

なんぞや:シリアル受信含む、pcとarduinoの結合
1byte:255
2byte:左 0-127-254

TODO:

*/
const int RightFWD = 5;
const int RightBWD = 6;
const int LeftFWD = 10;
const int LeftBWD = 11;

void setup() {
        pinMode(RightFWD, OUTPUT);
        pinMode(RightBWD, OUTPUT);
        pinMode(LeftFWD, OUTPUT);
        pinMode(LeftBWD, OUTPUT);

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
                                        // 右
                                        int speed1 = abs(motorSig[0]-127)*2;
                                        if(motorSig[0]>127){
                                                analogWrite(RightFWD, speed1);
                                                analogWrite(RightBWD, 0);
                                        }
                                        else{
                                                analogWrite(RightFWD, 0);
                                                analogWrite(RightBWD, speed1);
                                        }
                                        
                                        // 左
                                        int speed2 = abs(motorSig[1]-127)*2;
                                        if(motorSig[1]>127){
                                                analogWrite(LeftFWD, speed2);
                                                analogWrite(LeftBWD, 0);
                                        }
                                        else{
                                                analogWrite(LeftFWD, 0);
                                                analogWrite(LeftBWD, speed2);
                                        }
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



void mstop(){
        analogWrite(RightFWD, 0);
        analogWrite(RightBWD, 0);
        analogWrite(LeftFWD, 0);
        analogWrite(LeftBWD, 0);
}
