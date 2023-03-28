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


## やってみよう

### LRF編
周りが見える凄いレーダーみたいなやつ
1. LRFをPCにぶっ刺す(有線LAN or 太ったUSB接続)
> ![picture 1](images/posts/README/IMG_5589.jpg)
2. IPアドレスの設定[^1]
> IPアドレス:`172.16.0.11`  
> サブネット マスク:`255.255.255.0`  
> ![picture 2](images/posts/README/1679993990231.png)
3. UrgBenri起動
> IPアドレス:`172.16.0.10`  
> ![picture 3](images/posts/README/1679994513479.png)  

[^1]: 参考:[IPアドレスの設定(Windows)](https://sourceforge.net/p/urgnetwork/wiki/ip_address_jp/)

### PDRベルト編
歩幅(規定値)と歩数と角度で位置推定を行うらしい

1. ベルトの基盤をPCに刺す！
> ![picture 4](images\posts\README\IMG_5590.jpg)
2. 書き込むプログラムを起動!
> `\source\main\myArduino\myESP-socketconnection-4-1020\myESP-socketconnection-4-1020.ino`
3. 22行目(?)LABになってるか確認！
> ![picture 6](images/posts/README/1679995612007.png)  

4. 記載されてるWi-FiにPCを繋げる！ 
> ![picture 5](images/posts/README/1679995452050.png)
5. シリアルモニター起動！
> ![picture 7](images/posts/README/1679995884771.png)  
6. Arduinoに書き込む！
> ![picture 4](images/posts/README/1679995195344.png)  
7. 表示されるIPアドレスをメモ！
> ![picture 7](images/posts/README/address.png)

### 運用編

1. Visual Studioで`source\main\urg_library\vs2017\cpp\urg_cpp.sln`を起動！
2. 26行目の絶対パスを書き換える
> ![picture 8](images/posts/README/1679996473294.png)   
```cpp
#include <D:\Programming\SotsuKen\source\main\urg_library\samples\cpp\Connection_information_me.h>
```
3. 41行目をさっきメモったIPアドレスに書き換える
> ![picture 9](images/posts/README/1679996728620.png)  
```cpp
#define ESPIPADDR  "10.84.233.91"//"192.168.11.18"//192.168.208.233, 
```