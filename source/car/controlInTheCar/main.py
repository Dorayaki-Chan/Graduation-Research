#!/usr/bin/env python
import time
import argparse
from pmw3901 import PMW3901, BG_CS_FRONT_BCM, BG_CS_BACK_BCM

from openpyxl import Workbook
import os

# ワークブックを作成
workbook = Workbook()

# アクティブなシートを取得
sheet = workbook.active

# シート名を設定
sheet.title = "Sheet1"

# Pick the right class for the specified breakout
SensorClass = PMW3901 

flo = SensorClass(spi_port=0, spi_cs_gpio=BG_CS_FRONT_BCM)
flo.set_rotation(0)     # Rotation of sensor in degrees
                        # choices=[0, 90, 180, 270]

tx = 0
ty = 0

lists = []

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
        # 経過時間を表示
        elapsed_time = time.time() - start
        print("Elapsed_time:{0} [sec] | Relative: x {:03d} y {:03d} | Absolute: x {:03d} y {:03d}".format(get_elapsed_time(), x, y, tx, ty))
        lists.append([get_elapsed_time(), x, y, tx, ty])
        time.sleep(0.01)
except KeyboardInterrupt:
    # logフォルダーのパス
    log_folder_path = "./log"
    # logフォルダーが存在しない場合は作成する
    if not os.path.exists(log_folder_path):
        os.makedirs(log_folder_path)
    
    # セルに値を設定
    for i, list in enumerate(lists):
        sheet.cell(row=i+1, column=1).value = list[0]
        sheet.cell(row=i+1, column=2).value = list[1]
        sheet.cell(row=i+1, column=3).value = list[2]
        sheet.cell(row=i+1, column=4).value = list[3]

    # Excelファイルを保存
    file_path = os.path.join(log_folder_path, "coordinates.xlsx")
    workbook.save(file_path)
    pass
