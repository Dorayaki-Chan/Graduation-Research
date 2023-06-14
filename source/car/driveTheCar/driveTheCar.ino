const int MOTER_RIGHT_EN = 9;
const int MOTER_RIGHT_FWD = 8;
const int MOTER_RIGHT_BWD = 11;

const int MOTER_LEFT_EN = 10;
const int MOTER_LEFT_FWD = 12;
const int MOTER_LEFT_BWD = 13;

byte val = 0;

byte speed = 255;

int delayTime = 1;

void setup() {
  // put your setup code here, to run once:
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
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    val = Serial.read();
    if(val == 'f') {
      forward();
    }
    else if(val == 'b') {
      back();
    }
    else if(val == 'l') {
      left();
    }
    else if(val == 'r') {
      right();
    }
    else if(val == 's'){
      mstop();
    }
    delay(delayTime);
  }
  else {
    //mstop();
  }
}

// 前進
void forward(){
  digitalWrite(MOTER_RIGHT_FWD, HIGH);
  digitalWrite(MOTER_RIGHT_BWD, LOW);
  analogWrite(MOTER_RIGHT_EN, speed);

  digitalWrite(MOTER_LEFT_FWD, HIGH);
  digitalWrite(MOTER_LEFT_BWD, LOW);
  analogWrite(MOTER_LEFT_EN, speed);
}

// 後退
void back(){
  digitalWrite(MOTER_RIGHT_FWD, LOW);
  digitalWrite(MOTER_RIGHT_BWD, HIGH);
  analogWrite(MOTER_RIGHT_EN, speed);

  digitalWrite(MOTER_LEFT_FWD, LOW);
  digitalWrite(MOTER_LEFT_BWD, HIGH);
  analogWrite(MOTER_LEFT_EN, speed);
}

// 左回転
void left(){
  digitalWrite(MOTER_RIGHT_FWD, HIGH);
  digitalWrite(MOTER_RIGHT_BWD, LOW);
  analogWrite(MOTER_RIGHT_EN, speed);

  digitalWrite(MOTER_LEFT_FWD, LOW);
  digitalWrite(MOTER_LEFT_BWD, HIGH);
  analogWrite(MOTER_LEFT_EN, speed);
}

// 右回転
void right(){
  digitalWrite(MOTER_RIGHT_FWD, LOW);
  digitalWrite(MOTER_RIGHT_BWD, HIGH);
  analogWrite(MOTER_RIGHT_EN, speed);

  digitalWrite(MOTER_LEFT_FWD, HIGH);
  digitalWrite(MOTER_LEFT_BWD, LOW);
  analogWrite(MOTER_LEFT_EN, speed);
}

// 停止
void mstop(){
  digitalWrite(MOTER_RIGHT_FWD, HIGH);
  digitalWrite(MOTER_RIGHT_BWD, HIGH);
  analogWrite(MOTER_RIGHT_EN, speed);

  digitalWrite(MOTER_LEFT_FWD, HIGH);
  digitalWrite(MOTER_LEFT_BWD, HIGH);
  analogWrite(MOTER_LEFT_EN, speed);
}
