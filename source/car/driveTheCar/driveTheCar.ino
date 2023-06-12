int MOTER1_EN = 9;
int MOTER1_IN1 = 8;
int MOTER1_IN2 = 11;

int MOTER2_EN = 10;
int MOTER2_IN1 = 12;
int MOTER2_IN2 = 13;


void setup() {
  // put your setup code here, to run once:
  // モーター右
  pinMode(MOTER1_EN, OUTPUT); 
  pinMode(MOTER1_IN1, OUTPUT);
  pinMode(MOTER1_IN2, OUTPUT);

  // モーター左
  pinMode(MOTER2_EN, OUTPUT);
  pinMode(MOTER2_IN1, OUTPUT);
  pinMode(MOTER2_IN2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  forward();
  delay(1000);
  back();
  delay(1000);
}

// 前進
void forward(){
  digitalWrite(MOTER1_IN1, HIGH);
  digitalWrite(MOTER1_IN2, LOW);
  analogWrite(MOTER1_EN, 255);

  digitalWrite(MOTER2_IN1, HIGH);
  digitalWrite(MOTER2_IN2, LOW);
  analogWrite(MOTER2_EN, 255);
}

// 後退
void back(){
  digitalWrite(MOTER1_IN1, LOW);
  digitalWrite(MOTER1_IN2, HIGH);
  analogWrite(MOTER1_EN, 255);

  digitalWrite(MOTER2_IN1, LOW);
  digitalWrite(MOTER2_IN2, HIGH);
  analogWrite(MOTER2_EN, 255);
}
