/*
内容:
	10sごとに適当に数字を送信
    歩数と方向の差分を送信する規格を作る
情報:2022/09/28  N.Hirota

アドレス:
なんぞや:ソケット通信サーバー側
注意事項:https://lang-ship.com/blog/work/esp32-tcp-ip-socket/
http://den3.net/activity_diary/2021/03/30/3407/

Socket2と対
TODO:

*/
#include <WiFi.h>
#include <WiFiClient.h>

#define SERIALPRINT
#define LAB//どこで使うか、wifi HOMEorLAB

#ifdef HOME
const char *ssid="Buffalo-G-2070";         
const char *password="7t5s4uu4rakh4";
#endif

#ifdef LAB
const char *ssid="LCSWLANG";       
const char *password="arailabair";   
#endif

WiFiServer server(5000);

int8_t walkCount=0;//歩数が保存されてる 64
int8_t disDegree=99;//角度の差分を保存   0-360 受信側の都合でマイナスにならないように差分計算した後+180してほしい

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();

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
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    int count = 0;
    while (client.connected()) {

      if(walkCount<3) walkCount++;
      else             walkCount=0;
      #ifdef SERIALPRINT
        Serial.print("walkCount= ");
        Serial.print(walkCount);
      #endif

      if(disDegree<100) disDegree++;
      else             disDegree=99;  

      #ifdef SERIALPRINT
        Serial.print("disDegree= ");
        Serial.println(disDegree);
      #endif

      //disDegree+=180;//負の数面倒180を足す

      char sendDataChar[16];
      snprintf(sendDataChar,16,"%03d%03d",walkCount,disDegree);
      client.write(sendDataChar);
      #ifdef SERIALPRINT
        Serial.print("send data = ");
        Serial.println(sendDataChar);
      #endif
      delay(10000);//10s
    }
    client.stop();
    Serial.println();
    Serial.println("client.stop()");
  }

  delay(1);
}