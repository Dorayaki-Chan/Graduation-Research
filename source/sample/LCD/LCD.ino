#include<LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // put your setup code here, to run once:
  lcd.init(1, 12, 255, 11, 5, 4, 3, 2, 0, 0, 0, 0);
  lcd.begin(16, 2);
  lcd.print("Hello, World!!!");
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0, 1);
  lcd.print(millis()/1000);
}
