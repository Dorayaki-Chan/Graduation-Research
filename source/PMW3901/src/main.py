#!/usr/bin/env python
import time
import argparse
from pmw3901 import PMW3901, BG_CS_FRONT_BCM, BG_CS_BACK_BCM

# from openpyxl import Workbook
import csv
import os


# Pick the right class for the specified breakout
SensorClass = PMW3901 

flo = SensorClass(spi_port=0, spi_cs_gpio=BG_CS_FRONT_BCM)
flo.set_rotation(0)     # Rotation of sensor in degrees
                        # choices=[0, 90, 180, 270]

tx = 0
ty = 0

data = []

# 経過時間を計測するために開始時刻を保存
start = time.time()

# 経過時間を秒で返す
def get_elapsed_time():
    return time.time() - start

try:
    while True:
        try:
            x, y = flo.get_motion()
        except RuntimeError:
            continue
        tx += x
        ty += y
        print("Relative: x {:03d} y {:03d} | Absolute: x {:03d} y {:03d}".format(x, y, tx, ty))
        data.append([x, y, tx, ty])
        time.sleep(0.01)
except KeyboardInterrupt:
    # logフォルダーのパス
    log_folder_path = "./log"
    # logフォルダーが存在しない場合は作成する
    if not os.path.exists(log_folder_path):
        os.makedirs(log_folder_path)
    
    # CSVファイルを新規作成して書き込む
    filename = "data.csv"  # ファイル名を指定
    file_path = os.path.join(log_folder_path, filename)
    with open(file_path, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['X', 'Y', 'TX', 'TY'])  # ヘッダーを書き込む
        writer.writerows(data)  # データを書き込む

    print("CSVファイルの書き込みが完了しました。")
    pass
