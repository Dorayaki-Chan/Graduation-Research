# 卒業研究チュートリアル

## はじめに

### インストールするもの
#### アプリ
- [x] [ Arduino IDE](https://www.arduino.cc/en/software)
- [x]  [Visual Studio 2017](https://visualstudio.microsoft.com/ja/#vs-section)
- [ ] [UrgBenri](https://sourceforge.net/projects/urgbenri/)

#### その他
- [ ] [ESP32 環境構築](https://interface.cqpub.co.jp/esp32-arduino-ide-2/)

### ブックマーク推奨
- [ ] [LRF製品ページ](https://www.hokuyo-aut.co.jp/search/single.php?serial=16)
- [ ] [LRF取説](https://sourceforge.net/p/urgnetwork/wiki/top_jp/)

### ライブラリー関係
- [ ] [I2CdevとMPU6050](https://github.com/jrowberg/i2cdevlib/archive/master.zip)
> 1. Download the i2cdevlib project  
> 2. Unzip the downloaded file.
> 3. Select Sketch > Include Libraries > Add .ZIP Library from the Arduino IDE's menus.
> 4. Select the "Arduino/I2Cdev" subfolder of the unzipped folder.
> 5. Click the Open button.  
> 
> - フォルダーパス `\Arduino\I2Cdev`, `\Arduino\MPU6050`


## セッティング

### LRF編
周りが見える凄いレーダーみたいなやつ
1. LRFをPCにぶっ刺す(有線LAN or 太ったUSB接続)
2. IPアドレスの設定[^1]

[^1]: 参考[IPアドレスの設定(Windows)](https://sourceforge.net/p/urgnetwork/wiki/ip_address_jp/)

### PDRベルト
歩幅(規定値)と歩数と角度で位置推定を行うらしい

