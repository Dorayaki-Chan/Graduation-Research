const int RightFWD = 5;
const int RightBWD = 6;
const int LeftFWD = 10;
const int LeftBWD = 11;

byte val = 0;

byte speed = 255;

int delayTime = 1;

void setup() {
  // put your setup code here, to run once:
  pinMode(RightFWD, OUTPUT);
  pinMode(RightBWD, OUTPUT);
  pinMode(LeftFWD, OUTPUT);
  pinMode(LeftBWD, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    val = Serial.read();
    if(val == 'f') {
      driveForward();
      delay(delayTime);
    }
    else if(val == 'b') {
      driveBackward();
      delay(delayTime);
    }
    else if(val == 'l') {
      turnLeft();
      delay(delayTime);
    }
    else if(val == 'r') {
      turnRight();
      delay(delayTime);
    }
  }
  else {
    stop();
    delay(delayTime);
  }
}

void driveForward() {
  analogWrite(RightFWD, speed);
  analogWrite(RightBWD, 0);
  analogWrite(LeftFWD, speed);
  analogWrite(LeftBWD, 0);
}

void driveBackward() {
  analogWrite(RightFWD, 0);
  analogWrite(RightBWD, speed);
  analogWrite(LeftFWD, 0);
  analogWrite(LeftBWD, speed);
}

void turnLeft() {
  analogWrite(RightFWD, speed);
  analogWrite(RightBWD, 0);
  analogWrite(LeftFWD, 0);
  analogWrite(LeftBWD, speed);
}

void turnRight() {
  analogWrite(RightFWD, 0);
  analogWrite(RightBWD, speed);
  analogWrite(LeftFWD, speed);
  analogWrite(LeftBWD, 0);
}

void stop() {
  analogWrite(RightFWD, 0);
  analogWrite(RightBWD, 0);
  analogWrite(LeftFWD, 0);
  analogWrite(LeftBWD, 0);
}
