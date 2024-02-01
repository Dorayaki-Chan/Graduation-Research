# CSVファイルフォルダーを指定して、CSVファイルを読み込み、読み込んだCSVファイルのデータを全て同じ散布図にプロットする
# 各CSVファイルのデータは、それぞれ異なる色でプロットする
# グラフの範囲はX軸は-1500~1500、Y軸は0~4000とする
# グラフのマス目は方眼紙のように縦横1:1とし、1000ごとに目盛りを表示する

import os
import csv
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# CSVファイルのあるフォルダーを指定する
# フォルダーはダイアログで選択する
import tkinter as tk
from tkinter import filedialog
root = tk.Tk()
root.withdraw()
iDir = os.path.abspath(os.path.dirname(__file__))
iDirPath = filedialog.askdirectory(initialdir = iDir)

# iDirPathフォルダー内のCSVファイルを読み込む
# CSVファイルのファイル名をリストに格納する
fileList = os.listdir(iDirPath)

datas = []
# CSVファイルのファイル名をリストから1つずつ取り出し、CSVファイルを読み込む
# CSVファイルのデータをリストに格納する
for filename in fileList:
    # CSVファイルのファイル名をリストから1つずつ取り出す
    # CSVファイルのファイル名を表示する
    print(filename)
    # CSVファイルを読み込む
    # CSVファイルのパスを作成する
    csvPath = os.path.join(iDirPath, filename)
    # CSVファイルを読み込む
    # CSVファイルのデータをリストに格納する
    with open(csvPath, encoding="utf8", newline="") as f:
        reader = csv.reader(f)

        x = []
        y = []
        for row in reader:
            x.append(row[2])
            y.append(row[3])
        datas.append({"name":filename, "data":[row for row in reader], "x":x, "y":y})

# グラフの描画
plt.scatter(datas[0]["x"], datas[0]["y"], label=datas[0]["name"])

plt.savefig("test.png")
