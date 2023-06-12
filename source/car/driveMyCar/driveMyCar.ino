const int RightFWD = 5;
const int RightBWD = 6;
const int LeftFWD = 10;
const int LeftBWD = 11;

byte speed = 255;

void setup() {
  // put your setup code here, to run once:
  pinMode(RightFWD, OUTPUT);
  pinMode(RightBWD, OUTPUT);
  pinMode(LeftFWD, OUTPUT);
  pinMode(LeftBWD, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  driveForward();
  delay(1000);
  stop();
  delay(1000);
  driveBackward();
  delay(1000);
  stop();
  delay(1000);
  turnLeft();
  delay(1000);
  stop();
  delay(1000);
  turnRight();
  delay(1000);
  stop();
  delay(1000);
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
