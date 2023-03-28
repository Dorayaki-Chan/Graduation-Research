/*
内容:
	10sごとに適当に数字を送信
情報:2022/09/28  N.Hirota

アドレス:
なんぞや:ソケット通信サーバー側
注意事項:https://lang-ship.com/blog/work/esp32-tcp-ip-socket/
http://den3.net/activity_diary/2021/03/30/3407/
TODO:

*/
#include <WiFi.h>
#include <WiFiClient.h>

WiFiServer server(5000);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();

  WiFi.begin();
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
      //Serial.println("aaaaaaaaaaaa");
      //int size = client.available();
      //Serial.println("bbbbbbbbbbbbbbb");
      //if (size) {
        //Serial.println();
        //Serial.print("size:");
        //Serial.println(size);

        //client.write("test");
        //Serial.println("test");
        
        count++;
        char sendData[3];
        snprintf(sendData,3,"%d",count);
        client.write(sendData);
        Serial.println(sendData);

      //}

      delay(10000);//10s
    }
    client.stop();
    Serial.println();
    Serial.println("client.stop()");
  }

  delay(1);
}