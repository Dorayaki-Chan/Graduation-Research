import lgpio
import time, sys
# GPIO番号（BCM）を変数に代入
LED_PORT = 4
# dev/gpiochip0をオープン
h = lgpio.gpiochip_open(0)
# 指定したピンにモード（出力）を設定
lgpio.gpio_claim_output(h, LED_PORT)
try:
    while True:
        # 指定したピンを点灯（HIGHを設定）
        lgpio.gpio_write(h, LED_PORT, 1)
        # 0.3秒待つ
        time.sleep(0.3)
        # 指定したピンを消灯（LOWを設定）
        lgpio.gpio_write(h, LED_PORT, 0)
        # 0.3秒待つ
        time.sleep(0.3)
except KeyboardInterrupt:
    # dev/gpiochip0をクローズ
    lgpio.gpiochip_close(h)
