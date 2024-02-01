/*
    https://lang-ship.com/blog/work/esp32-tcp-ip-socket/
    2022/09/28 N.Hirota
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
    while (client.connected()) {
      int size = client.available();
      if (size) {
        Serial.println();
        Serial.print("size:");
        Serial.println(size);

        char input[256];//文字受信
        memset(input, 0, sizeof(input));
        client.read((uint8_t*)input, sizeof(input) - 1);
        Serial.printf("get: [%s]\n", input);

        char* command0 = strtok(input, ":");//文字を分割
        String command = command0;
        command0 = strtok(NULL, ":");
        String command2 = command0;
        command0 = strtok(NULL, ":");
        String command3 = command0;
        command0 = strtok(NULL, ":");
        String command4 = command0;

        command.trim();
        command2.trim();
        command3.trim();
        command4.trim();
        Serial.printf("command: [%s]\n", command);
        Serial.printf("command2: [%s]\n", command2);
        Serial.printf("command3: [%s]\n", command3);
        Serial.printf("command4: [%s]\n", command4);

        String ret = "";
        if (command == "*IDN?") {
          uint8_t mac[6];
          esp_read_mac(mac, ESP_MAC_WIFI_STA);
          char mac_str[18];
          snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

          ret = String("ESP32,VISA Test,") + mac_str + "," + __DATE__"\n";
          Serial.printf("\n");
        } else if (command == "*RST") {

        }

        if (ret != "") {
          client.write(ret.c_str());
        }
      }

      delay(1);
    }
    client.stop();
    Serial.println();
    Serial.println("client.stop()");
  }

  delay(1);
}